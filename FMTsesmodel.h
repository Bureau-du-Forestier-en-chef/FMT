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
