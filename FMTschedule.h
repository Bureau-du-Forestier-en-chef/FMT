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

#ifndef FMTschedule_H_INCLUDED
#define FMTschedule_H_INCLUDED

#include <map>
#include <vector>
#include "FMTdevelopment.h"
#include "FMTaction.h"
#include <iterator>
#include <boost/serialization/export.hpp>

namespace Core
{
// DocString: FMTschedule
/**
FMTschedule is the representation of a spatialy referenced schedule. It can be used by the FMTlpmodel and the FMTsesmodel.
It describes the area of a given developements for which an FMTaction can be operated (key).
The vector is a reprensentation of lock level at position 0 in the vector the lock level = 0 (if lock level 0 exist in the graph
, at position 1 lock level = 1.
*/
class FMTschedule
    {
	// DocString: FMTschedule::serialize
	/**
	serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(period);
		ar & BOOST_SERIALIZATION_NVP(elements);
	}
	// DocString: FMTschedule::period
	///The period at which the schedule needs to be applied
    int period;
	// DocString: FMTschedule::elements
	///Main FMTschedule elements action has key, followed by a map of FMTdevelopement and a vector of double
	///The vector of double represent the _lock level.
	std::map<FMTaction, std::map<FMTdevelopment, std::vector<double>>> elements;
	// DocString: FMTschedule::sameelements
	/**
	Return true if the FMTschedule contains the same elements.
	*/
	bool sameelements(const FMTschedule& rhs) const;
    public:
		// DocString: FMTschedule(const int&,std::vector<FMTschedule>&)
		/**
		FMTschedule constructor for a complete construction of FMTschedule with multiple FMTschedule
		*/
		FMTschedule(const int& lperiod, std::vector<FMTschedule>& schedules);
		// DocString: FMTschedule(int,std::map<FMTaction,std::map<FMTdevelopment,std::vector<double>>>)
		/**
		FMTschedule constructor for a complete construction of FMTschedule
		*/
        FMTschedule(int lperiod, std::map<FMTaction, std::map<FMTdevelopment, std::vector<double>>> mapping);
		// DocString: FMTschedule(int,const std::map<FMTaction,std::map<FMTdevelopment,std::map<int,double>>>&)
		/**
		FMTschedule constructor _lock based mapping (When a schedule is parsed).
		*/
		FMTschedule(int lperiod, const std::map<FMTaction, std::map<FMTdevelopment, std::map<int,double>>>& mapping);
		// DocString: FMTschedule()
		/**
		Default FMTschedule constructor.
		*/
        FMTschedule();
		// DocString: FMTschedule(const FMTschedule&)
		/**
		Default FMTschedule copy constructor.
		*/
        FMTschedule(const FMTschedule& rhs);
		// DocString: FMTschedule::operator=
		/**
		Default FMTschedule copy assignment.
		*/
        FMTschedule& operator = (const FMTschedule& rhs);
		// DocString: FMTschedule::operator==
		/**
		FMTschedule equality operator.
		*/
		bool operator == (const FMTschedule& rhs) const;
		// DocString: FMTschedule::operator!=
		/**
		FMTschedule nonequality operator.
		*/
		bool operator != (const FMTschedule& rhs) const;
		// DocString: FMTschedule::operator+=
		/**
		This function append an FMTschedule to an other FMTschedule.
		*/
		FMTschedule& operator += (const FMTschedule& rhs);
		// DocString: FMTschedule::operator+
		/**
		This function append an FMTschedule to an other FMTschedule and return a new one.
		*/
		FMTschedule operator + (const FMTschedule& rhs) const;
		// DocString: FMTschedule::operator std::string
		/**
		Convert the FMTschedule to a string like seen in a regular scedule file.
		*/
        operator std::string() const;
		// DocString: FMTschedule::getperiod
		/**
		Getter for the schedule period.
		*/
		inline int getperiod() const
			{
			return period;
			}
		// DocString: FMTschedule::actionarea
		/**
		Return the total harvested area of a FMTaction present in the FMTschedule.
		*/
        double actionarea(const FMTaction& action) const;
		// DocString: FMTschedule::area
		/**
		Return the total area of the schedule operated by all schedule actions.
		*/
        double area() const;
		// DocString: FMTschedule::operated
		/**
		Check if a given FMTdevelopment has been actioned for the this FMTschedule.
		*/
		bool operated(const FMTaction& action,
			const FMTdevelopment& developement) const;
		// DocString: FMTschedule::sort
		/**
		Sort the vector of double of the lock level per area for the FMTschedule.
		*/
		void sort();
		// DocString: FMTschedule::empty
		/**
		Return true is the FMTschedule is not empty.
		*/
		bool empty() const;
		// DocString: FMTschedule::size
		/**
		Return the size of the FMTschedule.
		*/
		size_t size() const;
		// DocString: FMTschedule::presolve
		/**
		Presolve all masks base on a basemask a preolved mask
		and presolved themes and presolved actions list. The returned schedule can also be empty.
		*/
		FMTschedule presolve(const FMTmask& presolvedmask,
			const std::vector<FMTtheme>& newthemes, const std::vector<FMTaction>&presolvedaction) const;
		// DocString: FMTschedule::postsolve
		/**
		Postsolve all masks base on a basemask and the original themes and actions.
		*/
		FMTschedule postsolve(const FMTmask& presolvedmask,
			const std::vector<FMTtheme>& originalbasethemes, const std::vector<FMTaction>&originalbasebaseactions) const;
		/**
		Functions that make sure the FMTschedule acts like a std c++ container
		*/
		typedef typename std::map<FMTaction, std::map<FMTdevelopment, std::vector<double>>>::value_type value_type;
		typedef typename std::map<FMTaction, std::map<FMTdevelopment, std::vector<double>>>::iterator iterator;
		typedef typename std::map<FMTaction, std::map<FMTdevelopment, std::vector<double>>>::const_iterator const_iterator;
		std::map<FMTdevelopment, std::vector<double>>& operator[](const FMTaction& action);
		const std::map<FMTdevelopment, std::vector<double>>& at(const FMTaction& action) const;
		iterator find(const FMTaction& actionkey);
		const_iterator find(const FMTaction& actionkey) const;
		iterator begin();
		const_iterator begin() const;
		iterator  end();
		const_iterator end() const;
    };


}
BOOST_CLASS_EXPORT_KEY(Core::FMTschedule);
#endif // FMTschedule_H_INCLUDED
