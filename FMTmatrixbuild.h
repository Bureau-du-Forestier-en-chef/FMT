#ifdef FMTWITHOSI

#ifndef FMTmatrixbuild_H_INCLUDED
#define FMTmatrixbuild_H_INCLUDED

#include "CoinBuild.hpp"
#include "OsiSolverInterface.hpp"
#include <memory>

namespace Models

{ 
	// DocString: FMTmatrixbuild
	/**
	Sometime adding constraints and/or variables one at a time to the matrix using osisolverinterface can be slow.
	CoinBuild object lets the user add row or variable to that class and then synchronize it with the matrix.
	This class keeps track of the last added columns and last added row to let the user virtualy add more stuff
	to the matrix (row and/or columns). This class is made for efficiency. 
	*/
	class FMTmatrixbuild
	{
		// DocString: FMTmatrixbuild::lastcol
		///lastcol is the matrix index's of the last column added to colsbuild object
		int colscount;
		// DocString: FMTmatrixbuild::lastrow
		///lastrow is the matrix index's of the last row added to rowsbuild object
		int rowscount;
		// DocString: FMTmatrixbuild::colsbuild
		///colsbuild keep the track of all columns to add to the matrix
		CoinBuild colsbuild;
		// DocString: FMTmatrixbuild::rowsbuild
		///rowsbuild keep the track of all columns to add to the matrix
		CoinBuild rowsbuild;
		// DocString: FMTmatrixbuild::deletedconstraints
		///Deleted constraints used in replanning context when the constraints indexes need to be updated.
		std::vector<int>deletedconstraints;
		// DocString: FMTmatrixbuild::deletedvariables
		///Deleted variables used in replanning context when the variables indexes need to be updated.
		std::vector<int>deletedvariables;
		void sortelementsandclean(std::vector<int>& elements) const;
	public:
		// DocString: FMTmatrixbuild::synchronize
		/**
		This function synchronize the cols from colsbuild and the row of rowbuild 
		with the Osisolverinterface matrix (solver)
		*/
		void synchronize(std::shared_ptr<OsiSolverInterface> solver);
		// DocString: FMTmatrixbuild()
		/**
			Default constructor of FMTmatrixbuild.
		*/
		FMTmatrixbuild();
		// DocString: FMTmatrixbuild(const FMTmatrixbuild)
		/**
		Copy constructor of FMTmatrixbuild.
		*/
		FMTmatrixbuild(const FMTmatrixbuild& rhs);
		// DocString: FMTmatrixbuild::operator=
		/**
		Copy assignment of FMTmatrixbuild.
		*/
		FMTmatrixbuild& operator = (const FMTmatrixbuild& rhs);
		// DocString: ~FMTmatrixbuild()
		/**
		Destructor of FMTmatrixbuild.
		*/
		~FMTmatrixbuild()=default;
		// DocString: FMTmatrixbuild::addCol
		/**
		Add a column to colsbuild to potentialy synchronize it with the synchronize function.
		*/
		inline void addCol(int numberInColumn, const int * rows, const double * elements, double columnLower = 0.0,
			double columnUpper = COIN_DBL_MAX, double objectiveValue = 0.0)
			{
			colsbuild.addCol(numberInColumn, rows, elements, columnLower,
				columnUpper, objectiveValue);
			++colscount;
			}
		// DocString: FMTmatrixbuild::addRow
		/**
		Add a row to rowbuild to potentialy synchronize it with the synchronize function.
		*/
		inline void addRow(int numberInRow, const int * columns,const double * elements, double rowLower = -COIN_DBL_MAX,double rowUpper = COIN_DBL_MAX)
			{
			rowsbuild.addRow(numberInRow, columns, elements, rowLower, rowUpper);
			++rowscount;
			}

		void deleteRow(const int& rowindex);
		void deleteCol(const int& colindex);

		inline int numbernewRows() const
			{
			return rowscount;
			}

		inline int numbernewCols() const
			{
			return colscount;
			}

		inline int numberofdeletedRows() const
			{
			return static_cast<int>(deletedconstraints.size());
			}

		inline int numberofdeletedCols() const
			{
			return static_cast<int>(deletedvariables.size());
			}

		inline const std::vector<int>& getdeletedconstraints() const
			{
			return deletedconstraints;
			}

		inline const std::vector<int>& getdeletedvariables() const
			{
			return deletedvariables;
			}
		void sortandcleandeleted();
		std::string getrowstosynchronize() const;
		std::string getcolstosynchronize() const;
	};
}

#endif
#endif