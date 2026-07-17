/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTmodelparameters_Hm_included
#define FMTmodelparameters_Hm_included

#include "FMTExceptionHandler.h"
#include "FMTObject.h"
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
        SEED = 1, /**< The seed used for stochastique process in FMTSaModel, FMTNssModel and FMTSesModel */
        NUMBER_OF_ITERATIONS = 2,/**< The number of iterations to do in FMTSesModel::greedyReferenceBuild  */
        PRESOLVE_ITERATIONS = 3,/**< The number of iterations to do in FMTModel::presolve */
        NUMBER_OF_THREADS = 4,/**< Number of thread use by solver for optimisation */
		MATRIX_TYPE = 5,/**< matrix forest model TYPE I,II,III (1,2,3) */
        UPDATE = 6,/**< At which period the model update period stop (excluded) if stops at 2 (2 is part of optimization) but 1 in update */
        MAX_MOVES = 7,/**< Maximal number of moves allowed by SA */
        MAX_ACCEPTED_CYCLE_MOVES = 8, /**< Maximal number of accepted moves per cycle */
        MAX_CYCLE_MOVES = 9, /**< Maximal number of accepted moves per cycle */
        LastIntModelParam = 10/**< End marker, used to allocate a fixed-sized array to store int parameters. */
    };
    //DocString: FMTdblmodelparameters
    //
    enum FMTdblmodelparameters
    {
        TOLERANCE = 0,/**< Double tolerance used in doPlanning */
        GOALING_SCHEDULE_WEIGHT = 1,/**< Not needed ... maybe more for task ....The weight to use when trying goal a schedule from a strategic model */
        INITIAL_ACCEPTANCE_PROBABILITY = 2,
        LastDblModelParam = 3/**< End marker, used to allocate a fixed-sized array to store double parameters. */
    };
    //DocString: FMTboolmodelparameters
    //
    enum FMTboolmodelparameters
    {
        FORCE_PARTIAL_BUILD = 0,/**< Force partial build of the graph if schedules are passed to doPlanning */
        STRICTLY_POSITIVE = 1,/**< Force matrix to have bound >= 0 for the outputs use in constraints or objective if coefficient is negative and it contains a yield for FMTLpModel */
        POSTSOLVE = 2,/**< Return a postsolved model for the doPlanning */
        SHOW_LOCK_IN_SCHEDULES = 3,//Not needed or change parameters in fonctions ... maybe more for task
		PRESOLVE_CAN_REMOVE_STATIC_THEMES =4,//The presolve will allow the removing of static themes even in use (it can alter the area section).
        DEBUG_MATRIX = 5,//If true, when an error occur in lpmodel initialSolve or resolve, the matrix will be written. 
        SETSOLUTION_THROW = 6, //Fix pour setSolution pour ne pas throw lorsque c'est vrai TODO meilleur def
        QUIET_LOGGING = 7,
        LastBoolModelParam = 8/**< End marker, used to allocate a fixed-sized array to store bool parameters. */
    };
    //DocString: FMTstrmodelparameters
    //
    enum FMTstrmodelparameters
    {
        SOLVER_COLD_START = 0,/*Cold start parameters located in scenario folder*<  */
        SOLVER_WARM_START = 1,/*Warm start parameters location in scenario folder*<  */
        WORKING_DIRECTORY = 2,/*SA working directory*<  */
        LastStrModelParam = 3/**< End marker, used to allocate a fixed-sized array to store bool parameters. */
    };


    // DocString: FMTModelParameters
    /**
    FMTModelParameters is a class which contains all the informations 
    a model need to be solve by the different types of FMTModel.
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
    class FMTEXPORT FMTModelParameters : public Core::FMTObject
    {
        public:
            // DocString: FMTModelParameters()
            /**
            Default constructor
            */
            FMTModelParameters();
            // DocString: FMTModelParameters(const FMTModelParameters&)
            /**
            Copy constructor
            */
            FMTModelParameters(const FMTModelParameters& rhs);
            // DocString: FMTModelParameters::operator=(const FMTModelParameters&)
            /**
            Copy assignment
            */
            FMTModelParameters& operator = (const FMTModelParameters& rhs); 
            // DocString: ~FMTModelParameters
            /**
                Default desctructor of FMTModelParameters.
		    */
            ~FMTModelParameters()=default;
            // DocString: FMTModelParameters(FMTModelParameters&&)
            /**
            Default move constructor for FMTModelParameters.
            */
            FMTModelParameters(FMTModelParameters&& rhs)=default;
            // DocString: FMTModelParameters::operator=(FMTModelParameters&& rhs) 
            /**
            Default move assignment for FMTModelParameters.
            */
            FMTModelParameters& operator =(FMTModelParameters&& rhs) =default;
            // DocString: FMTModelParameters::swap(FMTModelParameters& rhs)
            /**
            Default move assignment for FMTModelParameters.
            */
            void swap(FMTModelParameters& rhs);
            //###Setter
            bool setIntParameter(FMTintmodelparameters key,const int& value);
            bool setDblParameter(FMTdblmodelparameters key,const double& value);
            bool setBoolParameter(FMTboolmodelparameters key,const bool& value);
            bool setStrParameter(FMTstrmodelparameters p_key, const std::string& p_value);
            bool setPeriodCompressTime(const int& period, const int& value);
            //###Getter
            int getIntParameter(FMTintmodelparameters key) const;
            double getDblParameter(FMTdblmodelparameters key) const;
            bool getBoolParameter(FMTboolmodelparameters key) const;
            const std::string& getStrParameter(FMTstrmodelparameters p_key) const;
            int getPeriodCompressTime(const int& period)const;
            std::vector<int> getCompressTime() const;
        private:
            friend class boost::serialization::access;
            // DocString: FMTModelParameters::serialize
           /**
           Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
           */
            template<class Archive>
            void serialize(Archive& ar, const unsigned int version)
            {
                try {
                    ar& boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTObject>(*this));
                    ar& BOOST_SERIALIZATION_NVP(m_intparameters);
                    ar& BOOST_SERIALIZATION_NVP(m_dblparameters);
                    ar& BOOST_SERIALIZATION_NVP(m_boolparameters);
                    ar& BOOST_SERIALIZATION_NVP(m_compresstime);
                }
                catch (...)
                {
                    _exhandler->printExceptions("", "FMTModelParameters::serialize", __LINE__, __FILE__);
                }
            }
            std::array<int, LastIntModelParam> m_intparameters;
            std::array<double, LastDblModelParam> m_dblparameters;
            std::array<bool, LastBoolModelParam> m_boolparameters;
            std::array<std::string, LastStrModelParam> m_strparameters;
            std::vector<int> m_compresstime;
    };
}
BOOST_CLASS_EXPORT_KEY(Models::FMTModelParameters)
#endif // FMTmodelparameters_Hm_included