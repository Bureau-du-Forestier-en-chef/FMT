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
#include <iterator>
#include <numeric>


using namespace boost;
using namespace std;

namespace Spatial
    {
    class FMTforest: public FMTlayer<FMTdevelopment>
        {
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
			{
			ar & boost::serialization::make_nvp("layer", boost::serialization::base_object<FMTlayer<FMTdevelopment>>(*this));
			}
        public:
            FMTforest();
            FMTforest(const FMTforest& rhs);
            FMTforest(const FMTlayer<FMTdevelopment>& rhs);
            FMTforest& operator = (const FMTforest& rhs);
            vector<FMTactualdevelopment>getarea() const;
        FMTforest getcopy(bool copydata = true) const;
        FMTforest grow() const;
        FMTforest operate(const vector<FMTevent<FMTdevelopment>>& cuts,const FMTspatialaction& action,const FMTtransition& Transition,
                     const FMTyields& ylds,const vector<FMTtheme>& themes, boost::unordered_map<FMTdevelopment, FMTdevelopment>& cached_transitions, FMTschedule& schedule) const;
        vector<FMTlayer<string>> getthemes(const vector<FMTtheme>& themes) const;
        FMTlayer<int>getage() const;
        FMTlayer<string>getlock() const;
		void setperiod(int period);
        map<FMTaction,FMTforest> getschedule(const FMTschedule& selection,
			boost::unordered_map<FMTdevelopment, vector<bool>>& cached_operability,
                                          const FMTyields& yields = FMTyields(),
                                          bool schedule_only = true) const;
        FMTforest getallowable(const FMTspatialaction& targetaction,
                                const FMTdisturbancestack& disturbances) const;
        vector<FMTevent<FMTdevelopment>> buildharvest(const double& target,
                                    const FMTspatialaction& targetaction,
                                    default_random_engine& generator,
									const int& pass) const;
        };
    }
#endif // FMTFOREST_H_INCLUDED
