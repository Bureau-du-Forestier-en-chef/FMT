/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTSAMODEL_H
#define FMTSAMODEL_H

#include "FMTsemodel.h"
#include <memory>
#include <vector>
#include <random>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>

namespace Spatial
{
	class FMTForest;
    class FMTSaSchedule;
    class FMTExponentialSchedule;
}


namespace Models
{
#ifdef FMTWITHOSI
    class FMTLpModel;
#endif
/**
This model is an area restricted model (ARM) using the simulated annealing
meta-heuristic to solve the spatial optimization problem. Constraints must
be goals with weight and the objective function must maximize or minimize
those constraints. Only the functions needed to build a simulated annealing
algorithm are implemented in this class, the algorithm must be written by the
user using the functions exposed in this class.

An FMTForest is needed to set the initial map. An FMTSaSchedule is needed as
cooling schedule and FMTspatialaction must be set for the model.
*/

class FMTEXPORT FMTSaModel final: public FMTSeModel
    {
    public:
        // DocString: FMTSeModel::logMovesReport
        /**
        Log the Moves report
        */
        void logMovesReport() const;
        // DocString: FMTSaModel::isProvenOptimal
        /**
        Return true if is optimal based on the termination criteria
        */
        bool isProvenOptimal() const;
        // DocString: FMTSaModel::initialSolve
        /**
        Try to solve the model from a coldstart.
        */
        bool initialSolve();
        // DocString: FMTSaModel::build
        /**
        This function TRY to build the solution FMTSchedule to spatialschedule if there's a schedule if not it will
        randomly build the model to be ready to solve.
        */
        virtual bool build(std::vector<Core::FMTSchedule> schedules = std::vector<Core::FMTSchedule>());
        // DocString: FMTSaModel::solve
        /**
        This function call initialSolve on the solver.
        */
        virtual bool solve();
        // DocString: FMTSaModel::presolve
        /**
        This function use a vector of developments and the actual transitions of the model and return new unique pointer to presolved FMTModel.
        The function can reduce the number of global themes/actions/transitions/yields/lifespans/outputs/constraints data if the model is badly formulated.
        */
        virtual std::unique_ptr<FMTModel>presolve(std::vector<Core::FMTActualDevelopment> optionaldevelopments = std::vector<Core::FMTActualDevelopment>()) const;
        // DocString: FMTSaModel::getCopy
        /**
        This function returns a copy of the FMTModel of the selected period.
        If period = 0 it returns the FMTModel::getCopy if period > 0 then it returns
        a copy of the FMTModel based on the developments of the FMTGraph of the FMTLpModel.
        Need to have a builded graph with a solution to use this function.
        */
        virtual std::unique_ptr<FMTModel> getCopy(int period = 0) const;
        FMTSaModel()=default;
        ///Destructor
        ~FMTSaModel();
        ///Copy constructor
        FMTSaModel(const FMTSaModel& rhs);
        ///Copy constructor to use parent as argument in constructor
        FMTSaModel(const FMTModel& rhs);
        ///Copy constructor to use parent as argument in constructor
        FMTSaModel(const FMTModel& rhs, const Spatial::FMTForest& forest);
        ///Copy assignment operator
        FMTSaModel& operator = (const FMTSaModel& rhs);
        // DocString: FMTSaModel::clone
        /**
        Get a clone of the FMTSaModel
        */
        virtual std::unique_ptr<FMTModel>clone() const final;

        Graph::FMTGraphStats buildPeriod();
    protected:
        // DocString: FMTSaModel::swapPtr
        /**
        Swap with an abstract FMTModel
        */
        virtual void swapPtr(std::unique_ptr<FMTModel>& rhs);
    private:
    // DocString: FMTSaModel::Serialize
    /**
    Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
    */
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar& boost::serialization::make_nvp("semodel", boost::serialization::base_object<FMTSeModel>(*this));
    }
    enum FMTsamove
    {
        Local = 1,
        AreaConflictDestrutor = 2,
        AdjacencyConflictDestrutor = 3,
        GroupsConflictDestructor = 4,
        EventsSpread = 5,
        //ReBuilder = 7,
        MoveCount = 6
    };
    static std::string getMovesName(FMTsamove p_move);
    class FMTMoveStats
        {
        public:
            FMTMoveStats() = default;
            FMTMoveStats(const FMTMoveStats& rhs) = default;
            FMTMoveStats& operator=(const FMTMoveStats& rhs) = default;
            // DocString: FMTSaModel::FMTMoveStats::MoveType
            //The move done
            FMTsamove MoveType;
            // DocString: FMTSaModel::FMTMoveStats::MoveSize
            //Number of element changed
            size_t MoveSize;
            // DocString: FMTSaModel::FMTMoveStats::ObjectiveImpact
            //The amount of objective change due to the move
            double ObjectiveImpact;
            // DocString: FMTSaModel::FMTMoveStats::Accepted
            //If the move was Accepted
            bool Accepted;
        };
    // DocString: FMTSaModel::totalMoves
    ///Total number of moves done
    size_t m_TotalMoves;
    // DocString: FMTSaModel::cycleMoves
    ///The move done during the last cycle
    mutable std::vector<FMTMoveStats>m_CycleMoves;
    // DocString: FMTSaModel::coolingSchedule
    ///Cooling schedule for simulated annealing algorithm.
    std::unique_ptr<Spatial::FMTSaSchedule>m_CoolingSchedule;
    // DocString: FMTSaModel::notAcceptedMovesCount
    ///Count the number of cycle the moves gave had no acceptance
    std::array<size_t, FMTsamove::MoveCount>m_NotAcceptedMovesCount;
    // DocString: FMTSaModel::m_BestObjective
    ///The objective value of the best solution
    double m_BestObjective;
    // DocString: FMTSaModel::WARM_UP_ITERATIONS
    ///Number of iterations in the warm up
    static const size_t m_WARM_UP_ITERATIONS = 1000;
    // DocString: FMTSaModel::MINIMAL_ACCEPTED_MOVES
    ///Number of iterations in the warm up
    static const size_t m_MINIMAL_ACCEPTED_MOVES = 3;
    // DocString: FMTSaModel::UPDATE_PERIOD_FACTOR
   ///Factor multiplicator for period 1
    static const size_t m_UPDATE_PERIOD_FACTOR = 5;
    // DocString: FMTSaModel::SOLUTION_MERGE_ITERATIONS
    ///Number of iterations on greedy merge
    static const size_t m_SOLUTION_MERGE_ITERATIONS = 200;
    // DocString: FMTSaModel::m_MAX_NON_ACCEPTED_MOVES_FOR_TABOU
    ///Number of iterations on greedy merge
    static const size_t m_MAX_NON_ACCEPTED_MOVES_FOR_TABOU = 5;
    // DocString: FMTSaModel::m_TABOU_FLUSH
    ///Number of iterations before flushing tabou
    static const size_t m_TABOU_FLUSH = 100000;
    // DocString: FMTSaModel::m_MOVE_SIZE_FACTOR
    ///Factor to put on move size
    static const size_t m_MOVE_SIZE_FACTOR = 20;
    // DocString: FMTSaModel()
    /**
    Constructor for presolve use
    */
    FMTSaModel(const FMTSeModel& rhs);
    // DocString: FMTSaModel::_doWriteDisturbances
    /**
    @brief check if dirtubances writing needed
    @return true or false
    */
    bool _doWriteDisturbances() const;
    // DocString: FMTSaModel::_writeDisrturbances
     /**
     @brief Write the disturbances of the best solution
     */
    void _writeDisrturbances() const;
        // DocString: FMTSaModel::getFromBindings
        /**
        Get the selected action from the bindings
        */
        std::vector<bool> _getFromBindings(const Spatial::FMTSpatialSchedule::actionbindings& bindingactions, bool adjacency = false) const;
        // DocString: FMTSaModel::getCycleMoves
        /**
        Get the total number of moves of the last cycle
        */
        size_t _getCycleMoves() const;
        // DocString: FMTSaModel::getAcceptedCycleMoves
        /**
        Get the number of accepted move of the last cycle
        */
        size_t _getAcceptedCycleMoves() const;
        // DocString: FMTSaModel::allowDestruction
        /**
        Returns true if the bindings allow to destroy some events
       */
        bool _allowAreaDestruction(const Spatial::FMTSpatialSchedule& actual, const Spatial::FMTSpatialSchedule::actionbindings& bindings) const;
        // DocString: FMTSaModel::_allowAdjacencyDestruction
           /**
           Returns true if the bindings allow to destroy some events
          */
        bool _allowAdjacencyDestruction(const Spatial::FMTSpatialSchedule& actual, const Spatial::FMTSpatialSchedule::actionbindings& bindings) const;
        // DocString: FMTSaModel::_allowGroupDestruction
        /**
        @param[in] the solution to test
        @return Returns true if allow group destruction move
        */
        bool _allowGroupDestruction(const Spatial::FMTSpatialSchedule& p_actual) const;
        // DocString: FMTSaModel::_allowEventsSpread
        /**
        @brief check if event can be spread
        @param[in] the solution to test
        @return Returns true allow events spread
        */
        bool _allowEventsSpread(const Spatial::FMTSpatialSchedule& p_actual) const;
        // DocString: FMTSaModel::allowMove
        /**
        Check If you can allow the move 
       */
        bool _allowMove(const FMTsamove& move) const;
        // DocString: FMTSaModel::allowAnyMove
        /**
        Return true if you can do a move
       */
        bool _allowAnyMove() const;
        // DocString: FMTSaModel::getAMove
        /**
        Will return coordinates that might be good candidat to disturb
        */
        FMTsamove _getAMove(const Spatial::FMTSpatialSchedule& actual, const Spatial::FMTSpatialSchedule::actionbindings& bindings) const;
		// DocString: FMTSaModel::evaluate
		/**
		evaluate the actual and a candidat solution and return true if the candidat solution is choose to replace
		the actual solution.Based on a temp.
		*/
		bool _isBetter(double p_candidatObjective) const;
        // DocString: FMTSaModel::doLocalMove
        /**
        Do a loval move and disturb a random number of graph at a random period
        */
        Spatial::FMTSpatialSchedule _doLocalMove(const Spatial::FMTSpatialSchedule& actual,
            const Spatial::FMTSpatialSchedule::actionbindings& bindings) const;
        // DocString: FMTSaModel::doConflictDestruction
        /**
        Destroy the conflicts for a given periods and coordinates
         */
        Spatial::FMTSpatialSchedule _doConflictDestruction(const Spatial::FMTSpatialSchedule& actual,
            const Spatial::FMTSpatialSchedule::actionbindings& bindings,
           std::vector<std::vector<Spatial::FMTCoordinate>> selectionpool, const int& period) const;
       // DocString: FMTSaModel::doEventsAreaConflictDestrutorMove
       /**
       Destroy events that have some area conflict
       */
        Spatial::FMTSpatialSchedule _doEventsAreaConflictDestrutorMove(const Spatial::FMTSpatialSchedule& actual,
            const Spatial::FMTSpatialSchedule::actionbindings& bindings) const;
        // DocString: FMTSaModel::doEventsAdjacencyConflictDestrutorMove
      /**
      Destroy events that have adjacency conflict
      */
        Spatial::FMTSpatialSchedule _doEventsAdjacencyConflictDestrutorMove(const Spatial::FMTSpatialSchedule& actual,
            const Spatial::FMTSpatialSchedule::actionbindings& bindings) const;
        // DocString: FMTSaModel::doGroupsConflictDestrutorMove
         /**
        @brief Destroy events that have group conflict
        @param[in] the actual solution
        @return a new solution
        */
        Spatial::FMTSpatialSchedule _doGroupsConflictDestrutorMove(const Spatial::FMTSpatialSchedule& p_actual) const;
        // DocString: FMTSaModel::_doEventsSpread
         /**
        @brief Spread same actions to other cells
        @param[in] the actual solution
        @return a new solution
        */
        Spatial::FMTSpatialSchedule _doEventsSpread(const Spatial::FMTSpatialSchedule& p_actual) const;

        
        // DocString: FMTSaModel::move
		/**
		Perturb a solution and produce a new one
		*/
		Spatial::FMTSpatialSchedule _move(const Spatial::FMTSpatialSchedule& actual,
						const Spatial::FMTSpatialSchedule::actionbindings& bindings) const;
		// DocString: FMTSaModel::warmup
		/**
		Using an initprobability close to one, a base solution and a bunch of iterations get a initial temperature.
		*/
		double _warmup(const Spatial::FMTSpatialSchedule& actual,
			const Spatial::FMTSpatialSchedule::actionbindings& bindings);
        // DocString: FMTSaModel::initialgrow
        /**
        Do an initial grow till you reach the length of the model with the actual solution
        */
        void _initialGrow();
        // DocString: FMTSaModel::initialbuild
        /**
        Call a random build if there's no solution
        */
        void _randomBuild();
        // DocString: FMTSaModel::schedulesbuild
        /**
        Call schedules if there's no solution
        */
        void _schedulesBuild(const std::vector<Core::FMTSchedule>&schedules);
        // DocString: FMTSaModel:GetLocalMoveSize
        /**
        Generate the size of the local move.
         */
        size_t _getLocalMoveSize() const;
        // DocString: FMTSaModel:_getMaximalMoveSize
        /**
        @brief get maximal move size based on temperature
        @param[in] p_maxSize
        @return max move size
         */
        size_t _getMaximalMoveSize(size_t p_MaxSize) const;
        // DocString: FMTSaModel:_getRandomMoveSize
       /**
       @brief get random move size based on temperature
       @param[in] p_maxSize
       @return move size
        */
        size_t _getRandomMoveSize(size_t p_MaxSize) const;
        // DocString: FMTSaModel::getRebuild
        /**
        Take the actual non spatial solution of the actual solution and then
        Rebuild the solution using greedyReferenceBuild
        */
        Spatial::FMTSpatialSchedule _getRebuild(const Spatial::FMTSpatialSchedule& actual) const;
        // DocString: FMTSaModel::isCycleProvenOptimal
        /**
        Return true if is optimal based on the termination criteria of the actual temp level
        */
        bool _isCycleProvenOptimal() const;
        // DocString: FMTSaModel::dofactorization
        /**
        Do the constraint factorization
        */
        void _doFactorization();
        // DocString: FMTSaModel::logSolutionStatus
        /**
        Log the status of the best solution
        */
        void _logSolutionStatus() const;
        // DocString: FMTSaModel::logTemperatureStatus
        /**
        Log The temperature status and other usefull informations
        */
        void _logCycleStatus() const;
        // DocString: FMTSaModel::coolDown
        /**
        Cool down the annealer temp
        */
        void _coolDown();
        // DocString: FMTSaModel::updateFailedMoveCount
        /**
        Update failed move count using NotAcceptedMovesCount and the move stats
        */
        void _UpdateFailedMoveCount();

        std::vector<Core::FMTSchedule>_GetSchedules(const Spatial::FMTSpatialSchedule& p_SpatialSchedule, bool withlock) const;

        #ifdef FMTWITHOSI
            Models::FMTLpModel _getRandomLpModel(const Spatial::FMTSpatialSchedule& p_SpatialSchedule) const;
        #endif

        void _setBestSolutionTo(Spatial::FMTSpatialSchedule& p_NewBestSolution,
                                double p_ObjectiveValue);

        void _getConstraintsStats(const Spatial::FMTSpatialSchedule& p_NewBestSolution,double& p_Objective,
                                 double& p_SpatialRatio, double& p_InventoryRatio, double& p_TotalRatiom,
                                 double& p_PrimalInf) const;


        void _resetTabouMoves();

        std::vector<FMTsamove> _getNonTabouMoves() const;

        bool _allowMove(FMTsamove p_move, const Spatial::FMTSpatialSchedule& p_actual,
            const Spatial::FMTSpatialSchedule::actionbindings& p_bindings) const;
	

    };
}

BOOST_CLASS_EXPORT_KEY(Models::FMTSaModel)

#endif // FMTSAMODEL_H
