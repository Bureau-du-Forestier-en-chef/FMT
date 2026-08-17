/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
#include "FMTLpSolver.h"
#include "OsiSolverInterface.hpp"

#ifdef FMTWITHGLPK
	#include "OsiGlpkSolverInterface.hpp"
#endif

#ifdef FMTWITHCLP
	#include "OsiClpSolverInterface.hpp"
#endif




#ifdef FMTWITHMOSEK
	#include "mosek.h"
	#include "OsiMskSolverInterface.hpp"
#endif

#include "FMTDefaultExceptionHandler.h"
#include "FMTSolverLogger.h"
#include "FMTSerializableMatrix.h"
#include <sstream>


namespace Models
{

	bool FMTLpSolver::canUpdateSource() const
	{
	return (solverinterface.use_count() == 2);
	}


	std::shared_ptr<OsiSolverInterface> FMTLpSolver::buildSolverInterface(const FMTSolverInterface& lsolvertype) const
	{
		std::shared_ptr<OsiSolverInterface>newsolverinterface;
		try {
			switch (lsolvertype)
			{
			case FMTSolverInterface::CLP:
				newsolverinterface = std::shared_ptr<OsiClpSolverInterface>(new OsiClpSolverInterface());
				break;
			#ifdef  FMTWITHMOSEK
				case FMTSolverInterface::MOSEK:
					newsolverinterface = std::shared_ptr<OsiMskSolverInterface>(new OsiMskSolverInterface());
				break;
			#endif
			#if __has_include("OsiGlpkSolverInterface.hpp")
				case FMTSolverInterface::GLPK:
					newsolverinterface = std::shared_ptr<OsiGlpkSolverInterface>(new OsiGlpkSolverInterface());
				break;
			#endif
				/*case FMTSolverInterface::CPLEX:
					newsolverinterface = shared_ptr<OsiCpxSolverInterface>(new OsiCpxSolverInterface);
				break;
				case FMTSolverInterface::GUROBI:
					newsolverinterface = shared_ptr<OsiGrbSolverInterface>(new OsiGrbSolverInterface);
				break;*/
			default:
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					" Cannot use this solver type ",
					"FMTLpSolver::buildSolverInterface", __LINE__, __FILE__);
				break;
			}
			
		}catch (...)
			{
			_exhandler->raiseFromCatch("Cannot build solver","FMTLpSolver::buildSolverInterface", __LINE__, __FILE__);
			}
		return newsolverinterface;
	}


	std::shared_ptr<OsiSolverInterface> FMTLpSolver::copySolverInterface(const std::shared_ptr<OsiSolverInterface>& solver_ptr,const FMTSolverInterface& lsolvertype) const
	{
		std::shared_ptr<OsiSolverInterface>newsolverinterface;
		try{
			if (solver_ptr)
				{
					newsolverinterface.reset(solver_ptr->clone(true));
					newsolverinterface->resolve();
				}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("Cannot copy solver", +"FMTLpSolver::copySolverInterface", __LINE__, __FILE__);
		}
		return newsolverinterface;
	}

	FMTLpSolver::FMTLpSolver(const FMTLpSolver& rhs) :Core::FMTObject(rhs), solverinterface(), 
		matrixcache(rhs.matrixcache),solvertype(rhs.solvertype), usecache(rhs.usecache),
		m_ColdStartParameters(rhs.m_ColdStartParameters),m_WarmStartParameters(rhs.m_WarmStartParameters)
		{
		solverinterface = copySolverInterface(rhs.solverinterface, rhs.solvertype);
		//Fix because mosek resolve in the copysolver maybe return an non optimal solution 
		if(rhs.solverinterface->isProvenOptimal() && !solverinterface->isProvenOptimal())
		{
			this->resolve();
		}
		//passInMessageHandler(*_logger);
		}

	void FMTLpSolver::swap(FMTLpSolver& rhs)
	{
		matrixcache.swap(rhs.matrixcache);
		solvertype = rhs.solvertype;
		usecache = rhs.usecache;
		solverinterface.swap(rhs.solverinterface);
		m_ColdStartParameters.swap(rhs.m_ColdStartParameters);
		m_WarmStartParameters.swap(rhs.m_WarmStartParameters);
	}

	FMTLpSolver& FMTLpSolver::operator =(const FMTLpSolver& rhs)
		{
		if (this!=&rhs)
			{
			Core::FMTObject::operator = (rhs);
			matrixcache = rhs.matrixcache;
			usecache = rhs.usecache;
			solvertype = rhs.solvertype;
			solverinterface = copySolverInterface(rhs.solverinterface,rhs.solvertype);
			m_ColdStartParameters=rhs.m_ColdStartParameters;
			m_WarmStartParameters=rhs.m_WarmStartParameters;
			if(rhs.solverinterface->isProvenOptimal() && !solverinterface->isProvenOptimal())
			{
				this->resolve();
			}
			//passInMessageHandler(*_logger);
			}
		return *this;
		}
	FMTLpSolver::FMTLpSolver(FMTSolverInterface lsolvertype,
		const std::string& p_ColdStartParameters,
		const std::string& p_WarmStartParameters,
		const std::string& p_problemName):
		Core::FMTObject(),solverinterface(),matrixcache(), solvertype(lsolvertype), usecache(true),
		m_ColdStartParameters(strtoParams(p_ColdStartParameters)),
		m_WarmStartParameters(strtoParams(p_WarmStartParameters))
		{
		solverinterface = buildSolverInterface(lsolvertype);
		//solverinterface->setStrParam(OsiStrParam::OsiProbName, p_problemName);
		passInMessageHandler(*_logger);
		/*if (solvertype == FMTSolverInterface::MOSEK)//weird in debug...
			{
			OsiMskSolverInterface* mskSolver = dynamic_cast<OsiMskSolverInterface*>(solverinterface.get());
			MSKtask_t mskTask = mskSolver->getMutableLpPtr();
			MSK_puttaskname(mskTask, const_cast<char*>(p_problemName.c_str()));
		}else {
			solverinterface->setStrParam(OsiStrParam::OsiProbName, p_problemName);//do not work in debug with msk
			}*/
		}

	bool FMTLpSolver::resolve()
		{
		try {
			matrixcache.synchronize(solverinterface);
			bool erroroccured = false;
			switch (solvertype)
			{
				case FMTSolverInterface::CLP:
				{
					OsiClpSolverInterface* clpsolver = dynamic_cast<OsiClpSolverInterface*>(solverinterface.get());
					ClpSimplex* splexmodel = clpsolver->getModelPtr();
					splexmodel->setPerturbation(-6);
					splexmodel->setSpecialOptions(64 | 128 | 1024 | 2048 | 4096 | 32768 | 262144 | 0x01000000);
					//splexmodel->tightenPrimalBounds();
					splexmodel->dual();
				}
				break;
				#ifdef  FMTWITHMOSEK
				case FMTSolverInterface::MOSEK:
				{
					MSKrescodee error = static_cast<MSKrescodee>(_mskOptimizeWithParameters());

					if (error == MSK_RES_TRM_NUMERICAL_PROBLEM) //100025 Numéro pour MSK_RES_TRM_NUMERICAL_PROBLEM
					{
						_exhandler->raise(Exception::FMTexc::FMTMSKnumerical_problem,
							getMskErrorDesc(error), "FMTLpSolver::resolve", __LINE__, __FILE__);
						MSKrescodee error = static_cast<MSKrescodee>(_mskOptimizeWithDefaultParameters());
					}
					if (error > 0)
					{
						_exhandler->raise(Exception::FMTexc::FMTmskerror,
							getMskErrorDesc(error), "FMTLpSolver::resolve", __LINE__, __FILE__);
						//In case set to warning
						solverinterface->resolve();
						erroroccured = true;
					}
				}
				break;
				#endif
				default:
				{
					solverinterface->resolve();
				}
				break;
			}
			if (erroroccured && !gotLicense())
			{
				_exhandler->raise(Exception::FMTexc::FMTmissinglicense,
					" Missing solver " + getSolverName() + " License ",
					"FMTLpSolver::resolve", __LINE__, __FILE__);
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTLpSolver::resolve", __LINE__, __FILE__);
			}
		return solverinterface->isProvenOptimal();
		}

	void FMTLpSolver::setNumberOfThreads(const size_t& nthread)
		{
		try {
			switch (solvertype)
				{
				case FMTSolverInterface::CLP:
				{
				const OsiClpSolverInterface* clpsolver = dynamic_cast<OsiClpSolverInterface*>(solverinterface.get());
				ClpSimplex* splexmodel = clpsolver->getModelPtr();
				splexmodel->setNumberThreads(static_cast<int>(nthread));
				break;
				}
				#ifdef  FMTWITHMOSEK
				case FMTSolverInterface::MOSEK:
					{
					OsiMskSolverInterface* msksolver = dynamic_cast<OsiMskSolverInterface*>(solverinterface.get());
					MSKtask_t task = msksolver->getMutableLpPtr();
					MSK_putintparam(task, MSK_IPAR_NUM_THREADS, static_cast<int>(std::max(size_t(1), nthread)));
					break;
					}
				#endif
				default:
					_exhandler->raise(Exception::FMTexc::FMTignore, "Cannot set number of threads used for "+getSolverName(),
						"FMTLpSolver::setNumberOfThreads", __LINE__, __FILE__);
				break;
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTLpSolver::setNumberOfThreads", __LINE__, __FILE__);
			}
		}

	void FMTLpSolver::setMipGapTolerance(const double& gap)
	{
		try {
			switch (solvertype)
			{
			#ifdef  FMTWITHMOSEK
			case FMTSolverInterface::MOSEK:
				{
					OsiMskSolverInterface* msksolver = dynamic_cast<OsiMskSolverInterface*>(solverinterface.get());
					MSKtask_t task = msksolver->getMutableLpPtr();
					MSK_putdouparam(task, MSK_DPAR_MIO_TOL_REL_GAP, gap);
					break;
				}
			#endif
			default:
				_exhandler->raise(Exception::FMTexc::FMTignore, "Cannot set gap tolerance for " + getSolverName(),
					"FMTLpSolver::setMipGapTolerance", __LINE__, __FILE__);
				break;
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLpSolver::setMipGapTolerance", __LINE__, __FILE__);
		}
	}

	void FMTLpSolver::mipParameters()
		{
			try {
				switch (solvertype)
				{
				#ifdef  FMTWITHMOSEK
				case FMTSolverInterface::MOSEK:
					{
						OsiMskSolverInterface* msksolver = dynamic_cast<OsiMskSolverInterface*>(solverinterface.get());
						MSKtask_t task = msksolver->getMutableLpPtr();
						MSK_putintparam(task, MSK_IPAR_MIO_FEASPUMP_LEVEL, 1);
						MSK_putintparam(task, MSK_IPAR_MIO_HEURISTIC_LEVEL, 100);
						//MSK_putintparam(task, MSK_IPAR_MIO_MAX_NUM_ROOT_CUT_ROUNDS, 1);
						MSK_putintparam(task, MSK_IPAR_MIO_CUT_SELECTION_LEVEL, 0);
						MSK_putintparam(task, MSK_IPAR_MIO_RINS_MAX_NODES, 1000);
						MSK_putintparam(task, MSK_IPAR_MIO_CUT_CLIQUE, MSK_OFF);
						MSK_putintparam(task, MSK_IPAR_MIO_CUT_CMIR, MSK_OFF);
						MSK_putintparam(task, MSK_IPAR_MIO_CUT_GMI, MSK_OFF);
						MSK_putintparam(task, MSK_IPAR_MIO_CUT_IMPLIED_BOUND, MSK_OFF);
						MSK_putintparam(task, MSK_IPAR_MIO_CUT_KNAPSACK_COVER, MSK_OFF);
						MSK_putintparam(task, MSK_IPAR_MIO_ROOT_OPTIMIZER, MSK_OPTIMIZER_INTPNT);
						MSK_putintparam(task, MSK_IPAR_MIO_NODE_OPTIMIZER, MSK_MIO_NODE_SELECTION_FIRST);

						break;
					}
				#endif
				default:
					_exhandler->raise(Exception::FMTexc::FMTignore, "Cannot set gap feasible pump level for " + getSolverName(),
						"FMTLpSolver::mipParameters", __LINE__, __FILE__);
					break;
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTLpSolver::setMipGapTolerance", __LINE__, __FILE__);
			}
		}


	void FMTLpSolver::setOptimizerMaxTime(const double& time)
		{
			try {
				switch (solvertype)
				{
				#ifdef  FMTWITHMOSEK
				case FMTSolverInterface::MOSEK:
					{
						OsiMskSolverInterface* msksolver = dynamic_cast<OsiMskSolverInterface*>(solverinterface.get());
						MSKtask_t task = msksolver->getMutableLpPtr();
						MSK_putdouparam(task, MSK_DPAR_OPTIMIZER_MAX_TIME, time);
						break;
					}
				#endif
				default:
					_exhandler->raise(Exception::FMTexc::FMTignore, "Cannot set max time for " + getSolverName(),
						"FMTLpSolver::setOptimizerMaxTime", __LINE__, __FILE__);
					break;
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTLpSolver::setMipGapTolerance", __LINE__, __FILE__);
			}
		}

#ifdef FMTWITHMOSEK
	int FMTLpSolver::_mskOptimizeWithParameters()
	{
		OsiMskSolverInterface* msksolver = dynamic_cast<OsiMskSolverInterface*>(solverinterface.get());
		msksolver->freeCachedData();
		MSKtask_t task = msksolver->getMutableLpPtr();
		if (!m_ColdStartParameters.empty())
		{
			for (const std::pair<std::string, std::string>& PARAMETER : m_ColdStartParameters)
			{
				MSK_putparam(task, PARAMETER.first.c_str(), PARAMETER.second.c_str());
			}
		}
		else {
			MSK_putintparam(task, MSK_IPAR_OPTIMIZER, MSK_OPTIMIZER_INTPNT);
			MSK_putintparam(task, MSK_IPAR_INTPNT_BASIS, MSK_BI_IF_FEASIBLE);
			MSK_putintparam(task, MSK_IPAR_SIM_HOTSTART, MSK_SIM_HOTSTART_NONE);
			MSK_putintparam(task, MSK_IPAR_PRESOLVE_USE, MSK_ON);
			MSK_putintparam(task, MSK_IPAR_INTPNT_STARTING_POINT, MSK_STARTING_POINT_CONSTANT);
			MSK_putintparam(task, MSK_IPAR_BI_CLEAN_OPTIMIZER, MSK_OPTIMIZER_PRIMAL_SIMPLEX);
			MSK_putdouparam(task, MSK_DPAR_INTPNT_TOL_PSAFE, 100.0);
			MSK_putdouparam(task, MSK_DPAR_INTPNT_TOL_PATH, 1.0e-2);
			MSK_putintparam(task, MSK_IPAR_BI_MAX_ITERATIONS, 100000000);
		}
		MSK_putintparam(task, MSK_IPAR_LICENSE_WAIT, MSK_ON);
		// Si on veut un timeout de 1h avant que ça crash
		//MSK_putdouparam(task, MSK_DPAR_LICENSE_WAIT_TIME, 3600);
		MSKrescodee error = MSK_optimize(task);
		
		return error;
	}
#endif

	void FMTLpSolver::_setClpOptions()
	{
		OsiClpSolverInterface* clpsolver = nullptr;
		clpsolver = dynamic_cast<OsiClpSolverInterface*>(solverinterface.get());
		//options.setSpecialOption(which,value1,value2)
		/** which translation is:
				which:
				0 - startup in Dual  (nothing if basis exists).:
					0 - no basis
					1 - crash
					2 - use initiative about idiot! but no crash
				1 - startup in Primal (nothing if basis exists):
					0 - use initiative
					1 - use crash
					2 - use idiot and look at further info
					3 - use sprint and look at further info
					4 - use all slack
					5 - use initiative but no idiot
					6 - use initiative but no sprint
					7 - use initiative but no crash
					8 - do allslack or idiot
					9 - do allslack or sprint
					10 - slp before
					11 - no nothing and primal(0)
				2 - interrupt handling - 0 yes, 1 no (for threadsafe)
				3 - whether to make +- 1matrix - 0 yes, 1 no
				4 - for barrier
					0 - dense cholesky
					1 - Wssmp allowing some long columns
					2 - Wssmp not allowing long columns
					3 - Wssmp using KKT
					4 - Using Florida ordering
					8 - bit set to do scaling
					16 - set to be aggressive with gamma/delta?
					32 - Use KKT
				5 - for preSolve
					1 - switch off dual stuff
				6 - extra switches
				*/
		ClpSolve options;
		options.setSolveType(ClpSolve::useBarrier);
		//options.setSolveType(ClpSolve::useBarrierNoCross);
		//Do no cross over then when you get optimal switch to primal crossover!!!!
		//options.setSolveType(ClpSolve::tryDantzigWolfe);
		//options.setSolveType(ClpSolve::usePrimalorSprint);
		//options.setSolveType(ClpSolve::tryBenders);
		options.setPresolveType(ClpSolve::presolveOn);
		//options.setSpecialOption(1, 1);
		//options.setSpecialOption(1, 2);
		//options.setSpecialOption(4, 3, 4); //WSMP Florida
		//options.setSpecialOption(4, 0); //dense cholesky
		clpsolver->setSolveOptions(options);
		//ClpSolve simplexoptions;
		//simplexoptions.setSolveType(ClpSolve::usePrimal); //Or sprint?
		//simplexoptions.setSolveType(ClpSolve::usePrimalorSprint);
		//simplexoptions.setPresolveType(ClpSolve::presolveOn);
		//clpsolver->setSolveOptions(simplexoptions);
		//clpsolver->resolve();
	}
	#ifdef FMTWITHMOSEK
		int FMTLpSolver::_mskOptimizeWithDefaultParameters()
		{
			OsiMskSolverInterface* msksolver = dynamic_cast<OsiMskSolverInterface*>(solverinterface.get());
			msksolver->freeCachedData();
			MSKtask_t new_task = msksolver->getMutableLpPtr();
			#if MSK_VERSION_MAJOR < 11
				MSK_setdefaults(new_task);
			#else
				MSK_resetparameters(new_task);
			#endif
			MSK_putintparam(new_task, MSK_IPAR_LICENSE_WAIT, MSK_ON);
			MSKrescodee error = MSK_optimize(new_task);	

			return error;
		}
	#endif


	bool FMTLpSolver::initialSolve()
	{
		try {
			matrixcache.synchronize(solverinterface);
			bool erroroccured = false;
			switch (solvertype)
			{
				case FMTSolverInterface::CLP:
				{
					OsiClpSolverInterface* clpsolver = dynamic_cast<OsiClpSolverInterface*>(solverinterface.get());
					_setClpOptions();
					clpsolver->initialSolve();
				}
				break;
				#ifdef FMTWITHMOSEK
				case FMTSolverInterface::MOSEK:
				{
					MSKrescodee error = static_cast<MSKrescodee>(_mskOptimizeWithParameters());

					if (error == MSK_RES_TRM_NUMERICAL_PROBLEM) //100025 Numéro pour MSK_RES_TRM_NUMERICAL_PROBLEM
					{
						_exhandler->raise(Exception::FMTexc::FMTMSKnumerical_problem,
							getMskErrorDesc(error), "FMTLpSolver::initialSolve", __LINE__, __FILE__);
						error = static_cast<MSKrescodee>(_mskOptimizeWithDefaultParameters());
					}

					if (error > 0)
					{
						_exhandler->raise(Exception::FMTexc::FMTmskerror,
							getMskErrorDesc(error),"FMTLpSolver::initialSolve", __LINE__, __FILE__);
					}
				}
				break;
				#endif
				/*case FMTSolverInterface::CPLEX:
					solverinterface = unique_ptr<OsiCpxSolverInterface>(new OsiCpxSolverInterface);
				break;
				case FMTSolverInterface::GUROBI:
					solverinterface = unique_ptr<OsiGrbSolverInterface>(new OsiGrbSolverInterface);
				break;*/
				default:
				{
					solverinterface->initialSolve();
				}
				break;
			}
			if (erroroccured && !gotLicense())
				{
				_exhandler->raise(Exception::FMTexc::FMTmissinglicense,
						" Missing solver " + getSolverName() + " License ",
						"FMTLpSolver::initialSolve", __LINE__, __FILE__);
				}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", +"FMTLpSolver::initialSolve", __LINE__, __FILE__);
		}
		return solverinterface->isProvenOptimal();
		}


	void FMTLpSolver::_setGlpkLogLevel(Logging::FMTLogger& p_logger)
		{
		//Glpk does not use the message handler just the level 
		//No way for now to make it work like the other solvers...will need to capture the c print from the os :(
		if (solvertype == FMTSolverInterface::GLPK)//here is a patch
			{
			const int LOG_LEVEL = p_logger.getSolverLogger()->logLevel();
			bool sense = false;
			OsiHintStrength strength = OsiHintStrength::OsiHintIgnore;
			if (LOG_LEVEL == 0)
				{
				sense = true;
				strength = OsiHintStrength::OsiHintDo;
			}else {
				sense = false;
				strength = OsiHintStrength::OsiHintTry;
				}
			solverinterface->setHintParam(OsiHintParam::OsiDoReducePrint,
				sense, strength);
			}
		}

	void FMTLpSolver::passInMessageHandler(Logging::FMTLogger& logger)
		{
		try{
		solverinterface->passInMessageHandler(dynamic_cast<CoinMessageHandler*>(logger.getSolverLogger()));
		_setGlpkLogLevel(logger);
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTLpSolver::passInMessageHandler", __LINE__, __FILE__);
			}
		}

	bool FMTLpSolver::gotLicense() const
		{
		bool licensestatus = false;
		try{
		switch (solvertype)
		{
		#ifdef FMTWITHMOSEK
				case FMTSolverInterface::MOSEK:
				{
					const OsiMskSolverInterface* msksolver = dynamic_cast<OsiMskSolverInterface*>(solverinterface.get());
					licensestatus = !msksolver->isLicenseError();

				}
		break;
		#endif
		/*case FMTSolverInterface::CPLEX:
			solverinterface = unique_ptr<OsiCpxSolverInterface>(new OsiCpxSolverInterface);
		break;
		case FMTSolverInterface::GUROBI:
			solverinterface = unique_ptr<OsiGrbSolverInterface>(new OsiGrbSolverInterface);
		break;*/
		default:
		{
			licensestatus = true;
		}
		break;
		}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", +"FMTLpSolver::gotLicense", __LINE__, __FILE__);
		}
		return licensestatus;
		}


	std::string FMTLpSolver::getSolverName() const
		{
		std::string name;
		try {
			name = toString(getSolverType());
			}catch (...)
				{
				_exhandler->raiseFromCatch("", 
					"FMTLpSolver::getSolverName", __LINE__, __FILE__);
				}
		return name;
		}

	std::vector<Models::FMTSolverInterface> FMTLpSolver::getAvailableSolverInterface()
	{
		return {
		#ifdef FMTWITHCLP
				Models::FMTSolverInterface::CLP,
		#endif
		#ifdef FMTWITHGLPK
				Models::FMTSolverInterface::GLPK,
		#endif
		#ifdef FMTWITHMOSEK
				Models::FMTSolverInterface::MOSEK,
		#endif
		};
	}

	std::string_view FMTLpSolver::toString(Models::FMTSolverInterface p_solver)
	{
		switch (p_solver)
		{
		case Models::FMTSolverInterface::CLP:   return "CLP";
		case Models::FMTSolverInterface::GLPK:  return "GLPK";
		case Models::FMTSolverInterface::MOSEK: return "MOSEK";
		default:                                return "Unknown Solver";
		}
	}

	int FMTLpSolver::getNumCols() const
		{
		return solverinterface->getNumCols() + matrixcache.numberNewCols() - matrixcache.numberOfDeletedCols();
		}

	int FMTLpSolver::getNumRows() const
	{
		return solverinterface->getNumRows() + matrixcache.numberNewRows() - matrixcache.numberOfDeletedRows();
	}

	const double* FMTLpSolver::getColLower() const
	{
		matrixcache.synchronize(solverinterface);
		return solverinterface->getColLower();
	}

	const double* FMTLpSolver::getColUpper() const
	{
		matrixcache.synchronize(solverinterface);
		return solverinterface->getColUpper();
	}

	const double* FMTLpSolver::getColSolution() const
	{
		//matrixcache.synchronize(solverinterface);
		return solverinterface->getColSolution();
	}

	const double* FMTLpSolver::getRowPrice() const
	{
		//matrixcache.synchronize(solverinterface);
		return solverinterface->getRowPrice();
	}

	const double* FMTLpSolver::getRowActivity() const
	{
		//matrixcache.synchronize(solverinterface);
		return solverinterface->getRowActivity();
	}

	const double* FMTLpSolver::getRowUpper() const
	{
		matrixcache.synchronize(solverinterface);
		return solverinterface->getRowUpper();
	}

	const double* FMTLpSolver::getRowLower() const
	{
		matrixcache.synchronize(solverinterface);
		return solverinterface->getRowLower();
	}

	double FMTLpSolver::getObjSense() const
	{
		matrixcache.synchronize(solverinterface);
		return solverinterface->getObjSense();
	}

	const double* FMTLpSolver::getObjCoefficients() const
	{
		matrixcache.synchronize(solverinterface);
		return solverinterface->getObjCoefficients();
	}

	double FMTLpSolver::getObjValue() const
		{
		return solverinterface->getObjValue();
		}


	void FMTLpSolver::addRow(int numberInRow, const int * columns, const double * elements, double rowLower, double rowUpper)
	{
		if (usecache)
		{
			matrixcache.addRow(numberInRow, columns, elements, rowLower, rowUpper);
		}
		else {
			solverinterface->addRow(numberInRow, columns, elements, rowLower, rowUpper);
		}

	}

	void FMTLpSolver::addCol(int numberInColumn, const int * rows, const double * elements, double columnLower,
		double columnUpper, double objectiveValue)
	{
		if (usecache)
		{
			matrixcache.addCol(numberInColumn, rows, elements, columnLower, columnUpper, objectiveValue);
		}
		else {
			solverinterface->addCol(numberInColumn, rows, elements, columnLower, columnUpper, objectiveValue);
		}
	}

	void FMTLpSolver::addRows(const int numrows, const int* rowStarts, const int* columns,
		const double* elements, const double* rowlb, const double* rowub)
	{
		matrixcache.synchronize(solverinterface);
		solverinterface->addRows(numrows, rowStarts, columns, elements, rowlb, rowub);
	}

	void FMTLpSolver::addCols(const int numcols, const int* columnStarts, const int* rows,
		const double* elements, const double* collb, const double* colub, const double* obj)
	{
		matrixcache.synchronize(solverinterface);
		solverinterface->addCols(numcols, columnStarts, rows, elements, collb, colub, obj);
	}
	// DocString: FMTLpSolver::isProvenOptimal
	/**
	Returns true if the program is optimal but first will synchronize the matrix with the matrix cache.
	*/
	bool FMTLpSolver::isProvenOptimal() const
	{
		return solverinterface->isProvenOptimal();
	}

	void FMTLpSolver::setColSolution(const double* newsolution)
		{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->setColSolution(newsolution);
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLpSolver::setColSolution", __LINE__, __FILE__);
		}
		}

	void FMTLpSolver::setRowPrice(const double* rowprice)
	{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->setRowPrice(rowprice);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLpSolver::setRowPrice", __LINE__, __FILE__);
		}
	}


	void FMTLpSolver::setColSetBounds(const int* indexFirst, const int* indexLast, const double* boundlist)
		{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->setColSetBounds(indexFirst, indexLast, boundlist);
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTLpSolver::setColSetBounds", __LINE__, __FILE__);
			}
		}

	void FMTLpSolver::setRowSetBounds(const int* indexFirst, const int* indexLast, const double* boundlist)
		{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->setRowSetBounds(indexFirst, indexLast, boundlist);
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTLpSolver::setRowSetBounds", __LINE__, __FILE__);
			}
		}

	void FMTLpSolver::deleteRow(const int& rowindex)
		{
		try{
			matrixcache.deleteRow(rowindex);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLpSolver::deleteRow", __LINE__, __FILE__);
		}
		}
	void FMTLpSolver::deleteCol(const int& colindex)
		{
		try {
			matrixcache.deleteCol(colindex);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLpSolver::deleteCol", __LINE__, __FILE__);
		}
			
		}

	int FMTLpSolver::getIterationCount() const
		{
		int iterations = 0;
		try{
		switch (solvertype)
		{
		case FMTSolverInterface::CLP:
			{
			const OsiClpSolverInterface* clpsolver = dynamic_cast<OsiClpSolverInterface*>(solverinterface.get());
			const ClpSimplex* splexmodel = clpsolver->getModelPtr();
			iterations = splexmodel->numberIterations();
			}
		break;
		#ifdef FMTWITHMOSEK
		case FMTSolverInterface::MOSEK:
			{
			const OsiMskSolverInterface* msksolver = dynamic_cast<OsiMskSolverInterface*>(solverinterface.get());
			const MSKtask_t task = msksolver->getMutableLpPtr();
			MSK_getintinf(task, MSK_IINF_INTPNT_ITER, &iterations);
			}
		break;
		#endif
		/*case FMTSolverInterface::CPLEX:
			solverinterface = unique_ptr<OsiCpxSolverInterface>(new OsiCpxSolverInterface);
		break;
		case FMTSolverInterface::GUROBI:
			solverinterface = unique_ptr<OsiGrbSolverInterface>(new OsiGrbSolverInterface);
		break;*/
		default:
			{
			iterations =  solverinterface->getIterationCount();
			}
		break;
		}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLpSolver::getIterationCount", __LINE__, __FILE__);
		}
		return iterations;
		}

	void FMTLpSolver::setSolverType(FMTSolverInterface& lsolvertype) const
		{
		lsolvertype = solvertype;
		}

	void FMTLpSolver::passInSolver(const FMTLpSolver& solver)
		{
		try{
		Core::FMTObject::operator=(solver);
		usecache = solver.usecache;
		matrixcache = solver.matrixcache;
		solvertype = solver.solvertype;
		solverinterface = solver.solverinterface;
		passInMessageHandler(*_logger);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLpSolver::passInSolver", __LINE__, __FILE__);
		}
		}

	void FMTLpSolver::unmarkHotStart()
	{
		try {
			solverinterface->unmarkHotStart();
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLpSolver::unmarkHotStart", __LINE__, __FILE__);
		}
	}

	void FMTLpSolver::clearRowCache()
		{
		try{
		#ifdef  FMTWITHMOSEK
			matrixcache.synchronize(solverinterface);
			if (solvertype == Models::FMTSolverInterface::MOSEK)
				{
				OsiMskSolverInterface* msksolver = dynamic_cast<OsiMskSolverInterface*>(solverinterface.get());
				msksolver->freeCachedRowRim();
				}
		#endif
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLpSolver::clearRowCache", __LINE__, __FILE__);
		}
		}

	void FMTLpSolver::setInteger(const int& colindex)
		{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->setInteger(colindex);
		}catch (...)
			{
			_exhandler->raiseFromCatch("at column index " + std::to_string(colindex), "FMTLpSolver::setInteger", __LINE__, __FILE__);
			}
		}

	bool FMTLpSolver::stockResolve()
		{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->resolve();
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLpSolver::stockResolve", __LINE__, __FILE__);
		}
		return solverinterface->isProvenOptimal();
		}

	void FMTLpSolver::setInteger(const int* indices, int len)
		{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->setInteger(indices, len);
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTLpSolver::setInteger", __LINE__, __FILE__);
			}

		}

	void FMTLpSolver::setObjective(const double* objectivevalues)
		{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->setObjective(objectivevalues);
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTLpSolver::setObjective", __LINE__, __FILE__);
			}
		}
	void FMTLpSolver::setObjSense(const double& newsense)
		{
		try {
			solverinterface->setObjSense(newsense);
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTLpSolver::setObjSense", __LINE__, __FILE__);
			}
		}

	void FMTLpSolver::deleteRows(int numberofrows, const int* rowindexes)
		{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->deleteRows(numberofrows, rowindexes);
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTLpSolver::deleteRows", __LINE__, __FILE__);
			}

		}

	void FMTLpSolver::deleteCols(int numberofcols, const int* colindexes)
		{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->deleteCols(numberofcols, colindexes);
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTLpSolver::deleteCols", __LINE__, __FILE__);
			}
		}

	const CoinPackedMatrix* FMTLpSolver::getMatrixByRow() const
		{
		try {
			matrixcache.synchronize(solverinterface);
			return solverinterface->getMatrixByRow();
		}catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTLpSolver::getMatrixByRow", __LINE__, __FILE__);
			}
		return nullptr;
		}
	const CoinPackedMatrix* FMTLpSolver::getMatrixByCol() const
		{
		try {
			matrixcache.synchronize(solverinterface);
			return solverinterface->getMatrixByCol();
		}catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTLpSolver::getMatrixByCol", __LINE__, __FILE__);
			}
		return nullptr;
		}

	void FMTLpSolver::passInLogger(const std::unique_ptr<Logging::FMTLogger>& logger)
	{
		try {
			Core::FMTObject::passInLogger(logger);
			passInMessageHandler(*getLogger());
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLpSolver::passInLogger", __LINE__, __FILE__);
		}
	}

	void FMTLpSolver::setColName(const std::string& name, const int& columnid) const
	{
		try {
			matrixcache.setColName(name, columnid);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLpSolver::setColName", __LINE__, __FILE__);
		}
	}
	
	void FMTLpSolver::setRowName(const std::string& name, const int& rowid) const
	{
		try {
			matrixcache.setRowName(name, rowid);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLpSolver::setRowName", __LINE__, __FILE__);
		}
	}

	void FMTLpSolver::updateRowsAndColsNames(bool shortformat)
	{
		try {
			matrixcache.synchronize(solverinterface);
			matrixcache.formatAllNames(shortformat);
			std::vector<std::string>& cachedrownames = matrixcache.getRowNames();
			std::vector<std::string>& cachedcolnames = matrixcache.getColumnNames();
			if(cachedcolnames.size() != static_cast<size_t>(getNumCols()) || cachedrownames.size() != static_cast<size_t>(getNumRows()))
			{
				_exhandler->raise(Exception::FMTexc::FMTrangeerror, 
								"NumCols or NumRows size is different of the number of names given", "FMTLpSolver::updateRowsAndColsNames", __LINE__, __FILE__);		
			}
			if (solvertype == Models::FMTSolverInterface::MOSEK)
			{
				#ifdef FMTWITHMOSEK
				OsiMskSolverInterface* msksolver = dynamic_cast<OsiMskSolverInterface*>(solverinterface.get());
				MSKtask_t task = msksolver->getMutableLpPtr();
				for (int colid = 0; colid < static_cast<int>(cachedcolnames.size()); ++colid)
					{
					MSK_putvarname(task, colid, cachedcolnames.at(colid).c_str());
					}
				for (int rowid = 0; rowid < static_cast<int>(cachedrownames.size()); ++rowid)
				{
					MSK_putconname(task, rowid, cachedrownames.at(rowid).c_str());
				}
				#endif
				
			}else {
				solverinterface->setColNames(cachedcolnames, 0, static_cast<int>(cachedcolnames.size()), 0);
				solverinterface->setRowNames(cachedrownames, 0, static_cast<int>(cachedrownames.size()), 0);
			
			}

		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLpSolver::updateRowsAndColsNames", __LINE__, __FILE__);
		}
	}
	

	void FMTLpSolver::writeLp(const std::string& location) const
		{
		try{
			matrixcache.synchronize(solverinterface);
			
			if (solvertype == Models::FMTSolverInterface::MOSEK)
			{
				#ifdef FMTWITHMOSEK
				std::vector<char*>rownames;
				std::vector<char*>colnames;
				OsiMskSolverInterface* msksolver = dynamic_cast<OsiMskSolverInterface*>(solverinterface.get());
				MSKtask_t task = msksolver->getMutableLpPtr();
				for (int colid = 0; colid < solverinterface->getNumCols();++colid)
					{
					char buffer[COIN_MESSAGE_HANDLER_MAX_BUFFER_SIZE];
					MSK_getvarname(task, colid, COIN_MESSAGE_HANDLER_MAX_BUFFER_SIZE, buffer);
					colnames.push_back(strdup(buffer));
					if (strlen(buffer)>100)
						{
						_exhandler->raise(Exception::FMTexc::FMTrangeerror, "Buffer size for colnames is bigger than allowed in CLPIO", "FMTLpSolver::writeLp", __LINE__, __FILE__);
						}
					}
				for (int rowid = 0; rowid < solverinterface->getNumRows(); ++rowid)
				{
					char buffer[COIN_MESSAGE_HANDLER_MAX_BUFFER_SIZE];
					MSK_getconname(task, rowid, COIN_MESSAGE_HANDLER_MAX_BUFFER_SIZE, buffer);
					rownames.push_back(strdup(buffer));
					if (strlen(buffer)>100)
					{
						_exhandler->raise(Exception::FMTexc::FMTrangeerror, "Buffer size for rownames is bigger than allowed in CLPIO", "FMTLpSolver::writeLp", __LINE__, __FILE__);
					}
				}
				rownames.push_back(strdup("objective"));
				const std::string locationwextension(location + ".lp");
				solverinterface->writeLpNative(locationwextension.c_str(), &rownames[0], &colnames[0], 1.0e-5, 10, 5, 1);
				for (char* value : colnames)
					{
					free(value);
					}
				for (char* value : rownames)
					{
					free(value);
					}
			#endif
			}else {
				solverinterface->writeLp(location.c_str());
			}
			
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTLpSolver::writeLp", __LINE__, __FILE__);
			}
		}
	void FMTLpSolver::writeMps(const std::string& location) const
		{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->writeMps(location.c_str());
		}catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTLpSolver::writeMps", __LINE__, __FILE__);
			}
		}

	void FMTLpSolver::branchAndBound()
		{
		try{
		matrixcache.synchronize(solverinterface);
		solverinterface->branchAndBound();
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLpSolver::branchAndBound", __LINE__, __FILE__);
		}
		}

	void FMTLpSolver::synchronize()
		{
		try {
			matrixcache.synchronize(solverinterface);
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTLpSolver::synchronize", __LINE__, __FILE__);
			}
		}

	bool FMTLpSolver::operator == (const FMTLpSolver& rhs) const
		{
		matrixcache.synchronize(solverinterface);
		return (solvertype == rhs.solvertype && solverinterface == rhs.solverinterface);
		}
	bool FMTLpSolver::operator != (const FMTLpSolver& rhs) const
		{
		return (!(*this==rhs));
		}

	void FMTLpSolver::enableMatrixCaching()
		{
		usecache = true;
		}
	void FMTLpSolver::disableMatrixCaching()
		{
		try {
			matrixcache.synchronize(solverinterface);
			usecache = false;
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTLpSolver::disableMatrixCaching", __LINE__, __FILE__);
			}
		}

	void FMTLpSolver::sortDeletedCache()
		{
		try{
		matrixcache.sortAndCleanDeleted();
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTLpSolver::sortDeletedCache", __LINE__, __FILE__);
			}
		}

	FMTLpSolver::FMTLpSolver(): solverinterface(), usecache(), solvertype()
		{

		}

	std::string FMTLpSolver::getCacheElements() const
		{
		return matrixcache.getRowsToSynchronize() + "\n" +
			matrixcache.getColsToSynchronize();
		}

	std::string FMTLpSolver::lowerNUpperToStr(const double& lower, const double& upper) const
		{
		std::string value;
		try{
		value += " lower bound of ";
		if (lower==-COIN_DBL_MAX)
			{
			value += "-inf";
		}else {
			value += std::to_string(lower);
			}
		value += " upper bound of ";
		if (upper == COIN_DBL_MAX)
		{
			value += "inf";
		}
		else {
			value += std::to_string(upper);
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLpSolver::lowerNUpperToStr", __LINE__, __FILE__);
		}
		return value;
		}

	int FMTLpSolver::getRow(int whichRow, double &rowLower, double &rowUpper,
		std::vector<int>&indices, std::vector<double>&elements) const
		{
		try{
		const int numberofnoncacherows = solverinterface->getNumRows();
		if (matrixcache.numberOfDeletedRows() >0 || matrixcache.numberOfDeletedCols() > 0)
			{
			matrixcache.synchronize(solverinterface);
		}else if (whichRow >= numberofnoncacherows &&
			whichRow <(numberofnoncacherows + matrixcache.numberNewRows()))
			{
			return matrixcache.getRow(whichRow, rowLower, rowUpper, indices, elements);
			}
		if (whichRow<solverinterface->getNumRows())
			{
			rowLower = *(solverinterface->getRowLower()+ whichRow);
			rowUpper = *(solverinterface->getRowUpper() + whichRow);
			const CoinPackedMatrix* rowpacked = solverinterface->getMatrixByRow();
			const int vectorsize = rowpacked->getVectorSize(whichRow);
			const int* matrixindicies = rowpacked->getIndices();
			const int* vectorstarts = rowpacked->getVectorStarts();
			const double* matrixelements = rowpacked->getElements();
			indices.reserve(vectorsize);
			elements.reserve(vectorsize);
			for (int index = *(vectorstarts + whichRow); index < (*(vectorstarts + whichRow) + vectorsize); ++index)
				{
				indices.push_back(*(matrixindicies + index));
				elements.push_back(*(matrixelements + index));
				}
			return static_cast<int>(indices.size());
			}else {
					_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"for row id " + std::to_string(whichRow), "FMTLpSolver::getRow", __LINE__, __FILE__);
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTLpSolver::getRow", __LINE__, __FILE__);
			}
		return -1;
		}

	int FMTLpSolver::getCol(int whichCol, double &colLower, double &colUpper, double &objectiveValue,
		std::vector<int>& indices, std::vector<double>&elements) const
		{
		try{
		const int numberofnoncachecols = solverinterface-> getNumCols();
		const int numberofdeletedcols = matrixcache.numberOfDeletedCols();
		if (matrixcache.numberOfDeletedRows() > 0 || matrixcache.numberOfDeletedCols() > 0)
		{
			matrixcache.synchronize(solverinterface);
		}
		else if (whichCol >= numberofnoncachecols &&
			whichCol < (numberofnoncachecols + matrixcache.numberNewCols()))
		{
			return matrixcache.getCol(whichCol, colLower, colUpper, objectiveValue, indices, elements);
		}
		if (whichCol < solverinterface->getNumCols())
		{
			colLower = *(solverinterface->getColLower()+ whichCol);
			colUpper = *(solverinterface->getColUpper() + whichCol);
			objectiveValue = *(solverinterface->getObjCoefficients() + whichCol);
			const CoinPackedMatrix* colpacked = solverinterface->getMatrixByCol();
			const int vectorsize = colpacked->getVectorSize(whichCol);
			const int* matrixindicies = colpacked->getIndices();
			const int* vectorstarts = colpacked->getVectorStarts();
			const double* matrixelements = colpacked->getElements();
			indices.reserve(vectorsize);
			elements.reserve(vectorsize);
			for (int index = *(vectorstarts + whichCol); index < (*(vectorstarts + whichCol) + vectorsize); ++index)
			{
				indices.push_back(*(matrixindicies + index));
				elements.push_back(*(matrixelements + index));
			}
			return static_cast<int>(indices.size());
		}
		else {
			Exception::FMTDefaultExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed,
				"for column id " + std::to_string(whichCol), "FMTLpSolver::getCol", __LINE__, __FILE__);
		}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLpSolver::getCol", __LINE__, __FILE__);
		}
		return -1;
		}


		std::string FMTLpSolver::getMskErrorDesc(int error) const
		{
			std::string errordescription;
			char symname[MSK_MAX_STR_LEN];
			char desc[MSK_MAX_STR_LEN];
			MSK_getcodedesc(static_cast<MSKrescodee>(error), symname, desc);
			errordescription+=symname;
			errordescription+=" ";
			errordescription+=desc;
			return errordescription;
		}

		bool FMTLpSolver::SupportsMultiThreading() const
		{
			return (getSolverType() != FMTSolverInterface::GLPK);
		}

	std::vector<std::pair<std::string, std::string>>FMTLpSolver::strtoParams(const std::string& p_params)
		{
		std::vector<std::pair<std::string, std::string>>parameters;
		try {
			if (!p_params.empty())
			{
				std::stringstream ss(p_params);
				std::string line;
				while (std::getline(ss, line, '\n'))
				{
					const size_t SPACE = line.find(' ');
					const std::string PARAMETER_NAME = line.substr(0, SPACE);
					const std::string PARAMETER_VALUE = line.substr(SPACE + 1, line.size());
					parameters.push_back(std::pair<std::string, std::string>(PARAMETER_NAME, PARAMETER_VALUE));
				}

			}
			
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTLpSolver::strtoParams", __LINE__, __FILE__);
			}
		return parameters;
		}

}
BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTLpSolver)
#endif
