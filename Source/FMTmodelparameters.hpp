/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTmodelparameters_H_INCLUDED
#define FMTmodelparameters_H_INCLUDED

#include "FMTobject.hpp"
#include <boost/serialization/export.hpp>

namespace Models
{
    //DocString: FMTintmodelparameters
    //
    enum FMTintmodelparameters
    {
        LENGTH = 0,/**< The number of period to optimize or simulate */
        SEED, /**< The seed used for stochastique process in FMTsamodel, FMTnssmodel and FMTsesmodel */
        NUMBER_OF_ITERATIONS,/**< The number of iterations to do in FMTsesmodel::greedyreferencebuild  */
        PRESOLVE_ITERATIONS,/**< The number of iterations to do in FMTmodel::presolve */
        NUMBER_OF_THREADS,/**< Number of thread use by solver for optimisation */
        LastIntModelParam/**< End marker, used to allocate a fixed-sized array to store int parameters. */
    };
    //DocString: FMTdblmodelparameters
    //
    enum FMTdblmodelparameters
    {
        TOLERANCE = 0,/**< Double tolerance used in doplanning */
        GOALING_SCHEDULE_WEIGHT,/**< Not needed ... maybe more for task ....The weight to use when trying goal a schedule from a strategic model */
        LastDblModelParam/**< End marker, used to allocate a fixed-sized array to store double parameters. */
    };
    //DocString: FMTboolmodelparameters
    //
    enum FMTboolmodelparameters
    {
        FORCE_PARTIAL_BUILD = 0,/**< Force partial build of the graph if schedules are passed to doplanning */
        STRICTLY_POSITIVE,/**< Force matrix to have bound >= 0 for the outputs use in constraints or objective if coefficient is negative and it contains a yield for FMTlpmodel */
        POSTSOLVE,/**< Return a postsolved model for the doplanning */
        SHOW_LOCK_IN_SCHEDULES,//Not needed or change parameters in fonctions ... maybe more for task
        LastBoolModelParam/**< End marker, used to allocate a fixed-sized array to store bool parameters. */
    };


    // DocString: FMTmodelparameters
    /**
    FMTmodelparameters is a class which contains all the informations 
    a model need to be solve by the different types of FMTmodel.
    Default int parameters are : 
        LENGTH = 30
        SEED = 25
        NUMBER_OF_ITERATIONS = 10000
        PRESOLVE_ITERATIONS = 10
        NUMBER_OF_THREADS = 4
    Default double parameters are : 
        TOLERANCE = FMT_DBL_TOLERANCE
        GOALING_SCHEDULE_WEIGHT = 10000
    Default bool parameters are :
        FORCE_PARTIAL_BUILD = false
        STRICTLY_POSITIVE = true
        POSTSOLVE = true
        SHOW_LOCK_IN_SCHEDULES = false
    */
    class FMTEXPORT FMTmodelparameters : public Core::FMTobject
    {
        // DocString: FMTmodelparameters::serialize
        /**
        Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
        */
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
		try {
                ar & boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTobject>(*this));
                ar & BOOST_SERIALIZATION_NVP(intparameters);
                ar & BOOST_SERIALIZATION_NVP(dblparameters);
                ar & BOOST_SERIALIZATION_NVP(boolparameters);
                ar & BOOST_SERIALIZATION_NVP(compresstime);
	        }catch (...)
            {
                _exhandler->printexceptions("", "FMTmodelparameters::serialize", __LINE__, __FILE__);
            }
	    }
        private:
            std::array<int, LastIntModelParam> intparameters;
            std::array<double, LastDblModelParam> dblparameters;
            std::array<bool, LastBoolModelParam> boolparameters;
            std::vector<int> compresstime;
        public:
            // DocString: FMTmodelparameters()
            /**
            Default constructor
            */
            FMTmodelparameters();
            // DocString: FMTmodelparameters(const FMTmodelparameters&)
            /**
            Copy constructor
            */
            FMTmodelparameters(const FMTmodelparameters& rhs);
            // DocString: FMTmodelparameters::operator=(const FMTmodelparameters&)
            /**
            Copy assignment
            */
            FMTmodelparameters& operator = (const FMTmodelparameters& rhs); 
            // DocString: ~FMTmodelparameters
            /**
                Default desctructor of FMTmodelparameters.
		    */
            ~FMTmodelparameters()=default;
            // DocString: FMTmodelparameters(FMTmodelparameters&&)
            /**
            Default move constructor for FMTmodelparameters.
            */
            FMTmodelparameters(FMTmodelparameters&& rhs)=default;
            // DocString: FMTmodelparameters::operator=(FMTmodelparameters&& rhs) 
            /**
            Default move assignment for FMTmodelparameters.
            */
            FMTmodelparameters& operator =(FMTmodelparameters&& rhs) =default;
            //###Setter
            bool setintparameter(const FMTintmodelparameters& key,const int& value);
            bool setdblparameter(const FMTdblmodelparameters& key,const double& value);
            bool setboolparameter(const FMTboolmodelparameters& key,const bool& value);
            bool setperiodcompresstime(const int& period, const int& value);
            //###Getter
            int getintparameter(const FMTintmodelparameters& key) const;
            double getdblparameter(const FMTdblmodelparameters& key) const;
            bool getboolparameter(const FMTboolmodelparameters& key) const;
            int getperiodcompresstime(const int& period)const;
            std::vector<int> getcompresstime()const
            {
                return compresstime;
            }
    };
}
BOOST_CLASS_EXPORT_KEY(Models::FMTmodelparameters)
#endif // FMTmodelparameters_H_INCLUDED