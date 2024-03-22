/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTyieldmodeldecisiontree.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include "FMTyieldrequest.hpp"
#include "FMTexceptionhandler.hpp"
#include "FMToutput.hpp"
#include "FMTconstraint.hpp"
#include "FMTgraphvertextoyield.hpp"
#include "FMTquietlogger.hpp"
#include "FMTmodel.hpp"



namespace Core {


	Core::FMTmask FMTyieldmodeldecisiontree::GetMask() const
	{
		Core::FMTmask mask;
		try {
			if (reference && !reference->getsources().empty())
			{
				mask = reference->getsources().begin()->getmask();

			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodeldecisiontree::GetMask", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return mask;
	}




	std::vector<std::vector<double>>FMTyieldmodeldecisiontree::getperiodicvalues() const
	{
		std::vector<std::vector<double>>returned;
		try {
			if (!values.empty())
			{
				for (std::map<size_t, std::vector<double>>::const_iterator valuesit = values.begin(); valuesit != values.end(); ++valuesit)
				{
					returned.push_back(valuesit->second);
				}
			}

		}catch (...)
		{
			_exhandler->raisefromcatch(std::string(GetMask()), "FMTyieldmodeldecisiontree::getperiodicvalues", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return returned;
	}



	Core::FMTconstraint FMTyieldmodeldecisiontree::BuildConstraint(const std::string& name, const std::string& yld, const Core::FMTmask& mask, const double& lowerbound, const double& upperbound, const int& lag) const
	{
		Core::FMTconstraint constraint;
		try {
			std::vector<Core::FMToutputsource>sources;
			if (!mask.empty())
			{
				FMToutputsource source(Core::FMTspec(), mask,
					Core::FMTotar::inventory, yld);
				sources.push_back(source);
			}
			constraint = Core::FMTconstraint(Core::FMTconstrainttype::FMTstandard, Core::FMToutput(name, "", "Decision_tree", sources, std::vector<Core::FMToperator>()));
			constraint.setrhs(lowerbound, upperbound);
			constraint.setlength(1);
			const double dbl_lag = static_cast<double>(lag);
			constraint.addbounds(FMTyldbounds(FMTsection::Optimize, "LAG", dbl_lag, dbl_lag));
		}
		catch (...)
		{
			_exhandler->raisefromcatch(std::string(GetMask()), "FMTyieldmodeldecisiontree::BuildConstraint", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return constraint;
	}

	std::string FMTyieldmodeldecisiontree::GetModelType()
	{
		return "DECISION_TREE";
	}


	FMTyieldmodeldecisiontree::FMTyieldmodeldecisiontree(const boost::property_tree::ptree& jsonProps,const std::vector<std::string>& inputYields, const Core::FMTmask& mainmask):
		FMTyieldmodel(),
		mtx(),
		values(),
		reference(),
		nodes(),
		default_values()

	{
		try {
			boost::property_tree::ptree::const_assoc_iterator modelNameIt = jsonProps.find(JSON_PROP_MODEL_NAME);
			modelName = modelNameIt->second.data();
			modelYields = inputYields;
			std::vector<std::pair<std::string,std::string>>targets;
			std::map<std::string, size_t>locations;
			for (const boost::property_tree::ptree::value_type& node : jsonProps.get_child(JSON_PROP_TREE))
			{
				const std::string node_name = node.first;
				const size_t yieldid = node.second.get<size_t>("Yield");
				if (yieldid>=inputYields.size())
					{
					const std::string invalidyieldid = "No yield id " + std::to_string(yieldid) + " provided for FMTyieldmodeldecisiontree";
					_exhandler->raise(Exception::FMTexc::FMTrangeerror, invalidyieldid, "FMTyieldmodeldecisiontree::FMTyieldmodeldecisiontree",__LINE__, __FILE__, Core::FMTsection::Yield);
					}
				const std::string yield_name = inputYields.at(node.second.get<size_t>("Yield"));
				const int time_lag = node.second.get<int>("Time");
				std::vector<double>bounds;
				for (const boost::property_tree::ptree::value_type& bound : node.second.get_child("Bounds"))
					{
					bounds.push_back(bound.second.get_value<double>());
					}
				std::vector<std::string>branches;
				for (const boost::property_tree::ptree::value_type& branch : node.second.get_child("Branches"))
				{
					branches.push_back(branch.second.get_value<std::string>());
				}
				locations[node_name] = nodes.size();
				nodes.push_back(BuildConstraint(node_name,yield_name,mainmask,*bounds.begin(),bounds.back(),time_lag));
				targets.push_back(std::pair<std::string, std::string>(*branches.begin(), branches.back()));
				
			}
			for (const boost::property_tree::ptree::value_type& decision : jsonProps.get_child(JSON_PROP_DECISIONS))
				{
				const std::string decision_name = decision.first;
				const double value = decision.second.get<double>("Value");
				const double yieldid = decision.second.get<double>("Yield");
				const bool for_rest_of_period = decision.second.get<bool>("Rest");
				double lag_value = 0;
				if (for_rest_of_period)
					{
					lag_value = 1.0;
					}
				locations[decision_name] = nodes.size();
				nodes.push_back(BuildConstraint(decision_name, "", Core::FMTmask(), value, yieldid, lag_value));
				}
			size_t constraint_id = 0;
			for (Core::FMTconstraint& constraint : nodes)
			{
				if (constraint_id< targets.size())
				{
					const double positive = static_cast<double>(locations.at(targets.at(constraint_id).first));
					const double negative = static_cast<double>(locations.at(targets.at(constraint_id).second));
					constraint.addbounds(Core::FMTyldbounds(Core::FMTsection::Yield, "Decisions", positive, negative));
				}
				
				++constraint_id;
			}

			for (const boost::property_tree::ptree::value_type& defaultval : jsonProps.get_child(JSON_PROP_DEFAULT))
			{
				default_values.push_back(defaultval.second.get_value<double>());
			}


			const size_t yieldid = jsonProps.get<size_t>(JSON_PROP_REFERENCE);
			if (yieldid >= inputYields.size())
				{
				const std::string invalidyieldid = "No yield id " + std::to_string(yieldid) + " provided for FMTyieldmodeldecisiontree";
				_exhandler->raise(Exception::FMTexc::FMTrangeerror, invalidyieldid, "FMTyieldmodeldecisiontree::FMTyieldmodeldecisiontree", __LINE__, __FILE__, Core::FMTsection::Yield);
				}
			reference = std::unique_ptr<Core::FMToutput>( new Core::FMToutput(BuildConstraint(JSON_PROP_REFERENCE, inputYields.at(yieldid), mainmask, 1.0, 1.0,0.0)));
		
		
		}catch (...)
		{
			_exhandler->raisefromcatch(std::string(GetMask()), "FMTyieldmodeldecisiontree::FMTyieldmodeldecisiontree()", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
	}

	FMTyieldmodeldecisiontree::FMTyieldmodeldecisiontree(const FMTyieldmodeldecisiontree& rhs):
		Core::FMTyieldmodel(rhs),
		mtx(),
		values(rhs.values),
		reference(),
		nodes(rhs.nodes),
		default_values(rhs.default_values)
	{
		reference = std::move(std::unique_ptr<Core::FMToutput>(new Core::FMToutput(*rhs.reference)));
	}


	std::unique_ptr<FMTyieldmodel>FMTyieldmodeldecisiontree::Clone() const
	{
		try {
			return std::unique_ptr<FMTyieldmodel>(new FMTyieldmodeldecisiontree(*this));
		}
		catch (...)
		{
			_exhandler->raisefromcatch(std::string(GetMask()), "FMTyieldmodeldecisiontree::Clone", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTyieldmodel>(nullptr);
	}

	std::unique_ptr<Models::FMTmodel> FMTyieldmodeldecisiontree::GetNaturalGrowth(const Core::FMTyieldrequest& request) const
	{
		try {
			const Graph::FMTgraphvertextoyield* graphinfo = request.getvertexgraphinfo();
			const Models::FMTmodel* modelptr = graphinfo->getmodel();
			std::vector<Core::FMTactualdevelopment>newareas;
			const Core::FMTmask mask = reference->getsources().begin()->getmask();
			for (const Core::FMTactualdevelopment& developement : modelptr->getarea())
				{
				if (developement.getmask().issubsetof(mask))
					{
					newareas.push_back(developement);
					}
				}
			std::unique_ptr<Models::FMTmodel>naturalgrowth = modelptr->getcopy(0);
			naturalgrowth->setarea(newareas);//Will only work with lp model going to get big with semodel...
			naturalgrowth->setname(std::string(reference->getsources().begin()->getmask()));
			//std::vector<Core::FMTaction> newactions = naturalgrowth->getactions();
			const int updatestopat = modelptr->getparameter(Models::FMTintmodelparameters::UPDATE);
			/*for (Core::FMTaction& action : newactions)
			{
				if (action.getname() != "_DEATH")
					{
					for (auto& element : action)
						{
						const int lower = std::max(element.second.getperiodlowerbound(), 1);
						const int upper = std::max(element.second.getperiodupperbound(), updatestopat-1);
						element.second.setbounds(Core::FMTperbounds(Core::FMTsection::Action, upper, lower));
						}
					action.update();
					}
			}
			naturalgrowth->setactions(newactions);*/
			std::vector<Core::FMTconstraint>newconstraints;

			/*size_t constraintid = 0;
			for (const Core::FMTconstraint& constraint : naturalgrowth->getconstraints())
			{
				if (constraintid>0)
				{
					newconstraints.push_back(constraint);
				}else {*/
					std::vector<Core::FMToutputsource>sources;
					FMToutputsource source(Core::FMTspec(), mask,Core::FMTotar::inventory);
					sources.push_back(source);
					Core::FMToutput newoutput("naturalgrowth","","", sources, std::vector<Core::FMToperator>());
					Core::FMTconstraint newobjective(FMTconstrainttype::FMTMINobjective, newoutput);
					newobjective.setlength(1);
					newconstraints.push_back(newobjective);
			/* }

				++constraintid;
			}*/
			naturalgrowth->setconstraints(newconstraints);
			naturalgrowth->setactions(std::vector<Core::FMTaction>());
			//naturalgrowth->setconstraints(naturalgrowth->goalconstraints());
			//naturalgrowth->setparallellogger(Logging::FMTquietlogger());
			if (!naturalgrowth->doplanning(true))
				{
				_exhandler->raise(Exception::FMTexc::FMTinfeasibleconstraint,
					"Infeasible natural growth for "+std::string(mask) , "FMTyieldmodeldecisiontree::GetNaturalGrowth", __LINE__, __FILE__, Core::FMTsection::Yield);
				}
			return naturalgrowth;
		}catch (...)
		{
			_exhandler->raisefromcatch(std::string(GetMask()), "FMTyieldmodeldecisiontree::GetNaturalGrowth", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
	return std::unique_ptr<Models::FMTmodel>(nullptr);
	}

	size_t FMTyieldmodeldecisiontree::GetADecision(const std::unique_ptr<Models::FMTmodel>& naturalgrowth, const size_t& constraint_id, const int& period/*, std::string& decision_stack*/) const
		{
		size_t target = 0;
		try {
			const int time_lag = static_cast<int>(nodes.at(constraint_id).getyieldbound("LAG").getlower());
			const int update_period = naturalgrowth->getparameter(Models::FMTintmodelparameters::UPDATE);
			const int targeted_period = std::max(period + time_lag, update_period);
			const double reference_value = naturalgrowth->getoutput(*reference, targeted_period, Core::FMToutputlevel::totalonly).at("Total");
			const double value = naturalgrowth->getoutput(nodes.at(constraint_id), targeted_period, Core::FMToutputlevel::totalonly).at("Total");
			const double percentage_value = (value/reference_value) * 100;
			//decision_stack += ("("+std::to_string(percentage_value)+")");
			std::vector<double>evaluates;
			evaluates.push_back(percentage_value);
			if (nodes.at(constraint_id).evaluate(evaluates)>0)
				{
				target = static_cast<size_t>(nodes.at(constraint_id).getyieldbound("Decisions").getlower());
			}else {
				target = static_cast<size_t>(nodes.at(constraint_id).getyieldbound("Decisions").getupper());
				}
			
		}catch (...)
			{
			_exhandler->raisefromcatch("On constraint "+std::string(nodes.at(constraint_id)), "FMTyieldmodeldecisiontree::GetADecision", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return target;
		}

	const std::vector<double>FMTyieldmodeldecisiontree::Predict(const Core::FMTyieldrequest& request) const
	{
		std::vector<double>returned;
		try {
			const Graph::FMTgraphvertextoyield* graphinfo = request.getvertexgraphinfo();
			const Models::FMTmodel* modelptr = graphinfo->getmodel();
			const int modelLength = modelptr->getparameter(Models::FMTintmodelparameters::LENGTH);
			if (values.empty())
			{
				
				const int update_period = modelptr->getparameter(Models::FMTintmodelparameters::UPDATE);
				if (request.getdevelopment().getperiod()<update_period)
					{
					return default_values;
				}else {
					for (size_t yieldid = 0; yieldid < default_values.size(); ++yieldid)
						{
						std::vector<double>base_values(update_period, default_values.at(yieldid));//Dont forget period 0!
						values[yieldid] = base_values;
						}
					boost::lock_guard<boost::recursive_mutex> guard(mtx);
					const std::unique_ptr<Models::FMTmodel>naturalgrowthmodel = GetNaturalGrowth(request);
					/*std::ofstream decisionfile;
					decisionfile.open("D:/test/"+ std::string(request.getdevelopment()) +".txt");*/
					for (int period = update_period; period <= modelLength; ++period)
					{
						//std::string decision_stack(std::string(request.getdevelopment())+std::to_string(period));
						//size_t decisionid = 0;
						//decision_stack += "\nfrom " + std::string(decisionid, ' ') + nodes.at(0).getname();
						size_t target_node = GetADecision(naturalgrowthmodel, 0, period);// , decision_stack);
						//decision_stack += "\n";
						//++decisionid;
						//decision_stack += std::string(decisionid, ' ') + " to " + nodes.at(target_node).getname() + "\n";
						//++decisionid;
						while (!nodes.at(target_node).FMToutput::empty())//If you get and empty output then you make a decision!
						{
							//decision_stack += std::string(decisionid, ' ') + " from " + nodes.at(target_node).getname();
							target_node = GetADecision(naturalgrowthmodel, target_node, period);// , decision_stack);
							//decision_stack += "\n" + std::string(decisionid, ' ')+" to " + nodes.at(target_node).getname() + "\n";
							//++decisionid;
						}
						double lowerbound = 0;//value
						double upperbound = 0;//target yield
						nodes.at(target_node).getbounds(lowerbound, upperbound, 1);
						const size_t yieldid = static_cast<size_t>(upperbound);
						const int rest_of_period = static_cast<int>(nodes.at(target_node).getyieldbound("LAG").getlower());
						if (rest_of_period)
						{
							for (; period <= naturalgrowthmodel->getparameter(Models::FMTintmodelparameters::LENGTH); ++period)
							{
								values[yieldid].push_back(lowerbound);
							}
						}
						else {
							values[yieldid].push_back(lowerbound);
						}

						
						/*const std::string dev = std::string(request.getdevelopment());
						if (lowerbound<1)
						{
							decisionfile << decision_stack << "\n";
						}*/
					}
					//decisionfile.close();
					
				}
				//unlock
			}
			
			
			for (std::map<size_t,std::vector<double>>::const_iterator valuesit = values.begin(); valuesit!=values.end(); ++valuesit)
			{
				returned.push_back(valuesit->second.at(std::min(request.getdevelopment().getperiod(), modelLength)));
			}
			

		}
		catch (...)
		{
			_exhandler->raisefromcatch(std::string(GetMask()), "FMTyieldmodeldecisiontree::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return returned;
	}

	std::unique_ptr<FMTyieldmodel> FMTyieldmodeldecisiontree::Modify(const FMTmaskfilter& filter,
		const std::vector<FMTtheme>& newthemes, bool presolve) const
	{
		try {
			FMTyieldmodeldecisiontree newdecisions(*this);
			std::vector<Core::FMTaction>emptyactions;
			Core::FMTyields newtempyields;
			for (Core::FMTconstraint& constraint : newdecisions.nodes)
			{
				if (!constraint.FMToutput::empty())
				{
					Core::FMToutputsource oldsource = *constraint.getsources().begin();
					if (presolve)
					{
						oldsource.setmask(oldsource.getmask().presolve(filter, newthemes));
					}
					else {
						oldsource.setmask(oldsource.getmask().postsolve(filter, newthemes));
					}

					std::vector<Core::FMToutputsource>sources;
					sources.push_back(oldsource);
					Core::FMToutput newoutput(constraint.getname(), constraint.getdescription(), constraint.FMToutput::getgroup(), sources, std::vector<Core::FMToperator>());
					constraint.setoutput(newoutput);
				}
				
			}
			Core::FMToutputsource oldsource = *reference->getsources().begin();
			if (presolve)
			{
				oldsource.setmask(oldsource.getmask().presolve(filter, newthemes));
			}
			else {
				oldsource.setmask(oldsource.getmask().postsolve(filter, newthemes));
			}
			std::vector<Core::FMToutputsource>sources;
			sources.push_back(oldsource);
			newdecisions.reference = std::unique_ptr<Core::FMToutput>(new Core::FMToutput(reference->getname(), reference->getdescription(), reference->getgroup(), sources, std::vector<Core::FMToperator>()));
			return std::unique_ptr<FMTyieldmodel>(new FMTyieldmodeldecisiontree(newdecisions));

		}
		catch (...)
		{
			_exhandler->raisefromcatch(std::string(GetMask()), "FMTyieldmodeldecisiontree::Modify", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTyieldmodel>(nullptr);
	}


	std::unique_ptr<FMTyieldmodel> FMTyieldmodeldecisiontree::presolve(const FMTmaskfilter& filter,
		const std::vector<FMTtheme>& newthemes) const
	{
		try {
			return Modify(filter, newthemes);
		}
		catch (...)
		{
			_exhandler->raisefromcatch(std::string(GetMask()), "FMTyieldmodeldecisiontree::presolve", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTyieldmodel>(nullptr);
	}

	std::unique_ptr<FMTyieldmodel> FMTyieldmodeldecisiontree::postsolve(const FMTmaskfilter& filter,
		const std::vector<FMTtheme>& basethemes) const
	{
		try {
			return Modify(filter, basethemes,false);
		}
		catch (...)
		{
			_exhandler->raisefromcatch(std::string(GetMask()), "FMTyieldmodeldecisiontree::postsolve", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTyieldmodel>(nullptr);
	}



}