/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTMODEL_Hm_included
#define FMTMODEL_Hm_included

#include "FMTtheme.h"
#include "FMTaction.h"
#include "FMTtransition.h"
#include "FMTyields.h"
#include "FMTlifespans.h"
#include "FMTactualdevelopment.h"
#include "FMToutput.h"
#include "FMTconstraint.h"
#include "FMTlist.hpp"
#include "FMTmodelparameters.h"
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/export.hpp>
#include "FMToutputnode.h"
#include "FMTschedule.h"
#include <random>


namespace Graph
{
	template<class T1, class T2>
	class FMTgraph;
	class FMTlinegraph;
	class FMTgraphvertextoyield;
}

namespace Parser
{
	class FMTModelParser;
}

namespace Spatial
{
	class FMTSpatialSchedule;
	class FMTspatialnodescache;
	class FMTSpatialGraphs;
}
/// Namespace containing all enumerator and classes used to construct or manipulate the different type of model in FMT.
namespace Models
{
	class FMTModelStats;
	// DocString: FMTModel
	/**
	FMTModel is the base class of multiple models in FMT
	It' is the formulation has seen in the Woodstock files of a forest planning problem.
	FMTLpModel / FMTSaModel / FMTSesModel use the FMTModel protected member to
	generate the problem based on LP/SA or to simply simulate the FMTModel.
	*/

	class FMTEXPORT FMTModel : public Core::FMTObject
    {
		template<class T1,class T2>
		friend class Graph::FMTgraph;
		friend class Graph::FMTlinegraph;
		friend class Spatial::FMTSpatialSchedule;
		friend class Spatial::FMTspatialnodescache;
		friend class Spatial::FMTSpatialGraphs;
		friend class Parser::FMTModelParser;
		friend class Graph::FMTgraphvertextoyield;
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
		@brief check if set reignore needs to be call.
		@return true if got ignore else false.
		*/
		bool gotReIgnore(const int& p_replanningPeriod) const;
		// DocString: FMTModel::gotReplicate
		/**
		@brief check if setReplicate needs to be call.
		@return true if got replicate else false.
		*/
		bool gotReplicate(const int& p_replanningPeriod) const;
		// DocString: FMTModel::buildAction
		/**
		 * @brief Build an action based on the positive value of a yield
		 * @param[in] p_actionName the name of the action to create
		 * @param[in] p_Targetyield the yield to look for positive value.
		 * @return a Valid FMTModel.
		 */
		FMTModel buildAction(const std::string& p_actionName,
							const std::string& p_Targetyield) const;
		// DocString: FMTModel::buildSchedule
		/**
		 * @brief Build schedule by looking at operability of the actual devs.
		 * @param[in] p_action the action we want to build a schedule
		 * @param[in] p_yield names
		 * @param[in] p_BaseModel model
		 * @param[in] p_schedules the non extended schedule we want to build on.
		 * @return a complete schedule of length of the model with actual dev
		 */
		std::vector<Core::FMTSchedule> buildSchedule(const Core::FMTAction& p_action,
													const FMTModel& p_BaseModel,
													const std::string& p_Targetyield,
													const std::vector<Core::FMTSchedule>& p_schedules) const;
		
		// DocString: FMTModel::pushTheme
		/**
		 * @brief Push a new theme and update the whole model with it
		 * @param[in] p_themeName the Name of the theme to push
		 * @param[in] p_targetYield yield used for SBW
		 * @param[in] p_attributes the attributes of the theme ? will be the default of every masks.
		 */
		void pushTheme(const std::string& p_themeName,
			const std::string& p_yieldName,
			const std::vector<std::string>& p_attributes);
		// DocString: FMTModel::aggregateAllActions
		/**
		 * @brief Aggregate actions together and create en enterly new model with new
		 *	-actions,transitions,outputs,constraints,yields,lifespan
		 * @param[in] p_Aggregates the actions aggregate we want to merge together
		 * @param[in] p_ActionOrdering the order in which we want to consider the hierarchy of actions if empty will use the order of the actual actions
		 * @return newly created model with aggregated actions.
		 */
		Models::FMTModel aggregateAllActions(const std::vector<std::string>& p_Aggregates,
											std::vector<std::string> p_ActionOrdering) const;
		// DocString: FMTModel::splitActions
		/**
		 * @brief Split Actions using the masks provided. All new actions will have the action_mask as name
		 * The default one will avec action_default and they will be aggregated into the original action name
		 * @param[in] p_Actions The actions that you want to split
		 * @param[in] p_masks the mask you want to use to split the actions
		 * @return newly created model with splitted actions.
		 */
		Models::FMTModel splitActions(const std::vector<std::string>& p_Actions,
										const std::vector<std::string>& p_masks) const;
		// DocString: FMTSchedule::getPriorities
		/**
		@brief return the actions name in priority of area scheduled.
		@return a vector of action names.
		*/
		std::vector<std::string>getSchedulesPriorities(const std::vector<Core::FMTSchedule>& p_schedules) const;
		// DocString: FMTModel::aggregateSchedules
		/**
		 * @brief Build new schedule based on p_schedules with the corresponding aggregated actions
		 * @param[in] p_schedules the schedules of the original model.
		 * @return newly created schedules with aggregated actions.
		 */
		std::vector<Core::FMTSchedule> aggregateSchedules(const std::vector<Core::FMTSchedule>& p_schedules) const;
		// DocString: FMTModel::splitSchedules
		/**
		 * @brief Using the actions of the model change the actions of the schedule
		 * @param[in] p_schedules the schedules of the original model.
		 * @return newly created schedules with splitted actions.
		 */
		std::vector<Core::FMTSchedule> splitSchedules(const std::vector<Core::FMTSchedule>& p_schedules) const;
		// DocString: FMTModel::setParallelLogger
		/**
		Solver's logger cannot work in parallel so you need to pass a logger owned
		by the thead to the solver to make sure it does not work in concurrency.
		*/
		virtual void setParallelLogger(Logging::FMTLogger& logger);
		// DocString: FMTModel::clearCache
		/**
		Clear caching element to reduce memory usage.
		*/
		virtual void clearCache();
		// DocString: FMTModel(const std::vector<Core::FMTActualDevelopment>& larea, const std::vector<Core::FMTTheme>, const std::vector<Core::FMTAction>, const std::vector<Core::FMTTransition>, const Core::FMTYields, const Core::FMTLifespans, const std::string, const std::vector<Core::FMTOutput>, std::vector<Core::FMTConstraint>,FMTModelParameters)
		/**
		Main constructor for FMTModel used in Parser::FMTModelParser, the constraints and parameters are optional.
		For the FMTSesModel no constraints are needed.
		*/
		FMTModel(const std::vector<Core::FMTActualDevelopment>& larea, const std::vector<Core::FMTTheme>& lthemes,
			const std::vector<Core::FMTAction>& lactions,
			const std::vector<Core::FMTTransition>& ltransitions, const Core::FMTYields& lyields, const Core::FMTLifespans& llifespan,
			const std::string& lname, const std::vector<Core::FMTOutput>& loutputs, std::vector<Core::FMTConstraint> lconstraints = std::vector<Core::FMTConstraint>(),
			FMTModelParameters lparameters = FMTModelParameters());
		// DocString: FMTModel()
		/**
		Default constructor of FMTModel.
		*/
		FMTModel();
		// DocString: ~FMTModel
		/**
			Default virtual desctructor of FMTModel.
		*/
		virtual ~FMTModel() = default;
		// DocString: FMTModel(FMTModel&&)
		/**
		Default move constructor for FMTModel.
		*/
		FMTModel(FMTModel&& rhs);
		// DocString: FMTModel::operator=(FMTModel&&) 
		/**
		Default move assignment for FMTModel.
		*/
		FMTModel& operator =(FMTModel&& rhs);
		// DocString: FMTModel::getArea
		/**
			Virtual function to get the area of a given period into actualdevelopment. If before grow = true it will returns the development before it grow and death to the next period.
		*/
		virtual std::vector<Core::FMTActualDevelopment>getArea(int period = 0, bool beforegrowanddeath = false) const;
		// DocString: FMTModel::getCopy
		/**
		This function returns a copy of the FMTModel of the selected period.
		The function is going to clean the FMTconstraints and keep the objective.
		*/
		virtual std::unique_ptr<FMTModel> getCopy(int period = 0) const;
		// DocString: FMTModel::basePresolve
		/**
		This function use the existin area and call the presolve function
		The function can reduce the number of global themes/actions/transitions/yields/lifespans/outputs/constraints data
		if the model is badly formulated.
		*/
		FMTModel basePresolve() const;
		// DocString: FMTModel::getPresolveFilter
		/**
		Get the presolve filter.
		*/
		Core::FMTMaskFilter getPresolveFilter(const std::vector<Core::FMTTheme>& originalthemes) const;
		// DocString: FMTModel::getPostsolveFilter
		/**
		Get the postSolve filter used by the presolve (in case you want to turn presolved mask into postSolve mask).
		*/
		Core::FMTMaskFilter getPostsolveFilter(const std::vector<Core::FMTTheme>& originalthemes, const Core::FMTMask& devmask) const;
		// DocString: FMTModel::getSelectedMask
		/**
		If the model is presolved you can call this function to get the selection mask that you need
		to use to presolve a FMTMask.
		*/
		Core::FMTMask getSelectedMask(const std::vector<Core::FMTTheme>& originalthemes) const;
		// DocString: FMTModel::presolve
		/**
		This function use a vector of developments and the actual transitions of the model and return new presolved FMTModel.
		The function can reduce the number of global themes/actions/transitions/yields/lifespans/outputs/constraints data
		if the model is badly formulated.
		*/
		virtual std::unique_ptr<FMTModel>presolve(std::vector<Core::FMTActualDevelopment> optionaldevelopments = std::vector<Core::FMTActualDevelopment>()) const;
		// DocString: FMTModel::postSolve
		/*
		This function is for postsolving the presolved model into the original model.
		*/
		virtual void postSolve(const FMTModel& originalbasemodel);
		// DocString: FMTModel::presolveSchedule
		/**
		Using the original schedule (not presolved) and the original model (not presolved) it returns a new schedule with
		presolved FMTdevelopments masks and actions.
		*/
		Core::FMTSchedule presolveSchedule(const Core::FMTSchedule& originalbaseschedule,
			const FMTModel& originalbasemodel) const;
		// DocString: FMTModel::getModelStats
		/**
		Return the statistics of the model, the number of themes, yields, actions, transitions etc...
		*/
		FMTModelStats getModelStats() const;
		// DocString: FMTModel::cleanActionsNTransitions
		/**
		Function do delete action that have no defined transition and to delete transition that have no defined action.
		Actions and transitions are then sorted.
		*/
		void cleanActionsNTransitions();
		// DocString: FMTModel::getName
		/**
		Returns the name of the FMTModel
		*/
		inline std::string getName() const
		{
			return name;
		}
		// DocString: FMTModel::getThemes
		/**
		Getter returning a copy of the FMTthemes vector<> of the model.
		*/
		inline std::vector<Core::FMTTheme>getThemes() const
		{
			return themes;
		}
		// DocString: FMTModel::getactions
		/**
		Getter returning a copy of the FMTactions vector<> of the model.
		*/
		inline std::vector<Core::FMTAction>getactions() const
		{
			return actions;
		}
		// DocString: FMTModel::getTransitions
		/**
		Getter returning a copy of the FMTtransitions vector<> of the model.
		*/
		inline std::vector<Core::FMTTransition>getTransitions() const
		{
			return transitions;
		}
		// DocString: FMTModel::getYields
		/**
		Getter returning a copy of the FMTYields data of the model.
		*/
		inline Core::FMTYields getYields() const
		{
			return yields;
		}
		// DocString: FMTModel::getLifespan
		/**
		Getter returning a copy of the FMTlifespan data of the model.
		*/
		inline Core::FMTLifespans getLifespan() const
		{
			return lifespan;
		}
		// DocString: FMTModel::getOutputs
		/**
		Getter returning a copy of the FMToutputs vector<> of the model.
		*/
		inline std::vector<Core::FMTOutput> getOutputs() const
		{
			return outputs;
		}
		// DocString: FMTModel::getconstraints
		/**
		Getter returning a copy of the FMTconstraints vector<> of the model.
		*/
		inline std::vector<Core::FMTConstraint>getconstraints() const
		{
			return constraints;
		}
		// DocString: FMTModel::defaultDeathAction
		/**
		Returns the default death action when not specified by the user, base on specific lifespan and themes.
		*/
		static Core::FMTAction defaultDeathAction(const Core::FMTLifespans& llifespan,
			const std::vector<Core::FMTTheme>& lthemes);
		// DocString: FMTModel::defaultDeathTransition
		/**
		Returns the default death transition when not specified by the user, base on specific lifespan and themes.
		*/
		static Core::FMTTransition defaultDeathTransition(const Core::FMTLifespans& llifespan,
			const std::vector<Core::FMTTheme>& lthemes);
		// DocString: FMTModel::addOutput
		/**
		Change the function to accept a FMTouptut********
		Adds one output to the model base only on strings.
			name = output name
			maskstring = string of the mask "? ? ?"
			outputtarget = type of ouput created
			action = action string targeted
			yield = yield string targeted
			description = description of the output
		*/
		void addOutput(const std::string& name, const std::string& maskstring, Core::FMTotar outputtarget,
			std::string action = std::string(), std::string yield = std::string(), std::string description = std::string(), int targetTheme = -1);
		// DocString: FMTModel::addYieldHandlers
		/**
		Add FMTYieldHandler from vector (yieldhandlers) to yields. Can only be use for FMTAgeYieldHandler and FMTTimeYieldHandler. If you want
		to add FMTModelYieldHandler or FMTcomplexyieldhander it must be added in the .yld file to parse with the model.
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
		Add unique pointer of FMTYieldHandler from vector (yieldhandlers) to yields.
		*/
		void addYieldHandlersFromPtr(const std::vector<std::unique_ptr<Core::FMTYieldHandler>>& yieldhandlers);
		// DocString: FMTModel::operator==
		/**
		Comparison operator of FMTLpModel
		*/
		bool operator == (const FMTModel& rhs) const;
		// DocString: FMTModel::operator<
		/**
		less than operator of FMTLpModel
		*/
		bool operator < (const FMTModel& rhs) const;
		// DocString: FMTModel::setArea
		/**
		Setter for initial FMTActualDevelopment (area section) will replace the originals.
		This function also check if the actualdevelopment lifespan is shorter than the lock.
		If not so, it raise the error FMTdeathwithlock and if the error is set to warning,
		the locks of the FMTactualdevelpement will be reduced accordingly.
		*/
		void setArea(const std::vector<Core::FMTActualDevelopment>& ldevs);
		// DocString: FMTModel::setThemes
		/**
		Setter for the FMTthemes of the model will replace the originals.
		*/
		void setThemes(const std::vector<Core::FMTTheme>& lthemes);
		// DocString: FMTModel::setActions
		/**
		Setter for the FMTactions of the model will replace the originals.
		*/
		void setActions(const std::vector<Core::FMTAction>& lactions);
		// DocString: FMTModel::setTransitions
		/**
		Setter for the FMTtransitions of the model will replace the originals.
		*/
		void setTransitions(const std::vector<Core::FMTTransition>& ltransitions);
		// DocString: FMTModel::setConstraints
		/**
		Setter for the FMTconstraints of the model will replace the original.
		*/
		void setConstraints(const std::vector<Core::FMTConstraint>& lconstraint);
		// DocString: FMTModel::setYields
		/**
		Setter for the FMTYields data of the model will replace the original.
		*/
		void setYields(const Core::FMTYields& lylds);
		// DocString: FMTModel::setLifespan
		/**
		Setter for the FMTLifespans data of the model will replace the original.
		*/
		void setLifespan(const Core::FMTLifespans& llifespan);
		// DocString: FMTModel::setName
		/**
		Set the names of the FMTModel
		*/
		void setName(const std::string& newname);
		// DocString: FMTModel::setOutputs
		/**
		Setter for the model's outputs.
		*/
		void setOutputs(const std::vector<Core::FMTOutput>& newoutputs);
		// DocString: FMTModel::isValid
		/**
		This function validate all the FMTModel
		FMTTheme ->FMTactualdevelopments-> FMTYields -> FMTAction -> FMTTransition -> FMTOutput -> FMTConstraint
		return true if the model is valid.
		*/
		bool isValid();
		// DocString: FMTModel::locateStaticThemes(const Core::FMTOutput, bool ignoreoutputvariables)
		/**
		Based on the FMTModel looks at every theme to located in the FMTtransitions and return themes
		that are not used into the transition and in a given output (staticthemes)... will return themes based on spatial units.
		*/
		std::vector<const Core::FMTTheme*> locateStaticThemes(const Core::FMTOutput& output, bool ignoreoutputvariables = false) const;
		// DocString: FMTModel::locateStaticThemes(const Core::FMTOutputNode, bool ignoreoutputvariables)
		/**
		Based on the FMTModel looks at every theme to located in the FMTtransitions and return themes
		that are not used into the transition and in a given output node (staticthemes)... will return themes based on spatial units.
		*/
		std::vector<const Core::FMTTheme*> locateStaticThemes(const Core::FMTOutputNode& output, bool ignoreoutputvariables = false) const;
		// DocString: FMTModel::locateDynamicThemes
		/**
		Returns themes used in the transition scheme.
		*/
		std::vector<const Core::FMTTheme*> locateDynamicThemes(const Core::FMTOutput& outpu, bool ignoreoutputvariables = false) const;
		// DocString: FMTModel::getDynamicMask(const Core::FMTOutput, bool ignoreoutputvariables)
		/**
		Based on static and dynamic themes will return a mask where the static attributes will have a value of 0 and dynamic value a value of 1.
		*/
		Core::FMTMask getDynamicMask(const Core::FMTOutput& output, bool ignoreoutputvariables = false) const;
		// DocString: FMTModel::getDynamicMask(const Core::FMTOutputNode, bool ignoreoutputvariables)
		/**
		Based on static and dynamic themes will return a mask where the static attributes will have a value of 0 and dynamic value a value of 1.
		*/
		Core::FMTMask getDynamicMask(const Core::FMTOutputNode& node, bool ignoreoutputvariables = false) const;
		// DocString: FMTModel::getStaticMask
		/**
		Based on static and dynamic themes will return a mask where the static attributes will have a value of 1 and dynamic value a value of 0.
		*/
		virtual Core::FMTMask getStaticMask(const Core::FMTOutputNode& node, bool ignoreoutputvariables = false) const;
		// DocString: FMTModel::isStaticNode
		/**
		Based on static and dynamic themes will return true if the node can be used in staticmask fonctions.
		More that ratio of set is high the more you will get true from this fonction.
		*/
		bool isStaticNode(const Core::FMTOutputNode& node, double ratioofset = 0.1) const;
		// DocString: FMTModel::locateStaticTransitionsThemes
		/**
		Returns the static themes based on model transitions.
		*/
		std::vector<const Core::FMTTheme*>locateStaticTransitionsThemes() const;
		// DocString: FMTModel::locatenotestaticthemes
		/**
		Returns the static themes based on model transitions.
		*/
		std::vector<const Core::FMTTheme*>locateNodeStaticThemes(const Core::FMTOutputNode& node,
			bool ignoreoutputvariables = false,
			std::vector<const Core::FMTTheme*> basethemes = std::vector<const Core::FMTTheme*>()) const;
		// DocString: FMTModel(FMTModel)
		/**
		Copy constructor of FMTModel
		*/
		FMTModel(const FMTModel& rhs);
		// DocString: FMTModel::operator=
		/**
		Copy assignment of FMTModel
		*/
		FMTModel& operator = (const FMTModel& rhs);
		// DocString: FMTModel::empty
		/**
		Returns true if the FMTModel is empty.
		*/
		bool empty() const;
		// DocString: FMTModel::append
		/**
		This function append a FMTModel to an FMTModel.
		it does not override the objective of the base FMTModel.
		*/
		void push_back(const FMTModel& rhs);
		// DocString: FMTModel::getInitialArea
		/**
		Returns the sum of the area of the area vector.
		*/
		double getInitialArea() const;
		// DocString: FMTModel::setAreaPeriod
		/**
		Change to initial area period to the new targeted period.
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
		The function will remove (toRemove) developments from a selected vector (selection). Test operability of the
		difference and return a schedule containing an area of 1.0 if the development is operable to this actions.
		*/
		Core::FMTSchedule getPotentialSchedule(std::vector<Core::FMTActualDevelopment> toRemove,
			std::vector<Core::FMTActualDevelopment> selection, bool withlock = true) const;
		// DocString: FMTModel::getReplanningConstraints
		/**
		Ajust constraints that need to be set to a value (getOutput) of the global or local model and return all constraint.
		*/
		virtual std::vector<Core::FMTConstraint> getReplanningConstraints(const std::string& modeltype, const std::vector<Core::FMTConstraint>& localconstraints, const int& period) const;
		// DocString: FMTModel::doPlanning
		/**
		Build the model and do the initialSolve or simulate. This function use the build and solve overrided functions of the differents model type.
		*/
		virtual bool doPlanning(const bool& solve, std::vector<Core::FMTSchedule> schedules = std::vector<Core::FMTSchedule>());
		// DocString: FMTModel::getOutput
		/**
		Get the output value of a output for a given period using the spatial solution.
		the map key returned consist of output name
		if level == FMToutputlevel::standard || level == FMToutputlevel::totalonly,
		or development name if level == FMToutputlevel::developpement
		*/
		virtual std::map<std::string, double> getOutput(const Core::FMTOutput& output,
			int period, Core::FMToutputlevel level = Core::FMToutputlevel::standard) const;
		// DocString: FMTModel::getSolution
		/**
		Get the standard solution for a given period (FMTSchedule dont have natural growth solution included).
		If with lock is true then the schedule will contain locked development.
		*/
		virtual Core::FMTSchedule getSolution(int period, bool withlock = false) const;
		// DocString: FMTModel::getOutputsFromPeriods
		/**
		For each output get a vector of outputs values starting for first period to last period.
		*/
		virtual std::map<std::string, std::vector<std::vector<double>>>getOutputsFromPeriods(const std::vector<Core::FMTOutput>& theoutputs,
			const int& firstPeriod, const int& lastPeriod, Core::FMToutputlevel level = Core::FMToutputlevel::standard) const;
		// DocString: FMTModel::clone
		/**
		Get a clone of the FMTModel
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
		Setter for double model parameters. See FMTModelParameters.h.
		*/
		virtual bool setParameter(const FMTdblmodelparameters& key, const double& value);
		// DocString: FMTModel::setParameter(const FMTboolmodelparameters,const bool)
		/**
		Setter for bool model parameters. See FMTModelParameters.h.
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
		Getter for int model parameters. Return the parameters value. See FMTModelParameters.h.
		*/
		int getParameter(const FMTintmodelparameters& key) const;
		// DocString: FMTModel::getParameter(const FMTdblmodelparameters)
		/**
		Getter for double model parameters. Return the parameters value. See FMTModelParameters.h.
		*/
		double getParameter(const FMTdblmodelparameters& key) const;
		// DocString: FMTModel::getParameter(const FMTboolmodelparameters)
		/**
		Getter for bool model parameters. Return the parameters value. See FMTModelParameters.h.
		*/
		bool getParameter(const FMTboolmodelparameters& key) const;
		// DocString: FMTModel::getParameter(const FMTstrmodelparameters)
		/**
		@brief Getter for str model parameters. Return the parameters value. See FMTModelParameters.h.
		@param[in] p_key the param enum key.
		*/
		const std::string& getParameter(const FMTstrmodelparameters& p_key) const;
		// DocString: FMTModel::setCompressTime
		/**
		Set compresstime value for a range of periods (periodStart,periodStop). See FMTModelParameters.h.
		*/
		bool setCompressTime(const int& periodStart, const int& periodStop, const int& value);
		// DocString: FMTModel::getCompressTime
		/**
		Return avec vector with compresstime value for each periods. First period is the index 0 in the vector. See FMTModelParameters.h.
		*/
		std::vector<int> getCompressTime() const;
		// DocString: FMTModel::goalConstraints
		/**
		Return all constraints and objective with goals and penalty
		*/
		std::vector<Core::FMTConstraint>goalConstraints(double penalty = 999999) const;
		// DocString: FMTModel::getTacticalConstraints
		/**
		Get the constraints adapted for a tactical model.
		*/
		std::vector<Core::FMTConstraint>getTacticalConstraints(double penalty = 999999, double scheduleweight = 999999, double objective = 0, double objectivefactor = 1.0) const;
		// DocString: FMTModel::showParameters
		/**
		Show parameters values. If (showhelp) a brief description of each parameters will appear.
		*/
		virtual void showParameters(const bool& showhelp = false)const;
		// DocString: FMTModel::build
		/**
		Virtual function overrided all type of models. In child class, this function is use to build the elements needed to solve the model.
		*/
		virtual bool build(std::vector<Core::FMTSchedule> schedules = std::vector<Core::FMTSchedule>())
		{
			//Mettre message warning
			return false;
		}
		// DocString: FMTModel::solve
		/**
		Virtual function overrided all type of models. In child class, this function is use to solve the model.
		*/
		virtual bool solve()//Only for optimisation
		{
			//Mettre message warning
			return false;
		}
		// DocString: FMTModel::isOptimal
		/**
		Return true if the FMTModel is optimal
		*/
		virtual bool isOptimal() const;
		// DocString: FMTModel::getObjectiveValue
		/**
		Return the value of the globalobjective
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
		locate the static transition themes.
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
		If the user has not defined the _DEATH action and/or the _DEATH transition default _DEATH action and transition are
		going to be defined when the FMTModel class is constructed.
		*/
		void setDefaultObjects();
		// DocString: FMTModel::getBaseMask
		/**
		Return a mask covering the actual bits used by the FMTModel and the optionaldevelopments.
		*/
		Core::FMTMask getBaseMask(std::vector<Core::FMTActualDevelopment> optionaldevelopments) const;
		
		// DocString: FMTModel::getStaticPresolveThemes
		/**
		Using the transition themes look at constraints and actions or yields actions to return
		only themes that are not part of the model.
		*/
		std::vector<const Core::FMTTheme*>getStaticPresolveThemes() const;
		void clearActionsCache();
		void clearYieldCache();
		void clearTransitionCache();
		// DocString: FMTModel::swapPtr
		/**
		Swap *this for the element at the end of the rhs unique_ptr.
		*/
		virtual void swapPtr(std::unique_ptr<FMTModel>& rhs);
		// DocString: FMTModel::setUpSchedulesForBuild
		/**
		Base on the FORCE_PARTIAL_BUILD it will prepare the pass in schedules for building.
		Will make sure that the length is covered with a schedule or an empty one.
		It will always return a filled vector.
		*/
		std::vector<Core::FMTSchedule>setUpSchedulesForBuild(const std::vector<Core::FMTSchedule>& schedules) const;
		// DocString: FMTModel::useActionSerie
		/**
		Returns true if the model make use of action series.
		*/
		bool useActionSerie() const;
		// DocString: FMTModel::getSeriesMaxSize
		/**
		Get the maximal serie size for the whole model.
		*/
		size_t getSeriesMaxSize() const;

	private:
		// DocString: FMTModel::save
		/**
		Save function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
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
		Load function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
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
			Function used to validate FMTMask in a FMTList element like FMTAction, FMTTransition etc...
			Will throw exception if the FMTMask string does not match the FMTthemes.
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
		Function used to validate specifications (check for yields used)
		Will throw excetion if the yield used in the specificaiton is not defined in FMTYields.
		*/
		void validateListSpec(const Core::FMTSpec& specifier) const;
		// DocString: FMTModel::aggregateActions
		/**
		 * @brief Aggregate actions together and on this model (will call setactions)
		 * @param p_ActionsMapping the action aggregates you want to put together.
		 * @return Return the mask filter map to apply to any output with old action name as key
		 *		the tuple values are string for new action name and FMTMask for filter.
		 */
		std::map<std::string, std::pair<std::string,Core::FMTMask>> aggregateActions(std::vector<std::string> p_ActionsMapping);
		// DocString: FMTModel::aggregateTransitions
		/**
		 * @brief Aggregate transitions together and on this model (will call setTransitions)
		 * @param p_Filters the filter of the actions.
		 */
		void aggregateTransitions(const std::map<std::string, std::pair<std::string, Core::FMTMask>>& p_Filters);
		// DocString: FMTModel::aggregateOutputs
		/**
		 * @brief Change outputs by applying a filter to mask output that use aggregated actions.
		 *	It will alse change the action name by the newly aggregated one.
		 * @param[in] p_Filters is the filter generated by the actions aggregation.
		 * @param[in] p_Outputs pointers to the outputs to modify.
		 * @param[in] p_ActionOrdering the hierarchy of the actions.
		 */
		void aggregateOutputs(const std::map<std::string, std::pair<std::string, Core::FMTMask>>& p_Filters,
			std::vector<Core::FMTOutput*>& p_Outputs, const std::vector<std::string>& p_ActionOrdering);
		// DocString: FMTModel::addNewMask
		/**
		 * @brief This function takes a non string mapped FMTMask and add all the aggregates needed
		 * in the themes to produce a valid FMTMask with complete name.
		 * @param p_incompleteMask the mask that doest not have a complete name just bits.
		 * @return A valid mask with the right name with the newly created aggregates.
		 */
		Core::FMTMask addNewMask(const Core::FMTMask& p_incompleteMask);
		// DocString: FMTModel::extendSchedule
		/**
		 * @brief extend the schedule dev with the actual model which use more themes.
		 * @param[in] p_schedules schedules we want to extend.
		 * @return a complete schedule that fits the model.
		 */
		std::vector<Core::FMTSchedule> extendSchedule(const std::vector<Core::FMTSchedule>& p_schedules) const;
		// DocString: FMTModel::_gutsOfConstructor
		/**
		 * @brief the guts of the base constructor.
		 * @param[in] p_area the base area.
		 */
		void _gutsOfConstructor(const std::vector<Core::FMTActualDevelopment>& p_area);
		// DocString: FMTModel::_cleanVector
		/**
		 * @brief Clean a vector by the back to the front
		 * @param[in] p_toClean the vector to clean
		 * @param[in] p_whichtoKeep entries to keep
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
		 * @brief Get potential new themes attributes for a yields
		 * @param[in] p_yieldName the yield name
		 * @return the static aggregates split by -
		 */
		std::set<std::string>_getYieldsStraticAggregates(const std::string& p_yieldName) const;
		// DocString: FMTModel::_getAggregatesWrap
		/**
		 * @brief Wrap aggreagates together with - seprator
		 * @param[in] p_mask to check for aggregates
		 * @param[in] p_themes themes to check for
		 * @return wrapper aggregates.
		 */
		std::string _getAggregatesWrap(const Core::FMTMask& p_mask,
			const std::vector<size_t>& p_themes) const;
		// DocString: FMTModel::_getAggregatesThemes
		/**
		 * @brief select the yields containing p_yieldName and return the theme if of the aggregates in the mask
		 * @param[in]p_yieldName yield ot get
		 * @return id of the themes
		 */
		std::vector<size_t> _getAggregatesThemes(const std::string& p_yieldName) const;
		// DocString: FMTModel::_getYieldAttribute
		/**
		 * @brief Get the yield mask for a given dev containing p_yieldName
		 * @param[in]p_devMask
		 * @param[in]p_yieldName yield ot get
		 * @param[in]p_AggregatedThemes aggregated themes
		 * @return attribute
		 */
		std::string _getYieldAttribute(const Core::FMTMask& p_devMask,
			const std::string& p_yieldName, const std::vector<size_t>& p_AggregatedThemes) const;
    };
// DocString: FMTModelComparator
/**
This class is made to compare FMTModel using the std::find_if() function when FMTmodels are in a stl container.
*/
class FMTModelComparator
{
	// DocString: FMTModelComparator::model_name
	///name of the FMTModel (name membmer of FMTModel)
	std::string model_name;
public:
	// DocString: FMTModelComparator(std::string)
	/**
	Copy constructor class, (name) is the name of the model that we wish to check if it's in the stl container.
	*/
	FMTModelComparator(std::string name);
	// DocString: FMTModelComparator::operator()
	bool operator()(const FMTModel& model) const;

};

}

BOOST_CLASS_EXPORT_KEY(Models::FMTModel)

#endif // FMTMODEL_Hm_included
