/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTYieldModelDecisionTree.h"
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include "FMTYieldRequest.h"
#include "FMTExceptionHandler.h"
#include "FMTOutput.h"
#include "FMTConstraint.h"
#include "FMTGraphVertexToYield.h"
#include "FMTQuietLogger.h"
#include "FMTModel.h"



namespace Core {


	Core::FMTMask FMTYieldModelDecisionTree::_getMask() const
	{
		Core::FMTMask mask;
		try {
			if (m_reference && !m_reference->getSources().empty())
			{
				mask = m_reference->getSources().begin()->getMask();

			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYieldModelDecisionTree::getMask", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return mask;
	}




	std::vector<std::vector<double>>FMTYieldModelDecisionTree::getPeriodicValues() const
	{
		std::vector<std::vector<double>>returned;
		try {
			if (!m_values.empty())
			{
				for (std::map<size_t, std::vector<double>>::const_iterator valuesit = m_values.begin(); valuesit != m_values.end(); ++valuesit)
				{
					returned.push_back(valuesit->second);
				}
			}

		}catch (...)
		{
			_exhandler->raiseFromCatch(std::string(_getMask()), "FMTYieldModelDecisionTree::getPeriodicValues", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return returned;
	}



	Core::FMTConstraint FMTYieldModelDecisionTree::_buildConstraint(const std::string& name, const std::string& yld, const Core::FMTMask& mask, const double& lowerbound, const double& upperbound, const int& lag) const
	{
		Core::FMTConstraint constraint;
		try {
			std::vector<Core::FMTOutputSource>sources;
			if (!mask.empty())
			{
				FMTOutputSource source(Core::FMTSpec(), mask,
					Core::FMTotar::inventory, yld);
				sources.push_back(source);
			}
			constraint = Core::FMTConstraint(Core::FMTconstrainttype::FMTstandard, Core::FMTOutput(name, "", "Decision_tree", sources, std::vector<Core::FMTOperator>()));
			constraint.setRhs(lowerbound, upperbound);
			constraint.setLength(1);
			const double dbl_lag = static_cast<double>(lag);
			constraint.addBounds(FMTYldBounds(FMTsection::Optimize, "LAG", dbl_lag, dbl_lag));
		}
		catch (...)
		{
			_exhandler->raiseFromCatch(std::string(_getMask()), "FMTYieldModelDecisionTree::buildConstraint", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return constraint;
	}

	std::string FMTYieldModelDecisionTree::getModelType()
	{
		return "DECISION_TREE";
	}


	FMTYieldModelDecisionTree::FMTYieldModelDecisionTree(const boost::property_tree::ptree& jsonProps,
		const std::vector<std::string>& inputYields, const Core::FMTMask& mainmask):
		FMTYieldModel(),
		m_mtx(),
		m_values(),
		m_reference(),
		m_nodes(),
		m_default_values(),
		m_update()

	{
		try {
			boost::property_tree::ptree::const_assoc_iterator modelNameIt = jsonProps.find(m_JSON_PROP_MODEL_NAME);
			m_modelName = modelNameIt->second.data();
			m_modelYields = inputYields;
			std::vector<std::pair<std::string,std::string>>targets;
			std::map<std::string, size_t>locations;
			for (const boost::property_tree::ptree::value_type& node : jsonProps.get_child(m_JSON_PROP_TREE))
			{
				const std::string node_name = node.first;
				const size_t yieldid = node.second.get<size_t>("Yield");
				if (yieldid>=inputYields.size())
					{
					const std::string invalidyieldid = "No yield id " + std::to_string(yieldid) + " provided for FMTyieldmodeldecisiontree";
					_exhandler->raise(Exception::FMTexc::FMTrangeerror, invalidyieldid, "FMTYieldModelDecisionTree::FMTYieldModelDecisionTree",__LINE__, __FILE__, Core::FMTsection::Yield);
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
				locations[node_name] = m_nodes.size();
				m_nodes.push_back(_buildConstraint(node_name,yield_name,mainmask,*bounds.begin(),bounds.back(),time_lag));
				targets.push_back(std::pair<std::string, std::string>(*branches.begin(), branches.back()));
				
			}
			for (const boost::property_tree::ptree::value_type& decision : jsonProps.get_child(m_JSON_PROP_DECISIONS))
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
				locations[decision_name] = m_nodes.size();
				m_nodes.push_back(_buildConstraint(decision_name, "", Core::FMTMask(), value, yieldid, lag_value));
				}
			size_t constraint_id = 0;
			for (Core::FMTConstraint& constraint : m_nodes)
			{
				if (constraint_id< targets.size())
				{
					const double positive = static_cast<double>(locations.at(targets.at(constraint_id).first));
					const double negative = static_cast<double>(locations.at(targets.at(constraint_id).second));
					constraint.addBounds(Core::FMTYldBounds(Core::FMTsection::Yield, "Decisions", positive, negative));
				}
				
				++constraint_id;
			}

			for (const boost::property_tree::ptree::value_type& defaultval : jsonProps.get_child(m_JSON_PROP_DEFAULT))
			{
				m_default_values.push_back(defaultval.second.get_value<double>());
			}


			const size_t yieldid = jsonProps.get<size_t>(m_JSON_PROP_REFERENCE);
			if (yieldid >= inputYields.size())
				{
				const std::string invalidyieldid = "No yield id " + std::to_string(yieldid) + " provided for FMTyieldmodeldecisiontree";
				_exhandler->raise(Exception::FMTexc::FMTrangeerror, invalidyieldid, "FMTYieldModelDecisionTree::FMTYieldModelDecisionTree", __LINE__, __FILE__, Core::FMTsection::Yield);
				}
			m_reference = std::unique_ptr<Core::FMTOutput>( new Core::FMTOutput(_buildConstraint(m_JSON_PROP_REFERENCE, inputYields.at(yieldid), mainmask, 1.0, 1.0,0.0)));
		
			const size_t UPDATE_ID = jsonProps.get<size_t>(m_JSON_PROP_UPDATE);
			if (inputYields.size()>UPDATE_ID)
				{
				m_update = inputYields.at(UPDATE_ID);
				}
		
		}catch (...)
		{
			_exhandler->raiseFromCatch(std::string(_getMask()), "FMTYieldModelDecisionTree::FMTYieldModelDecisionTree()", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
	}

	FMTYieldModelDecisionTree::FMTYieldModelDecisionTree(const FMTYieldModelDecisionTree& rhs):
		Core::FMTYieldModel(rhs),
		m_mtx(),
		m_values(rhs.m_values),
		m_reference(),
		m_nodes(rhs.m_nodes),
		m_default_values(rhs.m_default_values),
		m_update(rhs.m_update)
	{
		m_reference = std::move(std::unique_ptr<Core::FMTOutput>(new Core::FMTOutput(*rhs.m_reference)));
	}


	std::unique_ptr<FMTYieldModel>FMTYieldModelDecisionTree::clone() const
	{
		try {
			return std::unique_ptr<FMTYieldModel>(new FMTYieldModelDecisionTree(*this));
		}
		catch (...)
		{
			_exhandler->raiseFromCatch(std::string(_getMask()), "FMTYieldModelDecisionTree::clone", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTYieldModel>(nullptr);
	}

	std::unique_ptr<Models::FMTModel> FMTYieldModelDecisionTree::_getNaturalGrowth(const Core::FMTYieldRequest& request) const
	{
		try {
			const Graph::FMTGraphVertexToYield* graphinfo = request.getVertexGraphInfo();
			//const Models::FMTModel* modelptr = graphinfo->getModel();
			std::vector<Core::FMTActualDevelopment>newareas;
			const Core::FMTMask mask = m_reference->getSources().begin()->getMask();
			for (const Core::FMTActualDevelopment& development : m_modelPtr->getArea())
				{
				if (development.getMask().isSubsetOf(mask))
					{
					newareas.push_back(development);
					}
				}
			
			std::unique_ptr<Models::FMTModel>naturalGrowth = m_modelPtr->getCopy(0);
			Logging::FMTQuietLogger ModelLogger;
			naturalGrowth->setParallelLogger(ModelLogger);
			naturalGrowth->setParameter(Models::FMTboolmodelparameters::QUIET_LOGGING, true);
			naturalGrowth->setArea(newareas);//Will only work with lp model going to get big with semodel...
			naturalGrowth->setName(std::string(m_reference->getSources().begin()->getMask()));
			//std::vector<Core::FMTAction> newactions = naturalGrowth->getActions();
			//const int updatestopat = _getUpdatePeriod(request);
			/*for (Core::FMTAction& action : newactions)
			{
				if (action.getName() != "_DEATH")
					{
					for (auto& element : action)
						{
						const int lower = std::max(element.second.getPeriodLowerBound(), 1);
						const int upper = std::max(element.second.getPeriodUpperBound(), updatestopat-1);
						element.second.setBounds(Core::FMTPerBounds(Core::FMTsection::Action, upper, lower));
						}
					action.update();
					}
			}
			naturalGrowth->setactions(newactions);*/
			std::vector<Core::FMTConstraint>newconstraints;

			/*size_t constraintid = 0;
			for (const Core::FMTConstraint& constraint : naturalGrowth->getConstraints())
			{
				if (constraintid>0)
				{
					newconstraints.push_back(constraint);
				}else {*/
					std::vector<Core::FMTOutputSource>sources;
					FMTOutputSource source(Core::FMTSpec(), mask,Core::FMTotar::inventory);
					sources.push_back(source);
					Core::FMTOutput newoutput("naturalgrowth","","", sources, std::vector<Core::FMTOperator>());
					Core::FMTConstraint newobjective(FMTconstrainttype::FMTMINobjective, newoutput);
					newobjective.setLength(1);
					newconstraints.push_back(newobjective);
			/* }

				++constraintid;
			}*/
			naturalGrowth->setConstraints(newconstraints);
			naturalGrowth->setActions(std::vector<Core::FMTAction>());
			//naturalGrowth->setConstraints(naturalGrowth->goalConstraints());
			//naturalGrowth->setParallelLogger(Logging::FMTQuietLogger());
			if (!naturalGrowth->doPlanning(true))
				{
				_exhandler->raise(Exception::FMTexc::FMTinfeasibleconstraint,
					"Infeasible natural growth for "+std::string(mask) , "FMTYieldModelDecisionTree::_getNaturalGrowth", __LINE__, __FILE__, Core::FMTsection::Yield);
				}
			return naturalGrowth;
		}catch (...)
		{
			_exhandler->raiseFromCatch(std::string(_getMask()), "FMTYieldModelDecisionTree::_getNaturalGrowth", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
	return std::unique_ptr<Models::FMTModel>(nullptr);
	}

	size_t FMTYieldModelDecisionTree::_getADecision(const std::unique_ptr<Models::FMTModel>& p_naturalGrowth,
												size_t p_constraint_id, int p_period, int p_update) const
		{
		size_t target = 0;
		try {
			const int time_lag = static_cast<int>(m_nodes.at(p_constraint_id).getYieldBound("LAG").getLower());
			const int targeted_period = std::max(p_period + time_lag, p_update);
			const double reference_value = p_naturalGrowth->getOutput(*m_reference, targeted_period, Core::FMToutputlevel::totalonly).at("Total");
			const double value = p_naturalGrowth->getOutput(m_nodes.at(p_constraint_id), targeted_period, Core::FMToutputlevel::totalonly).at("Total");
			const double percentage_value = (value/reference_value) * 100;
			//decision_stack += ("("+std::to_string(percentage_value)+")");
			std::vector<double>evaluates;
			evaluates.push_back(percentage_value);
			if (m_nodes.at(p_constraint_id).evaluate(evaluates)>0)
				{
				target = static_cast<size_t>(m_nodes.at(p_constraint_id).getYieldBound("Decisions").getLower());
			}else {
				target = static_cast<size_t>(m_nodes.at(p_constraint_id).getYieldBound("Decisions").getUpper());
				}
			
		}catch (...)
			{
			_exhandler->raiseFromCatch("On constraint "+std::string(m_nodes.at(p_constraint_id)), "FMTYieldModelDecisionTree::_getADecision", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return target;
		}

	const std::vector<double>FMTYieldModelDecisionTree::predict(const Core::FMTYieldRequest& request) const
	{
		std::vector<double>returned;
		try {
			const int MODEL_LENGTH = m_modelPtr->getParameter(Models::FMTintmodelparameters::LENGTH);
			if (m_values.empty())
			{
				const int DEV_PERIOD = request.getDevelopment().getPeriod();
				
				const int UPDATE_PERIOD = _getUpdatePeriod(request);
				if (DEV_PERIOD< UPDATE_PERIOD)
					{
					return m_default_values;
				}else {
					const int FIRST_PERIOD = m_modelPtr->getArea().cbegin()->getPeriod();
					const int FILL_IN = std::max(UPDATE_PERIOD, FIRST_PERIOD+1);
					for (size_t yieldid = 0; yieldid < m_default_values.size(); ++yieldid)
						{
						std::vector<double>base_values(FILL_IN, m_default_values.at(yieldid));//Dont forget period 0!
						m_values[yieldid] = base_values;
						}
					boost::lock_guard<boost::recursive_mutex> guard(m_mtx);
					const std::unique_ptr<Models::FMTModel>naturalgrowthmodel = _getNaturalGrowth(request);
					const int MAX_PERIOD = (FIRST_PERIOD + MODEL_LENGTH);
					for (int period = FILL_IN; period <= MAX_PERIOD; ++period)
					{
						size_t target_node = _getADecision(naturalgrowthmodel, 0, period, UPDATE_PERIOD);// , decision_stack);
						while (!m_nodes.at(target_node).FMTOutput::empty())//If you get and empty output then you make a decision!
						{
							target_node = _getADecision(naturalgrowthmodel, target_node, period, UPDATE_PERIOD);// , decision_stack);
						}
						double lowerbound = 0;//value
						double upperbound = 0;//target yield
						m_nodes.at(target_node).getBounds(lowerbound, upperbound, 1);
						const size_t yieldid = static_cast<size_t>(upperbound);
						const int rest_of_period = static_cast<int>(m_nodes.at(target_node).getYieldBound("LAG").getLower());
						if (rest_of_period)
						{
							for (; period <= MAX_PERIOD; ++period)
							{
								m_values[yieldid].push_back(lowerbound);
							}
						}
						else {
							m_values[yieldid].push_back(lowerbound);
						}
					}
					
				}
				//unlock
			}
			
			
			for (std::map<size_t,std::vector<double>>::const_iterator valuesit = m_values.begin(); valuesit!= m_values.end(); ++valuesit)
			{
				const size_t INDEX = std::min(static_cast<size_t>(request.getDevelopment().getPeriod()), valuesit->second.size() - 1);
				returned.push_back(valuesit->second.at(INDEX));
			}
			

		}
		catch (...)
		{
			_exhandler->raiseFromCatch(std::string(_getMask()), "FMTYieldModelDecisionTree::predict", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return returned;
	}

	int FMTYieldModelDecisionTree::_getUpdatePeriod(const Core::FMTYieldRequest& p_request) const
		{
		int update = 0;
		try {
			if (!m_update.empty())
				{
				update = p_request.getFirstSeen(m_update)->second->get(m_update, p_request);
				}else if (m_modelPtr)
					{
					update = m_modelPtr->getParameter(Models::FMTintmodelparameters::UPDATE);
					}
			}catch (...)
				{
				_exhandler->raiseFromCatch(std::string(_getMask()), 
					"FMTYieldModelDecisionTree::_getUpdatePeriod", __LINE__, __FILE__, Core::FMTsection::Yield);
				}
		return update;
		}


	std::unique_ptr<FMTYieldModel> FMTYieldModelDecisionTree::_modify(const FMTMaskFilter& filter,
		const std::vector<FMTTheme>& newthemes, bool preSolve) const
	{
		try {
			FMTYieldModelDecisionTree newdecisions(*this);
			std::vector<Core::FMTAction>emptyactions;
			Core::FMTYields newtempyields;
			for (Core::FMTConstraint& constraint : newdecisions.m_nodes)
			{
				if (!constraint.FMTOutput::empty())
				{
					Core::FMTOutputSource oldsource = *constraint.getSources().begin();
					if (preSolve)
					{
						oldsource.setMask(oldsource.getMask().preSolve(filter, newthemes));
					}
					else {
						oldsource.setMask(oldsource.getMask().postSolve(filter, newthemes));
					}

					std::vector<Core::FMTOutputSource>sources;
					sources.push_back(oldsource);
					Core::FMTOutput newoutput(constraint.getName(), constraint.getDescription(), constraint.FMTOutput::getGroup(), sources, std::vector<Core::FMTOperator>());
					constraint.setOutput(newoutput);
				}
				
			}
			Core::FMTOutputSource oldsource = *m_reference->getSources().begin();
			if (preSolve)
			{
				oldsource.setMask(oldsource.getMask().preSolve(filter, newthemes));
			}
			else {
				oldsource.setMask(oldsource.getMask().postSolve(filter, newthemes));
			}
			std::vector<Core::FMTOutputSource>sources;
			sources.push_back(oldsource);
			newdecisions.m_reference = std::unique_ptr<Core::FMTOutput>(new Core::FMTOutput(m_reference->getName(), m_reference->getDescription(), m_reference->getGroup(), sources, std::vector<Core::FMTOperator>()));
			return std::unique_ptr<FMTYieldModel>(new FMTYieldModelDecisionTree(newdecisions));

		}
		catch (...)
		{
			_exhandler->raiseFromCatch(std::string(_getMask()), "FMTYieldModelDecisionTree::_modify", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTYieldModel>(nullptr);
	}


	std::unique_ptr<FMTYieldModel> FMTYieldModelDecisionTree::preSolve(const FMTMaskFilter& filter,
		const std::vector<FMTTheme>& newthemes) const
	{
		try {
			return _modify(filter, newthemes);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch(std::string(_getMask()), "FMTYieldModelDecisionTree::preSolve", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTYieldModel>(nullptr);
	}

	std::unique_ptr<FMTYieldModel> FMTYieldModelDecisionTree::postSolve(const FMTMaskFilter& filter,
		const std::vector<FMTTheme>& basethemes) const
	{
		try {
			return _modify(filter, basethemes,false);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch(std::string(_getMask()), "FMTYieldModelDecisionTree::postSolve", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTYieldModel>(nullptr);
	}



}