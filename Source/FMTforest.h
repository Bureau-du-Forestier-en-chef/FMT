/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTFOREST_H_INCLUDED
#define FMTFOREST_H_INCLUDED


#include "FMTcoordinate.h"
#include "FMTlayer.h"
#include "FMTschedule.h"
#include "FMTdevelopment.h"
#include "FMTactualdevelopment.h"
#include "FMTfuturdevelopment.h"
#include "FMTspatialaction.h"
#include "FMTcut.h"
#include "FMTmask.h"
#include "FMTtheme.h"
#include <iterator>
#include <numeric>


namespace Spatial
    {
    class FMTforest: public FMTlayer<Core::FMTdevelopment>
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
		FMTforest presolve(const Core::FMTmask& selectedmask, const std::vector<Core::FMTtheme>&presolvedthemes) const;
		FMTforest postsolve(const Core::FMTmask& selectedmask, const std::vector<Core::FMTtheme>&originalbasethemes) const;
		void passinobject(const Core::FMTobject& rhs) override;
        };
    }
#endif // FMTFOREST_H_INCLUDED
