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
