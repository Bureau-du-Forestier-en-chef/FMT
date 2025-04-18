#ifdef FMTWITHOSI
	#include <vector>
	#include "FMTlpmodel.h"
	#include "FMTmodelparser.h"
	#include "FMTversion.h"
	#include "FMTdefaultlogger.h"
	#include "FMTscheduleparser.h"
	#include "FMTschedule.h"
	#include "FMToutputnode.h"
	#include "FMTfreeexceptionhandler.h"
#endif

int main(int argc, char *argv[])
	{
	#ifdef FMTWITHOSI
	Logging::FMTdefaultlogger().logstamp();
	if (Version::FMTversion().hasfeature("OSI"))
		{
		std::string primarylocation;
		std::vector<std::string>scenarios;
		double ovoltotrecvalue;
		if (argc>1)
		{
			primarylocation = std::string(argv[1]);
			scenarios = std::vector<std::string>(1, std::string(argv[2]));
			if (argc > 3)
				{
				ovoltotrecvalue = std::stod(argv[3]);
				}
		}else {
			primarylocation = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/Interne/FMT/Entretien/Modeles_test/01171/PC_7719_U01171_4_2018_VPF04.pri";
			scenarios = std::vector<std::string>(1, "scenario_4");
			ovoltotrecvalue = 2085200.026;
			argc = 4;
		}

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
			bool gotovoltotrec = false;
			for (const Core::FMToutput& output : optimizationmodel.getoutputs())
				{
				if (output.getname()=="OVOLTOTREC")
					{
					gotovoltotrec = true;
					const double returnedvalue = optimizationmodel.getoutput(output, 2, Core::FMToutputlevel::totalonly).at("Total");
                    Logging::FMTdefaultlogger() << "returned value : "+std::to_string(returnedvalue)+ "\nValue : "+std::to_string(ovoltotrecvalue) << "\n";
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
		Logging::FMTdefaultlogger() << "FMT needs to be compiled with OSI" << "\n";
		}
	#endif
	return 0;
	}