/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
#ifndef FMTsolve_Hm_included
#define FMTsolve_Hm_included
#include "FMTSolverInterface.h"
#include "FMTMatrixBuild.h"
#include "FMTExceptionHandler.h"
#include "FMTObject.h"
#include <memory>
#include <unordered_map>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/export.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include "FMTSerializableMatrix.h"
//#include <mutex>


class OsiSolverInterface;
class CoinPackedMatrix;

namespace Models
{

// DocString: FMTLpSolver
/**
@brief Abstract class to ease the usage of the OsiSolverInterface in FMT for linear programming models.
@details The FMTLpSolver reduces the overhead caused by the addRow and addCol calls of the OsiSolverInterface during recurrent calls. Calls related to adding or removing columns or rows, checking the primal or dual solution and getting the number of rows or columns do not need synchronization with the cache, so they are faster when the cache is used.
*/
class FMTEXPORT FMTLpSolver: public Core::FMTObject
	{
	public:
		// DocString: FMTLpSolver::swap
		/**
		@brief Swap this FMTLpSolver with another one.
		@param[in,out] rhs the FMTLpSolver to swap with.
		*/
		void swap(FMTLpSolver& rhs);
		// DocString: FMTLpSolver::passInLogger
		/**
		@brief Pass in the logger, overridden for the OsiSolverInterface.
		@param[in] logger the logger.
		*/
		void passInLogger(const std::unique_ptr<Logging::FMTLogger>& logger) override;
		// DocString: FMTLpSolver::unmarkHotStart
		/**
		@brief Clear all cached data in the OsiSolverInterface.
		*/
		void unmarkHotStart();
		// DocString: FMTLpSolver::stockResolve
		/**
		@brief Call the plain OsiSolverInterface resolve, synchronizing the matrix first.
		@return true if the solve results in an optimal solution else false.
		*/
		bool stockResolve();
		// DocString: FMTLpSolver()
		/**
		@brief Default constructor for FMTLpSolver.
		*/
		FMTLpSolver();
		// DocString: ~FMTLpSolver()
		/**
		@brief Default virtual destructor for FMTLpSolver.
		*/
		virtual ~FMTLpSolver() = default;
		// DocString: FMTLpSolver(const FMTLpSolver&)
		/**
		@brief Default copy constructor for FMTLpSolver.
		@param[in] rhs the FMTLpSolver to copy.
		*/
		FMTLpSolver(const FMTLpSolver& rhs);
		// DocString: FMTLpSolver(FMTLpSolver&&)
		/**
		@brief Default move constructor for FMTLpSolver.
		@param[in,out] rhs the FMTLpSolver to move from.
		*/
		FMTLpSolver(FMTLpSolver&& rhs)=default;
		// DocString: FMTLpSolver::operator=
		/**
		@brief Copy assignment operator for FMTLpSolver.
		@param[in] rhs the FMTLpSolver to copy.
		@return a reference to this FMTLpSolver.
		*/
		FMTLpSolver& operator =(const FMTLpSolver& rhs);
		// DocString: FMTLpSolver::operator=
		/**
		@brief Default move assignment for FMTLpSolver.
		@param[in,out] rhs the FMTLpSolver to move from.
		@return a reference to this FMTLpSolver.
		*/
		FMTLpSolver& operator =(FMTLpSolver&& rhs) =default;
		// DocString: FMTLpSolver(FMTSolverInterface,const std::string,const std::string)
		/**
		@brief Main constructor for FMTLpSolver with a solver type, cold and warm start parameters and a problem name.
		@param[in] lsolvertype the solver type.
		@param[in] p_ColdStartParameters the cold start parameters on the form param_name param_value per line.
		@param[in] p_WarmStartParameters the warm start parameters on the form param_name param_value per line.
		@param[in] p_problemName the name of the problem.
		*/
		FMTLpSolver(FMTSolverInterface lsolvertype,
			const std::string& p_ColdStartParameters,
			const std::string& p_WarmStartParameters,
			const std::string& p_problemName);
		// DocString: FMTLpSolver::operator==
		/**
		@brief Comparison operator for FMTLpSolver.
		@param[in] rhs the FMTLpSolver to compare to.
		@return true if both solvers are equal else false.
		*/
		bool operator == (const FMTLpSolver& rhs) const;
		// DocString: FMTLpSolver::operator!=
		/**
		@brief Inequality operator for FMTLpSolver.
		@param[in] rhs the FMTLpSolver to compare to.
		@return true if both solvers are different else false.
		*/
		bool operator != (const FMTLpSolver& rhs) const;
		// DocString: FMTLpSolver::resolve
		/**
		@brief Call solverinterface->resolve when some changes are done to the model, avoiding a full initialSolve.
		@return true if the resolve succeeded else false.
		*/
		virtual bool resolve();
		// DocString: FMTLpSolver::initialSolve
		/**
		@brief Cold start of the LP solve of a simple LP model.
		@details By default calls solverinterface->initialSolve, but based on the FMTSolverInterface it tries to use the best solver parameters for a type III forest planning model; interior point is considered the best algorithm for all solvers.
		@return true if the initial solve succeeded else false.
		*/
		virtual bool initialSolve();
		// DocString: FMTLpSolver::setNumberOfThreads
		/**
		@brief Set the maximum number of threads to be used by the solver.
		@details Not all solvers can have the number of threads set.
		@param[in] nthread the number of threads.
		*/
		void setNumberOfThreads(const size_t& nthread);
		// DocString: FMTLpSolver::setMipGapTolerance
		/**
		@brief Set the tolerance between the relaxed optimal and the integer optimal.
		@details The default is 1.0e-4; a larger gap such as 0.01 (1%) or even 5% may be better. Not all solvers can have the MIP gap set.
		@param[in] gap the MIP gap tolerance.
		*/
		void setMipGapTolerance(const double& gap);
		// DocString: FMTLpSolver::setOptimizerMaxTime
		/**
		@brief Set the maximum amount of time the optimizer is allowed to spend on the optimization.
		@details A negative number means infinity.
		@param[in] time the maximum optimization time.
		*/
		void setOptimizerMaxTime(const double& time);
		// DocString: FMTLpSolver::setFeasiblePumpImprovSol
		/**
		@brief Set the mixed-integer optimizer parameters, including the way the feasibility pump heuristic is employed to improve solution quality.
		*/
		void mipParameters();
		// DocString: FMTLpSolver::getObjValue
		/**
		@brief Get the objective value of the solved matrix.
		@return the objective value.
		*/
		double getObjValue() const;
		// DocString: FMTLpSolver::getIterationCount
		/**
		@brief Get the number of iterations done by the solver.
		@return the iteration count.
		*/
		int getIterationCount() const;
		// DocString: FMTLpSolver::passInSolver
		/**
		@brief Share the solver interface pointer of another solver.
		@param[in] solver the solver to share the interface of.
		*/
		void passInSolver(const FMTLpSolver& solver);
		// DocString: FMTLpSolver::passInMessageHandler
		/**
		@brief Pass the FMTLogger to the solver interface.
		@param[in] logger the logger.
		*/
		void passInMessageHandler(Logging::FMTLogger& logger);
		// DocString: FMTLpSolver::setSolverType
		/**
		@brief Setter of the solver type of the serializable matrix.
		@param[in] lsolvertype the solver type.
		*/
		void setSolverType(FMTSolverInterface& lsolvertype) const;
		// DocString: FMTLpSolver::gotLicense
		/**
		@brief Return true if the license of the solver is available.
		@return true if the license is available else false.
		*/
		bool gotLicense() const;
		// DocString: FMTLpSolver::getSolverName
		/**
		@brief Return the name of the solver used.
		@return the solver name.
		*/
		std::string getSolverName() const;
		// DocString: FMTLpSolver::getNumCols
		/**
		@brief Return the number of columns by looking in the solver interface and in the cache.
		@return the number of columns.
		*/
		int getNumCols() const;
		// DocString: FMTLpSolver::getNumRows
		/**
		@brief Return the number of rows by looking in the solver interface and in the cache.
		@return the number of rows.
		*/
		int getNumRows() const;
		// DocString: FMTLpSolver::getObjCoefficients
		/**
		@brief Return the objective coefficients of the matrix, synchronizing the solver interface with the cache first.
		@return the objective coefficients.
		*/
		const double* getObjCoefficients() const;
		// DocString: FMTLpSolver::getColLower
		/**
		@brief Return the column lower bounds of the matrix, synchronizing the solver interface with the cache first.
		@return the column lower bounds.
		*/
		const double* getColLower() const;
		// DocString: FMTLpSolver::getColUpper
		/**
		@brief Return the column upper bounds of the matrix, synchronizing the solver interface with the cache first.
		@return the column upper bounds.
		*/
		const double* getColUpper() const;
		// DocString: FMTLpSolver::getColSolution
		/**
		@brief Return the column solution of the solver interface, without synchronizing with the cache.
		@return the column solution.
		*/
		const double* getColSolution() const;
		// DocString: FMTLpSolver::getRowPrice
		/**
		@brief Return the row price (dual solution) of the solver interface, without synchronizing with the cache.
		@return the row price.
		*/
		const double* getRowPrice() const;
		// DocString: FMTLpSolver::getRowActivity
		/**
		@brief Return the row activity of the solver interface, without synchronizing with the cache.
		@return the row activity.
		*/
		const double* getRowActivity() const;
		// DocString: FMTLpSolver::getRowUpper
		/**
		@brief Return the row upper bounds of the solver interface, synchronizing with the cache first.
		@return the row upper bounds.
		*/
		const double* getRowUpper() const;
		// DocString: FMTLpSolver::getRowLower
		/**
		@brief Return the row lower bounds of the solver interface, synchronizing with the cache first.
		@return the row lower bounds.
		*/
		const double* getRowLower() const;
		// DocString: FMTLpSolver::getRow
		/**
		@brief Fill the bounds, indices and elements of a given row, trying to synchronize the matrix as little as possible.
		@param[in] whichRow the row index.
		@param[out] rowLower the row lower bound.
		@param[out] rowUpper the row upper bound.
		@param[out] indices the row indices.
		@param[out] elements the row elements.
		@return the number of variables in the row.
		*/
		int getRow(int whichRow, double &rowLower, double &rowUpper,
			std::vector<int>& indices, std::vector<double>&elements) const;
		// DocString: FMTLpSolver::getCol
		/**
		@brief Fill the bounds, objective, indices and elements of a given column, trying to synchronize the matrix as little as possible.
		@param[in] whichCol the column index.
		@param[out] colLower the column lower bound.
		@param[out] colUpper the column upper bound.
		@param[out] objectiveValue the column objective value.
		@param[out] indices the column indices.
		@param[out] elements the column elements.
		@return the number of elements in the column.
		*/
		int getCol(int whichCol, double &colLower, double &colUpper, double &objectiveValue,
			std::vector<int>& indices, std::vector<double>&elements) const;
		// DocString: FMTLpSolver::getObjSense
		/**
		@brief Return the objective sense of the solver interface, synchronizing with the cache first.
		@return the objective sense.
		*/
		double getObjSense() const;
		// DocString: FMTLpSolver::setObjective
		/**
		@brief Set the objective values of the columns of the matrix.
		@param[in] objectivevalues the objective values.
		*/
		void setObjective(const double* objectivevalues);
		// DocString: FMTLpSolver::setObjSense
		/**
		@brief Set the objective sense of the solver interface.
		@param[in] newsense the new objective sense.
		*/
		void setObjSense(const double& newsense);
		// DocString: FMTLpSolver::addRow
		/**
		@brief Add a row with its columns, elements and bounds.
		@details If the cache is used the row is only added to the matrix cache, otherwise it is added to the solver interface.
		@param[in] numberInRow the number of columns in the row.
		@param[in] columns the column indexes.
		@param[in] elements the elements.
		@param[in] rowLower the row lower bound.
		@param[in] rowUpper the row upper bound.
		*/
		void addRow(int numberInRow, const int * columns, const double * elements,
			double rowLower = -std::numeric_limits<double>::max(), double rowUpper = std::numeric_limits<double>::max());
		// DocString: FMTLpSolver::addCol
		/**
		@brief Add a column with its rows, elements, bounds and objective value.
		@details If the cache is used the column is only added to the matrix cache, otherwise it is added to the solver interface.
		@param[in] numberInColumn the number of rows in the column.
		@param[in] rows the row indexes.
		@param[in] elements the elements.
		@param[in] columnLower the column lower bound.
		@param[in] columnUpper the column upper bound.
		@param[in] objectiveValue the objective value.
		*/
		void addCol(int numberInColumn, const int * rows, const double * elements, double columnLower = 0.0,
			double columnUpper = std::numeric_limits<double>::max(), double objectiveValue = 0.0);
		// DocString: FMTLpSolver::addRows
		/**
		@brief Add multiple rows directly to the matrix, synchronizing the matrix with the cache first.
		@param[in] numrows the number of rows.
		@param[in] rowStarts the row starts.
		@param[in] columns the column indexes.
		@param[in] elements the elements.
		@param[in] rowlb the row lower bounds.
		@param[in] rowub the row upper bounds.
		*/
		void addRows(const int numrows, const int* rowStarts, const int* columns,
			const double* elements, const double* rowlb, const double* rowub);
		// DocString: FMTLpSolver::addCols
		/**
		@brief Add multiple columns directly to the matrix, synchronizing the matrix with the cache first.
		@param[in] numcols the number of columns.
		@param[in] columnStarts the column starts.
		@param[in] rows the row indexes.
		@param[in] elements the elements.
		@param[in] collb the column lower bounds.
		@param[in] colub the column upper bounds.
		@param[in] obj the objective values.
		*/
		void addCols(const int numcols, const int* columnStarts, const int* rows,
			const double* elements, const double* collb, const double* colub, const double* obj);
		// DocString: FMTLpSolver::isProvenOptimal
		/**
		@brief Return true if the program is optimal, synchronizing the matrix with the cache first.
		@return true if the program is proven optimal else false.
		*/
		bool isProvenOptimal() const;
		// DocString: FMTLpSolver::deleteRow
		/**
		@brief Delete a given row; if the cache is used it is only deleted from the cache.
		@param[in] rowindex the row index.
		*/
		void deleteRow(const int& rowindex);
		// DocString: FMTLpSolver::deleteCol
		/**
		@brief Delete a given column; if the cache is used it is only deleted from the cache.
		@param[in] colindex the column index.
		*/
		void deleteCol(const int& colindex);
		// DocString: FMTLpSolver::deleteRows
		/**
		@brief Delete multiple rows; if the cache is used the matrix is synchronized before deleting the rows.
		@param[in] numberofrows the number of rows.
		@param[in] rowindexes the row indexes.
		*/
		void deleteRows(int numberofrows,const int* rowindexes);
		// DocString: FMTLpSolver::deleteCols
		/**
		@brief Delete multiple columns; if the cache is used the matrix is synchronized before deleting the columns.
		@param[in] numberofcols the number of columns.
		@param[in] colindexes the column indexes.
		*/
		void deleteCols(int numberofcols, const int* colindexes);
		// DocString: FMTLpSolver::setColSolution
		/**
		@brief Set a solution for all columns of the solver interface, synchronizing the matrix first.
		@param[in] newsolution the new column solution.
		*/
		void setColSolution(const double* newsolution);
		// DocString: FMTLpSolver::setRowPrice
		/**
		@brief Set a solution for all rows of the solver interface, synchronizing the matrix first.
		@param[in] rowprice the new row price.
		*/
		void setRowPrice(const double* rowprice);
		// DocString: FMTLpSolver::setColSetBounds
		/**
		@brief Synchronize the matrix and set the lower and upper bounds of a set of columns.
		@param[in] indexFirst the first column index.
		@param[in] indexLast the last column index.
		@param[in] boundlist the bounds.
		*/
		void setColSetBounds(const int* indexFirst, const int* indexLast, const double* boundlist);
		// DocString: FMTLpSolver::setRowSetBoundss
		/**
		@brief Synchronize the matrix and set the lower and upper bounds of a set of rows.
		@param[in] indexFirst the first row index.
		@param[in] indexLast the last row index.
		@param[in] boundlist the bounds.
		*/
		void setRowSetBounds(const int* indexFirst, const int* indexLast, const double* boundlist);
		// DocString: FMTLpSolver::setInteger
		/**
		@brief Synchronize the matrix and set a given column as integer.
		@param[in] colindex the column index.
		*/
		void setInteger(const int& colindex);
		// DocString: FMTLpSolver::setInteger
		/**
		@brief Synchronize the matrix and set multiple columns as integer variables.
		@param[in] indices the column indices.
		@param[in] len the number of indices.
		*/
		void setInteger(const int* indices,int len);
		// DocString: FMTLpSolver::updateMatrixNaming
		/**
		@brief Update the constraints and variables names.
		*/
		//void updateMatrixNaming(const std::unordered_map<int, std::string>& colsnames,
		//	const std::unordered_map<int, std::string>& rownames);
		// DocString: FMTLpSolver::setColName
		/**
		@brief Set a column name.
		@param[in] name the column name.
		@param[in] columnid the column index.
		*/
		void setColName(const std::string& name, const int& columnid) const;
		// DocString: FMTLpSolver::setRowName
		/**
		@brief Set a row name.
		@param[in] name the row name.
		@param[in] rowid the row index.
		*/
		void setRowName(const std::string& name, const int& rowid) const;
		// DocString: FMTLpSolver::writeLp
		/**
		@brief Write the solver interface matrix to a file using the LP formulation.
		@param[in] location the file location.
		*/
		void writeLp(const std::string& location) const;
		// DocString: FMTLpSolver::writeMps
		/**
		@brief Write the solver interface matrix to a file using the MPS formulation.
		@param[in] location the file location.
		*/
		void writeMps(const std::string& location) const;
		// DocString: FMTLpSolver::branchAndBound
		/**
		@brief Synchronize the matrix cache and call the branch and bound MIP enumeration solver.
		*/
		void branchAndBound();
		// DocString: FMTLpSolver::enableMatrixCaching
		/**
		@brief Enable the rows and columns caching when adding or deleting a variable to the solver interface, reducing the number of calls made to the OsiSolverInterface.
		*/
		void enableMatrixCaching();
		// DocString: FMTLpSolver::disableMatrixCaching
		/**
		@brief Synchronize the matrix cache with the solver interface and turn off the matrix caching.
		*/
		void disableMatrixCaching();
		// DocString: FMTLpSolver::getSolverType()
		/**
		@brief Return the solver type of the solver.
		@return the solver type.
		*/
		inline FMTSolverInterface getSolverType() const
		{
			return solvertype;
		}
		// DocString: FMTLpSolver::synchronize
		/**
		@brief Update the OsiSolverInterface using the cached matrix information (new or deleted rows and columns) when constraints or variables have been added or deleted.
		*/
		void synchronize();
		// DocString: FMTLpSolver::sortDeletedCache
		/**
		@brief Sort and remove duplicates in the rows and columns to delete in the matrix cache.
		*/
		void sortDeletedCache();
		// DocString: FMTLpSolver::getCacheDeletedConstraints
		/**
		@brief Return the row ids that are in the cache and marked as deleted.
		@return the deleted constraint ids.
		*/
		inline const std::vector<int>& getCacheDeletedConstraints() const
		{
			return matrixcache.getDeletedConstraints();
		}
		// DocString: FMTLpSolver::getcachedeletedvariable
		/**
		@brief Return the column ids that are in the cache and marked as deleted.
		@return the deleted variable ids.
		*/
		inline const std::vector<int>& getCacheDeletedVariables() const
		{
			return matrixcache.getDeletedVariables();
		}
		// DocString: FMTLpSolver::lowerNUpperToStr
		/**
		@brief Convert double bounds to string bounds (-inf, inf).
		@param[in] lower the lower bound.
		@param[in] upper the upper bound.
		@return the string bounds.
		*/
		std::string lowerNUpperToStr(const double& lower, const double& upper) const;
		// DocString: FMTLpSolver::getCacheElements
		/**
		@brief Return the constraints and variables sitting in the matrix cache to be added to the solver interface, for debugging.
		@return the cache elements.
		*/
		std::string getCacheElements() const;
		// DocString: FMTLpSolver::updateRowsAndColsNames
		/**
		@brief Update the rows and columns names.
		@param[in] shortformat if true uses the short format.
		*/
		void updateRowsAndColsNames(bool shortformat = true);
		#ifdef FMTWITHMOSEK
			// DocString: FMTLpSolver::getMskErrorDesc
			/**
			@brief Return the description of an error code from Mosek.
			@param[in] error the error code.
			@return the error description.
			*/
			std::string getMskErrorDesc(int error) const;
		#endif
	protected:
		// DocString: FMTLpSolver::matrixcache
		///The matrix cache follow the constraints or variables that need to be added or removed to the problem.
		mutable FMTMatrixBuild matrixcache;
		// DocString: FMTLpSolver::solvertype
		///Solver type used maybe usefull for initialSolve or resolve to know what solver we are using to speed-up the process.
		FMTSolverInterface solvertype;
		// DocString: FMTLpSolver::canUpdateSource
		/**
		@brief Return true if the solver interface source can be updated.
		@return true if the source can be updated else false.
		*/
		bool canUpdateSource() const;
		// DocString: FMTLpSolver::buildSolverInterface
		/**
		@brief Build a shared pointer to a solver interface, passing the message handler to the pointer.
		@param[in] lsolvertype the solver type.
		@return a shared pointer to the solver interface.
		*/
		std::shared_ptr<OsiSolverInterface> buildSolverInterface(const FMTSolverInterface& lsolvertype) const;
		// DocString: FMTLpSolver::copySolverInterface
		/**
		@brief Copy a shared pointer to a solver interface into another shared pointer, passing the message handler to the pointer.
		@param[in] solver_ptr the solver interface to copy.
		@param[in] lsolvertype the solver type.
		@return a shared pointer to the copied solver interface.
		*/
		std::shared_ptr<OsiSolverInterface> copySolverInterface(const std::shared_ptr<OsiSolverInterface>& solver_ptr, const FMTSolverInterface& lsolvertype) const;
		// DocString: FMTLpSolver::clearRowCache
		/**
		@brief Clear the row caching of the OsiSolverInterface if Mosek is used.
		*/
		void clearRowCache();
		// DocString: FMTLpSolver::getMatrixByRow
		/**
		@brief Return a CoinPackedMatrix by row of the problem, synchronizing the matrix first.
		@return the matrix by row.
		*/
		const CoinPackedMatrix* getMatrixByRow() const;
		// DocString: FMTLpSolver::getMatrixByCol
		/**
		@brief Return a CoinPackedMatrix by column of the problem, synchronizing the matrix first.
		@return the matrix by column.
		*/
		const CoinPackedMatrix* getMatrixByCol() const;
	private:
		// DocString: FMTLpSolver::strtoParams
		/**
		@brief Transform a string into a vector of parameters.
		@param[in] p_params the parameters in string.
		@return a vector of parameters on the form param_name param_value.
		*/
		static std::vector<std::pair<std::string, std::string>>strtoParams(const std::string& p_params);
		friend class boost::serialization::access;
		// DocString: FMTLpSolver::save
		/**
		@brief Save function used for serialization to do multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to save to.
		@param[in] version the serialization version.
		*/
		template<class Archive>
		void save(Archive& ar, const unsigned int version) const
		{
			try {
				ar& boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<Core::FMTObject>(*this));
				ar& BOOST_SERIALIZATION_NVP(usecache);
				matrixcache.synchronize(solverinterface);
				const FMTSerializableMatrix matrix(solverinterface);
				ar& BOOST_SERIALIZATION_NVP(solvertype);
				ar& BOOST_SERIALIZATION_NVP(matrix);
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTLpSolver::save", __LINE__, __FILE__);
			}
		}
		// DocString: FMTLpSolver::load
		/**
		@brief Load function used for serialization to do multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to load from.
		@param[in] version the serialization version.
		*/
		template<class Archive>
		void load(Archive& ar, const unsigned int version)
		{
			try {
				ar& boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTObject>(*this));
				ar& BOOST_SERIALIZATION_NVP(usecache);
				matrixcache.synchronize(solverinterface);
				FMTSerializableMatrix matrix;
				ar& BOOST_SERIALIZATION_NVP(solvertype);
				ar& BOOST_SERIALIZATION_NVP(matrix);
				solverinterface = this->buildSolverInterface(solvertype);
				matrix.setMatrix(solverinterface);
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTLpSolver::load", __LINE__, __FILE__);
			}

		}
		BOOST_SERIALIZATION_SPLIT_MEMBER()
		// DocString: FMTLpSolver::solverinterface
		///The osisolverinterface Abstract class (constraints/objectives/matrix ....LP) can be shared with an heuristic!
		std::shared_ptr<OsiSolverInterface>solverinterface;
		// DocString: FMTLpSolver::usecache
		///If usecache is true then the matrix cache will be used by the FMTLpSolver to reduce the number of calls made to the solver.
		bool usecache;
		// DocString: FMTLpSolver::>m_ColdStartParameters
		///Parameters used by the linear programming solver. First = parameter, Seconde = parameter values.
		std::vector<std::pair<std::string, std::string>>m_ColdStartParameters;
		// DocString: FMTLpSolver::>m_WarmStartParameters
		///Parameters used by the linear programming solver. First = parameter, Seconde = parameter values.
		std::vector<std::pair<std::string, std::string>>m_WarmStartParameters;
		
	#ifdef FMTWITHMOSEK
		// DocString: FMTLpSolver::_mskOptimizeWithParameters
		/**
		@brief Set the parameters of a MSK task and optimize.
		@return the Mosek response code.
		*/
		int _mskOptimizeWithParameters();
		// DocString: FMTLpSolver::_mskOptimizeWithDefaultParameters
		/**
		@brief Set the default parameters for Mosek and optimize.
		@return the Mosek response code.
		*/
		int _mskOptimizeWithDefaultParameters();
	#endif
		// DocString: FMTLpSolver::_setClpOptions
		/**
		@brief Set the options of a CLP solver before optimization.
		*/
		void _setClpOptions();
		
	};
}
BOOST_CLASS_EXPORT_KEY(Models::FMTLpSolver)
#endif
#endif
