#include <vector>
#ifdef FMTWITHOSI
	#include "FMTlpmodel.hpp"
	#include "FMTmodelparser.hpp"
	#include "FMTversion.hpp"
	#include "FMTlogger.hpp"
	#include "FMTconstraint.hpp"
	#include "FMTscheduleparser.hpp"
	#include "FMTfreeexceptionhandler.hpp"
#endif


int main()
	{
#ifdef FMTWITHOSI
	Logging::FMTlogger().logstamp();
	if (Version::FMTversion().hasfeature("OSI"))
		{
		const std::string folder = "../../../../Examples/Models/TWD_land/";
		const std::string primarylocation = folder+"TWD_land.pri";
		Parser::FMTmodelparser modelparser;
		std::vector<std::string>scenarios;
		scenarios.push_back("levelopt1");
		scenarios.push_back("levelopt2");
		scenarios.push_back("levelopt3");
		scenarios.push_back("levelopt4");
		std::vector<double>objectivevalues;
		objectivevalues.push_back(15485.952);
		objectivevalues.push_back(1000);
		objectivevalues.push_back(2580.992);
		objectivevalues.push_back(50);
		const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
		for (size_t scnid = 0 ; scnid < models.size();++scnid)
		{
			Models::FMTlpmodel optimizationmodel(models.at(scnid), Models::FMTsolverinterface::CLP);
			for (size_t period = 0; period < 1; ++period)
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
			if (optimizationmodel.initialsolve())
			{
				const double value = optimizationmodel.getObjValue();
				if (1 < std::abs(value - objectivevalues.at(scnid)))
				{
					Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
						"FMTleveltest", __LINE__, primarylocation);
				}
			}
		}
		
	}else {
		Logging::FMTlogger() << "FMT needs to be compiled with OSI" << "\n";
		}
#endif 
	return 0;
	}
