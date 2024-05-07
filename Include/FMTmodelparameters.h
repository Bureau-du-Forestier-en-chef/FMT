/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTmodelparameters_H_INCLUDED
#define FMTmodelparameters_H_INCLUDED

#include "FMTexceptionhandler.h"
#include "FMTobject.h"
#include <boost/serialization/export.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/vector.hpp>
#include <array>
#include <vector>
#include <string>

namespace Models
{
    //DocString: FMTintmodelparameters
    //
    enum FMTintmodelparameters
    {
        LENGTH = 0,/**< The number of period to optimize or simulate */
        SEED = 1, /**< The seed used for stochastique process in FMTsamodel, FMTnssmodel and FMTsesmodel */
        NUMBER_OF_ITERATIONS = 2,/**< The number of iterations to do in FMTsesmodel::greedyreferencebuild  */
        PRESOLVE_ITERATIONS = 3,/**< The number of iterations to do in FMTmodel::presolve */
        NUMBER_OF_THREADS = 4,/**< Number of thread use by solver for optimisation */
		MATRIX_TYPE = 5,/**< matrix forest model TYPE I,II,III (1,2,3) */
        UPDATE = 6,/**< At which period the model update period stop (excluded) if stops at 2 (2 is part of optimization) but 1 in update */
        MAX_MOVES = 7,/**< Maximal number of moves allowed by SA */
        LastIntModelParam = 8/**< End marker, used to allocate a fixed-sized array to store int parameters. */
    };
    //DocString: FMTdblmodelparameters
    //
    enum FMTdblmodelparameters
    {
        TOLERANCE = 0,/**< Double tolerance used in doplanning */
        GOALING_SCHEDULE_WEIGHT = 1,/**< Not needed ... maybe more for task ....The weight to use when trying goal a schedule from a strategic model */
        LastDblModelParam = 2/**< End marker, used to allocate a fixed-sized array to store double parameters. */
    };
    //DocString: FMTboolmodelparameters
    //
    enum FMTboolmodelparameters
    {
        FORCE_PARTIAL_BUILD = 0,/**< Force partial build of the graph if schedules are passed to doplanning */
        STRICTLY_POSITIVE = 1,/**< Force matrix to have bound >= 0 for the outputs use in constraints or objective if coefficient is negative and it contains a yield for FMTlpmodel */
        POSTSOLVE = 2,/**< Return a postsolved model for the doplanning */
        SHOW_LOCK_IN_SCHEDULES = 3,//Not needed or change parameters in fonctions ... maybe more for task
		PRESOLVE_CAN_REMOVE_STATIC_THEMES =4,//The presolve will allow the removing of static themes even in use (it can alter the area section).
        DEBUG_MATRIX = 5,//If true, when an error occur in lpmodel initialsolve or resolve, the matrix will be written. 
        LastBoolModelParam = 6/**< End marker, used to allocate a fixed-sized array to store bool parameters. */
    };
    //DocString: FMTstrmodelparameters
    //
    enum FMTstrmodelparameters
    {
        SOLVER_COLD_START = 0,/*Cold start parameters located in scenario folder*<  */
        SOLVER_WARM_START = 1,/*Warm start parameters location in scenario folder*<  */
        LastStrModelParam = 2/**< End marker, used to allocate a fixed-sized array to store bool parameters. */
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
    Default str parameters are:
    */
    class FMTEXPORT FMTmodelparameters : public Core::FMTobject
    {
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
            bool setstrparameter(const FMTstrmodelparameters& p_key, const std::string& p_value);
            bool setperiodcompresstime(const int& period, const int& value);
            //###Getter
            int getintparameter(const FMTintmodelparameters& key) const;
            double getdblparameter(const FMTdblmodelparameters& key) const;
            bool getboolparameter(const FMTboolmodelparameters& key) const;
            const std::string& getstrparameter(const FMTstrmodelparameters& p_key) const;
            int getperiodcompresstime(const int& period)const;
            std::vector<int> getcompresstime() const;
        private:
            friend class boost::serialization::access;
            // DocString: FMTmodelparameters::serialize
           /**
           Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
           */
            template<class Archive>
            void serialize(Archive& ar, const unsigned int version)
            {
                try {
                    ar& boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTobject>(*this));
                    ar& BOOST_SERIALIZATION_NVP(m_intparameters);
                    ar& BOOST_SERIALIZATION_NVP(m_dblparameters);
                    ar& BOOST_SERIALIZATION_NVP(m_boolparameters);
                    ar& BOOST_SERIALIZATION_NVP(m_compresstime);
                }
                catch (...)
                {
                    _exhandler->printexceptions("", "FMTmodelparameters::serialize", __LINE__, __FILE__);
                }
            }
            std::array<int, LastIntModelParam> m_intparameters;
            std::array<double, LastDblModelParam> m_dblparameters;
            std::array<bool, LastBoolModelParam> m_boolparameters;
            std::array<std::string, LastStrModelParam> m_strparameters;
            std::vector<int> m_compresstime;
    };
}
BOOST_CLASS_EXPORT_KEY(Models::FMTmodelparameters)
#endif // FMTmodelparameters_H_INCLUDED