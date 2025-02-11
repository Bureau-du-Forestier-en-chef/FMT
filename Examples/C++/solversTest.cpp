#include <vector>
#ifdef FMTWITHOSI
#include "FMTlpmodel.h"
#include "FMTmodelparser.h"
#include "FMTversion.h"
#include "FMTdefaultlogger.h"
#include "FMTfreeexceptionhandler.h"
#endif


int main()
{
#ifdef FMTWITHOSI
	Logging::FMTdefaultlogger().logstamp();
	if (Version::FMTversion().hasfeature("OSI"))
	{
		const std::string folder = "../../../../Examples/Models/TWD_land/";
		const std::string primarylocation = folder + "TWD_land.pri";
		const std::string outdir = "../../tests/Simpleplanning/";
		Parser::FMTmodelparser modelparser;
		modelparser.setdebuglogger();
		const std::vector<std::string>scenarios(1, "LP");
		const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
		std::vector<Models::FMTsolverinterface>solvers;
		solvers.push_back(Models::FMTsolverinterface::CLP);
		#ifdef FMTWITHMOSEK
			solvers.push_back(Models::FMTsolverinterface::MOSEK);
		#endif
		#ifdef FMTWITHGLPK
			solvers.push_back(Models::FMTsolverinterface::GLPK);
		#endif
			for (const Models::FMTsolverinterface& SOLVER : solvers)
			{
				Models::FMTlpmodel optimizationmodel(models.at(0), SOLVER);
				for (size_t period = 0; period < 10; ++period)
				{
					optimizationmodel.buildperiod();
				}
				std::vector<Core::FMTconstraint>constraints = optimizationmodel.getconstraints();
				const Core::FMTconstraint objective = constraints.at(0);
				constraints.erase(constraints.begin());
				for (const Core::FMTconstraint& constraint : constraints)
				{
					optimizationmodel.setconstraint(constraint);
				}
				optimizationmodel.setobjective(objective);
				if (!optimizationmodel.initialsolve())
				{
					Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Cant solve with solver type number " + std::to_string(static_cast<int>(SOLVER)),
						"solversTest", __LINE__, __FILE__);
				}
			}
		
	}else {
		Logging::FMTdefaultlogger() << "FMT needs to be compiled with OSI" << "\n";
		}
#endif 
	return 0;
}

