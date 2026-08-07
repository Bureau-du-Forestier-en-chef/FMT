#ifdef FMTWITHOSI
	#include <vector>
	#include "FMTLpModel.h"
	#include "FMTModelParser.h"
	#include "FMTAreaParser.h"
	#include "FMTVersion.h"
	#include "FMTLogger.h"
	#include "FMTScheduleParser.h"
	#include "FMTSchedule.h"
	#include "FMTOperatingArea.h"
	#include "FMTOperatingAreaCluster.h"
	#include "FMTOperatingAreaClusterer.h"
	#include "FMTTheme.h"
	#include "FMTFreeExceptionHandler.h"
	#include <string>
#endif

int main(int argc, char *argv[])
	{
	#ifdef FMTWITHOSI
	const std::string folder = "../../../../Examples/Models/TWD_land/";
	const std::string primarylocation = folder + "TWD_land.pri";
	Parser::FMTModelParser modelparser;
	const std::vector<std::string>scenarios(1, "LP");
	const std::vector<Models::FMTModel> models = modelparser.readproject(primarylocation, scenarios);
	Models::FMTLpModel optmodel(models.at(0), Models::FMTSolverInterface::CLP);
	std::vector<Core::FMTTheme>themes = optmodel.getThemes();
	std::vector<Heuristics::FMTOperatingArea>opareas;
	const size_t themetarget(0);
	for (const std::string& attribute : themes.at(themetarget).getAttributes("UC"))
	{
		std::vector<std::string> mask;
		for (const Core::FMTTheme& theme : themes)
		{
			mask.push_back("?");
		}
		mask[themetarget] = attribute;
		opareas.push_back(Heuristics::FMTOperatingArea(Core::FMTMask(mask, themes), 0.01));
	}
	std::vector<std::string>themesfields;
	size_t thid = 1;
	for (const Core::FMTTheme& theme : themes)
	{
		themesfields.push_back(std::to_string(thid));
		++thid;
	}
	const std::string agefield("AGE");
	const std::string areafield("SUPERFICIE");
	std::vector<Heuristics::FMTOperatingAreaCluster>opareasclusterswithbounds;
	Parser::FMTAreaParser areaparser;
	const std::string maplocation = folder + "/Carte/TWD_land.shp";
	for (const Heuristics::FMTOperatingAreaCluster& oparea : areaparser.getClusters(opareas, themes, maplocation, agefield, areafield, 20000))
		{
		Heuristics::FMTOperatingAreaCluster newoparea(oparea,400,10000000000);
		opareasclusterswithbounds.push_back(newoparea);
		}
	for (size_t period = 0; period < 2; ++period)
	{
		optmodel.buildPeriod();
	}
	std::vector<Core::FMTConstraint>allconstraints = optmodel.getConstraints();
	const Core::FMTConstraint objective = allconstraints.at(0);
	allconstraints.erase(allconstraints.begin());
	for (const Core::FMTConstraint& constraint : allconstraints)
	{
		optmodel.setConstraint(constraint);
	}
	optmodel.setObjective(objective);
	if (optmodel.initialSolve())
	{
		Core::FMTOutput opareaareasoutput;
		Core::FMTOutput opareastatisticsoutput;
		for (const Core::FMTOutput& output : optmodel.getOutputs())
		{
			if ("VOLINVENT" == output.getName())
			{
				opareastatisticsoutput = output;
			}
			else if ("TOTALAREA" == output.getName())
			{
				opareaareasoutput = output;
			}
		}
		std::vector<Heuristics::FMTOperatingAreaClusterer>heuristics = optmodel.getOperatingAreaClustererHeuristics(opareasclusterswithbounds, opareastatisticsoutput, opareaareasoutput, 1, 1);
		heuristics[0].setNumberOfSimulationPass(25);
		heuristics[0].setQuietLogger();
		if (!heuristics[0].initialSolve())
		{
			Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed,"No solution",
				"testOPAreaclustering", __LINE__, primarylocation);
		}
		//heuristics[0].branchNBoundSolve();
		for (const Heuristics::FMTOperatingAreaCluster& cluster : heuristics.at(0).getSolution())
		{
			for (const Core::FMTMask& mask : cluster.getAllMasks())
			{
				std::cout << std::string(mask) << " ";
			}
			std::cout << "\n";
		}
	}
	else {
		Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed, "Infeasible model",
			"testOPAreaclustering", __LINE__, primarylocation);
	}
	#endif
	return 0;
	}

