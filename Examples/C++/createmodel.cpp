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
#include <memory>
#include <vector>
#include <string>
#endif


int main()
{
#ifdef FMTWITHOSI
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
	Models::FMTLpModel model(Models::FMTModel(),Models::FMTsolverinterface::MOSEK);
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
	Parser::FMTModelParser mparser;
	//Write model
	mparser.write(model, "../../tests/createmodel/");
	//Solve model
	model.doPlanning(true);
	return 0;
#endif
}


