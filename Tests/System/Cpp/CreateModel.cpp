/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
#include "FMTModel.h"
#include "FMTTheme.h"
#include "FMTMask.h"
#include "FMTYields.h"
#include "FMTAction.h"
#include "FMTTransition.h"
#include "FMTOutput.h"
#include "FMTConstraint.h"
#include "FMTBounds.hpp"
#include "FMTAgeYieldHandler.h"
#include "FMTActualDevelopment.h"
#include "FMTLifespans.h"
#include "FMTModelParser.h"
#include "FMTDefaultLogger.h"
#include "FMTLpModel.h"
#include "FMTTransitionMask.h"
#include "FMTFork.h"
#include <cmath>
#include <filesystem>
#include <memory>
#include <vector>
#include <string>
#endif
#include "TestTools.h"

// Checks a model built in code, section by section, then written by FMTModelParser::write: it must be optimal, and
// the model read back from what was written must reach the same optimum. Its sections are not compared: a model
// built in code leaves out what a read fills in, such as the names of the attributes, and FMT adds a default _DEATH.
int main()
{
#ifdef FMTWITHOSI
	return Testing::runTest([&]()
		{
		// Arrange
		Logging::FMTDefaultLogger().logStamp();

		//Create Themes
		//////////////////////////////////////////////////
		//Create attributes
		std::vector<std::vector<std::string>>attributes(3,std::vector<std::string>());
		attributes[0].push_back("GS1");
		attributes[0].push_back("GS2");
		attributes[0].push_back("GS3");
		attributes[1].push_back("FC4");
		attributes[1].push_back("FC6");
		attributes[2].push_back("COS8");
		attributes[2].push_back("COS20");
		attributes[2].push_back("COS30");
		attributes[2].push_back("COS60");
		//Create aggregates
		std::vector<std::vector<std::string>>aggregates(3,std::vector<std::string>());
		aggregates[2].push_back("COSSTA");
		//Create aggregates names
		std::vector<std::vector<std::vector<std::string>>>aggregatenames(3,std::vector<std::vector<std::string>>());
		aggregatenames[2].push_back(std::vector<std::string>());
		aggregatenames[2][0].push_back("COS8");
		aggregatenames[2][0].push_back("COS20");
		/////////////////empty values////////////////////
		std::vector<std::string>attributenames;
		std::vector<std::map<std::string, double>>indexes;
		/////////////////////////////////////////////////
		std::vector<Core::FMTTheme>themes;
		size_t fullsize = 0;
		for (size_t themeid = 0; themeid< attributes.size();++themeid)
		{
			themes.push_back(
				Core::FMTTheme(attributes.at(themeid),
					attributenames,
					aggregates.at(themeid),
					aggregatenames.at(themeid),
					indexes,
					themeid,fullsize,"Nom du theme")
				);
			fullsize += attributes.at(themeid).size();
		}
		///////////////////////////////////////////////////////////
		//Create Area
		std::vector<Core::FMTActualDevelopment>area;
		area.push_back(Core::FMTActualDevelopment(Core::FMTMask("GS1 FC6 COS30", themes),5,0,1200.00));
		area.push_back(Core::FMTActualDevelopment(Core::FMTMask("GS2 FC4 COS20", themes),10,0,1456.00));
		area.push_back(Core::FMTActualDevelopment(Core::FMTMask("GS3 FC6 COS60", themes),20,1,2352.00));
		area.push_back(Core::FMTActualDevelopment(Core::FMTMask("GS1 FC4 COS30", themes),25,0,5454.00));
		///////////////////////////////////////////////////////////
		//Create Yields
		Core::FMTYields yields;
		Core::FMTAgeYieldHandler fc6yield(Core::FMTMask("? FC6 ?",themes));
		Core::FMTAgeYieldHandler fc4yield(Core::FMTMask("? FC4 ?",themes));
		double fc6j = 0;
		double fc4j = 0;
		for (size_t age = 1 ; age < 50; ++age)
			{
			fc6yield.pushBase(age);
			fc4yield.pushBase(age);
			fc6yield.pushData("YVTOT", fc6j);
			fc4yield.pushData("YVTOT", fc4j);
			fc6j += 10;
			fc4j += 5;
			}
		yields.push_back(Core::FMTMask("? FC6 ?",themes), std::unique_ptr<Core::FMTYieldHandler>(new Core::FMTAgeYieldHandler(fc6yield)));
		yields.push_back(Core::FMTMask("? FC4 ?",themes), std::unique_ptr<Core::FMTYieldHandler>(new Core::FMTAgeYieldHandler(fc4yield)));
		yields.update();
		///////////////////////////////////////////////////////////
		//Create Action
		std::vector<Core::FMTAction>actions;
		actions.push_back(Core::FMTAction("ACT", true, true));
		Core::FMTSpec actionspec;
		actionspec.addBounds(Core::FMTAgeBounds(Core::FMTsection::Action,100,3));
		actions[0].push_back(Core::FMTMask("? ? ?",themes), actionspec);
		actions[0].update();
		//////////////////////////////////////////////////////////
		//Create Transition
		std::vector<Core::FMTTransition>transitions;
		transitions.push_back(Core::FMTTransition("ACT"));
		Core::FMTFork fork;
		fork.add(Core::FMTTransitionMask("? ? ?", themes, 100.0));
		transitions[0].push_back(Core::FMTMask("? ? ?", themes),fork);
		transitions[0].update();
		//////////////////////////////////////////////////////////
		//Create lifespan
		Core::FMTLifespans lifespan;
		lifespan.push_back(Core::FMTMask("? ? ?", themes),80);
		lifespan.update();
		//////////////////////////////////////////////////////////
		//Create Output
		std::vector<Core::FMTOutput>outputs;
		std::vector<Core::FMTOutputSource>sources;
		std::vector<Core::FMTOperator>oeprators;
		sources.push_back(Core::FMTOutputSource(Core::FMTSpec(), Core::FMTMask("? ? ?", themes), Core::FMTotar::actual
			, "YVTOT", "ACT", 0, -1));
		outputs.push_back(Core::FMTOutput("OVOLTOTREC","VOLUME RECOLTE","VOLUME",
							sources,
							oeprators));
		//////////////////////////////////////////////////////////
		//Create Constraint
		std::vector<Core::FMTConstraint>constraints;
		//objective
		Core::FMTConstraint objective(Core::FMTconstrainttype::FMTMAXMINobjective,outputs.at(0));
		objective.setLength(1);
		constraints.push_back(objective);
		//even flow
		Core::FMTConstraint evenflow(Core::FMTconstrainttype::FMTevenflow, outputs.at(0));
		evenflow.setLength(1);
		constraints.push_back(evenflow);
		///////////////////////////////////////////////////////////
		//Create Model
		Models::FMTLpModel model(Models::FMTModel(),Models::FMTSolverInterface::MOSEK);
		model.setName("TEST");
		model.setThemes(themes);
		model.setYields(yields);
		model.setArea(area);
		model.setLifespan(lifespan);
		model.setTransitions(transitions);
		model.setActions(actions);
		model.setOutputs(outputs);
		model.setConstraints(constraints);
		model.setParameter(Models::FMTintmodelparameters::LENGTH, 10);
		const std::filesystem::path OUTPUT_FOLDER = "../../tests/CreateModel/";
		std::filesystem::remove_all(OUTPUT_FOLDER);
		std::filesystem::create_directories(OUTPUT_FOLDER);
		Parser::FMTModelParser mparser;

		// Act
		mparser.write(model, OUTPUT_FOLDER.string());
		const bool SOLVED = model.doPlanning(true);
		const Models::FMTModel REREAD = mparser.readproject((OUTPUT_FOLDER / (model.getName() + ".pri")).string(),
			std::vector<std::string>(1, "ROOT")).at(0);
		Models::FMTLpModel rereadModel(REREAD, Models::FMTSolverInterface::MOSEK);
		rereadModel.setParameter(Models::FMTintmodelparameters::LENGTH, 10);
		const bool REREAD_SOLVED = rereadModel.doPlanning(true);

		// Assert
		Testing::Checker checker;
		checker.check(SOLVED && model.isOptimal(), "the model built in code is optimal");
		checker.check(REREAD_SOLVED && rereadModel.isOptimal(), "the model read back is optimal");
		checker.checkNear(rereadModel.getObjValue(), model.getObjValue(), 1e-6 * (std::max)(1.0, std::abs(model.getObjValue())),
			"objective of the model read back");
		return checker.exitCode();
		});
#else
	return Testing::skip("FMT is compiled without OSI");
#endif
}
