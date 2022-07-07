/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI

#ifndef FMTmatrixbuild_H_INCLUDED
#define FMTmatrixbuild_H_INCLUDED


#include <vector>
#include <string>
#include <memory>
#include "FMTutility.hpp"
#include "FMTobject.hpp"
class OsiSolverInterface;
class CoinBuild;


namespace Models

{ 
	// DocString: FMTmatrixbuild
	/**
	Sometime adding constraints and/or variables one at a time to the matrix using osisolverinterface can be slow.
	CoinBuild object lets the user add row or variable to that class and then synchronize it with the matrix.
	This class keeps track of the last added columns and last added row to let the user virtualy add more stuff
	to the matrix (row and/or columns). This class is made for efficiency. 
	*/
	class FMTEXPORT FMTmatrixbuild
	{
		// DocString: FMTmatrixbuild::lastcol
		///lastcol is the matrix index's of the last column added to colsbuild object
		int colscount;
		// DocString: FMTmatrixbuild::lastrow
		///lastrow is the matrix index's of the last row added to rowsbuild object
		int rowscount;
		// DocString: FMTmatrixbuild::colsbuild
		///colsbuild keep the track of all columns to add to the matrix
		std::unique_ptr<CoinBuild>colsbuild;
		// DocString: FMTmatrixbuild::rowsbuild
		///rowsbuild keep the track of all rows to add to the matrix
		std::unique_ptr<CoinBuild>rowsbuild;
		// DocString: FMTmatrixbuild::deletedconstraints
		///Deleted constraints used in replanning context when the constraints indexes need to be updated.
		std::vector<int>deletedconstraints;
		// DocString: FMTmatrixbuild::deletedvariables
		///Deleted variables used in replanning context when the variables indexes need to be updated.
		std::vector<int>deletedvariables;
		// DocString: FMTmatrixbuild::columnnames
		///Cached column names;
		std::vector<std::string>columnnames;
		// DocString: FMTmatrixbuild::rownames
		///Cached row names;
		std::vector<std::string>rownames;
		// DocString: FMTmatrixbuild::sortelementsandclean
		/**
		The function removes duplicate and sort a vector of (elements).
		*/
		void sortelementsandclean(std::vector<int>& elements) const;
		// DocString: FMTmatrixbuild::formatformatrixname
		/**
		Format a string for matrix naming variables and constraints.
		*/
		std::string formatformatrixname(std::string name, bool shortformat) const;
	public:
		// DocString: FMTmatrixbuild::swap
		/**
		Swap for FMTmatrixbuild
		*/
		void swap(FMTmatrixbuild& rhs);
		// DocString: FMTmatrixbuild::getcolumnnames
		/**
		Return cached column names.
		*/
		inline std::vector<std::string>& getcolumnnames()
			{
			return columnnames;
			}
		// DocString: FMTmatrixbuild::getrownames
		/**
		Return cached row names.
		*/
		inline std::vector<std::string>& getrownames()
			{
			return rownames;
			}
		// DocString: FMTmatrixbuild::formatallnames
		/**
		Formats names of row and column in cache.
		*/
		void formatallnames(bool shortformat);
		// DocString: FMTmatrixbuild::setcolname
		/**
		set column name.
		*/
		void setcolname(const std::string& name,const int& columnid);
		// DocString: FMTmatrixbuild::setrowname
		/**
		set row name.
		*/
		void setrowname(const std::string& name, const int& rowid);
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
		~FMTmatrixbuild();
		// DocString: FMTmatrixbuild::addCol
		/**
		Add a column to colsbuild to potentialy synchronize it with the synchronize function.
		*/
		void addCol(int numberInColumn, const int * rows, const double * elements, double columnLower = 0.0,
			double columnUpper = std::numeric_limits<double>::max(), double objectiveValue = 0.0);
		// DocString: FMTmatrixbuild::addRow
		/**
		Add a row to rowbuild to potentialy synchronize it with the synchronize function.
		*/
		void addRow(int numberInRow, const int * columns, const double * elements,
			double rowLower = -std::numeric_limits<double>::max(), double rowUpper = std::numeric_limits<double>::max());
		// DocString: FMTmatrixbuild::deleteRow
		/**
		The function delete a row (rowindex) from the matrix cache.
		*/
		void deleteRow(const int& rowindex);
		// DocString: FMTmatrixbuild::deleteCol
		/**
		The function delete a column (colindex) from the matrix cache.
		*/
		void deleteCol(const int& colindex);
		// DocString: FMTmatrixbuild::numbernewRows
		/**
		Returns the number of newly added rows that are in the matrix cache.
		*/
		inline int numbernewRows() const
			{
			return rowscount;
			}
		// DocString: FMTmatrixbuild::numbernewCols
		/**
		Returns the number of newly added columns that are in the matrix cache.
		*/
		inline int numbernewCols() const
			{
			return colscount;
			}
		// DocString: FMTmatrixbuild::numberofdeletedRows
		/**
		Returns the number of newly deleted rows that are in the matrix cache.
		*/
		inline int numberofdeletedRows() const
			{
			return static_cast<int>(deletedconstraints.size());
			}
		// DocString: FMTmatrixbuild::numberofdeletedCols
		/**
		Returns the number of newly deleted columns that are in the matrix cache.
		*/
		inline int numberofdeletedCols() const
			{
			return static_cast<int>(deletedvariables.size());
			}
		// DocString: FMTmatrixbuild::getdeletedconstraints
		/**
		Returns the newly deleted rows that are in the matrix cache.
		*/
		inline const std::vector<int>& getdeletedconstraints() const
			{
			return deletedconstraints;
			}
		// DocString: FMTmatrixbuild::getdeletedvariables
		/**
		Returns the newly deleted columns that are in the matrix cache.
		*/
		inline const std::vector<int>& getdeletedvariables() const
			{
			return deletedvariables;
			}
		// DocString: FMTmatrixbuild::sortandcleandeleted
		/**
		The function will remove duplicate from the deleted rows and columns elements and then sort the vector.
		*/
		void sortandcleandeleted();
		// DocString: FMTmatrixbuild::getrowstosynchronize
		/**
		The function is mainly for debugging it returns a string of all the rows that are in the matrix cache.
		*/
		std::string getrowstosynchronize() const;
		// DocString: FMTmatrixbuild::getrowstosynchronize
		/**
		The function is mainly for debugging it returns a string of all the columns that are in the matrix cache.
		*/
		std::string getcolstosynchronize() const;
		// DocString: FMTmatrixbuild::getrow
		/**
		Given a given row (whichRow) the function will fill up the row lower bound (rowLower), the row upper bound (rowUpper),
		the row's (indicies) and the row's elements present in the cache.
		*/
		int getrow(int whichRow, double &rowLower, double &rowUpper,
			std::vector<int>& indices, std::vector<double>&elements) const;
		// DocString: FMTmatrixbuild::getcol
		/**
		Given a col (whichCol) the function will fill up the column lower bound (colLower), the column upper bound (colUpper),
		the column's objective (objectiveValue), the column's (indicies) and the column's elements present in the cache.
		*/
		int getcol(int whichCol, double &colLower, double &colUpper, double &objectiveValue,
			std::vector<int>& indices, std::vector<double>&elements) const;


	};
}

#endif
#endif