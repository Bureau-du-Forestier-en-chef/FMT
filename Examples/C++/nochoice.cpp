#include <vector>
#ifdef FMTWITHOSI
#include "FMTlpmodel.h"
#include "FMTmodelparser.h"
#include "FMTscheduleparser.h"
#include "FMTversion.h"
#include "FMTdefaultlogger.h"
#include "FMTconstraint.h"
#include "FMTfreeexceptionhandler.h"
#include <boost/algorithm/string.hpp>
#include "FMTareaparser.h"
#endif


int main(int argc, char* argv[])
{
#ifdef FMTWITHOSI
	Logging::FMTDefaultLogger().logStamp();
	if (Version::FMTversion().hasFeature("OSI"))
	{
		const std::string vals = argv[1];
		std::vector<std::string>results;
		boost::split(results, vals, boost::is_any_of("|"));
		const std::string primarylocation = results.at(0);
		const std::string scenario = results.at(1);
		const std::string outputname =  argv[2];
		const int no_action_size =  std::stoi(argv[3]);
		const int length = 5;
		const std::vector<std::string>scenarios(1, scenario);
		Parser::FMTmodelparser modelparser;
		std::vector<Exception::FMTexc>errors;
		errors.push_back(Exception::FMTexc::FMTmissingyield);
		errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
		errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
		errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
		errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
		errors.push_back(Exception::FMTexc::FMToveridedyield);
		errors.push_back(Exception::FMTexc::FMTsourcetotarget_transition);
		errors.push_back(Exception::FMTexc::FMTsame_transitiontargets);
		errors.push_back(Exception::FMTexc::FMTunclosedforloop);
		errors.push_back(Exception::FMTexc::FMToutofrangeyield);
		modelparser.setErrorsToWarnings(errors);
		const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTlpmodel optimizationmodel(models.at(0), Models::FMTsolverinterface::CLP);
		optimizationmodel.setParameter(Models::FMTintmodelparameters::LENGTH, length);
		optimizationmodel.FMTmodel::setParameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE,true);
		optimizationmodel.setParameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS,10);
		optimizationmodel.setParameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, 1);
		optimizationmodel.doPlanning(false);
		for (const Core::FMTOutput& output : optimizationmodel.getOutputs())
			{
			if (output.getName() == outputname)
				{
					const Core::FMTMask mask = output.getSources().at(0).getMask();
					const int size_of_no_choice = optimizationmodel.getNoChoice(mask).size();
					if (size_of_no_choice!=no_action_size)
						{
						Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
							"FMTsetsolution", __LINE__, primarylocation);
						}
				break;
				}
			}
	}

#endif 
	return 0;
}

