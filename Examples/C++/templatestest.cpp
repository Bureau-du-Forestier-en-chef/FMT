#include <vector>
#ifdef FMTWITHOSI
	#include "FMTtaskhandler.hpp"
	#include "FMTplanningtask.hpp"
	#include "FMTlpmodel.hpp"
	#include "FMTnssmodel.hpp"
	#include "FMTfreeexceptionhandler.hpp"
	#include "FMTmodelparser.hpp"
#endif

int main(int argc, char *argv[])
	{
	#ifdef FMTWITHOSI
		Logging::FMTlogger().logstamp();
		const std::string folder = "../../../../Examples/Models/TWD_land/";
		const std::string templatefolder =  "../../../../Examples/Models/TWD_land/Templates";
		const std::string primlocation =  folder + "TWD_land.pri";
		Parser::FMTmodelparser modelparser;
		std::vector<Exception::FMTexc>errors;
		errors.push_back(Exception::FMTexc::FMTmissingyield);
		errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
		errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
		errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
		errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
		errors.push_back(Exception::FMTexc::FMToveridedyield);
		errors.push_back(Exception::FMTexc::FMToutofrangeyield);
		modelparser.seterrorstowarnings(errors);
		const std::vector<Models::FMTmodel> models = modelparser.readtemplates(primlocation, templatefolder);
		if (models.size() < 2)
			{
			Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong number of models loaded from tempalte folder","", __LINE__, "");
			}
		for (size_t id = 0; id < models.size();++id)
		{
			Models::FMTlpmodel test(models.at(id), Models::FMTsolverinterface::CLP);
			test.setparameter(Models::FMTintmodelparameters::LENGTH,5);
			test.doplanning(true);
		}
	#endif
	return 0;
	}

