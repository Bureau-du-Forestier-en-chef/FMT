/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTFOREST_Hm_included
#define FMTFOREST_Hm_included


#include "FMTlayer.hpp"
#include "FMTdevelopment.h"
#include <iterator>
#include <numeric>

namespace Core {
	class FMTTheme;
	class FMTActualDevelopment;
	class FMTFuturDevelopment;
}


namespace Spatial
    {
    class FMTEXPORT FMTforest: public FMTlayer<Core::FMTDevelopment>
        {
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
			{
			ar & boost::serialization::make_nvp("layer", boost::serialization::base_object<FMTlayer<Core::FMTDevelopment>>(*this));
			}
        public:
            FMTforest();
            FMTforest(const FMTforest& rhs);
            FMTforest(const FMTlayer<Core::FMTDevelopment>& rhs);
            FMTforest& operator = (const FMTforest& rhs);
			std::vector<Core::FMTActualDevelopment>getArea() const;
			void setLastThemeWithArea(const std::vector<Core::FMTActualDevelopment>& p_devs,
										const std::vector<Core::FMTTheme>& p_themes);
        FMTforest getCopy(bool copyData = true) const;
        FMTforest grow() const;
		std::vector<FMTlayer<std::string>> getThemes(const std::vector<Core::FMTTheme>& themes) const;
        FMTlayer<int>getAge() const;
        FMTlayer<std::string>getLock() const;
		void setPeriod(int period);
		FMTforest presolve(const Core::FMTMaskFilter& filter, const std::vector<Core::FMTTheme>&presolvedthemes) const;
		FMTforest postSolve(const Core::FMTMaskFilter& filter, const std::vector<Core::FMTTheme>&originalbasethemes) const;
        };
    }
#endif // FMTFOREST_Hm_included
