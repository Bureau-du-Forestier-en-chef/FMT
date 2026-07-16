#include <vector>
#ifdef FMTWITHOSI
#include "FMTlpmodel.h"
#include "FMTmodelparser.h"
#include "FMTversion.h"
#include "FMTdefaultlogger.h"
#include "FMTconstraint.h"
#include "FMTfreeexceptionhandler.h"
#endif


int main()
{
#ifdef FMTWITHOSI
	Logging::FMTDefaultLogger().logStamp();
	if (Version::FMTversion().hasFeature("OSI"))
	{
		const std::string primarylocation = "../../../../Examples/Models/TWD_land/TWD_land.pri";
		const std::string scenario = "landaggregates";
		const int length = 5;
		const double objectivevalue = 43140.504922;
		Parser::FMTModelParser modelparser;
		std::vector<Exception::FMTexc>errors;
		errors.push_back(Exception::FMTexc::FMTmissingyield);
		errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
		errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
		errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
		errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
		errors.push_back(Exception::FMTexc::FMToveridedyield);
		errors.push_back(Exception::FMTexc::FMTsourcetotarget_transition);
		errors.push_back(Exception::FMTexc::FMTsame_transitiontargets);
		modelparser.setErrorsToWarnings(errors);
		const std::vector<std::string>scenarios(1, scenario);
		const std::vector<Models::FMTModel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTLpModel optimizationmodel(models.at(0), Models::FMTsolverinterface::CLP);
		optimizationmodel.setParameter(Models::FMTintmodelparameters::LENGTH, length);
		optimizationmodel.FMTModel::setParameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, true);
		optimizationmodel.setParameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS,10);
		if (optimizationmodel.doPlanning(true)) {
			std::cout << std::to_string(optimizationmodel.getObjValue()) << std::endl;
		}
		if ((std::abs(optimizationmodel.getObjValue() - objectivevalue)) >= 1)
		{
			Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
				"presolvetest", __LINE__, primarylocation);
		}
	}
	else {
		Logging::FMTDefaultLogger() << "FMT needs to be compiled with OSI" << "\n";
	}
#endif 
	return 0;
}

