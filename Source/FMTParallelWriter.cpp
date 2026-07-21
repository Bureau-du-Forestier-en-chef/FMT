/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTParallelWriter.h"
#include "FMTModel.h"
#include "FMTScheduleParser.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include "FMTExceptionHandler.h"

#ifdef FMTWITHGDAL
	#include "gdal.h"
	#include "gdal_priv.h"
#endif
namespace Parallel
{

	void FMTParallelWriter::close() noexcept
	{
	#ifdef FMTWITHGDAL
		if (!m_resultslayer.empty())
			{
			m_resultslayer.clear();
			GDALClose(m_resultsdataset);
			}
			
	#endif
	}


	FMTParallelWriter::~FMTParallelWriter()
		{
		close();
		}

	FMTParallelWriter::FMTParallelWriter(const std::string& location,
		const std::string& driver,
		const std::vector<Core::FMTOutput>& outputs,
		const std::vector<Models::FMTModel*>& allmodels,
		std::vector<std::string>layersoptions,
		double minimaldrift,
		Core::FMToutputlevel outputlevel) :
		m_outputstowrite(outputs),
		#ifdef FMTWITHGDAL
			m_resultsdataset(),
			m_resultslayer(),
			m_driftlayer(),
		#endif
		m_mtx(),
		m_resultsminimaldrift(minimaldrift),
		m_outputslevel(outputlevel),
		m_alllayeroptions(layersoptions),
		m_outputfirstperiod(),
		m_outputlastperiod(),
		m_projectdirectory(),
		m_projectname(),
		m_outputLocationPath(location)
		
	{
		try {
		#ifdef FMTWITHGDAL
			m_resultsdataset = createOGRDataset(location, driver);
		#endif
			if (outputs.empty())
			{
				_exhandler->raise(Exception::FMTexc::FMTrangeerror,
					"No outputs to write",
					"FMTParallelWriter::FMTParallelWriter()", __LINE__, __FILE__);
			}
			for (const Models::FMTModel* modelptr : allmodels)
				{
				setLayer(modelptr->getName());
				}
			#ifdef FMTWITHGDAL
			m_driftlayer = createDriftLayer(m_resultsdataset);
			#endif
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTParallelWriter::FMTParallelWriter", __LINE__, __FILE__);
			}
	}

	FMTParallelWriter::FMTParallelWriter(const std::string& location,
		const std::string& driver,
		Core::FMToutputlevel outputlevel,
		std::vector<std::string>layersoptions,
		int firstPeriod,
		int lastPeriod,
		std::string primaryfilelocation):
		m_outputstowrite(),
		#ifdef FMTWITHGDAL
		m_resultsdataset(createOGRDataset(location, driver)),
		m_resultslayer(),
		m_driftlayer(),
		#endif
		m_mtx(),
		m_resultsminimaldrift(),
		m_outputslevel(outputlevel),
		m_alllayeroptions(layersoptions),
		m_outputfirstperiod(firstPeriod),
		m_outputlastperiod(lastPeriod),
		m_projectdirectory(),
		m_projectname(),
		m_outputLocationPath(location)

	{
		if (!primaryfilelocation.empty())
		{
			if (!boost::filesystem::is_regular_file(boost::filesystem::path(primaryfilelocation)))
				{
				_exhandler->raise(Exception::FMTexc::FMTinvalid_path,
					primaryfilelocation + " is not a valid primary file", "FMTParallelWriter::FMTParallelWriter(...)", __LINE__, __FILE__);
				}
			const boost::filesystem::path boutdirectory = (boost::filesystem::path(primaryfilelocation).parent_path()).string();
			if (!boost::filesystem::is_directory(boutdirectory))
				{
				_exhandler->raise(Exception::FMTexc::FMTinvalid_path,
					boutdirectory.string() + " is not a valid scenarios directory", "FMTParallelWriter::FMTParallelWriter(...)", __LINE__, __FILE__);
				}
			m_projectdirectory = boutdirectory.string();
			m_projectname = boost::filesystem::path(primaryfilelocation).stem().string();
		}
		
	}

	void FMTParallelWriter::setLayer(const std::string& p_name)
	{
		try {
			#ifdef FMTWITHGDAL
			m_resultslayer[p_name] = createResultsLayer(p_name, m_resultsdataset,m_alllayeroptions);
			#endif
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTParallelWriter::setLayer", __LINE__, __FILE__);
		}
	}

	std::map<std::string, std::vector<std::vector<double>>> FMTParallelWriter::getResults(
		const std::unique_ptr<Models::FMTModel>& modelptr,
		const int& firstPeriod,
		const int& lastPeriod) const
	{
		std::map<std::string, std::vector<std::vector<double>>> results;
		try {
			if (modelptr)
			{
			results = modelptr->getOutputsFromPeriods(m_outputstowrite, firstPeriod, lastPeriod, m_outputslevel);
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", " FMTparallelwriter::getresults", __LINE__, __FILE__);
		}
		return results;
	}

	const std::map<std::string, std::map<double, std::vector<double>>>FMTParallelWriter::_getDriftProbability(
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
						"FMTParallelWriter::_getDriftProbability", __LINE__, __FILE__);
					continue;
				}

				for (double drift = m_resultsminimaldrift; drift >= 0; drift -= 0.05)
				{
					driftprob = drift;
					if (localvalues.find(globaloutput.first)== localvalues.end())
					{
						_exhandler->raise(Exception::FMTexc::FMTrangeerror,
							"No output "+ globaloutput.first +" in local",
							"FMTParallelWriter::_getDriftProbability", __LINE__, __FILE__);
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
								"FMTParallelWriter::_getDriftProbability", __LINE__, __FILE__);
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
				"FMTParallelWriter::_getDriftProbability", __LINE__, __FILE__);

		}
		return drifts;
	}



	void FMTParallelWriter::setDriftProbability(const std::string& globalmodel, const std::string& localmodel) const
	{
		boost::lock_guard<boost::recursive_mutex> lock(m_mtx);
		try {
		#ifdef FMTWITHGDAL
			if (m_resultslayer.find(globalmodel)!= m_resultslayer.end()&&
				m_resultslayer.find(localmodel) != m_resultslayer.end()&&
				m_resultslayer.at(globalmodel) &&
				m_resultslayer.at(localmodel))
			{
				const std::map<std::string, std::vector<std::vector<double>>> globalvalues = getIterationsValues(m_resultslayer.at(globalmodel));
				const std::map<std::string, std::vector<std::vector<double>>> localvalues = getIterationsValues(m_resultslayer.at(localmodel));
				if (!globalvalues.empty() && !localvalues.empty())
				{
					const std::map<std::string, std::map<double, std::vector<double>>>lowerdrifts = _getDriftProbability(globalvalues, localvalues);
					const std::map<std::string, std::map<double, std::vector<double>>>upperdrifts = _getDriftProbability(globalvalues, localvalues, false);
					writeDrift(m_driftlayer, lowerdrifts, upperdrifts);
				}
				else {
					_exhandler->raise(Exception::FMTexc::FMTignore,
						"Empty result layers, no drift probability generated",
						"FMTParallelWriter::setDriftProbability", __LINE__, __FILE__);
				}
			}
			else {
				_exhandler->raise(Exception::FMTexc::FMTrangeerror,
					"No layers to get",
					"FMTParallelWriter::setDriftProbability", __LINE__, __FILE__);
			}
		#endif
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTParallelWriter::setDriftProbability", __LINE__, __FILE__);
		}
	}




	void FMTParallelWriter::write(const std::string& modelname,
		const std::map<std::string, std::vector<std::vector<double>>>& results,
		const int& firstPeriod, const int& lastPeriod, const int& iteration) const
	{
		boost::lock_guard<boost::recursive_mutex> lock(m_mtx);
		try {
		#ifdef FMTWITHGDAL
			if (!results.empty())
				{
				writeFeatures(m_resultslayer.at(modelname), firstPeriod, iteration, m_outputstowrite, results);
			}else {
				fillUpInfeasibles(m_resultslayer.at(modelname), m_outputstowrite, iteration, firstPeriod, lastPeriod);
				}
		#endif
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTParallelWriter::write", __LINE__, __FILE__);
		}

	}

	void FMTParallelWriter::getAndWrite(
		const std::unique_ptr<Models::FMTModel>& modelptr, 
		const std::vector<Core::FMTOutput>& loutputs)
	{
		try {
			const int firstPeriod = m_outputfirstperiod;
			const int lastPeriod = std::min(m_outputlastperiod, modelptr->getParameter(Models::FMTintmodelparameters::LENGTH));
			boost::lock_guard<boost::recursive_mutex> lock(m_mtx);
			m_outputstowrite = loutputs;
			if (!m_outputstowrite.empty())
			{
				int replicateId = 0;
				std::string writeName = modelptr->getName();
				if (modelptr->getParameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD)&&
					(!writeName.empty() && writeName.find_first_not_of("0123456789") == std::string::npos))
					{
					replicateId = std::stoi(modelptr->getName());
					writeName = m_projectname;
					}
				write(writeName,
					getResults(modelptr, firstPeriod, lastPeriod),
					firstPeriod,
					lastPeriod,
					replicateId);
			}
			if (!(m_projectdirectory.empty()) && !(modelptr->getParameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD)))
				{
				const std::string scenarioname = modelptr->getName();
				std::string schedulelocation = m_projectdirectory+"/Scenarios/"+scenarioname+"/"+m_projectname+"._seq";
				if (scenarioname=="ROOT")
					{
					schedulelocation = m_projectdirectory + "/" + m_projectname + ".seq";
					}
				Parser::FMTScheduleParser parser;
				std::vector<Core::FMTSchedule>solution;
				for (int period = 1 ; period <= modelptr->getParameter(Models::FMTintmodelparameters::LENGTH);++period)
					{
					solution.push_back(modelptr->getSolution(period,true));
					}
				parser.write(solution, schedulelocation);
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("","FMTParallelWriter::getAndWrite", __LINE__, __FILE__);
			}
	}
	void FMTParallelWriter::writeSchedules(const std::string schedulePath, const std::vector<Core::FMTSchedule> scheduleList, bool append) const
	{
		try
		{
			Parser::FMTScheduleParser parser;
			boost::lock_guard<boost::recursive_mutex> lock(m_mtx);
			parser.write(scheduleList, schedulePath, append);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTParallelWriter::writeSchedules", __LINE__, __FILE__);
		}

	}

}