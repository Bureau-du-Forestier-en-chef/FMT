/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTparallelwriter.h"
#include "FMTmodel.h"
#include "FMTscheduleparser.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include "FMTexceptionhandler.h"

#ifdef FMTWITHGDAL
	#include "gdal.h"
	#include "gdal_priv.h"
#endif
namespace Parallel
{

	void FMTparallelwriter::close() noexcept
	{
	#ifdef FMTWITHGDAL
		if (!resultslayer.empty())
			{
			resultslayer.clear();
			GDALClose(resultsdataset);
			}
			
	#endif
	}


	FMTparallelwriter::~FMTparallelwriter()
		{
		close();
		}

	FMTparallelwriter::FMTparallelwriter(const std::string& location,
		const std::string& driver,
		const std::vector<Core::FMToutput>& outputs,
		const std::vector<Models::FMTmodel*>& allmodels,
		std::vector<std::string>layersoptions,
		double minimaldrift,
		Core::FMToutputlevel outputlevel) :
		outputstowrite(outputs),
		#ifdef FMTWITHGDAL
			resultsdataset(),
			resultslayer(),
			driftlayer(),
		#endif
		mtx(),
		resultsminimaldrift(minimaldrift),
		outputslevel(outputlevel),
		alllayeroptions(layersoptions),
		outputfirstperiod(),
		outputlastperiod(),
		projectdirectory(),
		projectname(),
		m_outputLocationPath(location)
		
	{
		try {
		#ifdef FMTWITHGDAL
			resultsdataset = createOGRDataset(location, driver);
		#endif
			if (outputs.empty())
			{
				_exhandler->raise(Exception::FMTexc::FMTrangeerror,
					"No outputs to write",
					"FMTparallelwriter::FMTparallelwriter()", __LINE__, __FILE__);
			}
			for (const Models::FMTmodel* modelptr : allmodels)
				{
				setLayer(modelptr->getName());
				}
			#ifdef FMTWITHGDAL
			driftlayer = createDriftLayer(resultsdataset);
			#endif
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTparallelwriter::FMTparallelwriter", __LINE__, __FILE__);
			}
	}

	FMTparallelwriter::FMTparallelwriter(const std::string& location,
		const std::string& driver,
		Core::FMToutputlevel outputlevel,
		std::vector<std::string>layersoptions,
		int firstPeriod,
		int lastPeriod,
		std::string primaryfilelocation):
		outputstowrite(),
		#ifdef FMTWITHGDAL
		resultsdataset(createOGRDataset(location, driver)),
		resultslayer(),
		driftlayer(),
		#endif
		mtx(),
		resultsminimaldrift(),
		outputslevel(outputlevel),
		alllayeroptions(layersoptions),
		outputfirstperiod(firstPeriod),
		outputlastperiod(lastPeriod),
		projectdirectory(),
		projectname(),
		m_outputLocationPath(location)

	{
		if (!primaryfilelocation.empty())
		{
			if (!boost::filesystem::is_regular_file(boost::filesystem::path(primaryfilelocation)))
				{
				_exhandler->raise(Exception::FMTexc::FMTinvalid_path,
					primaryfilelocation + " is not a valid primary file", "FMTparallelwriter::FMTparallelwriter(...)", __LINE__, __FILE__);
				}
			const boost::filesystem::path boutdirectory = (boost::filesystem::path(primaryfilelocation).parent_path()).string();
			if (!boost::filesystem::is_directory(boutdirectory))
				{
				_exhandler->raise(Exception::FMTexc::FMTinvalid_path,
					boutdirectory.string() + " is not a valid scenarios directory", "FMTparallelwriter::FMTparallelwriter(...)", __LINE__, __FILE__);
				}
			projectdirectory = boutdirectory.string();
			projectname = boost::filesystem::path(primaryfilelocation).stem().string();
		}
		
	}

	void FMTparallelwriter::setLayer(const std::string& p_name)
	{
		try {
			#ifdef FMTWITHGDAL
			resultslayer[p_name] = createResultsLayer(p_name, resultsdataset,alllayeroptions);
			#endif
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTparallelwriter::setLayer", __LINE__, __FILE__);
		}
	}

	std::map<std::string, std::vector<std::vector<double>>> FMTparallelwriter::getResults(
		const std::unique_ptr<Models::FMTmodel>& modelptr,
		const int& firstPeriod,
		const int& lastPeriod) const
	{
		std::map<std::string, std::vector<std::vector<double>>> results;
		try {
			if (modelptr)
			{
			results = modelptr->getOutputsFromPeriods(outputstowrite, firstPeriod, lastPeriod, outputslevel);
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", " FMTparallelwriter::getresults", __LINE__, __FILE__);
		}
		return results;
	}

	const std::map<std::string, std::map<double, std::vector<double>>>FMTparallelwriter::getDriftProbability(
		const std::map<std::string, std::vector<std::vector<double>>>& globalvalues,
		const std::map<std::string, std::vector<std::vector<double>>>& localvalues,
		const bool lower) const
	{
		std::map<std::string, std::map<double, std::vector<double>>>drifts;
		std::string outputname;
		double driftprob;
		int periodof;
		size_t iterationid = 0;
		try {
			for (const auto& globaloutput : globalvalues)
			{
				outputname = globaloutput.first;
				drifts[globaloutput.first] = std::map<double, std::vector<double>>();
				//const std::vector<double>& baseglobal = globaloutput.second.at(0)
				
				if (localvalues.find(globaloutput.first) == localvalues.end() ||
					globaloutput.second.size() != localvalues.at(globaloutput.first).size())
				{
					_exhandler->raise(Exception::FMTexc::FMTignore,
						"No drift calculated for missing values in " + globaloutput.first,
						"FMTparallelwriter::getDriftProbability", __LINE__, __FILE__);
					continue;
				}

				for (double drift = resultsminimaldrift; drift >= 0; drift -= 0.05)
				{
					driftprob = drift;
					if (localvalues.find(globaloutput.first)== localvalues.end())
					{
						_exhandler->raise(Exception::FMTexc::FMTrangeerror,
							"No output "+ globaloutput.first +" in local",
							"FMTparallelwriter::getDriftProbability", __LINE__, __FILE__);
					}
					// TODO regarder si les length sont les mêmes entre les période (pas juste la première) donc un max()
					std::size_t max_size = 0;
					for (auto it = localvalues.begin(); it != localvalues.end(); ++it) {
						const auto& value_list = it->second;

						if (!value_list.empty()) {
							std::size_t current_size = value_list.begin()->size();

							if (current_size > max_size)
								max_size = current_size;
						}
					}
					
					std::vector<bool> passedlastiteration(max_size, true);
					
					drifts[globaloutput.first][drift] = std::vector<double>();
					int periodid = 0;
					for (const std::vector<double>& iterationvalues : localvalues.at(globaloutput.first))
					{
						periodof = periodid;
						const double total = static_cast<double> (iterationvalues.size());
						if (static_cast<size_t> (periodid) >= globaloutput.second.size())
						{
							_exhandler->raise(Exception::FMTexc::FMTrangeerror,
								"No iteration " + std::to_string(periodid) + " in global",
								"FMTparallelwriter::getDriftProbability", __LINE__, __FILE__);
							return drifts;
						}
						const double globalvalue = globaloutput.second.at(periodid).at(0);
						double count = 0;
						if (!std::isnan(globalvalue))
						{
							iterationid = 0;
							for (const double& localvalue : iterationvalues)
							{
								if (passedlastiteration.at(iterationid))
								{
									if (std::isnan(localvalue))
									{
										count = 0;
										passedlastiteration[iterationid] = false;
										break;
									}
									if ((lower && (localvalue >= (globalvalue - (globalvalue *drift))))||
										(!lower && (localvalue <= (globalvalue + (globalvalue *drift)))))
									{
										++count;
									}
									else {
										passedlastiteration[iterationid] = false;
									}
								}
								++iterationid;
							}
						}
						drifts[globaloutput.first][drift].push_back((count / total));
						++periodid;
					}
				}
			}

		}
		catch (...)
		{
			_exhandler->raiseFromCatch("On output "+outputname+" "+std::to_string(driftprob)
				+" period id "+ std::to_string(periodof) + " on replicate " + std::to_string(iterationid + 1),
				"FMTparallelwriter::getDriftProbability", __LINE__, __FILE__);

		}
		return drifts;
	}



	void FMTparallelwriter::setDriftProbability(const std::string& globalmodel, const std::string& localmodel) const
	{
		boost::lock_guard<boost::recursive_mutex> lock(mtx);
		try {
		#ifdef FMTWITHGDAL
			if (resultslayer.find(globalmodel)!= resultslayer.end()&&
				resultslayer.find(localmodel) != resultslayer.end()&&
				resultslayer.at(globalmodel) &&
				resultslayer.at(localmodel))
			{
				const std::map<std::string, std::vector<std::vector<double>>> globalvalues = getIterationsValues(resultslayer.at(globalmodel));
				const std::map<std::string, std::vector<std::vector<double>>> localvalues = getIterationsValues(resultslayer.at(localmodel));
				if (!globalvalues.empty() && !localvalues.empty())
				{
					const std::map<std::string, std::map<double, std::vector<double>>>lowerdrifts = getDriftProbability(globalvalues, localvalues);
					const std::map<std::string, std::map<double, std::vector<double>>>upperdrifts = getDriftProbability(globalvalues, localvalues, false);
					writeDrift(driftlayer, lowerdrifts, upperdrifts);
				}
				else {
					_exhandler->raise(Exception::FMTexc::FMTignore,
						"Empty result layers, no drift probability generated",
						"FMTparallelwriter::setDriftProbability", __LINE__, __FILE__);
				}
			}
			else {
				_exhandler->raise(Exception::FMTexc::FMTrangeerror,
					"No layers to get",
					"FMTparallelwriter::setDriftProbability", __LINE__, __FILE__);
			}
		#endif
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTparallelwriter::setDriftProbability", __LINE__, __FILE__);
		}
	}




	void FMTparallelwriter::write(const std::string& modelname,
		const std::map<std::string, std::vector<std::vector<double>>>& results,
		const int& firstPeriod, const int& lastPeriod, const int& iteration) const
	{
		boost::lock_guard<boost::recursive_mutex> lock(mtx);
		try {
		#ifdef FMTWITHGDAL
			if (!results.empty())
				{
				writeFeatures(resultslayer.at(modelname), firstPeriod, iteration, outputstowrite, results);
			}else {
				fillUpInfeasibles(resultslayer.at(modelname), outputstowrite, iteration, firstPeriod, lastPeriod);
				}
		#endif
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTparallelwriter::write", __LINE__, __FILE__);
		}

	}

	void FMTparallelwriter::getAndWrite(
		const std::unique_ptr<Models::FMTmodel>& modelptr, 
		const std::vector<Core::FMToutput>& loutputs)
	{
		try {
			const int firstPeriod = outputfirstperiod;
			const int lastPeriod = std::min(outputlastperiod, modelptr->getParameter(Models::FMTintmodelparameters::LENGTH));
			boost::lock_guard<boost::recursive_mutex> lock(mtx);
			outputstowrite = loutputs;
			if (!outputstowrite.empty())
			{
				int replicateId = 0;
				std::string writeName = modelptr->getName();
				if (modelptr->getParameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD)&&
					(!writeName.empty() && writeName.find_first_not_of("0123456789") == std::string::npos))
					{
					replicateId = std::stoi(modelptr->getName());
					writeName = projectname;
					}
				write(writeName,
					getResults(modelptr, firstPeriod, lastPeriod),
					firstPeriod,
					lastPeriod,
					replicateId);
			}
			if (!(projectdirectory.empty()) && !(modelptr->getParameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD)))
				{
				const std::string scenarioname = modelptr->getName();
				std::string schedulelocation = projectdirectory+"/Scenarios/"+scenarioname+"/"+projectname+"._seq";
				if (scenarioname=="ROOT")
					{
					schedulelocation = projectdirectory + "/" + projectname + ".seq";
					}
				Parser::FMTscheduleparser parser;
				std::vector<Core::FMTschedule>solution;
				for (int period = 1 ; period <= modelptr->getParameter(Models::FMTintmodelparameters::LENGTH);++period)
					{
					solution.push_back(modelptr->getSolution(period,true));
					}
				parser.write(solution, schedulelocation);
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("","FMTparallelwriter::getAndWrite", __LINE__, __FILE__);
			}
	}
	void FMTparallelwriter::writeSchedules(const std::string schedulePath, const std::vector<Core::FMTschedule> scheduleList, bool append) const
	{
		try
		{
			Parser::FMTscheduleparser parser;
			boost::lock_guard<boost::recursive_mutex> lock(mtx);
			parser.write(scheduleList, schedulePath, append);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTparallelwriter::writeSchedules", __LINE__, __FILE__);
		}

	}

}