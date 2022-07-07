#include "FMTmodel.hpp"
#include "FMTtheme.hpp"
#include "FMTmask.hpp"
#include "FMTyields.hpp"
#include "FMTaction.hpp"
#include "FMTtransition.hpp"
#include "FMToutput.hpp"
#include "FMTconstraint.hpp"
#include "FMTbounds.hpp"
#include "FMTageyieldhandler.hpp"
#include "FMTactualdevelopment.hpp"
#include "FMTlifespans.hpp"
#include "FMTmodelparser.hpp"
#include "FMTlogger.hpp"
#include "FMTlpmodel.hpp"
#include "FMTtransitionmask.hpp"
#include "FMTfork.hpp"
#include <memory>
#include <vector>
#include <string>


int main()
{
	Logging::FMTlogger().logstamp();

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
	std::vector<Core::FMTtheme>themes;
	size_t fullsize = 0;
	for (size_t themeid = 0; themeid< attributes.size();++themeid)
	{
		themes.push_back(
			Core::FMTtheme(attributes.at(themeid),
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
	std::vector<Core::FMTactualdevelopment>area;
	area.push_back(Core::FMTactualdevelopment(Core::FMTmask("GS1 FC6 COS30", themes),5,0,1200.00));
	area.push_back(Core::FMTactualdevelopment(Core::FMTmask("GS2 FC4 COS20", themes),10,0,1456.00));
	area.push_back(Core::FMTactualdevelopment(Core::FMTmask("GS3 FC6 COS60", themes),20,1,2352.00));
	area.push_back(Core::FMTactualdevelopment(Core::FMTmask("GS1 FC4 COS30", themes),25,0,5454.00));
	///////////////////////////////////////////////////////////
	//Create Yields
	Core::FMTyields yields;
	Core::FMTageyieldhandler fc6yield(Core::FMTmask("? FC6 ?",themes));
	Core::FMTageyieldhandler fc4yield(Core::FMTmask("? FC4 ?",themes));
	double fc6j = 0;
	double fc4j = 0;
	for (size_t age = 1 ; age < 50; ++age)
		{
		fc6yield.push_base(age);
		fc4yield.push_base(age);
		fc6yield.push_data("YVTOT", fc6j);
		fc4yield.push_data("YVTOT", fc4j);
		fc6j += 10;
		fc4j += 5;
		}
	yields.push_back(Core::FMTmask("? FC6 ?",themes), std::unique_ptr<Core::FMTyieldhandler>(new Core::FMTageyieldhandler(fc6yield)));
	yields.push_back(Core::FMTmask("? FC4 ?",themes), std::unique_ptr<Core::FMTyieldhandler>(new Core::FMTageyieldhandler(fc4yield)));
	yields.update();
	///////////////////////////////////////////////////////////
	//Create Action
	std::vector<Core::FMTaction>actions;
	actions.push_back(Core::FMTaction("ACT", true, true));
	Core::FMTspec actionspec;
	actionspec.addbounds(Core::FMTagebounds(Core::FMTsection::Action,100,3));
	actions[0].push_back(Core::FMTmask("? ? ?",themes), actionspec);
	actions[0].update();
	//////////////////////////////////////////////////////////
	//Create Transition
	std::vector<Core::FMTtransition>transitions;
	transitions.push_back(Core::FMTtransition("ACT"));
	Core::FMTfork fork;
	fork.add(Core::FMTtransitionmask("? ? ?", themes, 100.0));
	transitions[0].push_back(Core::FMTmask("? ? ?", themes),fork);
	transitions[0].update();
	//////////////////////////////////////////////////////////
	//Create lifespan
	Core::FMTlifespans lifespan;
	lifespan.push_back(Core::FMTmask("? ? ?", themes),80);
	lifespan.update();
	//////////////////////////////////////////////////////////
	//Create Output
	std::vector<Core::FMToutput>outputs;
	std::vector<Core::FMToutputsource>sources;
	std::vector<Core::FMToperator>oeprators;
	sources.push_back(Core::FMToutputsource(Core::FMTspec(), Core::FMTmask("? ? ?", themes), Core::FMTotar::actual
		, "YVTOT", "ACT", 0, -1));
	outputs.push_back(Core::FMToutput("OVOLTOTREC","VOLUME RECOLTE",
						sources,
						oeprators));
	//////////////////////////////////////////////////////////
	//Create Constraint
	std::vector<Core::FMTconstraint>constraints;
	//objective
	Core::FMTconstraint objective(Core::FMTconstrainttype::FMTMAXMINobjective,outputs.at(0));
	objective.setlength(1);
	constraints.push_back(objective);
	//even flow
	Core::FMTconstraint evenflow(Core::FMTconstrainttype::FMTevenflow, outputs.at(0));
	evenflow.setlength(1);
	constraints.push_back(evenflow);
	///////////////////////////////////////////////////////////
	//Create Model
	Models::FMTlpmodel model(Models::FMTmodel(),Models::FMTsolverinterface::MOSEK);
	model.setname("TEST");
	model.setthemes(themes);
	model.setyields(yields);
	model.setarea(area);
	model.setlifespan(lifespan);
	model.settransitions(transitions);
	model.setactions(actions);
	model.setoutputs(outputs);
	model.setconstraints(constraints);
	model.setparameter(Models::FMTintmodelparameters::LENGTH, 10);
	Parser::FMTmodelparser mparser;
	//Write model
	mparser.write(model, "tests/createmodel/");
	//Solve model
	model.doplanning(true);
	return 0;
}
