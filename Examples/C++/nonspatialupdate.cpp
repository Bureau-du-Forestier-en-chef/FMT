#include <vector>
#ifdef FMTWITHOSI
	#include "FMTLpModel.h"
	#include "FMTModelParser.h"
	#include "FMTVersion.h"
	#include "FMTDefaultLogger.h"
	#include "FMTConstraint.h"
	#include "FMTFreeExceptionHandler.h"
	#include "FMTActualDevelopment.h"
	#include "FMTTimeYieldHandler.h"
	#include "FMTData.h"
	#include "FMTConstraint.h"
	#include "FMTBounds.hpp"
	#include <boost/algorithm/string.hpp>

bool keepandupdate(Core::FMTSpec& spec, const int& updateto)
{
	if (spec.emptyPeriod())
	{
		return true;
	}
	const int lower_length = spec.getPeriodLowerBound();
	const int upper_length = spec.getPeriodUpperBound();
	if (!(upper_length <= updateto))//Keep the constraint
	{
		const int firstPeriod = std::max(1, lower_length - updateto);
		int lastPeriod = upper_length;
		if (lastPeriod != std::numeric_limits<int>::max())
		{
			lastPeriod -= updateto;
		}
		spec.setBounds(Core::FMTPerBounds(Core::FMTsection::Optimize, lastPeriod, firstPeriod));
		return true;
	}
	return false;
}
#endif




int main(int argc, char *argv[])
{
#ifdef FMTWITHOSI
	Logging::FMTDefaultLogger().logStamp();
	
	if (Version::FMTVersion().hasFeature("OSI"))
	{
		const std::string folder = "../../../../Examples/Models/TWD_land/";
		const std::string primarylocation = folder + "TWD_land.pri";
		const std::string scenario = "LP_uddate";
		const int length = 5;
		const int updateto = 3;
		Parser::FMTModelParser modelparser;
		std::vector<Exception::FMTexc>errors;
		errors.push_back(Exception::FMTexc::FMTmissingyield);
		errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
		errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
		errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
		errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
		errors.push_back(Exception::FMTexc::FMToveridedyield);
		modelparser.setErrorsToWarnings(errors);
		const std::vector<std::string>scenarios(1,scenario);
		const std::vector<Models::FMTModel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTLpModel optimizationmodel(models.at(0), Models::FMTsolverinterface::MOSEK);
		optimizationmodel.setParameter(Models::FMTintmodelparameters::LENGTH,length);
		optimizationmodel.FMTModel::setParameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, true);
		optimizationmodel.setParameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS,10);
		optimizationmodel.doPlanning(true);
		const std::vector<Core::FMTActualDevelopment>newarea = optimizationmodel.getArea(updateto);
		optimizationmodel.setArea(newarea);
		std::vector<Core::FMTConstraint>constraints;
		for (const Core::FMTConstraint& constraint : optimizationmodel.getconstraints())
			{
			Core::FMTConstraint newconstraint(constraint);
			if (keepandupdate(newconstraint, updateto))
				{
				constraints.push_back(newconstraint);
				}
			}
		optimizationmodel.setConstraints(constraints);
		std::vector<Core::FMTAction>actions;
		for (const Core::FMTAction& action : optimizationmodel.getactions())
			{
			Core::FMTAction newaction(action);
			bool gotone = false;
			for (auto& actl : newaction)
				{
				if (keepandupdate(actl.second, updateto))
					{
					gotone = true;
					}

				}
			if (gotone)
				{
				actions.push_back(newaction);
				}
			}
		optimizationmodel.setActions(actions);
		std::vector<Core::FMTTransition>transitions;
		for (const Core::FMTTransition& transition : optimizationmodel.getTransitions())
		{
			Core::FMTTransition newtransition(transition);
			bool gotone = false;
			for (auto& actl : newtransition)
			{
				if (keepandupdate(actl.second, updateto))
				{
					gotone = true;
				}

			}
			if (gotone)
			{
				transitions.push_back(newtransition);
			}
		}
		optimizationmodel.setTransitions(transitions);
		Core::FMTYields yields = optimizationmodel.getYields();
		for (auto& yieldlist : yields)
		{
			if (yieldlist.second->getType() == Core::FMTyldtype::FMTtimeyld)
			{
				Core::FMTTimeYieldHandler* timeyield = dynamic_cast<Core::FMTTimeYieldHandler*>(yieldlist.second.get());
				const int firstbase = *(timeyield->getBases().begin());
				int newbase = std::max(0, firstbase - updateto);
				std::vector<int>bases(1, newbase);
				for (const std::string& yield : timeyield->getYieldNames())
				{
					Core::FMTData& values = (*timeyield)[yield];
					if (firstbase <= updateto)
					{
						int keepfirst = 0;
						if (firstbase == 0)
						{
							keepfirst = 1;
						}
						values.data.erase(values.data.begin() + keepfirst, values.data.begin() + (updateto - firstbase)+1);
					}
				}
				timeyield->setBase(bases);
			}
		}
		optimizationmodel.setYields(yields);
		modelparser.write(optimizationmodel,"../../tests/nonspatialupdate/");


	}else {
		Logging::FMTDefaultLogger() << "FMT needs to be compiled with OSI" << "\n";
	}
#endif 
	return 0;
}
