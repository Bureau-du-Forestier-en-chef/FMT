/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifdef FMTWITHOSI
#ifndef FMTsolve_H_INCLUDED
#define FMTsolve_H_INCLUDED
#include <OsiSolverInterface.hpp>
#include "FMTsolverinterface.h"
#include "FMTserializablematrix.h"
#include "FMTmatrixbuild.h"
#include "FMTobject.h"
#include <memory>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/export.hpp>

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
class FMTlpsolver
	{
	// DocString: FMTlpsolver::save
	/**
	Save function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void save(Archive& ar, const unsigned int version) const
		{
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
	// DocString: FMTlpsolver::stockresolve
	/**
	Returns true if the solving result in a optimal solution. It calls the plain old osisolverinterface->resolve().
	A synchronization will be done when calling this function.
	*/
	bool stockresolve();

	void synchronize();

	std::string getcacheelements() const;

	std::string lowernuppertostr(const double& lower, const double& upper) const;
	public:
		// DocString: FMTlpsolver()
		/**
		Default constructor for FMTlpsolver.
		*/
		FMTlpsolver() = default;
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
		// DocString: FMTlpsolver::operator=
		/**
		Default copy assignment for FMTlpsolver.
		*/
		FMTlpsolver& operator =(const FMTlpsolver& rhs);
		// DocString: FMTlpsolver(FMTsolverinterface,Logging::FMTlogger&)
		/**
		Constructor for FMTlpsolver with a (lsolvertype) and a (logger). It's the main constructor used.
		*/
		FMTlpsolver(FMTsolverinterface lsolvertype,Logging::FMTlogger& logger);


		bool operator == (const FMTlpsolver& rhs) const;
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
		void passinsolver(FMTlpsolver& solver);
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



		inline int getNumCols() const
			{
			return solverinterface->getNumCols() + matrixcache.numbernewCols() - matrixcache.numberofdeletedCols();
			}

		inline int getNumRows() const
			{
			return solverinterface->getNumRows() + matrixcache.numbernewRows() - matrixcache.numberofdeletedRows();
			}

		inline const double* getObjCoefficients() const
			{
			matrixcache.synchronize(solverinterface);
			return solverinterface->getObjCoefficients();
			}
		inline const double* getColLower() const
			{
			matrixcache.synchronize(solverinterface);
			return solverinterface->getColLower();
			}
		inline const double* getColUpper() const
			{
			matrixcache.synchronize(solverinterface);
			return solverinterface->getColUpper();
			}
		inline const double* getColSolution() const
			{
			//matrixcache.synchronize(solverinterface);
			return solverinterface->getColSolution();
			}

		inline const double* getRowActivity() const
			{
			//matrixcache.synchronize(solverinterface);
			return solverinterface->getRowActivity();
			}

		inline const double* getRowUpper() const
			{
			matrixcache.synchronize(solverinterface);
			return solverinterface->getRowUpper();
			}

		inline const double* getRowLower() const
			{
			matrixcache.synchronize(solverinterface);
			return solverinterface->getRowLower();
			}

		inline double getObjSense() const
			{
			matrixcache.synchronize(solverinterface);
			return solverinterface->getObjSense();
			}

		void setObjective(const double* objectivevalues);
		void setObjSense(const double& newsense);
		

		inline void addRow(int numberInRow, const int * columns, const double * elements, double rowLower = -COIN_DBL_MAX, double rowUpper = COIN_DBL_MAX)
			{
			if (usecache)
				{
				matrixcache.addRow(numberInRow, columns, elements, rowLower, rowUpper);
			}else {
				solverinterface->addRow(numberInRow, columns, elements, rowLower, rowUpper);
				}
			
			}

		inline void addCol(int numberInColumn, const int * rows, const double * elements, double columnLower = 0.0,
			double columnUpper = COIN_DBL_MAX, double objectiveValue = 0.0)
			{
			if (usecache)
			{
			matrixcache.addCol(numberInColumn, rows, elements, columnLower,columnUpper, objectiveValue);
			}else {
				solverinterface->addCol(numberInColumn, rows, elements, columnLower, columnUpper, objectiveValue);
				}
			}

		inline void addRows(const int numrows, const int* rowStarts, const int* columns,
			const double* elements, const double* rowlb, const double* rowub)
			{
			matrixcache.synchronize(solverinterface);
			solverinterface->addRows(numrows, rowStarts, columns, elements, rowlb, rowub);
			}

		inline void addCols(const int numcols, const int* columnStarts, const int* rows,
			const double* elements, const double* collb, const double* colub,const double* obj)
			{
			matrixcache.synchronize(solverinterface);
			solverinterface->addCols(numcols,columnStarts,rows,elements,collb,colub,obj);
			}

		inline bool isProvenOptimal() const
			{
			return solverinterface->isProvenOptimal();
			}

		void deleteRow(const int& rowindex);
		void deleteRows(int numberofrows,const int* rowindexes);
		void deleteCols(int numberofcols, const int* colindexes);
		void deleteCol(const int& colindex);
		void setColSolution(const double* newsolution);
		void setColSetBounds(const int* indexFirst, const int* indexLast, const double* boundlist);
		void setRowSetBounds(const int* indexFirst, const int* indexLast, const double* boundlist);
		void setInteger(const int& colindex);
		void setInteger(const int* indices,int len);
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

		void branchAndBound();

		void enablematrixcaching();
		void disablematrixcaching();

		inline const std::vector<int>& getcachedeletedconstraints() const
			{
			return matrixcache.getdeletedconstraints();
			}

		inline const std::vector<int>& getcachedeletedvariables() const
			{
			return matrixcache.getdeletedvariables();
			}

		void sortdeletedcache();

		
	
			
	};
}
BOOST_CLASS_EXPORT_KEY(Models::FMTlpsolver);
#endif
#endif
