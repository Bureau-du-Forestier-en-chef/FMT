#ifdef FMTWITHOSI
	#include "FMTlpmodel.h"
	#include "FMTmodelparser.h"
	#include "FMTscheduleparser.h"
	#include "FMTversion.h"
	#include "FMTexception.h"
	#include "FMTdevelopment.h"
	#include "FMTyieldrequest.h"
	#include "FMTgraphvertextoyield.h"
	#include "FMTfreeexceptionhandler.h"
	#include "FMTdefaultlogger.h"
	#include <vector>
#endif

int main(int argc, char *argv[])
	{
	#ifdef FMTWITHOSI
	Logging::FMTDefaultLogger().logStamp();
	if (Version::FMTversion().hasFeature("OSI"))
		{
		std::string primarylocation;
		std::string scenario;
		std::string maskstr;
		int age;
		int period;
		std::string yieldname;
		double yieldvalue;
		if (argc > 1) {
		std::vector<std::string>results;
		const std::string vals = argv[1];
		boost::split(results, vals, boost::is_any_of("|"));
		primarylocation =  results.at(0);
		scenario = results.at(1);
		maskstr = results.at(2);
		age = std::stoi(results.at(3));
		period = std::stoi(results.at(4));
		yieldname = std::string(argv[2]);
		yieldvalue = std::stod(argv[3]);
		}
		else
		{
			primarylocation = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/Interne/FMT/Entretien/Modeles_test/CahierCOS_SPAT_P10_IT1_V/13_Sc5a_Determin_avsp.pri";
			scenario = "ROOT";
			maskstr = "? ? ? ? ? ? ? INC ? ? ? ? ? P27037 ? ? ? ? ? ?";
			yieldname = "YOUVERT";
			yieldvalue = 1;
			age = 0;
			period = 17;

		}
		Parser::FMTmodelparser modelparser;
		modelparser.setDefaultExceptionHandler();
		std::vector<Exception::FMTexc>errors;
		errors.push_back(Exception::FMTexc::FMTmissingyield);
		errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
		errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
		errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
		errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
		errors.push_back(Exception::FMTexc::FMToveridedyield);
		errors.push_back(Exception::FMTexc::FMToutofrangeyield);
		modelparser.setErrorsToWarnings(errors);

		const std::vector<std::string>scenarios(1, scenario);
		const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTlpmodel optimizationmodel(models.at(0), Models::FMTsolverinterface::MOSEK);
		const std::vector<Core::FMTschedule>schedules = modelparser.readschedules(primarylocation,models).at(0);
		
		const double tolerance = 0.01;
		optimizationmodel.setParameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, true);
		optimizationmodel.FMTmodel::setParameter(Models::FMTdblmodelparameters::TOLERANCE, tolerance);
		//optimizationmodel.FMTmodel::setParameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS, 0);
		optimizationmodel.FMTmodel::setParameter(Models::FMTintmodelparameters::LENGTH, schedules.back().getPeriod());
		
		const bool FEASIBLE = optimizationmodel.doPlanning( false, schedules);
		/*if (FEASIBLE)
		{
			std::vector<Core::FMTschedule>OutSchedules;
			for (int period = 1; period <= 10; ++period)
			{
				OutSchedules.push_back(optimizationmodel.getSolution(period));
			}
			Parser::FMTscheduleparser scparser;
			scparser.write(OutSchedules, "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/Interne/FMT/Entretien/Modeles_test/CahierCOS_SPAT_P10_IT1_V/test.seq");
		}*/
		
		bool gotovoltotrec = false;
		const Core::FMTdevelopment adev(Core::FMTmask(maskstr,optimizationmodel.getThemes()), age, 0, period);
		const Graph::FMTgraphvertextoyield graph_info = optimizationmodel.getGraphVertexToYield();
		const Core::FMTyieldrequest yieldrequest = adev.getYieldRequest(&graph_info);
		const double returnedvalue = optimizationmodel.getYields().get(yieldrequest,yieldname);
		Logging::FMTDefaultLogger() << returnedvalue << "\n";
		if ((returnedvalue < (yieldvalue - tolerance)) || (returnedvalue > (yieldvalue + tolerance)))
			{
			Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
				"FMTsetsolution", __LINE__, primarylocation);
			}
	}else {
		Logging::FMTDefaultLogger() << "FMT needs to be compiled with OSI" << "\n";
		}
	#endif
	return 0;
	}

