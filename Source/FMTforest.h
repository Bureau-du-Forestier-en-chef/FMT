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
#include "FMTdisturbancestack.h"
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
        FMTforest operate(const std::vector<FMTsesevent<Core::FMTdevelopment>>& cuts,const FMTspatialaction& action,const Core::FMTtransition& Transition,
                     const Core::FMTyields& ylds,const std::vector<Core::FMTtheme>& themes, boost::unordered_map<Core::FMTdevelopment,Core::FMTdevelopment>& cached_transitions, Core::FMTschedule& schedule) const;
		std::vector<FMTlayer<std::string>> getthemes(const std::vector<Core::FMTtheme>& themes) const;
        FMTlayer<int>getage() const;
        FMTlayer<std::string>getlock() const;
		void setperiod(int period);
		std::map<Core::FMTaction,FMTforest> getschedule(const Core::FMTschedule& selection,
			boost::unordered_map<Core::FMTdevelopment, std::vector<bool>>& cached_operability,
                                          const Core::FMTyields& yields = Core::FMTyields(),
                                          bool schedule_only = true) const;
        FMTforest getallowable(const FMTspatialaction& targetaction,
                                const FMTdisturbancestack& disturbances) const;
		std::vector<FMTsesevent<Core::FMTdevelopment>> buildharvest(const double& target,
                                    const FMTspatialaction& targetaction,
									std::default_random_engine& generator,
									const int& pass) const;
		FMTforest presolve(const Core::FMTmask& selectedmask, const std::vector<Core::FMTtheme>&presolvedthemes) const;
		FMTforest postsolve(const Core::FMTmask& selectedmask, const std::vector<Core::FMTtheme>&originalbasethemes) const;
        };
    }
#endif // FMTFOREST_H_INCLUDED
