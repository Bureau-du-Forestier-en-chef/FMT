/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
#ifndef FMTsolve_H_INCLUDED
#define FMTsolve_H_INCLUDED
#include "FMTsolverinterface.hpp"
#include "FMTserializablematrix.hpp"
#include "FMTmatrixbuild.hpp"
#include "FMTobject.hpp"
#include <memory>
#include <unordered_map>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/export.hpp>

#ifdef FMTWITHMOSEK
	#include "mosek.h"
#endif


class OsiSolverInterface;

namespace Models
{

// DocString: FMTlpsolver
/**
This class is an abstract class to make the usage of osisolverinterface more easy in FMT.
Usefull stuff if dealing with linear programming model. the FMTlpsolver class was needed to
reduce the overhead caused by the addrow / addcol osisolverinterface during recurrent calls.
All calls related to add/remove columns or rows checking for primal or dual solution getting the number of
rows or columns dont need synchronization with the cache. So those calls are going to be faster when usecache = true.
*/
class FMTEXPORT FMTlpsolver: public Core::FMTobject
	{
	// DocString: FMTlpsolver::save
	/**
	Save function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void save(Archive& ar, const unsigned int version) const
		{
		ar & boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<Core::FMTobject>(*this));
		ar & BOOST_SERIALIZATION_NVP(usecache);
		matrixcache.synchronize(solverinterface);
		const FMTserializablematrix matrix(solverinterface);
		ar & BOOST_SERIALIZATION_NVP(solvertype);
		ar & BOOST_SERIALIZATION_NVP(matrix);
		}
	// DocString: FMTlpsolver::load
	/**
	Load function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	template<class Archive>
	void load(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTobject>(*this));
		ar & BOOST_SERIALIZATION_NVP(usecache);
		matrixcache.synchronize(solverinterface);
		FMTserializablematrix matrix;
		ar & BOOST_SERIALIZATION_NVP(solvertype);
		ar & BOOST_SERIALIZATION_NVP(matrix);
		solverinterface = this->buildsolverinterface(solvertype);
		matrix.setmatrix(solverinterface);
	}
	BOOST_SERIALIZATION_SPLIT_MEMBER()
	// DocString: FMTlpsolver::solverinterface
	///The osisolverinterface Abstract class (constraints/objectives/matrix ....LP) can be shared with an heuristic!
	std::shared_ptr<OsiSolverInterface>solverinterface;
	// DocString: FMTlpsolver::usecache
	///If usecache is true then the matrix cache will be used by the FMTlpsolver to reduce the number of calls made to the solver.
	bool usecache;
	protected:
	// DocString: FMTlpsolver::matrixcache
	///The matrix cache follow the constraints or variables that need to be added or removed to the problem.
	mutable FMTmatrixbuild matrixcache;
	// DocString: FMTlpsolver::solvertype
	///Solver type used maybe usefull for initialsolve or resolve to know what solver we are using to speed-up the process.
	FMTsolverinterface solvertype;
	// DocString: FMTlpsolver::canupdatesource
	/**

	*/
	bool canupdatesource() const;
	// DocString: FMTlpsolver::buildsolverinterface
	/**
	Function used to build a shared pointer of a solverinterface passing the message handler to the pointer.
	*/
	std::shared_ptr<OsiSolverInterface> buildsolverinterface(const FMTsolverinterface& lsolvertype) const;
	// DocString: FMTlpsolver::copysolverinterface
	/**
	Function used to copy a shared pointer of a solverinterface passing the message handler to the pointer to a other shared pointer.
	*/
	std::shared_ptr<OsiSolverInterface> copysolverinterface(const std::shared_ptr<OsiSolverInterface>& solver_ptr, const FMTsolverinterface& lsolvertype) const;
	// DocString: FMTlpsolver::clearrowcache
		/**
		Clears the row caching of the osisolverinterface if Mosek is used.
		*/
	void clearrowcache();
	// DocString: FMTlpsolver::getMatrixByRow
	/**
	Returns a Coinpackedmatrix by row of the problem. A synchronization will be done when calling this function.
	*/
	const CoinPackedMatrix* getMatrixByRow() const;
	// DocString: FMTlpsolver::getMatrixByCol
	/**
	Returns a Coinpackedmatrix by column of the problem. A synchronization will be done when calling this function.
	*/
	const CoinPackedMatrix* getMatrixByCol() const;
	public:
		// DocString: FMTlpsolver::swap
		/**
		Swap a FMTlpsolver.
		*/
		void swap(FMTlpsolver& rhs);
		// DocString: FMTlpsolver::passinlogger
		/**
		We need to override the passinlogger for the osisolverinterface
		*/
		void passinlogger(const std::shared_ptr<Logging::FMTlogger>& logger) override;
		// DocString: FMTlpsolver::unmarkHotStart
		/**
		Clear all cached data in osisolverinterface.
		*/
		void unmarkHotStart();
		// DocString: FMTlpsolver::stockresolve
		/**
		Returns true if the solving result in a optimal solution. It calls the plain old osisolverinterface->resolve().
		A synchronization will be done when calling this function.
		*/
		bool stockresolve();
		// DocString: FMTlpsolver()
		/**
		Default constructor for FMTlpsolver.
		*/
		FMTlpsolver();
		// DocString: ~FMTlpsolver()
		/**
		Default virtual destructor for FMTlpsolver.
		*/
		virtual ~FMTlpsolver() = default;
		// DocString: FMTlpsolver(const FMTlpsolver&)
		/**
		Default copy constructor for FMTlpsolver.
		*/
		FMTlpsolver(const FMTlpsolver& rhs);
		// DocString: FMTlpsolver(FMTlpsolver&&)
		/**
		Default move constructor for FMTlpsolver.
		*/
		FMTlpsolver(FMTlpsolver&& rhs)=default;
		// DocString: FMTlpsolver::operator=
		/**
		Default copy assignment for FMTlpsolver.
		*/
		FMTlpsolver& operator =(const FMTlpsolver& rhs);
		// DocString: FMTlpsolver::operator=
		/**
		Default move assignment for FMTlpsolver.
		*/
		FMTlpsolver& operator =(FMTlpsolver&& rhs) =default;
		// DocString: FMTlpsolver(FMTsolverinterface,Logging::FMTlogger&)
		/**
		Constructor for FMTlpsolver with a (lsolvertype) and a (logger). It's the main constructor used.
		*/
		FMTlpsolver(FMTsolverinterface lsolvertype/*,Logging::FMTlogger& logger*/);
		// DocString: FMTlpsolver::operator==
		/**
		Comparison operator of FMTlpsolver
		*/
		bool operator == (const FMTlpsolver& rhs) const;
		// DocString: FMTlpsolver::operator==
		/**
		FMTlpsolver nonequality operator.
		*/
		bool operator != (const FMTlpsolver& rhs) const;
		// DocString: FMTlpsolver::resolve
		/**
		By default call solverinterface->resolve() when some changes are done to the model.
		The user dont necessery need the call initialsolve every time the matrix has changed a call to resolve maybe enought.
		*/
		virtual bool resolve();
		// DocString: FMTlpsolver::initialsolve
		/**
		Cold start of the LPsolve of a simple LP model.
		By default initialsolve will call solverinterface->initialsolve() but using the FMTsolverinterface enum
		this function will try to use the best solver parameters for a Type III Forest planning model.
		For all solvers interior point is considered the best algorith.
		*/
		virtual bool initialsolve();
		// DocString: FMTlpsolver::setnumberofthreads
		/**
		set the maximum number of threads to be used by the solver.
		Not all solver can have numberof threads sets.
		*/
		void setnumberofthreads(const size_t& nthread);
		// DocString: FMTlpsolver::setMIPgaptolerance
		/**
		set The tolerance between the relaxed optimal and the integer optimal.
		Default it 1.0-4...might be better to use a larger gap like 0.01 (1%) or even 5%...
		Not all solver can have the MIPgap set...
		*/
		void setMIPgaptolerance(const double& gap);
		// DocString: FMTlpsolver::setoptimizerMAXtime
		/**
		Maximum amount of time the optimizer is allowed to spent on the optimization. A negative number means infinity.
		*/
		void setoptimizerMAXtime(const double& time);
		// DocString: FMTlpsolver::setFeasiblePumpImprovSol
		/**
		Controls the way the Feasibility Pump heuristic is employed by the mixed-integer optimizer.The Feasibility Pump is enabled with an effort to improve solution quality. And others
		*/
		void MIPparameters();
		// DocString: FMTlpsolver::getObjValue
		/**
		Get the objective value of the solved matrix.
		*/
		double getObjValue() const;
		// DocString: FMTlpsolver::getiterationcount
		/**
		Get the number of iterations done by the solver.
		*/
		int getiterationcount() const;
		// DocString: FMTlpsolver::passinsolver
		/**
		This will share the solverinterface pointer of (solver).
		*/
		void passinsolver(const FMTlpsolver& solver);
		// DocString: FMTlpsolver::passinmessagehandler
		/**
		This will pass the FMTlogger to the solverinterface.
		*/
		void passinmessagehandler(Logging::FMTlogger& logger);
		// DocString: FMTlpsolver::setsolvertype
		/**
		Setter of the solvertype of the serializable matrix.
		*/
		void setsolvertype(FMTsolverinterface& lsolvertype) const;
		// DocString: FMTlpsolver::gotlicense
		/**
		Returns true if the license of the solver is available.
		*/
		bool gotlicense() const;
		// DocString: FMTlpsolver::getsolvername
		/**
		Returns the name of the solver used.
		*/
		std::string getsolvername() const;
		// DocString: FMTlpsolver::getNumCols
		/**
		Returns the number of columns by looking in the solverinterface and in the cache.
		*/
		int getNumCols() const;
		// DocString: FMTlpsolver::getNumRows
		/**
		Returns the number of rows by looking in the solverinterface and in the cache.
		*/
		int getNumRows() const;
		// DocString: FMTlpsolver::getObjCoefficients
		/**
		Returns the objective coefficients of the matrix. The function will synchronize the solverinterface with the
		matrix cache before getting the coefficients.
		*/
		const double* getObjCoefficients() const;
		// DocString: FMTlpsolver::getColLower
		/**
		Returns columns lower bounds of the matrix. The function will synchronize the solverinterface with the
		matrix cache before getting the bounds.
		*/
		const double* getColLower() const;
		// DocString: FMTlpsolver::getColUpper
		/**
		Returns columns upper bounds of the matrix. The function will synchronize the solverinterface with the
		matrix cache before getting the bounds.
		*/
		const double* getColUpper() const;
		// DocString: FMTlpsolver::getColSolution
		/**
		Returns the columns solution of the solverinterface. The function wont synchronize the solverinterface with the cache.
		*/
		const double* getColSolution() const;
		// DocString: FMTlpsolver::getRowActivity
		/**
		Returns the rows solution of the solverinterface. The function wont synchronize the solverinterface with the cache.
		*/
		const double* getRowActivity() const;
		// DocString: FMTlpsolver::getRowUpper
		/**
		Returns rows upper bounds of the solverinterface. The function will synchronize the solverinterface with the
		matrix cache before getting the bounds.
		*/
		const double* getRowUpper() const;
		// DocString: FMTlpsolver::getRowLower
		/**
		Returns rows lower bounds of the solverinterface. The function will synchronize the solverinterface with the
		matrix cache before getting the bounds.
		*/
		const double* getRowLower() const;
		// DocString: FMTlpsolver::getrow
		/**
		Given a g row (whichRow) the function will fill up the row lower bound (rowLower), the row upper bound (rowUpper),
		the row's (indicies) and the row's elements. The function will try to not synchronize the matrix has much has possible.
		It will returns the number of variables within the row.
		*/
		int getrow(int whichRow, double &rowLower, double &rowUpper,
			std::vector<int>& indices, std::vector<double>&elements) const;
		// DocString: FMTlpsolver::getcol
		/**
		Given a col (whichCol) the function will fill up the column lower bound (colLower), the column upper bound (colUpper),
		the column's objective (objectiveValue), the column's (indicies) and the column's elements.
		The function will try to not synchronize the matrix has much has possible.
		*/
		int getcol(int whichCol, double &colLower, double &colUpper, double &objectiveValue,
			std::vector<int>& indices, std::vector<double>&elements) const;
		// DocString: FMTlpsolver::getObjSense
		/**
		The function returns the objective sense of the solverinterface.The function will synchronize the solverinterface with the
		matrix cache before getting the objective sense.
		*/
		double getObjSense() const;
		// DocString: FMTlpsolver::setObjective
		/**
		Sets the objective values of the columns of the matrix.
		*/
		void setObjective(const double* objectivevalues);
		// DocString: FMTlpsolver::setObjSense
		/**
		Sets the objective sense (newsense) of the solverinterface.
		*/
		void setObjSense(const double& newsense);
		// DocString: FMTlpsolver::addRow
		/**
		Add a row, with a number of columns (numberInRow),with columns indexes (columns), with (elements) with a given lower bound (rowLower),
		and a given row upper bound (rowUpper).If usecache is true then it will be only added to the matrix cache else it will
		be added to the solverinterface.
		*/
		void addRow(int numberInRow, const int * columns, const double * elements, double rowLower = -COIN_DBL_MAX, double rowUpper = COIN_DBL_MAX);
		// DocString: FMTlpsolver::addCol
		/**
		Add a col, with a number of rows (numberInColumn),with rows indexes (rows), with (elements) with a given lower bound (colLower),
		and a given column upper bound (colUpper) and an objective value (objectiveValue).If usecache is true then it will be only added to the matrix cache else it will
		be added to the solverinterface.
		*/
		void addCol(int numberInColumn, const int * rows, const double * elements, double columnLower = 0.0,
			double columnUpper = COIN_DBL_MAX, double objectiveValue = 0.0);
		// DocString: FMTlpsolver::addRows
		/**
		Add multiple rows directly to the matrix but first will synchronize the matrix with the matrix cache.
		*/
		void addRows(const int numrows, const int* rowStarts, const int* columns,
			const double* elements, const double* rowlb, const double* rowub);
		// DocString: FMTlpsolver::addCols
		/**
		Add multiple columns directly to the matrix but first will synchronize the matrix with the matrix cache.
		*/
		void addCols(const int numcols, const int* columnStarts, const int* rows,
			const double* elements, const double* collb, const double* colub, const double* obj);
		// DocString: FMTlpsolver::isProvenOptimal
		/**
		Returns true if the program is optimal but first will synchronize the matrix with the matrix cache.
		*/
		bool isProvenOptimal() const;
		// DocString: FMTlpsolver::deleteRow
		/**
		Delete's a given row (rowindex), if usecache is true then it will only delete it from the cache.
		*/
		void deleteRow(const int& rowindex);
		// DocString: FMTlpsolver::deleteRow
		/**
		Delete's a given column (colindex), if usecache is true then it will only delete it from the cache.
		*/
		void deleteCol(const int& colindex);
		// DocString: FMTlpsolver::deleteRows
		/**
		Delete's multiple Rows, if usecache is true then it will synchronize first the matrix befores deleting the rows.
		*/
		void deleteRows(int numberofrows,const int* rowindexes);
		// DocString: FMTlpsolver::deleteCols
		/**
		Delete's multiple Cols, if usecache is true then it will synchronize first the matrix befores deleting the columns.
		*/
		void deleteCols(int numberofcols, const int* colindexes);
		// DocString: FMTlpsolver::setColSolution
		/**
		The function will set a solution for all columns of the solverinterface,
		it will synchronize the matrix before setting the new column solution.
		*/
		void setColSolution(const double* newsolution);
		// DocString: FMTlpsolver::setColSetBounds
		/**
		It synchronize the matrix and then
		set the columns index (indexFirst) to (indexlast) lower and upper bounds (boundlist)
		*/
		void setColSetBounds(const int* indexFirst, const int* indexLast, const double* boundlist);
		// DocString: FMTlpsolver::setRowSetBoundss
		/**
		It synchronize the matrix and then
		set the rows index (indexFirst) to (indexlast) lower and upper bounds (boundlist)
		*/
		void setRowSetBounds(const int* indexFirst, const int* indexLast, const double* boundlist);
		// DocString: FMTlpsolver::setInteger
		/**
		It synchronize the matrix and then
		sets a given column (colindex) integer.
		*/
		void setInteger(const int& colindex);
		// DocString: FMTlpsolver::setInteger
		/**
		It synchronize the matrix and then
		sets multiple columns (indices) with a given size (len) integer variable.
		*/
		void setInteger(const int* indices,int len);
		// DocString: FMTlpsolver::updatematrixnaming
		/**
		Update constraints and variables names.
		*/
		//void updatematrixnaming(const std::unordered_map<int, std::string>& colsnames,
		//	const std::unordered_map<int, std::string>& rownames);
		// DocString: FMTlpsolver::setcolname
		/**
		set column name.
		*/
		void setcolname(const std::string& name, const int& columnid);
		// DocString: FMTlpsolver::setrowname
		/**
		set row name.
		*/
		void setrowname(const std::string& name, const int& rowid);
		// DocString: FMTlpsolver::writeLP
		/**
		Write the solverinterface matrix to a file (location) using the lp formulation.
		*/
		void writeLP(const std::string& location) const;
		// DocString: FMTlpsolver::writeMPS
		/**
		Write the solverinterface matrix to a file (location) using the MPS formulation.
		*/
		void writeMPS(const std::string& location) const;
		// DocString: FMTlpsolver::branchAndBound
		/**
		Synchronize the matrix cache and then calls the branch and bound MIP enumeration solver.
		*/
		void branchAndBound();
		// DocString: FMTlpsolver::enablematrixcaching
		/**
		Enable the rows and columns caching when adding or deleting a variable to the solverinterface.
		It reduces the number of calls made to the osisolverinterface.
		*/
		void enablematrixcaching();
		// DocString: FMTlpsolver::disablematrixcaching
		/**
		First it will synchronize the matrix cache with the solverinterface and then
		turn off the matrix caching, the number of calls made to osisolverinterface will be greater.
		*/
		void disablematrixcaching();
		// DocString: FMTlpsolver::getsolvertype()
		/**
		Return the solver type of the solver.
		*/
		inline FMTsolverinterface getsolvertype() const
		{
			return solvertype;
		}
		// DocString: FMTlpsolver::synchronize
		/**
		If the users as added or deleted constraints or variables this function is going to update the
		Osisolverinterface using the cachedmatrix informations (new or deleted rows and columns).
		*/
		void synchronize();
		// DocString: FMTlpsolver::sortdeletedcache
		/**
		Sorts and remove replicates present in the rows and columns to delete matrix cache.
		*/
		void sortdeletedcache();
		// DocString: FMTlpsolver::getcachedeletedconstraints
		/**
		It will returns rows ids that are in the cache and marked as deleted.
		*/
		inline const std::vector<int>& getcachedeletedconstraints() const
		{
			return matrixcache.getdeletedconstraints();
		}
		// DocString: FMTlpsolver::getcachedeletedvariable
		/**
		It will returns the cols ids that are in the cache and marked as deleted.
		*/
		inline const std::vector<int>& getcachedeletedvariables() const
		{
			return matrixcache.getdeletedvariables();
		}
		// DocString: FMTlpsolver::lowernuppertostr
		/**
		Function to convert double bounds to string bounds (-inf,inf).
		*/
		std::string lowernuppertostr(const double& lower, const double& upper) const;
		// DocString: FMTlpsolver::getcacheelements
		/**
		This function is for debugging it returns the constraints and variables to add to the solverinterface
		sitting in the matrixcache.
		*/
		std::string getcacheelements() const;
		// DocString: FMTlpsolver::updaterowsandcolsnames
		/**
		Update the rows and columns names
		*/
		void updaterowsandcolsnames(bool shortformat = true);
		#ifdef FMTWITHMOSEK
			// DocString: FMTlpsolver::getmskerrordesc
			/**
			Return description of error code from Mosek.
			*/
			std::string getmskerrordesc(MSKrescodee error) const;
		#endif
	};
}
BOOST_CLASS_EXPORT_KEY(Models::FMTlpsolver)
#endif
#endif
