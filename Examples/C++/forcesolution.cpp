#ifdef FMTWITHOSI
	#include <vector>
	#include "FMTlpmodel.hpp"
	#include "FMTmodelparser.hpp"
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
		Parser::FMTmodelparser modelparser;
		modelparser.setdefaultexceptionhandler();
		const std::vector<std::string>scenarios(1,"LP");
		const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTlpmodel optimizationmodel(models.at(0), Models::FMTsolverinterface::CLP);
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
        Models::FMTlpmodel modifoptimizationmodel(models.at(0), Models::FMTsolverinterface::CLP);
        const std::vector<Core::FMTactualdevelopment> oldarea = optimizationmodel.getarea();
        std::vector<Core::FMTactualdevelopment> newarea(oldarea.begin(),oldarea.begin()+oldarea.size()/2);
        modifoptimizationmodel.setarea(newarea);
        for (size_t period = 1; period <= 6; ++period)
			{
			modifoptimizationmodel.buildperiod();
			}
		for (size_t period = 1; period <= 6; ++period)
			{
                modifoptimizationmodel.forcesolution(period,lockedproportionscheduled.at(period-1), tolerance);
            }
	}else {
		Logging::FMTlogger() << "FMT needs to be compiled with OSI" << "\n";
		}
	#endif
	return 0;
	}

