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

int main(int argc, char *argv[])
	{
	#ifdef FMTWITHOSI
	Logging::FMTlogger().logstamp();
	if (Version::FMTversion().hasfeature("OSI"))
		{
		const std::string modellocation = "../../../../Examples/Models/TWD_land/";
        const std::string	primarylocation = modellocation + "TWD_land.pri";
		const std::string testfolderout = "D:/forcesolution/";
		Parser::FMTmodelparser modelparser;
		Parser::FMTareaparser areaparser;
		Parser::FMTscheduleparser scheparser;
		const std::vector<std::string>scenarios(1,"LP");
		const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTlpmodel optimizationmodel(models.at(0), Models::FMTsolverinterface::MOSEK);
		const std::vector<Core::FMTschedule>schedules = modelparser.readschedules(primarylocation,models).at(0);
		const double tolerance = 0.001;
        std::vector<Core::FMTschedule> lockedproportionscheduled;
		for (size_t period = 1; period <= 6; ++period)
			{
			optimizationmodel.buildperiod();
			}
		for (size_t period = 1; period <= 6; ++period)
			{
			optimizationmodel.setsolution(period,schedules.at(period-1), tolerance);
            lockedproportionscheduled.push_back(optimizationmodel.getscheduleproportions(period,true));
			}
		scheparser.write(lockedproportionscheduled,testfolderout+"lockedandpropos._seq");
        Models::FMTlpmodel modifoptimizationmodel(models.at(0), Models::FMTsolverinterface::MOSEK);
        const std::vector<Core::FMTactualdevelopment> oldarea = optimizationmodel.getarea();
        std::vector<Core::FMTactualdevelopment> newarea(oldarea.begin(),oldarea.begin()+oldarea.size()/2);
		areaparser.write(newarea,testfolderout+"modif._area");
        modifoptimizationmodel.setarea(newarea);
        for (size_t period = 1; period <= 6; ++period)
		{
			modifoptimizationmodel.buildperiod();
		}
		std::cout<<"Forcing solution"<<std::endl;
		for (size_t period = 1; period <= 6; ++period)
		{
            modifoptimizationmodel.forcesolution(period,lockedproportionscheduled.at(period-1), tolerance);
        }
		std::vector<Core::FMTschedule> modifschedules;
		for (size_t period = 1; period <= 6; ++period)
		{
			modifschedules.push_back(modifoptimizationmodel.getsolution(period,true));
			std::cout<<std::string(modifoptimizationmodel.getsolution(period,true))<<std::endl;
		}
		scheparser.write(modifschedules,testfolderout+"modif._seq");
	}else {
		Logging::FMTlogger() << "FMT needs to be compiled with OSI" << "\n";
		}
	#endif
	return 0;
	}

