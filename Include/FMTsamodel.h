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
        ReBuilder = 2,
        AreaConflictDestrutor = 3,
        AdjacencyConflictDestrutor = 4,
        MoveCount = 5
    };
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
    size_t TotalMoves;
    // DocString: FMTsamodel::CycleMoves
    ///The move done during the last cycle
    mutable std::vector<FMTmovestats>CycleMoves;
    // DocString: FMTsamodel::LastGlobalObjectiveValue
    ///The value of the last globalobjective of the last level
    double LastGlobalObjectiveValue;
    // DocString: FMTsamodel::CoolingSchedule
    ///Cooling schedule for simulated annealing algorithm.
    std::unique_ptr<Spatial::FMTsaschedule> CoolingSchedule;
    // DocString: FMTsamodel::NotAcceptedMovesCount
    ///Count the number of cycle the moves gave had no acceptance
    std::array<size_t, FMTsamove::MoveCount>NotAcceptedMovesCount;
    // DocString: FMTsamodel::m_WorkingDirectory
    ///The working directory to write disturbances
    std::string m_WorkingDirectory;
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
    protected:
        // DocString: FMTsamodel::GetFromBindings
        /**
        Get the selected action from the bindings
        */
        std::vector<bool> GetFromBindings(const Spatial::FMTSpatialSchedule::actionbindings& bindingactions, bool adjacency = false) const;
        // DocString: FMTsamodel::GetCycleMoves
        /**
        Get the total number of moves of the last cycle
        */
        size_t GetCycleMoves() const;
        // DocString: FMTsamodel::GetAcceptedCycleMoves
        /**
        Get the number of accepted move of the last cycle
        */
        size_t GetAcceptedCycleMoves() const;
        // DocString: FMTsamodel::AllowDestruction
        /**
        Returns true if the bindings allow to destroy some events
       */
        bool AllowAreaDestruction(const Spatial::FMTSpatialSchedule& actual, const Spatial::FMTSpatialSchedule::actionbindings& bindings) const;
        // DocString: FMTsamodel::AllowAdjacencyDestruction
           /**
           Returns true if the bindings allow to destroy some events
          */
        bool AllowAdjacencyDestruction(const Spatial::FMTSpatialSchedule& actual, const Spatial::FMTSpatialSchedule::actionbindings& bindings) const;
        // DocString: FMTsamodel::AllowMove
        /**
        Check If you can allow the move 
       */
        bool AllowMove(const FMTsamove& move) const;
        // DocString: FMTsamodel::AllowAnyMove
        /**
        Return true if you can do a move
       */
        bool AllowAnyMove() const;
        // DocString: FMTsamodel::GetAMove
        /**
        Will return coordinates that might be good candidat to disturb
        */
        FMTsamove GetAMove(const Spatial::FMTSpatialSchedule& actual, const Spatial::FMTSpatialSchedule::actionbindings& bindings) const;
		// DocString: FMTsamodel::evaluate
		/**
		Evaluate the actual and a candidat solution and return true if the candidat solution is choose to replace
		the actual solution.Based on a temp.
		*/
		bool IsBetter(const Spatial::FMTSpatialSchedule& actual, const Spatial::FMTSpatialSchedule& candidat) const;
        // DocString: FMTsamodel::DoLocalMove
        /**
        Do a loval move and disturb a random number of graph at a random period
        */
        Spatial::FMTSpatialSchedule DoLocalMove(const Spatial::FMTSpatialSchedule& actual,
            const Spatial::FMTSpatialSchedule::actionbindings& bindings,
            const std::vector<Spatial::FMTcoordinate>* movable,
            boost::unordered_map<Core::FMTdevelopment, bool>* operability) const;
        // DocString: FMTsamodel::DoConflictDestruction
        /**
        Destroy the conflicts for a given periods and coordinates
         */
        Spatial::FMTSpatialSchedule DoConflictDestruction(const Spatial::FMTSpatialSchedule& actual,
            const Spatial::FMTSpatialSchedule::actionbindings& bindings,
           std::vector<std::vector<Spatial::FMTcoordinate>> selectionpool, const int& period) const;
       // DocString: FMTsamodel::DoEventsAreaConflictDestrutorMove
       /**
       Destroy events that have some area conflict
       */
        Spatial::FMTSpatialSchedule DoEventsAreaConflictDestrutorMove(const Spatial::FMTSpatialSchedule& actual,
            const Spatial::FMTSpatialSchedule::actionbindings& bindings) const;
        // DocString: FMTsamodel::DoEventsAdjacencyConflictDestrutorMove
      /**
      Destroy events that have adjacency conflict
      */
        Spatial::FMTSpatialSchedule DoEventsAdjacencyConflictDestrutorMove(const Spatial::FMTSpatialSchedule& actual,
            const Spatial::FMTSpatialSchedule::actionbindings& bindings) const;
        // DocString: FMTsamodel::move
		/**
		Perturb a solution and produce a new one
		*/
		Spatial::FMTSpatialSchedule Move(const Spatial::FMTSpatialSchedule& actual,
						const Spatial::FMTSpatialSchedule::actionbindings& bindings,
						const std::vector<Spatial::FMTcoordinate>*movable = nullptr,
						boost::unordered_map<Core::FMTdevelopment, bool>*operability= nullptr) const;
		// DocString: FMTsamodel::warmup
		/**
		Using an initprobability close to one, a base solution and a bunch of iterations get a initial temperature.
		*/
		double Warmup(const Spatial::FMTSpatialSchedule& actual,
			const Spatial::FMTSpatialSchedule::actionbindings& bindings,
			const std::vector<Spatial::FMTcoordinate>*movable = nullptr,
			boost::unordered_map<Core::FMTdevelopment, bool>*operability = nullptr,
			double initprobability = 0.5,size_t iterations=10);
        // DocString: FMTsamodel::initialgrow
        /**
        Do an initial grow till you reach the length of the model with the actual solution
        */
        void InitialGrow();
        // DocString: FMTsamodel::initialbuild
        /**
        Call a random build if there's no solution
        */
        void RandomBuild();
        // DocString: FMTsamodel::schedulesbuild
        /**
        Call schedules if there's no solution
        */
        void SchedulesBuild(const std::vector<Core::FMTschedule>&schedules);
        // DocString: FMTsamodel::swap_ptr
        /**
        Swap with an abstract FMTmodel
        */
        virtual void swap_ptr(std::unique_ptr<FMTmodel>& rhs);
        // DocString: FMTsamodel:GetLocalMoveSize
        /**
        Generate the size of the local move.
         */
        size_t GetLocalMoveSize() const;
        // DocString: FMTsamodel::GetRebuild
        /**
        Take the actual non spatial solution of the actual solution and then
        Rebuild the solution using greedyreferencebuild
        */
        Spatial::FMTSpatialSchedule GetRebuild(const Spatial::FMTSpatialSchedule& actual) const;
        // DocString: FMTsamodel::isCycleProvenOptimal
        /**
        Return true if is optimal based on the termination criteria of the actual temp level
        */
        bool isCycleProvenOptimal() const;
        // DocString: FMTsamodel::Dofactorization
        /**
        Do the constraint factorization
        */
        void DoFactorization();
        // DocString: FMTsamodel::LogSolutionStatus
        /**
        Log the status of the best solution
        */
        void LogSolutionStatus() const;
        // DocString: FMTsamodel::LogTemperatureStatus
        /**
        Log The temperature status and other usefull informations
        */
        void LogCycleStatus() const;
        // DocString: FMTsamodel::CoolDown
        /**
        Cool down the annealer temp
        */
        void CoolDown();
        // DocString: FMTsamodel::UpdateFailedMoveCount
        /**
        Update failed move count using NotAcceptedMovesCount and the move stats
        */
        void UpdateFailedMoveCount();
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
        ///Constructor
        FMTsamodel();
        ///Destructor
        ~FMTsamodel();
        ///Copy constructor
        FMTsamodel(const FMTsamodel& rhs);
        ///Copy constructor to use parent as argument in constructor
        FMTsamodel(const FMTmodel& rhs);
        ///Copy constructor to use parent as argument in constructor
        FMTsamodel(const FMTmodel& rhs,const Spatial::FMTforest& forest);
        ///Copy assignment operator
        FMTsamodel& operator = (const FMTsamodel& rhs);
		// DocString: FMTsamodel::clone
		/**
		Get a clone of the FMTsamodel
		*/
		virtual std::unique_ptr<FMTmodel>clone() const final;
        
        Graph::FMTgraphstats buildperiod();

        void SetWorkingDirectory(const std::string& p_ValidDirectory);


    };
}

BOOST_CLASS_EXPORT_KEY(Models::FMTsamodel)

#endif // FMTSAMODEL_H
