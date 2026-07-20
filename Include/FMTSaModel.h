/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTSAMODEL_H
#define FMTSAMODEL_H

#include "FMTSeModel.h"
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
// DocString: FMTSaModel
/**
@brief Area restricted model (ARM) using the simulated annealing meta-heuristic to solve the spatial optimization problem.
@details Constraints must be goals with weight and the objective function must maximize or minimize those constraints. Only the functions needed to build a simulated annealing algorithm are implemented; the algorithm itself must be written by the user using the functions exposed in this class. An FMTForest is needed to set the initial map, an FMTSaSchedule is needed as cooling schedule and the FMTspatialaction must be set for the model.
*/

class FMTEXPORT FMTSaModel final: public FMTSeModel
    {
    public:
        // DocString: FMTSeModel::logMovesReport
        /**
        @brief Log the moves report.
        */
        void logMovesReport() const;
        // DocString: FMTSaModel::isProvenOptimal
        /**
        @brief Return true if the solution is optimal based on the termination criteria.
        @return true if the solution is proven optimal else false.
        */
        bool isProvenOptimal() const;
        // DocString: FMTSaModel::initialSolve
        /**
        @brief Try to solve the model from a cold start.
        @return true if the initial solve succeeded else false.
        */
        bool initialSolve();
        // DocString: FMTSaModel::build
        /**
        @brief Try to build the spatial schedule from a schedule if one is provided, otherwise randomly build the model to be ready to solve.
        @param[in] schedules the schedules.
        @return true if the build succeeded else false.
        */
        virtual bool build(std::vector<Core::FMTSchedule> schedules = std::vector<Core::FMTSchedule>());
        // DocString: FMTSaModel::solve
        /**
        @brief Call initialSolve on the solver.
        @return true if the solve succeeded else false.
        */
        virtual bool solve();
        // DocString: FMTSaModel::presolve
        /**
        @brief Return a presolved copy of the model using a vector of developments and the actual transitions, reducing the model data if it is badly formulated.
        @param[in] optionaldevelopments the optional developments.
        @return the presolved model.
        */
        virtual std::unique_ptr<FMTModel>presolve(std::vector<Core::FMTActualDevelopment> optionaldevelopments = std::vector<Core::FMTActualDevelopment>()) const;
        // DocString: FMTSaModel::getCopy
        /**
        @brief Return a copy of the FMTModel for the selected period.
        @details For period 0 returns FMTModel::getCopy; for period greater than 0 returns a copy based on the developments of the FMTGraph. Needs a built graph with a solution.
        @param[in] period the period.
        @return the copied model.
        */
        virtual std::unique_ptr<FMTModel> getCopy(int period = 0) const;
        // DocString: FMTSaModel()
        /**
        @brief Default constructor for FMTSaModel.
        */
        FMTSaModel()=default;
        // DocString: ~FMTSaModel()
        /**
        @brief Destructor for FMTSaModel.
        */
        ~FMTSaModel();
        // DocString: FMTSaModel(const FMTSaModel)
        /**
        @brief Copy constructor for FMTSaModel.
        @param[in] rhs the FMTSaModel to copy.
        */
        FMTSaModel(const FMTSaModel& rhs);
        // DocString: FMTSaModel(const FMTModel)
        /**
        @brief Construct a FMTSaModel from a FMTModel, using the parent as argument.
        @param[in] rhs the model.
        */
        FMTSaModel(const FMTModel& rhs);
        // DocString: FMTSaModel(const FMTModel, const FMTForest)
        /**
        @brief Construct a FMTSaModel from a FMTModel and a forest.
        @param[in] rhs the model.
        @param[in] forest the forest.
        */
        FMTSaModel(const FMTModel& rhs, const Spatial::FMTForest& forest);
        // DocString: FMTSaModel::operator=
        /**
        @brief Copy assignment operator for FMTSaModel.
        @param[in] rhs the FMTSaModel to copy.
        @return a reference to this FMTSaModel.
        */
        FMTSaModel& operator = (const FMTSaModel& rhs);
        // DocString: FMTSaModel::clone
        /**
        @brief Get a clone of the FMTSaModel.
        @return a unique pointer to the cloned model.
        */
        virtual std::unique_ptr<FMTModel>clone() const final;

        // DocString: FMTSaModel::buildPeriod
        /**
        @brief Build a period of the model.
        @return the graph stats.
        */
        Graph::FMTGraphStats buildPeriod();
    protected:
        // DocString: FMTSaModel::swapPtr
        /**
        @brief Swap this model with an abstract FMTModel.
        @param[in,out] rhs the unique pointer to swap with.
        */
        virtual void swapPtr(std::unique_ptr<FMTModel>& rhs);
    private:
    // DocString: FMTSaModel::Serialize
    /**
    @brief Serialize the FMTSaModel through its base FMTSeModel for multiprocessing across multiple cpus (pickle in Python).
    @tparam Archive the archive type.
    @param[in,out] ar the archive to serialize to or from.
    @param[in] version the serialization version.
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
    // DocString: FMTSaModel::getMovesName
    /**
    @brief Return the name of a move.
    @param[in] p_move the move.
    @return the move name.
    */
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
    // DocString: FMTSaModel(const FMTSeModel)
    /**
    @brief Construct a FMTSaModel from a FMTSeModel, for presolve use.
    @param[in] rhs the FMTSeModel.
    */
    FMTSaModel(const FMTSeModel& rhs);
    // DocString: FMTSaModel::_doWriteDisturbances
    /**
    @brief Check if writing the disturbances is needed.
    @return true if the disturbances must be written else false.
    */
    bool _doWriteDisturbances() const;
    // DocString: FMTSaModel::_writeDisrturbances
    /**
    @brief Write the disturbances of the best solution.
    */
    void _writeDisrturbances() const;
        // DocString: FMTSaModel::getFromBindings
        /**
        @brief Get the selected actions from the bindings.
        @param[in] bindingactions the action bindings.
        @param[in] adjacency if true considers adjacency.
        @return a vector of booleans for the selected actions.
        */
        std::vector<bool> _getFromBindings(const Spatial::FMTSpatialSchedule::actionbindings& bindingactions, bool adjacency = false) const;
        // DocString: FMTSaModel::getCycleMoves
        /**
        @brief Get the total number of moves of the last cycle.
        @return the number of moves of the last cycle.
        */
        size_t _getCycleMoves() const;
        // DocString: FMTSaModel::getAcceptedCycleMoves
        /**
        @brief Get the number of accepted moves of the last cycle.
        @return the number of accepted moves of the last cycle.
        */
        size_t _getAcceptedCycleMoves() const;
        // DocString: FMTSaModel::allowDestruction
        /**
        @brief Return true if the bindings allow to destroy some events by area conflict.
        @param[in] actual the actual solution.
        @param[in] bindings the action bindings.
        @return true if area destruction is allowed else false.
        */
        bool _allowAreaDestruction(const Spatial::FMTSpatialSchedule& actual, const Spatial::FMTSpatialSchedule::actionbindings& bindings) const;
        // DocString: FMTSaModel::_allowAdjacencyDestruction
        /**
        @brief Return true if the bindings allow to destroy some events by adjacency conflict.
        @param[in] actual the actual solution.
        @param[in] bindings the action bindings.
        @return true if adjacency destruction is allowed else false.
        */
        bool _allowAdjacencyDestruction(const Spatial::FMTSpatialSchedule& actual, const Spatial::FMTSpatialSchedule::actionbindings& bindings) const;
        // DocString: FMTSaModel::_allowGroupDestruction
        /**
        @brief Return true if the solution allows a group destruction move.
        @param[in] p_actual the solution to test.
        @return true if group destruction is allowed else false.
        */
        bool _allowGroupDestruction(const Spatial::FMTSpatialSchedule& p_actual) const;
        // DocString: FMTSaModel::_allowEventsSpread
        /**
        @brief Return true if events can be spread for the solution.
        @param[in] p_actual the solution to test.
        @return true if events spread is allowed else false.
        */
        bool _allowEventsSpread(const Spatial::FMTSpatialSchedule& p_actual) const;
        // DocString: FMTSaModel::allowMove
        /**
        @brief Check if the move can be allowed.
        @param[in] move the move.
        @return true if the move is allowed else false.
        */
        bool _allowMove(const FMTsamove& move) const;
        // DocString: FMTSaModel::allowAnyMove
        /**
        @brief Return true if any move can be done.
        @return true if a move can be done else false.
        */
        bool _allowAnyMove() const;
        // DocString: FMTSaModel::getAMove
        /**
        @brief Return coordinates that might be good candidates to disturb.
        @param[in] actual the actual solution.
        @param[in] bindings the action bindings.
        @return the selected move.
        */
        FMTsamove _getAMove(const Spatial::FMTSpatialSchedule& actual, const Spatial::FMTSpatialSchedule::actionbindings& bindings) const;
		// DocString: FMTSaModel::evaluate
		/**
		@brief Evaluate the actual and a candidate solution and return true if the candidate is chosen to replace the actual solution, based on a temperature.
		@param[in] p_candidatObjective the candidate objective value.
		@return true if the candidate is better else false.
		*/
		bool _isBetter(double p_candidatObjective) const;
        // DocString: FMTSaModel::doLocalMove
        /**
        @brief Do a local move, disturbing a random number of graphs at a random period.
        @param[in] actual the actual solution.
        @param[in] bindings the action bindings.
        @return the new spatial schedule.
        */
        Spatial::FMTSpatialSchedule _doLocalMove(const Spatial::FMTSpatialSchedule& actual,
            const Spatial::FMTSpatialSchedule::actionbindings& bindings) const;
        // DocString: FMTSaModel::doConflictDestruction
        /**
        @brief Destroy the conflicts for given periods and coordinates.
        @param[in] actual the actual solution.
        @param[in] bindings the action bindings.
        @param[in] selectionpool the selection pool of coordinates.
        @param[in] period the period.
        @return the new spatial schedule.
        */
        Spatial::FMTSpatialSchedule _doConflictDestruction(const Spatial::FMTSpatialSchedule& actual,
            const Spatial::FMTSpatialSchedule::actionbindings& bindings,
           std::vector<std::vector<Spatial::FMTCoordinate>> selectionpool, const int& period) const;
       // DocString: FMTSaModel::doEventsAreaConflictDestrutorMove
       /**
       @brief Destroy events that have an area conflict.
       @param[in] actual the actual solution.
       @param[in] bindings the action bindings.
       @return the new spatial schedule.
       */
        Spatial::FMTSpatialSchedule _doEventsAreaConflictDestrutorMove(const Spatial::FMTSpatialSchedule& actual,
            const Spatial::FMTSpatialSchedule::actionbindings& bindings) const;
        // DocString: FMTSaModel::doEventsAdjacencyConflictDestrutorMove
        /**
        @brief Destroy events that have an adjacency conflict.
        @param[in] actual the actual solution.
        @param[in] bindings the action bindings.
        @return the new spatial schedule.
        */
        Spatial::FMTSpatialSchedule _doEventsAdjacencyConflictDestrutorMove(const Spatial::FMTSpatialSchedule& actual,
            const Spatial::FMTSpatialSchedule::actionbindings& bindings) const;
        // DocString: FMTSaModel::doGroupsConflictDestrutorMove
        /**
        @brief Destroy events that have a group conflict.
        @param[in] p_actual the actual solution.
        @return the new spatial schedule.
        */
        Spatial::FMTSpatialSchedule _doGroupsConflictDestrutorMove(const Spatial::FMTSpatialSchedule& p_actual) const;
        // DocString: FMTSaModel::_doEventsSpread
        /**
        @brief Spread the same actions to other cells.
        @param[in] p_actual the actual solution.
        @return the new spatial schedule.
        */
        Spatial::FMTSpatialSchedule _doEventsSpread(const Spatial::FMTSpatialSchedule& p_actual) const;

        
        // DocString: FMTSaModel::move
        /**
        @brief Perturb a solution and produce a new one.
        @param[in] actual the actual solution.
        @param[in] bindings the action bindings.
        @return the new spatial schedule.
        */
		Spatial::FMTSpatialSchedule _move(const Spatial::FMTSpatialSchedule& actual,
						const Spatial::FMTSpatialSchedule::actionbindings& bindings) const;
		// DocString: FMTSaModel::warmup
		/**
		@brief Get an initial temperature using an initial probability close to one, a base solution and a number of iterations.
		@param[in] actual the actual solution.
		@param[in] bindings the action bindings.
		@return the initial temperature.
		*/
		double _warmup(const Spatial::FMTSpatialSchedule& actual,
			const Spatial::FMTSpatialSchedule::actionbindings& bindings);
        // DocString: FMTSaModel::initialgrow
        /**
        @brief Do an initial grow until the length of the model is reached with the actual solution.
        */
        void _initialGrow();
        // DocString: FMTSaModel::initialbuild
        /**
        @brief Call a random build if there is no solution.
        */
        void _randomBuild();
        // DocString: FMTSaModel::schedulesbuild
        /**
        @brief Build from the schedules if there is no solution.
        @param[in] schedules the schedules.
        */
        void _schedulesBuild(const std::vector<Core::FMTSchedule>&schedules);
        // DocString: FMTSaModel::_getLocalMoveSize
        /**
        @brief Generate the size of the local move.
        @return the local move size.
        */
        size_t _getLocalMoveSize() const;
        // DocString: FMTSaModel::_getMaximalMoveSize
        /**
        @brief Get the maximal move size based on the temperature.
        @param[in] p_MaxSize the maximum size.
        @return the maximal move size.
        */
        size_t _getMaximalMoveSize(size_t p_MaxSize) const;
        // DocString: FMTSaModel::_getRandomMoveSize
        /**
        @brief Get a random move size based on the temperature.
        @param[in] p_MaxSize the maximum size.
        @return the random move size.
        */
        size_t _getRandomMoveSize(size_t p_MaxSize) const;
        // DocString: FMTSaModel::getRebuild
        /**
        @brief Take the non spatial solution of the actual solution and rebuild the solution using greedyReferenceBuild.
        @param[in] actual the actual solution.
        @return the rebuilt spatial schedule.
        */
        Spatial::FMTSpatialSchedule _getRebuild(const Spatial::FMTSpatialSchedule& actual) const;
        // DocString: FMTSaModel::isCycleProvenOptimal
        /**
        @brief Return true if the solution is optimal based on the termination criteria of the actual temperature level.
        @return true if the cycle is proven optimal else false.
        */
        bool _isCycleProvenOptimal() const;
        // DocString: FMTSaModel::dofactorization
        /**
        @brief Do the constraint factorization.
        */
        void _doFactorization();
        // DocString: FMTSaModel::logSolutionStatus
        /**
        @brief Log the status of the best solution.
        */
        void _logSolutionStatus() const;
        // DocString: FMTSaModel::logTemperatureStatus
        /**
        @brief Log the temperature status and other useful information.
        */
        void _logCycleStatus() const;
        // DocString: FMTSaModel::coolDown
        /**
        @brief Cool down the annealer temperature.
        */
        void _coolDown();
        // DocString: FMTSaModel::updateFailedMoveCount
        /**
        @brief Update the failed move count using the not accepted moves count and the move stats.
        */
        void _UpdateFailedMoveCount();

        // DocString: FMTSaModel::_GetSchedules
        /**
        @brief Get the schedules of a spatial solution.
        @param[in] p_SpatialSchedule the spatial schedule.
        @param[in] withlock if true includes the locked developments.
        @return the schedules.
        */
        std::vector<Core::FMTSchedule>_GetSchedules(const Spatial::FMTSpatialSchedule& p_SpatialSchedule, bool withlock) const;

        #ifdef FMTWITHOSI
            // DocString: FMTSaModel::_getRandomLpModel
            /**
            @brief Return a random FMTLpModel built from a spatial schedule.
            @param[in] p_SpatialSchedule the spatial schedule.
            @return the random FMTLpModel.
            */
            Models::FMTLpModel _getRandomLpModel(const Spatial::FMTSpatialSchedule& p_SpatialSchedule) const;
        #endif

        // DocString: FMTSaModel::_setBestSolutionTo
        /**
        @brief Set the best solution and its objective value.
        @param[in,out] p_NewBestSolution the new best solution.
        @param[in] p_ObjectiveValue the objective value.
        */
        void _setBestSolutionTo(Spatial::FMTSpatialSchedule& p_NewBestSolution,
                                double p_ObjectiveValue);

        // DocString: FMTSaModel::_getConstraintsStats
        /**
        @brief Compute the constraint statistics of a solution.
        @param[in] p_NewBestSolution the solution.
        @param[out] p_Objective the objective value.
        @param[out] p_SpatialRatio the spatial ratio.
        @param[out] p_InventoryRatio the inventory ratio.
        @param[out] p_TotalRatiom the total ratio.
        @param[out] p_PrimalInf the primal infeasibility.
        */
        void _getConstraintsStats(const Spatial::FMTSpatialSchedule& p_NewBestSolution,double& p_Objective,
                                 double& p_SpatialRatio, double& p_InventoryRatio, double& p_TotalRatiom,
                                 double& p_PrimalInf) const;


        // DocString: FMTSaModel::_resetTabouMoves
        /**
        @brief Reset the tabou moves.
        */
        void _resetTabouMoves();

        // DocString: FMTSaModel::_getNonTabouMoves
        /**
        @brief Return the moves that are not tabou.
        @return the non tabou moves.
        */
        std::vector<FMTsamove> _getNonTabouMoves() const;

        // DocString: FMTSaModel::_allowMove
        /**
        @brief Return true if a move is allowed for the actual solution and bindings.
        @param[in] p_move the move.
        @param[in] p_actual the actual solution.
        @param[in] p_bindings the action bindings.
        @return true if the move is allowed else false.
        */
        bool _allowMove(FMTsamove p_move, const Spatial::FMTSpatialSchedule& p_actual,
            const Spatial::FMTSpatialSchedule::actionbindings& p_bindings) const;
	

    };
}

BOOST_CLASS_EXPORT_KEY(Models::FMTSaModel)

#endif // FMTSAMODEL_H
