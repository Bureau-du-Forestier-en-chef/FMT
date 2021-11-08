/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTMASKFILTER_H_INCLUDED
#define FMTMASKFILTER_H_INCLUDED


#include <boost/dynamic_bitset.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <vector>
#include  <boost/functional/hash.hpp>


namespace Core
{
	class FMTmask;
	class FMTtheme;


class FMTmaskfilter
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		/*ar & BOOST_SERIALIZATION_NVP(fullset);*/
		ar & BOOST_SERIALIZATION_NVP(selection);
		ar & BOOST_SERIALIZATION_NVP(flippedselection);
		//ar & BOOST_SERIALIZATION_NVP(index);
		}
    protected:
		/*boost::dynamic_bitset<> fullset;*/
        boost::dynamic_bitset<> selection;
        boost::dynamic_bitset<> flippedselection;
		//std::vector<size_t>index;
    public:
        FMTmaskfilter();
        virtual ~FMTmaskfilter()=default;
        FMTmaskfilter(const FMTmaskfilter& rhs);
        FMTmaskfilter& operator = (const FMTmaskfilter& rhs);
        FMTmaskfilter(std::vector<FMTmask>& masks);
        FMTmaskfilter(std::vector<FMTmask>& masks,const std::vector<FMTtheme>& themes);
        FMTmask filter(const FMTmask& devmask) const;
        //bool within(const FMTmask& intersect) const;
		inline bool empty() const
			{
			return selection.empty();
			}
		inline size_t hash() const
			{
			return (boost::hash<boost::dynamic_bitset<>>()(selection) ^ boost::hash<boost::dynamic_bitset<>>()(flippedselection));
			}
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
