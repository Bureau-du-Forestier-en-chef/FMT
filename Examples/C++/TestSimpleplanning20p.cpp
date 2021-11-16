#include <vector>
#include "FMTversion.hpp"
#include "FMTlogger.hpp"
#include "FMTmodelparser.hpp"
#ifdef FMTWITHOSI
    #include "FMTlpmodel.hpp"
    #include "FMTconstraint.hpp"
    #include "FMTfreeexceptionhandler.hpp"
    #include "FMTexception.hpp"
#endif



int roundobjectivevalue(const double& value)
{
    return static_cast<int>((value < 0) ? floor(value) : ceil(value);
}

int main(int argc, char *argv[])
	{
	Logging::FMTlogger().logstamp();
    Version::FMTversion version();
    const std::string primarylocation = argv[1];
    const std::string scenario = argv[2];
    const double objvalue = argv[3]
    Parser::FMTmodelparser modelparser;
    const std::vector<std::string>scenarios(1, scenario);
    const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
    #ifdef FMTWITHOSI
        Models::FMTsolverinterface interface = Models::FMTsolverinterface::CLP
        if(version.hasfeature("MOSEK"))
        {
            interface = Models::FMTsolverinterface::MOSEK;
        }
		Models::FMTlpmodel optimizationmodel(models.at(0), Models::FMTsolverinterface::CLP);
        optimizationmodel.setstrictlypositivesoutputsmatrix();
		for (size_t period = 0; period < 20; ++period)
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
                const double modelobjvalue = optimizationmodel.getObjValue();
                if(roundobjectivevalue(objvalue) != roundobjectivevalue(modelobjvalue)) 
                {
                    std::string message = "Optimizing the model gave a different goal value than expected : \n\tResult : "+std::to_string(roundobjectivevalue(modelobjvalue))+"\n\tExpected : "+std::to_string(roundobjectivevalue(objvalue));
                    //Logging::FMTlogger()<< message <<"\n";
                    Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed,message,
                                                                "TestSimpleplanning20p", 48, primarylocation)
                }
			}else{
                    std::string message = "The model is unsolvable."
                    Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed,message,
                                                                "TestSimpleplanning20p", 46, primarylocation)
            }
	#endif
	return 0;
	}

