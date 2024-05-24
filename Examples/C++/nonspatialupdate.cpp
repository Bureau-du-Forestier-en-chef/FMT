#include <vector>
#ifdef FMTWITHOSI
	#include "FMTlpmodel.h"
	#include "FMTmodelparser.h"
	#include "FMTversion.h"
	#include "FMTdefaultlogger.h"
	#include "FMTconstraint.h"
	#include "FMTfreeexceptionhandler.h"
	#include "FMTactualdevelopment.h"
	#include "FMTtimeyieldhandler.h"
	#include "FMTdata.h"
	#include "FMTconstraint.h"
	#include "FMTbounds.hpp"
	#include <boost/algorithm/string.hpp>

bool keepandupdate(Core::FMTspec& spec, const int& updateto)
{
	if (spec.emptyperiod())
	{
		return true;
	}
	const int lower_length = spec.getperiodlowerbound();
	const int upper_length = spec.getperiodupperbound();
	if (!(upper_length <= updateto))//Keep the constraint
	{
		const int firstperiod = std::max(1, lower_length - updateto);
		int lastperiod = upper_length;
		if (lastperiod != std::numeric_limits<int>::max())
		{
			lastperiod -= updateto;
		}
		spec.setbounds(Core::FMTperbounds(Core::FMTsection::Optimize, lastperiod, firstperiod));
		return true;
	}
	return false;
}
#endif




int main(int argc, char *argv[])
{
#ifdef FMTWITHOSI
	Logging::FMTdefaultlogger().logstamp();
	
	if (Version::FMTversion().hasfeature("OSI"))
	{
		const std::string folder = "../../../../Examples/Models/TWD_land/";
		const std::string primarylocation = folder + "TWD_land.pri";
		const std::string scenario = "LP_uddate";
		const int length = 5;
		const int updateto = 3;
		Parser::FMTmodelparser modelparser;
		std::vector<Exception::FMTexc>errors;
		errors.push_back(Exception::FMTexc::FMTmissingyield);
		errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
		errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
		errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
		errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
		errors.push_back(Exception::FMTexc::FMToveridedyield);
		modelparser.seterrorstowarnings(errors);
		const std::vector<std::string>scenarios(1,scenario);
		const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTlpmodel optimizationmodel(models.at(0), Models::FMTsolverinterface::MOSEK);
		optimizationmodel.setparameter(Models::FMTintmodelparameters::LENGTH,length);
		optimizationmodel.FMTmodel::setparameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, true);
		optimizationmodel.setparameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS,10);
		optimizationmodel.doplanning(true);
		const std::vector<Core::FMTactualdevelopment>newarea = optimizationmodel.getarea(updateto);
		optimizationmodel.setarea(newarea);
		std::vector<Core::FMTconstraint>constraints;
		for (const Core::FMTconstraint& constraint : optimizationmodel.getconstraints())
			{
			Core::FMTconstraint newconstraint(constraint);
			if (keepandupdate(newconstraint, updateto))
				{
				constraints.push_back(newconstraint);
				}
			}
		optimizationmodel.setconstraints(constraints);
		std::vector<Core::FMTaction>actions;
		for (const Core::FMTaction& action : optimizationmodel.getactions())
			{
			Core::FMTaction newaction(action);
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
		optimizationmodel.setactions(actions);
		std::vector<Core::FMTtransition>transitions;
		for (const Core::FMTtransition& transition : optimizationmodel.gettransitions())
		{
			Core::FMTtransition newtransition(transition);
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
		optimizationmodel.settransitions(transitions);
		Core::FMTyields yields = optimizationmodel.getyields();
		for (auto& yieldlist : yields)
		{
			if (yieldlist.second->gettype() == Core::FMTyldtype::FMTtimeyld)
			{
				Core::FMTtimeyieldhandler* timeyield = dynamic_cast<Core::FMTtimeyieldhandler*>(yieldlist.second.get());
				const int firstbase = *(timeyield->getbases().begin());
				int newbase = std::max(0, firstbase - updateto);
				std::vector<int>bases(1, newbase);
				for (const std::string& yield : timeyield->getyieldnames())
				{
					Core::FMTdata& values = (*timeyield)[yield];
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
				timeyield->setbase(bases);
			}
		}
		optimizationmodel.setyields(yields);
		modelparser.write(optimizationmodel,"../../tests/nonspatialupdate/");


	}else {
		Logging::FMTdefaultlogger() << "FMT needs to be compiled with OSI" << "\n";
	}
#endif 
	return 0;
}
