/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI

#ifndef FMTmatrixbuild_Hm_included
#define FMTmatrixbuild_Hm_included


#include <vector>
#include <string>
#include <memory>
#include "FMTutility.h"
#include "FMTobject.h"
class OsiSolverInterface;
class CoinBuild;


namespace Models

{ 
	// DocString: FMTMatrixBuild
	/**
	Sometime adding constraints and/or variables one at a time to the matrix using osisolverinterface can be slow.
	CoinBuild object lets the user add row or variable to that class and then synchronize it with the matrix.
	This class keeps track of the last added columns and last added row to let the user virtualy add more stuff
	to the matrix (row and/or columns). This class is made for efficiency. 
	*/
	class FMTEXPORT FMTMatrixBuild
	{
		// DocString: FMTMatrixBuild::lastcol
		///lastcol is the matrix index's of the last column added to colsbuild object
		int colscount;
		// DocString: FMTMatrixBuild::lastrow
		///lastrow is the matrix index's of the last row added to rowsbuild object
		int rowscount;
		// DocString: FMTMatrixBuild::colsbuild
		///colsbuild keep the track of all columns to add to the matrix
		std::unique_ptr<CoinBuild>colsbuild;
		// DocString: FMTMatrixBuild::rowsbuild
		///rowsbuild keep the track of all rows to add to the matrix
		std::unique_ptr<CoinBuild>rowsbuild;
		// DocString: FMTMatrixBuild::deletedconstraints
		///Deleted constraints used in replanning context when the constraints indexes need to be updated.
		std::vector<int>deletedconstraints;
		// DocString: FMTMatrixBuild::deletedvariables
		///Deleted variables used in replanning context when the variables indexes need to be updated.
		std::vector<int>deletedvariables;
		// DocString: FMTMatrixBuild::columnnames
		///Cached column names;
		std::vector<std::string>columnnames;
		// DocString: FMTMatrixBuild::rownames
		///Cached row names;
		std::vector<std::string>rownames;
		// DocString: FMTMatrixBuild::sortElementsAndClean
		/**
		The function removes duplicate and sort a vector of (elements).
		*/
		void sortElementsAndClean(std::vector<int>& elements) const;
		// DocString: FMTMatrixBuild::formatForMatrixName
		/**
		Format a string for matrix naming variables and constraints.
		*/
		std::string formatForMatrixName(std::string name, bool shortformat) const;
	public:
		// DocString: FMTMatrixBuild::swap
		/**
		Swap for FMTMatrixBuild
		*/
		void swap(FMTMatrixBuild& rhs);
		// DocString: FMTMatrixBuild::getColumnNames
		/**
		Return cached column names.
		*/
		inline std::vector<std::string>& getColumnNames()
			{
			return columnnames;
			}
		// DocString: FMTMatrixBuild::getRowNames
		/**
		Return cached row names.
		*/
		inline std::vector<std::string>& getRowNames()
			{
			return rownames;
			}
		// DocString: FMTMatrixBuild::formatAllNames
		/**
		Formats names of row and column in cache.
		*/
		void formatAllNames(bool shortformat);
		// DocString: FMTMatrixBuild::setColName
		/**
		set column name.
		*/
		void setColName(const std::string& name,const int& columnid);
		// DocString: FMTMatrixBuild::setRowName
		/**
		set row name.
		*/
		void setRowName(const std::string& name, const int& rowid);
		// DocString: FMTMatrixBuild::synchronize
		/**
		This function synchronize the cols from colsbuild and the row of rowbuild 
		with the Osisolverinterface matrix (solver)
		*/
		void synchronize(std::shared_ptr<OsiSolverInterface> solver);
		// DocString: FMTMatrixBuild()
		/**
			Default constructor of FMTMatrixBuild.
		*/
		FMTMatrixBuild();
		// DocString: FMTMatrixBuild(const FMTMatrixBuild)
		/**
		Copy constructor of FMTMatrixBuild.
		*/
		FMTMatrixBuild(const FMTMatrixBuild& rhs);
		// DocString: FMTMatrixBuild::operator=
		/**
		Copy assignment of FMTMatrixBuild.
		*/
		FMTMatrixBuild& operator = (const FMTMatrixBuild& rhs);
		// DocString: ~FMTMatrixBuild()
		/**
		Destructor of FMTMatrixBuild.
		*/
		~FMTMatrixBuild();
		// DocString: FMTMatrixBuild::addCol
		/**
		Add a column to colsbuild to potentialy synchronize it with the synchronize function.
		*/
		void addCol(int numberInColumn, const int * rows, const double * elements, double columnLower = 0.0,
			double columnUpper = std::numeric_limits<double>::max(), double objectiveValue = 0.0);
		// DocString: FMTMatrixBuild::addRow
		/**
		Add a row to rowbuild to potentialy synchronize it with the synchronize function.
		*/
		void addRow(int numberInRow, const int * columns, const double * elements,
			double rowLower = -std::numeric_limits<double>::max(), double rowUpper = std::numeric_limits<double>::max());
		// DocString: FMTMatrixBuild::deleteRow
		/**
		The function delete a row (rowindex) from the matrix cache.
		*/
		void deleteRow(const int& rowindex);
		// DocString: FMTMatrixBuild::deleteCol
		/**
		The function delete a column (colindex) from the matrix cache.
		*/
		void deleteCol(const int& colindex);
		// DocString: FMTMatrixBuild::numbernewRows
		/**
		Returns the number of newly added rows that are in the matrix cache.
		*/
		inline int numbernewRows() const
			{
			return rowscount;
			}
		// DocString: FMTMatrixBuild::numbernewCols
		/**
		Returns the number of newly added columns that are in the matrix cache.
		*/
		inline int numbernewCols() const
			{
			return colscount;
			}
		// DocString: FMTMatrixBuild::numberofdeletedRows
		/**
		Returns the number of newly deleted rows that are in the matrix cache.
		*/
		inline int numberofdeletedRows() const
			{
			return static_cast<int>(deletedconstraints.size());
			}
		// DocString: FMTMatrixBuild::numberofdeletedCols
		/**
		Returns the number of newly deleted columns that are in the matrix cache.
		*/
		inline int numberofdeletedCols() const
			{
			return static_cast<int>(deletedvariables.size());
			}
		// DocString: FMTMatrixBuild::getDeletedConstraints
		/**
		Returns the newly deleted rows that are in the matrix cache.
		*/
		inline const std::vector<int>& getDeletedConstraints() const
			{
			return deletedconstraints;
			}
		// DocString: FMTMatrixBuild::getDeletedVariables
		/**
		Returns the newly deleted columns that are in the matrix cache.
		*/
		inline const std::vector<int>& getDeletedVariables() const
			{
			return deletedvariables;
			}
		// DocString: FMTMatrixBuild::sortAndCleanDeleted
		/**
		The function will remove duplicate from the deleted rows and columns elements and then sort the vector.
		*/
		void sortAndCleanDeleted();
		// DocString: FMTMatrixBuild::getRowsToSynchronize
		/**
		The function is mainly for debugging it returns a string of all the rows that are in the matrix cache.
		*/
		std::string getRowsToSynchronize() const;
		// DocString: FMTMatrixBuild::getRowsToSynchronize
		/**
		The function is mainly for debugging it returns a string of all the columns that are in the matrix cache.
		*/
		std::string getColsToSynchronize() const;
		// DocString: FMTMatrixBuild::getRow
		/**
		Given a given row (whichRow) the function will fill up the row lower bound (rowLower), the row upper bound (rowUpper),
		the row's (indicies) and the row's elements present in the cache.
		*/
		int getRow(int whichRow, double &rowLower, double &rowUpper,
			std::vector<int>& indices, std::vector<double>&elements) const;
		// DocString: FMTMatrixBuild::getCol
		/**
		Given a col (whichCol) the function will fill up the column lower bound (colLower), the column upper bound (colUpper),
		the column's objective (objectiveValue), the column's (indicies) and the column's elements present in the cache.
		*/
		int getCol(int whichCol, double &colLower, double &colUpper, double &objectiveValue,
			std::vector<int>& indices, std::vector<double>&elements) const;


	};
}

#endif
#endif