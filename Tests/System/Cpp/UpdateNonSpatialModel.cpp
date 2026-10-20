/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include <vector>
#include "TestTools.h"
#ifdef FMTWITHOSI
	#include <filesystem>
	#include <string>
	#include "FMTLpModel.h"
	#include "FMTModelParser.h"
	#include "FMTDefaultLogger.h"
	#include "FMTConstraint.h"
	#include "FMTActualDevelopment.h"
	#include "FMTTimeYieldHandler.h"
	#include "FMTData.h"
	#include "FMTBounds.hpp"
	#include <boost/algorithm/string.hpp>

// Checks a solved model moved to a later period, as a replanning does: its area becomes the area of the solution at
// that period (FMTLpModel::getArea), its constraints, actions, transitions and time yields are shifted by as many
// periods, and it is written by FMTModelParser::write. The area of the model moved must be the total area of the
// inventory, and the model read back from what was written must have this area and the shifted constraints.
//
// argv[1]: the primary file, TWD_land by default.
// argv[2]: the scenario, LP_uddate by default.
// argv[3]: the total area of its inventory, computed by hand from its area section.
namespace
{
	double getTotalArea(const std::vector<Core::FMTActualDevelopment>& p_area)
	{
		double totalArea = 0.0;
		for (const Core::FMTActualDevelopment& development : p_area)
		{
			totalArea += development.getArea();
		}
		return totalArea;
	}
}

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
	return Testing::runTest([&]()
		{
		// Arrange
		Logging::FMTDefaultLogger().logStamp();
		const std::string primarylocation = (argc > 1) ? argv[1] : "../../../../Examples/Models/TWD_land/TWD_land.pri";
		const std::string scenario = (argc > 2) ? argv[2] : "LP_uddate";
		const double EXPECTED_TOTAL_AREA = (argc > 3) ? std::stod(argv[3]) : 1814.76;
		const double AREA_TOLERANCE = 1e-4;
		const int length = 5;
		const int updateto = 3;
		const std::filesystem::path OUTPUT_FOLDER = "../../tests/UpdateNonSpatialModel/";
		std::filesystem::remove_all(OUTPUT_FOLDER);
		std::filesystem::create_directories(OUTPUT_FOLDER);
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
		Models::FMTLpModel optimizationmodel(models.at(0), Models::FMTSolverInterface::MOSEK);
		optimizationmodel.setParameter(Models::FMTintmodelparameters::LENGTH,length);
		optimizationmodel.FMTModel::setParameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, true);
		optimizationmodel.setParameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS,10);
		const bool SOLVED = optimizationmodel.doPlanning(true);

		// Act
		const std::vector<Core::FMTActualDevelopment>newarea = optimizationmodel.getArea(updateto);
		optimizationmodel.setArea(newarea);
		std::vector<Core::FMTConstraint>constraints;
		for (const Core::FMTConstraint& constraint : optimizationmodel.getConstraints())
			{
			Core::FMTConstraint newconstraint(constraint);
			if (keepandupdate(newconstraint, updateto))
				{
				constraints.push_back(newconstraint);
				}
			}
		optimizationmodel.setConstraints(constraints);
		std::vector<Core::FMTAction>actions;
		for (const Core::FMTAction& action : optimizationmodel.getActions())
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
		modelparser.write(optimizationmodel, OUTPUT_FOLDER.string());
		const Models::FMTModel REREAD = modelparser.readproject((OUTPUT_FOLDER / (optimizationmodel.getName() + ".pri")).string(),
			std::vector<std::string>(1, "ROOT")).at(0);

		// Assert
		Testing::Checker checker;
		checker.check(SOLVED, scenario + " is solved over " + std::to_string(length) + " periods");
		checker.checkNear(getTotalArea(optimizationmodel.getArea()), EXPECTED_TOTAL_AREA, AREA_TOLERANCE,
			"total area of the model moved to period " + std::to_string(updateto));
		checker.check(REREAD.getArea().size() == optimizationmodel.getArea().size(),
			std::to_string(REREAD.getArea().size()) + " developments read back, " + std::to_string(optimizationmodel.getArea().size()) + " written");
		checker.checkNear(getTotalArea(REREAD.getArea()), EXPECTED_TOTAL_AREA, AREA_TOLERANCE, "total area read back");
		checker.check(REREAD.getConstraints() == optimizationmodel.getConstraints(), "shifted constraints read back");
		return checker.exitCode();
		});
#else
	return Testing::skip("FMT is compiled without OSI");
#endif
}
