#ifdef FMTWITHOSI
	#include <vector>
	#include <cmath>
	#include "FMTlpmodel.h"
	#include "FMTmodelparser.h"
	#include "FMTareaparser.h"
	#include "FMTversion.h"
	#include "FMTdefaultlogger.h"
	#include "FMTscheduleparser.h"
	#include "FMTschedule.h"
	#include "FMToutputnode.h"
	#include "FMTfreeexceptionhandler.h"
	#include "FMTactualdevelopment.h"
#endif

int main()
	{
	#ifdef FMTWITHOSI
	Logging::FMTdefaultlogger().logStamp();
	if (Version::FMTversion().hasFeature("OSI"))
		{
		const std::string modellocation = "../../../../Examples/Models/TWD_land/";
        const std::string	primarylocation = modellocation + "TWD_land.pri";
		const std::string testfolderout = "../../tests/forcesolution/";
		Parser::FMTmodelparser modelparser;
		Parser::FMTareaparser areaparser;
		Parser::FMTscheduleparser scheparser;
		const std::vector<std::string>scenarios(1,"Forcesolutiontest");
		const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTmodel initialmodel = models.at(0);
		const int lenght = 6;
		initialmodel.setParameter(Models::FMTintmodelparameters::LENGTH, lenght);
		initialmodel.setParameter(Models::FMTboolmodelparameters::POSTSOLVE, false);
		initialmodel.setParameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS, 0);
		initialmodel.setParameter(Models::FMTdblmodelparameters::TOLERANCE, 0.001);
		Models::FMTlpmodel optimizationmodel(initialmodel, Models::FMTsolverinterface::CLP);
		const std::vector<Core::FMTschedule>schedules = modelparser.readschedules(primarylocation,models).at(0);
		optimizationmodel.doPlanning(false,schedules);
        std::vector<Core::FMTschedule> lockedproportionscheduled;
		for (size_t period = 1; period <= static_cast<size_t>(lenght); ++period)
		{
			lockedproportionscheduled.push_back(optimizationmodel.getScheduleProportions(period,true));
		}
		optimizationmodel = Models::FMTlpmodel(initialmodel, Models::FMTsolverinterface::CLP);
		// ici on vient changer la section area 
		#ifdef FMTWITHGDAL
		const std::vector<Core::FMTactualdevelopment> newarea = areaparser.readVectors(	optimizationmodel.getThemes(),
																						modellocation+"Carte/TWD_LAND_forcesolution_modif.shp",
                                   														"AGE","SUPERFICIE", 1.0, 1);
		#else
		const std::vector<Core::FMTactualdevelopment> newarea = areaparser.read(optimizationmodel.getThemes(), Core::FMTconstants(), modellocation + "Carte/TWD_LAND_forcesolution_modif.are")
		#endif
		areaparser.write(newarea,testfolderout+"forcemodifshp._area");
		scheparser.write(lockedproportionscheduled,testfolderout+"lockedandpropos._seq");
        optimizationmodel.setArea(newarea);
		std::vector<Core::FMTschedule> mschedules;
        for (size_t period = 1; period <=  static_cast<size_t>(lenght); ++period)
		{
			const Core::FMTschedule periodpropschedule = lockedproportionscheduled.at(period-1);
			optimizationmodel.buildPeriod(periodpropschedule,true);
			optimizationmodel.forceSolution(period,periodpropschedule);
			mschedules.push_back(optimizationmodel.getSolution(period,true));
		}
		scheparser.write(mschedules,testfolderout+"forcemodif._seq");
		const std::vector<Core::FMTactualdevelopment> aream = optimizationmodel.getArea();
		double totarea=0;
		for(const auto& actdev : aream)
		{
			totarea+=actdev.getArea();
		}
		areaparser.write(aream,testfolderout+"forcemodif._area");
		const std::vector<Core::FMToutput> outputs = optimizationmodel.getOutputs();
		const std::vector<Core::FMToutput>::const_iterator out_it = find_if(outputs.begin(),outputs.end(),Core::FMTOutputComparator("AREAACTIONS"));
		const std::vector<double> values = {1533.167187, 1710.335763, 1520.664235,1807.036679, 1319.965304, 667.893761};
		for (size_t period = 1; period <= static_cast<size_t>(lenght); ++period)
		{
			const double sum_actions = optimizationmodel.getOutput(*out_it,period,Core::FMToutputlevel::totalonly).at("Total");
			//std::cout<<std::to_string(period)+" "+std::to_string(sum_actions)<<std::endl;
			if(std::abs(sum_actions-values.at(period-1))>0.001)
			{
				Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
					"forcesolution", __LINE__, primarylocation);
			}
			double parea=0;
			for(const auto& actdev : optimizationmodel.getArea(period))
			{
				parea+=actdev.getArea();
			}
			if(std::abs(parea-totarea)>0.01)
			{
				std::cout<<"period start : "+std::to_string(parea)+" initial : "+std::to_string(totarea)<<std::endl;
				Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Difference in area at the period begining",
					"forcesolution", __LINE__, primarylocation);
			}
		}
	}else {
		Logging::FMTdefaultlogger() << "FMT needs to be compiled with OSI" << "\n";
		}
	#endif
	return 0;
	}

