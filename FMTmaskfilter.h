#ifndef FMTMASKFILTER_H_INCLUDED
#define FMTMASKFILTER_H_INCLUDED

#include <boost/python.hpp>

using namespace boost::python;
using namespace std;

#include "FMTmask.h"
#include <boost/dynamic_bitset.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

namespace Core
{


class FMTmaskfilter
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		ar & BOOST_SERIALIZATION_NVP(fullset);
		ar & BOOST_SERIALIZATION_NVP(selection);
		ar & BOOST_SERIALIZATION_NVP(flippedselection);
		ar & BOOST_SERIALIZATION_NVP(index);
		}
    protected://Need to had a new object after calling resume so we can call is_subset and not iterate over everythin if it's not part of this!
		boost::dynamic_bitset<> fullset;
        boost::dynamic_bitset<> selection;
        boost::dynamic_bitset<> flippedselection;
        vector<int>index;
    public:
        FMTmaskfilter();
        virtual ~FMTmaskfilter()=default;
        FMTmaskfilter(const FMTmaskfilter& rhs);
        FMTmaskfilter& operator = (const FMTmaskfilter& rhs);
        FMTmaskfilter(vector<FMTmask>& masks);
        FMTmaskfilter(vector<FMTmask>& masks,const vector<FMTtheme>& themes);
        FMTmask filter(const FMTmask& devmask) const;
        bool within(const FMTmask& intersect);
		size_t hash() const;
    };



}

namespace boost {

	template <>
	struct hash<Core::FMTmaskfilter>
	{
		std::size_t operator()(const Core::FMTmaskfilter& filter) const
		{

			return (filter.hash());
		}
	};


}

#endif // FMTMASKFILTER_H_INCLUDED
