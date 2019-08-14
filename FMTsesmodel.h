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

#ifndef FMTSESM_H_INCLUDED
#define FMTSESM_H_INCLUDED

#include "FMTmodel.h"
#include "FMTevent.h"
#include "FMTcut.h"
#include "FMTspatialaction.h"
#include "FMTlayer.h"
#include "FMTforest.h"
#include "FMTdisturbancestack.h"
#include <vector>
#include <map>
//#include <boost/serialization/export.hpp>
using namespace Spatial;

namespace Models
{

class FMTsesmodel : public FMTmodel
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		ar & boost::serialization::make_nvp("model", boost::serialization::base_object<FMTmodel>(*this));
		ar & BOOST_SERIALIZATION_NVP(mapping);
		ar & BOOST_SERIALIZATION_NVP(disturbances);
		ar & BOOST_SERIALIZATION_NVP(spactions);
		}
    protected:
        FMTforest mapping;
		vector<FMTschedule> operatedschedule;
        FMTdisturbancestack disturbances;
        vector<FMTspatialaction>spactions;//should be FMTmodel action pointer....
    public:
        FMTsesmodel();
        FMTsesmodel(const FMTsesmodel& rhs);
        FMTsesmodel(const FMTmodel& rhs);
        FMTsesmodel& operator = (const FMTsesmodel& rhs);
        FMTforest getmapping() const;
        FMTdisturbancestack getdisturbances() const;
        bool setinitialmapping(const FMTforest& forest);
        bool setspactions(const vector<FMTspatialaction>& lspactions);
        map<string,double> simulate(const FMTschedule& schedule,
                        bool schedule_only = true,
                        unsigned int seed = 0);
		string getdisturbancestats() const;
        vector<FMTschedule> getschedule() const;
    };

}

#endif // FMTSESM_H_INCLUDED
