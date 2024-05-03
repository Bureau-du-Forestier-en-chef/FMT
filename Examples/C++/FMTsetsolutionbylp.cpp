#ifdef FMTWITHOSI
	#include <vector>
	#include "FMTlpmodel.h"
	#include "FMTmodelparser.h"
	#include "FMTversion.h"
	#include "FMTlogger.h"
	#include "FMTscheduleparser.h"
	#include "FMTschedule.h"
	#include "FMToutputnode.h"
	#include "FMTfreeexceptionhandler.h"
#endif

int main(int argc, char *argv[])
	{
	#ifdef FMTWITHOSI
	Logging::FMTlogger().logstamp();
	if (Version::FMTversion().hasfeature("OSI"))
		{
		const std::string primarylocation = std::string(argv[1]);
		Parser::FMTmodelparser modelparser;
		modelparser.setdefaultexceptionhandler();
		std::vector<Exception::FMTexc>errors;
		errors.push_back(Exception::FMTexc::FMTmissingyield);
		errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
		errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
		errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
		errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
		errors.push_back(Exception::FMTexc::FMToveridedyield);
        errors.push_back(Exception::FMTexc::FMTdeathwithlock);
		modelparser.seterrorstowarnings(errors);
		const std::vector<std::string>scenarios(1,std::string(argv[2]));
		const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTlpmodel optimizationmodel(models.at(0), Models::FMTsolverinterface::CLP);
		const std::vector<Core::FMTschedule>schedules = modelparser.readschedules(primarylocation,models).at(0);
		const double tolerance = 0.0001;
		for (size_t period = 1; period <= 6; ++period)
			{
			optimizationmodel.buildperiod(schedules.at(period-1),true);
			}
		for (size_t period = 1; period <= 6; ++period)
			{
			optimizationmodel.setsolutionbylp(period,schedules.at(period-1), tolerance);
			}
		if (argc>3)//Got the double for validation!
			{
			const double ovoltotrecvalue = std::stod(argv[3]);
			bool gotovoltotrec = false;
			for (const Core::FMToutput& output : optimizationmodel.getoutputs())
				{
				if (output.getname()=="OVOLTOTREC")
					{
					gotovoltotrec = true;
					const double returnedvalue = optimizationmodel.getoutput(output, 2, Core::FMToutputlevel::totalonly).at("Total");
                    Logging::FMTlogger() << "returned value : "+std::to_string(returnedvalue)+ "\nValue : "+std::to_string(ovoltotrecvalue) << "\n";
					if ((returnedvalue < (ovoltotrecvalue - 1))||(returnedvalue > (ovoltotrecvalue + 1)))
						{
						Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
							"FMTsetsolutionbylp", __LINE__, primarylocation);
						}
					break;
					}
				}
			if (!gotovoltotrec)
				{
				Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "No OVOLTOTREC OUPUT",
					"FMTsetsolution", __LINE__, primarylocation);
				}
			}

	}else {
		Logging::FMTlogger() << "FMT needs to be compiled with OSI" << "\n";
		}
	#endif
	return 0;
	}