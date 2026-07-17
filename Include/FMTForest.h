/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTFOREST_Hm_included
#define FMTFOREST_Hm_included


#include "FMTLayer.hpp"
#include "FMTDevelopment.h"
#include <iterator>
#include <numeric>

namespace Core {
	class FMTTheme;
	class FMTActualDevelopment;
	class FMTFuturDevelopment;
}


namespace Spatial
    {
    class FMTEXPORT FMTForest: public FMTLayer<Core::FMTDevelopment>
        {
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
			{
			ar & boost::serialization::make_nvp("layer", boost::serialization::base_object<FMTLayer<Core::FMTDevelopment>>(*this));
			}
        public:
            FMTForest();
            FMTForest(const FMTForest& rhs);
            FMTForest(const FMTLayer<Core::FMTDevelopment>& rhs);
            FMTForest& operator = (const FMTForest& rhs);
			std::vector<Core::FMTActualDevelopment>getArea() const;
			void setLastThemeWithArea(const std::vector<Core::FMTActualDevelopment>& p_devs,
										const std::vector<Core::FMTTheme>& p_themes);
        FMTForest getCopy(bool copyData = true) const;
        FMTForest grow() const;
		std::vector<FMTLayer<std::string>> getThemes(const std::vector<Core::FMTTheme>& themes) const;
        FMTLayer<int>getAge() const;
        FMTLayer<std::string>getLock() const;
		void setPeriod(int period);
		FMTForest presolve(const Core::FMTMaskFilter& filter, const std::vector<Core::FMTTheme>&presolvedthemes) const;
		FMTForest postSolve(const Core::FMTMaskFilter& filter, const std::vector<Core::FMTTheme>&originalbasethemes) const;
        };
    }
#endif // FMTFOREST_Hm_included
