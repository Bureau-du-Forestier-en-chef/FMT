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

#ifndef FMTDEV_H_INCLUDED
#define FMTDEV_H_INCLUDED

#include "FMTmask.h"
#include "FMTlifespans.h"
#include "FMTtransition.h"
#include "FMTyields.h"
#include "FMTaction.h"
#include "FMTtransitionmask.h"
//#include "FMToutputsource.h"
#include "FMTdevelopmentpath.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include "FMTbounds.h"
#include "FMTobject.h"
#include <memory>
#include <vector>


namespace Core
{

class FMTfuturdevelopment;
class  FMTtransition;

class FMTdevelopment : public FMTobject
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(mask);
		ar & BOOST_SERIALIZATION_NVP(age);
		ar & BOOST_SERIALIZATION_NVP(lock);
		ar & BOOST_SERIALIZATION_NVP(period);
	}
    public:
        FMTmask mask;
        int age,lock,period;
        FMTdevelopment();
		FMTdevelopment(FMTdevelopment&& rhs) noexcept;
		FMTdevelopment& operator = (FMTdevelopment&& rhs);
        virtual ~FMTdevelopment() = default;
		virtual unique_ptr<FMTdevelopment> Clone() const;
        FMTdevelopment(FMTmask mask,int age,int lock);
        FMTdevelopment(FMTmask lmask,int lage,int llock, int lperiod);
        FMTdevelopment(const FMTdevelopment& rhs);
        FMTdevelopment& operator = (const FMTdevelopment& rhs);
        bool operator == (const FMTdevelopment& rhs) const;
        bool operator != (const FMTdevelopment& rhs) const;
        FMTfuturdevelopment grow() const;
        bool operable(const FMTaction& action,const FMTyields& ylds) const;
		bool anyoperable(const vector<const FMTaction*>& actions,const FMTyields& ylds) const;
        bool operator < (const FMTdevelopment& rhs) const;
        virtual operator string() const;
		virtual double getarea() const;
		FMTdevelopment clearlock() const;
		vector<FMTdevelopmentpath> operate(const FMTaction& action,
			const FMTtransition& Transition,
			const FMTyields& ylds,
			const vector<FMTtheme>& themes) const;
		//bool within(const FMToutputsource& output_source,const FMTyields& ylds) const;
		double getinventorycoef(const FMTyields& ylds, const string& target_yield) const;
		double getharvestcoef(const vector<FMTdevelopmentpath>& topaths,
			const FMTaction& action,const FMTyields& ylds,const string& target_yield) const;
		bool is(const FMTspec& specification, const FMTyields& ylds) const;
		bool worthtestingoperability(const FMTaction& action) const;
		vector<int> anyworthtestingoperability(const vector<const FMTaction*>& actions, const vector<int>& action_IDS) const;
		
    };

}

namespace boost {

  template <>
  struct hash<Core::FMTdevelopment>
  {
    std::size_t operator()(const Core::FMTdevelopment& dev) const
        {
		std::size_t seed = 0;
		boost::hash_combine(seed, hash<Core::FMTmask>()(dev.mask));
		boost::hash_combine(seed, hash<int>()(dev.age));
		boost::hash_combine(seed, hash<int>()(dev.lock));
		boost::hash_combine(seed, hash<int>()(dev.period));
		return seed;
        //return (hash<Core::FMTmask>()(dev.mask) ^ hash<int>()(dev.age) ^ (hash<int>()(dev.lock) ^ (hash<int>()(dev.period))));
        }
  };


}

#endif // FMTDEV_H_INCLUDED
