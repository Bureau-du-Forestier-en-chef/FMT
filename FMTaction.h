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


#ifndef FMTACT_H_INCLUDED
#define FMTACT_H_INCLUDED

#include "FMTlist.h"
#include "FMTbounds.h"
#include <boost/serialization/string.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <string>
#include <vector>
#include <boost/serialization/export.hpp>

namespace Core
{

class FMTactioncomparator;
// DocString: FMTaction
/**
FMTaction hold informations to check if a FMTdevelopement is operable or not to this.
The operability testing for a given action is in the FMTdevelopement class.
FMTaction consist of a list of specifications that dictate the operability of 
a subsets of FMTdevelopments.
*/
class FMTaction : public FMTlist<FMTspec>
    {
	friend class FMTactioncomparator;
	// DocString: FMTaction::serialize
	/**
	serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		ar & boost::serialization::make_nvp("specs",boost::serialization::base_object<FMTlist<FMTspec>>(*this));
		ar & BOOST_SERIALIZATION_NVP(partials);
		ar & BOOST_SERIALIZATION_NVP(agelowerbound);
		ar & BOOST_SERIALIZATION_NVP(ageupperbound);
		ar & BOOST_SERIALIZATION_NVP(periodlowerbound);
		ar & BOOST_SERIALIZATION_NVP(periodupperbound);
		ar & BOOST_SERIALIZATION_NVP(name);
		ar & BOOST_SERIALIZATION_NVP(aggregates);
		ar & BOOST_SERIALIZATION_NVP(lock);
		ar & BOOST_SERIALIZATION_NVP(reset);
		}
	protected:
		// DocString: FMTaction::aggregates
		///An action can be part of a aggregate so this data member gets the name of all aggregate the action is being part of.
		std::vector<std::string>aggregates;
		// DocString: FMTaction::partials
		///Keeps the yields name for determining the amount of wood harvested in case of partial cut.
        std::vector<std::string>partials;
		// DocString: FMTaction::agelowerbound
		///Those data members are for optimization only,
		///the class determine within which bounds the aciton can take place for a given development.
		int agelowerbound, ageupperbound, periodlowerbound, periodupperbound;
		// DocString: FMTaction::name
		///The name of the action
		std::string name;
		// DocString: FMTaction::lock
		///If lock is true the action is not _lockexempt when false the action is _LOCKEXEMPT
		bool lock;
		// DocString: FMTaction::reset
		///If reset is true then the action is age reset Y else the action  doen't reset age
		bool reset;
		// DocString: FMTaction::setbounds
		/**
		This function is for optimization it iterates on the specifications to get the 
		agelowerbound, ageupperbound,periodlowerbound,periodupperbound and sets all member data.
		*/
		void setbounds();
    public:
		// DocString: FMTaction()
		/**
		Default constructor for FMTaction
		*/
        FMTaction();
		// DocString: ~FMTaction()
		/**
		Destructor for FMTaction
		*/
        virtual ~FMTaction() = default;
		// DocString: FMTaction(const std::string&)
		/**
		Construct a empty FMTaction with a given name
		*/
        FMTaction(const std::string& lname);
		// DocString: FMTaction(const std::string&,const bool&,const bool&)
		/**
		Construct a empty FMTaction with a given name, respectlock? and resetage?
		*/
        FMTaction(const std::string& lname, const bool& lock,const bool& reset);
		// DocString: FMTaction::push_aggregate
		/**
		Push back aggregate to the aggregates data member vector
		*/
		void push_aggregate(const std::string& aggregate);
		// DocString: FMTaction::push_partials
		/**
		Push back a partiel yield name to the partials data member vector
		*/
        void push_partials(const std::string& yield);
		// DocString: FMTaction(const FMTaction&)
		/**
		Copy constructor of FMTaction
		*/
        FMTaction(const FMTaction& rhs);
		/**
		Copy assignement of FMTaction
		*/
        FMTaction& operator = (const FMTaction& rhs);
		/**
		Every FMTlist container need to have a update() function
		this function calls setbounds() to update all bounds and also shrink
		the FMTlist (all for optimization of the process)
		*/
		void update() override;
		/**
		Eliminate FMTspecification and presolve all masks base on a basemask a preolved mask
		and presolved themes. The returned action can also be empty.
		*/
		FMTaction presolve(const FMTmask& basemask,
			const std::vector<FMTtheme>& originalthemes,
			const FMTmask& presolvedmask,
			const std::vector<FMTtheme>& newthemes) const;
		/**
		Get the age lower bound for all FMTspecs.
		*/
		inline const int& getagelowerbound() const
			{
			return agelowerbound;
			}
		/**
		Get the age upper bound for all FMTspecs.
		*/
		inline const int& getageupperbound() const
			{
			return ageupperbound;
			}
		/**
		Get the period lower bound for all FMTspecs.
		*/
		inline const int& getperiodlowerbound() const
			{
			return periodlowerbound;
			}
		/**
		Get the period upper bound for all FMTspecs.
		*/
		inline const int& getperiodupperbound() const
			{
			return periodupperbound;
			}
		/**
		Return the hash value base on the action name.
		*/
		inline size_t hash() const
			{
			return boost::hash<std::string>()(name);
			}
		/**
		Get action name of the FMTaction.
		*/
		inline std::string getname() const
			{
			return name;
			}
		/**
		If True the action needs to respect the _lock stade of the developement.
		Else the action doesn't car about the _lock of the developement.
		*/
		inline bool dorespectlock() const
			{
			return lock;
			}
		/**
		If True When operated by this the FMTdevelopement is set to 0.
		Else it stays the same
		*/
		inline bool isresetage() const
			{
			return reset;
			}
		/**
		Gets all the aggregates name of which the action is part of.
		*/
		std::vector<std::string>getaggregates() const;
		/**
		Gets all the partial yields name of the FMTaction.
		*/
		std::vector<std::string>getpartials() const;
		/**
		FMTaction less than operator (first test the action length and then the string name to place the action).
		*/
        bool operator < (const FMTaction& rhs) const;
		/**
		FMTaction equality operator check if FMTactions have the same name.
		*/
        bool operator == (const FMTaction& rhs) const;
		/**
		FMTaction nonequality operator check if FMTactions have the not same name.
		*/
        bool operator != (const FMTaction& rhs) const;
		/**
		Convert the FMTaction to a string (like in a regular .act file)
		*/
        operator std::string() const;
		/**
		Check if the yield needs to be considered has partial for this action.
		*/
		bool partial(const std::string& yield) const;
    };

/**
FMTactioncomparator to check if the action_name already exist in a std container.
It can also check for aggregates.
*/
class FMTactioncomparator
	{
	///The action named that we are looking for.
	std::string action_name;
	///If true the comparator will also check for aggregates.
	bool checkaggregate;
	public:
		/**
		FMTactioncomparator constructor name is the name of the action we want to match
		if checkaggregate = true it will also return actions within the named aggregate.
		*/
		FMTactioncomparator(std::string name, bool lcheckaggregate = false);
		/**
		Looking at the actions vector if aggregateonly = false it will returns actions mathching
		the action_name if aggregateonly = true it will return only matching aggregates.
		*/
		std::vector<const FMTaction*>getallaggregates(const std::vector<FMTaction>&actions,bool aggregateonly = false) const;
		/**
		Matching test operator for FMTactioncomparator.
		*/
		bool operator()(const FMTaction& action) const;

	};


}

namespace boost {

/**
boost hash for FMTaction
*/
  template <>
  struct hash<Core::FMTaction>
  {
    std::size_t operator()(const Core::FMTaction& act) const
        {
        return (act.hash());
        }
  };


}
BOOST_CLASS_EXPORT_KEY(Core::FMTaction);
#endif // FMTACT_H_INCLUDED
