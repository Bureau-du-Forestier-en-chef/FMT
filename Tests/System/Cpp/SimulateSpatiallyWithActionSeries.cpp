/*
Copyright (c) 2019 Gouvernement du Quebec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include <vector>
#include <string>
#ifdef FMTWITHGDAL
#include "FMTSesModel.h"
#include "FMTModelParser.h"
#include "FMTAreaParser.h"
#include "FMTDefaultLogger.h"
#include "FMTForest.h"
#include "FMTSpatialSchedule.h"
#include "FMTLineGraph.h"
#include "FMTFreeExceptionHandler.h"
#include <boost/algorithm/string.hpp>
#endif

// A spatial simulation (SES) of a scenario with an *ACTIONSERIES first -> next, and a competing action that has the
// same operability as next. The series must hold in every cell as it holds in the LP graph: a cell operated by first
// never gets the competing action before next (BFECGCBM, model 8451: ACTPL -> APL, then ACTPL;APLCC in the SES).
// Arguments: <primary>|<scenario>|<first action>|<next action>|<competing action> <periods>

int main(int argc, char* argv[])
{
#ifdef FMTWITHGDAL
	Logging::FMTDefaultLogger().logStamp();
	std::string primarylocation = "../../../../Examples/Models/TWD_land/TWD_land.pri";
	std::string scenario = "Spatial_actionseries";
	std::string firstaction = "COUPETOTALE";
	std::string nextaction = "PLANTATION";
	std::string competingaction = "PLANTATIONCC";
	int periods = 10;
	if (argc > 1)
		{
		std::vector<std::string>results;
		const std::string vals = argv[1];
		boost::split(results, vals, boost::is_any_of("|"));
		primarylocation = results.at(0);
		scenario = results.at(1);
		firstaction = results.at(2);
		nextaction = results.at(3);
		competingaction = results.at(4);
		periods = std::stoi(argv[2]);
		}
	const std::string modellocation = primarylocation.substr(0, primarylocation.find_last_of("/\\") + 1);
	Parser::FMTModelParser mparser;
	std::vector<Exception::FMTexc>errors;
	errors.push_back(Exception::FMTexc::FMTmissingyield);
	errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
	errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
	errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
	errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
	errors.push_back(Exception::FMTexc::FMToveridedyield);
	errors.push_back(Exception::FMTexc::FMTsourcetotarget_transition);
	errors.push_back(Exception::FMTexc::FMTsame_transitiontargets);
	errors.push_back(Exception::FMTexc::FMTunclosedforloop);
	mparser.setErrorsToWarnings(errors);
	const std::vector<std::string>scenarios(1, scenario);
	std::vector<Models::FMTModel> models = mparser.readproject(primarylocation, scenarios);
	models.at(0).setParameter(Models::FMTintmodelparameters::LENGTH, periods);
	Models::FMTSesModel simulationmodel(models.at(0));
	const std::vector<std::vector<Core::FMTSchedule>> schedules = mparser.readSchedules(primarylocation, models);
	std::vector<Core::FMTTransition> strans;
	for (const auto& tran : simulationmodel.getTransitions())
		{
		strans.push_back(tran.single());
		}
	simulationmodel.setTransitions(strans);
	Parser::FMTAreaParser areaparser;
	const std::string rastpath = modellocation + "rasters/";
	std::vector<std::string> themesrast;
	for (int i = 1; i <= simulationmodel.getThemes().size(); i++)
		{
		themesrast.push_back(rastpath + "THEME" + std::to_string(i) + ".tif");
		}
	const Spatial::FMTForest initialforestmap = areaparser.readRasters(simulationmodel.getThemes(), themesrast, rastpath + "AGE.tif", 1, 0.0001);
	simulationmodel.setInitialMapping(initialforestmap);
	for (int period = 0; period < periods; ++period)
		{
		simulationmodel.greedyReferenceBuild(schedules.at(0).at(period), 10);
		}
	const std::vector<Core::FMTAction> actions = simulationmodel.getActions();
	int firstid = -1;
	int nextid = -1;
	int competingid = -1;
	for (int id = 0; id < static_cast<int>(actions.size()); ++id)
		{
		const std::string NAME = boost::to_upper_copy(actions.at(id).getName());
		if (NAME == firstaction) firstid = id;
		if (NAME == nextaction) nextid = id;
		if (NAME == competingaction) competingid = id;
		}
	if (firstid < 0 || nextid < 0 || competingid < 0)
		{
		Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed,
			"Missing action " + firstaction + ", " + nextaction + " or " + competingaction,
			"SimulateSpatiallyWithActionSeries", __LINE__, primarylocation);
		}
	// The actions of each cell in their order, then the action that follows each first action.
	size_t followedbynext = 0;
	size_t followedbycompeting = 0;
	std::string firstbrokencell;
	const Spatial::FMTSpatialSchedule spatialsolution = simulationmodel.getSpSchedule();
	for (Spatial::FMTSpatialSchedule::const_iterator cellit = spatialsolution.begin(); cellit != spatialsolution.end(); ++cellit)
		{
		const Graph::FMTLineGraph& linegraph = cellit->second.getLineGraph();
		std::vector<int> history;
		for (int period = 1; period < static_cast<int>(linegraph.size()); ++period)
			{
			for (const int& actionid : linegraph.getPeriodActionIds(period))
				{
				history.push_back(actionid);
				}
			}
		for (size_t index = 0; index + 1 < history.size(); ++index)
			{
			if (history.at(index) == firstid)
				{
				if (history.at(index + 1) == nextid)
					{
					++followedbynext;
				}else if (history.at(index + 1) == competingid)
					{
					++followedbycompeting;
					if (firstbrokencell.empty())
						{
						firstbrokencell = std::string(cellit->first);
						}
					}
				}
			}
		}
	Logging::FMTDefaultLogger() << firstaction << " -> " << nextaction << ": " << followedbynext << " cells, "
		<< firstaction << " -> " << competingaction << ": " << followedbycompeting << " cells\n";
	if (followedbycompeting > 0)
		{
		Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed,
			"The series " + firstaction + " -> " + nextaction + " is broken by " + competingaction + " in " +
			std::to_string(followedbycompeting) + " cells, the first at " + firstbrokencell,
			"SimulateSpatiallyWithActionSeries", __LINE__, primarylocation);
		}
	if (followedbynext == 0)
		{
		Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed,
			"No cell followed the series " + firstaction + " -> " + nextaction,
			"SimulateSpatiallyWithActionSeries", __LINE__, primarylocation);
		}
#endif
	return 0;
}
