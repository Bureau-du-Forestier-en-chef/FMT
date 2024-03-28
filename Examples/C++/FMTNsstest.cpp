#include <vector>
#include <cmath>
#include "FMTnssmodel.hpp"
#include "FMTmodelparser.hpp"
#include "FMTversion.hpp"
#include "FMTlogger.hpp"
#include "FMTfreeexceptionhandler.hpp"
#include "FMToutput.hpp"
	

int main(int argc, char* argv[])
{
	Logging::FMTlogger().logstamp();
	/*const std::string vals = argv[1];
	std::vector<std::string>results;
	boost::split(results, vals, boost::is_any_of("|"));
	const std::string PRIMARY = results.at(0);
	const std::string SCENARIO = results.at(1);
	const std::string  OUTPUT = results.at(2);
	const int LENGTH = std::stoi(argv[2]);
	const double OUTPUT_VALUE = std::stod(argv[3]);*/
	const std::string PRIMARY = "D:/FMT/Examples/Models/TWD_land/TWD_land.pri";
	const std::string SCENARIO = "UNIT_COVERAGE";
	const std::string  OUTPUT = "UNIT_VOLUME";
	const int LENGTH = 1;
	const double OUTPUT_VALUE = 84.73;

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
	
	const double RESULT = NssModel.getoutput(sumOutput,1, Core::FMToutputlevel::totalonly).at("Total");
	Logging::FMTlogger() << "VALUE OF "<< RESULT <<"\n";
	if (std::abs(RESULT - OUTPUT_VALUE)>1)
	{
		Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
			"FMTNsstest", __LINE__, PRIMARY);
	}
	return 0;
}
        
