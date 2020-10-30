/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTMODEL_H_INCLUDED
#define FMTMODEL_H_INCLUDED

#include <vector>
#include <string>

#include "FMTtheme.h"
#include "FMTaction.h"
#include "FMTtransition.h"
#include "FMTschedule.h"
#include "FMTyields.h"
#include "FMTlifespans.h"
#include "FMTactualdevelopment.h"
#include "FMTconstants.h"
#include "FMToutput.h"
#include "FMTconstraint.h"
#include <memory>
#include "FMTlist.h"
#include "FMTmodelstats.h"
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/unordered_map.hpp>

#include <boost/serialization/export.hpp>


namespace Graph
{
	template<class T1, class T2>
	class FMTgraph;
	class FMTlinegraph;
}

namespace Spatial
{
	class FMTspatialschedule;
	class FMTspatialnodescache;
}


namespace Models
{
// DocString: FMTmodel
/**
FMTmodel is the base class of multiple models in FMT
It' is the formulation has seen in the Woodstock files of a forest planning problem.
FMTlpmodel / FMTsamodel / FMTsesmodel use the FMTmodel protected member to
generate the problem based on LP/SA or to simply simulate the FMTmodel.
*/

class FMTmodel : public Core::FMTobject
    {
	template<class T1,class T2>
    friend class Graph::FMTgraph;
	friend class Graph::FMTlinegraph;
	friend class Spatial::FMTspatialschedule;
	friend class Spatial::FMTspatialnodescache;
	// DocString: FMTmodel::save
	/**
	Save function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void save(Archive& ar, const unsigned int version) const
	{
		ar & boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTobject>(*this));
		FMTobject::forcesave(ar, version);
		ar & BOOST_SERIALIZATION_NVP(area);
		ar & BOOST_SERIALIZATION_NVP(themes);
		ar & BOOST_SERIALIZATION_NVP(actions);
		ar & BOOST_SERIALIZATION_NVP(transitions);
		ar & BOOST_SERIALIZATION_NVP(yields);
		ar & BOOST_SERIALIZATION_NVP(lifespan);
		ar & BOOST_SERIALIZATION_NVP(outputs);
		ar & BOOST_SERIALIZATION_NVP(constraints);
		ar & BOOST_SERIALIZATION_NVP(name);
	}
	// DocString: FMTmodel::load
	/**
	Load function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	template<class Archive>
	void load(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTobject>(*this));
		FMTobject::forceload(ar, version);//get the object information for the global object
		ar & BOOST_SERIALIZATION_NVP(area);
		ar & BOOST_SERIALIZATION_NVP(themes);
		ar & BOOST_SERIALIZATION_NVP(actions);
		ar & BOOST_SERIALIZATION_NVP(transitions);
		ar & BOOST_SERIALIZATION_NVP(yields);
		ar & BOOST_SERIALIZATION_NVP(lifespan);
		ar & BOOST_SERIALIZATION_NVP(outputs);
		ar & BOOST_SERIALIZATION_NVP(constraints);
		ar & BOOST_SERIALIZATION_NVP(name);
		this->passinobject(*this);//Distribute global object to local object
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
		try{
			for (const auto& listobject : container)
				{
				std::string name = std::string(listobject.first);
				this->validate(themes, name);
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTsesmodel::validatelistmasks", __LINE__, __FILE__);
			}
		}
	// DocString: FMTmodel::validatelistspec
	/**
	Function used to validate specifications (check for yields used)
	Will throw excetion if the yield used in the specificaiton is not defined in FMTyields.
	*/
	void validatelistspec(const Core::FMTspec& specifier) const;
    protected:
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
		// DocString: FMTmodel::getselectedmask
		/**
		If the model is presolved you can call this function to get the selection mask that you need
		to use to presolve a FMTmask.
		*/
		Core::FMTmask getselectedmask(const std::vector<Core::FMTtheme>& originalthemes) const;
		// DocString: FMTmodel::getbasemask
		/**
		Return a mask covering the actual bits used by the FMTmodel and the optionaldevelopements.
		*/
		Core::FMTmask getbasemask(std::vector<Core::FMTactualdevelopment> optionaldevelopments) const;
		// DocString: FMTmodel::locatestatictransitionthemesptr
		/**
		locate the static transition themes.
		*/
		std::vector<size_t>getstatictransitionthemes() const;
    public:
		// DocString: FMTmodel()
		/**
		Main constructor for FMTmodel used in Parser::FMTmodelparser, the constraints are optional.
		For the FMTsesmodel no constraints are needed.
		*/
		FMTmodel(const std::vector<Core::FMTactualdevelopment>& larea, const std::vector<Core::FMTtheme>& lthemes,
			const std::vector<Core::FMTaction>& lactions,
			const std::vector<Core::FMTtransition>& ltransitions, const Core::FMTyields& lyields, const Core::FMTlifespans& llifespan,
			const std::string& lname, const std::vector<Core::FMToutput>& loutputs, std::vector<Core::FMTconstraint> lconstraints = std::vector<Core::FMTconstraint>());
		// DocString: FMTmodel()
		/**
			Default constructor of FMTmodel.
		*/
		FMTmodel();
		// DocString: ~FMTmodel()
		/**
			Default virtual desctructor of FMTmodel.
		*/
		virtual ~FMTmodel()=default;
		// DocString: FMTmodel::getarea
		/**
			Virtual function to get the area of a given period into actualdevelopement.
		*/
		virtual std::vector<Core::FMTactualdevelopment>getarea(int period = 0) const;
		// DocString: FMTmodel::getcopy
		/**
		This function returns a copy of the FMTmodel of the selected period.
		The function is going to clean the FMTconstraints and keep the objective.
		*/
		virtual FMTmodel getcopy(int period = 0) const;
		// DocString: FMTmodel::presolve
		/**
		This function use a vector of developments and the actual transitions of the model and return new presolved FMTmodel.
		The function can reduce the number of global themes/actions/transitions/yields/lifespans/outputs/constraints data
		if the model is badly formulated.
		*/
		virtual std::unique_ptr<FMTmodel>presolve(int presolvepass = 10,std::vector<Core::FMTactualdevelopment> optionaldevelopments = std::vector<Core::FMTactualdevelopment>()) const;
		// DocString: FMTmodel::postsolve
		/*
		This function is for postsolving the presolved model into the original model.
		*/
		virtual std::unique_ptr<FMTmodel>postsolve(const FMTmodel& originalbasemodel) const;
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
		Adds one output to the model base only on strings.
		name = output name
		maskstring = string of the mask "? ? ?"
		outputtarget = type of ouput created
		action = action string targeted
		yield = yield string targeted
		description = description of the output
		*/
		void addoutput(const std::string& name,const std::string& maskstring, Core::FMTotar outputtarget,
			std::string action = std::string(), std::string yield = std::string(), std::string description = std::string(),int targettheme = -1);
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
		// DocString: FMTmodel::locatestaticthemes
		/**
		Based on the FMTmodel looks at every theme to located in the FMTtransitions and return themes 
		that are not used into the transition and in a given output (staticthemes)... will return themes based on spatial units.
		*/
		std::vector<Core::FMTtheme> locatestaticthemes(const Core::FMToutput& output, bool ignoreoutputvariables = false) const;
		// DocString: FMTmodel::locatestaticthemes
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
		// DocString: FMTmodel::getdynamicmask
		/**
		Based on static and dynamic themes will return a mask where the static attributes will have a value of 0 and dynamic value a value of 1.
		*/
		Core::FMTmask getdynamicmask(const Core::FMToutput& output, bool ignoreoutputvariables = false) const;
		// DocString: FMTmodel::getdynamicmask
		/**
		Based on static and dynamic themes will return a mask where the static attributes will have a value of 0 and dynamic value a value of 1.
		*/
		Core::FMTmask getdynamicmask(const Core::FMToutputnode& node, bool ignoreoutputvariables = false) const;
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
		// DocString: FMTmodel::passinlogger
		/**
		It's sometime usefull to pass in the logger of an other FMTobject.
		*/
		void passinlogger(const std::shared_ptr<Logging::FMTlogger>& logger) override;
		// DocString: FMTmodel::passinexceptionhandler
		/**
		It's sometime usefull to pass in the exception handler of an other FMTobject.
		*/
		void passinexceptionhandler(const std::shared_ptr<Exception::FMTexceptionhandler>& exhandler) override;
		// DocString: FMTmodel::passinobject
		/**
		It's sometime usefull to pass in the exception handler and the logger  of an other FMTobject to
		a FMTobject.
		*/
		void passinobject(const Core::FMTobject& rhs) override;
		// DocString: FMTmodel::setdefaultlogger
		/**
		Create and set a default logger to the FMTobject.
		*/
		void setdefaultlogger() override;
		// DocString: FMTmodel::setquietlogger
		/**
		Create and set a quiet logger to the FMTobject.
		*/
		void setquietlogger() override;
		// DocString: FMTmodel::setdebuglogger
		/**
		Create and set a debug logger to the FMTobject.
		*/
		void setdebuglogger() override;
		// DocString: FMTmodel::setdefaultexceptionhandler
		/**
		Create and set a default exception handler to the FMTobject.
		*/
		void setdefaultexceptionhandler() override;
		// DocString: FMTmodel::setquietexceptionhandler
		/**
		Create and set a quiet exception handler to the FMTobject.
		*/
		void setquietexceptionhandler() override;
		// DocString: FMTmodel::setdebugexceptionhandler
		/**
		Create and set a debug exception handler to the FMTobject.
		*/
		void setdebugexceptionhandler() override;
		// DocString: FMTmodel::setfreeexceptionhandle
		/**
		Create and set a free exception handler to the FMTobject.
		*/
		void setfreeexceptionhandler() override;
		// DocString: FMTmodel::disablenestedexceptions
		/**
		Disable nested exception throw of the Exceptionhandler by default all handlers
		do nested exception throw.
		*/
		void disablenestedexceptions() override;
		// DocString: FMTmodel::enablenestedexceptions
		/**
		Enable nested exception throw of the Exceptionhandler by default all handlers
		do nested exception throw.
		*/
		void enablenestedexceptions() override;

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
