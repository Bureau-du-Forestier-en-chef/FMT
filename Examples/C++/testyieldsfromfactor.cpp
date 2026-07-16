#include <vector>
#ifdef FMTWITHOSI
	#include "FMTversion.h"
	#include "FMTdefaultlogger.h"
	#include "FMTmodelparser.h"
    #include "FMTlpmodel.h"
    #include "FMTconstraint.h"
    #include "FMTfreeexceptionhandler.h"
    #include "FMTexception.h"
#endif

int main()
	{
	#ifdef FMTWITHOSI
	Logging::FMTDefaultLogger().logStamp();
	const std::string folder = "../../../../Examples/Models/TWD_land/";
	const std::string primarylocation = folder + "TWD_land.pri";
	std::vector<Exception::FMTexc>errors;
	errors.push_back(Exception::FMTexc::FMTmissingyield);
	errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
	errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
	errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
	errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
	errors.push_back(Exception::FMTexc::FMToveridedyield);
	Parser::FMTmodelparser modelparser;
	modelparser.setDefaultExceptionHandler();
	modelparser.setErrorsToWarnings(errors);
	const std::vector<std::string>scenarios(1,"LP");
	const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
	Models::FMTlpmodel optimizationmodel(models.at(0), Models::FMTsolverinterface::CLP);
	optimizationmodel.setParameter(Models::FMTintmodelparameters::LENGTH, 5);
	optimizationmodel.FMTmodel::setParameter(Models::FMTboolmodelparameters::POSTSOLVE, true);
	optimizationmodel.FMTmodel::setParameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, true);
	optimizationmodel.setParameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS, 10);
	Core::FMTOutput out;
	for (const Core::FMTOutput& output : optimizationmodel.getOutputs())
		{
			if (output.getName() == "OVOLREC")
			{
				out = output;
				break;
			}
		}
	Models::FMTlpmodel lowerboundmodel, upperboundmodel;
	lowerboundmodel = upperboundmodel = optimizationmodel;
	//Select the yields to change.
	std::vector<std::string>yieldstochange;
	yieldstochange.push_back("VOLUMETOTAL");
	//Change the Yields...
	const Core::FMTYields baseyields = optimizationmodel.getYields();
	const Core::FMTYields lowerboundyields = baseyields.getFromFactor(0.9, yieldstochange);//-10%
	lowerboundmodel.setYields(lowerboundyields);
	const Core::FMTYields upperboundyields = baseyields.getFromFactor(1.1, yieldstochange);//+10%
	upperboundmodel.setYields(upperboundyields);
	//Solve all models
	lowerboundmodel.doPlanning(true);
	upperboundmodel.doPlanning(true);
	optimizationmodel.doPlanning(true);
	const double basevalue = optimizationmodel.getOutput(out, 1, Core::FMToutputlevel::totalonly).at("Total");
	const double lowervalue = lowerboundmodel.getOutput(out, 1, Core::FMToutputlevel::totalonly).at("Total");
	const double uppervalue = upperboundmodel.getOutput(out, 1, Core::FMToutputlevel::totalonly).at("Total");
	Logging::FMTDefaultLogger() << "LOWER: "<< lowervalue << "BASE: "<< basevalue << "UPPER: "<< uppervalue << "\n";
	if ((lowervalue>uppervalue)||(basevalue<lowervalue)||(basevalue > uppervalue))
	{
		Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
			"presolvetest", __LINE__, primarylocation);
	}
	#endif
	return 0;
	}

