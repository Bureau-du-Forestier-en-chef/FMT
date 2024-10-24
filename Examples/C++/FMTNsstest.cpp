#include <vector>
#include <cmath>
#include "FMTnssmodel.h"
#include "FMTmodelparser.h"
#include "FMTversion.h"
#include "FMTdefaultlogger.h"
#include "FMTfreeexceptionhandler.h"
#include "FMToutput.h"
	

int main(int argc, char* argv[])
{
	Logging::FMTdefaultlogger().logstamp();
	std::string PRIMARY;
	std::string SCENARIO;
	std::string  OUTPUT;
	int LENGTH;
	double OUTPUT_VALUE;
	int PERIOD;
	if (argc > 1)
	{
		const std::string vals = argv[1];
		std::vector<std::string>results;
		boost::split(results, vals, boost::is_any_of("|"));
		PRIMARY = results.at(0);
		SCENARIO = results.at(1);
		OUTPUT = results.at(2);
		PERIOD = std::stoi(results.at(3));
		LENGTH = std::stoi(argv[2]);
		if (argc == 4)
		{
			OUTPUT_VALUE = std::stod(argv[3]);
		}
	}
	else {
		argc = 4;
		PRIMARY = "D:/FMT/Examples/Models/TWD_land/TWD_land.pri";
		SCENARIO = "DECISION";
		OUTPUT = "UNIT_REC";
		LENGTH = 1;
		OUTPUT_VALUE = 60;
		PERIOD = 5;
	}
	Parser::FMTmodelparser mparser;
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
	errors.push_back(Exception::FMTexc::FMTinvalid_geometry);
	mparser.seterrorstowarnings(errors);
	const std::vector<std::string>SCENARIOS(1, SCENARIO);
	const std::vector<Models::FMTmodel> MODELS = mparser.readproject(PRIMARY, SCENARIOS);
	Models::FMTnssmodel NssModel(MODELS.at(0),0);
	NssModel.setparameter(Models::FMTintmodelparameters::UPDATE, 1);
	std::vector<Core::FMTactualdevelopment>newDevs;
	for (Core::FMTactualdevelopment dev : NssModel.getarea())
		{
		dev.setperiod(PERIOD-1);
		newDevs.push_back(dev);
		}
	NssModel.setarea(newDevs);
	NssModel.setparameter(Models::FMTintmodelparameters::LENGTH, LENGTH);
	NssModel.doplanning(true);
	Core::FMToutput sumOutput;
	for (const Core::FMToutput& output : NssModel.getoutputs())
	{
		if (output.getname() == OUTPUT)
		{
			sumOutput = output;
		}
	}
	
	const double RESULT = NssModel.getoutput(sumOutput,PERIOD, Core::FMToutputlevel::totalonly).at("Total");
	Logging::FMTdefaultlogger() << "VALUE OF "<< RESULT<<" "<< OUTPUT_VALUE << "\n";
	if (argc == 4 && std::abs(RESULT - OUTPUT_VALUE)>1)
	{
		Logging::FMTdefaultlogger() << "bad" << "\n";
		Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
			"FMTNsstest", __LINE__, PRIMARY);
	}
	return 0;
}
        
