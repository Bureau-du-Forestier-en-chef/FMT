/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
#include "FMTlpsolver.h"

#ifdef FMTWITHMOSEK
	#include "OsiMskSolverInterface.hpp"
	#include "mosek.h"
#endif

#include "OsiClpSolverInterface.hpp"

namespace Models
{
	std::shared_ptr<OsiSolverInterface> FMTlpsolver::buildsolverinterface(const FMTsolverinterface& lsolvertype) const
	{
		std::shared_ptr<OsiSolverInterface>newsolverinterface;
		switch (lsolvertype)
		{
		case FMTsolverinterface::CLP:
			newsolverinterface = std::shared_ptr<OsiClpSolverInterface>(new OsiClpSolverInterface);
			break;
#ifdef  FMTWITHMOSEK
		case FMTsolverinterface::MOSEK:
			newsolverinterface = std::shared_ptr<OsiMskSolverInterface>(new OsiMskSolverInterface);
			break;
#endif
			/*case FMTsolverinterface::CPLEX:
				newsolverinterface = shared_ptr<OsiCpxSolverInterface>(new OsiCpxSolverInterface);
			break;
			case FMTsolverinterface::GUROBI:
				newsolverinterface = shared_ptr<OsiGrbSolverInterface>(new OsiGrbSolverInterface);
			break;*/
		default:
			newsolverinterface = std::shared_ptr<OsiClpSolverInterface>(new OsiClpSolverInterface);
			break;
		}
		return newsolverinterface;
	}


	std::shared_ptr<OsiSolverInterface> FMTlpsolver::copysolverinterface(const std::shared_ptr<OsiSolverInterface>& solver_ptr,const FMTsolverinterface& lsolvertype) const
	{
		std::shared_ptr<OsiSolverInterface>newsolverinterface;
		switch (lsolvertype)
		{
		case FMTsolverinterface::CLP:
			newsolverinterface = std::shared_ptr<OsiClpSolverInterface>(new OsiClpSolverInterface(*dynamic_cast<OsiClpSolverInterface*>(solver_ptr.get())));
			break;
	#ifdef  FMTWITHMOSEK
			case FMTsolverinterface::MOSEK:
				newsolverinterface = std::shared_ptr<OsiMskSolverInterface>(new OsiMskSolverInterface(*dynamic_cast<OsiMskSolverInterface*>(solver_ptr.get())));
				break;
	#endif
			/*case FMTsolverinterface::CPLEX:
				newsolverinterface = shared_ptr<OsiCpxSolverInterface>(new OsiCpxSolverInterface(*dynamic_cast<OsiCpxSolverInterface*>(solver_ptr.get())));
			break;
			case FMTsolverinterface::GUROBI:
				newsolverinterface = shared_ptr<OsiGrbSolverInterface>(new OsiGrbSolverInterface(*dynamic_cast<OsiGrbSolverInterface*>(solver_ptr.get())));
			break;*/
		default:
			newsolverinterface = std::shared_ptr<OsiClpSolverInterface>(new OsiClpSolverInterface(*dynamic_cast<OsiClpSolverInterface*>(solver_ptr.get())));
			break;
		}
		return newsolverinterface;
	}

	FMTlpsolver::FMTlpsolver(const FMTlpsolver& rhs) : solverinterface(), usecache(rhs.usecache),matrixcache(rhs.matrixcache),solvertype(rhs.solvertype)
		{
		solverinterface = copysolverinterface(rhs.solverinterface, rhs.solvertype);
		solverinterface->passInMessageHandler(rhs.solverinterface->messageHandler());
		}
	FMTlpsolver& FMTlpsolver::operator =(const FMTlpsolver& rhs)
		{
		if (this!=&rhs)
			{
			matrixcache = rhs.matrixcache;
			usecache = rhs.usecache;
			solvertype = rhs.solvertype;
			solverinterface = copysolverinterface(rhs.solverinterface,rhs.solvertype);
			solverinterface->passInMessageHandler(rhs.solverinterface->messageHandler());
			}
		return *this;
		}
	FMTlpsolver::FMTlpsolver(FMTsolverinterface lsolvertype, Logging::FMTlogger& logger):solverinterface(), usecache(true),matrixcache(), solvertype(lsolvertype)
		{
		solverinterface = buildsolverinterface(lsolvertype);
		this->passinmessagehandler(logger);
		}

	bool FMTlpsolver::resolve()
		{
		matrixcache.synchronize(solverinterface);
		if (solvertype == Models::FMTsolverinterface::CLP)//clp with dual simplex
		{
			OsiClpSolverInterface* clpsolver = dynamic_cast<OsiClpSolverInterface*>(solverinterface.get());
			ClpSimplex* splexmodel = clpsolver->getModelPtr();
			splexmodel->setPerturbation(-6);
			splexmodel->setSpecialOptions(64 | 128 | 1024 | 2048 | 4096 | 32768 | 262144 | 0x01000000);
			splexmodel->tightenPrimalBounds();
			splexmodel->dual();
		}
		#ifdef  FMTWITHMOSEK
		else if (solvertype == Models::FMTsolverinterface::MOSEK) //Mosek with interior point
		{
			OsiMskSolverInterface* msksolver = dynamic_cast<OsiMskSolverInterface*>(solverinterface.get());
			msksolver->freeCachedData();
			MSKtask_t task = msksolver->getMutableLpPtr();
			MSK_putintparam(task, MSK_IPAR_OPTIMIZER, MSK_OPTIMIZER_INTPNT);
			MSK_putintparam(task, MSK_IPAR_INTPNT_BASIS, MSK_BI_IF_FEASIBLE);
			MSK_putintparam(task, MSK_IPAR_SIM_HOTSTART, MSK_SIM_HOTSTART_NONE);
			MSK_putintparam(task, MSK_IPAR_PRESOLVE_USE, MSK_ON);
			MSK_putintparam(task, MSK_IPAR_INTPNT_STARTING_POINT, MSK_STARTING_POINT_CONSTANT);
			MSK_putintparam(task, MSK_IPAR_BI_CLEAN_OPTIMIZER, MSK_OPTIMIZER_PRIMAL_SIMPLEX);
			MSK_putdouparam(task, MSK_DPAR_INTPNT_TOL_PSAFE, 100.0);
			MSK_putdouparam(task, MSK_DPAR_INTPNT_TOL_PATH, 1.0e-2);
			MSK_putintparam(task, MSK_IPAR_LOG, 10);
			MSK_putintparam(task, MSK_IPAR_LOG_INTPNT, 4);
			MSKrescodee error = MSK_optimize(task);
			
		}
#endif
		else {//default
			solverinterface->resolve();
			}
		return solverinterface->isProvenOptimal();
		}

	bool FMTlpsolver::initialsolve()
		{
		matrixcache.synchronize(solverinterface);
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
			MSK_putintparam(task, MSK_IPAR_OPTIMIZER, MSK_OPTIMIZER_INTPNT);
			MSK_putintparam(task, MSK_IPAR_INTPNT_BASIS, MSK_BI_IF_FEASIBLE);
			MSK_putintparam(task, MSK_IPAR_SIM_HOTSTART, MSK_SIM_HOTSTART_NONE);
			MSK_putintparam(task, MSK_IPAR_PRESOLVE_USE, MSK_ON);
			MSK_putintparam(task, MSK_IPAR_INTPNT_STARTING_POINT, MSK_STARTING_POINT_CONSTANT);
			MSK_putintparam(task, MSK_IPAR_BI_CLEAN_OPTIMIZER, MSK_OPTIMIZER_PRIMAL_SIMPLEX);
			MSK_putdouparam(task, MSK_DPAR_INTPNT_TOL_PSAFE, 100.0);
			MSK_putdouparam(task, MSK_DPAR_INTPNT_TOL_PATH, 1.0e-2);
			MSKrescodee error = MSK_optimize(task);
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
		return solverinterface->isProvenOptimal();
		}

	void FMTlpsolver::passinmessagehandler(Logging::FMTlogger& logger)
		{
		solverinterface->passInMessageHandler(&logger);
		}

	bool FMTlpsolver::gotlicense() const
		{
		bool licensestatus = false;
		switch (solvertype)
		{
		#ifdef FMTWITHMOSEK
				case FMTsolverinterface::MOSEK:
				{
					const OsiMskSolverInterface* msksolver = dynamic_cast<OsiMskSolverInterface*>(solverinterface.get());
					const MSKtask_t task = msksolver->getMutableLpPtr();
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
		return licensestatus;
		}

	std::string FMTlpsolver::getsolvername() const
		{
		std::string name;
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
		return name;
		}

	double FMTlpsolver::getObjValue() const
		{
		return solverinterface->getObjValue();
		}

	void FMTlpsolver::setColSolution(const double* newsolution)
		{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->setColSolution(newsolution);
		}catch (...)
		{
			Exception::FMTexceptionhandler().raisefromcatch("", "FMTlpsolver::setColSolution", __LINE__, __FILE__);
		}
		}


	void FMTlpsolver::setColSetBounds(const int* indexFirst, const int* indexLast, const double* boundlist,int firstfutur)
		{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->setColSetBounds(indexFirst, indexLast, boundlist);
			if (solvertype == FMTsolverinterface::CLP)
				{
				if (firstfutur < 0)
					{
						Exception::FMTexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed,
							"Need first futur developement index for CLP interface",
							"FMTlpsolver::setColSetBounds", __LINE__, __FILE__);
					}
				for (int index = firstfutur; index < solverinterface->getNumCols();++index)
					{
					solverinterface->setColBounds(index,0.0,COIN_DBL_MAX);
					}
				}
		}catch (...)
			{
			Exception::FMTexceptionhandler().raisefromcatch("", "FMTlpsolver::setColSetBounds", __LINE__, __FILE__);
			}
		}

	void FMTlpsolver::setRowSetBounds(const int* indexFirst, const int* indexLast, const double* boundlist)
		{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->setRowSetBounds(indexFirst, indexLast, boundlist);
		}catch (...)
			{
			Exception::FMTexceptionhandler().raisefromcatch("", "FMTlpsolver::setRowSetBounds", __LINE__, __FILE__);
			}
		}

	void FMTlpsolver::deleteRow(const int& rowindex)
		{
		//if (usecache)
		//	{
			matrixcache.deleteRow(rowindex);
		//}else {
		//	solverinterface->deleteRows(1, &rowindex);
		//	}
		}
	void FMTlpsolver::deleteCol(const int& colindex)
		{
		//if (usecache)
		//	{
			matrixcache.deleteCol(colindex);
		//	}else {
		//	solverinterface->deleteCols(1, &colindex);
		//	}
		}

	int FMTlpsolver::getiterationcount() const
		{
		int iterations = 0;
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
		return iterations;
		}

	void FMTlpsolver::setsolvertype(FMTsolverinterface& lsolvertype) const
		{
		lsolvertype = solvertype;
		}

	void FMTlpsolver::passinsolver(FMTlpsolver& solver)
		{
		usecache = solver.usecache;
		matrixcache = solver.matrixcache;
		solvertype = solver.solvertype;
		solverinterface = solver.solverinterface;
		solverinterface->passInMessageHandler(solver.solverinterface->messageHandler());
		}

	void FMTlpsolver::clearrowcache()
		{
		#ifdef  FMTWITHMOSEK
			matrixcache.synchronize(solverinterface);
			if (solvertype == Models::FMTsolverinterface::MOSEK)
				{
				OsiMskSolverInterface* msksolver = dynamic_cast<OsiMskSolverInterface*>(solverinterface.get());
				msksolver->freeCachedRowRim();
				}
		#endif
		}

	void FMTlpsolver::setInteger(const int& colindex)
		{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->setInteger(colindex);
		}catch (...)
			{
			Exception::FMTexceptionhandler().raisefromcatch("at column index " + colindex, "FMTlpsolver::setInteger", __LINE__, __FILE__);
			}
		}

	bool FMTlpsolver::stockresolve()
		{
		matrixcache.synchronize(solverinterface);
		solverinterface->resolve();
		return solverinterface->isProvenOptimal();
		}

	void FMTlpsolver::setInteger(const int* indices, int len)
		{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->setInteger(indices, len);
		}catch (...)
			{
			Exception::FMTexceptionhandler().raisefromcatch("", "FMTlpsolver::setInteger", __LINE__, __FILE__);
			}

		}

	void FMTlpsolver::setObjective(const double* objectivevalues)
		{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->setObjective(objectivevalues);
		}catch (...)
			{
			Exception::FMTexceptionhandler().raisefromcatch("", "FMTlpsolver::setObjective", __LINE__, __FILE__);
			}
		}
	void FMTlpsolver::setObjSense(const double& newsense)
		{
		try {
			solverinterface->setObjSense(newsense);
		}catch (...)
			{
			Exception::FMTexceptionhandler().raisefromcatch("", "FMTlpsolver::setObjSense", __LINE__, __FILE__);
			}
		}

	void FMTlpsolver::deleteRows(int numberofrows, const int* rowindexes)
		{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->deleteRows(numberofrows, rowindexes);
		}catch (...)
			{
			Exception::FMTexceptionhandler().raisefromcatch("", "FMTlpsolver::deleteRows", __LINE__, __FILE__);
			}

		}

	void FMTlpsolver::deleteCols(int numberofcols, const int* colindexes)
		{
		try {
			matrixcache.synchronize(solverinterface);
			solverinterface->deleteCols(numberofcols, colindexes);
		}catch (...)
			{
			Exception::FMTexceptionhandler().raisefromcatch("", "FMTlpsolver::deleteCols", __LINE__, __FILE__);
			}
		}

	const CoinPackedMatrix* FMTlpsolver::getMatrixByRow() const
		{
		matrixcache.synchronize(solverinterface);
		return solverinterface->getMatrixByRow();
		}
	const CoinPackedMatrix* FMTlpsolver::getMatrixByCol() const
		{
		matrixcache.synchronize(solverinterface);
		return solverinterface->getMatrixByCol();
		}

	void FMTlpsolver::writeLP(const std::string& location) const
		{
		matrixcache.synchronize(solverinterface);
		solverinterface->writeLp(location.c_str());
		}
	void FMTlpsolver::writeMPS(const std::string& location) const
		{
		matrixcache.synchronize(solverinterface);
		solverinterface->writeMps(location.c_str());
		}

	void FMTlpsolver::branchAndBound()
		{
		matrixcache.synchronize(solverinterface);
		solverinterface->branchAndBound();
		}

	void FMTlpsolver::synchronize()
		{
		try {
			matrixcache.synchronize(solverinterface);
		}catch (...)
			{
			Exception::FMTexceptionhandler().raisefromcatch("", "FMTlpsolver::synchronize", __LINE__, __FILE__);
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
		matrixcache.synchronize(solverinterface);
		usecache = false;
		}

	void FMTlpsolver::sortdeletedcache()
		{
		matrixcache.sortandcleandeleted();
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
		return value;
		}

	int FMTlpsolver::getrow(int whichRow, double &rowLower, double &rowUpper,
		std::vector<int>&indices, std::vector<double>&elements) const
		{
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
				Exception::FMTexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed,
					"for row id " + whichRow, "FMTlpsolver::getrow", __LINE__, __FILE__);
				}
		return -1;
		}

	int FMTlpsolver::getcol(int whichCol, double &colLower, double &colUpper, double &objectiveValue,
		std::vector<int>& indices, std::vector<double>&elements) const
		{
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
			Exception::FMTexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed,
				"for column id " + std::to_string(whichCol), "FMTlpsolver::getcol", __LINE__, __FILE__);
		}
		return -1;
		}

}
BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTlpsolver)
#endif