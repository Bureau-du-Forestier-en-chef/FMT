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
#include "FMTmodel.hpp"



namespace Core {




	FMTyieldmodeldecisiontree::operator std::string() const
	{
		std::string value = "";
		try {
			const std::string completename = GetModelName();
			const boost::filesystem::path modelpath(completename);
			const boost::filesystem::path dir = modelpath.parent_path();
			const std::string shortmodelname = dir.stem().string();
			std::string data(shortmodelname);
			for (const std::string yield : GetModelYields())
			{
				data += ("," + yield);
			}
			value = " _PRED(" + data + ")\n";
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodeldecisiontree::operator std::string()", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return value;
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
			_exhandler->raisefromcatch("", "FMTyieldmodeldecisiontree::getperiodicvalues", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return returned;
	}



	Core::FMTconstraint FMTyieldmodeldecisiontree::buildconstraint(const std::string& name, const std::string& yld, const Core::FMTmask& mask, const double& lowerbound, const double& upperbound) const
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
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodeldecisiontree::buildconstraint", __LINE__, __FILE__, Core::FMTsection::Yield);
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
		nodes()

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
				nodes.push_back(buildconstraint(node_name,yield_name,mainmask,*bounds.begin(),bounds.back()));
				targets.push_back(std::pair<std::string, std::string>(*branches.begin(), branches.back()));
				
			}
			for (const boost::property_tree::ptree::value_type& decision : jsonProps.get_child(JSON_PROP_DECISIONS))
				{
				const std::string decision_name = decision.first;
				const double value = decision.second.get<double>("Value");
				const double yieldid = decision.second.get<double>("Yield");
				locations[decision_name] = nodes.size();
				nodes.push_back(buildconstraint(decision_name, "", Core::FMTmask(), value, yieldid));
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
			const size_t yieldid = jsonProps.get<size_t>(JSON_PROP_REFERENCE);
			if (yieldid >= inputYields.size())
				{
				const std::string invalidyieldid = "No yield id " + std::to_string(yieldid) + " provided for FMTyieldmodeldecisiontree";
				_exhandler->raise(Exception::FMTexc::FMTrangeerror, invalidyieldid, "FMTyieldmodeldecisiontree::FMTyieldmodeldecisiontree", __LINE__, __FILE__, Core::FMTsection::Yield);
				}
			reference = std::unique_ptr<Core::FMToutput>( new Core::FMToutput(buildconstraint(JSON_PROP_REFERENCE, inputYields.at(yieldid), mainmask, 1.0, 1.0)));
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodeldecisiontree::FMTyieldmodeldecisiontree()", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
	}

	FMTyieldmodeldecisiontree::FMTyieldmodeldecisiontree(const FMTyieldmodeldecisiontree& rhs):
		Core::FMTyieldmodel(rhs),
		mtx(),
		values(rhs.values),
		reference(),
		nodes(rhs.nodes)
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
			_exhandler->raisefromcatch("", "FMTyieldmodeldecisiontree::Clone", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTyieldmodel>(nullptr);
	}

	std::unique_ptr<Models::FMTmodel> FMTyieldmodeldecisiontree::getnaturalgrowth(const Core::FMTyieldrequest& request) const
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
			std::vector<Core::FMTaction>newactions;
			std::vector<Core::FMTtransition>newtransitions;
			const std::vector<Core::FMTaction> oldactions = naturalgrowth->getactions();
			const std::vector<Core::FMTtransition> oldtransitions = naturalgrowth->gettransitions();
			for (size_t actionid = 0 ; actionid < oldactions.size();++actionid)
			{
				if (oldactions.at(actionid).getname()=="_DEATH")
					{
					newactions.push_back(oldactions.at(actionid));
					newtransitions.push_back(oldtransitions.at(actionid));
					break;
					}
			}
			naturalgrowth->setactions(newactions);
			naturalgrowth->settransitions(newtransitions);
			std::vector<Core::FMTconstraint>newconstraints;
			size_t constraintid = 0;
			for (const Core::FMTconstraint& constraint : naturalgrowth->getconstraints())
			{
				if (constraintid>0)
				{
					newconstraints.push_back(constraint);
				}else {
					std::vector<Core::FMToutputsource>sources;
					FMToutputsource source(Core::FMTspec(), mask,Core::FMTotar::inventory);
					sources.push_back(source);
					Core::FMToutput newoutput("naturalgrowth","","", sources, std::vector<Core::FMToperator>());
					Core::FMTconstraint newobjective(FMTconstrainttype::FMTMINobjective, newoutput);
					newobjective.setlength(1);
					newconstraints.push_back(newobjective);
				}

				++constraintid;
			}
			naturalgrowth->setconstraints(newconstraints);
			if (!naturalgrowth->doplanning(true))
				{

				_exhandler->raise(Exception::FMTexc::FMTinfeasibleconstraint,
					"Infeasible natural growth for "+std::string(mask) , "FMTyieldmodeldecisiontree::getnaturalgrowth", __LINE__, __FILE__, Core::FMTsection::Yield);
				}
			return naturalgrowth;
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodeldecisiontree::getnaturalgrowth", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
	return std::unique_ptr<Models::FMTmodel>(nullptr);
	}

	size_t FMTyieldmodeldecisiontree::getadecision(const std::unique_ptr<Models::FMTmodel>& naturalgrowth, const size_t& constraint_id, const int& period) const
		{
		size_t target = 0;
		try {
			const double reference_value = naturalgrowth->getoutput(*reference, period, Core::FMToutputlevel::totalonly).at("Total");
			const double value = naturalgrowth->getoutput(nodes.at(constraint_id),period, Core::FMToutputlevel::totalonly).at("Total");
			const double percentage_value = (value/reference_value) * 100;
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
			_exhandler->raisefromcatch("", "FMTyieldmodeldecisiontree::getadecision", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return target;
		}

	const std::vector<double>FMTyieldmodeldecisiontree::Predict(const Core::FMTyieldrequest& request) const
	{
		std::vector<double>returned;
		try {
			if (values.empty())
			{
				boost::lock_guard<boost::recursive_mutex> guard(mtx);
				const std::unique_ptr<Models::FMTmodel>naturalgrowthmodel = getnaturalgrowth(request);
				for (int period = 0; period <= naturalgrowthmodel->getparameter(Models::FMTintmodelparameters::LENGTH);++period)
				{
					size_t target_node = getadecision(naturalgrowthmodel,0,period);
					while (!nodes.at(target_node).FMToutput::empty())
					{
						target_node = getadecision(naturalgrowthmodel, target_node,period);
					}
					double lowerbound = 0;//value
					double upperbound = 0;//target yield
					nodes.at(target_node).getbounds(lowerbound, upperbound, 1);
					const size_t yieldid = static_cast<size_t>(upperbound);
					if (values.find(yieldid)== values.end())
					{
						values[yieldid] = std::vector<double>();
					}
					values[yieldid].push_back(lowerbound);
				}
				//unlock
			}
			for (std::map<size_t,std::vector<double>>::const_iterator valuesit = values.begin(); valuesit!=values.end(); ++valuesit)
			{
				returned.push_back(valuesit->second.at(request.getdevelopment().getperiod()));
			}
			

		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodeldecisiontree::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return returned;
	}

	std::unique_ptr<FMTyieldmodel> FMTyieldmodeldecisiontree::modify(const FMTmaskfilter& filter,
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
			_exhandler->raisefromcatch("", "FMTyieldmodeldecisiontree::modify", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTyieldmodel>(nullptr);
	}


	std::unique_ptr<FMTyieldmodel> FMTyieldmodeldecisiontree::presolve(const FMTmaskfilter& filter,
		const std::vector<FMTtheme>& newthemes) const
	{
		try {
			return modify(filter, newthemes);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodeldecisiontree::presolve", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTyieldmodel>(nullptr);
	}

	std::unique_ptr<FMTyieldmodel> FMTyieldmodeldecisiontree::postsolve(const FMTmaskfilter& filter,
		const std::vector<FMTtheme>& basethemes) const
	{
		try {
			return modify(filter, basethemes,false);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodeldecisiontree::postsolve", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTyieldmodel>(nullptr);
	}



}