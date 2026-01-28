/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTsamodel.h"
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <boost/filesystem.hpp>
#include <cstring>
#include "FMTforest.h"
#include "FMTexceptionhandler.h"
#include "FMTsaschedule.h"
#include "FMTexponentialschedule.h"
#include "FMTareaparser.h"
#include "FMTGCBMtransition.h"
#include "FMTversion.h"
#include "FMTlpmodel.h"
#include "FMTquietlogger.h"

namespace Models

{

    std::string FMTsamodel::GetMovesName(FMTsamove p_move)
    {
        switch (p_move)
            {
            case FMTsamove::Local:
            {
                return "Local";
                break;
            }
            case FMTsamove::ReBuilder:
            {
                return "ReBuilder";
                break;
            }
            case FMTsamove::AreaConflictDestrutor:
            {
                return "AreaConflictDestructor";
                break;
            }
            case FMTsamove::AdjacencyConflictDestrutor:
            {
                return "AdjacencyConflictDestrutor";
                break;
            }
            case FMTsamove::GroupsConflictDestructor:
            {
                return "GroupsConflictDestructor";
                break;
            }
            default:
                break;
            }
        return "";
    }

    void FMTsamodel::_CoolDown()
    {
        try {
            CoolingSchedule->ReduceTemp();
        }catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::_CoolDown", __LINE__, __FILE__);
        }
    }

    void FMTsamodel::_UpdateFailedMoveCount()
    {
        try {
            std::vector<bool>increment(FMTsamove::MoveCount,true);
            for (const FMTmovestats& stat : CycleMoves)
                {
                if (stat.Accepted)
                    {
                    increment[static_cast<int>(stat.MoveType) - 1] = false;
                    }
                }
            size_t moveid = 0;
            for (size_t& count : NotAcceptedMovesCount)
            {
                if (increment.at(moveid))
                    {
                    ++count;
                }
                else {
                    count = 0;
                }
            ++moveid;
            }
        }
        catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::_UpdateFailedMoveCount", __LINE__, __FILE__);
        }
    }


    FMTsamodel::FMTsamodel(const FMTsamodel& rhs):
        FMTsemodel(rhs),
        TotalMoves(rhs.TotalMoves),
        CycleMoves(rhs.CycleMoves),
        LastGlobalObjectiveValue(rhs.LastGlobalObjectiveValue),
        CoolingSchedule(rhs.CoolingSchedule->Clone()),
        NotAcceptedMovesCount()
    {

    }

    FMTsamodel::FMTsamodel(const FMTmodel& rhs):
        FMTsemodel(rhs),
        TotalMoves(),
        CycleMoves(),
        LastGlobalObjectiveValue(),
        CoolingSchedule(std::unique_ptr<Spatial::FMTexponentialschedule>(new Spatial::FMTexponentialschedule())),
        NotAcceptedMovesCount()
    {

    }

    FMTsamodel::FMTsamodel(const FMTmodel& rhs, const Spatial::FMTforest& forest):
        FMTsemodel(rhs,forest),
        TotalMoves(),
        CycleMoves(),
        LastGlobalObjectiveValue(),
        CoolingSchedule(std::unique_ptr<Spatial::FMTexponentialschedule>(new Spatial::FMTexponentialschedule())),
        NotAcceptedMovesCount()
    {

    }

    FMTsamodel::FMTsamodel(const FMTsemodel& rhs):
        FMTsemodel(rhs),
        TotalMoves(),
        CycleMoves(),
        LastGlobalObjectiveValue(),
        CoolingSchedule(std::unique_ptr<Spatial::FMTexponentialschedule>(new Spatial::FMTexponentialschedule())),
        NotAcceptedMovesCount()
    {

    }


    FMTsamodel& FMTsamodel::operator = (const FMTsamodel& rhs)
    {
        if (this!=&rhs)
            {
            FMTsemodel::operator = (rhs);
            TotalMoves = rhs.TotalMoves;
            CycleMoves = rhs.CycleMoves;
            LastGlobalObjectiveValue = rhs.LastGlobalObjectiveValue;
            CoolingSchedule = std::move(CoolingSchedule->Clone());;
            }
        return *this;
    }

    void FMTsamodel::swap_ptr(std::unique_ptr<FMTmodel>& rhs)
    {
        *this = std::move(*dynamic_cast<FMTsamodel*>(rhs.get()));
    }

	std::unique_ptr<FMTmodel>FMTsamodel::clone() const
		{
		return std::unique_ptr<FMTmodel>(new FMTsamodel(*this));
		}


    Graph::FMTgraphstats FMTsamodel::buildperiod()
    {
		return m_BestSolution.randombuild(*this,m_generator);
    }

    bool FMTsamodel::_DoWriteDisturbances() const
    {
        return !getparameter(Models::FMTstrmodelparameters::WORKING_DIRECTORY).empty();
    }

    void FMTsamodel::_WriteDisrturbances() const
    {
        try {
            if (_DoWriteDisturbances())
            {
                Parser::FMTareaparser AreaParser;
                const int LENGTH = getparameter(Models::FMTintmodelparameters::LENGTH);
                const std::string COOLING_LEVEL = "Level" + std::to_string(CoolingSchedule->GetLevel());
                const std::string DIRECTORY = AreaParser.CreateSubDirectory(
                    getparameter(Models::FMTstrmodelparameters::WORKING_DIRECTORY),
                    COOLING_LEVEL);
                for (int period = 1; period <= LENGTH; ++period)
                {
                    AreaParser.writedisturbances(DIRECTORY, getspschedule(),
                        actions, themes, period);
                }
            }
           
        }catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::_WriteDisrturbances", __LINE__, __FILE__);
        }
    }

    void FMTsamodel::_SetFactorByConstraintsLength(std::vector<double>& p_factors) const
    {
        try {
            const int LENGTH = getparameter(Models::FMTintmodelparameters::LENGTH);
            const int UPDATE = getparameter(Models::FMTintmodelparameters::UPDATE);
            for (size_t i = 0 ; i < constraints.size(); ++i)
                {
                const int UPPER = constraints[i].getperiodupperbound();
                if (UPPER < UPDATE)//Update period only...
                    {
                    p_factors.at(i) *= static_cast<double>(UPDATE_PERIOD_FACTOR);
                    }

                }

        }catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::_SetFactorByConstraintsLength"
                                        , __LINE__, __FILE__);
        }
    }


	bool FMTsamodel::_IsBetter(const Spatial::FMTSpatialSchedule& actual,const Spatial::FMTSpatialSchedule& candidat) const
		{
		try {
            const double temp = CoolingSchedule->GetTemp();
			const double actualglobalobjective = GetGlobalObjective(actual);
			const double candidatglobalobjective = GetGlobalObjective(candidat);
			double probability = 1;
            const double objectivediff = (actualglobalobjective - candidatglobalobjective);
            CycleMoves.back().ObjectiveImpact = objectivediff;
			if (candidatglobalobjective>=actualglobalobjective)
				{
				probability =  std::exp(objectivediff / temp);
				}
			std::uniform_real_distribution<double>random_distribution(0.0,1.0);
			const double random_probability = random_distribution(m_generator);
			return  (probability > random_probability);
		}catch (...)
			{
			_exhandler->raisefromcatch("For move type "+std::to_string(CycleMoves.back().MoveType), "FMTsamodel::_IsBetter", __LINE__, __FILE__);
		}
		return false;
		}
   size_t FMTsamodel::_GetLocalMoveSize() const
    {
       size_t sizeofmove = 0;
       try {
           const double MAP_RATIO = (CoolingSchedule->GetTemp() / CoolingSchedule->GetInitialTemp());
          
           const size_t MINIMUM_MOVES = size_t(1);
           const size_t MAXIMUM_MOVES = static_cast<size_t>(
                                                static_cast<double>(m_BestSolution.size() / 20) * MAP_RATIO);
           const size_t MAXIMAL_SIZE = std::max(MINIMUM_MOVES, MAXIMUM_MOVES);//20 % of the map
           std::uniform_int_distribution<int> mosizedist(1, static_cast<int>(MAXIMAL_SIZE));
           sizeofmove = mosizedist(m_generator);
           CycleMoves.back().MoveSize = sizeofmove;
       }
       catch (...)
       {
           _exhandler->raisefromcatch("", "FMTsamodel::_GetLocalMoveSize", __LINE__, __FILE__);
       }
       return sizeofmove;
    }

   Spatial::FMTSpatialSchedule FMTsamodel::_DoConflictDestruction(const Spatial::FMTSpatialSchedule& actual,
       const Spatial::FMTSpatialSchedule::actionbindings& bindings,
       std::vector<std::vector<Spatial::FMTcoordinate>> selectionpool, const int& period) const
   {
       try {
           const size_t movesize = _GetLocalMoveSize();
           if (selectionpool.empty())
           {
               _exhandler->raise(Exception::FMTexc::FMTrangeerror,
                   "Empty solution ", "FMTsamodel::_move()", __LINE__, __FILE__);
           }
           std::shuffle(selectionpool.begin(), selectionpool.end(),m_generator);
           size_t totalsize = 0;
           std::vector<Spatial::FMTcoordinate>finalselection;
           std::vector<std::vector<Spatial::FMTcoordinate>>::const_iterator selected = selectionpool.begin();
           while (totalsize < movesize && selected != selectionpool.end())
           {
               for (const Spatial::FMTcoordinate& coordinate : *selected)
               {
                   finalselection.push_back(coordinate);
                   ++totalsize;
               }
               ++selected;
           }
           std::vector<Spatial::FMTcoordinate>::const_iterator luckycoordinateit = finalselection.begin();
           Spatial::FMTSpatialSchedule newsolution(actual, finalselection.begin(), finalselection.end());
           while (luckycoordinateit != finalselection.end())
           {
               newsolution.perturbgraph(*luckycoordinateit, period, *this, m_generator, bindings);
               ++luckycoordinateit;
           }
           return newsolution;
       }
       catch (...)
       {
           _exhandler->raisefromcatch("", "FMTsamodel::_DoConflictDestruction", __LINE__, __FILE__);
       }
       return actual;
   }

   Spatial::FMTSpatialSchedule FMTsamodel::_DoEventsAdjacencyConflictDestrutorMove(const Spatial::FMTSpatialSchedule& actual,
       const Spatial::FMTSpatialSchedule::actionbindings& bindings) const
   {
       try {
           const std::vector<bool>selectedactions = _GetFromBindings(bindings,true);
           const int period = actual.getperiodwithmaximalevents(selectedactions);
           const std::vector<std::vector<Spatial::FMTcoordinate>> selectionpool = actual.getadjacencyconflictcoordinates(bindings, period);
           return _DoConflictDestruction(actual, bindings, selectionpool, period);
       }catch (...)
            {
           _exhandler->raisefromcatch("", "FMTsamodel::_DoEventsAdjacencyConflictDestrutorMove", __LINE__, __FILE__);
            }
       return actual;
   }

   Spatial::FMTSpatialSchedule  FMTsamodel::_DoGroupsConflictDestrutorMove(const Spatial::FMTSpatialSchedule& p_actual) const
   {
       try {
           std::vector<Core::FMTconstraint>::const_iterator ConstraintIt = constraints.begin();
           std::vector<Spatial::FMTcoordinate>allCoordinates;
           while (ConstraintIt != constraints.end())
           {
               if (ConstraintIt->getconstrainttype() == Core::FMTconstrainttype::FMTSpatialGroup)
               {
                   const  std::vector<Spatial::FMTcoordinate> CONSTRAINT_C = p_actual.GetGroupsConflict(
                       *ConstraintIt, m_SpatialGraphs);
                   allCoordinates.insert(allCoordinates.end(), CONSTRAINT_C.begin(), CONSTRAINT_C.end());
               }
               ++ConstraintIt;
           }
           Spatial::FMTSpatialSchedule newSolution(p_actual);
           newSolution.SetGrow(allCoordinates, *this);
           return newSolution;
       }catch (...)
            {
           _exhandler->raisefromcatch("", "FMTsamodel::_DoGroupsConflictDestrutorMove", __LINE__, __FILE__);
            }
       return p_actual;
   }

   bool FMTsamodel::_AllowAreaDestruction(const Spatial::FMTSpatialSchedule& actual, const Spatial::FMTSpatialSchedule::actionbindings& bindings) const
   {
       try {
           const std::vector<bool>selectedactions = _GetFromBindings(bindings);
           if (!selectedactions.empty())
           {
               return (actual.getperiodwithmaximalevents(selectedactions) > 0);
           }
           
       }
       catch (...)
       {
           _exhandler->raisefromcatch("", "FMTsamodel::FMTsamodel::_AllowAreaDestruction", __LINE__, __FILE__);
       }
       return false;
   }

   bool FMTsamodel::_AllowAdjacencyDestruction(const Spatial::FMTSpatialSchedule& actual, const Spatial::FMTSpatialSchedule::actionbindings& bindings) const
   {
       try {
           const std::vector<bool>selectedactions = _GetFromBindings(bindings,true);
           if (!selectedactions.empty())
           {
               return (actual.getperiodwithmaximalevents(selectedactions) > 0);
           }
       }
       catch (...)
       {
           _exhandler->raisefromcatch("", "FMTsamodel::_AllowAdjacencyDestruction", __LINE__, __FILE__);
       }
       return false;
   }

   bool FMTsamodel::_AllowGroupDestruction(const Spatial::FMTSpatialSchedule& p_actual) const
   {
       bool allowed = false;
       try {
           std::vector<Core::FMTconstraint>::const_iterator ConstraintIt = constraints.begin();
           while (!allowed && ConstraintIt!= constraints.end())
           {
               if (ConstraintIt->getconstrainttype() == Core::FMTconstrainttype::FMTSpatialGroup)
                    {
                   allowed = !p_actual.GetGroupsConflict(
                       *ConstraintIt, m_SpatialGraphs).empty();
                    }
               ++ConstraintIt;
           }
         
       }catch (...)
            {
           _exhandler->raisefromcatch("", "FMTsamodel::_AllowGroupDestruction", __LINE__, __FILE__);
            }
       return  allowed;
   }


   std::vector<bool> FMTsamodel::_GetFromBindings(const Spatial::FMTSpatialSchedule::actionbindings& bindingactions,bool adjacency) const
   {
       std::vector<bool>selectedactions(actions.size(), false);
       try {
           bool gotaction = false;
           for (const auto& binding : bindingactions)
           {
               for (size_t actionid = 0; actionid < actions.size(); ++actionid)
               {
                   if (!adjacency&& binding.at(actionid).isspatialyareabinding())
                   {
                       selectedactions[actionid] = true;
                       gotaction = true;
                   }
                   else if (adjacency && binding.at(actionid).isspatialyadjacencybinding())
                   {
                       selectedactions[actionid] = true;
                       gotaction = true;
                   }

               }
           }
           if (!gotaction)
           {
               selectedactions.clear();
           }
       }catch (...)
       {
           _exhandler->raisefromcatch("", "FMTsamodel::_GetFromBindings", __LINE__, __FILE__);
       }
       return selectedactions;
   }

  

    Spatial::FMTSpatialSchedule FMTsamodel::_DoEventsAreaConflictDestrutorMove(const Spatial::FMTSpatialSchedule& actual,
        const Spatial::FMTSpatialSchedule::actionbindings& bindings) const
    {
        try {
            const std::vector<bool>selectedactions = _GetFromBindings(bindings);
            const int period = actual.getperiodwithmaximalevents(selectedactions);
            const std::vector<std::vector<Spatial::FMTcoordinate>> selectionpool = actual.getareaconflictcoordinates(bindings, period);
            return _DoConflictDestruction(actual, bindings, selectionpool, period);
        }
        catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::_DoEventsAreaConflictDestrutorMove", __LINE__, __FILE__);
        }
        return actual;
    }

    Spatial::FMTSpatialSchedule FMTsamodel::_DoLocalMove(const Spatial::FMTSpatialSchedule& actual,
        const Spatial::FMTSpatialSchedule::actionbindings& bindings,
        const std::vector<Spatial::FMTcoordinate>* movable,
        boost::unordered_map<Core::FMTdevelopment, bool>* operability) const
    {
        try {
            const size_t MOVE_SIZE = _GetLocalMoveSize();
            std::uniform_int_distribution<int> perioddistribution(1, actual.actperiod() - 1);//period to change
            std::vector<Spatial::FMTcoordinate> selectionPool;
            int period = 0;
            while (selectionPool.empty())
            {
                period = perioddistribution(m_generator);
                selectionPool = actual.getmovablecoordinates(*this, period, movable, operability);
            }
            if (selectionPool.empty())
            {
                _exhandler->raise(Exception::FMTexc::FMTrangeerror,
                    "Empty solution ", "FMTsamodel::move()", __LINE__, __FILE__);
            }
            std::shuffle(selectionPool.begin(), selectionPool.end(), m_generator);
            std::vector<Spatial::FMTcoordinate>::const_iterator luckycoordinateit = selectionPool.begin();
            size_t perturbationdone = 0;
            const size_t SELECTED = std::min(MOVE_SIZE, selectionPool.size()-1);
            _logger->logwithlevel("Local Move Selected " + std::to_string(SELECTED)+"\n", 2);
            Spatial::FMTSpatialSchedule newsolution(actual, selectionPool.begin(), selectionPool.begin() + SELECTED);
            //Spatial::FMTspatialschedule newsolution(actual);
            while (luckycoordinateit != selectionPool.end() && perturbationdone < MOVE_SIZE)
            {
                newsolution.perturbgraph(*luckycoordinateit, period, *this, m_generator, bindings);
                ++perturbationdone;
                ++luckycoordinateit;
            }
            return newsolution;
        }
        catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::_DoLocalMove", __LINE__, __FILE__);
        }
        return actual;
    }

	Spatial::FMTSpatialSchedule FMTsamodel::_Move(const Spatial::FMTSpatialSchedule& actual,
										const Spatial::FMTSpatialSchedule::actionbindings& bindings,
										const std::vector<Spatial::FMTcoordinate>*movable,
										boost::unordered_map<Core::FMTdevelopment, bool>*operability) const
		{
		try {
            const FMTsamove MOVE = _GetAMove(actual,bindings);
            CycleMoves.push_back(FMTmovestats());
            CycleMoves.back().MoveType = MOVE;
            switch (MOVE)
            {
            case FMTsamove::Local:
            {
                return _DoLocalMove(actual, bindings, movable, operability);
                break;
            }
            case FMTsamove::ReBuilder:
            {
                return _GetRebuild(actual);
                break;
            }
            case FMTsamove::AreaConflictDestrutor:
            {
                return _DoEventsAreaConflictDestrutorMove(actual, bindings);
                break;
            }
            case FMTsamove::AdjacencyConflictDestrutor:
            {
                return _DoEventsAdjacencyConflictDestrutorMove(actual, bindings);
                break;
            }
            case FMTsamove::GroupsConflictDestructor:
            {
                return _DoGroupsConflictDestrutorMove(actual);
                break;
            }
            default:
                break;
            }
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTsamodel::_move", __LINE__, __FILE__);
			}
		//return newsolution;
		return actual;
		}

    void FMTsamodel::_InitialGrow()
    {
        try {
            int modellength = getparameter(Models::FMTintmodelparameters::LENGTH);
            while (modellength>0)
                {
                m_BestSolution.grow();
                --modellength;
                }
        }catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::_initialgrow", __LINE__, __FILE__);
        }
    }

    void FMTsamodel::_RandomBuild()
    {
        try {
            int modellength = getparameter(Models::FMTintmodelparameters::LENGTH);
            while (modellength > 0)
            {
                m_BestSolution.randombuild(*this, m_generator);
                --modellength;
            }
        }
        catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::_randombuild", __LINE__, __FILE__);
        }
    }

    #ifdef FMTWITHOSI
        Models::FMTlpmodel FMTsamodel::_GetRandomLpModel(const Spatial::FMTSpatialSchedule& p_SpatialSchedule) const
            {
            Models::FMTlpmodel newLp;
            try {
                    std::vector<Core::FMTconstraint> shuffledConstraints;
                    const std::vector<double> FACTORS = p_SpatialSchedule.getConstraintsFactor();
                    size_t constraintId = 0;
                    for (const Core::FMTconstraint& CONSTRAINT : constraints)
                        {
                        if (!CONSTRAINT.isspatial())
                            {
                            shuffledConstraints.push_back(CONSTRAINT);
                            if (CONSTRAINT.isobjective())
                            {
                                const std::string SENSE = CONSTRAINT.sense() < 0 ? "-" : "+";
                                std::vector<std::string>Weights(1, "_ALL");
                                shuffledConstraints.back().setpenalties(SENSE, Weights);
                            }else {
                                shuffledConstraints.back().setgoal("G" + std::to_string(constraintId),
                                    FACTORS.at(constraintId));
                                }
                            }
                        ++constraintId;
                        }
                    std::vector<Core::FMTconstraint>::iterator firstToShuffle = shuffledConstraints.begin();
                    if (!constraints.empty() &&
                        constraints.at(0).isobjective())
                        {
                        ++firstToShuffle;
                        }
                    std::shuffle(firstToShuffle, shuffledConstraints.end(), m_generator);
                    std::vector<Core::FMTactualdevelopment>shuffledArea(area);
                    std::shuffle(shuffledArea.begin(), shuffledArea.end(), m_generator);
                    FMTsolverinterface SolverInterface = FMTsolverinterface::CLP;
                    if (Version::FMTversion::hasfeature("MOSEK"))
                        {
                       SolverInterface = FMTsolverinterface::MOSEK;
                        }
                    newLp = Models::FMTlpmodel(*this, SolverInterface);
                    newLp.setarea(shuffledArea);
                    newLp.setconstraints(shuffledConstraints);
                    newLp.setparameter(Models::FMTboolmodelparameters::QUIET_LOGGING, true);
            }catch (...)
                {
                _exhandler->raisefromcatch("", "FMTsamodel::_GetRandomLpModel", __LINE__, __FILE__);
                }
            return  newLp;
            }
    #endif


   std::vector<double>FMTsamodel::_GetMergeFactors() const
        {
        std::vector<double> MergeFactors;
        try {
            const std::vector<double> FACTORS = m_BestSolution.getConstraintsFactor();
            const int UPDATE = getparameter(Models::FMTintmodelparameters::UPDATE);
            MergeFactors = std::vector<double>(FACTORS.size(),0.0);
            size_t constraintID = 0;
            for (const Core::FMTconstraint& CONSTRAINT : constraints)
                {
                const int UPPER = constraints[constraintID].getperiodupperbound();
                if (CONSTRAINT.isinventory() || //inventory constraints
                    CONSTRAINT.isspatial() || //spatial constraints
                    CONSTRAINT.acrossperiod() ||
                    UPPER < UPDATE) //Update constraints
                    {
                    MergeFactors[constraintID] = FACTORS[constraintID];
                    }
                ++constraintID;
                }
        }catch (...)
            {
            _exhandler->raisefromcatch("", "FMTsamodel::_GetMergeFactors", __LINE__, __FILE__);
            }
        return MergeFactors;
        }

   void  FMTsamodel::_ResetTabouMoves()
        {
       for (auto& MOVE : NotAcceptedMovesCount)
            {
           MOVE = size_t(0);
            }
        }

   void FMTsamodel::_DoGreedyMerge(const Spatial::FMTSpatialSchedule::actionbindings& p_bindings,
                                   const std::vector<Spatial::FMTcoordinate>* p_movable,
                                   boost::unordered_map<Core::FMTdevelopment, bool>* p_operability)
        {
           try {
               double PrimalInfeasibility = 0;
               double Objective = 0;
               const std::vector<double> MERGE_FACTORS = _GetMergeFactors();
               m_BestSolution.setconstraintsfactor(*this, MERGE_FACTORS);
               m_BestSolution.getsolutionstatus(Objective, PrimalInfeasibility,
                   m_SpatialGraphs,true,true);
               if (PrimalInfeasibility > 0)
                   {
                   size_t MoveTillBetterFound = 0;
                   CoolingSchedule->SetTemp(0.0);
                   _ResetTabouMoves();
                   CycleMoves.clear();
                   *_logger << "Merging" << "\n";
                   while (PrimalInfeasibility > FMT_DBL_TOLERANCE &&
                       MoveTillBetterFound < SOLUTION_MERGE_ITERATIONS)
                       {
                       Spatial::FMTSpatialSchedule newsolution = _Move(m_BestSolution, p_bindings, p_movable, p_operability);
                       if (_IsBetter(m_BestSolution, newsolution))
                            {
                            _SetBestSolutionTo(newsolution);
                            m_BestSolution.getsolutionstatus(Objective, PrimalInfeasibility,
                                m_SpatialGraphs, true, true);
                            MoveTillBetterFound = 0;
                       }else {
                           ++MoveTillBetterFound;
                            }
                       ++TotalMoves;
                       _LogSolutionStatus();
                       CycleMoves.clear();
                       }
                   *_logger << "Merge done." << "\n";
                   }
           }catch (...)
                {
               _exhandler->raisefromcatch("", "FMTsamodel::_DoGreedyMerge", __LINE__, __FILE__);
                }
        }

    std::vector<Core::FMTschedule>FMTsamodel::_GetSchedules(const Spatial::FMTSpatialSchedule& p_SpatialSchedule, bool withlock) const
    {
        std::vector<Core::FMTschedule>newSchedules;
        try {
                #ifdef FMTWITHOSI
                     Models::FMTlpmodel newLp = _GetRandomLpModel(p_SpatialSchedule);
                     Logging::FMTquietlogger TEMP_LOGGER;
                     newLp.setparallellogger(TEMP_LOGGER);
                    if (newLp.doplanning(true))
                        {
                        for (int period = 1; period <= getparameter(Models::FMTintmodelparameters::LENGTH);++period)
                            {
                            newSchedules.push_back(newLp.getsolution(period, withlock));
                            }
                    }else {
                        newSchedules = GetSchedules(p_SpatialSchedule, withlock);
                        }
                #else
                    newSchedules = GetSchedules(p_SpatialSchedule, withlock);
                #endif
        }catch (...)
            {
            _exhandler->raisefromcatch("", "FMTsamodel::_GetSchedules", __LINE__, __FILE__);
            }
        return newSchedules;
    }


    Spatial::FMTSpatialSchedule FMTsamodel::_GetRebuild(const Spatial::FMTSpatialSchedule& actual) const
    {
        Spatial::FMTSpatialSchedule newsolution;
        try {
            const std::vector<Core::FMTschedule>nonspatialschedules = _GetSchedules(actual,true);
            if (nonspatialschedules.empty())
            {
                _exhandler->raise(Exception::FMTexc::FMTrangeerror,
                    "Cannot rebuild empty solution", "FMTsamodel::_GetRebuild", __LINE__, __FILE__);
            }
            newsolution = GetNewSolution(actual);
            const std::vector<double>& FACTORS = actual.getConstraintsFactor();
            if (!FACTORS.empty())
            {
                newsolution.setconstraintsfactor(*this, FACTORS);
            }
            for (const Core::FMTschedule& schedule : nonspatialschedules)
                {
                GreedyReferenceBuild(newsolution, schedule,
                    getparameter(NUMBER_OF_ITERATIONS),
                    getparameter(Models::FMTintmodelparameters::SEED), FMT_DBL_TOLERANCE, false);
                }
        }catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::_GetRebuild", __LINE__, __FILE__);
        }
        return newsolution;
    }

    bool  FMTsamodel::_AllowMove(const FMTsamove& move) const
    {
        try {
            return  (NotAcceptedMovesCount.at(static_cast<int>(move) - 1) <= MINIMAL_ACCEPTED_MOVES);
        }catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::_AllowMove", __LINE__, __FILE__);
        }
        return false;
    }

    bool FMTsamodel::_AllowAnyMove() const
    {
        try {
            for (int enumid = 1; enumid < FMTsamove::MoveCount; ++enumid)
                {
                if (_AllowMove(static_cast<FMTsamove>(enumid)))
                    {
                    return true;
                    }
                }
        }
        catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::_AllowAnyMove", __LINE__, __FILE__);
        }
    return false;
    }


    FMTsamodel::FMTsamove FMTsamodel::_GetAMove(const Spatial::FMTSpatialSchedule& actual, const Spatial::FMTSpatialSchedule::actionbindings& bindings) const
    {
        FMTsamove move =  FMTsamove::Local;
        try {
            std::vector<FMTsamove>allmoves;
            allmoves.push_back(FMTsamove::Local);
            if (!actual.emptyevents())
                {
                if (_AllowAreaDestruction(actual,bindings))
                    {
                    allmoves.push_back(FMTsamove::AreaConflictDestrutor);
                    }
                if (_AllowAdjacencyDestruction(actual, bindings))
                    {
                    allmoves.push_back(FMTsamove::AdjacencyConflictDestrutor);
                    }
                if (_AllowGroupDestruction(actual))
                    {
                    allmoves.push_back(FMTsamove::GroupsConflictDestructor);
                    }
                /*if (CycleMoves.size() % 100 == 0)//Each 1000 moves...
                    {
                    allmoves.push_back(FMTsamove::ReBuilder);
                    }*/
                }
            std::vector<FMTsamove>nontaboumoves;
            for (const FMTsamove& move : allmoves)
            {
                if (NotAcceptedMovesCount.at(static_cast<int>(move) - 1)<=3)
                    {
                    nontaboumoves.push_back(move);
                    }
            }
            std::shuffle(nontaboumoves.begin(), nontaboumoves.end(), m_generator);
            move = nontaboumoves.back();
        }catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::_GetAMove", __LINE__, __FILE__);
        }
        return move;
    }


    void FMTsamodel::_SchedulesBuild(const std::vector<Core::FMTschedule>& schedules)
    {
        try {
            const size_t alliterations = static_cast<size_t>(getparameter(Models::FMTintmodelparameters::NUMBER_OF_ITERATIONS));
            for (const Core::FMTschedule& schedule : schedules)
                {
                GreedyReferenceBuild(m_BestSolution, schedule, alliterations);
                }
        }
        catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::_schedulesbuild", __LINE__, __FILE__);
        }

    }



	double FMTsamodel::_Warmup(const Spatial::FMTSpatialSchedule& actual,
		const Spatial::FMTSpatialSchedule::actionbindings& bindings,
		const std::vector<Spatial::FMTcoordinate>*movable,
		boost::unordered_map<Core::FMTdevelopment, bool>*operability,
		double initprobability)
		{
		double temperature = 0;
		try {
        //const double actualobjective = actual.getglobalobjective(*this);
        const std::vector<double>actuals = GetConstraintsValues(actual);
        std::vector<double>maximals = actuals;
        std::vector<double>deltasums(constraints.size(), 0);
        size_t iterations = WARM_UP_ITERATIONS;
        const double totalits = static_cast<double>(WARM_UP_ITERATIONS);
        //double deltasum = 0;
        while (iterations > 0)
        {
            const Spatial::FMTSpatialSchedule newsolution = _Move(actual, bindings, movable, operability);
            size_t cntid = 0;
            for (const double& value : GetConstraintsValues(newsolution))
            {
                //*_logger << "Value " << value << "\n";
                if (value != 0 && (maximals.at(cntid) > 0 && maximals.at(cntid) < value ||
                    maximals.at(cntid) < 0 && maximals.at(cntid) > value))
                {
                    maximals[cntid] = value;
                }
                deltasums[cntid] += (std::abs(actuals.at(cntid) - value));
                ++cntid;
            }
            //const double newobjective = newsolution.getglobalobjective(*this);
            //*_logger << "old " << actualobjective << " new " << newobjective << "\n";
            //deltasum += std::abs(actualobjective - newobjective);
            --iterations;
        }
        double deltasum = 0;
        size_t cntid = 0;
        for (double& value : maximals)//Need to normalize the calculated delta
        {
            if (value == 0)
            {
                value = 1;
            }
             value = (1000 / (std::abs(value)));
            deltasum += (deltasums[cntid] * value);
            ++cntid;
        }
        temperature = -(deltasum / totalits) / std::log(initprobability);
        _SetFactorByConstraintsLength(maximals);
        maximals[0] = 1.0;
        m_BestSolution.setconstraintsfactor(*this, maximals);
                }
        catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::_warmup", __LINE__, __FILE__);
        }
        return temperature;
        }

        bool FMTsamodel::isProvenOptimal() const
        {
            try {
               
                return ((TotalMoves >= getparameter(FMTintmodelparameters::MAX_MOVES)) || 
                             (CoolingSchedule->GetTemp() < FMT_DBL_TOLERANCE && TotalMoves > 0 && !_AllowAnyMove()));
            }
            catch (...)
            {
                _exhandler->raisefromcatch("", "FMTsamodel::_isProvenOptimal", __LINE__, __FILE__);
            }
            return false;
        }

        size_t FMTsamodel::_GetCycleMoves() const
        {
            return CycleMoves.size();
        }

        size_t FMTsamodel::_GetAcceptedCycleMoves() const
        {
            size_t count = 0;
            for (const FMTsamodel::FMTmovestats& stats : CycleMoves)
            {
                if (stats.Accepted)
                    {
                    ++count;
                    }
            }
            return count;
        }

        bool FMTsamodel::_isCycleProvenOptimal() const
        {
            try {
                return (!((_GetCycleMoves() - _GetAcceptedCycleMoves()) < getparameter(FMTintmodelparameters::MAX_ACCEPTED_CYCLE_MOVES) && 
                    _GetCycleMoves() < getparameter(FMTintmodelparameters::MAX_CYCLE_MOVES) &&
                    TotalMoves < getparameter(FMTintmodelparameters::MAX_MOVES)));
            }
            catch (...)
            {
                _exhandler->raisefromcatch("", "FMTsamodel::_isCycleProvenOptimal", __LINE__, __FILE__);
            }
            return false;
        }

        void FMTsamodel::_DoFactorization()
        {
            try {
                if (TotalMoves % 100 == 0)
                {
                    DoReFactortorization(m_BestSolution);
                }
            }
            catch (...)
            {
                _exhandler->raisefromcatch("", "FMTsamodel::_Dofactorization", __LINE__, __FILE__);
            }
        }

        void FMTsamodel::_LogSolutionStatus() const
        {
            try {
                if (TotalMoves % 100 == 0)
                {
                    double objective = 0;
                    double primalinf = 0;
                    GetSolutionStatus(m_BestSolution, objective, primalinf);
                    m_BestSolution.logsolutionstatus(TotalMoves, objective, primalinf);
                    
                
            }
        }
        catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::_LogSolutionStatus", __LINE__, __FILE__);
        }
    }

    void FMTsamodel::LogMovesReport() const
        {
        try {
            *_logger << "Moves report" << "\n";
                std::map<FMTsamove, size_t>Numberofaccepted;
                std::map<FMTsamove, size_t>Numberofdone;
                std::map<FMTsamove, double>TotalGain;
                std::map<double, size_t>acceptedpersize;
                for (const FMTsamodel::FMTmovestats& stats : CycleMoves)
                {
                    if (stats.Accepted)
                    {
                        ++Numberofaccepted[stats.MoveType];
                    }
                    else if (Numberofaccepted.find(stats.MoveType) == Numberofaccepted.end())
                    {
                        Numberofaccepted[stats.MoveType] = 0;
                    }
                    ++Numberofdone[stats.MoveType];
                    TotalGain[stats.MoveType] += stats.ObjectiveImpact;
                    acceptedpersize[stats.ObjectiveImpact] += stats.MoveSize;
                }
                for (const auto& element : Numberofaccepted)
                {
                    const std::string moveid = "   Move: " + GetMovesName(element.first);
                    const std::string acceptedratio = "    Accepted (" + std::to_string((static_cast<double>(element.second) / static_cast<double>(Numberofdone.at(element.first))) * 100) + "%)";
                    const std::string averagegainperMOVE = "    Average Energy(" + std::to_string((TotalGain.at(element.first) / static_cast<double>(Numberofdone.at(element.first)))) + ")";
                    *_logger << moveid + "\n" + acceptedratio + "\n" + averagegainperMOVE << "\n";
                }
           *_logger << "Moves report done" << "\n";
        }catch (...)
            {
            _exhandler->raisefromcatch("", "FMTsamodel::_LogMovesReport", __LINE__, __FILE__);
            }
        }

    void FMTsamodel::_LogCycleStatus() const
    {
        try {
             const double acceptanceratio = (static_cast<double>(_GetAcceptedCycleMoves()) / static_cast<double>(_GetCycleMoves())) * 100;
             const double Temperature = CoolingSchedule->GetTemp();
             if (_logger->logwithlevel("Temp(" + std::to_string(Temperature) + ") Accepted(" + std::to_string(acceptanceratio) + "%) Level(" + std::to_string(CoolingSchedule->GetLevel()) + ")\n", 1))
                {
                LogMovesReport();
                }
             
        }catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::_LogCycleStatus", __LINE__, __FILE__);
        }
    }


	bool FMTsamodel::initialsolve()
		{
		try {
			const std::vector<Spatial::FMTcoordinate>movables = m_BestSolution.getstaticsmovablecoordinates(*this);
			const Spatial::FMTSpatialSchedule::actionbindings actionsbinding = m_BestSolution.getbindingactionsbyperiod(*this);
			boost::unordered_map<Core::FMTdevelopment, bool>operability;
            *_logger << "Generator initial state: " + std::to_string(m_generator()) << "\n";
			const double initialtemperature = _Warmup(m_BestSolution, actionsbinding,
                &movables,&operability,0.9);
            CoolingSchedule->SetInitialTemperature(initialtemperature);
            //LogCycleStatus();
			while (!isProvenOptimal())
				{
                //LevelMoves = 0;
                //LevelAcceptedMoves = 0;
                CycleMoves.clear();
                LastGlobalObjectiveValue = 0;
				while (!_isCycleProvenOptimal())
					{
					Spatial::FMTSpatialSchedule newsolution = _Move(m_BestSolution, actionsbinding,&movables,&operability);
                    if (_IsBetter(m_BestSolution, newsolution))
                        {
                        _SetBestSolutionTo(newsolution);
                         LastGlobalObjectiveValue = std::max(LastGlobalObjectiveValue, 
                                GetGlobalObjective(m_BestSolution));//Get the Worst one
                            CycleMoves.back().Accepted = true;
                        }
                        _DoFactorization();
                        _LogSolutionStatus();
					++TotalMoves;
					}
                _UpdateFailedMoveCount();
                _LogCycleStatus();
                _WriteDisrturbances();
				_CoolDown();
				}
            _DoGreedyMerge(actionsbinding, 
                &movables, &operability);
            LogConstraintsFactors();
            LogConstraintsInfeasibilities();
            *_logger << "Generator final state: " << m_generator() << "\n";
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTsamodel::initialsolve", __LINE__, __FILE__);
			}
        return true;
		}

    void FMTsamodel::_SetBestSolutionTo(Spatial::FMTSpatialSchedule& p_NewBestSolution)
    {
        if (p_NewBestSolution.ispartial())
        {
            m_BestSolution.copyfrompartial(p_NewBestSolution);
        }
        else {
            m_BestSolution.swap(p_NewBestSolution);
        }
    }

    bool FMTsamodel::build(std::vector<Core::FMTschedule> schedules)
    {
        try {
            if (schedules.empty()||
                (!schedules.empty() && schedules.begin()->empty())) //From no solution
            {
                    _RandomBuild();
            }else {
                _SchedulesBuild(schedules);
                }
        }catch (...)
        {
            _exhandler->printexceptions("", "FMTsamodel::build", __LINE__, __FILE__);
        }
        return true;
    }

 
    bool FMTsamodel::solve()
    {
        try {
            return initialsolve();
        }catch (...)
        {
            _exhandler->printexceptions("", "FMTsamodel::solve", __LINE__, __FILE__);
        }
        return false;
    }

    std::unique_ptr<FMTmodel>FMTsamodel::presolve(std::vector<Core::FMTactualdevelopment> optionaldevelopments) const
    {
        try {
            std::unique_ptr<FMTmodel>BASE_PRESOLVE = FMTsemodel::presolve(optionaldevelopments);
            std::unique_ptr<FMTmodel>PRESOLVED = std::unique_ptr<FMTmodel>(
                                new FMTsamodel(
                                    *(dynamic_cast<FMTsemodel*>(BASE_PRESOLVE.get()))));
            FMTsamodel* SA = dynamic_cast<FMTsamodel*>(PRESOLVED.get());
            return PRESOLVED;
        }
        catch (...)
        {
            _exhandler->printexceptions("", "FMTsamodel::presolve", __LINE__, __FILE__);
        }
        return std::unique_ptr<FMTmodel>(nullptr);
    }

    std::unique_ptr<FMTmodel>FMTsamodel::getcopy(int period) const
    {
        try {
            return std::unique_ptr<FMTmodel>(new FMTsamodel(*dynamic_cast<FMTsemodel*>(FMTsemodel::getcopy(period).get())));
        }
        catch (...)
        {
            _exhandler->printexceptions("", "FMTsamodel::getcopy", __LINE__, __FILE__);
        }
        return std::unique_ptr<FMTmodel>(nullptr);
    }

    FMTsamodel::~FMTsamodel() = default;



}

BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTsamodel)