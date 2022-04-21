/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTFOREST_H_INCLUDED
#define FMTFOREST_H_INCLUDED


#include "FMTlayer.hpp"
#include "FMTdevelopment.hpp"
#include <iterator>
#include <numeric>

namespace Core {
	class FMTtheme;
	class FMTactualdevelopment;
	class FMTfuturdevelopment;
}


namespace Spatial
    {
    class FMTEXPORT FMTforest: public FMTlayer<Core::FMTdevelopment>
        {
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
			{
			ar & boost::serialization::make_nvp("layer", boost::serialization::base_object<FMTlayer<Core::FMTdevelopment>>(*this));
			}
        public:
            FMTforest();
            FMTforest(const FMTforest& rhs);
            FMTforest(const FMTlayer<Core::FMTdevelopment>& rhs);
            FMTforest& operator = (const FMTforest& rhs);
			std::vector<Core::FMTactualdevelopment>getarea() const;
        FMTforest getcopy(bool copydata = true) const;
        FMTforest grow() const;
		std::vector<FMTlayer<std::string>> getthemes(const std::vector<Core::FMTtheme>& themes) const;
        FMTlayer<int>getage() const;
        FMTlayer<std::string>getlock() const;
		void setperiod(int period);
		FMTforest presolve(const Core::FMTmaskfilter& filter, const std::vector<Core::FMTtheme>&presolvedthemes) const;
		FMTforest postsolve(const Core::FMTmaskfilter& filter, const std::vector<Core::FMTtheme>&originalbasethemes) const;
        };
    }
#endif // FMTFOREST_H_INCLUDED
