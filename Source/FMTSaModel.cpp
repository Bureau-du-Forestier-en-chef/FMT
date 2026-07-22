/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTSaModel.h"
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <boost/filesystem.hpp>
#include <cstring>
#include "FMTForest.h"
#include "FMTExceptionHandler.h"
#include "FMTSaSchedule.h"
#include "FMTExponentialSchedule.h"
#include "FMTAreaParser.h"
#include "FMTGCBMTransition.h"
#include "FMTVersion.h"
#include "FMTLpModel.h"
#include "FMTQuietLogger.h"

namespace Models

{

    std::string FMTSaModel::getMovesName(FMTsamove p_move)
    {
        switch (p_move)
            {
            case FMTsamove::Local:
            {
                return "Local";
                break;
            }
            /* case FMTsamove::ReBuilder:
            {
                return "ReBuilder";
                break;
            }*/
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
            case FMTsamove::EventsSpread:
            {
                return "EventsSpread";
                break;
            }
            default:
                break;
            }
        return "";
    }

    void FMTSaModel::_coolDown()
    {
        try {
            m_CoolingSchedule->reduceTemp();
        }catch (...)
        {
            _exhandler->raiseFromCatch("", "FMTSaModel::_coolDown", __LINE__, __FILE__);
        }
    }

    void FMTSaModel::_UpdateFailedMoveCount()
    {
        try {
            std::vector<bool>increment(FMTsamove::MoveCount,true);
            for (const FMTMoveStats& stat : m_CycleMoves)
                {
                if (stat.Accepted)
                    {
                    increment[static_cast<int>(stat.MoveType) - 1] = false;
                    }
                }
            size_t moveid = 0;
            for (size_t& count : m_NotAcceptedMovesCount)
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
            _exhandler->raiseFromCatch("", "FMTSaModel::_UpdateFailedMoveCount", __LINE__, __FILE__);
        }
    }


    FMTSaModel::FMTSaModel(const FMTSaModel& rhs):
        FMTSeModel(rhs),
        m_TotalMoves(rhs.m_TotalMoves),
        m_CycleMoves(rhs.m_CycleMoves),
        m_CoolingSchedule(rhs.m_CoolingSchedule->Clone()),
        m_NotAcceptedMovesCount(),
        m_BestObjective()
    {

    }

    FMTSaModel::FMTSaModel(const FMTModel& rhs):
        FMTSeModel(rhs),
        m_TotalMoves(),
        m_CycleMoves(),
        m_CoolingSchedule(std::unique_ptr<Spatial::FMTExponentialSchedule>(new Spatial::FMTExponentialSchedule())),
        m_NotAcceptedMovesCount(),
        m_BestObjective()
    {

    }

    FMTSaModel::FMTSaModel(const FMTModel& rhs, const Spatial::FMTForest& forest):
        FMTSeModel(rhs,forest),
        m_TotalMoves(),
        m_CycleMoves(),
        m_CoolingSchedule(std::unique_ptr<Spatial::FMTExponentialSchedule>(new Spatial::FMTExponentialSchedule())),
        m_NotAcceptedMovesCount(),
        m_BestObjective()
    {

    }

    FMTSaModel::FMTSaModel(const FMTSeModel& rhs):
        FMTSeModel(rhs),
        m_TotalMoves(),
        m_CycleMoves(),
        m_CoolingSchedule(std::unique_ptr<Spatial::FMTExponentialSchedule>(new Spatial::FMTExponentialSchedule())),
        m_NotAcceptedMovesCount(),
        m_BestObjective()
    {

    }


    FMTSaModel& FMTSaModel::operator = (const FMTSaModel& rhs)
    {
        if (this!=&rhs)
            {
            FMTSeModel::operator = (rhs);
            m_TotalMoves = rhs.m_TotalMoves;
            m_CycleMoves = rhs.m_CycleMoves;
            m_CoolingSchedule = std::move(m_CoolingSchedule->Clone());
            m_BestObjective = rhs.m_BestObjective;
            }
        return *this;
    }

    void FMTSaModel::swapPtr(std::unique_ptr<FMTModel>& rhs)
    {
        *this = std::move(*dynamic_cast<FMTSaModel*>(rhs.get()));
    }

	std::unique_ptr<FMTModel>FMTSaModel::clone() const
		{
		return std::unique_ptr<FMTModel>(new FMTSaModel(*this));
		}


    Graph::FMTGraphStats FMTSaModel::buildPeriod()
    {
		return m_BestSolution.randomBuild(*this,m_generator);
    }

    bool FMTSaModel::_doWriteDisturbances() const
    {
        return !getParameter(Models::FMTstrmodelparameters::WORKING_DIRECTORY).empty();
    }

    void FMTSaModel::_writeDisrturbances() const
    {
        try {
            if (_doWriteDisturbances())
            {
                Parser::FMTAreaParser AreaParser;
                const int LENGTH = getParameter(Models::FMTintmodelparameters::LENGTH);
                const std::string COOLING_LEVEL = "Level" + std::to_string(m_CoolingSchedule->getLevel());
                const std::string DIRECTORY = AreaParser.createSubDirectory(
                    getParameter(Models::FMTstrmodelparameters::WORKING_DIRECTORY),
                    COOLING_LEVEL);
                for (int period = 1; period <= LENGTH; ++period)
                {
                    AreaParser.writeDisturbances(DIRECTORY, getSpSchedule(),
                        actions, themes, period);
                }
            }
           
        }catch (...)
        {
            _exhandler->raiseFromCatch("", "FMTSaModel::_writeDisrturbances", __LINE__, __FILE__);
        }
    }


	bool FMTSaModel::_isBetter(double p_candidatObjective) const
		{
		try {
            const double temp = m_CoolingSchedule->getTemp();
			double probability = 1;
            const double objectivediff = (m_BestObjective - p_candidatObjective);
            m_CycleMoves.back().ObjectiveImpact = objectivediff;
			if (p_candidatObjective >= m_BestObjective)
				{
				probability =  std::exp(objectivediff / temp);
				}
			std::uniform_real_distribution<double>random_distribution(0.0,1.0);
			const double random_probability = random_distribution(m_generator);
			return  (probability > random_probability);
		}catch (...)
			{
			_exhandler->raiseFromCatch("For move type "+std::to_string(m_CycleMoves.back().MoveType),
                "FMTSaModel::_isBetter", __LINE__, __FILE__);
		}
		return false;
		}

    size_t FMTSaModel::_getMaximalMoveSize(size_t p_MaxSize) const
    {
        size_t sizeOfMove = 0;
        try {
            const double MAP_RATIO = (m_CoolingSchedule->getTemp() / 
                                            m_CoolingSchedule->getInitialTemp());
            const size_t MINIMUM_MOVES = size_t(1);
            const size_t MAXIMUM_MOVES = static_cast<size_t>(
                static_cast<double>(p_MaxSize) * MAP_RATIO);
            sizeOfMove = std::max(MINIMUM_MOVES,MAXIMUM_MOVES / m_MOVE_SIZE_FACTOR);
        }catch (...)
            {
            _exhandler->raiseFromCatch("", 
                "FMTSaModel::_getMaximalMoveSize", __LINE__, __FILE__);
            }
        return sizeOfMove;
    }

    size_t FMTSaModel::_getRandomMoveSize(size_t p_MaxSize) const
    {
        size_t sizeOfMove = 0;
        try {
            const size_t MAXIMAL_SIZE = _getMaximalMoveSize(p_MaxSize);
            std::uniform_int_distribution<size_t> moveSizeDistribution(size_t(1), MAXIMAL_SIZE);
            sizeOfMove = moveSizeDistribution(m_generator);
        }catch (...)
            {
            _exhandler->raiseFromCatch("",
                "FMTSaModel::_getRandomMoveSize", __LINE__, __FILE__);
            }
        return sizeOfMove;
    }

   size_t FMTSaModel::_getLocalMoveSize() const
    {
       size_t sizeOfMove = 0;
       try {
           sizeOfMove = _getRandomMoveSize(m_BestSolution.size());
           if (sizeOfMove >= m_BestSolution.size())
                {
                _exhandler->raise(Exception::FMTexc::FMTrangeerror,
                   "Move Too Large ", "FMTSaModel::_getLocalMoveSize", __LINE__, __FILE__);
                }
           m_CycleMoves.back().MoveSize = sizeOfMove;
       } catch (...)
            {
            _exhandler->raiseFromCatch("", "FMTSaModel::_getLocalMoveSize", __LINE__, __FILE__);
            }
       return sizeOfMove;
    }

   Spatial::FMTSpatialSchedule FMTSaModel::_doConflictDestruction(const Spatial::FMTSpatialSchedule& actual,
       const Spatial::FMTSpatialSchedule::actionbindings& bindings,
       std::vector<std::vector<Spatial::FMTCoordinate>> selectionpool, const int& period) const
   {
       try {
           const size_t MOVE_SIZE = _getLocalMoveSize();
           if (selectionpool.empty())
           {
               _exhandler->raise(Exception::FMTexc::FMTrangeerror,
                   "Empty solution ", "FMTSaModel::_move()", __LINE__, __FILE__);
           }
           std::shuffle(selectionpool.begin(), selectionpool.end(),m_generator);
           size_t totalsize = 0;
           std::vector<Spatial::FMTCoordinate>finalSelection;
           std::vector<std::vector<Spatial::FMTCoordinate>>::const_iterator selected = selectionpool.begin();
           while (totalsize < MOVE_SIZE && 
               selected != selectionpool.end())
           {
               for (const Spatial::FMTCoordinate& coordinate : *selected)
               {
                   finalSelection.push_back(coordinate);
                   ++totalsize;
               }
               ++selected;
           }
           std::vector<Spatial::FMTCoordinate>::const_iterator luckycoordinateit = finalSelection.begin();
           Spatial::FMTSpatialSchedule newsolution(actual, finalSelection.begin(), finalSelection.end());
           newsolution.enableSolutionTracker(m_SpatialGraphs);
           while (luckycoordinateit != finalSelection.end())
           {
               newsolution.perturbGraph(*luckycoordinateit, period, *this, m_generator, bindings);
               ++luckycoordinateit;
           }
           return newsolution;
       }
       catch (...)
       {
           _exhandler->raiseFromCatch("", "FMTSaModel::_doConflictDestruction", __LINE__, __FILE__);
       }
       return actual;
   }

   Spatial::FMTSpatialSchedule FMTSaModel::_doEventsAdjacencyConflictDestrutorMove(const Spatial::FMTSpatialSchedule& actual,
       const Spatial::FMTSpatialSchedule::actionbindings& bindings) const
   {
       try {
           const std::vector<bool>selectedActions = _getFromBindings(bindings,true);
           const int period = actual.getPeriodWithMaximalEvents(selectedActions);
           const std::vector<std::vector<Spatial::FMTCoordinate>> selectionpool = actual.getAdjacencyConflictCoordinates(bindings, period);
           return _doConflictDestruction(actual, bindings, selectionpool, period);
       }catch (...)
            {
           _exhandler->raiseFromCatch("", "FMTSaModel::_doEventsAdjacencyConflictDestrutorMove", __LINE__, __FILE__);
            }
       return actual;
   }

   Spatial::FMTSpatialSchedule  FMTSaModel::_doGroupsConflictDestrutorMove(const Spatial::FMTSpatialSchedule& p_actual) const
   {
       try {
           std::vector<Core::FMTConstraint>::const_iterator ConstraintIt = constraints.begin();
           std::vector<Spatial::FMTCoordinate>allCoordinates;
           while (ConstraintIt != constraints.end())
           {
               if (ConstraintIt->getConstraintType() == Core::FMTconstrainttype::FMTSpatialGroup)
               {
                   const  std::vector<Spatial::FMTCoordinate> CONSTRAINT_C = p_actual.getGroupsConflict(
                       *ConstraintIt, m_SpatialGraphs);
                   allCoordinates.insert(allCoordinates.end(), CONSTRAINT_C.begin(), CONSTRAINT_C.end());
               }
               ++ConstraintIt;
           }
           Spatial::FMTSpatialSchedule newSolution(p_actual);
           newSolution.enableSolutionTracker(m_SpatialGraphs);
           const size_t MOVE_SIZE = _getRandomMoveSize(allCoordinates.size()-1);
           allCoordinates.erase(allCoordinates.begin() + MOVE_SIZE, allCoordinates.end());
           std::shuffle(allCoordinates.begin(), allCoordinates.end(), m_generator);
           newSolution.setGrow(allCoordinates, *this);
           return newSolution;
       }catch (...)
            {
           _exhandler->raiseFromCatch("", "FMTSaModel::_doGroupsConflictDestrutorMove", __LINE__, __FILE__);
            }
       return p_actual;
   }

   bool FMTSaModel::_allowAreaDestruction(const Spatial::FMTSpatialSchedule& actual, const Spatial::FMTSpatialSchedule::actionbindings& bindings) const
   {
       try {
           if (!actual.emptyEvents())
               {
                   const std::vector<bool>selectedActions = _getFromBindings(bindings);
                   if (!selectedActions.empty())
                   {
                       return (actual.getPeriodWithMaximalEvents(selectedActions) > 0);
                   }
               }
           
       }
       catch (...)
       {
           _exhandler->raiseFromCatch("", "FMTSaModel::FMTSaModel::_AllowAreaDestruction", __LINE__, __FILE__);
       }
       return false;
   }

   bool FMTSaModel::_allowAdjacencyDestruction(const Spatial::FMTSpatialSchedule& actual, const Spatial::FMTSpatialSchedule::actionbindings& bindings) const
   {
       try {
           if (!actual.emptyEvents())
           {
               const std::vector<bool>selectedActions = _getFromBindings(bindings, true);
               if (!selectedActions.empty())
               {
                   return (actual.getPeriodWithMaximalEvents(selectedActions) > 0);
               }
           }
       }
       catch (...)
       {
           _exhandler->raiseFromCatch("", "FMTSaModel::_allowAdjacencyDestruction", __LINE__, __FILE__);
       }
       return false;
   }

   bool FMTSaModel::_allowGroupDestruction(const Spatial::FMTSpatialSchedule& p_actual) const
   {
       bool allowed = false;
       try {
           if (!p_actual.emptyEvents())
           {
               std::vector<Core::FMTConstraint>::const_iterator ConstraintIt = constraints.begin();
               while (!allowed && ConstraintIt != constraints.end())
               {
                   if (ConstraintIt->getConstraintType() == Core::FMTconstrainttype::FMTSpatialGroup)
                   {
                       allowed = !p_actual.getGroupsConflict(
                           *ConstraintIt, m_SpatialGraphs).empty();
                   }
                   ++ConstraintIt;
               }
           }
         
       }catch (...)
            {
           _exhandler->raiseFromCatch("", "FMTSaModel::_allowGroupDestruction", __LINE__, __FILE__);
            }
       return  allowed;
   }

   bool FMTSaModel::_allowEventsSpread(const Spatial::FMTSpatialSchedule& p_actual) const
   {
       bool allowed = false;
       try {
           if (!p_actual.emptyEvents())
           {
               int modelLength = getParameter(Models::FMTintmodelparameters::LENGTH);
               while (!allowed && modelLength > 0)
               {
                   allowed = p_actual.canDoEventSpread(modelLength);
                   --modelLength;
               }
           }
       }catch (...)
            {
           _exhandler->raiseFromCatch("", 
               "FMTSaModel::_allowEventsSpread", __LINE__, __FILE__);
            }
       return  allowed;
   }

   Spatial::FMTSpatialSchedule FMTSaModel::_doEventsSpread(const Spatial::FMTSpatialSchedule& p_actual) const
   {
       Spatial::FMTSpatialSchedule newSolution(p_actual);
       try {
           newSolution.enableSolutionTracker(m_SpatialGraphs);
           const int MODEL_LENGTH = getParameter(Models::FMTintmodelparameters::LENGTH);
           std::vector<
           std::vector<Spatial::FMTSpatialSchedule::EventSpread>>AllPotentials;
           for (int period = 1; period <= MODEL_LENGTH; ++period)
                {
                const  std::vector<Spatial::FMTSpatialSchedule::EventSpread> LOCAL = 
                    newSolution.getPotentialSpread(period);
                if (!LOCAL.empty())
                    {
                    AllPotentials.push_back(LOCAL);
                    }
                }
           std::shuffle(AllPotentials.begin(), AllPotentials.end(), m_generator);
           const size_t MOVE_SIZE = _getRandomMoveSize(AllPotentials.begin()->size());
           std::shuffle(AllPotentials.begin()->begin(), 
               AllPotentials.begin()->begin() + MOVE_SIZE, m_generator);
           newSolution.setSpread(AllPotentials.begin()->begin(),
               AllPotentials.begin()->begin() + MOVE_SIZE);
       }catch (...)
            {
           _exhandler->raiseFromCatch("",
               "FMTSaModel::_doEventsSpread", __LINE__, __FILE__);
            }
       return newSolution;
   }


   std::vector<bool> FMTSaModel::_getFromBindings(const Spatial::FMTSpatialSchedule::actionbindings& bindingactions,
                                                            bool adjacency) const
   {
       std::vector<bool>selectedActions(actions.size(), false);
       try {
           bool gotaction = false;
           for (const auto& binding : bindingactions)
           {
               for (size_t actionid = 0; actionid < actions.size(); ++actionid)
               {
                   if (!adjacency&& binding.at(actionid).isSpatiallyAreaBinding())
                   {
                       selectedActions[actionid] = true;
                       gotaction = true;
                   }
                   else if (adjacency && binding.at(actionid).isSpatiallyAdjacencyBinding())
                   {
                       selectedActions[actionid] = true;
                       gotaction = true;
                   }

               }
           }
           if (!gotaction)
           {
               selectedActions.clear();
           }
       }catch (...)
       {
           _exhandler->raiseFromCatch("", "FMTSaModel::_getFromBindings", __LINE__, __FILE__);
       }
       return selectedActions;
   }

  

    Spatial::FMTSpatialSchedule FMTSaModel::_doEventsAreaConflictDestrutorMove(const Spatial::FMTSpatialSchedule& actual,
        const Spatial::FMTSpatialSchedule::actionbindings& bindings) const
    {
        try {
            const std::vector<bool>selectedActions = _getFromBindings(bindings);
            const int period = actual.getPeriodWithMaximalEvents(selectedActions);
            const std::vector<std::vector<Spatial::FMTCoordinate>> selectionpool = actual.getAreaConflictCoordinates(bindings, period);
            return _doConflictDestruction(actual, bindings, selectionpool, period);
        }
        catch (...)
        {
            _exhandler->raiseFromCatch("", "FMTSaModel::_doEventsAreaConflictDestrutorMove", __LINE__, __FILE__);
        }
        return actual;
    }

    Spatial::FMTSpatialSchedule FMTSaModel::_doLocalMove(const Spatial::FMTSpatialSchedule& actual,
        const Spatial::FMTSpatialSchedule::actionbindings& bindings) const
    {
        try {
            const size_t MOVE_SIZE = _getLocalMoveSize();
            std::uniform_int_distribution<int> perioddistribution(1, actual.actPeriod() - 1);//period to change
            std::vector<Spatial::FMTCoordinate> selectionPool;
            int period = 0;
            while (selectionPool.empty())
            {
                period = perioddistribution(m_generator);
                selectionPool = actual.getMovableCoordinates(*this, period);
            }
            if (selectionPool.empty())
            {
                _exhandler->raise(Exception::FMTexc::FMTrangeerror,
                    "Empty solution ", "FMTSaModel::move()", __LINE__, __FILE__);
            }
            std::shuffle(selectionPool.begin(), selectionPool.end(), m_generator);
            std::vector<Spatial::FMTCoordinate>::const_iterator luckycoordinateit = selectionPool.begin();
            size_t perturbationdone = 0;
            const size_t SELECTED = std::min(MOVE_SIZE, selectionPool.size());
            _logger->logWithLevel("Local Move Selected " + std::to_string(SELECTED)+"\n", 2);
            Spatial::FMTSpatialSchedule newsolution(actual, selectionPool.begin(), selectionPool.begin() + SELECTED);
            newsolution.enableSolutionTracker(m_SpatialGraphs);
            while (luckycoordinateit != selectionPool.end() && perturbationdone < MOVE_SIZE)
            {
                newsolution.perturbGraph(*luckycoordinateit, period, *this, m_generator, bindings);
                ++perturbationdone;
                ++luckycoordinateit;
            }
            return newsolution;
        }
        catch (...)
        {
            _exhandler->raiseFromCatch("", "FMTSaModel::_doLocalMove", __LINE__, __FILE__);
        }
        return actual;
    }

	Spatial::FMTSpatialSchedule FMTSaModel::_move(const Spatial::FMTSpatialSchedule& actual,
										const Spatial::FMTSpatialSchedule::actionbindings& bindings) const
		{
		try {
            const FMTsamove MOVE = _getAMove(actual,bindings);
            m_CycleMoves.push_back(FMTMoveStats());
            m_CycleMoves.back().MoveType = MOVE;
            switch (MOVE)
            {
            case FMTsamove::Local:
            {
                return _doLocalMove(actual, bindings);
                break;
            }
            /*case FMTsamove::ReBuilder:
            {
                return _GetRebuild(actual);
                break;
            }*/
            case FMTsamove::AreaConflictDestrutor:
            {
                return _doEventsAreaConflictDestrutorMove(actual, bindings);
                break;
            }
            case FMTsamove::AdjacencyConflictDestrutor:
            {
                return _doEventsAdjacencyConflictDestrutorMove(actual, bindings);
                break;
            }
            case FMTsamove::GroupsConflictDestructor:
            {
                return _doGroupsConflictDestrutorMove(actual);
                break;
            }
            case FMTsamove::EventsSpread:
            {
                return _doEventsSpread(actual);
                break;
            }
            default:
                break;
            }
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTSaModel::_move", __LINE__, __FILE__);
			}
		//return newsolution;
		return actual;
		}

    void FMTSaModel::_initialGrow()
    {
        try {
            int modellength = getParameter(Models::FMTintmodelparameters::LENGTH);
            while (modellength>0)
                {
                m_BestSolution.grow();
                --modellength;
                }
        }catch (...)
        {
            _exhandler->raiseFromCatch("", "FMTSaModel::_initialGrow", __LINE__, __FILE__);
        }
    }

    void FMTSaModel::_randomBuild()
    {
        try {
            int modellength = getParameter(Models::FMTintmodelparameters::LENGTH);
            while (modellength > 0)
            {
                m_BestSolution.randomBuild(*this, m_generator);
                --modellength;
            }
        }
        catch (...)
        {
            _exhandler->raiseFromCatch("", "FMTSaModel::_randomBuild", __LINE__, __FILE__);
        }
    }

    #ifdef FMTWITHOSI
        Models::FMTLpModel FMTSaModel::_getRandomLpModel(const Spatial::FMTSpatialSchedule& p_SpatialSchedule) const
            {
            Models::FMTLpModel newLp;
            try {
                    std::vector<Core::FMTConstraint> shuffledConstraints;
                    const std::vector<double> FACTORS = p_SpatialSchedule.getConstraintsFactor();
                    size_t constraintId = 0;
                    for (const Core::FMTConstraint& CONSTRAINT : constraints)
                        {
                        if (!CONSTRAINT.isSpatial())
                            {
                            shuffledConstraints.push_back(CONSTRAINT);
                            if (CONSTRAINT.isObjective())
                            {
                                const std::string SENSE = CONSTRAINT.sense() < 0 ? "-" : "+";
                                std::vector<std::string>Weights(1, "_ALL");
                                shuffledConstraints.back().setPenalties(SENSE, Weights);
                            }else {
                                shuffledConstraints.back().setGoal("G" + std::to_string(constraintId),
                                    FACTORS.at(constraintId));
                                }
                            }
                        ++constraintId;
                        }
                    std::vector<Core::FMTConstraint>::iterator firstToShuffle = shuffledConstraints.begin();
                    if (!constraints.empty() &&
                        constraints.at(0).isObjective())
                        {
                        ++firstToShuffle;
                        }
                    std::shuffle(firstToShuffle, shuffledConstraints.end(), m_generator);
                    std::vector<Core::FMTActualDevelopment>shuffledArea(area);
                    std::shuffle(shuffledArea.begin(), shuffledArea.end(), m_generator);
                    FMTsolverinterface SolverInterface = FMTsolverinterface::CLP;
                    if (Version::FMTVersion::hasFeature("MOSEK"))
                        {
                       SolverInterface = FMTsolverinterface::MOSEK;
                        }
                    newLp = Models::FMTLpModel(*this, SolverInterface);
                    newLp.setArea(shuffledArea);
                    newLp.setConstraints(shuffledConstraints);
                    newLp.setParameter(Models::FMTboolmodelparameters::QUIET_LOGGING, true);
            }catch (...)
                {
                _exhandler->raiseFromCatch("", "FMTSaModel::_getRandomLpModel", __LINE__, __FILE__);
                }
            return  newLp;
            }
    #endif


  

   void  FMTSaModel::_resetTabouMoves()
        {
        if (m_TotalMoves % m_TABOU_FLUSH  == 0)
            {
            for (auto& MOVE : m_NotAcceptedMovesCount)
                {
                MOVE = size_t(0);
                }
            }
        }

 


   void  FMTSaModel::_getConstraintsStats(const Spatial::FMTSpatialSchedule& p_NewBestSolution,
       double& p_Objective, double& p_SpatialRatio, double& p_InventoryRatio, double& p_TotalRatio,
       double& p_PrimalInf) const
   {
       size_t i = 0;
       double spatialCount = 0;
       double brokenSpatial = 0;
       double InventoryCount = 0;
       double brokenInventory = 0;
       double totalBroken = 0;
       double Total = 0;
       for (const double VALUE : p_NewBestSolution.getConstraintsValues(m_SpatialGraphs))
            {
            if (i == 0)
               {
               p_Objective = VALUE;
            }else {
                const double BROKEN_UP = static_cast<double>(VALUE > 0);
                if (constraints.at(i).isSpatial())
                    {
                    brokenSpatial += BROKEN_UP;
                    ++spatialCount;
                }else if (constraints.at(i).isInventory())
                    {
                    brokenInventory += BROKEN_UP;
                    ++InventoryCount;
                    }
                totalBroken += BROKEN_UP;
                p_PrimalInf += VALUE;
                ++Total;
                }
            ++i;
            }
       p_SpatialRatio = brokenSpatial / spatialCount;
       p_InventoryRatio = brokenInventory / InventoryCount;
       p_TotalRatio = totalBroken / Total;
   }


    std::vector<Core::FMTSchedule>FMTSaModel::_GetSchedules(const Spatial::FMTSpatialSchedule& p_SpatialSchedule, bool withlock) const
    {
        std::vector<Core::FMTSchedule>newSchedules;
        try {
                #ifdef FMTWITHOSI
                     Models::FMTLpModel newLp = _getRandomLpModel(p_SpatialSchedule);
                     Logging::FMTQuietLogger TEMP_LOGGER;
                     newLp.setParallelLogger(TEMP_LOGGER);
                    if (newLp.doPlanning(true))
                        {
                        for (int period = 1; period <= getParameter(Models::FMTintmodelparameters::LENGTH);++period)
                            {
                            newSchedules.push_back(newLp.getSolution(period, withlock));
                            }
                    }else {
                        newSchedules = getSchedules(p_SpatialSchedule, withlock);
                        }
                #else
                    newSchedules = getSchedules(p_SpatialSchedule, withlock);
                #endif
        }catch (...)
            {
            _exhandler->raiseFromCatch("", "FMTSaModel::_GetSchedules", __LINE__, __FILE__);
            }
        return newSchedules;
    }


    Spatial::FMTSpatialSchedule FMTSaModel::_getRebuild(const Spatial::FMTSpatialSchedule& actual) const
    {
        Spatial::FMTSpatialSchedule newsolution;
        try {
            const std::vector<Core::FMTSchedule>nonspatialschedules = _GetSchedules(actual,true);
            if (nonspatialschedules.empty())
            {
                _exhandler->raise(Exception::FMTexc::FMTrangeerror,
                    "Cannot rebuild empty solution", "FMTSaModel::_getRebuild", __LINE__, __FILE__);
            }
            newsolution = getNewSolution(actual);

            const std::vector<double>& FACTORS = actual.getConstraintsFactor();
            if (!FACTORS.empty())
            {
                newsolution.setConstraintsFactor(*this, FACTORS);
            }
            for (const Core::FMTSchedule& schedule : nonspatialschedules)
                {
                greedyReferenceBuild(newsolution, schedule,
                    getParameter(NUMBER_OF_ITERATIONS),
                    getParameter(Models::FMTintmodelparameters::SEED), FMT_DBL_TOLERANCE, false);
                }
        }catch (...)
        {
            _exhandler->raiseFromCatch("", "FMTSaModel::_getRebuild", __LINE__, __FILE__);
        }
        return newsolution;
    }

    bool  FMTSaModel::_allowMove(const FMTsamove& move) const
    {
        try {
            return  (m_NotAcceptedMovesCount.at(static_cast<int>(move) - 1) <= m_MINIMAL_ACCEPTED_MOVES);
        }catch (...)
        {
            _exhandler->raiseFromCatch("", "FMTSaModel::_allowMove", __LINE__, __FILE__);
        }
        return false;
    }

    bool FMTSaModel::_allowAnyMove() const
    {
        try {
            for (int enumid = 1; enumid < FMTsamove::MoveCount; ++enumid)
                {
                if (_allowMove(static_cast<FMTsamove>(enumid)))
                    {
                    return true;
                    }
                }
        }
        catch (...)
        {
            _exhandler->raiseFromCatch("", "FMTSaModel::_allowAnyMove", __LINE__, __FILE__);
        }
    return false;
    }

    std::vector<FMTSaModel::FMTsamove> FMTSaModel::_getNonTabouMoves() const
    {
        std::vector<FMTsamove>potentialMoves;
        potentialMoves.reserve(
            static_cast<size_t>(FMTsamove::MoveCount));
        for (int i = 1; i < static_cast<int>(FMTsamove::MoveCount);++i)
        {
            if (m_NotAcceptedMovesCount.at(i - 1)
                <= m_MAX_NON_ACCEPTED_MOVES_FOR_TABOU)
            {
                potentialMoves.push_back(static_cast<FMTsamove>(i));
            }
        }
        return potentialMoves;
    }

    bool FMTSaModel::_allowMove(FMTsamove p_move, const Spatial::FMTSpatialSchedule& p_actual,
        const Spatial::FMTSpatialSchedule::actionbindings& p_bindings) const
    {
        bool useMove = false;
        switch (p_move)
            {
                case FMTsamove::AreaConflictDestrutor:
                {
                    useMove = _allowAreaDestruction(p_actual, p_bindings);
                    break;
                }
                case FMTsamove::AdjacencyConflictDestrutor:
                {
                    useMove = _allowAdjacencyDestruction(p_actual, p_bindings);
                    break;
                }
                case FMTsamove::GroupsConflictDestructor:
                {
                    useMove = _allowGroupDestruction(p_actual);
                    break;
                }
                case FMTsamove::EventsSpread:
                {
                    useMove = _allowEventsSpread(p_actual);
                    break;
                }
                default:
                    useMove = true;
                    break;
            }
        return useMove;
    }



    FMTSaModel::FMTsamove FMTSaModel::_getAMove(const Spatial::FMTSpatialSchedule& actual,
                            const Spatial::FMTSpatialSchedule::actionbindings& bindings) const
    {
        FMTsamove returned =  FMTsamove::Local;
        try {
            std::vector<FMTsamove>potentialMoves;
            potentialMoves.reserve(static_cast<size_t>(FMTsamove::MoveCount));
            for (const FMTsamove& MOVE : _getNonTabouMoves())
                {
                if (_allowMove(MOVE, actual, bindings))
                    {
                    potentialMoves.push_back(MOVE);
                    }
                }
            if (!potentialMoves.empty())
                {
                std::shuffle(potentialMoves.begin(),
                    potentialMoves.end(), m_generator);
                returned = potentialMoves.back();
                }
           
        }catch (...)
        {
            _exhandler->raiseFromCatch("", "FMTSaModel::_getAMove", __LINE__, __FILE__);
        }
        return returned;
    }


    void FMTSaModel::_schedulesBuild(const std::vector<Core::FMTSchedule>& schedules)
    {
        try {
            const size_t alliterations = static_cast<size_t>(getParameter(Models::FMTintmodelparameters::NUMBER_OF_ITERATIONS));
            for (const Core::FMTSchedule& schedule : schedules)
                {
                greedyReferenceBuild(m_BestSolution, schedule, alliterations);
                }
        }
        catch (...)
        {
            _exhandler->raiseFromCatch("", "FMTSaModel::_schedulesBuild", __LINE__, __FILE__);
        }

    }



	double FMTSaModel::_warmup(const Spatial::FMTSpatialSchedule& actual,
		const Spatial::FMTSpatialSchedule::actionbindings& bindings)
		{
		double temperature = 0;
		try {
        const std::vector<double>actuals = getConstraintsValues(actual);
        std::vector<double>maximals = actuals;
        std::vector<double>deltasums(constraints.size(), 0);
        size_t iterations = m_WARM_UP_ITERATIONS;
        const double totalits = static_cast<double>(m_WARM_UP_ITERATIONS);
        //double deltasum = 0;
        while (iterations > 0)
        {
            const Spatial::FMTSpatialSchedule newsolution = _move(actual, bindings);
            size_t cntid = 0;
            for (const double& value : getConstraintsValues(newsolution))
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
            --iterations;
        }
        double deltasum = 0;
        size_t cntid = 0;
        //double AverageFactor = 0.0;
        //double AverageCount = 0.0;
        for (double& value : maximals)//Need to normalize the calculated delta
        {
            value = getConstraintFactor(cntid, value);
            /*if (isValidFactor(value))
                {
                AverageFactor += value;
                AverageCount += 1;
                }*/
            deltasum += (deltasums[cntid] * value);
            ++cntid;
        }
        //maximals[0] = (AverageFactor / AverageCount);
        temperature = (- (deltasum / totalits) * 100) / std::log(
            getParameter(Models::FMTdblmodelparameters::INITIAL_ACCEPTANCE_PROBABILITY));
        m_BestSolution.setConstraintsFactor(*this, maximals);
        m_BestObjective = getGlobalObjective(m_BestSolution);
        }catch (...)
            {
            _exhandler->raiseFromCatch("", "FMTSaModel::_warmup", __LINE__, __FILE__);
            }
        return temperature;
        }

        bool FMTSaModel::isProvenOptimal() const
        {
            try {
               
                return ((m_TotalMoves >= getParameter(FMTintmodelparameters::MAX_MOVES)) ||
                             (m_CoolingSchedule->getTemp() < FMT_DBL_TOLERANCE && m_TotalMoves > 0 && !_allowAnyMove()));
            }
            catch (...)
            {
                _exhandler->raiseFromCatch("", "FMTSaModel::isProvenOptimal", __LINE__, __FILE__);
            }
            return false;
        }

        size_t FMTSaModel::_getCycleMoves() const
        {
            return m_CycleMoves.size();
        }

        size_t FMTSaModel::_getAcceptedCycleMoves() const
        {
            size_t count = 0;
            for (const FMTSaModel::FMTMoveStats& stats : m_CycleMoves)
            {
                if (stats.Accepted)
                    {
                    ++count;
                    }
            }
            return count;
        }

        bool FMTSaModel::_isCycleProvenOptimal() const
        {
            try {
                return (!((_getCycleMoves() - _getAcceptedCycleMoves()) < getParameter(FMTintmodelparameters::MAX_ACCEPTED_CYCLE_MOVES) && 
                    _getCycleMoves() < getParameter(FMTintmodelparameters::MAX_CYCLE_MOVES) &&
                    m_TotalMoves < getParameter(FMTintmodelparameters::MAX_MOVES)));
            }
            catch (...)
            {
                _exhandler->raiseFromCatch("", "FMTSaModel::_isCycleProvenOptimal", __LINE__, __FILE__);
            }
            return false;
        }

        void FMTSaModel::_doFactorization()
        {
            try {
                if (m_TotalMoves % 100 == 0)
                {
                    doRefactorization(m_BestSolution);
                }
            }
            catch (...)
            {
                _exhandler->raiseFromCatch("", "FMTSaModel::_doFactorization", __LINE__, __FILE__);
            }
        }

        void FMTSaModel::_logSolutionStatus() const
        {
            try {
                if (m_TotalMoves % 100 == 0)
                {
                    double objective = 0;
                    double primalinf = 0;
                    getSolutionStatus(m_BestSolution, objective, primalinf);
                    m_BestSolution.logSolutionStatus(m_TotalMoves, objective, primalinf);
                    
                
            }
        }
        catch (...)
        {
            _exhandler->raiseFromCatch("", "FMTSaModel::_logSolutionStatus", __LINE__, __FILE__);
        }
    }

    void FMTSaModel::logMovesReport() const
        {
        try {
            *_logger << "Moves report" << "\n";
                std::map<FMTsamove, size_t>Numberofaccepted;
                std::map<FMTsamove, size_t>Numberofdone;
                std::map<FMTsamove, double>TotalGain;
                std::map<double, size_t>acceptedpersize;
                for (const FMTSaModel::FMTMoveStats& stats : m_CycleMoves)
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
                    const std::string moveid = "   Move: " + getMovesName(element.first);
                    const std::string acceptedratio = "    Accepted (" + std::to_string((static_cast<double>(element.second) / static_cast<double>(Numberofdone.at(element.first))) * 100) + "%)";
                    *_logger << moveid + "\n" + acceptedratio  << "\n";
                }
           *_logger << "Moves report done" << "\n";
        }catch (...)
            {
            _exhandler->raiseFromCatch("", "FMTSaModel::logMovesReport", __LINE__, __FILE__);
            }
        }

    void FMTSaModel::_logCycleStatus() const
    {
        try {
             const double acceptanceratio = (static_cast<double>(_getAcceptedCycleMoves()) / static_cast<double>(_getCycleMoves())) * 100;
             const double Temperature = m_CoolingSchedule->getTemp();
             if (_logger->logWithLevel("Temp(" + std::to_string(Temperature) + ") Accepted(" + std::to_string(acceptanceratio) + "%) Level(" + std::to_string(m_CoolingSchedule->getLevel()) + ")\n", 1))
                {
                logMovesReport();
                }
             
        }catch (...)
        {
            _exhandler->raiseFromCatch("", "FMTSaModel::_logCycleStatus", __LINE__, __FILE__);
        }
    }


	bool FMTSaModel::initialSolve()
		{
		try {
			const Spatial::FMTSpatialSchedule::actionbindings ACTIONS_BINDING = 
                m_BestSolution.getBindingActionsByPeriod(*this);
            m_BestSolution.setStaticsMovableCoordinates(*this);
            *_logger << "Generator initial state: " + std::to_string(m_generator()) << "\n";
			const double INITIAL_TEMPERATURE = _warmup(m_BestSolution, ACTIONS_BINDING);
            m_CoolingSchedule->setInitialTemperature(INITIAL_TEMPERATURE);
			while (!isProvenOptimal())
				{
                m_CycleMoves.clear();
				while (!_isCycleProvenOptimal())
					{
					Spatial::FMTSpatialSchedule newSolution = _move(m_BestSolution, ACTIONS_BINDING);
                    const double CANDIDAT_OBJECTIVE = getGlobalObjective(newSolution);
                    if (_isBetter(CANDIDAT_OBJECTIVE))
                        {
                        _setBestSolutionTo(newSolution,
                            CANDIDAT_OBJECTIVE);
                        m_CycleMoves.back().Accepted = true;
                        }
                    _doFactorization();
                    _logSolutionStatus();
                    _resetTabouMoves();
					++m_TotalMoves;
					}
                _UpdateFailedMoveCount();
                _logCycleStatus();
                _writeDisrturbances();
				_coolDown();
               	}
            logConstraintsFactors();
            logConstraintsInfeasibilities();
            *_logger << "Generator final state: " << m_generator() << "\n";
		}catch (...)
			{
			_exhandler->printExceptions("", "FMTSaModel::initialSolve", __LINE__, __FILE__);
			}
        return true;
		}

    void FMTSaModel::_setBestSolutionTo(Spatial::FMTSpatialSchedule& p_NewBestSolution,
                                        double p_ObjectiveValue)
    {
        if (p_NewBestSolution.isPartial())
        {
            m_BestSolution.copyFromPartial(p_NewBestSolution);
        }
        else {
            m_BestSolution.swap(p_NewBestSolution);
        }
        m_BestObjective = p_ObjectiveValue;
    }

    bool FMTSaModel::build(std::vector<Core::FMTSchedule> schedules)
    {
        try {
            if (schedules.empty()||
                (!schedules.empty() && schedules.begin()->empty())) //From no solution
            {
                    _randomBuild();
            }else {
                _schedulesBuild(schedules);
                }
        }catch (...)
        {
            _exhandler->printExceptions("", "FMTSaModel::build", __LINE__, __FILE__);
        }
        return true;
    }

 
    bool FMTSaModel::solve()
    {
        try {
            return initialSolve();
        }catch (...)
        {
            _exhandler->printExceptions("", "FMTSaModel::solve", __LINE__, __FILE__);
        }
        return false;
    }

    std::unique_ptr<FMTModel>FMTSaModel::presolve(std::vector<Core::FMTActualDevelopment> optionaldevelopments) const
    {
        try {
            std::unique_ptr<FMTModel>BASE_PRESOLVE = FMTSeModel::presolve(optionaldevelopments);
            std::unique_ptr<FMTModel>PRESOLVED = std::unique_ptr<FMTModel>(
                                new FMTSaModel(
                                    *(dynamic_cast<FMTSeModel*>(BASE_PRESOLVE.get()))));
            FMTSaModel* SA = dynamic_cast<FMTSaModel*>(PRESOLVED.get());
            return PRESOLVED;
        }
        catch (...)
        {
            _exhandler->printExceptions("", "FMTSaModel::presolve", __LINE__, __FILE__);
        }
        return std::unique_ptr<FMTModel>(nullptr);
    }

    std::unique_ptr<FMTModel>FMTSaModel::getCopy(int period) const
    {
        try {
            return std::unique_ptr<FMTModel>(new FMTSaModel(*dynamic_cast<FMTSeModel*>(FMTSeModel::getCopy(period).get())));
        }
        catch (...)
        {
            _exhandler->printExceptions("", "FMTSaModel::getCopy", __LINE__, __FILE__);
        }
        return std::unique_ptr<FMTModel>(nullptr);
    }

    FMTSaModel::~FMTSaModel() = default;



}

BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTSaModel)