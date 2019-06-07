#ifndef FMTACT_H_INCLUDED
#define FMTACT_H_INCLUDED

#include "FMTlist.h"
#include "FMTbounds.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
namespace Core
{

class FMTaction : public FMTlist<FMTspec>
    {
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
		ar & BOOST_SERIALIZATION_NVP(lock);
		ar & BOOST_SERIALIZATION_NVP(reset);
		}
        vector<string>partials;
		int agelowerbound, ageupperbound, periodlowerbound, periodupperbound;
    public:
        string name;
        bool lock,reset;
        FMTaction();
        virtual ~FMTaction() = default;
        FMTaction(string& lname);
        FMTaction(const string& lname);
        FMTaction(string& lname, bool lock,bool reset);
        bool push_partials(string yield);
        FMTaction(const FMTaction& rhs);
        FMTaction& operator = (const FMTaction& rhs);
		void setbounds();
		int getagelowerbound() const;
		int getageupperbound() const;
		int getperiodlowerbound() const;
		int getperiodupperbound() const;
		bool inperiod() const;
        bool operator < (const FMTaction& rhs) const;
        bool operator == (const FMTaction& rhs) const;
        bool operator != (const FMTaction& rhs) const;
        vector<string>getpartials() const;
        operator string() const;
		bool partial(const string& yield) const;
    };

class FMTactioncomparator
	{
		string action_name;
	public:
		FMTactioncomparator(string name);
		bool operator()(const FMTaction& action) const;

	};

}

namespace boost {

  template <>
  struct hash<Core::FMTaction>
  {
    std::size_t operator()(const Core::FMTaction& act) const
        {
        return (hash<string>()(act.name));
        }
  };


}

#endif // FMTACT_H_INCLUDED
