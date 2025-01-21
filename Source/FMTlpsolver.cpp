/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
#include "FMTlpsolver.h"
#include "OsiSolverInterface.hpp"

#ifdef FMTWITHMOSEK
	#include "mosek.h"
	#include "OsiMskSolverInterface.hpp"
#endif


#include "OsiClpSolverInterface.hpp"
#include "FMTdefaultexceptionhandler.h"
#include "FMTsolverlogger.h"
#include "FMTserializablematrix.h"
#include <sstream>


namespace Models
{


	bool FMTlpsolver::canupdatesource() const
	{
	return (solverinterface.use_count() == 2);
	}


	std::shared_ptr<OsiSolverInterface> FMTlpsolver::buildsolverinterface(const FMTsolverinterface& lsolvertype) const
	{
		std::shared_ptr<OsiSolverInterface>newsolverinterface;
		try {
			switch (lsolvertype)
			{
			case FMTsolverinterface::CLP:
				newsolverinterface = std::shared_ptr<OsiClpSolverInterface>(new OsiClpSolverInterface());
				break;
#ifdef  FMTWITHMOSEK
			case FMTsolverinterface::MOSEK:
				newsolverinterface = std::shared_ptr<OsiMskSolverInterface>(new OsiMskSolverInterface());
				break;
#endif
				/*case FMTsolverinterface::CPLEX:
					newsolverinterface = shared_ptr<OsiCpxSolverInterface>(new OsiCpxSolverInterface);
				break;
				case FMTsolverinterface::GUROBI:
					newsolverinterface = shared_ptr<OsiGrbSolverInterface>(new OsiGrbSolverInterface);
				break;*/
			default:
				newsolverinterface = std::shared_ptr<OsiClpSolverInterface>(new OsiClpSolverInterface());
				break;
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("Cannot build solver","FMTlpsolver::buildsolverinterface", __LINE__, __FILE__);
			}
		return newsolverinterface;
	}


	std::shared_ptr<OsiSolverInterface> FMTlpsolver::copysolverinterface(const std::shared_ptr<OsiSolverInterface>& solver_ptr,const FMTsolverinterface& lsolvertype) const
	{
		std::shared_ptr<OsiSolverInterface>newsolverinterface;
		try{
			if (solver_ptr)
				{
				newsolverinterface.reset(solver_ptr->clone(true));
				newsolverinterface->resolve();
				}
		/*switch (lsolvertype)
		{
		case FMTsolverinterface::CLP:
			newsolverinterface = std::shared_ptr<OsiClpSolverInterface>(new OsiClpSolverInterface(*dynamic_cast<OsiClpSolverInterface*>(solver_ptr.get())));
			break;
	#ifdef  FMTWITHMOSEK
			case FMTsolverinterface::MOSEK:
				newsolverinterface = std::shared_ptr<OsiMskSolverInterface>(new OsiMskSolverInterface(*dynamic_cast<OsiMskSolverInterface*>(solver_ptr.get())));
				newsolverinterface->resolve();
				break;
	#endif
			//case FMTsolverinterface::CPLEX:
				//newsolverinterface = shared_ptr<OsiCpxSolverInterface>(new OsiCpxSolverInterface(*dynamic_cast<OsiCpxSolverInterface*>(solver_ptr.get())));
			//break;
			//case FMTsolverinterface::GUROBI:
				//newsolverinterface = shared_ptr<OsiGrbSolverInterface>(new OsiGrbSolverInterface(*dynamic_cast<OsiGrbSolverInterface*>(solver_ptr.get())));
			//break;
		default:
			newsolverinterface = std::shared_ptr<OsiClpSolverInterface>(new OsiClpSolverInterface(*dynamic_cast<OsiClpSolverInterface*>(solver_ptr.get())));
			break;
		}*/
		}
		catch (...)
		{
			_exhandler->raisefromcatch("Cannot copy solver", +"FMTlpsolver::copysolverinterface", __LINE__, __FILE__);
		}
		return newsolverinterface;
	}

	FMTlpsolver::FMTlpsolver(const FMTlpsolver& rhs) :Core::FMTobject(rhs), solverinterface(), 
		matrixcache(rhs.matrixcache),solvertype(rhs.solvertype), usecache(rhs.usecache),
		m_ColdStartParameters(rhs.m_ColdStartParameters),m_WarmStartParameters(rhs.m_WarmStartParameters)
		{
		solverinterface = copysolverinterface(rhs.solverinterface, rhs.solvertype);
		//Fix because mosek resolve in the copysolver maybe return an non optimal solution 
		if(rhs.solverinterface->isProvenOptimal() && !solverinterface->isProvenOptimal())
		{
			this->resolve();
		}
		//passinmessagehandler(*_logger);
		}

	void FMTlpsolver::swap(FMTlpsolver& rhs)
	{
		matrixcache.swap(rhs.matrixcache);
		solvertype = rhs.solvertype;
		usecache = rhs.usecache;
		solverinterface.swap(rhs.solverinterface);
		m_ColdStartParameters.swap(rhs.m_ColdStartParameters);
		m_WarmStartParameters.swap(rhs.m_WarmStartParameters);
	}

	FMTlpsolver& FMTlpsolver::operator =(const FMTlpsolver& rhs)
		{
		if (this!=&rhs)
			{
			Core::FMTobject::operator = (rhs);
			matrixcache = rhs.matrixcache;
			usecache = rhs.usecache;
			solvertype = rhs.solvertype;
			solverinterface = copysolverinterface(rhs.solverinterface,rhs.solvertype);
			m_ColdStartParameters=rhs.m_ColdStartParameters;
			m_WarmStartParameters=rhs.m_WarmStartParameters;
			if(rhs.solverinterface->isProvenOptimal() && !solverinterface->isProvenOptimal())
			{
				this->resolve();
			}
			//passinmessagehandler(*_logger);
			}
		return *this;
		}
	FMTlpsolver::FMTlpsolver(FMTsolverinterface lsolvertype,
		const std::string& p_ColdStartParameters,
		const std::string& p_WarmStartParameters,
		const std::string& p_problemName):
		Core::FMTobject(),solverinterface(),matrixcache(), solvertype(lsolvertype), usecache(true),
		m_ColdStartParameters(strtoParams(p_ColdStartParameters)),
		m_WarmStartParameters(strtoParams(p_WarmStartParameters))
		{
		solverinterface = buildsolverinterface(lsolvertype);
		passinmessagehandler(*_logger);
		if (solvertype== FMTsolverinterface::MOSEK)//weird in debug...
			{
			OsiMskSolverInterface* mskSolver = dynamic_cast<OsiMskSolverInterface*>(solverinterface.get());
			MSKtask_t mskTask = mskSolver->getMutableLpPtr();
			MSK_puttaskname(mskTask, const_cast<char*>(p_problemName.c_str()));
		}else {
			solverinterface->setStrParam(OsiStrParam::OsiProbName, p_problemName);//do not work in debug with msk
			}
		}

	bool FMTlpsolver::resolve()
		{
		try {
			bool erroroccured = false;
			matrixcache.synchronize(solverinterface);
			if (solvertype == Models::FMTsolverinterface::CLP)//clp with dual simplex
			{
				OsiClpSolverInterface* clpsolver = dynamic_cast<OsiClpSolverInterface*>(solverinterface.get());
				ClpSimplex* splexmodel = clpsolver->getModelPtr();
				splexmodel->setPerturbation(-6);
				splexmodel->setSpecialOptions(64 | 128 | 1024 | 2048 | 4096 | 32768 | 262144 | 0x01000000);
				//splexmodel->tightenPrimalBounds();
				splexmodel->dual();
			}
#ifdef  FMTWITHMOSEK
			else if (solvertype == Models::FMTsolverinterface::MOSEK) //Mosek with interior point
			{
				OsiMskSolverInterface* msksolver = dynamic_cast<OsiMskSolverInterface*>(solverinterface.get());
				msksolver->freeCachedData();
				MSKtask_t task = msksolver->getMutableLpPtr();
				if (!m_WarmStartParameters.empty())
				{
					for (const std::pair<std::string, std::string>& PARAMETER : m_WarmStartParameters)
					{
						MSK_putparam(task, PARAMETER.first.c_str(), PARAMETER.second.c_str());
					}
				}else {
					MSK_putintparam(task, MSK_IPAR_OPTIMIZER, MSK_OPTIMIZER_INTPNT);
					MSK_putintparam(task, MSK_IPAR_INTPNT_BASIS, MSK_BI_IF_FEASIBLE);
					MSK_putintparam(task, MSK_IPAR_SIM_HOTSTART, MSK_SIM_HOTSTART_NONE);
					//MSK_putintparam(task, MSK_IPAR_INTPNT_HOTSTART, MSK_INTPNT_HOTSTART_NONE);
					MSK_putintparam(task, MSK_IPAR_PRESOLVE_USE, MSK_ON);
					MSK_putintparam(task, MSK_IPAR_INTPNT_STARTING_POINT, MSK_STARTING_POINT_CONSTANT);
					MSK_putintparam(task, MSK_IPAR_BI_CLEAN_OPTIMIZER, MSK_OPTIMIZER_PRIMAL_SIMPLEX);
					MSK_putintparam(task, MSK_IPAR_BI_MAX_ITERATIONS, 100000000);
					MSK_putdouparam(task, MSK_DPAR_INTPNT_TOL_PSAFE, 100.0);
					MSK_putdouparam(task, MSK_DPAR_INTPNT_TOL_PATH, 1.0e-2);
					MSK_putintparam(task, MSK_IPAR_LOG, 10);
					MSK_putintparam(task, MSK_IPAR_LOG_INTPNT, 4);
					}
				MSKrescodee error = MSK_optimize(task);
				if (error > 0)
					{
					_exhandler->raise(Exception::FMTexc::FMTmskerror,getmskerrordesc(error),"FMTlpsolver::resolve", __LINE__, __FILE__);
					//In case set to warning
					solverinterface->resolve();
					erroroccured = true;
					}

			}
#endif
			else {//default
				solverinterface->resolve();
			}
			if (erroroccured && !gotlicense())
			{
				_exhandler->raise(Exception::FMTexc::FMTmissinglicense,
					" Missing solver " + getsolvername() + " License ",
					"FMTlpsolver::resolve", __LINE__, __FILE__);
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlpsolver::resolve", __LINE__, __FILE__);
			}
		return solverinterface->isProvenOptimal();
		}

	void FMTlpsolver::setnumberofthreads(const size_t& nthread)
		{
		try {
			switch (solvertype)
				{
				case FMTsolverinterface::CLP:
				{
				const OsiClpSolverInterface* clpsolver = dynamic_cast<OsiClpSolverInterface*>(solverinterface.get());
				ClpSimplex* splexmodel = clpsolver->getModelPtr();
				splexmodel->setNumberThreads(static_cast<int>(nthread));
				break;
				}
				#ifdef  FMTWITHMOSEK
				case FMTsolverinterface::MOSEK:
					{
					OsiMskSolverInterface* msksolver = dynamic_cast<OsiMskSolverInterface*>(solverinterface.get());
					MSKtask_t task = msksolver->getMutableLpPtr();
					MSK_putintparam(task, MSK_IPAR_NUM_THREADS, static_cast<int>(std::max(size_t(1), nthread)));
					break;
					}
				#endif
				default:
					_exhandler->raise(Exception::FMTexc::FMTignore, "Cannot set number of threads used for "+getsolvername(),
						"FMTlpsolver::setnumberofthreads", __LINE__, __FILE__);
				break;
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlpsolver::setnumberofthreads", __LINE__, __FILE__);
			}
		}

	void FMTlpsolver::setMIPgaptolerance(const double& gap)
	{
		try {
			switch (solvertype)
			{
			#ifdef  FMTWITHMOSEK
			case FMTsolverinterface::MOSEK:
				{
					OsiMskSolverInterface* msksolver = dynamic_cast<OsiMskSolverInterface*>(solverinterface.get());
					MSKtask_t task = msksolver->getMutableLpPtr();
					MSK_putdouparam(task, MSK_DPAR_MIO_TOL_REL_GAP, gap);
					break;
				}
			#endif
			default:
				_exhandler->raise(Exception::FMTexc::FMTignore, "Cannot set gap tolerance for " + getsolvername(),
					"FMTlpsolver::setMIPgaptolerance", __LINE__, __FILE__);
				break;
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpsolver::setMIPgaptolerance", __LINE__, __FILE__);
		}
	}

	void FMTlpsolver::MIPparameters()
		{
			try {
				switch (solvertype)
				{
				#ifdef  FMTWITHMOSEK
				case FMTsolverinterface::MOSEK:
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
					_exhandler->raise(Exception::FMTexc::FMTignore, "Cannot set gap feasible pump level for " + getsolvername(),
						"FMTlpsolver::MIPparameters", __LINE__, __FILE__);
					break;
				}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTlpsolver::setMIPgaptolerance", __LINE__, __FILE__);
			}
		}


	void FMTlpsolver::setoptimizerMAXtime(const double& time)
		{
			try {
				switch (solvertype)
				{
				#ifdef  FMTWITHMOSEK
				case FMTsolverinterface::MOSEK:
					{
						OsiMskSolverInterface* msksolver = dynamic_cast<OsiMskSolverInterface*>(solverinterface.get());
						MSKtask_t task = msksolver->getMutableLpPtr();
						MSK_putdouparam(task, MSK_DPAR_OPTIMIZER_MAX_TIME, time);
						break;
					}
				#endif
				default:
					_exhandler->raise(Exception::FMTexc::FMTignore, "Cannot set max time for " + getsolvername(),
						"FMTlpsolver::setoptimizerMAXtime", __LINE__, __FILE__);
					break;
				}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTlpsolver::setMIPgaptolerance", __LINE__, __FILE__);
			}
		}

	bool FMTlpsolver::initialsolve()
		{
		try{
		matrixcache.synchronize(solverinterface);
		bool erroroccured = false;
		switch (solvertype)
		{
		case FMTsolverinterface::CLP:
		{
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
					 5 - for presolve
								  1 - switch off dual stuff
					 6 - extra switches
				 */
			OsiClpSolverInterface* clpsolver = dynamic_cast<OsiClpSolverInterface*>(solverinterface.get());
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
			clpsolver->initialSolve();
			//ClpSolve simplexoptions;
			//simplexoptions.setSolveType(ClpSolve::usePrimal); //Or sprint?
			//simplexoptions.setSolveType(ClpSolve::usePrimalorSprint);
			//simplexoptions.setPresolveType(ClpSolve::presolveOn);
			//clpsolver->setSolveOptions(simplexoptions);
			//clpsolver->resolve();
		}
		break;
#ifdef FMTWITHMOSEK
		case FMTsolverinterface::MOSEK:
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
			}else {
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
			MSKrescodee error = MSK_optimize(task);
			if (error > 0)
				{
					_exhandler->raise(Exception::FMTexc::FMTmskerror,getmskerrordesc(error),"FMTlpsolver::initialsolve", __LINE__, __FILE__);
					//Just to make sure the class is updated...
					solverinterface->initialSolve();
					erroroccured = true;
				}
			
		}
		break;
		#endif
		/*case FMTsolverinterface::CPLEX:
			solverinterface = unique_ptr<OsiCpxSolverInterface>(new OsiCpxSolverInterface);
		break;
		case FMTsolverinterface::GUROBI:
			solverinterface = unique_ptr<OsiGrbSolverInterface>(new OsiGrbSolverInterface);
		break;*/
		default:
		{
			solverinterface->initialSolve();
		}
		break;
		}
		if (erroroccured && !gotlicense())
			{
			_exhandler->raise(Exception::FMTexc::FMTmissinglicense,
					" Missing solver " + getsolvername() + " License ",
					"FMTlpsolver::initialsolve", __LINE__, __FILE__);
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", +"FMTlpsolver::initialsolve", __LINE__, __FILE__);
		}
		return solverinterface->isProvenOptimal();
		}

	void FMTlpsolver::passinmessagehandler(Logging::FMTlogger& logger)
		{
		try{
		solverinterface->passInMessageHandler(dynamic_cast<CoinMessageHandler*>(logger.getsolverlogger()));
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlpsolver::passinmessagehandler", __LINE__, __FILE__);
			}
		}

	bool FMTlpsolver::gotlicense() const
		{
		bool licensestatus = false;
		try{
		switch (solvertype)
		{
		#ifdef FMTWITHMOSEK
				case FMTsolverinterface::MOSEK:
				{
					const OsiMskSolverInterface* msksolver = dynamic_cast<OsiMskSolverInterface*>(solverinterface.get());
					licensestatus = !msksolver->isLicenseError();

				}
		break;
		#endif
		/*case FMTsolverinterface::CPLEX:
			solverinterface = unique_ptr<OsiCpxSolverInterface>(new OsiCpxSolverInterface);
		break;
		case FMTsolverinterface::GUROBI:
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
			_exhandler->raisefromcatch("", +"FMTlpsolver::gotlicense", __LINE__, __FILE__);
		}
		return licensestatus;
		}

	std::string FMTlpsolver::getsolvername() const
		{
		std::string name;
		try {
			switch (solvertype)
			{
			case FMTsolverinterface::CLP:
				name = "CLP";
				break;
#ifdef FMTWITHMOSEK
			case FMTsolverinterface::MOSEK:
				name = "MOSEK";
				break;
#endif

				/*case FMTsolverinterface::CPLEX:
					name = "CPLEX";
				break;
				case FMTsolverinterface::GUROBI:
					name = "GUROBI";
				break;*/
			default:
			{
				name = "CLP";
			}
			break;
			}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", +"FMTlpsolver::getsolvername", __LINE__, __FILE__);
			}
		return name;
		}

	int FMTlpsolver::getNumCols() const
		{
		return solverinterface->getNumCols() + matrixcache.numbernewCols() - matrixcache.numberofdeletedCols();
		}

	int FMTlpsolver::getNumRows() const
	{
		return solverinterface->getNumRows() + matrixcache.numbernewRows() - matrixcache.numberofdeletedRows();
	}

	const double* FMTlpsolver::getColLower() const
	{
		matrixcache.synchronize(solverinterface);
		return solverinterface->getColLower();
	}

	const double* FMTlpsolver::getColUpper() const
	{
		matrixcache.synchronize(solverinterface);
		return solverinterface->getColUpper();
	}

	const double* FMTlpsolver::getColSolution() const
	{
		//matrixcache.synchronize(solverinterface);
		return solverinterface->getColSolution();
	}

	const double* FMTlpsolver::getRowPrice() const
	{
		//matrixcache.synchronize(solverinterface);
		return solverinterface->getRowPrice();
	}

	const double* FMTlpsolver::getRowActivity() const
	{
		//matrixcache.synchronize(solverinterface);
		return solverinterface->getRowActivity();
	}

	const double* FMTlpsolver::getRowUpper() const
	{
		matrixcache.synchronize(solverinterface);
		return solverinterface->getRowUpper();
	}

	const double* FMTlpsolver::getRowLower() const
	{
		matrixcache.synchronize(solverinterface);
		return solverinterface->getRowLower();
	}

	double FMTlpsolver::getObjSense() const
	{
		matrixcache.synchronize(solverinterface);
		return solverinterface->getObjSense();
	}

	const double* FMTlpsolver::getObjCoefficients() const
	{
		matrixcache.synchronize(solverinterface);
		return solverinterface->getObjCoefficients();
	}

	double FMTlpsolver::getObjValue() const
		{
		return solverinterface->getObjValue();
		}


	void FMTlpsolver::addRow(int numberInRow, const int * columns, const double * elements, double rowLower, double rowUpper)
	{
		if (usecache)
		{
			matrixcache.addRow(numberInRow, columns, elements, rowLower, rowUpper);
		}
		else {
			solverinterface->addRow(numberInRow, columns, elements, rowLower, rowUpper);
		}

	}

	void FMTlpsolver::addCol(int numberInColumn, const int * rows, const double * elements, double columnLower,
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

	void FMTlpsolver::addRows(const int numrows, const int* rowStarts, const int* columns,
		const double* elements, const double* rowlb, const double* rowub)
	{
		matrixcache.synchronize(solverinterface);
		solverinterface->addRows(numrows, rowStarts, columns, elements, rowlb, rowub);
	}

	void FMTlpsolver::addCols(const int numcols, const int* columnStarts, const int* rows,
		const double* elements, const double* collb, const double* colub, const double* obj)
	{
		matrixcache.synchronize(solverinterface);
		solverinterface->addCols(numcols, columnStarts, rows, elements, collb, colub, obj);
	}
	// DocString: FMTlpsolver::isProvenOptimal
	/**
	Returns true if the program is optimal but first will synchronize the matrix with the matrix cache.
	*/
	bool FMTlpsolver::isProvenOptimal() const
	{
		return solverinterface->isProvenOptimal();
	}

	void FMTlpsolver::setColSolution(const double* newsolution)
		{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->setColSolution(newsolution);
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpsolver::setColSolution", __LINE__, __FILE__);
		}
		}

	void FMTlpsolver::setRowPrice(const double* rowprice)
	{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->setRowPrice(rowprice);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpsolver::setRowPrice", __LINE__, __FILE__);
		}
	}


	void FMTlpsolver::setColSetBounds(const int* indexFirst, const int* indexLast, const double* boundlist)
		{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->setColSetBounds(indexFirst, indexLast, boundlist);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlpsolver::setColSetBounds", __LINE__, __FILE__);
			}
		}

	void FMTlpsolver::setRowSetBounds(const int* indexFirst, const int* indexLast, const double* boundlist)
		{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->setRowSetBounds(indexFirst, indexLast, boundlist);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlpsolver::setRowSetBounds", __LINE__, __FILE__);
			}
		}

	void FMTlpsolver::deleteRow(const int& rowindex)
		{
		try{
			matrixcache.deleteRow(rowindex);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpsolver::deleteRow", __LINE__, __FILE__);
		}
		}
	void FMTlpsolver::deleteCol(const int& colindex)
		{
		try {
			matrixcache.deleteCol(colindex);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpsolver::deleteCol", __LINE__, __FILE__);
		}
			
		}

	int FMTlpsolver::getiterationcount() const
		{
		int iterations = 0;
		try{
		switch (solvertype)
		{
		case FMTsolverinterface::CLP:
			{
			const OsiClpSolverInterface* clpsolver = dynamic_cast<OsiClpSolverInterface*>(solverinterface.get());
			const ClpSimplex* splexmodel = clpsolver->getModelPtr();
			iterations = splexmodel->numberIterations();
			}
		break;
		#ifdef FMTWITHMOSEK
		case FMTsolverinterface::MOSEK:
			{
			const OsiMskSolverInterface* msksolver = dynamic_cast<OsiMskSolverInterface*>(solverinterface.get());
			const MSKtask_t task = msksolver->getMutableLpPtr();
			MSK_getintinf(task, MSK_IINF_INTPNT_ITER, &iterations);
			}
		break;
		#endif
		/*case FMTsolverinterface::CPLEX:
			solverinterface = unique_ptr<OsiCpxSolverInterface>(new OsiCpxSolverInterface);
		break;
		case FMTsolverinterface::GUROBI:
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
			_exhandler->raisefromcatch("", "FMTlpsolver::getiterationcount", __LINE__, __FILE__);
		}
		return iterations;
		}

	void FMTlpsolver::setsolvertype(FMTsolverinterface& lsolvertype) const
		{
		lsolvertype = solvertype;
		}

	void FMTlpsolver::passinsolver(const FMTlpsolver& solver)
		{
		try{
		Core::FMTobject::operator=(solver);
		usecache = solver.usecache;
		matrixcache = solver.matrixcache;
		solvertype = solver.solvertype;
		solverinterface = solver.solverinterface;
		passinmessagehandler(*_logger);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpsolver::passinsolver", __LINE__, __FILE__);
		}
		}

	void FMTlpsolver::unmarkHotStart()
	{
		try {
			solverinterface->unmarkHotStart();
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpsolver::unmarkHotStart", __LINE__, __FILE__);
		}
	}

	void FMTlpsolver::clearrowcache()
		{
		try{
		#ifdef  FMTWITHMOSEK
			matrixcache.synchronize(solverinterface);
			if (solvertype == Models::FMTsolverinterface::MOSEK)
				{
				OsiMskSolverInterface* msksolver = dynamic_cast<OsiMskSolverInterface*>(solverinterface.get());
				msksolver->freeCachedRowRim();
				}
		#endif
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpsolver::clearrowcache", __LINE__, __FILE__);
		}
		}

	void FMTlpsolver::setInteger(const int& colindex)
		{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->setInteger(colindex);
		}catch (...)
			{
			_exhandler->raisefromcatch("at column index " + std::to_string(colindex), "FMTlpsolver::setInteger", __LINE__, __FILE__);
			}
		}

	bool FMTlpsolver::stockresolve()
		{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->resolve();
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpsolver::stockresolve", __LINE__, __FILE__);
		}
		return solverinterface->isProvenOptimal();
		}

	void FMTlpsolver::setInteger(const int* indices, int len)
		{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->setInteger(indices, len);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlpsolver::setInteger", __LINE__, __FILE__);
			}

		}

	void FMTlpsolver::setObjective(const double* objectivevalues)
		{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->setObjective(objectivevalues);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlpsolver::setObjective", __LINE__, __FILE__);
			}
		}
	void FMTlpsolver::setObjSense(const double& newsense)
		{
		try {
			solverinterface->setObjSense(newsense);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlpsolver::setObjSense", __LINE__, __FILE__);
			}
		}

	void FMTlpsolver::deleteRows(int numberofrows, const int* rowindexes)
		{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->deleteRows(numberofrows, rowindexes);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlpsolver::deleteRows", __LINE__, __FILE__);
			}

		}

	void FMTlpsolver::deleteCols(int numberofcols, const int* colindexes)
		{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->deleteCols(numberofcols, colindexes);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlpsolver::deleteCols", __LINE__, __FILE__);
			}
		}

	const CoinPackedMatrix* FMTlpsolver::getMatrixByRow() const
		{
		try {
			matrixcache.synchronize(solverinterface);
			return solverinterface->getMatrixByRow();
		}catch (...)
			{
				_exhandler->raisefromcatch("", "FMTlpsolver::getMatrixByRow", __LINE__, __FILE__);
			}
		return nullptr;
		}
	const CoinPackedMatrix* FMTlpsolver::getMatrixByCol() const
		{
		try {
			matrixcache.synchronize(solverinterface);
			return solverinterface->getMatrixByCol();
		}catch (...)
			{
				_exhandler->raisefromcatch("", "FMTlpsolver::getMatrixByCol", __LINE__, __FILE__);
			}
		return nullptr;
		}

	void FMTlpsolver::passinlogger(const std::unique_ptr<Logging::FMTlogger>& logger)
	{
		try {
			Core::FMTobject::passinlogger(logger);
			passinmessagehandler(*getLogger());
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpsolver::passinlogger", __LINE__, __FILE__);
		}
	}

	void FMTlpsolver::setcolname(const std::string& name, const int& columnid) const
	{
		try {
			matrixcache.setcolname(name, columnid);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpsolver::setcolname", __LINE__, __FILE__);
		}
	}
	
	void FMTlpsolver::setrowname(const std::string& name, const int& rowid) const
	{
		try {
			matrixcache.setrowname(name, rowid);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpsolver::setrowname", __LINE__, __FILE__);
		}
	}

	void FMTlpsolver::updaterowsandcolsnames(bool shortformat)
	{
		try {
			matrixcache.synchronize(solverinterface);
			matrixcache.formatallnames(shortformat);
			std::vector<std::string>& cachedrownames = matrixcache.getrownames();
			std::vector<std::string>& cachedcolnames = matrixcache.getcolumnnames();
			if(cachedcolnames.size() != static_cast<size_t>(getNumCols()) || cachedrownames.size() != static_cast<size_t>(getNumRows()))
			{
				_exhandler->raise(Exception::FMTexc::FMTrangeerror, 
								"NumCols or NumRows size is different of the number of names given", "FMTlpsolver::updaterowsandcolsnames", __LINE__, __FILE__);		
			}
			if (solvertype == Models::FMTsolverinterface::MOSEK)
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
			_exhandler->raisefromcatch("", "FMTlpsolver::updaterowsandcolsnames", __LINE__, __FILE__);
		}
	}
	/*
	void FMTlpsolver::updatematrixnaming(const std::unordered_map<int, std::string>& colsnames,
		const std::unordered_map<int, std::string>& rownames)
	{
		try {
			matrixcache.synchronize(solverinterface);
			for (int colid = 0 ; colid < solverinterface->getNumCols();++colid)
			{
				std::string name = "C"  + std::to_string(colid);
				if (colsnames.find(colid)!= colsnames.end())
					{
					name = colsnames.at(colid);
					}
			if (solvertype== Models::FMTsolverinterface::MOSEK)
				{
				OsiMskSolverInterface* msksolver = dynamic_cast<OsiMskSolverInterface*>(solverinterface.get());
				MSKtask_t task = msksolver->getMutableLpPtr();
				MSK_putvarname(task, colid, name.c_str());
			}else {
				solverinterface->setColName(colid, name);
				}
			
			}
			for (int rowid = 0; rowid < solverinterface->getNumRows(); ++rowid)
			{
				std::string name = "R" + std::to_string(rowid);
				if (rownames.find(rowid) != rownames.end())
					{
					name = rownames.at(rowid);
					}
				if (solvertype == Models::FMTsolverinterface::MOSEK)
				{
					OsiMskSolverInterface* msksolver = dynamic_cast<OsiMskSolverInterface*>(solverinterface.get());
					MSKtask_t task = msksolver->getMutableLpPtr();
					MSK_putconname(task, rowid, name.c_str());
				}
				else {
					solverinterface->setRowName(rowid, name);
				}
				
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpsolver::updatematrixnaming", __LINE__, __FILE__);
		}
	}
	*/

	void FMTlpsolver::writeLP(const std::string& location) const
		{
		try{
			matrixcache.synchronize(solverinterface);
			
			if (solvertype == Models::FMTsolverinterface::MOSEK)
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
						_exhandler->raise(Exception::FMTexc::FMTrangeerror, "Buffer size for colnames is bigger than allowed in CLPIO", "FMTlpsolver::writeLP", __LINE__, __FILE__);
						}
					}
				for (int rowid = 0; rowid < solverinterface->getNumRows(); ++rowid)
				{
					char buffer[COIN_MESSAGE_HANDLER_MAX_BUFFER_SIZE];
					MSK_getconname(task, rowid, COIN_MESSAGE_HANDLER_MAX_BUFFER_SIZE, buffer);
					rownames.push_back(strdup(buffer));
					if (strlen(buffer)>100)
					{
						_exhandler->raise(Exception::FMTexc::FMTrangeerror, "Buffer size for rownames is bigger than allowed in CLPIO", "FMTlpsolver::writeLP", __LINE__, __FILE__);
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
			_exhandler->raisefromcatch("", "FMTlpsolver::writeLP", __LINE__, __FILE__);
			}
		}
	void FMTlpsolver::writeMPS(const std::string& location) const
		{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->writeMps(location.c_str());
		}catch (...)
			{
				_exhandler->raisefromcatch("", "FMTlpsolver::writeMPS", __LINE__, __FILE__);
			}
		}

	void FMTlpsolver::branchAndBound()
		{
		try{
		matrixcache.synchronize(solverinterface);
		solverinterface->branchAndBound();
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpsolver::branchAndBound", __LINE__, __FILE__);
		}
		}

	void FMTlpsolver::synchronize()
		{
		try {
			matrixcache.synchronize(solverinterface);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlpsolver::synchronize", __LINE__, __FILE__);
			}
		}

	bool FMTlpsolver::operator == (const FMTlpsolver& rhs) const
		{
		matrixcache.synchronize(solverinterface);
		return (solvertype == rhs.solvertype && solverinterface == rhs.solverinterface);
		}
	bool FMTlpsolver::operator != (const FMTlpsolver& rhs) const
		{
		return (!(*this==rhs));
		}

	void FMTlpsolver::enablematrixcaching()
		{
		usecache = true;
		}
	void FMTlpsolver::disablematrixcaching()
		{
		try {
			matrixcache.synchronize(solverinterface);
			usecache = false;
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlpsolver::disablematrixcaching", __LINE__, __FILE__);
			}
		}

	void FMTlpsolver::sortdeletedcache()
		{
		try{
		matrixcache.sortandcleandeleted();
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlpsolver::sortdeletedcache", __LINE__, __FILE__);
			}
		}

	FMTlpsolver::FMTlpsolver(): solverinterface(), usecache(), solvertype()
		{

		}

	std::string FMTlpsolver::getcacheelements() const
		{
		return matrixcache.getrowstosynchronize() + "\n" +
			matrixcache.getcolstosynchronize();
		}

	std::string FMTlpsolver::lowernuppertostr(const double& lower, const double& upper) const
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
			_exhandler->raisefromcatch("", "FMTlpsolver::lowernuppertostr", __LINE__, __FILE__);
		}
		return value;
		}

	int FMTlpsolver::getrow(int whichRow, double &rowLower, double &rowUpper,
		std::vector<int>&indices, std::vector<double>&elements) const
		{
		try{
		const int numberofnoncacherows = solverinterface->getNumRows();
		if (matrixcache.numberofdeletedRows() >0 || matrixcache.numberofdeletedCols() > 0)
			{
			matrixcache.synchronize(solverinterface);
		}else if (whichRow >= numberofnoncacherows &&
			whichRow <(numberofnoncacherows + matrixcache.numbernewRows()))
			{
			return matrixcache.getrow(whichRow, rowLower, rowUpper, indices, elements);
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
					"for row id " + std::to_string(whichRow), "FMTlpsolver::getrow", __LINE__, __FILE__);
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlpsolver::getrow", __LINE__, __FILE__);
			}
		return -1;
		}

	int FMTlpsolver::getcol(int whichCol, double &colLower, double &colUpper, double &objectiveValue,
		std::vector<int>& indices, std::vector<double>&elements) const
		{
		try{
		const int numberofnoncachecols = solverinterface-> getNumCols();
		const int numberofdeletedcols = matrixcache.numberofdeletedCols();
		if (matrixcache.numberofdeletedRows() > 0 || matrixcache.numberofdeletedCols() > 0)
		{
			matrixcache.synchronize(solverinterface);
		}
		else if (whichCol >= numberofnoncachecols &&
			whichCol < (numberofnoncachecols + matrixcache.numbernewCols()))
		{
			return matrixcache.getcol(whichCol, colLower, colUpper, objectiveValue, indices, elements);
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
			Exception::FMTdefaultexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed,
				"for column id " + std::to_string(whichCol), "FMTlpsolver::getcol", __LINE__, __FILE__);
		}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpsolver::getcol", __LINE__, __FILE__);
		}
		return -1;
		}

	#ifdef FMTWITHMOSEK
		std::string FMTlpsolver::getmskerrordesc(int error) const
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
	#endif

	std::vector<std::pair<std::string, std::string>>FMTlpsolver::strtoParams(const std::string& p_params)
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
			_exhandler->raisefromcatch("", "FMTlpsolver::strtoParams", __LINE__, __FILE__);
			}
		return parameters;
		}

}
BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTlpsolver)
#endif
