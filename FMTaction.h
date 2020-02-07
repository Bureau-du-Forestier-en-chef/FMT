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
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
namespace Core
{

class FMTactioncomparator;

class FMTaction : public FMTlist<FMTspec>
    {
	friend class FMTactioncomparator;
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
		std::vector<std::string>aggregates;
        std::vector<std::string>partials;
		int agelowerbound, ageupperbound, periodlowerbound, periodupperbound;
		std::string name;
		bool lock, reset;
    public:
        FMTaction();
        virtual ~FMTaction() = default;
        FMTaction(const std::string& lname);
        FMTaction(const std::string& lname, const bool& lock,const bool& reset);
		void push_aggregate(const std::string& aggregate);
        void push_partials(const std::string& yield);
        FMTaction(const FMTaction& rhs);
        FMTaction& operator = (const FMTaction& rhs);
		void setbounds();
		int getagelowerbound() const;
		int getageupperbound() const;
		int getperiodlowerbound() const;
		int getperiodupperbound() const;
		inline size_t hash() const
			{
			return boost::hash<std::string>()(name);
			}
		inline std::string getname() const
			{
			return name;
			}
		inline bool dorespectlock() const
			{
			return lock;
			}
		inline bool isresetage() const
			{
			return reset;
			}
		std::vector<std::string>getaggregates() const;
		bool inperiod() const;
        bool operator < (const FMTaction& rhs) const;
        bool operator == (const FMTaction& rhs) const;
        bool operator != (const FMTaction& rhs) const;
		std::vector<std::string>getpartials() const;
        operator std::string() const;
		bool partial(const std::string& yield) const;
    };

class FMTactioncomparator
	{
	bool checkaggregate;
	std::string action_name;
	public:
		FMTactioncomparator(std::string name, bool lcheckaggregate = false);
		std::vector<const FMTaction*>getallaggregates(const std::vector<FMTaction>&actions,bool aggregateonly = false) const;
		bool operator()(const FMTaction& action) const;

	};

}

namespace boost {

  template <>
  struct hash<Core::FMTaction>
  {
    std::size_t operator()(const Core::FMTaction& act) const
        {
        return (act.hash());
        }
  };


}

#endif // FMTACT_H_INCLUDED
