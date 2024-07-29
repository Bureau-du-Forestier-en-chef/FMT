#include <vector>
#include <cmath>
#ifdef FMTWITHOSI
	#include "FMTversion.h"
	#include "FMTdefaultlogger.h"
	#include "FMTmodelparser.h"
    #include "FMTlpmodel.h"
    #include "FMTconstraint.h"
    #include "FMTfreeexceptionhandler.h"
    #include "FMTexception.h"
#endif



int roundobjectivevalue(const double& value)
{
    return static_cast<int>((value < 0) ? std::floor(value) : std::ceil(value));
}

int main(int argc, char *argv[])
	{
	#ifdef FMTWITHOSI
	Logging::FMTdefaultlogger().logstamp();
    const std::string primarylocation = argv[1];
    const std::string scenario = argv[2];
	const double objvalue = std::stod(argv[3]);
	/*const std::string primarylocation = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/09_FMT/Modeles_test/FM/PC_7001892_U03772_SSP02.pri";
	const std::string scenario = "03_sc1a_bfecopt";
	const double objvalue = 100;*/
	std::vector<Exception::FMTexc>errors;
	errors.push_back(Exception::FMTexc::FMTmissingyield);
	errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
	errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
	errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
	errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
	errors.push_back(Exception::FMTexc::FMToveridedyield);
	Parser::FMTmodelparser modelparser;
	modelparser.setdefaultexceptionhandler();
	modelparser.seterrorstowarnings(errors);
    const std::vector<std::string>scenarios(1, scenario);
    const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
   
		Models::FMTsolverinterface solverinterface = Models::FMTsolverinterface::CLP;
        if(Version::FMTversion::hasfeature("MOSEK"))
        {
			solverinterface = Models::FMTsolverinterface::MOSEK;
        }
		Models::FMTlpmodel optimizationmodel(models.at(0), solverinterface);
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
					Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, message,
						"TestSimpleplanning20p", 48, primarylocation);
                }
			}else{
			std::string message = "The model is unsolvable.";
				Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, message,
					"TestSimpleplanning20p", 46, primarylocation);
            }
	#endif
	return 0;
	}

