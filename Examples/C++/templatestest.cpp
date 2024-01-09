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
		const std::string templatefolder = "../../../../Examples/Models/TWD_land/Templates";
		const std::string primlocation = folder + "TWD_land.pri";
		Parser::FMTmodelparser modelparser;
		const std::vector<Models::FMTmodel> models = modelparser.readtemplates(primlocation, templatefolder);
		if (models.size() < 2)
			{
			Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong number of models loaded from tempalte folder","", __LINE__, "");
			}
		for (size_t id = 1; id < models.size();++id)
		{
			Models::FMTlpmodel test(models.at(id), Models::FMTsolverinterface::CLP);
			test.setparameter(Models::FMTintmodelparameters::LENGTH, 5);
			test.doplanning(true);
		}
	#endif
	return 0;
	}

