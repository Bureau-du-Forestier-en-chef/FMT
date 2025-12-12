#include <vector>
#ifdef FMTWITHOSI
	#include "FMTtaskhandler.h"
	#include "FMTplanningtask.h"
	#include "FMTlpmodel.h"
	#include "FMTnssmodel.h"
	#include "FMTfreeexceptionhandler.h"
	#include "FMTmodelparser.h"
#endif
#include "FMTdefaultlogger.h"

int main(int argc, char *argv[])
	{
	#ifdef FMTWITHOSI
		Logging::FMTdefaultlogger().logstamp();
		std::string primlocation;
		std::string templatefolder;
		int length;
		if (argc > 1) 
		{
			primlocation = argv[1];
			templatefolder = argv[2];
			length = std::stoi(argv[3]);
		}
		else
		{
			primlocation = "C:\\Users\\Admlocal\\Documents\\issues\\C2_01010307\\PC_10116_U05151_MODB01_20250912\\PC_10116_U05151_2028_MODB01.pri";
			templatefolder = "C:\\Users\\Admlocal\\Documents\\issues\\C2_01010307\\Template_complexite";
			length = 20;
		}
		Parser::FMTmodelparser modelparser;
		std::vector<Exception::FMTexc> errors;
		errors.push_back(Exception::FMTexc::FMTmissingyield);
		errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
		errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
		errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
		errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
		errors.push_back(Exception::FMTexc::FMToveridedyield);
		errors.push_back(Exception::FMTexc::FMToutofrangeyield);
		modelparser.seterrorstowarnings(errors);
		const std::vector<Models::FMTmodel> models = modelparser.readtemplates(primlocation, templatefolder);
		if (models.size() < 1)
			{
			Exception::FMTfreeexceptionhandler().raise(
				Exception::FMTexc::FMTfunctionfailed, 
				"Wrong number of models loaded from template folder","", __LINE__, "");
			}
		for (size_t id = 0; id < models.size(); ++id)
		{
			Models::FMTsolverinterface solver = (argc > 1)
				? Models::FMTsolverinterface::CLP 
				: Models::FMTsolverinterface::MOSEK;
			Models::FMTlpmodel test(models.at(id), solver);
			test.setparameter(Models::FMTintmodelparameters::LENGTH, length);
			test.doplanning(true);
		}
	#endif
	return 0;
	}

