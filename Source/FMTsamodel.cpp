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

namespace Models

{

    void FMTsamodel::CoolDown()
    {
        try {
            CoolingSchedule->ReduceTemp();
        }catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::CoolDown", __LINE__, __FILE__);
        }
    }

    void FMTsamodel::UpdateFailedMoveCount()
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
            _exhandler->raisefromcatch("", "FMTsamodel::UpdateFailedMoveCount", __LINE__, __FILE__);
        }
    }


    FMTsamodel::~FMTsamodel() = default;

    FMTsamodel::FMTsamodel():
        FMTsemodel(),
        TotalMoves(),
        CycleMoves(),
        LastGlobalObjectiveValue(),
        CoolingSchedule(),
        NotAcceptedMovesCount()
    {

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
            CoolingSchedule = std::move(CoolingSchedule->Clone());
            }
        return *this;
    }

    void FMTsamodel::swap_ptr(const std::unique_ptr<FMTmodel>& rhs)
    {
        *this = std::move(*dynamic_cast<FMTsamodel*>(rhs.get()));
    }

	std::unique_ptr<FMTmodel>FMTsamodel::clone() const
		{
		return std::unique_ptr<FMTmodel>(new FMTsamodel(*this));
		}


    Graph::FMTgraphstats FMTsamodel::buildperiod()
    {
		return solution.randombuild(*this,m_generator);
    }


	bool FMTsamodel::IsBetter(const Spatial::FMTspatialschedule& actual,const Spatial::FMTspatialschedule& candidat) const
		{
		try {
            const double temp = CoolingSchedule->GetTemp();
			const double actualglobalobjective = actual.getglobalobjective(*this);
			const double candidatglobalobjective = candidat.getglobalobjective(*this);
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
			_exhandler->raisefromcatch("For move type "+std::to_string(CycleMoves.back().MoveType), "FMTsamodel::IsBetter", __LINE__, __FILE__);
		}
		return false;
		}
   size_t FMTsamodel::GetLocalMoveSize() const
    {
       size_t sizeofmove = 0;
       try {
           const double mapratio = (CoolingSchedule->GetTemp() / CoolingSchedule->GetInitialTemp());
           const size_t maxmoves = static_cast<size_t>(static_cast<double>(solution.size()/10) * mapratio);
           const size_t movamaximalsize = std::max(size_t(1), maxmoves);//10 % of the map
           std::uniform_int_distribution<int> mosizedist(1, static_cast<int>(movamaximalsize));
           sizeofmove = mosizedist(m_generator);
           CycleMoves.back().MoveSize = sizeofmove;
       }
       catch (...)
       {
           _exhandler->raisefromcatch("", "FMTsamodel::GetLocalMoveSize", __LINE__, __FILE__);
       }
       return sizeofmove;
    }

   Spatial::FMTspatialschedule FMTsamodel::DoConflictDestruction(const Spatial::FMTspatialschedule& actual,
       const Spatial::FMTspatialschedule::actionbindings& bindings,
       std::vector<std::vector<Spatial::FMTcoordinate>> selectionpool, const int& period) const
   {
       try {
           const size_t movesize = GetLocalMoveSize();
           if (selectionpool.empty())
           {
               _exhandler->raise(Exception::FMTexc::FMTrangeerror,
                   "Empty solution ", "FMTsamodel::move()", __LINE__, __FILE__);
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
           Spatial::FMTspatialschedule newsolution(actual, finalselection.begin(), finalselection.end());
           while (luckycoordinateit != finalselection.end())
           {
               newsolution.perturbgraph(*luckycoordinateit, period, *this, m_generator, bindings);
               ++luckycoordinateit;
           }
           return newsolution;
       }
       catch (...)
       {
           _exhandler->raisefromcatch("", "FMTsamodel::DoConflictDestruction", __LINE__, __FILE__);
       }
       return actual;
   }

   Spatial::FMTspatialschedule FMTsamodel::DoEventsAdjacencyConflictDestrutorMove(const Spatial::FMTspatialschedule& actual,
       const Spatial::FMTspatialschedule::actionbindings& bindings) const
   {
       try {
           const std::vector<bool>selectedactions = GetFromBindings(bindings);
           const int period = actual.getperiodwithmaximalevents(selectedactions);
           const std::vector<std::vector<Spatial::FMTcoordinate>> selectionpool = actual.getadjacencyconflictcoordinates(bindings, period);
           return DoConflictDestruction(actual, bindings, selectionpool, period);
       }catch (...)
            {
           _exhandler->raisefromcatch("", "FMTsamodel::DoEventsAdjacencyConflictDestrutorMove", __LINE__, __FILE__);
            }
       return actual;
   }

   bool FMTsamodel::AllowDestruction(const Spatial::FMTspatialschedule& actual, const Spatial::FMTspatialschedule::actionbindings& bindings) const
   {
       try {
           const std::vector<bool>selectedactions = GetFromBindings(bindings);
           return (actual.getperiodwithmaximalevents(selectedactions) > 0);
       }
       catch (...)
       {
           _exhandler->raisefromcatch("", "FMTsamodel::FMTsamodel::AllowDestruction", __LINE__, __FILE__);
       }
       return false;
   }


   std::vector<bool> FMTsamodel::GetFromBindings(const Spatial::FMTspatialschedule::actionbindings& bindingactions) const
   {
       std::vector<bool>selectedactions(actions.size(), false);
       try {
           bool gotaction = false;
           for (const auto& binding : bindingactions)
           {
               for (size_t actionid = 0; actionid < actions.size(); ++actionid)
               {
                   if (binding.at(actionid).isspatialyareabinding())
                   {
                       selectedactions[actionid] = true;
                       gotaction = true;
                   }
               }
           }
           if (!gotaction)
           {
               _exhandler->raise(Exception::FMTexc::FMTrangeerror,
                   "No action selected", "FMTsamodel::GetFromBinding", __LINE__, __FILE__);
           }
       }catch (...)
       {
           _exhandler->raisefromcatch("", "FMTsamodel::GetFromBindings", __LINE__, __FILE__);
       }
       return selectedactions;
   }

  

    Spatial::FMTspatialschedule FMTsamodel::DoEventsAreaConflictDestrutorMove(const Spatial::FMTspatialschedule& actual,
        const Spatial::FMTspatialschedule::actionbindings& bindings) const
    {
        try {
            const std::vector<bool>selectedactions = GetFromBindings(bindings);
            const int period = actual.getperiodwithmaximalevents(selectedactions);
            const std::vector<std::vector<Spatial::FMTcoordinate>> selectionpool = actual.getareaconflictcoordinates(bindings, period);
            return DoConflictDestruction(actual, bindings, selectionpool, period);
        }
        catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::DoEventsAreaConflictDestrutorMove", __LINE__, __FILE__);
        }
        return actual;
    }

    Spatial::FMTspatialschedule FMTsamodel::DoLocalMove(const Spatial::FMTspatialschedule& actual,
        const Spatial::FMTspatialschedule::actionbindings& bindings,
        const std::vector<Spatial::FMTcoordinate>* movable,
        boost::unordered_map<Core::FMTdevelopment, bool>* operability) const
    {
        try {
            const size_t movesize = GetLocalMoveSize();
            std::uniform_int_distribution<int> perioddistribution(1, actual.actperiod() - 1);//period to change
            std::vector<Spatial::FMTcoordinate> selectionpool;
            int period = 0;
            while (selectionpool.empty())
            {
                period = perioddistribution(m_generator);
                selectionpool = actual.getmovablecoordinates(*this, period, movable, operability);
            }
            if (selectionpool.empty())
            {
                _exhandler->raise(Exception::FMTexc::FMTrangeerror,
                    "Empty solution ", "FMTsamodel::move()", __LINE__, __FILE__);
            }
            std::shuffle(selectionpool.begin(), selectionpool.end(), m_generator);
            std::vector<Spatial::FMTcoordinate>::const_iterator luckycoordinateit = selectionpool.begin();
            size_t perturbationdone = 0;
            const size_t selected = std::min(movesize, selectionpool.size());
            Spatial::FMTspatialschedule newsolution(actual, selectionpool.begin(), selectionpool.begin() + selected);
            //Spatial::FMTspatialschedule newsolution(actual);
            while (luckycoordinateit != selectionpool.end() && perturbationdone < movesize)
            {
                newsolution.perturbgraph(*luckycoordinateit, period, *this, m_generator, bindings);
                ++perturbationdone;
                ++luckycoordinateit;
            }
            return newsolution;
        }
        catch (...)
        {
            _exhandler->raisefromcatch("", "Spatial::FMTspatialschedule::DoLocalMove", __LINE__, __FILE__);
        }
        return actual;
    }

	Spatial::FMTspatialschedule FMTsamodel::Move(const Spatial::FMTspatialschedule& actual,
										const Spatial::FMTspatialschedule::actionbindings& bindings,
										const std::vector<Spatial::FMTcoordinate>*movable,
										boost::unordered_map<Core::FMTdevelopment, bool>*operability) const
		{
		try {
            FMTsamove move = GetAMove(actual,bindings);
            CycleMoves.push_back(FMTmovestats());
            CycleMoves.back().MoveType = move;
            switch (move)
            {
            case FMTsamove::Local:
            {
                return DoLocalMove(actual, bindings, movable, operability);
                break;
            }
            case FMTsamove::ReBuilder:
            {
                return GetRebuild(actual);
                break;
            }
            case FMTsamove::AreaConflictDestrutor:
            {
                return DoEventsAreaConflictDestrutorMove(actual, bindings);
                break;
            }
            case FMTsamove::AdjacencyConflictDestrutor:
            {
                return DoEventsAdjacencyConflictDestrutorMove(actual, bindings);
                break;
            }
            default:
                break;
            }
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTsamodel::move", __LINE__, __FILE__);
			}
		//return newsolution;
		return actual;
		}

    void FMTsamodel::InitialGrow()
    {
        try {
            int modellength = getparameter(Models::FMTintmodelparameters::LENGTH);
            while (modellength>0)
                {
                solution.grow();
                --modellength;
                }
        }catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::initialgrow", __LINE__, __FILE__);
        }
    }

    void FMTsamodel::RandomBuild()
    {
        try {
            int modellength = getparameter(Models::FMTintmodelparameters::LENGTH);
            while (modellength > 0)
            {
                solution.randombuild(*this, m_generator);
                --modellength;
            }
        }
        catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::randombuild", __LINE__, __FILE__);
        }
    }

    Spatial::FMTspatialschedule FMTsamodel::GetRebuild(const Spatial::FMTspatialschedule& actual) const
    {
        Spatial::FMTspatialschedule newsolution;
        try {
            const std::vector<Core::FMTschedule>nonspatialschedules = actual.getschedules(actions, true);
            if (nonspatialschedules.empty())
            {
                _exhandler->raise(Exception::FMTexc::FMTrangeerror,
                    "Cannot rebuild empty solution", "FMTsamodel::GetRebuild", __LINE__, __FILE__);
            }
            const Spatial::FMTforest baseforest = actual.getforestperiod(0);
            newsolution = Spatial::FMTspatialschedule(baseforest);
            const std::vector<double>factors = actual.getconstraintsfactor();
            if (!factors.empty())
            {
                newsolution.setconstraintsfactor(*this, factors);
            }
            for (const Core::FMTschedule& schedule : nonspatialschedules)
                {
                newsolution.greedyreferencebuild(schedule, *this, 10,getparameter(Models::FMTintmodelparameters::SEED), FMT_DBL_TOLERANCE,false);
                }
        }catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::GetRebuild", __LINE__, __FILE__);
        }
        return newsolution;
    }

    bool  FMTsamodel::AllowMove(const FMTsamove& move) const
    {
        try {
            return  (NotAcceptedMovesCount.at(static_cast<int>(move) - 1) <= 3);
        }catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::AllowMove", __LINE__, __FILE__);
        }
        return false;
    }

    bool FMTsamodel::AllowAnyMove() const
    {
        try {
            for (int enumid = 1; enumid < FMTsamove::MoveCount; ++enumid)
                {
                if (AllowMove(static_cast<FMTsamove>(enumid)))
                    {
                    return true;
                    }
                }
        }
        catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::AllowAnyMove", __LINE__, __FILE__);
        }
    return false;
    }


    FMTsamodel::FMTsamove FMTsamodel::GetAMove(const Spatial::FMTspatialschedule& actual, const Spatial::FMTspatialschedule::actionbindings& bindings) const
    {
        FMTsamove move =  FMTsamove::Local;
        try {
            std::vector<FMTsamove>allmoves;
            allmoves.push_back(FMTsamove::Local);
            if (!actual.emptyevents())
                {
                if (AllowDestruction(actual,bindings))
                    {
                    allmoves.push_back(FMTsamove::AreaConflictDestrutor);
                    allmoves.push_back(FMTsamove::AdjacencyConflictDestrutor);
                    }
                if (CycleMoves.size() % 500 == 0)//Each 1000 moves...
                    {
                    allmoves.push_back(FMTsamove::ReBuilder);
                    }
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
            _exhandler->raisefromcatch("", "FMTsamodel::GetAMove", __LINE__, __FILE__);
        }
        return move;
    }


    void FMTsamodel::SchedulesBuild(const std::vector<Core::FMTschedule>& schedules)
    {
        try {
            const size_t alliterations = static_cast<size_t>(getparameter(Models::FMTintmodelparameters::NUMBER_OF_ITERATIONS));
            for (const Core::FMTschedule& schedule : schedules)
                {
                solution.greedyreferencebuild(schedule,*this, alliterations);
                }
        }
        catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::schedulesbuild", __LINE__, __FILE__);
        }

    }



	double FMTsamodel::Warmup(const Spatial::FMTspatialschedule& actual,
		const Spatial::FMTspatialschedule::actionbindings& bindings,
		const std::vector<Spatial::FMTcoordinate>*movable,
		boost::unordered_map<Core::FMTdevelopment, bool>*operability,
		double initprobability, size_t iterations)
		{
		double temperature = 0;
		try {
        //const double actualobjective = actual.getglobalobjective(*this);
        const std::vector<double>actuals = actual.getconstraintsvalues(*this);
        std::vector<double>maximals = actuals;
        std::vector<double>deltasums(constraints.size(), 0);
        const double normalizationfactor = 2;
        const double totalits = static_cast<double>(iterations);
        //double deltasum = 0;
        while (iterations > 0)
        {
            const Spatial::FMTspatialschedule newsolution = Move(actual, bindings, movable, operability);
            size_t cntid = 0;
            for (const double& value : newsolution.getconstraintsvalues(*this))
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
            value = (1000 / (std::abs(value) * normalizationfactor));
            deltasum += (deltasums[cntid] * value);
            ++cntid;
        }
        temperature = -(deltasum / totalits) / std::log(initprobability);
        solution.setconstraintsfactor(*this, maximals);
                }
        catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::warmup", __LINE__, __FILE__);
        }
        return temperature;
        }

        bool FMTsamodel::isProvenOptimal() const
        {
            try {
               
                return ((TotalMoves >= getparameter(FMTintmodelparameters::MAX_MOVES)) || 
                             (CoolingSchedule->GetTemp() < FMT_DBL_TOLERANCE && TotalMoves > 0 && !AllowAnyMove()));
            }
            catch (...)
            {
                _exhandler->raisefromcatch("", "FMTsamodel::isProvenOptimal", __LINE__, __FILE__);
            }
            return false;
        }

        size_t FMTsamodel::GetCycleMoves() const
        {
            return CycleMoves.size();
        }

        size_t FMTsamodel::GetAcceptedCycleMoves() const
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

        bool FMTsamodel::isCycleProvenOptimal() const
        {
            try {
                return (!((GetCycleMoves() - GetAcceptedCycleMoves()) < 500 && GetCycleMoves() < 1000));
            }
            catch (...)
            {
                _exhandler->raisefromcatch("", "FMTsamodel::isCycleProvenOptimal", __LINE__, __FILE__);
            }
            return false;
        }

        void FMTsamodel::DoFactorization()
        {
            try {
                if (TotalMoves % 100 == 0)
                {
                    solution.dorefactortorization(*this);
                }
            }
            catch (...)
            {
                _exhandler->raisefromcatch("", "FMTsamodel::Dofactorization", __LINE__, __FILE__);
            }
        }

        void FMTsamodel::LogSolutionStatus() const
        {
            try {
                if (TotalMoves % 100 == 0)
                {
                    double objective = 0;
                    double primalinf = 0;
                    solution.getsolutionstatus(objective, primalinf, *this);
                    solution.logsolutionstatus(TotalMoves, objective, primalinf);
                    
                
            }
        }
        catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::LogSolutionStatus", __LINE__, __FILE__);
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
                    const std::string moveid = "   Move ID: " + std::to_string(static_cast<int>(element.first));
                    const std::string acceptedratio = "    Accepted (" + std::to_string((static_cast<double>(element.second) / static_cast<double>(Numberofdone.at(element.first))) * 100) + "%)";
                    const std::string averagegainperMOVE = "    Average Energy(" + std::to_string((TotalGain.at(element.first) / static_cast<double>(Numberofdone.at(element.first)))) + ")";
                    *_logger << moveid + "\n" + acceptedratio + "\n" + averagegainperMOVE << "\n";
                }
           *_logger << "Moves report done" << "\n";
        }catch (...)
            {
            _exhandler->raisefromcatch("", "FMTsamodel::LogMovesReport", __LINE__, __FILE__);
            }
        }

    void FMTsamodel::LogCycleStatus() const
    {
        try {
             const double acceptanceratio = (static_cast<double>(GetAcceptedCycleMoves()) / static_cast<double>(GetCycleMoves())) * 100;
             const double Temperature = CoolingSchedule->GetTemp();
             if (_logger->logwithlevel("Temp(" + std::to_string(Temperature) + ") Accepted(" + std::to_string(acceptanceratio) + "%) Level(" + std::to_string(CoolingSchedule->GetLevel()) + ")\n", 1))
                {
                LogMovesReport();
                }
             
        }catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::LogCycleStatus", __LINE__, __FILE__);
        }
    }


	bool FMTsamodel::initialsolve()
		{
		try {
			const std::vector<Spatial::FMTcoordinate>movables = solution.getstaticsmovablecoordinates(*this);
			const Spatial::FMTspatialschedule::actionbindings actionsbinding = solution.getbindingactionsbyperiod(*this);
			boost::unordered_map<Core::FMTdevelopment, bool>operability;
            *_logger << "Generator initial state: " << m_generator() << "\n";
			const double initialtemperature = Warmup(solution, actionsbinding,&movables,&operability,0.9);
            CoolingSchedule->SetInitialTemperature(initialtemperature);
            //LogCycleStatus();
			while (!isProvenOptimal())
				{
                //LevelMoves = 0;
                //LevelAcceptedMoves = 0;
                CycleMoves.clear();
                LastGlobalObjectiveValue = 0;
				while (!isCycleProvenOptimal())
					{
					Spatial::FMTspatialschedule newsolution = Move(solution, actionsbinding,&movables,&operability);
                    if (IsBetter( solution, newsolution))
                    {
                        if (newsolution.ispartial())
                        {
                            solution.copyfrompartial(newsolution);
                        }
                        else {
                            solution.swap(newsolution);
                        }
                        LastGlobalObjectiveValue = std::max(LastGlobalObjectiveValue, solution.getglobalobjective(*this));//Get the Worst one
                        CycleMoves.back().Accepted = true;
                    }
                    DoFactorization();
                    LogSolutionStatus();
					++TotalMoves;
					}
                UpdateFailedMoveCount();
                LogCycleStatus();
				CoolDown();
				}
            LogConstraintsFactors();
            LogConstraintsInfeasibilities();
            *_logger << "Generator final state: " << m_generator() << "\n";
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTsamodel::initialsolve", __LINE__, __FILE__);
			}
        return true;
		}

    bool FMTsamodel::build(std::vector<Core::FMTschedule> schedules)
    {
        try {
            solution.setPeriodCache(true);
            if (schedules.empty()||
                (!schedules.empty() && schedules.begin()->empty())) //From no solution
            {
                /* const bool forcepartialbuild = parameters.getboolparameter(FORCE_PARTIAL_BUILD);
                if (forcepartialbuild)
                {
                    InitialGrow();
                }else {*/
                    RandomBuild();
               // }
            }else {
                SchedulesBuild(schedules);
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
            return std::unique_ptr<FMTmodel>(new FMTsamodel(*(dynamic_cast<FMTsemodel*>(FMTsemodel::presolve(optionaldevelopments).get()))));
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



}

BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTsamodel)