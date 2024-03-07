/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTMODEL_H_INCLUDED
#define FMTMODEL_H_INCLUDED

#include "FMTtheme.hpp"
#include "FMTaction.hpp"
#include "FMTtransition.hpp"
#include "FMTyields.hpp"
#include "FMTlifespans.hpp"
#include "FMTactualdevelopment.hpp"
#include "FMToutput.hpp"
#include "FMTconstraint.hpp"
#include "FMTlist.hpp"
#include "FMTmodelparameters.hpp"
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/export.hpp>
#include "FMToutputnode.hpp"
#include "FMTschedule.hpp"


namespace Graph
{
	template<class T1, class T2>
	class FMTgraph;
	class FMTlinegraph;
	class FMTgraphvertextoyield;
}

namespace Parser
{
	class FMTmodelparser;
}

namespace Spatial
{
	class FMTspatialschedule;
	class FMTspatialnodescache;
}
/// Namespace containing all enumerator and classes used to construct or manipulate the different type of model in FMT.
namespace Models
{
	class FMTmodelstats;
	// DocString: FMTmodel
	/**
	FMTmodel is the base class of multiple models in FMT
	It' is the formulation has seen in the Woodstock files of a forest planning problem.
	FMTlpmodel / FMTsamodel / FMTsesmodel use the FMTmodel protected member to
	generate the problem based on LP/SA or to simply simulate the FMTmodel.
	*/

	class FMTEXPORT FMTmodel : public Core::FMTobject
    {
		template<class T1,class T2>
		friend class Graph::FMTgraph;
		friend class Graph::FMTlinegraph;
		friend class Spatial::FMTspatialschedule;
		friend class Spatial::FMTspatialnodescache;
		friend class Parser::FMTmodelparser;
		friend class Graph::FMTgraphvertextoyield;
	public:
		// DocString: FMTmodel::aggregateAllActions
		/**
		 * @brief Aggregate actions together and create en enterly new model with new
		 *	-actions,transitions,outputs,constraints,yields,lifespan
		 * @param p_Aggregates the actions aggregate we want to merge together
		 * @return newly created model with aggregated actions.
		 */
		Models::FMTmodel aggregateAllActions(const std::vector<std::string>& p_Aggregates) const;
		// DocString: FMTmodel::aggregateSchedules
		/**
		 * @brief Build new schedule based on p_schedules with the corresponding aggregated actions
		 * @param p_schedules the schedules of the original model.
		 * @return newly created schedules with aggregated actions.
		 */
		std::vector<Core::FMTschedule> aggregateSchedules(const std::vector<Core::FMTschedule>& p_schedules) const;
		// DocString: FMTmodel::setparallellogger
		/**
		Solver's logger cannot work in parallel so you need to pass a logger owned
		by the thead to the solver to make sure it does not work in concurrency.
		*/
		virtual void setparallellogger(Logging::FMTlogger& logger);
		// DocString: FMTmodel::clearcache
		/**
		Clear caching element to reduce memory usage.
		*/
		virtual void clearcache();
		// DocString: FMTmodel(const std::vector<Core::FMTactualdevelopment>& larea, const std::vector<Core::FMTtheme>, const std::vector<Core::FMTaction>, const std::vector<Core::FMTtransition>, const Core::FMTyields, const Core::FMTlifespans, const std::string, const std::vector<Core::FMToutput>, std::vector<Core::FMTconstraint>,FMTmodelparameters)
		/**
		Main constructor for FMTmodel used in Parser::FMTmodelparser, the constraints and parameters are optional.
		For the FMTsesmodel no constraints are needed.
		*/
		FMTmodel(const std::vector<Core::FMTactualdevelopment>& larea, const std::vector<Core::FMTtheme>& lthemes,
			const std::vector<Core::FMTaction>& lactions,
			const std::vector<Core::FMTtransition>& ltransitions, const Core::FMTyields& lyields, const Core::FMTlifespans& llifespan,
			const std::string& lname, const std::vector<Core::FMToutput>& loutputs, std::vector<Core::FMTconstraint> lconstraints = std::vector<Core::FMTconstraint>(),
			FMTmodelparameters lparameters = FMTmodelparameters());
		// DocString: FMTmodel()
		/**
		Default constructor of FMTmodel.
		*/
		FMTmodel();
		// DocString: ~FMTmodel
		/**
			Default virtual desctructor of FMTmodel.
		*/
		virtual ~FMTmodel() = default;
		// DocString: FMTmodel(FMTmodel&&)
		/**
		Default move constructor for FMTmodel.
		*/
		FMTmodel(FMTmodel&& rhs) = default;
		// DocString: FMTmodel::operator=(FMTmodel&&) 
		/**
		Default move assignment for FMTmodel.
		*/
		FMTmodel& operator =(FMTmodel&& rhs) = default;
		// DocString: FMTmodel::getarea
		/**
			Virtual function to get the area of a given period into actualdevelopement. If before grow = true it will returns the developement before it grow and death to the next period.
		*/
		virtual std::vector<Core::FMTactualdevelopment>getarea(int period = 0, bool beforegrowanddeath = false) const;
		// DocString: FMTmodel::getcopy
		/**
		This function returns a copy of the FMTmodel of the selected period.
		The function is going to clean the FMTconstraints and keep the objective.
		*/
		virtual std::unique_ptr<FMTmodel> getcopy(int period = 0) const;
		// DocString: FMTmodel::basepresolve
		/**
		This function use the existin area and call the presolve function
		The function can reduce the number of global themes/actions/transitions/yields/lifespans/outputs/constraints data
		if the model is badly formulated.
		*/
		FMTmodel basepresolve() const;
		// DocString: FMTmodel::getpresolvefilter
		/**
		Get the presolve filter.
		*/
		Core::FMTmaskfilter getpresolvefilter(const std::vector<Core::FMTtheme>& originalthemes) const;
		// DocString: FMTmodel::getpostsolvefilter
		/**
		Get the postsolve filter used by the presolve (in case you want to turn presolved mask into postsolve mask).
		*/
		Core::FMTmaskfilter getpostsolvefilter(const std::vector<Core::FMTtheme>& originalthemes, const Core::FMTmask& devmask) const;
		// DocString: FMTmodel::getselectedmask
		/**
		If the model is presolved you can call this function to get the selection mask that you need
		to use to presolve a FMTmask.
		*/
		Core::FMTmask getselectedmask(const std::vector<Core::FMTtheme>& originalthemes) const;
		// DocString: FMTmodel::presolve
		/**
		This function use a vector of developments and the actual transitions of the model and return new presolved FMTmodel.
		The function can reduce the number of global themes/actions/transitions/yields/lifespans/outputs/constraints data
		if the model is badly formulated.
		*/
		virtual std::unique_ptr<FMTmodel>presolve(std::vector<Core::FMTactualdevelopment> optionaldevelopments = std::vector<Core::FMTactualdevelopment>()) const;
		// DocString: FMTmodel::postsolve
		/*
		This function is for postsolving the presolved model into the original model.
		*/
		virtual void postsolve(const FMTmodel& originalbasemodel);
		// DocString: FMTmodel::presolveschedule
		/**
		Using the original schedule (not presolved) and the original model (not presolved) it returns a new schedule with
		presolved FMTdevelopements masks and actions.
		*/
		Core::FMTschedule presolveschedule(const Core::FMTschedule& originalbaseschedule,
			const FMTmodel& originalbasemodel) const;
		// DocString: FMTmodel::getmodelstats
		/**
		Return the statistics of the model, the number of themes, yields, actions, transitions etc...
		*/
		FMTmodelstats getmodelstats() const;
		// DocString: FMTmodel::cleanactionsntransitions
		/**
		Function do delete action that have no defined transition and to delete transition that have no defined action.
		Actions and transitions are then sorted.
		*/
		void cleanactionsntransitions();
		// DocString: FMTmodel::getname
		/**
		Returns the name of the FMTmodel
		*/
		inline std::string getname() const
		{
			return name;
		}
		// DocString: FMTmodel::getthemes
		/**
		Getter returning a copy of the FMTthemes vector<> of the model.
		*/
		inline std::vector<Core::FMTtheme>getthemes() const
		{
			return themes;
		}
		// DocString: FMTmodel::getactions
		/**
		Getter returning a copy of the FMTactions vector<> of the model.
		*/
		inline std::vector<Core::FMTaction>getactions() const
		{
			return actions;
		}
		// DocString: FMTmodel::gettransitions
		/**
		Getter returning a copy of the FMTtransitions vector<> of the model.
		*/
		inline std::vector<Core::FMTtransition>gettransitions() const
		{
			return transitions;
		}
		// DocString: FMTmodel::getyields
		/**
		Getter returning a copy of the FMTyields data of the model.
		*/
		inline Core::FMTyields getyields() const
		{
			return yields;
		}
		// DocString: FMTmodel::getlifespan
		/**
		Getter returning a copy of the FMTlifespan data of the model.
		*/
		inline Core::FMTlifespans getlifespan() const
		{
			return lifespan;
		}
		// DocString: FMTmodel::getoutputs
		/**
		Getter returning a copy of the FMToutputs vector<> of the model.
		*/
		inline std::vector<Core::FMToutput> getoutputs() const
		{
			return outputs;
		}
		// DocString: FMTmodel::getconstraints
		/**
		Getter returning a copy of the FMTconstraints vector<> of the model.
		*/
		inline std::vector<Core::FMTconstraint>getconstraints() const
		{
			return constraints;
		}
		// DocString: FMTmodel::defaultdeathaction
		/**
		Returns the default death action when not specified by the user, base on specific lifespan and themes.
		*/
		static Core::FMTaction defaultdeathaction(const Core::FMTlifespans& llifespan,
			const std::vector<Core::FMTtheme>& lthemes);
		// DocString: FMTmodel::defaultdeathtransition
		/**
		Returns the default death transition when not specified by the user, base on specific lifespan and themes.
		*/
		static Core::FMTtransition defaultdeathtransition(const Core::FMTlifespans& llifespan,
			const std::vector<Core::FMTtheme>& lthemes);
		// DocString: FMTmodel::addoutput
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
		void addoutput(const std::string& name, const std::string& maskstring, Core::FMTotar outputtarget,
			std::string action = std::string(), std::string yield = std::string(), std::string description = std::string(), int targettheme = -1);
		// DocString: FMTmodel::addyieldhandlers
		/**
		Add FMTyieldhandler from vector (yieldhandlers) to yields. Can only be use for FMTageyieldhandler and FMTtimeyieldhandler. If you want
		to add FMTmodelyieldhandler or FMTcomplexyieldhander it must be added in the .yld file to parse with the model.
		*/
		template<typename T>
		void addyieldhandlers(const std::vector<T>& yieldhandlers)
		{
			try {
				yields.unshrink(themes);
				for (const auto& yldhandler : yieldhandlers)
				{
					std::unique_ptr<Core::FMTyieldhandler> yldhandlerptr = yldhandler.clone();
					yields.push_back(yldhandlerptr->getmask(), yldhandlerptr);
				}
				yields.update();
			}
			catch (...) {
				_exhandler->printexceptions("", "FMTmodel:::addyieldhandlers", __LINE__, __FILE__);
			}
		}
		// DocString: FMTmodel::addyieldhandlersfromptr
		/**
		Add unique pointer of FMTyieldhandler from vector (yieldhandlers) to yields.
		*/
		void addyieldhandlersfromptr(const std::vector<std::unique_ptr<Core::FMTyieldhandler>>& yieldhandlers);
		// DocString: FMTmodel::operator==
		/**
		Comparison operator of FMTlpmodel
		*/
		bool operator == (const FMTmodel& rhs) const;
		// DocString: FMTmodel::operator<
		/**
		less than operator of FMTlpmodel
		*/
		bool operator < (const FMTmodel& rhs) const;
		// DocString: FMTmodel::setarea
		/**
		Setter for initial FMTactualdevelopment (area section) will replace the originals.
		This function also check if the actualdevelopement lifespan is shorter than the lock.
		If not so, it raise the error FMTdeathwithlock and if the error is set to warning,
		the locks of the FMTactualdevelpement will be reduced accordingly.
		*/
		void setarea(const std::vector<Core::FMTactualdevelopment>& ldevs);
		// DocString: FMTmodel::setthemes
		/**
		Setter for the FMTthemes of the model will replace the originals.
		*/
		void setthemes(const std::vector<Core::FMTtheme>& lthemes);
		// DocString: FMTmodel::setactions
		/**
		Setter for the FMTactions of the model will replace the originals.
		*/
		void setactions(const std::vector<Core::FMTaction>& lactions);
		// DocString: FMTmodel::settransitions
		/**
		Setter for the FMTtransitions of the model will replace the originals.
		*/
		void settransitions(const std::vector<Core::FMTtransition>& ltransitions);
		// DocString: FMTmodel::setconstraints
		/**
		Setter for the FMTconstraints of the model will replace the original.
		*/
		void setconstraints(const std::vector<Core::FMTconstraint>& lconstraint);
		// DocString: FMTmodel::setyields
		/**
		Setter for the FMTyields data of the model will replace the original.
		*/
		void setyields(const Core::FMTyields& lylds);
		// DocString: FMTmodel::setlifespan
		/**
		Setter for the FMTlifespans data of the model will replace the original.
		*/
		void setlifespan(const Core::FMTlifespans& llifespan);
		// DocString: FMTmodel::setname
		/**
		Set the names of the FMTmodel
		*/
		void setname(const std::string& newname);
		// DocString: FMTmodel::setoutputs
		/**
		Setter for the model's outputs.
		*/
		void setoutputs(const std::vector<Core::FMToutput>& newoutputs);
		// DocString: FMTmodel::isvalid
		/**
		This function validate all the FMTmodel
		FMTtheme ->FMTactualdevelopements-> FMTyields -> FMTaction -> FMTtransition -> FMToutput -> FMTconstraint
		return true if the model is valid.
		*/
		bool isvalid();
		// DocString: FMTmodel::locatestaticthemes(const Core::FMToutput, bool ignoreoutputvariables)
		/**
		Based on the FMTmodel looks at every theme to located in the FMTtransitions and return themes
		that are not used into the transition and in a given output (staticthemes)... will return themes based on spatial units.
		*/
		std::vector<Core::FMTtheme> locatestaticthemes(const Core::FMToutput& output, bool ignoreoutputvariables = false) const;
		// DocString: FMTmodel::locatestaticthemes(const Core::FMToutputnode, bool ignoreoutputvariables)
		/**
		Based on the FMTmodel looks at every theme to located in the FMTtransitions and return themes
		that are not used into the transition and in a given output node (staticthemes)... will return themes based on spatial units.
		*/
		std::vector<Core::FMTtheme> locatestaticthemes(const Core::FMToutputnode& output, bool ignoreoutputvariables = false) const;
		// DocString: FMTmodel::locatedynamicthemes
		/**
		Returns themes used in the transition scheme.
		*/
		std::vector<Core::FMTtheme> locatedynamicthemes(const Core::FMToutput& outpu, bool ignoreoutputvariables = false) const;
		// DocString: FMTmodel::getdynamicmask(const Core::FMToutput, bool ignoreoutputvariables)
		/**
		Based on static and dynamic themes will return a mask where the static attributes will have a value of 0 and dynamic value a value of 1.
		*/
		Core::FMTmask getdynamicmask(const Core::FMToutput& output, bool ignoreoutputvariables = false) const;
		// DocString: FMTmodel::getdynamicmask(const Core::FMToutputnode, bool ignoreoutputvariables)
		/**
		Based on static and dynamic themes will return a mask where the static attributes will have a value of 0 and dynamic value a value of 1.
		*/
		Core::FMTmask getdynamicmask(const Core::FMToutputnode& node, bool ignoreoutputvariables = false) const;
		// DocString: FMTmodel::getstaticmask
		/**
		Based on static and dynamic themes will return a mask where the static attributes will have a value of 1 and dynamic value a value of 0.
		*/
		Core::FMTmask getstaticmask(const Core::FMToutputnode& node, bool ignoreoutputvariables = false) const;
		// DocString: FMTmodel::isstaticnode
		/**
		Based on static and dynamic themes will return true if the node can be used in staticmask fonctions.
		More that ratio of set is high the more you will get true from this fonction.
		*/
		bool isstaticnode(const Core::FMToutputnode& node, double ratioofset = 0.1) const;
		// DocString: FMTmodel::locatestatictransitionsthemes
		/**
		Returns the static themes based on model transitions.
		*/
		std::vector<Core::FMTtheme>locatestatictransitionsthemes() const;
		// DocString: FMTmodel::locatenotestaticthemes
		/**
		Returns the static themes based on model transitions.
		*/
		std::vector<Core::FMTtheme>locatenodestaticthemes(const Core::FMToutputnode& node,
			bool ignoreoutputvariables = false,
			std::vector<Core::FMTtheme> basethemes = std::vector<Core::FMTtheme>()) const;
		// DocString: FMTmodel(FMTmodel)
		/**
		Copy constructor of FMTmodel
		*/
		FMTmodel(const FMTmodel& rhs);
		// DocString: FMTmodel::operator=
		/**
		Copy assignment of FMTmodel
		*/
		FMTmodel& operator = (const FMTmodel& rhs);
		// DocString: FMTmodel::empty
		/**
		Returns true if the FMTmodel is empty.
		*/
		bool empty() const;
		// DocString: FMTmodel::append
		/**
		This function append a FMTmodel to an FMTmodel.
		it does not override the objective of the base FMTmodel.
		*/
		void push_back(const FMTmodel& rhs);
		// DocString: FMTmodel::getinitialarea
		/**
		Returns the sum of the area of the area vector.
		*/
		double getinitialarea() const;
		// DocString: FMTmodel::setareaperiod
		/**
		Change to initial area period to the new targeted period.
		*/
		void setareaperiod(const int& period);
		// DocString: FMTmodel::getpotentialschedule
		/**
		The function will remove (toremove) developments from a selected vector (selection). Test operability of the
		difference and return a schedule containing an area of 1.0 if the developement is operable to this actions.
		*/
		Core::FMTschedule getpotentialschedule(std::vector<Core::FMTactualdevelopment> toremove,
			std::vector<Core::FMTactualdevelopment> selection, bool withlock = true) const;
		// DocString: FMTmodel::getreplanningconstraints
		/**
		Ajust constraints that need to be set to a value (getoutput) of the global or local model and return all constraint.
		*/
		virtual std::vector<Core::FMTconstraint> getreplanningconstraints(const std::string& modeltype, const std::vector<Core::FMTconstraint>& localconstraints, const int& period) const;
		// DocString: FMTmodel::doplanning
		/**
		Build the model and do the initialsolve or simulate. This function use the build and solve overrided functions of the differents model type.
		*/
		virtual bool doplanning(const bool& solve, std::vector<Core::FMTschedule> schedules = std::vector<Core::FMTschedule>());
		// DocString: FMTmodel::getoutput
		/**
		Get the output value of a output for a given period using the spatial solution.
		the map key returned consist of output name
		if level == FMToutputlevel::standard || level == FMToutputlevel::totalonly,
		or developement name if level == FMToutputlevel::developpement
		*/
		virtual std::map<std::string, double> getoutput(const Core::FMToutput& output,
			int period, Core::FMToutputlevel level = Core::FMToutputlevel::standard) const;
		// DocString: FMTmodel::getsolution
		/**
		Get the standard solution for a given period (FMTschedule dont have natural growth solution included).
		If with lock is true then the schedule will contain locked developement.
		*/
		virtual Core::FMTschedule getsolution(int period, bool withlock = false) const;
		// DocString: FMTmodel::getoutputsfromperiods
		/**
		For each output get a vector of outputs values starting for first period to last period.
		*/
		virtual std::map<std::string, std::vector<std::vector<double>>>getoutputsfromperiods(const std::vector<Core::FMToutput>& theoutputs,
			const int& firstperiod, const int& lastperiod, Core::FMToutputlevel level = Core::FMToutputlevel::standard) const;
		// DocString: FMTmodel::clone
		/**
		Get a clone of the FMTmodel
		*/
		virtual std::unique_ptr<FMTmodel>clone() const;
		// DocString: FMTmodel::setparameter(const FMTintmodelparameters,const int&)
		/**
		Setter for int model parameters. See FMTmodelparameters.h.
		*/
		virtual bool setparameter(const FMTintmodelparameters& key, const int& value);
		// DocString: FMTmodel::setparameter(const FMTdblmodelparameters,const double)
		/**
		Setter for double model parameters. See FMTmodelparameters.h.
		*/
		virtual bool setparameter(const FMTdblmodelparameters& key, const double& value);
		// DocString: FMTmodel::setparameter(const FMTboolmodelparameters,const bool)
		/**
		Setter for bool model parameters. See FMTmodelparameters.h.
		*/
		virtual bool setparameter(const FMTboolmodelparameters& key, const bool& value);
		// DocString: FMTmodel::getparameter(const FMTintmodelparameters)
		/**
		Getter for int model parameters. Return the parameters value. See FMTmodelparameters.h.
		*/
		int getparameter(const FMTintmodelparameters& key) const;
		// DocString: FMTmodel::getparameter(const FMTdblmodelparameters)
		/**
		Getter for double model parameters. Return the parameters value. See FMTmodelparameters.h.
		*/
		double getparameter(const FMTdblmodelparameters& key) const;
		// DocString: FMTmodel::getparameter(const FMTboolmodelparameters)
		/**
		Getter for bool model parameters. Return the parameters value. See FMTmodelparameters.h.
		*/
		bool getparameter(const FMTboolmodelparameters& key) const;
		// DocString: FMTmodel::setcompresstime
		/**
		Set compresstime value for a range of periods (periodstart,periodstop). See FMTmodelparameters.h.
		*/
		bool setcompresstime(const int& periodstart, const int& periodstop, const int& value);
		// DocString: FMTmodel::getcompresstime
		/**
		Return avec vector with compresstime value for each periods. First period is the index 0 in the vector. See FMTmodelparameters.h.
		*/
		std::vector<int> getcompresstime() const;
		// DocString: FMTmodel::goalconstraints
		/**
		Return all constraints and objective with goals and penalty
		*/
		std::vector<Core::FMTconstraint>goalconstraints(double penalty = 999999) const;
		// DocString: FMTmodel::gettacticalconstraints
		/**
		Get the constraints adapted for a tactical model.
		*/
		std::vector<Core::FMTconstraint>gettacticalconstraints(double penalty = 999999, double scheduleweight = 999999, double objective = 0, double objectivefactor = 1.0) const;
		// DocString: FMTmodel::showparameters
		/**
		Show parameters values. If (showhelp) a brief description of each parameters will appear.
		*/
		virtual void showparameters(const bool& showhelp = false)const;
		// DocString: FMTmodel::build
		/**
		Virtual function overrided all type of models. In child class, this function is use to build the elements needed to solve the model.
		*/
		virtual bool build(std::vector<Core::FMTschedule> schedules = std::vector<Core::FMTschedule>())
		{
			//Mettre message warning
			return false;
		}
		// DocString: FMTmodel::solve
		/**
		Virtual function overrided all type of models. In child class, this function is use to solve the model.
		*/
		virtual bool solve()//Only for optimisation
		{
			//Mettre message warning
			return false;
		}
		// DocString: FMTmodel::isoptimal
		/**
		Return true if the FMTmodel is optimal
		*/
		virtual bool isoptimal() const;
		// DocString: FMTmodel::getobjectivevalue
		/**
		Return the value of the globalobjective
		*/
		virtual double getobjectivevalue() const;
    protected:
		// DocString: FMTmodel::parameters
		///Parameters needed for the function doplanning by the different types of FMTmodel. 
		///See FMTmodelparameters for detail description or use FMTmodel::showparameters(true) to see your parameters and a brief description.
		FMTmodelparameters parameters;
		// DocString: FMTmodel::area
		///Actualdevelopments for period 0, seen in the area section or the shapefile/raster
		std::vector<Core::FMTactualdevelopment>area;
		// DocString: FMTmodel::themes
		///Model themes of the landscape section file.
		std::vector<Core::FMTtheme>themes;
		// DocString: FMTmodel::actions
		///Model actions from the action file and also the _death action
		std::vector<Core::FMTaction>actions;
		// DocString: FMTmodel::transitions
		///Model transitions from the transition file and also the _death transition
		std::vector<Core::FMTtransition>transitions;
		// DocString: FMTmodel::yields
		///Yields data comming from the yield file
		Core::FMTyields yields;
		// DocString: FMTmodel::lifespan
		///lifespan data comming from the lifespan file
		Core::FMTlifespans lifespan;
		// DocString: FMTmodel::outputs
		///Outputs comming from the ouput file
		std::vector<Core::FMToutput> outputs;
		// DocString: FMTmodel::constraints
		///Outputs comming from the optimization file
		std::vector<Core::FMTconstraint>constraints;
		// DocString: FMTmodel::name
		///The name of the Model (name of the .pri file without extension)
		std::string name;
		// DocString: FMTmodel::statictransitionthemes
		///The location of the themes static from transitions
		std::vector<size_t>statictransitionthemes;
		// DocString: FMTmodel::setdefaultobjects
		/**
		If the user has not defined the _DEATH action and/or the _DEATH transition default _DEATH action and transition are
		going to be defined when the FMTmodel class is constructed.
		*/
		void setdefaultobjects();
		// DocString: FMTmodel::getbasemask
		/**
		Return a mask covering the actual bits used by the FMTmodel and the optionaldevelopements.
		*/
		Core::FMTmask getbasemask(std::vector<Core::FMTactualdevelopment> optionaldevelopments) const;
		// DocString: FMTmodel::getstatictransitionthemes
		/**
		locate the static transition themes.
		*/
		std::vector<size_t>getstatictransitionthemes() const;
		// DocString: FMTmodel::getstaticpresolvethemes
		/**
		Using the transition themes look at constraints and actions or yields actions to return
		only themes that are not part of the model.
		*/
		std::vector<Core::FMTtheme>getstaticpresolvethemes() const;
		void clearactionscache();
		void clearyieldcache();
		void cleartransitioncache();
		// DocString: FMTmodel::swap_ptr
		/**
		Swap *this for the element at the end of the rhs unique_ptr.
		*/
		virtual void swap_ptr(const std::unique_ptr<FMTmodel>& rhs);
		// DocString: FMTmodel::setupschedulesforbuild
		/**
		Base on the FORCE_PARTIAL_BUILD it will prepare the pass in schedules for building.
		Will make sure that the length is covered with a schedule or an empty one.
		It will always return a filled vector.
		*/
		std::vector<Core::FMTschedule>setupschedulesforbuild(const std::vector<Core::FMTschedule>& schedules) const;
		// DocString: FMTmodel::useactionserie
		/**
		Returns true if the model make use of action series.
		*/
		bool useactionserie() const;
		// DocString: FMTmodel::getseriesmaxsize
		/**
		Get the maximal serie size for the whole model.
		*/
		size_t getseriesmaxsize() const;
	private:
		// DocString: FMTmodel::save
		/**
		Save function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void save(Archive& ar, const unsigned int version) const
		{
			ar& boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTobject>(*this));
			FMTobject::forcesave(ar, version);
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
		// DocString: FMTmodel::load
		/**
		Load function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		template<class Archive>
		void load(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTobject>(*this));
			FMTobject::forceload(ar, version);//get the object information for the global object
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
			// DocString: FMTmodel::validatelistmasks
			/**
			Function used to validate FMTmask in a FMTlist element like FMTaction, FMTtransition etc...
			Will throw exception if the FMTmask string does not match the FMTthemes.
			*/
			template<class T>
		void validatelistmasks(const Core::FMTlist<T>& container) const
		{
			try {
				for (const auto& listobject : container)
				{
					std::string name = std::string(listobject.first);
					Core::FMTtheme::validate(themes, name);
				}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTmodel::validatelistmasks", __LINE__, __FILE__);
			}
		}
		// DocString: FMTmodel::validatelistspec
		/**
		Function used to validate specifications (check for yields used)
		Will throw excetion if the yield used in the specificaiton is not defined in FMTyields.
		*/
		void validatelistspec(const Core::FMTspec& specifier) const;
		// DocString: FMTmodel::aggregateActions
		/**
		 * @brief Aggregate actions together and on this model (will call setactions)
		 * @param p_ActionsMapping the action aggregates you want to put together.
		 * @return Return the mask filter map to apply to any output with old action name as key
		 *		the tuple values are string for new action name and FMTmask for filter.
		 */
		std::map<std::string, std::pair<std::string,Core::FMTmask>> aggregateActions(const std::vector<std::string>& p_ActionsMapping);
		// DocString: FMTmodel::aggregateTransitions
		/**
		 * @brief Aggregate transitions together and on this model (will call settransitions)
		 * @param p_Filters the filter of the actions.
		 */
		void aggregateTransitions(const std::map<std::string, std::pair<std::string, Core::FMTmask>>& p_Filters);
		// DocString: FMTmodel::aggregateOutputs
		/**
		 * @brief Change outputs by applying a filter to mask output that use aggregated actions.
		 *	It will alse change the action name by the newly aggregated one.
		 * @param p_Filters is the filter generated by the actions aggregation.
		 * @param p_Outputs pointers to the outputs to modify.
		 */
		void aggregateOutputs(const std::map<std::string, std::pair<std::string, Core::FMTmask>>& p_Filters,
			std::vector<Core::FMToutput*>& p_Outputs);
		// DocString: FMTmodel::addNewMask
		/**
		 * @brief This function takes a non string mapped FMTmask and add all the aggregates needed
		 * in the themes to produce a valid FMTmask with complete name.
		 * @param p_incompleteMask the mask that doest not have a complete name just bits.
		 * @return A valid mask with the right name with the newly created aggregates.
		 */
		Core::FMTmask addNewMask(const Core::FMTmask& p_incompleteMask);
		
		

    };
// DocString: FMTmodelcomparator
/**
This class is made to compare FMTmodel using the std::find_if() function when FMTmodels are in a stl container.
*/
class FMTmodelcomparator
{
	// DocString: FMTmodelcomparator::model_name
	///name of the FMTmodel (name membmer of FMTmodel)
	std::string model_name;
public:
	// DocString: FMTmodelcomparator(std::string)
	/**
	Copy constructor class, (name) is the name of the model that we wish to check if it's in the stl container.
	*/
	FMTmodelcomparator(std::string name);
	// DocString: FMTmodelcomparator::operator()
	bool operator()(const FMTmodel& model) const;

};

}

BOOST_CLASS_EXPORT_KEY(Models::FMTmodel)

#endif // FMTMODEL_H_INCLUDED
