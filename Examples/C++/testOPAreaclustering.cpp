#ifdef FMTWITHOSI
	#include <vector>
	#include "FMTlpmodel.hpp"
	#include "FMTmodelparser.hpp"
	#include "FMTareaparser.hpp"
	#include "FMTversion.hpp"
	#include "FMTlogger.hpp"
	#include "FMTscheduleparser.hpp"
	#include "FMTschedule.hpp"
	#include "FMToperatingarea.hpp"
	#include "FMToperatingareacluster.hpp"
	#include "FMToperatingareaclusterer.hpp"
	#include "FMTtheme.hpp"
	#include "FMTfreeexceptionhandler.hpp"
	#include <string>
#endif

int main(int argc, char *argv[])
	{
	#ifdef FMTWITHOSI
	const std::string folder = "../../../../Examples/Models/TWD_land/";
	const std::string primarylocation = folder + "TWD_land.pri";
	Parser::FMTmodelparser modelparser;
	const std::vector<std::string>scenarios(1, "LP");
	const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
	Models::FMTlpmodel optmodel(models.at(0), Models::FMTsolverinterface::CLP);
	std::vector<Core::FMTtheme>themes = optmodel.getthemes();
	std::vector<Heuristics::FMToperatingarea>opareas;
	const size_t themetarget(0);
	for (const std::string& attribute : themes.at(themetarget).getattributes("UC"))
	{
		std::vector<std::string> mask;
		for (const Core::FMTtheme& theme : themes)
		{
			mask.push_back("?");
		}
		mask[themetarget] = attribute;
		opareas.push_back(Heuristics::FMToperatingarea(Core::FMTmask(mask, themes), 0.01));
	}
	std::vector<std::string>themesfields;
	size_t thid = 1;
	for (const Core::FMTtheme& theme : themes)
	{
		themesfields.push_back(std::to_string(thid));
		++thid;
	}
	const std::string agefield("AGE");
	const std::string areafield("SUPERFICIE");
	std::vector<Heuristics::FMToperatingareacluster>opareasclusterswithbounds;
	Parser::FMTareaparser areaparser;
	const std::string maplocation = folder + "/Carte/TWD_land.shp";
	for (const Heuristics::FMToperatingareacluster& oparea : areaparser.getclusters(opareas, themes, maplocation, agefield, areafield, 20000))
		{
		Heuristics::FMToperatingareacluster newoparea(oparea,400,10000000000);
		opareasclusterswithbounds.push_back(newoparea);
		}
	for (size_t period = 0; period < 2; ++period)
	{
		optmodel.buildperiod();
	}
	std::vector<Core::FMTconstraint>allconstraints = optmodel.getconstraints();
	const Core::FMTconstraint objective = allconstraints.at(0);
	allconstraints.erase(allconstraints.begin());
	for (const Core::FMTconstraint& constraint : allconstraints)
	{
		optmodel.setconstraint(constraint);
	}
	optmodel.setobjective(objective);
	if (optmodel.initialsolve())
	{
		Core::FMToutput opareaareasoutput;
		Core::FMToutput opareastatisticsoutput;
		for (const Core::FMToutput& output : optmodel.getoutputs())
		{
			if ("VOLINVENT" == output.getname())
			{
				opareastatisticsoutput = output;
			}
			else if ("TOTALAREA" == output.getname())
			{
				opareaareasoutput = output;
			}
		}
		std::vector<Heuristics::FMToperatingareaclusterer>heuristics = optmodel.getoperatingareaclustererheuristics(opareasclusterswithbounds, opareastatisticsoutput, opareaareasoutput, 1, 1);
		heuristics[0].setnumberofsimulationpass(25);
		heuristics[0].setquietlogger();
		if (!heuristics[0].initialsolve())
		{
			Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed,"No solution",
				"testOPAreaclustering", __LINE__, primarylocation);
		}
		//heuristics[0].branchnboundsolve();
		for (const Heuristics::FMToperatingareacluster& cluster : heuristics.at(0).getsolution())
		{
			for (const Core::FMTmask& mask : cluster.getallmasks())
			{
				std::cout << std::string(mask) << " ";
			}
			std::cout << "\n";
		}
	}
	else {
		Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Infeasible model",
			"testOPAreaclustering", __LINE__, primarylocation);
	}
	#endif
	return 0;
	}

