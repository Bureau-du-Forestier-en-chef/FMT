/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTparallelwriter.hpp"
#include "FMTmodel.hpp"

#ifdef FMTWITHGDAL
	#include "gdal.h"
	#include "gdal_priv.h"
#endif
namespace Parallel
{

	FMTparallelwriter::FMTparallelwriter(const std::string& location,
		const std::string& driver,
		const std::vector<Core::FMToutput>& outputs,
		const std::vector<Models::FMTmodel*>& allmodels,
		std::vector<std::string>layersoptions,
		double minimaldrift,
		Core::FMToutputlevel outputlevel) :
		outputstowrite(outputs),
		#ifdef FMTWITHGDAL
			resultsdataset(createOGRdataset(location, driver)),
			resultslayer(),
			driftlayer(),
		#endif
		mtx(),
		resultsminimaldrift(minimaldrift),
		outputslevel(outputlevel)

	{
		try {
			if (outputs.empty())
			{
				_exhandler->raise(Exception::FMTexc::FMTrangeerror,
					"No outputs to write",
					"FMTparallelwriter::FMTparallelwriter()", __LINE__, __FILE__);
			}
			for (const Models::FMTmodel* modelptr : allmodels)
			{
				resultslayer[modelptr->getname()] = createresultslayer(*modelptr, resultsdataset, layersoptions);
			}
			driftlayer = createdriftlayer(resultsdataset, layersoptions);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTparallelwriter::FMTparallelwriter", __LINE__, __FILE__);
			}
	}

	std::map<std::string, std::vector<std::vector<double>>> FMTparallelwriter::getresults(const std::unique_ptr<Models::FMTmodel>& modelptr, const int& firstperiod,
		const int& lastperiod) const
	{
		std::map<std::string, std::vector<std::vector<double>>>results;
		try {
			if (modelptr)
			{
				results = modelptr->getoutputsfromperiods(outputstowrite, firstperiod, lastperiod,outputslevel);
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
					std::vector<bool>passedlastiteration((*localvalues.at(globaloutput.first).begin()).size(), true);
					drifts[globaloutput.first][drift] = std::vector<double>();
					int periodid = 0;
					for (const std::vector<double>& iterationvalues : localvalues.at(globaloutput.first))
					{
						periodof = periodid;
						const double total = static_cast<double>(iterationvalues.size());
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
		try {
		#ifdef FMTWITHGDAL
			if (resultslayer.find(globalmodel)!= resultslayer.end()&&
				resultslayer.find(localmodel) != resultslayer.end()&&
				resultslayer.at(globalmodel) &&
				resultslayer.at(localmodel))
			{
				const std::map<std::string, std::vector<std::vector<double>>> globalvalues = getiterationsvalues(resultslayer.at(globalmodel));
				const std::map<std::string, std::vector<std::vector<double>>> localvalues = getiterationsvalues(resultslayer.at(localmodel));
				const std::map<std::string, std::map<double, std::vector<double>>>lowerdrifts = getdriftprobability(globalvalues, localvalues);
				const std::map<std::string, std::map<double, std::vector<double>>>upperdrifts = getdriftprobability(globalvalues, localvalues,false);
				writedrift(driftlayer, lowerdrifts, upperdrifts);
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

}