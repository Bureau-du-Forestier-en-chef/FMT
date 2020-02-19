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

#ifndef FMTOSR_H_INCLUDED
#define FMTOSR_H_INCLUDED

#include "FMTbounds.h"
#include "FMTmask.h"
#include "FMTaction.h"
#include "FMTyields.h"
#include <vector>
#include "FMTdevelopment.h"
#include <boost/serialization/serialization.hpp>
#include <string>
#include "FMTaction.h"
#include "FMTutility.h"

namespace Core
{

class FMToutputsource : public FMTspec
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("specification", boost::serialization::base_object<FMTspec>(*this));
		ar & BOOST_SERIALIZATION_NVP(mask);
		ar & BOOST_SERIALIZATION_NVP(target);
		ar & BOOST_SERIALIZATION_NVP(action);
		ar & BOOST_SERIALIZATION_NVP(yield);
		ar & BOOST_SERIALIZATION_NVP(values);
	}
    FMTmask mask;
    FMTotar target;
	std::string action,yield;
	std::vector<double>values;
	bool average;
    public:
		FMToutputsource();
        FMToutputsource(const FMTotar ltarget,double lvalue = 0, std::string lyield= "", std::string laction = "");
        FMToutputsource(const FMTotar ltarget, std::vector<double>lvalues);
        FMToutputsource(const FMTspec& spec,const FMTmask& lmask,
               const FMTotar ltarget, std::string lyield= "",
               std::string laction = "");
        FMToutputsource(const FMToutputsource& rhs);
        FMToutputsource& operator = (const FMToutputsource& rhs);
        operator std::string() const override;
		bool operator == (const FMToutputsource& rhs) const;
		bool operator != (const FMToutputsource& rhs) const;
		bool operator < (const FMToutputsource& rhs) const;
		bool isinaggregate(const FMToutputsource& rhs, const std::vector<Core::FMTaction>& actions) const;
		bool issubsetof(const FMToutputsource& rhs, const std::vector<Core::FMTaction>& actions) const;
		bool issubsetof(const FMToutputsource& rhs) const;
		bool issamebutdifferentaction(const FMToutputsource& rhs) const;
		const FMTmask& getmask() const;
		void setmask(const FMTmask& newmask);
		std::string getaction() const;
		std::string getyield() const;
		FMTotar gettarget() const;
		double getvalue(int period = 0) const;
		bool isvariable() const;
		bool islevel() const;
		bool isconstant() const;
		bool isvariablelevel() const;
		std::string getlevel() const;
		bool isnull(const FMTyields& ylds) const;
		bool istimeyield() const;
		std::vector<const FMTaction*>targets(const std::vector<FMTaction>& actions) const;
		bool isinventory() const;
		bool useinedges() const;
		bool isnextperiod() const;
		bool useoutedges() const;
		double getcoef(const FMTdevelopment& development,
			const FMTyields& yields, const FMTaction& action,
			const std::vector<FMTdevelopmentpath>& paths) const;
		inline bool use(const FMTdevelopment& development, const FMTyields& ylds) const
			{
			return (mask && development.mask.issubsetof(mask) && development.is(*this, ylds));
			}
		FMToutputsource presolve(const FMTmask& presolvedmask,const std::vector<FMTtheme>& newthemes) const;
		void setaverage();
		bool isaverage() const;
		size_t hash(int period = -1) const;
    };

class FMToutputsourcecomparator
	{
		bool variable;
	public:
		FMToutputsourcecomparator(bool lvariable);
		bool operator()(const FMToutputsource& source) const;

	};



}



#endif // FMTOSR_H_INCLUDED
