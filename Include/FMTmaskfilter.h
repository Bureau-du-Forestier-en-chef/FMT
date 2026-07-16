/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTMASKFILTER_Hm_included
#define FMTMASKFILTER_Hm_included


#include <boost/dynamic_bitset.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <vector>
#include  <boost/functional/hash.hpp>
#include "FMTutility.h"


namespace Core
{
	class FMTMask;
	class FMTTheme;


class FMTEXPORT FMTMaskFilter
    {
	friend class boost::serialization::access;
	friend class FMTTheme;
	friend class FMTMask;
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
        boost::dynamic_bitset<uint8_t> selection;
        boost::dynamic_bitset<uint8_t> flippedselection;
		//std::vector<size_t>index;
    public:
        FMTMaskFilter();
        virtual ~FMTMaskFilter()=default;
        FMTMaskFilter(const FMTMaskFilter& rhs);
        FMTMaskFilter& operator = (const FMTMaskFilter& rhs);
		FMTMaskFilter(const FMTMask& presolveselection, const FMTMask& buffermask);
		FMTMaskFilter(const FMTMask& presolveselection);
        FMTMaskFilter(std::vector<FMTMask>& masks);
        FMTMaskFilter(std::vector<FMTMask>& masks,const std::vector<FMTTheme>& themes);
        FMTMask filter(const FMTMask& devmask) const;
		void swap(FMTMaskFilter& rhs);
		FMTMaskFilter presolve(const std::vector<FMTTheme>& themes) const;
		bool canPresolve(const FMTMask& mask, const std::vector<const Core::FMTTheme*>& themes) const;
		std::vector<const Core::FMTTheme*> getSelectedThemes(const std::vector<Core::FMTTheme>& themes) const;
		inline bool emptyFlipped() const
			{
			return flippedselection.empty();
			}
		inline bool empty() const
			{
			return selection.empty();
			}
		inline size_t hash() const
			{
			return (boost::hash<boost::dynamic_bitset<uint8_t>>()(selection) ^ boost::hash<boost::dynamic_bitset<uint8_t>>()(flippedselection));
			}
    };



}

namespace boost {

	template <>
	struct hash<Core::FMTMaskFilter>
	{
		std::size_t operator()(const Core::FMTMaskFilter& filter) const
		{

			return (filter.hash());
		}
	};


}

#endif // FMTMASKFILTER_Hm_included
