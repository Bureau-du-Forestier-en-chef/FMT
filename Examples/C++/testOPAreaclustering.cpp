#ifdef FMTWITHOSI
	#include <vector>
	#include "FMTlpmodel.h"
	#include "FMTmodelparser.h"
	#include "FMTareaparser.h"
	#include "FMTversion.h"
	#include "FMTlogger.h"
	#include "FMTscheduleparser.h"
	#include "FMTschedule.h"
	#include "FMToperatingarea.h"
	#include "FMToperatingareacluster.h"
	#include "FMToperatingareaclusterer.h"
	#include "FMTtheme.h"
	#include "FMTfreeexceptionhandler.h"
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
	Models::FMTLpModel optmodel(models.at(0), Models::FMTsolverinterface::CLP);
	std::vector<Core::FMTTheme>themes = optmodel.getThemes();
	std::vector<Heuristics::FMToperatingarea>opareas;
	const size_t themetarget(0);
	for (const std::string& attribute : themes.at(themetarget).getAttributes("UC"))
	{
		std::vector<std::string> mask;
		for (const Core::FMTTheme& theme : themes)
		{
			mask.push_back("?");
		}
		mask[themetarget] = attribute;
		opareas.push_back(Heuristics::FMToperatingarea(Core::FMTMask(mask, themes), 0.01));
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
	std::vector<Heuristics::FMToperatingareacluster>opareasclusterswithbounds;
	Parser::FMTAreaParser areaparser;
	const std::string maplocation = folder + "/Carte/TWD_land.shp";
	for (const Heuristics::FMToperatingareacluster& oparea : areaparser.getClusters(opareas, themes, maplocation, agefield, areafield, 20000))
		{
		Heuristics::FMToperatingareacluster newoparea(oparea,400,10000000000);
		opareasclusterswithbounds.push_back(newoparea);
		}
	for (size_t period = 0; period < 2; ++period)
	{
		optmodel.buildPeriod();
	}
	std::vector<Core::FMTConstraint>allconstraints = optmodel.getconstraints();
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
		std::vector<Heuristics::FMToperatingareaclusterer>heuristics = optmodel.getOperatingAreaClustererHeuristics(opareasclusterswithbounds, opareastatisticsoutput, opareaareasoutput, 1, 1);
		heuristics[0].setNumberOfSimulationPass(25);
		heuristics[0].setQuietLogger();
		if (!heuristics[0].initialSolve())
		{
			Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed,"No solution",
				"testOPAreaclustering", __LINE__, primarylocation);
		}
		//heuristics[0].branchNBoundSolve();
		for (const Heuristics::FMToperatingareacluster& cluster : heuristics.at(0).getSolution())
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

