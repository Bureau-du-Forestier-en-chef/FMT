/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTmodelparser.h"
#include <map>

namespace Parser{

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

void FMTmodelparser::write(const Models::FMTmodel& model,const std::string& folder) const
    {
	try {
		FMTlandscapeparser landparser;
		landparser.write(model.getthemes(), folder + model.getname() + ".lan");
		const std::vector<Core::FMTactualdevelopment>devs = model.getarea();
		if (!devs.empty())
		{
			FMTareaparser areaparser;
			areaparser.write(devs, folder + model.getname() + ".are");
		}
		FMTyieldparser yldparser;
		yldparser.write(model.getyields(), folder + model.getname() + ".yld");
		FMTactionparser actparser;
		actparser.write(model.getactions(), folder + model.getname() + ".act");
		FMTtransitionparser trnparser;
		trnparser.write(model.gettransitions(), folder + model.getname() + ".trn");
		const std::vector<Core::FMToutput>outputs = model.getoutputs();
		if (!outputs.empty())
		{
			FMToutputparser outparser;
			outparser.write(outputs, folder + model.getname() + ".out");
		}
		const std::vector<Core::FMTconstraint>constraints = model.getconstraints();
		if (!constraints.empty())
		{
			FMToptimizationparser optparser;
			optparser.write(constraints, folder + model.getname() + ".opt");
		}
	}catch (...)
	{
		_exhandler->printexceptions(" at " + folder, "FMTmodelparser::write", __LINE__, __FILE__, _section);
	}

	
    }

Models::FMTmodel FMTmodelparser::read(const std::string& con,const std::string& lan,
                      const std::string& lif,const std::string& are,const std::string& yld,
                      const std::string& act,const std::string& trn,const std::string& out, std::string opt)
    {
	std::map<std::string, std::vector<int>> commons;
	std::vector<Models::FMTmodel>models;
	return referenceread(commons, models,
		con, lan, lif, are, yld, act, trn, out, opt);
    }


Models::FMTmodel FMTmodelparser::referenceread(std::map<std::string, std::vector<int>>& common_sections,
	std::vector<Models::FMTmodel>& models,
	const std::string& con, const std::string& lan,
	const std::string& lif, const std::string& are, const std::string& yld,
	const std::string& act, const std::string& tr, const std::string& out,
	std::string opt, bool allow_mapping)
	{
	Models::FMTmodel returnedmodel;
	try {
		Core::FMTconstants constants;
		std::vector<Core::FMTtheme>themes;
		std::vector<Core::FMTactualdevelopment>areas;
		Core::FMTyields yields;
		Core::FMTlifespans lifespan;
		std::vector<Core::FMTaction>actions;
		std::vector<Core::FMTtransition>transitions;
		std::vector<Core::FMToutput>outputs;
		std::vector<Core::FMTconstraint>constraints;
		if (allow_mapping)
			{ 
			std::map<std::string, std::vector<int>>::const_iterator constants_it = common_sections.find(con);
			std::map<std::string, std::vector<int>>::const_iterator themes_it = common_sections.find(lan);
			std::map<std::string, std::vector<int>>::const_iterator lifespan_it = common_sections.find(lif);
			std::map<std::string, std::vector<int>>::const_iterator area_it = common_sections.find(are);
			std::map<std::string, std::vector<int>>::const_iterator yield_it = common_sections.find(yld);
			std::map<std::string, std::vector<int>>::const_iterator actions_it = common_sections.find(act);
			std::map<std::string, std::vector<int>>::const_iterator transitions_it = common_sections.find(tr);
			std::map<std::string, std::vector<int>>::const_iterator outputs_it = common_sections.find(out);
			std::map<std::string, std::vector<int>>::const_iterator optimize_it = common_sections.find(opt);
			std::vector<int>::iterator common_it;
			if (constants_it != common_sections.end() && themes_it != common_sections.end())
				{
				std::vector<int>common_sets(themes_it->second.size()+ constants_it->second.size());
				common_it = std::set_intersection(themes_it->second.begin(), themes_it->second.end(),
					 constants_it->second.begin(), constants_it->second.end(), common_sets.begin());
				common_sets.resize(common_it - common_sets.begin());
				if (!common_sets.empty())
					{
					themes = models.at(*common_it).getthemes();
					std::sort(common_sets.begin(), common_sets.end());
					if (area_it != common_sections.end())
						{
						std::vector<int>common_area(common_sets.size()+ area_it->second.size());
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
						std::vector<int>common_yield(common_sets.size() + yield_it->second.size());
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
						std::vector<int>common_lif(common_sets.size() + lifespan_it->second.size());
						common_it = std::set_intersection(common_sets.begin(), common_sets.end(),
							lifespan_it->second.begin(), lifespan_it->second.end(), common_lif.begin());
						common_lif.resize(common_it - common_lif.begin());
						if (!common_lif.empty())
							{
							lifespan = models.at(*common_it).getlifespan();
							std::sort(common_lif.begin(), common_lif.end());
							if (actions_it != common_sections.end())
								{
								std::vector<int>common_actions(common_lif.size() + actions_it->second.size());
								common_it = std::set_intersection(common_lif.begin(), common_lif.end(),
									actions_it->second.begin(), actions_it->second.end(), common_actions.begin());
								common_actions.resize(common_it - common_actions.begin());
								if (!common_actions.empty())
									{
									actions = models.at(*common_it).getactions();
									std::sort(common_actions.begin(), common_actions.end());
									if (transitions_it != common_sections.end())
										{
										std::vector<int>common_transitions(common_actions.size() + transitions_it->second.size());
										common_it = std::set_intersection(common_actions.begin(), common_actions.end(),
											transitions_it->second.begin(), transitions_it->second.end(), common_transitions.begin());
										common_transitions.resize(common_it - common_transitions.begin());
										if (!common_transitions.empty())
											{
											transitions = models.at(*common_it).gettransitions();
											std::sort(common_transitions.begin(), common_transitions.end());
											}
										}
									}
								}
							}
						}

					if (outputs_it != common_sections.end())//should be with action??
						{
						std::vector<int>common_output(common_sets.size()+ outputs_it->second.size());
						common_it = std::set_intersection(common_sets.begin(), common_sets.end(),
							outputs_it->second.begin(), outputs_it->second.end(), common_output.begin());
						common_output.resize(common_it - common_output.begin());
						if (!common_output.empty())
							{
							outputs = models.at(*common_it).getoutputs();
							std::sort(common_output.begin(), common_output.end());
							if (optimize_it != common_sections.end())
								{
								std::vector<int>common_optimize(common_output.size() + optimize_it->second.size());
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

			const int model_location = static_cast<int>(models.size());
			if (constants_it == common_sections.end())
				{
				common_sections[con] = std::vector<int>();
				}
			common_sections[con].push_back(model_location);
			if (themes_it == common_sections.end())
				{
				common_sections[lan] = std::vector<int>();
				}
			common_sections[lan].push_back(model_location);
			if (area_it == common_sections.end())
				{
					common_sections[are] = std::vector<int>();
				}
			common_sections[are].push_back(model_location);
			if (lifespan_it == common_sections.end())
				{
					common_sections[lif] = std::vector<int>();
				}
			common_sections[lif].push_back(model_location);
			if (yield_it == common_sections.end())
				{
				common_sections[yld] = std::vector<int>();
				}
			common_sections[yld].push_back(model_location);
			if (actions_it == common_sections.end())
				{
				common_sections[act] = std::vector<int>();
				}
			common_sections[act].push_back(model_location);
			if (transitions_it == common_sections.end())
				{
				common_sections[tr] = std::vector<int>();
				}
			common_sections[tr].push_back(model_location);
			if (outputs_it == common_sections.end())
				{
				common_sections[out] = std::vector<int>();
				}
			common_sections[out].push_back(model_location);
			if (optimize_it == common_sections.end())
				{
				common_sections[opt] = std::vector<int>();
				}
			common_sections[opt].push_back(model_location);
			}
			const boost::filesystem::path landfile(lan);
			const std::string modelname = landfile.stem().string();
		
				FMTconstantparser cparser;
				cparser.passinobject(*this);
				constants = cparser.read(con);
				if (themes.empty())
				{
					FMTlandscapeparser landparser;
					landparser.passinobject(*this);
					themes = landparser.read(constants, lan);
				}
				if (areas.empty())
				{
					FMTareaparser areaparser;
					areaparser.passinobject(*this);
					areas = areaparser.read(themes, constants, are);
				}
				if (lifespan.empty())
				{
					FMTlifespanparser lifespanparser;
					lifespanparser.passinobject(*this);
					lifespan = lifespanparser.read(themes, constants, lif);
				}
				if (yields.empty())
				{
					FMTyieldparser yldparser;
					yldparser.passinobject(*this);
					yields = yldparser.read(themes, constants, yld);
				}
				if (actions.empty())
				{
					FMTactionparser actparser;
					actparser.passinobject(*this);
					actions = actparser.read(themes, yields, constants, act);
					if (find_if(actions.begin(), actions.end(), Core::FMTactioncomparator("_DEATH")) == actions.end())
					{
						_exhandler->raise(Exception::FMTexc::FMTundefineddeathaction,
							"_DEATH","FMTmodelparser::referenceread", __LINE__, __FILE__, Core::FMTsection::Action);
						actions.push_back(Models::FMTmodel::defaultdeathaction(lifespan, themes));
					}
				}
				if (transitions.empty())
				{
					FMTtransitionparser trnparser;
					trnparser.passinobject(*this);
					transitions = trnparser.read(themes, actions, yields, constants, tr);
					if (find_if(transitions.begin(), transitions.end(), Core::FMTtransitioncomparator("_DEATH")) == transitions.end())
					{
						_exhandler->raise(Exception::FMTexc::FMTundefineddeathtransition,
							"_DEATH","FMTmodelparser::referenceread", __LINE__, __FILE__, Core::FMTsection::Transition);
						transitions.push_back(Models::FMTmodel::defaultdeathtransition(lifespan, themes));
					}
				}
				if (outputs.empty())
				{
					FMToutputparser outparser;
					outparser.passinobject(*this);
					outputs = outparser.read(themes, actions, yields, constants, out);
				}
				if (!opt.empty() && constraints.empty())
				{

					FMToptimizationparser optzparser;
					std::vector<Core::FMTaction>excluded(actions); //should we realy use? excluded is actualy the same actions but with more period specification...
					optzparser.passinobject(*this);
					constraints = optzparser.read(themes, actions, constants, outputs, excluded, opt);
					actions = excluded; //here we go
					returnedmodel = Models::FMTmodel(areas, themes, actions,
						transitions, yields, lifespan, modelname, outputs, constraints);
				}
				else {
					returnedmodel = Models::FMTmodel(areas, themes, actions,
						transitions, yields, lifespan, modelname, outputs);
				}
			returnedmodel.passinobject(*this);
			returnedmodel.cleanactionsntransitions();
			if (allow_mapping)
				{
				models.push_back(returnedmodel);
				}
		}catch (...)
		{
			_exhandler->raisefromcatch("","FMTmodelparser::referenceread", __LINE__, __FILE__);
		}
		return returnedmodel;
		}


std::vector<Models::FMTmodel>FMTmodelparser::readproject(const std::string& primary_location,
											std::vector<std::string>scenarios,
											bool readarea, bool readoutputs, bool readoptimize)
	{
	std::vector<Models::FMTmodel>sortedmodels;
	try {
		std::vector<Models::FMTmodel>models;
		std::map<std::string, std::vector<int>>commons;
		std::map<Core::FMTsection, std::string>bases = getprimary(primary_location);
		if (!readarea)
		{
			bases.at(Core::FMTsection::Area) = "";
		}
		if (!readoutputs)
		{
			bases.at(Core::FMTsection::Outputs) = "";
			bases.at(Core::FMTsection::Optimize) = "";
		}
		if (!readoptimize)
		{
			bases.at(Core::FMTsection::Optimize) = "";
		}
		bool tookroot = (std::find(scenarios.begin(), scenarios.end(), "ROOT") != scenarios.end());
		if (tookroot || scenarios.empty()) //load the modelroot!
		{
			Models::FMTmodel scenario = referenceread(commons,
				models,
				bases.at(Core::FMTsection::Constants),
				bases.at(Core::FMTsection::Landscape),
				bases.at(Core::FMTsection::Lifespan),
				bases.at(Core::FMTsection::Area),
				bases.at(Core::FMTsection::Yield),
				bases.at(Core::FMTsection::Action),
				bases.at(Core::FMTsection::Transition),
				bases.at(Core::FMTsection::Outputs),
				bases.at(Core::FMTsection::Optimize), true);
			models.back().setname("ROOT");
		}
		const boost::filesystem::path primary_path(primary_location);
		std::string main_name = primary_path.stem().string();
		boost::to_lower(main_name);
		const boost::filesystem::path scenarios_path = (primary_path.parent_path() / boost::filesystem::path("Scenarios"));
		if (boost::filesystem::is_directory(scenarios_path))
		{
			boost::filesystem::directory_iterator end_itr;
			std::string model_name;
			for (boost::filesystem::directory_iterator itr(scenarios_path); itr != end_itr; ++itr)
			{
				if (boost::filesystem::is_directory(itr->path()))
				{
					model_name = itr->path().stem().string();
					if (scenarios.empty() || std::find(scenarios.begin(), scenarios.end(), model_name) != scenarios.end())
					{
						std::map<Core::FMTsection, std::string>scenario_files = bases;
						boost::filesystem::directory_iterator end_fileitr;
						for (boost::filesystem::directory_iterator fileitr(itr->path()); fileitr != end_fileitr; ++fileitr)
						{
							if (boost::filesystem::is_regular_file(fileitr->path()))
							{
								const std::string extension = boost::filesystem::extension(fileitr->path().string());
								Core::FMTsection section = from_extension(extension);
								std::string file_name = fileitr->path().stem().string();
								boost::to_lower(file_name);
								if (section != Core::FMTsection::Empty && file_name == main_name)
								{
									scenario_files[section] = fileitr->path().string();

								}
							}
						}

						if (!readarea)
						{
							scenario_files.at(Core::FMTsection::Area) = "";
						}
						if (!readoutputs)
						{
							scenario_files.at(Core::FMTsection::Outputs) = "";
							scenario_files.at(Core::FMTsection::Optimize) = "";
						}
						if (!readoptimize)
						{
							scenario_files.at(Core::FMTsection::Optimize) = "";
						}
						Models::FMTmodel scenario = referenceread(commons,
							models,
							scenario_files.at(Core::FMTsection::Constants),
							scenario_files.at(Core::FMTsection::Landscape),
							scenario_files.at(Core::FMTsection::Lifespan),
							scenario_files.at(Core::FMTsection::Area),
							scenario_files.at(Core::FMTsection::Yield),
							scenario_files.at(Core::FMTsection::Action),
							scenario_files.at(Core::FMTsection::Transition),
							scenario_files.at(Core::FMTsection::Outputs),
							scenario_files.at(Core::FMTsection::Optimize), true);
						models.back().setname(model_name);
					}
				}
			}
		}

		if (scenarios.empty())
		{

			models.begin()->cleanactionsntransitions();
			sortedmodels.push_back(*models.begin());
		}
		else {
			for (const std::string& scenario : scenarios)
			{
				std::vector<Models::FMTmodel>::iterator modelit = std::find_if(models.begin(), models.end(), Models::FMTmodelcomparator(scenario));
				if (modelit != models.end())
				{
					modelit->cleanactionsntransitions();
					sortedmodels.push_back(*modelit);
				}
			}
		}
	}catch (...)
		{
		_exhandler->printexceptions("at " + primary_location, "FMTmodelparser::readproject", __LINE__, __FILE__);
		}
	return sortedmodels;
	}

std::vector<std::vector<Core::FMTschedule>>FMTmodelparser::readschedules(const std::string& primary_location,
	const std::vector<Models::FMTmodel>& models)
	{
	std::vector<std::vector<Core::FMTschedule>>schedules(models.size());
	try {
		const boost::filesystem::path primary_path(primary_location);
		const std::map<Core::FMTsection, std::string>bases = getprimary(primary_location);
		FMTscheduleparser scheduleparser;
		scheduleparser.passinexceptionhandler(_exhandler);
		std::vector<Models::FMTmodel>::const_iterator model_it = std::find_if(models.begin(), models.end(), Models::FMTmodelcomparator("ROOT"));
		if (model_it != models.end())
			{
			const size_t location = std::distance<std::vector<Models::FMTmodel>::const_iterator>(models.begin(), model_it);
			const boost::filesystem::path root_solution(bases.at(Core::FMTsection::Schedule));
			const std::vector<Core::FMTaction>actions = model_it->getactions();
			const std::vector<Core::FMTtheme>themes = model_it->getthemes();
			schedules[location] = scheduleparser.read(themes, actions, root_solution.string());
			}
		const boost::filesystem::path scenarios_path = (primary_path.parent_path() / boost::filesystem::path("Scenarios"));
		if (boost::filesystem::is_directory(scenarios_path))
			{
			std::string name = boost::filesystem::path(bases.at(Core::FMTsection::Schedule)).filename().string();
			boost::replace_all(name, ".seq", "._seq");
			boost::replace_all(name, ".SEQ", "._SEQ");
			const boost::filesystem::path file_name(name);
			boost::filesystem::directory_iterator end_itr;
			std::string model_name;
			for (boost::filesystem::directory_iterator itr(scenarios_path); itr != end_itr; ++itr)
				{
					if (boost::filesystem::is_directory(itr->path()))
						{
						model_name = itr->path().stem().string();
						model_it = std::find_if(models.begin(), models.end(), Models::FMTmodelcomparator(model_name));
						if (model_it != models.end())
							{
							boost::filesystem::path solutionpath = (itr->path() / file_name);
							if (boost::filesystem::is_regular_file(solutionpath))
								{
								const size_t location = std::distance<std::vector<Models::FMTmodel>::const_iterator>(models.begin(), model_it);
								const std::vector<Core::FMTaction>actions = model_it->getactions();
								const std::vector<Core::FMTtheme>themes = model_it->getthemes();
								schedules[location] = scheduleparser.read(themes, actions, solutionpath.string());
								}
							}
						}
				}
			}
		}catch (...)
			{
				_exhandler->printexceptions("at " + primary_location, "FMTmodelparser::readschedules", __LINE__, __FILE__);
			}

	return schedules;
	}
}
