/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "FMTmodelparser.h"

namespace WSParser{

FMTmodelparser::FMTmodelparser():FMTparser()
    {

    }

FMTmodelparser::FMTmodelparser(const FMTmodelparser& rhs) : FMTparser(rhs)
    {

    }
FMTmodelparser& FMTmodelparser::operator = (const FMTmodelparser& rhs)
    {
    if (this!=&rhs)
        {
        FMTparser::operator=(rhs);
        }
    return *this;
    }

bool FMTmodelparser::write(const FMTmodel& model,const string& folder)
    {
    FMTlandscapeparser landparser;
    landparser.write(model.getthemes(),folder+model.name+".lan");
	vector<FMTactualdevelopment>devs = model.getarea();
	if (!devs.empty())
		{
		FMTareaparser areaparser;
		areaparser.write(devs, folder + model.name + ".are");
		}
    FMTyieldparser yldparser;
    yldparser.write(model.getyields(),folder+model.name+".yld");
    FMTactionparser actparser;
    actparser.write(model.getactions(),folder+model.name+".act",model.getactionaggregates());
    FMTtransitionparser trnparser;
    trnparser.write(model.gettransitions(),folder+model.name+".trn");
	vector<FMToutput>outputs = model.getoutputs();
	if (!outputs.empty())
		{
		FMToutputparser outparser;
		outparser.write(outputs, folder + model.name + ".out");
		}
	vector<FMTconstraint>constraints = model.getconstraints();
	if (!constraints.empty())
		{
		FMToptimizationparser optparser;
		optparser.write(constraints, folder + model.name + ".opt");
		}

    return true;
    }

FMTmodel FMTmodelparser::read(const string& con,const string& lan,
                      const string& lif,const string& are,const string& yld,
                      const string& act,const string& trn,const string& out,string opt)
    {
	map<string, vector<int>> commons;
	vector<FMTmodel>models;
	
	return referenceread(commons, models,
		con, lan, lif, are, yld, act, trn, out, opt);
    }


FMTmodel FMTmodelparser::referenceread(map<string,vector<int>>& common_sections,
	vector<FMTmodel>& models,
	const string& con, const string& lan,
	const string& lif, const string& are, const string& yld,
	const string& act, const string& tr, const string& out,
	string opt, bool allow_mapping)
	{
	FMTconstants constants;
	vector<FMTtheme>themes;
	vector<FMTactualdevelopment>areas;
	FMTyields yields;
	FMTlifespans lifespan;
	vector<FMTaction>actions;
	vector<FMTtransition>transitions;
	vector<FMToutput>outputs;
	vector<FMTconstraint>constraints;
	map<string, vector<string>>action_aggregates;
	if (allow_mapping)
		{ 
		map<string, vector<int>>::const_iterator constants_it = common_sections.find(con);
		map<string, vector<int>>::const_iterator themes_it = common_sections.find(lan);
		map<string, vector<int>>::const_iterator lifespan_it = common_sections.find(lif);
		map<string, vector<int>>::const_iterator area_it = common_sections.find(are);
		map<string, vector<int>>::const_iterator yield_it = common_sections.find(yld);
		map<string, vector<int>>::const_iterator actions_it = common_sections.find(act);
		map<string, vector<int>>::const_iterator transitions_it = common_sections.find(tr);
		map<string, vector<int>>::const_iterator outputs_it = common_sections.find(out);
		map<string, vector<int>>::const_iterator optimize_it = common_sections.find(opt);
		vector<int>::iterator common_it;
		if (constants_it != common_sections.end() && themes_it != common_sections.end())
			{
			vector<int>common_sets(themes_it->second.size()+ constants_it->second.size());
			common_it = std::set_intersection(themes_it->second.begin(), themes_it->second.end(),
				 constants_it->second.begin(), constants_it->second.end(), common_sets.begin());
			common_sets.resize(common_it - common_sets.begin());
			if (!common_sets.empty())
				{
				themes = models.at(*common_it).getthemes();
				sort(common_sets.begin(), common_sets.end());
				if (area_it != common_sections.end())
					{
					vector<int>common_area(common_sets.size()+ area_it->second.size());
					common_it = std::set_intersection(common_sets.begin(), common_sets.end(),
						area_it->second.begin(), area_it->second.end(), common_area.begin());
					common_area.resize(common_it - common_area.begin());
					if (!common_area.empty())
						{
						areas = models.at(*common_it).getarea();
						}
					}
				if (yield_it != common_sections.end())
					{
					vector<int>common_yield(common_sets.size() + yield_it->second.size());
					common_it = std::set_intersection(common_sets.begin(), common_sets.end(),
						yield_it->second.begin(), yield_it->second.end(), common_yield.begin());
					common_yield.resize(common_it - common_yield.begin());
					if (!common_yield.empty())
						{
						yields = models.at(*common_it).getyields();
						}
					}
				if (lifespan_it != common_sections.end())
					{
					vector<int>common_lif(common_sets.size() + lifespan_it->second.size());
					common_it = std::set_intersection(common_sets.begin(), common_sets.end(),
						lifespan_it->second.begin(), lifespan_it->second.end(), common_lif.begin());
					common_lif.resize(common_it - common_lif.begin());
					if (!common_lif.empty())
						{
						lifespan = models.at(*common_it).getlifespan();
						sort(common_lif.begin(), common_lif.end());
						if (actions_it != common_sections.end())
							{
							vector<int>common_actions(common_lif.size() + actions_it->second.size());
							common_it = std::set_intersection(common_lif.begin(), common_lif.end(),
								actions_it->second.begin(), actions_it->second.end(), common_actions.begin());
							common_actions.resize(common_it - common_actions.begin());
							if (!common_actions.empty())
								{
								actions = models.at(*common_it).getactions();
								action_aggregates = models.at(*common_it).getactionaggregates();
								sort(common_actions.begin(), common_actions.end());
								if (transitions_it != common_sections.end())
									{
									vector<int>common_transitions(common_actions.size() + transitions_it->second.size());
									common_it = std::set_intersection(common_actions.begin(), common_actions.end(),
										transitions_it->second.begin(), transitions_it->second.end(), common_transitions.begin());
									common_transitions.resize(common_it - common_transitions.begin());
									if (!common_transitions.empty())
										{
										transitions = models.at(*common_it).gettransitions();
										sort(common_transitions.begin(), common_transitions.end());
										}
									}
								}
							}
						}
					}

				if (outputs_it != common_sections.end())//should be with action??
					{
					vector<int>common_output(common_sets.size()+ outputs_it->second.size());
					common_it = std::set_intersection(common_sets.begin(), common_sets.end(),
						outputs_it->second.begin(), outputs_it->second.end(), common_output.begin());
					common_output.resize(common_it - common_output.begin());
					if (!common_output.empty())
						{
						outputs = models.at(*common_it).getoutputs();
						sort(common_output.begin(), common_output.end());
						if (optimize_it != common_sections.end())
							{
							vector<int>common_optimize(common_output.size() + optimize_it->second.size());
							common_it = std::set_intersection(common_output.begin(), common_output.end(),
								optimize_it->second.begin(), optimize_it->second.end(), common_optimize.begin());
							common_optimize.resize(common_it - common_optimize.begin());
							if (!common_optimize.empty())
								{
								constraints = models.at(*common_it).getconstraints();
								}
							}
						}
					}
				}
			}

		int model_location = int(models.size());
		if (constants_it == common_sections.end())
			{
			common_sections[con] = vector<int>();
			}
		common_sections[con].push_back(model_location);
		if (themes_it == common_sections.end())
			{
			common_sections[lan] = vector<int>();
			}
		common_sections[lan].push_back(model_location);
		if (area_it == common_sections.end())
			{
				common_sections[are] = vector<int>();
			}
		common_sections[are].push_back(model_location);
		if (lifespan_it == common_sections.end())
			{
				common_sections[lif] = vector<int>();
			}
		common_sections[lif].push_back(model_location);
		if (yield_it == common_sections.end())
			{
			common_sections[yld] = vector<int>();
			}
		common_sections[yld].push_back(model_location);
		if (actions_it == common_sections.end())
			{
			common_sections[act] = vector<int>();
			}
		common_sections[act].push_back(model_location);
		if (transitions_it == common_sections.end())
			{
			common_sections[tr] = vector<int>();
			}
		common_sections[tr].push_back(model_location);
		if (outputs_it == common_sections.end())
			{
			common_sections[out] = vector<int>();
			}
		common_sections[out].push_back(model_location);
		if (optimize_it == common_sections.end())
			{
			common_sections[opt] = vector<int>();
			}
		common_sections[opt].push_back(model_location);
		}

		boost::filesystem::path landfile(lan);
		string modelname = landfile.stem().string();

		FMTconstantparser cparser;
		cparser.passinexceptionhandler(_exhandler);
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << con << "\n";
		constants = cparser.read(con);
		//cparser.write(constants, "C:/Users/cyrgu3/source/repos/FMT/x64/Release/debug_pc/test.con");
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " LANDSCAPE " << "\n";
		if (themes.empty())
			{
			FMTlandscapeparser landparser;
			landparser.passinexceptionhandler(_exhandler);
			themes = landparser.read(constants, lan);
			}
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " AREA " << "\n";
		if (areas.empty())
			{
			FMTareaparser areaparser;
			areaparser.passinexceptionhandler(_exhandler);
			areas = areaparser.read(themes, constants, are);
			}
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " LIFESPAN " << "\n";
		if (lifespan.empty())
			{
			FMTlifespanparser lifespanparser;
			lifespanparser.passinexceptionhandler(_exhandler);
			lifespan = lifespanparser.read(themes, constants, lif);
			}
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " YIELDS " << "\n";
		if (yields.empty())
			{
			FMTyieldparser yldparser;
			yldparser.passinexceptionhandler(_exhandler);
			yields = yldparser.read(themes, constants, yld);
			}
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " ACTIONS " << "\n";
		if (actions.empty())
			{
			FMTactionparser actparser;
			actparser.passinexceptionhandler(_exhandler);
			actions = actparser.read(themes, yields, constants, act, action_aggregates);
			}
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "TRANSITIONS " << "\n";
		if (transitions.empty())
			{
			
			FMTtransitionparser trnparser;
			trnparser.passinexceptionhandler(_exhandler);
			transitions = trnparser.read(themes, actions, yields, constants, tr);
			}
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "OUTPUTS " << "\n";
		if (outputs.empty())
			{
			FMToutputparser outparser;
			outparser.passinexceptionhandler(_exhandler);
			outputs = outparser.read(themes, actions, yields, constants, action_aggregates, out);
			}

		//settransitionsNactions(actions, transitions);
		FMTmodel returnedmodel;
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "OPTIMIZE " << "\n";
		
		if (!opt.empty() && constraints.empty())
		{
			
			
			FMToptimizationparser optzparser;
			vector<FMTaction>excluded(actions); //should we realy use that crap? excluded is actualy the same actions but with more period specification...
			optzparser.passinexceptionhandler(_exhandler);
			
			constraints = optzparser.read(themes, actions, constants, action_aggregates, outputs, excluded, opt);
			//actions = excluded; //here we go let the user create a clusterfuck
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "OPTIMIZE " << constraints.size() << "\n";
			returnedmodel =  FMTmodel(areas, themes, actions, action_aggregates,
				transitions, yields, lifespan, modelname, outputs, constraints);
		}else {
			returnedmodel = FMTmodel(areas, themes, actions, action_aggregates,
				transitions, yields, lifespan, modelname, outputs);
			}
		if (allow_mapping)
			{
			models.push_back(returnedmodel);
			}
		returnedmodel.cleanactionsntransitions();
		return returnedmodel;
		}


vector<FMTmodel>FMTmodelparser::readproject(const string& primary_location,
												vector<string>scenarios, 
											bool readarea, bool readoutputs, bool readoptimize)
	{
	vector<FMTmodel>models;
	map<string, vector<int>>commons;
	map<FMTwssect, string>bases = getprimary(primary_location);
	if (!readarea)
		{
		bases.at(FMTwssect::Area) = "";
		}
	if (!readoutputs)
		{
		bases.at(FMTwssect::Outputs) = "";
		bases.at(FMTwssect::Optimize) = "";
		}
	if (!readoptimize)
		{
		bases.at(FMTwssect::Optimize) = "";
		}
	bool tookroot = (std::find(scenarios.begin(), scenarios.end(), "ROOT") != scenarios.end());
	if (tookroot ||scenarios.empty()) //load the modelroot!
		{
		FMTmodel scenario = referenceread(commons,
			models,
			bases.at(FMTwssect::Constants),
			bases.at(FMTwssect::Landscape),
			bases.at(FMTwssect::Lifespan),
			bases.at(FMTwssect::Area),
			bases.at(FMTwssect::Yield),
			bases.at(FMTwssect::Action),
			bases.at(FMTwssect::Transition),
			bases.at(FMTwssect::Outputs),
			bases.at(FMTwssect::Optimize),true);
		models.back().name = "ROOT";
		//models.push_back(scenario);
		}
	boost::filesystem::path primary_path(primary_location);
	string main_name = primary_path.stem().string();
	boost::filesystem::path scenarios_path = (primary_path.parent_path() / boost::filesystem::path("Scenarios"));
	if (boost::filesystem::is_directory(scenarios_path))
		{
		boost::filesystem::directory_iterator end_itr;
		string model_name;
		for (boost::filesystem::directory_iterator itr(scenarios_path); itr != end_itr; ++itr)
			{
			if (boost::filesystem::is_directory(itr->path()))
				{
				model_name = itr->path().stem().string();
				if (scenarios.empty()||std::find(scenarios.begin(), scenarios.end(), model_name) != scenarios.end())
					{
						map<FMTwssect, string>scenario_files = bases;
						boost::filesystem::directory_iterator end_fileitr;
						for (boost::filesystem::directory_iterator fileitr(itr->path()); fileitr != end_fileitr; ++fileitr)
						{
							if (boost::filesystem::is_regular_file(fileitr->path()))
							{
								string extension = boost::filesystem::extension(fileitr->path().string());
								FMTwssect section = from_extension(extension);
								string file_name = fileitr->path().stem().string();
								if (section != FMTwssect::Empty && file_name == main_name)
								{
									scenario_files[section] = fileitr->path().string();
								}
							}
						}
						//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << model_name << "\n";
						if (!readarea)
							{
							scenario_files.at(FMTwssect::Area) = "";
							}
						if (!readoutputs)
							{
							scenario_files.at(FMTwssect::Outputs) = "";
							scenario_files.at(FMTwssect::Optimize) = "";
							}
						if (!readoptimize)
							{
							scenario_files.at(FMTwssect::Optimize) = "";
							}
						FMTmodel scenario = referenceread(commons,
							models,
							scenario_files.at(FMTwssect::Constants),
							scenario_files.at(FMTwssect::Landscape),
							scenario_files.at(FMTwssect::Lifespan),
							scenario_files.at(FMTwssect::Area),
							scenario_files.at(FMTwssect::Yield),
							scenario_files.at(FMTwssect::Action),
							scenario_files.at(FMTwssect::Transition),
							scenario_files.at(FMTwssect::Outputs),
							scenario_files.at(FMTwssect::Optimize),true);
						models.back().name = model_name;
						//models.push_back(scenario);
					}
				}
			}
		}
	for (FMTmodel& model : models)//clean referenced models!!!
		{
		model.cleanactionsntransitions();
		}
	return models;
	}

vector<vector<FMTschedule>>FMTmodelparser::readschedules(const string& primary_location,
	const vector<FMTmodel>& models)
	{
	vector<vector<FMTschedule>>schedules(models.size());
	boost::filesystem::path primary_path(primary_location);
	string file_name = primary_path.stem().string();
	FMTscheduleparser scheduleparser;
	scheduleparser.passinexceptionhandler(_exhandler);
	vector<FMTmodel>::const_iterator model_it = std::find_if(models.begin(), models.end(), FMTmodelcomparator("ROOT"));
	if (model_it != models.end())
		{
		size_t location = std::distance<vector<FMTmodel>::const_iterator>(models.begin(), model_it);
		boost::filesystem::path root_solution = (primary_path.parent_path() / boost::filesystem::path(file_name + ".seq"));
		vector<FMTaction>actions = model_it->getactions();
		vector<FMTtheme>themes = model_it->getthemes();
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "Schedule read in "<< root_solution.string() << "\n";
		schedules[location] = scheduleparser.read(themes, actions, root_solution.string());
		}
	boost::filesystem::path scenarios_path = (primary_path.parent_path() / boost::filesystem::path("Scenarios"));
	if (boost::filesystem::is_directory(scenarios_path))
		{
		boost::filesystem::directory_iterator end_itr;
		string model_name;
		for (boost::filesystem::directory_iterator itr(scenarios_path); itr != end_itr; ++itr)
			{
				if (boost::filesystem::is_directory(itr->path()))
					{
					model_name = itr->path().stem().string();
					model_it = std::find_if(models.begin(), models.end(), FMTmodelcomparator(model_name));
					if (model_it != models.end())
						{
						boost::filesystem::path solutionpath = (itr->path() / boost::filesystem::path(file_name + "._seq"));
						if (boost::filesystem::is_regular_file(solutionpath))
							{
							size_t location = std::distance<vector<FMTmodel>::const_iterator>(models.begin(), model_it);
							vector<FMTaction>actions = model_it->getactions();
							vector<FMTtheme>themes = model_it->getthemes();
							//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "Schedule read in " << solutionpath.string() << "\n";
							schedules[location] = scheduleparser.read(themes, actions, solutionpath.string());
							}
						}
					}
			}
		}
	return schedules;
	}
}
