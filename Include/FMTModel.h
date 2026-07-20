/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTMODEL_Hm_included
#define FMTMODEL_Hm_included

#include "FMTTheme.h"
#include "FMTAction.h"
#include "FMTTransition.h"
#include "FMTYields.h"
#include "FMTLifespans.h"
#include "FMTActualDevelopment.h"
#include "FMTOutput.h"
#include "FMTConstraint.h"
#include "FMTList.hpp"
#include "FMTModelParameters.h"
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/export.hpp>
#include "FMTOutputNode.h"
#include "FMTSchedule.h"
#include <random>


namespace Graph
{
	template<class T1, class T2>
	class FMTGraph;
	class FMTLineGraph;
	class FMTGraphVertexToYield;
}

namespace Parser
{
	class FMTModelParser;
}

namespace Spatial
{
	class FMTSpatialSchedule;
	class FMTSpatialNodesCache;
	class FMTSpatialGraphs;
}
/// Namespace containing all enumerator and classes used to construct or manipulate the different type of model in FMT.
namespace Models
{
	class FMTModelStats;
	// DocString: FMTModel
	/**
	@brief Base class for the different models in FMT.
	@details FMTModel is the formulation seen in the Woodstock files of a forest planning problem.
	FMTLpModel / FMTSaModel / FMTSesModel use the FMTModel protected members to generate the
	problem based on LP/SA or to simply simulate the FMTModel.
	*/

	class FMTEXPORT FMTModel : public Core::FMTObject
    {
		template<class T1,class T2>
		friend class Graph::FMTGraph;
		friend class Graph::FMTLineGraph;
		friend class Spatial::FMTSpatialSchedule;
		friend class Spatial::FMTSpatialNodesCache;
		friend class Spatial::FMTSpatialGraphs;
		friend class Parser::FMTModelParser;
		friend class Graph::FMTGraphVertexToYield;
	public:
        // DocString: FMTModel::getYieldValue
		/**
		@brief Get yield value 
		@param[in] p_mask mask in string format
		@param[in] p_yield the yield name.
        @param[in] p_age Development age
        @param[in] p_period Development period
        @return the yield value.
		*/
		double getYieldValue(const std::string& p_mask, const std::string& p_yield, int p_age, int p_period) const;
		// DocString: FMTModel::setReplicate
		/**
		@brief Set the constraints to the specified Replanning Period.
		@param[in] p_replicate the replicate of the replanning task
		@param[in] p_ReplanningPeriod the replanning period.
		*/
		void setReplicate(size_t p_replicate,int p_ReplanningPeriod);
		// DocString: FMTModel::gotReIgnore
		/**
		@brief Check if setReIgnore needs to be called.
		@param[in] p_replanningPeriod the replanning period to check.
		@return true if reignore is set else false.
		*/
		bool gotReIgnore(const int& p_replanningPeriod) const;
		// DocString: FMTModel::gotReplicate
		/**
		@brief Check if setReplicate needs to be called.
		@param[in] p_replanningPeriod the replanning period to check.
		@return true if replicate is set else false.
		*/
		bool gotReplicate(const int& p_replanningPeriod) const;
		// DocString: FMTModel::buildAction
		/**
		@brief Build an action based on the positive value of a yield.
		@param[in] p_actionName the name of the action to create.
		@param[in] p_Targetyield the yield to look for positive value.
		@return a valid FMTModel.
		*/
		FMTModel buildAction(const std::string& p_actionName,
							const std::string& p_Targetyield) const;
		// DocString: FMTModel::buildSchedule
		/**
		@brief Build a schedule by looking at operability of the actual developments.
		@param[in] p_action the action we want to build a schedule for.
		@param[in] p_BaseModel the base model.
		@param[in] p_Targetyield the yield name to look for.
		@param[in] p_schedules the non extended schedule we want to build on.
		@return a complete schedule of the length of the model with actual developments.
		*/
		std::vector<Core::FMTSchedule> buildSchedule(const Core::FMTAction& p_action,
													const FMTModel& p_BaseModel,
													const std::string& p_Targetyield,
													const std::vector<Core::FMTSchedule>& p_schedules) const;
		
		// DocString: FMTModel::pushTheme
		/**
		@brief Push a new theme and update the whole model with it.
		@param[in] p_themeName the name of the theme to push.
		@param[in] p_yieldName the yield used for SBW.
		@param[in] p_attributes the attributes of the theme, will be the default of every mask.
		*/
		void pushTheme(const std::string& p_themeName,
			const std::string& p_yieldName,
			const std::vector<std::string>& p_attributes);
		// DocString: FMTModel::aggregateAllActions
		/**
		@brief Aggregate actions together and create a new model with new actions, transitions, outputs, constraints, yields and lifespan.
		@param[in] p_Aggregates the action aggregates we want to merge together.
		@param[in] p_ActionOrdering the order used for the hierarchy of actions, if empty uses the order of the actual actions.
		@return a newly created model with aggregated actions.
		*/
		Models::FMTModel aggregateAllActions(const std::vector<std::string>& p_Aggregates,
											std::vector<std::string> p_ActionOrdering) const;
		// DocString: FMTModel::splitActions
		/**
		@brief Split actions using the provided masks, new actions are named action_mask and aggregated into the original action name.
		@param[in] p_Actions the actions that you want to split.
		@param[in] p_masks the masks you want to use to split the actions.
		@return a newly created model with split actions.
		*/
		Models::FMTModel splitActions(const std::vector<std::string>& p_Actions,
										const std::vector<std::string>& p_masks) const;
		// DocString: FMTSchedule::getPriorities
		/**
		@brief Return the action names in priority of area scheduled.
		@param[in] p_schedules the schedules to look at.
		@return a vector of action names.
		*/
		std::vector<std::string>getSchedulesPriorities(const std::vector<Core::FMTSchedule>& p_schedules) const;
		// DocString: FMTModel::aggregateSchedules
		/**
		@brief Build a new schedule based on p_schedules with the corresponding aggregated actions.
		@param[in] p_schedules the schedules of the original model.
		@return newly created schedules with aggregated actions.
		*/
		std::vector<Core::FMTSchedule> aggregateSchedules(const std::vector<Core::FMTSchedule>& p_schedules) const;
		// DocString: FMTModel::splitSchedules
		/**
		@brief Using the actions of the model change the actions of the schedule.
		@param[in] p_schedules the schedules of the original model.
		@return newly created schedules with split actions.
		*/
		std::vector<Core::FMTSchedule> splitSchedules(const std::vector<Core::FMTSchedule>& p_schedules) const;
		// DocString: FMTModel::setParallelLogger
		/**
		@brief Set a thread owned logger so the solver does not work in concurrency.
		@param[in,out] logger the logger owned by the thread to pass to the solver.
		*/
		virtual void setParallelLogger(Logging::FMTLogger& logger);
		// DocString: FMTModel::clearCache
		/**
		@brief Clear caching elements to reduce memory usage.
		*/
		virtual void clearCache();
		// DocString: FMTModel(const std::vector<Core::FMTActualDevelopment>& larea, const std::vector<Core::FMTTheme>, const std::vector<Core::FMTAction>, const std::vector<Core::FMTTransition>, const Core::FMTYields, const Core::FMTLifespans, const std::string, const std::vector<Core::FMTOutput>, std::vector<Core::FMTConstraint>,FMTModelParameters)
		/**
		@brief Main constructor for FMTModel used in Parser::FMTModelParser, constraints and parameters are optional.
		@param[in] larea the initial actual developments (area section).
		@param[in] lthemes the model themes.
		@param[in] lactions the model actions.
		@param[in] ltransitions the model transitions.
		@param[in] lyields the model yields.
		@param[in] llifespan the model lifespans.
		@param[in] lname the model name.
		@param[in] loutputs the model outputs.
		@param[in] lconstraints the model constraints, optional and not needed for FMTSesModel.
		@param[in] lparameters the model parameters.
		*/
		FMTModel(const std::vector<Core::FMTActualDevelopment>& larea, const std::vector<Core::FMTTheme>& lthemes,
			const std::vector<Core::FMTAction>& lactions,
			const std::vector<Core::FMTTransition>& ltransitions, const Core::FMTYields& lyields, const Core::FMTLifespans& llifespan,
			const std::string& lname, const std::vector<Core::FMTOutput>& loutputs, std::vector<Core::FMTConstraint> lconstraints = std::vector<Core::FMTConstraint>(),
			FMTModelParameters lparameters = FMTModelParameters());
		// DocString: FMTModel()
		/**
		@brief Default constructor of FMTModel.
		*/
		FMTModel();
		// DocString: ~FMTModel
		/**
		@brief Default virtual destructor of FMTModel.
		*/
		virtual ~FMTModel() = default;
		// DocString: FMTModel(FMTModel&&)
		/**
		@brief Default move constructor for FMTModel.
		@param[in,out] rhs the FMTModel to move from.
		*/
		FMTModel(FMTModel&& rhs);
		// DocString: FMTModel::operator=(FMTModel&&) 
		/**
		@brief Default move assignment for FMTModel.
		@param[in,out] rhs the FMTModel to move from.
		@return a reference to this FMTModel.
		*/
		FMTModel& operator =(FMTModel&& rhs);
		// DocString: FMTModel::getArea
		/**
		@brief Get the area of a given period as actual developments.
		@param[in] period the period to get the area from.
		@param[in] beforegrowanddeath if true returns the developments before growth and death to the next period.
		@return the actual developments of the requested period.
		*/
		virtual std::vector<Core::FMTActualDevelopment>getArea(int period = 0, bool beforegrowanddeath = false) const;
		// DocString: FMTModel::getCopy
		/**
		@brief Return a copy of the FMTModel for the selected period, cleaning the constraints and keeping the objective.
		@param[in] period the period to copy.
		@return a unique pointer to the copied FMTModel.
		*/
		virtual std::unique_ptr<FMTModel> getCopy(int period = 0) const;
		// DocString: FMTModel::basePresolve
		/**
		@brief Presolve the model using the existing area.
		@details Can reduce the number of global themes, actions, transitions, yields, lifespans, outputs and constraints if the model is badly formulated.
		@return the presolved FMTModel.
		*/
		FMTModel basePresolve() const;
		// DocString: FMTModel::getPresolveFilter
		/**
		@brief Get the presolve filter.
		@param[in] originalthemes the original themes of the model.
		@return the presolve mask filter.
		*/
		Core::FMTMaskFilter getPresolveFilter(const std::vector<Core::FMTTheme>& originalthemes) const;
		// DocString: FMTModel::getPostsolveFilter
		/**
		@brief Get the postsolve filter used by the presolve, to turn a presolved mask into a postsolve mask.
		@param[in] originalthemes the original themes of the model.
		@param[in] devmask the development mask.
		@return the postsolve mask filter.
		*/
		Core::FMTMaskFilter getPostsolveFilter(const std::vector<Core::FMTTheme>& originalthemes, const Core::FMTMask& devmask) const;
		// DocString: FMTModel::getSelectedMask
		/**
		@brief Get the selection mask needed to presolve a FMTMask when the model is presolved.
		@param[in] originalthemes the original themes of the model.
		@return the selection mask.
		*/
		Core::FMTMask getSelectedMask(const std::vector<Core::FMTTheme>& originalthemes) const;
		// DocString: FMTModel::presolve
		/**
		@brief Return a new presolved FMTModel using developments and the actual transitions of the model.
		@details Can reduce the number of global themes, actions, transitions, yields, lifespans, outputs and constraints if the model is badly formulated.
		@param[in] optionaldevelopments optional developments to presolve on.
		@return a unique pointer to the presolved FMTModel.
		*/
		virtual std::unique_ptr<FMTModel>presolve(std::vector<Core::FMTActualDevelopment> optionaldevelopments = std::vector<Core::FMTActualDevelopment>()) const;
		// DocString: FMTModel::postSolve
		/**
		@brief Postsolve the presolved model into the original model.
		@param[in] originalbasemodel the original base model.
		*/
		virtual void postSolve(const FMTModel& originalbasemodel);
		// DocString: FMTModel::presolveSchedule
		/**
		@brief Return a new schedule with presolved development masks and actions.
		@param[in] originalbaseschedule the original (not presolved) schedule.
		@param[in] originalbasemodel the original (not presolved) model.
		@return the presolved schedule.
		*/
		Core::FMTSchedule presolveSchedule(const Core::FMTSchedule& originalbaseschedule,
			const FMTModel& originalbasemodel) const;
		// DocString: FMTModel::getModelStats
		/**
		@brief Return the statistics of the model such as the number of themes, yields, actions and transitions.
		@return the model statistics.
		*/
		FMTModelStats getModelStats() const;
		// DocString: FMTModel::cleanActionsNTransitions
		/**
		@brief Delete actions that have no defined transition and transitions that have no defined action, then sort them.
		*/
		void cleanActionsNTransitions();
		// DocString: FMTModel::getName
		/**
		@brief Return the name of the FMTModel.
		@return the model name.
		*/
		inline std::string getName() const
		{
			return name;
		}
		// DocString: FMTModel::getThemes
		/**
		@brief Return a copy of the themes of the model.
		@return the model themes.
		*/
		inline std::vector<Core::FMTTheme>getThemes() const
		{
			return themes;
		}
		// DocString: FMTModel::getactions
		/**
		@brief Return a copy of the actions of the model.
		@return the model actions.
		*/
		inline std::vector<Core::FMTAction>getactions() const
		{
			return actions;
		}
		// DocString: FMTModel::getTransitions
		/**
		@brief Return a copy of the transitions of the model.
		@return the model transitions.
		*/
		inline std::vector<Core::FMTTransition>getTransitions() const
		{
			return transitions;
		}
		// DocString: FMTModel::getYields
		/**
		@brief Return a copy of the yields of the model.
		@return the model yields.
		*/
		inline Core::FMTYields getYields() const
		{
			return yields;
		}
		// DocString: FMTModel::getLifespan
		/**
		@brief Return a copy of the lifespan of the model.
		@return the model lifespan.
		*/
		inline Core::FMTLifespans getLifespan() const
		{
			return lifespan;
		}
		// DocString: FMTModel::getOutputs
		/**
		@brief Return a copy of the outputs of the model.
		@return the model outputs.
		*/
		inline std::vector<Core::FMTOutput> getOutputs() const
		{
			return outputs;
		}
		// DocString: FMTModel::getconstraints
		/**
		@brief Return a copy of the constraints of the model.
		@return the model constraints.
		*/
		inline std::vector<Core::FMTConstraint>getconstraints() const
		{
			return constraints;
		}
		// DocString: FMTModel::defaultDeathAction
		/**
		@brief Return the default death action, based on the lifespan and themes, when not specified by the user.
		@param[in] llifespan the lifespan data.
		@param[in] lthemes the model themes.
		@return the default death action.
		*/
		static Core::FMTAction defaultDeathAction(const Core::FMTLifespans& llifespan,
			const std::vector<Core::FMTTheme>& lthemes);
		// DocString: FMTModel::defaultDeathTransition
		/**
		@brief Return the default death transition, based on the lifespan and themes, when not specified by the user.
		@param[in] llifespan the lifespan data.
		@param[in] lthemes the model themes.
		@return the default death transition.
		*/
		static Core::FMTTransition defaultDeathTransition(const Core::FMTLifespans& llifespan,
			const std::vector<Core::FMTTheme>& lthemes);
		// DocString: FMTModel::addOutput
		/**
		@brief Add one output to the model based only on strings.
		@param[in] name the output name.
		@param[in] maskstring the mask string (? ? ?).
		@param[in] outputtarget the type of output created.
		@param[in] action the targeted action string.
		@param[in] yield the targeted yield string.
		@param[in] description the description of the output.
		@param[in] targetTheme the targeted theme index.
		*/
		void addOutput(const std::string& name, const std::string& maskstring, Core::FMTotar outputtarget,
			std::string action = std::string(), std::string yield = std::string(), std::string description = std::string(), int targetTheme = -1);
		// DocString: FMTModel::addYieldHandlers
		/**
		@brief Add FMTYieldHandler from a vector to the yields.
		@details Can only be used for FMTAgeYieldHandler and FMTTimeYieldHandler. FMTModelYieldHandler or FMTcomplexyieldhandler must be added in the .yld file to parse with the model.
		@tparam T the yield handler type.
		@param[in] yieldhandlers the yield handlers to add.
		*/
		template<typename T>
		void addYieldHandlers(const std::vector<T>& yieldhandlers)
		{
			try {
				yields.unShrink(themes);
				for (const auto& yldhandler : yieldhandlers)
				{
					std::unique_ptr<Core::FMTYieldHandler> yldhandlerptr = yldhandler.clone();
					yields.push_back(yldhandlerptr->getMask(), yldhandlerptr);
				}
				yields.update();
			}
			catch (...) {
				_exhandler->printExceptions("", "FMTModel:::addyieldhandlers", __LINE__, __FILE__);
			}
		}
		// DocString: FMTModel::addYieldHandlersFromPtr
		/**
		@brief Add unique pointers of FMTYieldHandler from a vector to the yields.
		@param[in] yieldhandlers the yield handlers to add.
		*/
		void addYieldHandlersFromPtr(const std::vector<std::unique_ptr<Core::FMTYieldHandler>>& yieldhandlers);
		// DocString: FMTModel::operator==
		/**
		@brief Equality comparison operator of FMTModel.
		@param[in] rhs the FMTModel to compare with.
		@return true if both models are equal else false.
		*/
		bool operator == (const FMTModel& rhs) const;
		// DocString: FMTModel::operator<
		/**
		@brief Less than comparison operator of FMTModel.
		@param[in] rhs the FMTModel to compare with.
		@return true if this model is less than rhs else false.
		*/
		bool operator < (const FMTModel& rhs) const;
		// DocString: FMTModel::setArea
		/**
		@brief Setter for the initial actual developments (area section), replacing the originals.
		@details Also checks if the actual development lifespan is shorter than the lock, raising FMTdeathwithlock, and reduces the locks accordingly if the error is set to warning.
		@param[in] ldevs the actual developments to set.
		*/
		void setArea(const std::vector<Core::FMTActualDevelopment>& ldevs);
		// DocString: FMTModel::setThemes
		/**
		@brief Setter for the themes of the model, replacing the originals.
		@param[in] lthemes the themes to set.
		*/
		void setThemes(const std::vector<Core::FMTTheme>& lthemes);
		// DocString: FMTModel::setActions
		/**
		@brief Setter for the actions of the model, replacing the originals.
		@param[in] lactions the actions to set.
		*/
		void setActions(const std::vector<Core::FMTAction>& lactions);
		// DocString: FMTModel::setTransitions
		/**
		@brief Setter for the transitions of the model, replacing the originals.
		@param[in] ltransitions the transitions to set.
		*/
		void setTransitions(const std::vector<Core::FMTTransition>& ltransitions);
		// DocString: FMTModel::setConstraints
		/**
		@brief Setter for the constraints of the model, replacing the original.
		@param[in] lconstraint the constraints to set.
		*/
		void setConstraints(const std::vector<Core::FMTConstraint>& lconstraint);
		// DocString: FMTModel::setYields
		/**
		@brief Setter for the yields of the model, replacing the original.
		@param[in] lylds the yields to set.
		*/
		void setYields(const Core::FMTYields& lylds);
		// DocString: FMTModel::setLifespan
		/**
		@brief Setter for the lifespan of the model, replacing the original.
		@param[in] llifespan the lifespan to set.
		*/
		void setLifespan(const Core::FMTLifespans& llifespan);
		// DocString: FMTModel::setName
		/**
		@brief Set the name of the FMTModel.
		@param[in] newname the new model name.
		*/
		void setName(const std::string& newname);
		// DocString: FMTModel::setOutputs
		/**
		@brief Setter for the outputs of the model.
		@param[in] newoutputs the outputs to set.
		*/
		void setOutputs(const std::vector<Core::FMTOutput>& newoutputs);
		// DocString: FMTModel::isValid
		/**
		@brief Validate the whole FMTModel through themes, actual developments, yields, actions, transitions, outputs and constraints.
		@return true if the model is valid else false.
		*/
		bool isValid();
		// DocString: FMTModel::locateStaticThemes(const Core::FMTOutput, bool ignoreoutputvariables)
		/**
		@brief Return the themes that are not used in the transitions and in a given output (static themes).
		@param[in] output the output to look into.
		@param[in] ignoreoutputvariables if true ignores the output variables.
		@return the static themes.
		*/
		std::vector<const Core::FMTTheme*> locateStaticThemes(const Core::FMTOutput& output, bool ignoreoutputvariables = false) const;
		// DocString: FMTModel::locateStaticThemes(const Core::FMTOutputNode, bool ignoreoutputvariables)
		/**
		@brief Return the themes that are not used in the transitions and in a given output node (static themes).
		@param[in] output the output node to look into.
		@param[in] ignoreoutputvariables if true ignores the output variables.
		@return the static themes.
		*/
		std::vector<const Core::FMTTheme*> locateStaticThemes(const Core::FMTOutputNode& output, bool ignoreoutputvariables = false) const;
		// DocString: FMTModel::locateDynamicThemes
		/**
		@brief Return the themes used in the transition scheme.
		@param[in] outpu the output to look into.
		@param[in] ignoreoutputvariables if true ignores the output variables.
		@return the dynamic themes.
		*/
		std::vector<const Core::FMTTheme*> locateDynamicThemes(const Core::FMTOutput& outpu, bool ignoreoutputvariables = false) const;
		// DocString: FMTModel::getDynamicMask(const Core::FMTOutput, bool ignoreoutputvariables)
		/**
		@brief Return a mask where static attributes have a value of 0 and dynamic attributes a value of 1.
		@param[in] output the output to look into.
		@param[in] ignoreoutputvariables if true ignores the output variables.
		@return the dynamic mask.
		*/
		Core::FMTMask getDynamicMask(const Core::FMTOutput& output, bool ignoreoutputvariables = false) const;
		// DocString: FMTModel::getDynamicMask(const Core::FMTOutputNode, bool ignoreoutputvariables)
		/**
		@brief Return a mask where static attributes have a value of 0 and dynamic attributes a value of 1.
		@param[in] node the output node to look into.
		@param[in] ignoreoutputvariables if true ignores the output variables.
		@return the dynamic mask.
		*/
		Core::FMTMask getDynamicMask(const Core::FMTOutputNode& node, bool ignoreoutputvariables = false) const;
		// DocString: FMTModel::getStaticMask
		/**
		@brief Return a mask where static attributes have a value of 1 and dynamic attributes a value of 0.
		@param[in] node the output node to look into.
		@param[in] ignoreoutputvariables if true ignores the output variables.
		@return the static mask.
		*/
		virtual Core::FMTMask getStaticMask(const Core::FMTOutputNode& node, bool ignoreoutputvariables = false) const;
		// DocString: FMTModel::isStaticNode
		/**
		@brief Return true if the node can be used in static mask functions.
		@details The higher the ratio of set the more likely this function returns true.
		@param[in] node the output node to check.
		@param[in] ratioofset the ratio of set threshold.
		@return true if the node is static else false.
		*/
		bool isStaticNode(const Core::FMTOutputNode& node, double ratioofset = 0.1) const;
		// DocString: FMTModel::locateStaticTransitionsThemes
		/**
		@brief Return the static themes based on the model transitions.
		@return the static transition themes.
		*/
		std::vector<const Core::FMTTheme*>locateStaticTransitionsThemes() const;
		// DocString: FMTModel::locatenotestaticthemes
		/**
		@brief Return the static themes of a node based on the model transitions.
		@param[in] node the output node to look into.
		@param[in] ignoreoutputvariables if true ignores the output variables.
		@param[in] basethemes optional base themes to start from.
		@return the static themes of the node.
		*/
		std::vector<const Core::FMTTheme*>locateNodeStaticThemes(const Core::FMTOutputNode& node,
			bool ignoreoutputvariables = false,
			std::vector<const Core::FMTTheme*> basethemes = std::vector<const Core::FMTTheme*>()) const;
		// DocString: FMTModel(FMTModel)
		/**
		@brief Copy constructor of FMTModel.
		@param[in] rhs the FMTModel to copy.
		*/
		FMTModel(const FMTModel& rhs);
		// DocString: FMTModel::operator=
		/**
		@brief Copy assignment of FMTModel.
		@param[in] rhs the FMTModel to copy.
		@return a reference to this FMTModel.
		*/
		FMTModel& operator = (const FMTModel& rhs);
		// DocString: FMTModel::empty
		/**
		@brief Return true if the FMTModel is empty.
		@return true if the model is empty else false.
		*/
		bool empty() const;
		// DocString: FMTModel::append
		/**
		@brief Append a FMTModel to this FMTModel without overriding the objective of the base model.
		@param[in] rhs the FMTModel to append.
		*/
		void push_back(const FMTModel& rhs);
		// DocString: FMTModel::getInitialArea
		/**
		@brief Return the sum of the area of the area vector.
		@return the total initial area.
		*/
		double getInitialArea() const;
		// DocString: FMTModel::setAreaPeriod
		/**
		@brief Change the initial area period to the targeted period.
		@param[in] period the new area period.
		*/
		void setAreaPeriod(const int& period);
		// DocString: FMTModel::getAreaPeriod
		/**
		@brief get the period of the area section.
		@return the period of the first dev of the area.
		*/
		int getAreaPeriod() const;
		// DocString: FMTModel::getPotentialSchedule
		/**
		@brief Test operability of a selection minus the developments to remove and return a schedule with an area of 1.0 for each operable development.
		@param[in] toRemove the developments to remove from the selection.
		@param[in] selection the developments to test.
		@param[in] withlock if true keeps the locked developments.
		@return a schedule containing the operable developments.
		*/
		Core::FMTSchedule getPotentialSchedule(std::vector<Core::FMTActualDevelopment> toRemove,
			std::vector<Core::FMTActualDevelopment> selection, bool withlock = true) const;
		// DocString: FMTModel::getReplanningConstraints
		/**
		@brief Adjust the constraints that need to be set to a value of the global or local model and return all constraints.
		@param[in] modeltype the type of the model.
		@param[in] localconstraints the local constraints.
		@param[in] period the replanning period.
		@return the adjusted replanning constraints.
		*/
		virtual std::vector<Core::FMTConstraint> getReplanningConstraints(const std::string& modeltype, const std::vector<Core::FMTConstraint>& localconstraints, const int& period) const;
		// DocString: FMTModel::doPlanning
		/**
		@brief Build the model and do the initial solve or simulate, using the build and solve overridden functions of the different model types.
		@param[in] solve if true solves the model, otherwise simulates it.
		@param[in] schedules the schedules used to build the model.
		@return true if the planning succeeded else false.
		*/
		virtual bool doPlanning(const bool& solve, std::vector<Core::FMTSchedule> schedules = std::vector<Core::FMTSchedule>());
		// DocString: FMTModel::getOutput
		/**
		@brief Get the output value of an output for a given period using the spatial solution.
		@details The map key is the output name when level is standard or totalonly, or the development name when level is developpement.
		@param[in] output the output to evaluate.
		@param[in] period the period to evaluate.
		@param[in] level the output level of detail.
		@return a map of output or development names to their values.
		*/
		virtual std::map<std::string, double> getOutput(const Core::FMTOutput& output,
			int period, Core::FMToutputlevel level = Core::FMToutputlevel::standard) const;
		// DocString: FMTModel::getSolution
		/**
		@brief Get the standard solution for a given period, without the natural growth solution.
		@param[in] period the period to get the solution from.
		@param[in] withlock if true the schedule contains the locked developments.
		@return the solution schedule.
		*/
		virtual Core::FMTSchedule getSolution(int period, bool withlock = false) const;
		// DocString: FMTModel::getOutputsFromPeriods
		/**
		@brief For each output get a vector of output values from the first period to the last period.
		@param[in] theoutputs the outputs to evaluate.
		@param[in] firstPeriod the first period.
		@param[in] lastPeriod the last period.
		@param[in] level the output level of detail.
		@return a map of output names to their values per period.
		*/
		virtual std::map<std::string, std::vector<std::vector<double>>>getOutputsFromPeriods(const std::vector<Core::FMTOutput>& theoutputs,
			const int& firstPeriod, const int& lastPeriod, Core::FMToutputlevel level = Core::FMToutputlevel::standard) const;
		// DocString: FMTModel::clone
		/**
		@brief Get a clone of the FMTModel.
		@return a unique pointer to the cloned FMTModel.
		*/
		virtual std::unique_ptr<FMTModel>clone() const;
		// DocString: FMTModel::setParameter(const FMTintmodelparameters,const int&)
		// DocString: FMTNssModel::setParameter
		/**
		@brief set int parameters to nss model.
		@param[in] p_key the int key to change.
		@param[in] p_value to set to the p_key.
		@return true if the parameter is set.
		*/
		virtual bool setParameter(const FMTintmodelparameters& p_key, const int& p_value);
		// DocString: FMTModel::setParameter(const FMTdblmodelparameters,const double)
		/**
		@brief Setter for double model parameters. See FMTModelParameters.h.
		@param[in] key the parameter key.
		@param[in] value the value to set.
		@return true if the parameter is set else false.
		*/
		virtual bool setParameter(const FMTdblmodelparameters& key, const double& value);
		// DocString: FMTModel::setParameter(const FMTboolmodelparameters,const bool)
		/**
		@brief Setter for bool model parameters. See FMTModelParameters.h.
		@param[in] key the parameter key.
		@param[in] value the value to set.
		@return true if the parameter is set else false.
		*/
		virtual bool setParameter(const FMTboolmodelparameters& key, const bool& value);
		// DocString: FMTModel::setParameter(const FMTboolstrparameters,const std::string)
		/**
		@brief Setter forstr model parameters. See FMTModelParameters.h.
		@param[in] p_key the enum key of the parameter.
		@param[in] p_value the value of the parameter.
		@return true if parameter set else false.
		*/
		virtual bool setParameter(const FMTstrmodelparameters& p_key, const std::string& p_value);
		// DocString: FMTModel::getParameter(const FMTintmodelparameters)
		/**
		@brief Getter for int model parameters. See FMTModelParameters.h.
		@param[in] key the parameter key.
		@return the parameter value.
		*/
		int getParameter(const FMTintmodelparameters& key) const;
		// DocString: FMTModel::getParameter(const FMTdblmodelparameters)
		/**
		@brief Getter for double model parameters. See FMTModelParameters.h.
		@param[in] key the parameter key.
		@return the parameter value.
		*/
		double getParameter(const FMTdblmodelparameters& key) const;
		// DocString: FMTModel::getParameter(const FMTboolmodelparameters)
		/**
		@brief Getter for bool model parameters. See FMTModelParameters.h.
		@param[in] key the parameter key.
		@return the parameter value.
		*/
		bool getParameter(const FMTboolmodelparameters& key) const;
		// DocString: FMTModel::getParameter(const FMTstrmodelparameters)
		/**
		@brief Getter for str model parameters. See FMTModelParameters.h.
		@param[in] p_key the parameter key.
		@return the parameter value.
		*/
		const std::string& getParameter(const FMTstrmodelparameters& p_key) const;
		// DocString: FMTModel::setCompressTime
		/**
		@brief Set the compresstime value for a range of periods. See FMTModelParameters.h.
		@param[in] periodStart the first period of the range.
		@param[in] periodStop the last period of the range.
		@param[in] value the compresstime value to set.
		@return true if the value is set else false.
		*/
		bool setCompressTime(const int& periodStart, const int& periodStop, const int& value);
		// DocString: FMTModel::getCompressTime
		/**
		@brief Return a vector with the compresstime value for each period, the first period being index 0. See FMTModelParameters.h.
		@return the compresstime values per period.
		*/
		std::vector<int> getCompressTime() const;
		// DocString: FMTModel::goalConstraints
		/**
		@brief Return all constraints and the objective with goals and penalty.
		@param[in] penalty the penalty applied to the goals.
		@return the goal constraints.
		*/
		std::vector<Core::FMTConstraint>goalConstraints(double penalty = 999999) const;
		// DocString: FMTModel::getTacticalConstraints
		/**
		@brief Get the constraints adapted for a tactical model.
		@param[in] penalty the penalty applied to the goals.
		@param[in] scheduleweight the weight of the schedule.
		@param[in] objective the objective value.
		@param[in] objectivefactor the objective factor.
		@return the tactical constraints.
		*/
		std::vector<Core::FMTConstraint>getTacticalConstraints(double penalty = 999999, double scheduleweight = 999999, double objective = 0, double objectivefactor = 1.0) const;
		// DocString: FMTModel::showParameters
		/**
		@brief Show the parameter values.
		@param[in] showhelp if true shows a brief description of each parameter.
		*/
		virtual void showParameters(const bool& showhelp = false)const;
		// DocString: FMTModel::build
		/**
		@brief Build the elements needed to solve the model, overridden by each type of model.
		@param[in] schedules the schedules used to build the model.
		@return true if the build succeeded else false.
		*/
		virtual bool build(std::vector<Core::FMTSchedule> schedules = std::vector<Core::FMTSchedule>())
		{
			//Mettre message warning
			return false;
		}
		// DocString: FMTModel::solve
		/**
		@brief Solve the model, overridden by each type of model.
		@return true if the solve succeeded else false.
		*/
		virtual bool solve()//Only for optimisation
		{
			//Mettre message warning
			return false;
		}
		// DocString: FMTModel::isOptimal
		/**
		@brief Return true if the FMTModel is optimal.
		@return true if the model is optimal else false.
		*/
		virtual bool isOptimal() const;
		// DocString: FMTModel::getObjectiveValue
		/**
		@brief Return the value of the global objective.
		@return the objective value.
		*/
		virtual double getObjectiveValue() const;
		// DocString: FMTModel::getGeneratorPtr
		/**
		@brief Get a pointer to the generator.
		@return a pointer to the generator or a nullptr;
		*/
		std::default_random_engine* getGeneratorPtr() const;
		// DocString: FMTModel::getStaticTransitionThemes
		/**
		@brief Locate the static transition themes.
		@return the indexes of the static transition themes.
		*/
		std::vector<size_t>getStaticTransitionThemes() const;
    protected:
		// DocString: FMTModel::m_generator
		///Random number generator.
		mutable std::default_random_engine m_generator;
		// DocString: FMTModel::parameters
		///Parameters needed for the function doPlanning by the different types of FMTModel. 
		///See FMTModelParameters for detail description or use FMTModel::showParameters(true) to see your parameters and a brief description.
		FMTModelParameters parameters;
		// DocString: FMTModel::area
		///Actualdevelopments for period 0, seen in the area section or the shapefile/raster
		std::vector<Core::FMTActualDevelopment>area;
		// DocString: FMTModel::themes
		///Model themes of the landscape section file.
		std::vector<Core::FMTTheme>themes;
		// DocString: FMTModel::actions
		///Model actions from the action file and also the _death action
		std::vector<Core::FMTAction>actions;
		// DocString: FMTModel::transitions
		///Model transitions from the transition file and also the _death transition
		std::vector<Core::FMTTransition>transitions;
		// DocString: FMTModel::yields
		///Yields data comming from the yield file
		Core::FMTYields yields;
		// DocString: FMTModel::lifespan
		///lifespan data comming from the lifespan file
		Core::FMTLifespans lifespan;
		// DocString: FMTModel::outputs
		///Outputs comming from the ouput file
		std::vector<Core::FMTOutput> outputs;
		// DocString: FMTModel::constraints
		///Outputs comming from the optimization file
		std::vector<Core::FMTConstraint>constraints;
		// DocString: FMTModel::name
		///The name of the Model (name of the .pri file without extension)
		std::string name;
		// DocString: FMTModel::statictransitionthemes
		///The location of the themes static from transitions
		std::vector<size_t>statictransitionthemes;
		// DocString: FMTModel::setSeed
		/**
		@brief seed the random number generator
		@param[in] p_seed the seed.
		*/
		void setSeed(const int& p_seed);
		// DocString: FMTModel::setDefaultObjects
		/**
		@brief Define the default _DEATH action and transition when they are not defined by the user.
		*/
		void setDefaultObjects();
		// DocString: FMTModel::getBaseMask
		/**
		@brief Return a mask covering the actual bits used by the FMTModel and the optional developments.
		@param[in] optionaldevelopments the optional developments to cover.
		@return the base mask.
		*/
		Core::FMTMask getBaseMask(std::vector<Core::FMTActualDevelopment> optionaldevelopments) const;
		
		// DocString: FMTModel::getStaticPresolveThemes
		/**
		@brief Return the themes that are not part of the model, using the transition themes, constraints, actions and yields.
		@return the static presolve themes.
		*/
		std::vector<const Core::FMTTheme*>getStaticPresolveThemes() const;
		// DocString: FMTModel::clearActionsCache
		/**
		@brief Clear the cached actions data.
		*/
		void clearActionsCache();
		// DocString: FMTModel::clearYieldCache
		/**
		@brief Clear the cached yields data.
		*/
		void clearYieldCache();
		// DocString: FMTModel::clearTransitionCache
		/**
		@brief Clear the cached transitions data.
		*/
		void clearTransitionCache();
		// DocString: FMTModel::swapPtr
		/**
		@brief Swap this model for the element at the end of the rhs unique pointer.
		@param[in,out] rhs the unique pointer to swap with.
		*/
		virtual void swapPtr(std::unique_ptr<FMTModel>& rhs);
		// DocString: FMTModel::setUpSchedulesForBuild
		/**
		@brief Prepare the passed schedules for building based on FORCE_PARTIAL_BUILD, always returning a filled vector covering the whole length.
		@param[in] schedules the schedules to prepare.
		@return the prepared schedules.
		*/
		std::vector<Core::FMTSchedule>setUpSchedulesForBuild(const std::vector<Core::FMTSchedule>& schedules) const;
		// DocString: FMTModel::useActionSerie
		/**
		@brief Return true if the model makes use of action series.
		@return true if the model uses action series else false.
		*/
		bool useActionSerie() const;
		// DocString: FMTModel::getSeriesMaxSize
		/**
		@brief Get the maximal serie size for the whole model.
		@return the maximal serie size.
		*/
		size_t getSeriesMaxSize() const;

	private:
		// DocString: FMTModel::save
		/**
		@brief Save function used for serialization to do multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to save to.
		@param[in] version the serialization version.
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void save(Archive& ar, const unsigned int version) const
		{
			ar& boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTObject>(*this));
			FMTObject::forceSave(ar, version);
			std::stringstream basegenerator;
			basegenerator << m_generator;
			const std::string baseGeneratorString(basegenerator.str());
			ar& BOOST_SERIALIZATION_NVP(baseGeneratorString);
			ar& BOOST_SERIALIZATION_NVP(parameters);
			ar& BOOST_SERIALIZATION_NVP(area);
			ar& BOOST_SERIALIZATION_NVP(themes);
			ar& BOOST_SERIALIZATION_NVP(actions);
			ar& BOOST_SERIALIZATION_NVP(transitions);
			ar& BOOST_SERIALIZATION_NVP(yields);
			ar& BOOST_SERIALIZATION_NVP(lifespan);
			ar& BOOST_SERIALIZATION_NVP(outputs);
			ar& BOOST_SERIALIZATION_NVP(constraints);
			ar& BOOST_SERIALIZATION_NVP(name);
		}
		// DocString: FMTModel::load
		/**
		@brief Load function used for serialization to do multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to load from.
		@param[in] version the serialization version.
		*/
		template<class Archive>
		void load(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTObject>(*this));
			FMTObject::forceLoad(ar, version);//get the object information for the global object
			std::string baseGeneratorString;
			ar& BOOST_SERIALIZATION_NVP(baseGeneratorString);
			std::stringstream(baseGeneratorString) >> m_generator;
			ar& BOOST_SERIALIZATION_NVP(parameters);
			ar& BOOST_SERIALIZATION_NVP(area);
			ar& BOOST_SERIALIZATION_NVP(themes);
			ar& BOOST_SERIALIZATION_NVP(actions);
			ar& BOOST_SERIALIZATION_NVP(transitions);
			ar& BOOST_SERIALIZATION_NVP(yields);
			ar& BOOST_SERIALIZATION_NVP(lifespan);
			ar& BOOST_SERIALIZATION_NVP(outputs);
			ar& BOOST_SERIALIZATION_NVP(constraints);
			ar& BOOST_SERIALIZATION_NVP(name);
			//this->passinobject(*this);//Distribute global object to local object
		}
		BOOST_SERIALIZATION_SPLIT_MEMBER()
			// DocString: FMTModel::validateListMasks
			/**
			@brief Validate the FMTMask of the elements in a FMTList such as FMTAction or FMTTransition.
			@details Throws an exception if a FMTMask string does not match the themes.
			@tparam T the list element type.
			@param[in] container the list to validate.
			*/
			template<class T>
		void validateListMasks(const Core::FMTList<T>& container) const
		{
			try {
				for (const auto& listobject : container)
				{
					std::string name = std::string(listobject.first);
					Core::FMTTheme::validate(themes, name);
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTModel::validateListMasks", __LINE__, __FILE__);
			}
		}
		// DocString: FMTModel::validateListSpec
		/**
		@brief Validate a specification by checking the yields used.
		@details Throws an exception if a yield used in the specification is not defined in the yields.
		@param[in] specifier the specification to validate.
		*/
		void validateListSpec(const Core::FMTSpec& specifier) const;
		// DocString: FMTModel::aggregateActions
		/**
		@brief Aggregate actions together on this model, calling setActions.
		@param[in] p_ActionsMapping the action aggregates you want to put together.
		@return the mask filter map to apply to any output, with the old action name as key and a pair of the new action name and FMTMask filter as value.
		*/
		std::map<std::string, std::pair<std::string,Core::FMTMask>> aggregateActions(std::vector<std::string> p_ActionsMapping);
		// DocString: FMTModel::aggregateTransitions
		/**
		@brief Aggregate transitions together on this model, calling setTransitions.
		@param[in] p_Filters the filters of the actions.
		*/
		void aggregateTransitions(const std::map<std::string, std::pair<std::string, Core::FMTMask>>& p_Filters);
		// DocString: FMTModel::aggregateOutputs
		/**
		@brief Change outputs by applying a filter to mask outputs that use aggregated actions and rename the action to the newly aggregated one.
		@param[in] p_Filters the filter generated by the action aggregation.
		@param[in,out] p_Outputs the outputs to modify.
		@param[in] p_ActionOrdering the hierarchy of the actions.
		*/
		void aggregateOutputs(const std::map<std::string, std::pair<std::string, Core::FMTMask>>& p_Filters,
			std::vector<Core::FMTOutput*>& p_Outputs, const std::vector<std::string>& p_ActionOrdering);
		// DocString: FMTModel::addNewMask
		/**
		@brief Add the aggregates needed in the themes to produce a valid FMTMask with a complete name from a non string mapped mask.
		@param[in] p_incompleteMask the mask that does not have a complete name, just bits.
		@return a valid mask with the right name and the newly created aggregates.
		*/
		Core::FMTMask addNewMask(const Core::FMTMask& p_incompleteMask);
		// DocString: FMTModel::extendSchedule
		/**
		@brief Extend the schedule developments with the actual model which uses more themes.
		@param[in] p_schedules the schedules we want to extend.
		@return a complete schedule that fits the model.
		*/
		std::vector<Core::FMTSchedule> extendSchedule(const std::vector<Core::FMTSchedule>& p_schedules) const;
		// DocString: FMTModel::_gutsOfConstructor
		/**
		@brief The guts of the base constructor.
		@param[in] p_area the base area.
		*/
		void _gutsOfConstructor(const std::vector<Core::FMTActualDevelopment>& p_area);
		// DocString: FMTModel::_cleanVector
		/**
		@brief Clean a vector from the back to the front.
		@tparam T the vector element type.
		@param[in,out] p_toClean the vector to clean.
		@param[in] p_whichtoKeep the entries to keep.
		*/
		template<class T>
		static void _cleanVector(std::vector<T>& p_toClean, const std::vector<bool>& p_whichtoKeep)
			{
			size_t Id = p_toClean.size();
			while (Id != 0)
				{
				--Id;
				if (!p_whichtoKeep[Id])
					{
					p_toClean.erase(p_toClean.begin() + Id);
					}
				}
			}
		// DocString: FMTModel::_getYieldsStraticAggregates
		/**
		@brief Get the potential new theme attributes for a yield.
		@param[in] p_yieldName the yield name.
		@return the static aggregates split by -.
		*/
		std::set<std::string>_getYieldsStraticAggregates(const std::string& p_yieldName) const;
		// DocString: FMTModel::_getAggregatesWrap
		/**
		@brief Wrap aggregates together with a - separator.
		@param[in] p_mask the mask to check for aggregates.
		@param[in] p_themes the themes to check for.
		@return the wrapped aggregates.
		*/
		std::string _getAggregatesWrap(const Core::FMTMask& p_mask,
			const std::vector<size_t>& p_themes) const;
		// DocString: FMTModel::_getAggregatesThemes
		/**
		@brief Select the yields containing p_yieldName and return the theme id of the aggregates in the mask.
		@param[in] p_yieldName the yield to get.
		@return the ids of the themes.
		*/
		std::vector<size_t> _getAggregatesThemes(const std::string& p_yieldName) const;
		// DocString: FMTModel::_getYieldAttribute
		/**
		@brief Get the yield mask attribute for a given development containing p_yieldName.
		@param[in] p_devMask the development mask.
		@param[in] p_yieldName the yield to get.
		@param[in] p_AggregatedThemes the aggregated themes.
		@return the yield attribute.
		*/
		std::string _getYieldAttribute(const Core::FMTMask& p_devMask,
			const std::string& p_yieldName, const std::vector<size_t>& p_AggregatedThemes) const;
    };
// DocString: FMTModelComparator
/**
@brief Comparator used to compare FMTModel with std::find_if when models are in a stl container.
*/
class FMTModelComparator
{
	// DocString: FMTModelComparator::model_name
	///name of the FMTModel (name membmer of FMTModel)
	std::string model_name;
public:
	// DocString: FMTModelComparator(std::string)
	/**
	@brief Constructor of the comparator.
	@param[in] name the name of the model to check for in the stl container.
	*/
	FMTModelComparator(std::string name);
	// DocString: FMTModelComparator::operator()
	/**
	@brief Return true if the given model matches the name held by the comparator.
	@param[in] model the model to compare.
	@return true if the model name matches else false.
	*/
	bool operator()(const FMTModel& model) const;

};

}

BOOST_CLASS_EXPORT_KEY(Models::FMTModel)

#endif // FMTMODEL_Hm_included
