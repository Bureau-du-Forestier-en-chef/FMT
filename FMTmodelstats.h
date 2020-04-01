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
#ifndef FMTmodelstats_H_INCLUDED
#define FMTmodelstats_H_INCLUDED

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <string>

namespace Models
{
	// DocString: FMTmodelstats
	/**
	This class is used to report the stats of a FMTmodel. Used alot with the presolve and postsolve functions.
	Calling presolve or postsolve will changes the model stats.
	*/
	class FMTmodelstats
	{
	friend class boost::serialization::access;
	// DocString: FMTmodelstats::serialize
	/**
	Serialize is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		ar & BOOST_SERIALIZATION_NVP(themes);
		ar & BOOST_SERIALIZATION_NVP(themesdata);
		ar & BOOST_SERIALIZATION_NVP(actions);
		ar & BOOST_SERIALIZATION_NVP(actionsdata);
		ar & BOOST_SERIALIZATION_NVP(transitions);
		ar & BOOST_SERIALIZATION_NVP(transitionsdata);
		ar & BOOST_SERIALIZATION_NVP(yieldsdata);
		ar & BOOST_SERIALIZATION_NVP(lifespansdata);
		ar & BOOST_SERIALIZATION_NVP(outputs);
		ar & BOOST_SERIALIZATION_NVP(outputsdata);
		ar & BOOST_SERIALIZATION_NVP(constraints);
		ar & BOOST_SERIALIZATION_NVP(constraintsdata);
		}
	public:
		// DocString : FMTmodelstats::themes
		/// The number of themes the FMTmodel.
		int themes;
		// DocString : FMTmodelstats::themesdata
		/// The themes's data size.
		int themesdata;
		// DocString : FMTmodelstats::actions
		/// The number of actions of the FMTmodel.
		int actions;
		// DocString : FMTmodelstats::actionsdata
		/// The actions's data size.
		int actionsdata;
		// DocString : FMTmodelstats::transitions
		/// The number of transitions of the FMTmodel.
		int transitions;
		// DocString : FMTmodelstats::transitionsdata
		/// The transitions's data size.
		int transitionsdata;
		// DocString : FMTmodelstats::yieldsdata
		/// The yields's data size.
		int yieldsdata;
		// DocString : FMTmodelstats::lifespansdata
		/// The lifespans's data size.
		int lifespansdata;
		// DocString : FMTmodelstats::outputs
		/// The number of outputs of the FMTmodel.
		int outputs;
		// DocString : FMTmodelstats::outputsdata
		/// The outputsdata's data size.
		int outputsdata;
		// DocString : FMTmodelstats::constraints
		/// The number of constraints of the FMTmodel.
		int constraints;
		// DocString : FMTmodelstats::constraintsdata
		/// The constraintsdata's data size.
		int constraintsdata;
		// DocString : FMTmodelstats(const size_t,const size_t,const size_t, const size_t,const size_t,const size_t, const size_t, const size_t,const size_t, const size_t,const size_t,const size_t)
		/**
		FMTmodelstats constructor for complete stats.
		*/
		FMTmodelstats(const size_t& lthemes,const size_t& lthemesdata, const size_t& lactions, const size_t& lactionsdata,
			const size_t& ltransitions, const size_t& ltransitionsdata, const size_t& lyieldsdata,const size_t& llifespansdata,
			const size_t& loutputs, const size_t& loutputsdata,const size_t& lconstraints, const size_t& lconstraintsdata);
		// DocString : FMTmodelstats()
		/**
		FMTmodelstats default constructor.
		*/
		FMTmodelstats() = default;
		// DocString : FMTmodelstats(const FMTmodelstats)
		/**
		FMTmodelstats default copy constructor.
		*/
		FMTmodelstats(const FMTmodelstats& rhs) = default;
		// DocString : ~FMTmodelstats()
		/**
		FMTmodelstats destructor.
		*/
		~FMTmodelstats() = default;
		// DocString : FMTmodelstats::operator=
		/**
		FMTmodelstats default copy assignment.
		*/
		FMTmodelstats& operator = (const FMTmodelstats& rhs) = default;
		// DocString : FMTmodelstats::operator+=
		/**
		FMTmodelstats addition assignment with an other FMTmodelstats (rhs)
		*/
		FMTmodelstats& operator += (const FMTmodelstats& rhs);
		// DocString : FMTmodelstats::operator-=
		/**
		FMTmodelstats substraction assignment with an other FMTmodelstats (rhs)
		*/
		FMTmodelstats& operator -= (const FMTmodelstats& rhs);
		// DocString : FMTmodelstats::operator+
		/**
		FMTmodelstats copy addition assignment with an other FMTmodelstats (rhs)
		*/
		FMTmodelstats operator + (const FMTmodelstats& rhs);
		// DocString : FMTmodelstats::operator-
		/**
		FMTmodelstats copy substraction assignment with an other FMTmodelstats (rhs)
		*/
		FMTmodelstats operator - (const FMTmodelstats& rhs);
		// DocString : FMTmodelstats::operator==
		/**
		FMTmodelstats equality operator.
		*/
		bool operator == (const FMTmodelstats& rhs) const;
		// DocString : FMTmodelstats::operator!=
		/**
		FMTmodelstats non equality operator.
		*/
		bool operator != (const FMTmodelstats& rhs) const;
		// DocString : FMTmodelstats::operator std::string()
		/**
		FMTmodelstats to string operator.
		*/
		operator std::string() const;
		
	};
}

#endif