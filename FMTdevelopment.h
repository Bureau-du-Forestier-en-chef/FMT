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
