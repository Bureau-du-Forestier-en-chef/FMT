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
	class FMTforest;
    class FMTsaschedule;
    class FMTexponentialschedule;
}


namespace Models
{
#ifdef FMTWITHOSI
    class FMTlpmodel;
#endif
/**
This model is an area restricted model (ARM) using the simulated annealing
meta-heuristic to solve the spatial optimization problem. Constraints must
be goals with weight and the objective function must maximize or minimize
those constraints. Only the functions needed to build a simulated annealing
algorithm are implemented in this class, the algorithm must be written by the
user using the functions exposed in this class.

An FMTforest is needed to set the initial map. An FMTsaschedule is needed as
cooling schedule and FMTspatialaction must be set for the model.
*/

class FMTEXPORT FMTsamodel final: public FMTsemodel
    {
    public:
        // DocString: FMTsemodel::LogMovesReport
        /**
        Log the Moves report
        */
        void LogMovesReport() const;
        // DocString: FMTsamodel::isProvenOptimal
        /**
        Return true if is optimal based on the termination criteria
        */
        bool isProvenOptimal() const;
        // DocString: FMTsamodel::initialsolve
        /**
        Try to solve the model from a coldstart.
        */
        bool initialsolve();
        // DocString: FMTsamodel::build
        /**
        This function TRY to build the solution FMTschedule to spatialschedule if there's a schedule if not it will
        randomly build the model to be ready to solve.
        */
        virtual bool build(std::vector<Core::FMTschedule> schedules = std::vector<Core::FMTschedule>());
        // DocString: FMTsamodel::solve
        /**
        This function call initialsolve on the solver.
        */
        virtual bool solve();
        // DocString: FMTsamodel::presolve
        /**
        This function use a vector of developments and the actual transitions of the model and return new unique pointer to presolved FMTmodel.
        The function can reduce the number of global themes/actions/transitions/yields/lifespans/outputs/constraints data if the model is badly formulated.
        */
        virtual std::unique_ptr<FMTmodel>presolve(std::vector<Core::FMTactualdevelopment> optionaldevelopments = std::vector<Core::FMTactualdevelopment>()) const;
        // DocString: FMTsamodel::getcopy
        /**
        This function returns a copy of the FMTmodel of the selected period.
        If period = 0 it returns the FMTmodel::getcopy if period > 0 then it returns
        a copy of the FMTmodel based on the developements of the FMTgraph of the FMTlpmodel.
        Need to have a builded graph with a solution to use this function.
        */
        virtual std::unique_ptr<FMTmodel> getcopy(int period = 0) const;
        FMTsamodel()=default;
        ///Destructor
        ~FMTsamodel();
        ///Copy constructor
        FMTsamodel(const FMTsamodel& rhs);
        ///Copy constructor to use parent as argument in constructor
        FMTsamodel(const FMTmodel& rhs);
        ///Copy constructor to use parent as argument in constructor
        FMTsamodel(const FMTmodel& rhs, const Spatial::FMTforest& forest);
        ///Copy assignment operator
        FMTsamodel& operator = (const FMTsamodel& rhs);
        // DocString: FMTsamodel::clone
        /**
        Get a clone of the FMTsamodel
        */
        virtual std::unique_ptr<FMTmodel>clone() const final;

        Graph::FMTgraphstats buildperiod();
    protected:
        // DocString: FMTsamodel::swap_ptr
        /**
        Swap with an abstract FMTmodel
        */
        virtual void swap_ptr(std::unique_ptr<FMTmodel>& rhs);
    private:
    // DocString: FMTsamodel::Serialize
    /**
    Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
    */
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar& boost::serialization::make_nvp("semodel", boost::serialization::base_object<FMTsemodel>(*this));
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
    static std::string GetMovesName(FMTsamove p_move);
    class FMTmovestats
        {
        public:
            FMTmovestats() = default;
            FMTmovestats(const FMTmovestats& rhs) = default;
            FMTmovestats& operator=(const FMTmovestats& rhs) = default;
            // DocString: FMTsamodel::FMTmovestats::MoveType
            //The move done
            FMTsamove MoveType;
            // DocString: FMTsamodel::FMTmovestats::MoveSize
            //Number of element changed
            size_t MoveSize;
            // DocString: FMTsamodel::FMTmovestats::ObjectiveImpact
            //The amount of objective change due to the move
            double ObjectiveImpact;
            // DocString: FMTsamodel::FMTmovestats::Accepted
            //If the move was Accepted
            bool Accepted;
        };
    // DocString: FMTsamodel::TotalMoves
    ///Total number of moves done
    size_t m_TotalMoves;
    // DocString: FMTsamodel::CycleMoves
    ///The move done during the last cycle
    mutable std::vector<FMTmovestats>m_CycleMoves;
    // DocString: FMTsamodel::CoolingSchedule
    ///Cooling schedule for simulated annealing algorithm.
    std::unique_ptr<Spatial::FMTsaschedule>m_CoolingSchedule;
    // DocString: FMTsamodel::NotAcceptedMovesCount
    ///Count the number of cycle the moves gave had no acceptance
    std::array<size_t, FMTsamove::MoveCount>m_NotAcceptedMovesCount;
    // DocString: FMTsamodel::m_BestObjective
    ///The objective value of the best solution
    double m_BestObjective;
    // DocString: FMTsamodel::WARM_UP_ITERATIONS
    ///Number of iterations in the warm up
    static const size_t m_WARM_UP_ITERATIONS = 1000;
    // DocString: FMTsamodel::MINIMAL_ACCEPTED_MOVES
    ///Number of iterations in the warm up
    static const size_t m_MINIMAL_ACCEPTED_MOVES = 3;
    // DocString: FMTsamodel::UPDATE_PERIOD_FACTOR
   ///Factor multiplicator for period 1
    static const size_t m_UPDATE_PERIOD_FACTOR = 5;
    // DocString: FMTsamodel::SOLUTION_MERGE_ITERATIONS
    ///Number of iterations on greedy merge
    static const size_t m_SOLUTION_MERGE_ITERATIONS = 200;
    // DocString: FMTsamodel::m_MAX_NON_ACCEPTED_MOVES_FOR_TABOU
    ///Number of iterations on greedy merge
    static const size_t m_MAX_NON_ACCEPTED_MOVES_FOR_TABOU = 5;
    // DocString: FMTsamodel::m_TABOU_FLUSH
    ///Number of iterations before flushing tabou
    static const size_t m_TABOU_FLUSH = 100000;
    // DocString: FMTsamodel::m_MOVE_SIZE_FACTOR
    ///Factor to put on move size
    static const size_t m_MOVE_SIZE_FACTOR = 20;
    // DocString: FMTsamodel::INITIAL_ACCEPTANCE_PROBABILITY
    ///Initial acceptance probability of the SA
    static const double m_INITIAL_ACCEPTANCE_PROBABILITY;
    // DocString: FMTsamodel()
    /**
    Constructor for presolve use
    */
    FMTsamodel(const FMTsemodel& rhs);
    // DocString: FMTsamodel::_DoWriteDisturbances
    /**
    @brief check if dirtubances writing needed
    @return true or false
    */
    bool _DoWriteDisturbances() const;
    // DocString: FMTsamodel::_WriteDisrturbances
     /**
     @brief Write the disturbances of the best solution
     */
    void _WriteDisrturbances() const;
        // DocString: FMTsamodel::GetFromBindings
        /**
        Get the selected action from the bindings
        */
        std::vector<bool> _GetFromBindings(const Spatial::FMTSpatialSchedule::actionbindings& bindingactions, bool adjacency = false) const;
        // DocString: FMTsamodel::GetCycleMoves
        /**
        Get the total number of moves of the last cycle
        */
        size_t _GetCycleMoves() const;
        // DocString: FMTsamodel::GetAcceptedCycleMoves
        /**
        Get the number of accepted move of the last cycle
        */
        size_t _GetAcceptedCycleMoves() const;
        // DocString: FMTsamodel::AllowDestruction
        /**
        Returns true if the bindings allow to destroy some events
       */
        bool _AllowAreaDestruction(const Spatial::FMTSpatialSchedule& actual, const Spatial::FMTSpatialSchedule::actionbindings& bindings) const;
        // DocString: FMTsamodel::_AllowAdjacencyDestruction
           /**
           Returns true if the bindings allow to destroy some events
          */
        bool _AllowAdjacencyDestruction(const Spatial::FMTSpatialSchedule& actual, const Spatial::FMTSpatialSchedule::actionbindings& bindings) const;
        // DocString: FMTsamodel::_AllowGroupDestruction
        /**
        @param[in] the solution to test
        @return Returns true if allow group destruction move
        */
        bool _AllowGroupDestruction(const Spatial::FMTSpatialSchedule& p_actual) const;
        // DocString: FMTsamodel::_AllowEventsSpread
        /**
        @brief check if event can be spread
        @param[in] the solution to test
        @return Returns true allow events spread
        */
        bool _AllowEventsSpread(const Spatial::FMTSpatialSchedule& p_actual) const;
        // DocString: FMTsamodel::AllowMove
        /**
        Check If you can allow the move 
       */
        bool _AllowMove(const FMTsamove& move) const;
        // DocString: FMTsamodel::AllowAnyMove
        /**
        Return true if you can do a move
       */
        bool _AllowAnyMove() const;
        // DocString: FMTsamodel::GetAMove
        /**
        Will return coordinates that might be good candidat to disturb
        */
        FMTsamove _GetAMove(const Spatial::FMTSpatialSchedule& actual, const Spatial::FMTSpatialSchedule::actionbindings& bindings) const;
		// DocString: FMTsamodel::evaluate
		/**
		Evaluate the actual and a candidat solution and return true if the candidat solution is choose to replace
		the actual solution.Based on a temp.
		*/
		bool _IsBetter(double p_candidatObjective) const;
        // DocString: FMTsamodel::DoLocalMove
        /**
        Do a loval move and disturb a random number of graph at a random period
        */
        Spatial::FMTSpatialSchedule _DoLocalMove(const Spatial::FMTSpatialSchedule& actual,
            const Spatial::FMTSpatialSchedule::actionbindings& bindings) const;
        // DocString: FMTsamodel::DoConflictDestruction
        /**
        Destroy the conflicts for a given periods and coordinates
         */
        Spatial::FMTSpatialSchedule _DoConflictDestruction(const Spatial::FMTSpatialSchedule& actual,
            const Spatial::FMTSpatialSchedule::actionbindings& bindings,
           std::vector<std::vector<Spatial::FMTcoordinate>> selectionpool, const int& period) const;
       // DocString: FMTsamodel::DoEventsAreaConflictDestrutorMove
       /**
       Destroy events that have some area conflict
       */
        Spatial::FMTSpatialSchedule _DoEventsAreaConflictDestrutorMove(const Spatial::FMTSpatialSchedule& actual,
            const Spatial::FMTSpatialSchedule::actionbindings& bindings) const;
        // DocString: FMTsamodel::DoEventsAdjacencyConflictDestrutorMove
      /**
      Destroy events that have adjacency conflict
      */
        Spatial::FMTSpatialSchedule _DoEventsAdjacencyConflictDestrutorMove(const Spatial::FMTSpatialSchedule& actual,
            const Spatial::FMTSpatialSchedule::actionbindings& bindings) const;
        // DocString: FMTsamodel::DoGroupsConflictDestrutorMove
         /**
        @brief Destroy events that have group conflict
        @param[in] the actual solution
        @return a new solution
        */
        Spatial::FMTSpatialSchedule _DoGroupsConflictDestrutorMove(const Spatial::FMTSpatialSchedule& p_actual) const;
        // DocString: FMTsamodel::_DoEventsSpread
         /**
        @brief Spread same actions to other cells
        @param[in] the actual solution
        @return a new solution
        */
        Spatial::FMTSpatialSchedule _DoEventsSpread(const Spatial::FMTSpatialSchedule& p_actual) const;

        
        // DocString: FMTsamodel::move
		/**
		Perturb a solution and produce a new one
		*/
		Spatial::FMTSpatialSchedule _Move(const Spatial::FMTSpatialSchedule& actual,
						const Spatial::FMTSpatialSchedule::actionbindings& bindings) const;
		// DocString: FMTsamodel::warmup
		/**
		Using an initprobability close to one, a base solution and a bunch of iterations get a initial temperature.
		*/
		double _Warmup(const Spatial::FMTSpatialSchedule& actual,
			const Spatial::FMTSpatialSchedule::actionbindings& bindings);
        // DocString: FMTsamodel::initialgrow
        /**
        Do an initial grow till you reach the length of the model with the actual solution
        */
        void _InitialGrow();
        // DocString: FMTsamodel::initialbuild
        /**
        Call a random build if there's no solution
        */
        void _RandomBuild();
        // DocString: FMTsamodel::schedulesbuild
        /**
        Call schedules if there's no solution
        */
        void _SchedulesBuild(const std::vector<Core::FMTschedule>&schedules);
        // DocString: FMTsamodel:GetLocalMoveSize
        /**
        Generate the size of the local move.
         */
        size_t _GetLocalMoveSize() const;
        // DocString: FMTsamodel:_GetMaximalMoveSize
        /**
        @brief get maximal move size based on temperature
        @param[in] p_maxSize
        @return max move size
         */
        size_t _GetMaximalMoveSize(size_t p_MaxSize) const;
        // DocString: FMTsamodel:_GetRandomMoveSize
       /**
       @brief get random move size based on temperature
       @param[in] p_maxSize
       @return move size
        */
        size_t _GetRandomMoveSize(size_t p_MaxSize) const;
        // DocString: FMTsamodel::GetRebuild
        /**
        Take the actual non spatial solution of the actual solution and then
        Rebuild the solution using greedyreferencebuild
        */
        Spatial::FMTSpatialSchedule _GetRebuild(const Spatial::FMTSpatialSchedule& actual) const;
        // DocString: FMTsamodel::isCycleProvenOptimal
        /**
        Return true if is optimal based on the termination criteria of the actual temp level
        */
        bool _isCycleProvenOptimal() const;
        // DocString: FMTsamodel::Dofactorization
        /**
        Do the constraint factorization
        */
        void _DoFactorization();
        // DocString: FMTsamodel::LogSolutionStatus
        /**
        Log the status of the best solution
        */
        void _LogSolutionStatus() const;
        // DocString: FMTsamodel::LogTemperatureStatus
        /**
        Log The temperature status and other usefull informations
        */
        void _LogCycleStatus() const;
        // DocString: FMTsamodel::CoolDown
        /**
        Cool down the annealer temp
        */
        void _CoolDown();
        // DocString: FMTsamodel::UpdateFailedMoveCount
        /**
        Update failed move count using NotAcceptedMovesCount and the move stats
        */
        void _UpdateFailedMoveCount();

        std::vector<Core::FMTschedule>_GetSchedules(const Spatial::FMTSpatialSchedule& p_SpatialSchedule, bool withlock) const;

        #ifdef FMTWITHOSI
            Models::FMTlpmodel _GetRandomLpModel(const Spatial::FMTSpatialSchedule& p_SpatialSchedule) const;
        #endif

        void _SetBestSolutionTo(Spatial::FMTSpatialSchedule& p_NewBestSolution,
                                double p_ObjectiveValue);

        void _GetConstraintsStats(const Spatial::FMTSpatialSchedule& p_NewBestSolution,double& p_Objective,
                                 double& p_SpatialRatio, double& p_InventoryRatio, double& p_TotalRatiom,
                                 double& p_PrimalInf) const;


        void _ResetTabouMoves();

        std::vector<FMTsamove> _GetNonTabouMoves() const;

        bool _AllowMove(FMTsamove p_move, const Spatial::FMTSpatialSchedule& p_actual,
            const Spatial::FMTSpatialSchedule::actionbindings& p_bindings) const;
	

    };
}

BOOST_CLASS_EXPORT_KEY(Models::FMTsamodel)

#endif // FMTSAMODEL_H
