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
	class FMTmodelstats
	{
	friend class boost::serialization::access;
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
		int themes;
		int themesdata;
		int actions;
		int actionsdata;
		int transitions;
		int transitionsdata;
		int yieldsdata;
		int lifespansdata;
		int outputs;
		int outputsdata;
		int constraints;
		int constraintsdata;
		FMTmodelstats(const size_t& lthemes,const size_t& lthemesdata, const size_t& lactions, const size_t& lactionsdata,
			const size_t& ltransitions, const size_t& ltransitionsdata, const size_t& lyieldsdata,const size_t& llifespansdata,
			const size_t& loutputs, const size_t& loutputsdata,const size_t& lconstraints, const size_t& lconstraintsdata);
		FMTmodelstats() = default;
		FMTmodelstats(const FMTmodelstats& rhs) = default;
		~FMTmodelstats() = default;
		FMTmodelstats& operator = (const FMTmodelstats& rhs) = default;
		FMTmodelstats& operator += (const FMTmodelstats& rhs);
		FMTmodelstats& operator -= (const FMTmodelstats& rhs);
		FMTmodelstats operator + (const FMTmodelstats& rhs);
		FMTmodelstats operator - (const FMTmodelstats& rhs);
		bool operator == (const FMTmodelstats& rhs) const;
		bool operator != (const FMTmodelstats& rhs) const;
		operator std::string() const;
		
	};
}

#endif