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
    // DocString: FMTintmodelparameters
    //
    /**
    @brief Enumerator of the integer parameters of a FMTModel.
    */
    enum FMTintmodelparameters
    {
        LENGTH = 0,/**< The number of period to optimize or simulate */
        SEED = 1, /**< The seed used for stochastique process in FMTSaModel, FMTNssModel and FMTSesModel */
        NUMBER_OF_ITERATIONS = 2,/**< The number of iterations to do in FMTSesModel::greedyReferenceBuild  */
        PRESOLVE_ITERATIONS = 3,/**< The number of iterations to do in FMTModel::preSolve */
        NUMBER_OF_THREADS = 4,/**< Number of thread use by solver for optimisation */
		MATRIX_TYPE = 5,/**< matrix forest model TYPE I,II,III (1,2,3) */
        UPDATE = 6,/**< At which period the model update period stop (excluded) if stops at 2 (2 is part of optimization) but 1 in update */
        MAX_MOVES = 7,/**< Maximal number of moves allowed by SA */
        MAX_ACCEPTED_CYCLE_MOVES = 8, /**< Maximal number of accepted moves per cycle */
        MAX_CYCLE_MOVES = 9, /**< Maximal number of accepted moves per cycle */
        WARM_UP_ITERATIONS = 10,/**< Number of iterations in the SA warm up */
        MINIMAL_ACCEPTED_MOVES = 11,/**< Minimal number of accepted moves per cycle before a move is allowed */
        UPDATE_PERIOD_FACTOR = 12,/**< Multiplicator factor for period 1 */
        SOLUTION_MERGE_ITERATIONS = 13,/**< Number of iterations on the greedy merge */
        MAX_NON_ACCEPTED_MOVES_FOR_TABOU = 14,/**< Number of non accepted moves before a move becomes tabou */
        TABOU_FLUSH = 15,/**< Number of iterations before flushing the tabou */
        MOVE_SIZE_FACTOR = 16,/**< Factor applied to the move size */
        LastIntModelParam = 17/**< End marker, used to allocate a fixed-sized array to store int parameters. */
    };
    // DocString: FMTdblmodelparameters
    //
    /**
    @brief Enumerator of the double parameters of a FMTModel.
    */
    enum FMTdblmodelparameters
    {
        TOLERANCE = 0,/**< Double tolerance used in doPlanning */
        GOALING_SCHEDULE_WEIGHT = 1,/**< Not needed ... maybe more for task ....The weight to use when trying goal a schedule from a strategic model */
        INITIAL_ACCEPTANCE_PROBABILITY = 2,
        ANNEALING_RATE = 3,/**< Cooling rate used by the exponential cooling schedule in FMTsamodel */
        LastDblModelParam = 4/**< End marker, used to allocate a fixed-sized array to store double parameters. */
    };
    // DocString: FMTboolmodelparameters
    //
    /**
    @brief Enumerator of the boolean parameters of a FMTModel.
    */
    enum FMTboolmodelparameters
    {
        FORCE_PARTIAL_BUILD = 0,/**< Force partial build of the graph if schedules are passed to doPlanning */
        STRICTLY_POSITIVE = 1,/**< Force matrix to have bound >= 0 for the outputs use in constraints or objective if coefficient is negative and it contains a yield for FMTLpModel */
        POSTSOLVE = 2,/**< Return a postsolved model for the doPlanning */
        SHOW_LOCK_IN_SCHEDULES = 3,//Not needed or change parameters in fonctions ... maybe more for task
		PRESOLVE_CAN_REMOVE_STATIC_THEMES =4,//The preSolve will allow the removing of static themes even in use (it can alter the area section).
        DEBUG_MATRIX = 5,//If true, when an error occur in lpmodel initialSolve or resolve, the matrix will be written. 
        SETSOLUTION_THROW = 6, //Fix pour setSolution pour ne pas throw lorsque c'est vrai TODO meilleur def
        QUIET_LOGGING = 7,
        LastBoolModelParam = 8/**< End marker, used to allocate a fixed-sized array to store bool parameters. */
    };
    // DocString: FMTstrmodelparameters
    //
    /**
    @brief Enumerator of the string parameters of a FMTModel.
    */
    enum FMTstrmodelparameters
    {
        SOLVER_COLD_START = 0,/*Cold start parameters located in scenario folder*<  */
        SOLVER_WARM_START = 1,/*Warm start parameters location in scenario folder*<  */
        WORKING_DIRECTORY = 2,/*SA working directory*<  */
        LastStrModelParam = 3/**< End marker, used to allocate a fixed-sized array to store bool parameters. */
    };


    // DocString: FMTModelParameters
    /**
    @brief Class containing all the parameters a model needs to be solved by the different types of FMTModel.
    */
    class FMTEXPORT FMTModelParameters : public Core::FMTObject
    {
        public:
            // DocString: FMTModelParameters()
            /**
            @brief Default constructor for FMTModelParameters.
            */
            FMTModelParameters();
            // DocString: FMTModelParameters(const FMTModelParameters&)
            /**
            @brief Copy constructor for FMTModelParameters.
            @param[in] rhs the FMTModelParameters to copy.
            */
            FMTModelParameters(const FMTModelParameters& rhs);
            // DocString: FMTModelParameters::operator=(const FMTModelParameters&)
            /**
            @brief Copy assignment operator for FMTModelParameters.
            @param[in] rhs the FMTModelParameters to copy.
            @return a reference to this FMTModelParameters.
            */
            FMTModelParameters& operator = (const FMTModelParameters& rhs); 
            // DocString: ~FMTModelParameters
            /**
            @brief Default destructor for FMTModelParameters.
            */
            ~FMTModelParameters()=default;
            // DocString: FMTModelParameters(FMTModelParameters&&)
            /**
            @brief Default move constructor for FMTModelParameters.
            @param[in] rhs the FMTModelParameters to move from.
            */
            FMTModelParameters(FMTModelParameters&& rhs)=default;
            // DocString: FMTModelParameters::operator=(FMTModelParameters&& rhs) 
            /**
            @brief Default move assignment for FMTModelParameters.
            @param[in] rhs the FMTModelParameters to move from.
            @return a reference to this FMTModelParameters.
            */
            FMTModelParameters& operator =(FMTModelParameters&& rhs) =default;
            // DocString: FMTModelParameters::swap(FMTModelParameters& rhs)
            /**
            @brief Swap this FMTModelParameters with another.
            @param[in,out] rhs the FMTModelParameters to swap with.
            */
            void swap(FMTModelParameters& rhs);
            //###Setter
            // DocString: FMTModelParameters::setIntParameter
            /**
            @brief Set a integer parameter.
            @param[in] key the parameter key.
            @param[in] value the value to set.
            @return true if the parameter is set else false.
            */
            bool setIntParameter(FMTintmodelparameters key,const int& value);
            // DocString: FMTModelParameters::setDblParameter
            /**
            @brief Set a double parameter.
            @param[in] key the parameter key.
            @param[in] value the value to set.
            @return true if the parameter is set else false.
            */
            bool setDblParameter(FMTdblmodelparameters key,const double& value);
            // DocString: FMTModelParameters::setBoolParameter
            /**
            @brief Set a boolean parameter.
            @param[in] key the parameter key.
            @param[in] value the value to set.
            @return true if the parameter is set else false.
            */
            bool setBoolParameter(FMTboolmodelparameters key,const bool& value);
            // DocString: FMTModelParameters::setStrParameter
            /**
            @brief Set a string parameter.
            @param[in] p_key the parameter key.
            @param[in] p_value the value to set.
            @return true if the parameter is set else false.
            */
            bool setStrParameter(FMTstrmodelparameters p_key, const std::string& p_value);
            // DocString: FMTModelParameters::setPeriodCompressTime
            /**
            @brief Set the compress time for a given period.
            @param[in] period the period.
            @param[in] value the compress time value.
            @return true if the value is set else false.
            */
            bool setPeriodCompressTime(const int& period, const int& value);
            //###Getter
            // DocString: FMTModelParameters::getIntParameter
            /**
            @brief Return a integer parameter.
            @param[in] key the parameter key.
            @return the parameter value.
            */
            int getIntParameter(FMTintmodelparameters key) const;
            // DocString: FMTModelParameters::getDblParameter
            /**
            @brief Return a double parameter.
            @param[in] key the parameter key.
            @return the parameter value.
            */
            double getDblParameter(FMTdblmodelparameters key) const;
            // DocString: FMTModelParameters::getBoolParameter
            /**
            @brief Return a boolean parameter.
            @param[in] key the parameter key.
            @return the parameter value.
            */
            bool getBoolParameter(FMTboolmodelparameters key) const;
            // DocString: FMTModelParameters::getStrParameter
            /**
            @brief Return a string parameter.
            @param[in] p_key the parameter key.
            @return the parameter value.
            */
            const std::string& getStrParameter(FMTstrmodelparameters p_key) const;
            // DocString: FMTModelParameters::getPeriodCompressTime
            /**
            @brief Return the compress time for a given period.
            @param[in] period the period.
            @return the compress time value.
            */
            int getPeriodCompressTime(const int& period)const;
            // DocString: FMTModelParameters::getCompressTime
            /**
            @brief Return the compress time for each period.
            @return the compress time values.
            */
            std::vector<int> getCompressTime() const;
        private:
            friend class boost::serialization::access;
            // DocString: FMTModelParameters::serialize
            /**
            @brief Serialize the FMTModelParameters through its base FMTObject for multiprocessing across multiple cpus (pickle in Python).
            @tparam Archive the archive type.
            @param[in,out] ar the archive to serialize to or from.
            @param[in] version the serialization version.
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