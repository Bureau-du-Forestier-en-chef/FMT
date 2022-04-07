#ifdef FMTWITHOSI
	#include <vector>
	#include "FMTlpmodel.hpp"
	#include "FMTmodelparser.hpp"
	#include "FMTareaparser.hpp"
	#include "FMTversion.hpp"
	#include "FMTlogger.hpp"
	#include "FMTscheduleparser.hpp"
	#include "FMTschedule.hpp"
	#include "FMToutputnode.hpp"
	#include "FMTfreeexceptionhandler.hpp"
#endif

/*bool validate(modifmodel,initialmodel,propschedules)
{

}*/

int main()
	{
	#ifdef FMTWITHOSI
	Logging::FMTlogger().logstamp();
	if (Version::FMTversion().hasfeature("OSI"))
		{
		const std::string modellocation = "../../../../Examples/Models/TWD_land/";
        const std::string	primarylocation = modellocation + "TWD_land.pri";
		const std::string testfolderout = "tests/";
		Parser::FMTmodelparser modelparser;
		Parser::FMTareaparser areaparser;
		Parser::FMTscheduleparser scheparser;
		const std::vector<std::string>scenarios(1,"Forcesolutiontest");
		const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTmodel initialmodel = models.at(0);
		const int lenght = 6;
		initialmodel.setparameter(Models::FMTintmodelparameters::LENGTH, lenght);
		initialmodel.setparameter(Models::FMTboolmodelparameters::POSTSOLVE, false);
		initialmodel.setparameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS, 0);
		initialmodel.setparameter(Models::FMTdblmodelparameters::TOLERANCE, 0.001);
		Models::FMTlpmodel optimizationmodel(initialmodel, Models::FMTsolverinterface::CLP);
		const std::vector<Core::FMTschedule>schedules = modelparser.readschedules(primarylocation,models).at(0);
		optimizationmodel.doplanning(false,schedules);
        std::vector<Core::FMTschedule> lockedproportionscheduled;
		for (size_t period = 1; period <= static_cast<size_t>(lenght); ++period)
		{
			lockedproportionscheduled.push_back(optimizationmodel.getscheduleproportions(period,true));
		}
		Models::FMTlpmodel modifoptimizationmodel(initialmodel, Models::FMTsolverinterface::CLP);
		// ici on vient changer la section area 
		const std::vector<Core::FMTactualdevelopment> newarea = areaparser.readvectors(	modifoptimizationmodel.getthemes(),
																						modellocation+"Carte/TWD_LAND_forcesolution_modif.shp",
                                   														"AGE","SUPERFICIE", 1.0, 1);
        modifoptimizationmodel.setarea(newarea);
        for (size_t period = 1; period <=  static_cast<size_t>(lenght); ++period)
		{
			const Core::FMTschedule periodpropschedule = lockedproportionscheduled.at(period-1);
			modifoptimizationmodel.buildperiod(periodpropschedule,true);
			modifoptimizationmodel.forcesolution(period,periodpropschedule);
		}
		/*if(!validate(modifoptimizationmodel,optimizationmodel,lockedproportionscheduled))
		{
			//failed
		}*/
	}else {
		Logging::FMTlogger() << "FMT needs to be compiled with OSI" << "\n";
		}
	#endif
	return 0;
	}

