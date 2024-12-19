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
			resultsdataset = createOGRdataset(location, driver);
		#endif
			if (outputs.empty())
			{
				_exhandler->raise(Exception::FMTexc::FMTrangeerror,
					"No outputs to write",
					"FMTparallelwriter::FMTparallelwriter()", __LINE__, __FILE__);
			}
			for (const Models::FMTmodel* modelptr : allmodels)
				{
				setLayer(modelptr->getname());
				}
			#ifdef FMTWITHGDAL
			driftlayer = createdriftlayer(resultsdataset);
			#endif
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTparallelwriter::FMTparallelwriter", __LINE__, __FILE__);
			}
	}

	FMTparallelwriter::FMTparallelwriter(const std::string& location,
		const std::string& driver,
		Core::FMToutputlevel outputlevel,
		std::vector<std::string>layersoptions,
		int firstperiod,
		int lastperiod,
		std::string primaryfilelocation):
		outputstowrite(),
		#ifdef FMTWITHGDAL
		resultsdataset(createOGRdataset(location, driver)),
		resultslayer(),
		driftlayer(),
		#endif
		mtx(),
		resultsminimaldrift(),
		outputslevel(outputlevel),
		alllayeroptions(layersoptions),
		outputfirstperiod(firstperiod),
		outputlastperiod(lastperiod),
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
			_exhandler->raisefromcatch("", "FMTparallelwriter::setlayer", __LINE__, __FILE__);
		}
	}




	std::map<std::string, std::vector<std::vector<double>>> FMTparallelwriter::getresults(const std::unique_ptr<Models::FMTmodel>& modelptr,const int& firstperiod,
		const int& lastperiod) const
	{
		std::map<std::string, std::vector<std::vector<double>>>results;
		try {
			if (modelptr)
			{
			results = modelptr->getoutputsfromperiods(outputstowrite, firstperiod, lastperiod, outputslevel);
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", " FMTparallelwriter::getresults", __LINE__, __FILE__);
		}
		return results;
	}
	const std::map<std::string, std::map<double, std::vector<double>>>FMTparallelwriter::getdriftprobability(
		const std::map<std::string, std::vector<std::vector<double>>>& globalvalues,
		const std::map<std::string, std::vector<std::vector<double>>>& localvalues,
		const bool lower) const
	{
		std::map<std::string, std::map<double, std::vector<double>>>drifts;
		std::string outputname;
		double driftprob;
		int periodof;
		try {
			for (const auto& globaloutput : globalvalues)
			{
				outputname = globaloutput.first;
				drifts[globaloutput.first] = std::map<double, std::vector<double>>();
				//const std::vector<double>& baseglobal = globaloutput.second.at(0);
				for (double drift = resultsminimaldrift; drift >= 0; drift -= 0.05)
				{
					driftprob = drift;
					if (localvalues.find(globaloutput.first)== localvalues.end())
					{
						_exhandler->raise(Exception::FMTexc::FMTrangeerror,
							"No output "+ globaloutput.first +" in local",
							"FMTparallelwriter::getdriftprobability", __LINE__, __FILE__);
					}
					std::vector<bool>passedlastiteration((*localvalues.at(globaloutput.first).begin()).size(), true);
					drifts[globaloutput.first][drift] = std::vector<double>();
					int periodid = 0;
					for (const std::vector<double>& iterationvalues : localvalues.at(globaloutput.first))
					{
						periodof = periodid;
						const double total = static_cast<double>(iterationvalues.size());
						if (static_cast<size_t>(periodid)>=globaloutput.second.size())
						{
							_exhandler->raise(Exception::FMTexc::FMTrangeerror,
								"No iteration " + std::to_string(periodid) + " in global",
								"FMTparallelwriter::getdriftprobability", __LINE__, __FILE__);
							return drifts;
						}
						const double globalvalue = globaloutput.second.at(periodid).at(0);
						double count = 0;
						if (!std::isnan(globalvalue))
						{
							size_t iterationid = 0;
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
			_exhandler->raisefromcatch("On output "+outputname+" "+std::to_string(driftprob)+" period id "+ std::to_string(periodof),
										"FMTparallelwriter::getdriftprobability", __LINE__, __FILE__);

		}
		return drifts;
	}



	void FMTparallelwriter::setdriftprobability(const std::string& globalmodel, const std::string& localmodel) const
	{
		boost::lock_guard<boost::recursive_mutex> lock(mtx);
		try {
		#ifdef FMTWITHGDAL
			if (resultslayer.find(globalmodel)!= resultslayer.end()&&
				resultslayer.find(localmodel) != resultslayer.end()&&
				resultslayer.at(globalmodel) &&
				resultslayer.at(localmodel))
			{
				const std::map<std::string, std::vector<std::vector<double>>> globalvalues = getiterationsvalues(resultslayer.at(globalmodel));
				const std::map<std::string, std::vector<std::vector<double>>> localvalues = getiterationsvalues(resultslayer.at(localmodel));
				if (!globalvalues.empty() && !localvalues.empty())
				{
					const std::map<std::string, std::map<double, std::vector<double>>>lowerdrifts = getdriftprobability(globalvalues, localvalues);
					const std::map<std::string, std::map<double, std::vector<double>>>upperdrifts = getdriftprobability(globalvalues, localvalues, false);
					writedrift(driftlayer, lowerdrifts, upperdrifts);
				}
				else {
					_exhandler->raise(Exception::FMTexc::FMTrangeerror,
						"Empty result layers",
						"FMTparallelwriter::setdriftprobability", __LINE__, __FILE__);
				}
			}
			else {
				_exhandler->raise(Exception::FMTexc::FMTrangeerror,
					"No layers to get",
					"FMTparallelwriter::setdriftprobability", __LINE__, __FILE__);
			}
		#endif
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTparallelwriter::setdriftprobability", __LINE__, __FILE__);
		}
	}




	void FMTparallelwriter::write(const std::string& modelname,
		const std::map<std::string, std::vector<std::vector<double>>>& results,
		const int& firstperiod, const int& lastperiod, const int& iteration) const
	{
		boost::lock_guard<boost::recursive_mutex> lock(mtx);
		try {
		#ifdef FMTWITHGDAL
			if (!results.empty())
				{
				writefeatures(resultslayer.at(modelname), firstperiod, iteration, outputstowrite, results);
			}else {
				fillupinfeasibles(resultslayer.at(modelname), outputstowrite, iteration, firstperiod, lastperiod);
				}
		#endif
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTparallelwriter::write", __LINE__, __FILE__);
		}

	}

	void FMTparallelwriter::getandwrite(const std::unique_ptr<Models::FMTmodel>& modelptr, const std::vector<Core::FMToutput>& loutputs)
	{
		try {
			const int firstperiod = outputfirstperiod;
			const int lastperiod = std::min(outputlastperiod, modelptr->getparameter(Models::FMTintmodelparameters::LENGTH));
			boost::lock_guard<boost::recursive_mutex> lock(mtx);
			outputstowrite = loutputs;
			if (!outputstowrite.empty())
			{
				int replicateId = 0;
				std::string writeName = modelptr->getname();
				if (modelptr->getparameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD)&&
					(!writeName.empty() && writeName.find_first_not_of("0123456789") == std::string::npos))
					{
					replicateId = std::stoi(modelptr->getname());
					writeName = projectname;
					}
				write(writeName,
					getresults(modelptr, firstperiod, lastperiod),
					firstperiod,
					lastperiod,
					replicateId);
			}
			if (!(projectdirectory.empty()) && !(modelptr->getparameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD)))
				{
				const std::string scenarioname = modelptr->getname();
				std::string schedulelocation = projectdirectory+"/Scenarios/"+scenarioname+"/"+projectname+"._seq";
				if (scenarioname=="ROOT")
					{
					schedulelocation = projectdirectory + "/" + projectname + ".seq";
					}
				Parser::FMTscheduleparser parser;
				std::vector<Core::FMTschedule>solution;
				for (int period = 1 ; period <= modelptr->getparameter(Models::FMTintmodelparameters::LENGTH);++period)
					{
					solution.push_back(modelptr->getsolution(period,true));
					}
				parser.write(solution, schedulelocation);
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("","FMTparallelwriter::getandwrite", __LINE__, __FILE__);
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
			_exhandler->raisefromcatch("", "FMTparallelwriter::writeSchedules", __LINE__, __FILE__);
		}

	}

}