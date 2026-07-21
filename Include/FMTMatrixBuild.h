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
#include "FMTObject.h"
class OsiSolverInterface;
class CoinBuild;


namespace Models

{ 
	// DocString: FMTMatrixBuild
	/**
	@brief Helper caching rows and columns to add to a solver matrix in batch for efficiency.
	@details CoinBuild lets the user add rows or columns and then synchronize them with the matrix. This class keeps track of the last added columns and rows to let the user virtually add more to the matrix.
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
		@brief Remove duplicates and sort a vector of elements.
		@param[in,out] elements the elements to sort and clean.
		*/
		void sortElementsAndClean(std::vector<int>& elements) const;
		// DocString: FMTMatrixBuild::formatForMatrixName
		/**
		@brief Format a string for naming matrix variables and constraints.
		@param[in] name the name to format.
		@param[in] shortformat if true uses a short format.
		@return the formatted name.
		*/
		std::string formatForMatrixName(std::string name, bool shortformat) const;
	public:
		// DocString: FMTMatrixBuild::swap
		/**
		@brief Swap this FMTMatrixBuild with another.
		@param[in,out] rhs the FMTMatrixBuild to swap with.
		*/
		void swap(FMTMatrixBuild& rhs);
		// DocString: FMTMatrixBuild::getColumnNames
		/**
		@brief Return the cached column names.
		@return a reference to the cached column names.
		*/
		inline std::vector<std::string>& getColumnNames()
			{
			return columnnames;
			}
		// DocString: FMTMatrixBuild::getRowNames
		/**
		@brief Return the cached row names.
		@return a reference to the cached row names.
		*/
		inline std::vector<std::string>& getRowNames()
			{
			return rownames;
			}
		// DocString: FMTMatrixBuild::formatAllNames
		/**
		@brief Format the cached row and column names.
		@param[in] shortformat if true uses a short format.
		*/
		void formatAllNames(bool shortformat);
		// DocString: FMTMatrixBuild::setColName
		/**
		@brief Set a column name.
		@param[in] name the name to set.
		@param[in] columnid the column id.
		*/
		void setColName(const std::string& name,const int& columnid);
		// DocString: FMTMatrixBuild::setRowName
		/**
		@brief Set a row name.
		@param[in] name the name to set.
		@param[in] rowid the row id.
		*/
		void setRowName(const std::string& name, const int& rowid);
		// DocString: FMTMatrixBuild::synchronize
		/**
		@brief Synchronize the cached columns and rows with the solver matrix.
		@param[in] solver the solver interface.
		*/
		void synchronize(std::shared_ptr<OsiSolverInterface> solver);
		// DocString: FMTMatrixBuild()
		/**
		@brief Default constructor for FMTMatrixBuild.
		*/
		FMTMatrixBuild();
		// DocString: FMTMatrixBuild(const FMTMatrixBuild)
		/**
		@brief Copy constructor for FMTMatrixBuild.
		@param[in] rhs the FMTMatrixBuild to copy.
		*/
		FMTMatrixBuild(const FMTMatrixBuild& rhs);
		// DocString: FMTMatrixBuild::operator=
		/**
		@brief Copy assignment operator for FMTMatrixBuild.
		@param[in] rhs the FMTMatrixBuild to copy.
		@return a reference to this FMTMatrixBuild.
		*/
		FMTMatrixBuild& operator = (const FMTMatrixBuild& rhs);
		// DocString: ~FMTMatrixBuild()
		/**
		@brief Destructor for FMTMatrixBuild.
		*/
		~FMTMatrixBuild();
		// DocString: FMTMatrixBuild::addCol
		/**
		@brief Add a column to the cache to potentially synchronize it later.
		@param[in] numberInColumn the number of elements in the column.
		@param[in] rows the row indices.
		@param[in] elements the elements.
		@param[in] columnLower the column lower bound.
		@param[in] columnUpper the column upper bound.
		@param[in] objectiveValue the objective value.
		*/
		void addCol(int numberInColumn, const int * rows, const double * elements, double columnLower = 0.0,
			double columnUpper = std::numeric_limits<double>::max(), double objectiveValue = 0.0);
		// DocString: FMTMatrixBuild::addRow
		/**
		@brief Add a row to the cache to potentially synchronize it later.
		@param[in] numberInRow the number of elements in the row.
		@param[in] columns the column indices.
		@param[in] elements the elements.
		@param[in] rowLower the row lower bound.
		@param[in] rowUpper the row upper bound.
		*/
		void addRow(int numberInRow, const int * columns, const double * elements,
			double rowLower = -std::numeric_limits<double>::max(), double rowUpper = std::numeric_limits<double>::max());
		// DocString: FMTMatrixBuild::deleteRow
		/**
		@brief Delete a row from the matrix cache.
		@param[in] rowindex the index of the row to delete.
		*/
		void deleteRow(const int& rowindex);
		// DocString: FMTMatrixBuild::deleteCol
		/**
		@brief Delete a column from the matrix cache.
		@param[in] colindex the index of the column to delete.
		*/
		void deleteCol(const int& colindex);
		// DocString: FMTMatrixBuild::numbernewRows
		/**
		@brief Return the number of newly added rows in the matrix cache.
		@return the number of newly added rows.
		*/
		inline int numbernewRows() const
			{
			return rowscount;
			}
		// DocString: FMTMatrixBuild::numbernewCols
		/**
		@brief Return the number of newly added columns in the matrix cache.
		@return the number of newly added columns.
		*/
		inline int numbernewCols() const
			{
			return colscount;
			}
		// DocString: FMTMatrixBuild::numberofdeletedRows
		/**
		@brief Return the number of newly deleted rows in the matrix cache.
		@return the number of newly deleted rows.
		*/
		inline int numberofdeletedRows() const
			{
			return static_cast<int>(deletedconstraints.size());
			}
		// DocString: FMTMatrixBuild::numberofdeletedCols
		/**
		@brief Return the number of newly deleted columns in the matrix cache.
		@return the number of newly deleted columns.
		*/
		inline int numberofdeletedCols() const
			{
			return static_cast<int>(deletedvariables.size());
			}
		// DocString: FMTMatrixBuild::getDeletedConstraints
		/**
		@brief Return the newly deleted rows in the matrix cache.
		@return the deleted constraints.
		*/
		inline const std::vector<int>& getDeletedConstraints() const
			{
			return deletedconstraints;
			}
		// DocString: FMTMatrixBuild::getDeletedVariables
		/**
		@brief Return the newly deleted columns in the matrix cache.
		@return the deleted variables.
		*/
		inline const std::vector<int>& getDeletedVariables() const
			{
			return deletedvariables;
			}
		// DocString: FMTMatrixBuild::sortAndCleanDeleted
		/**
		@brief Remove duplicates from the deleted rows and columns and sort them.
		*/
		void sortAndCleanDeleted();
		// DocString: FMTMatrixBuild::getRowsToSynchronize
		/**
		@brief Return a string of all the rows in the matrix cache, mainly for debugging.
		@return the rows to synchronize.
		*/
		std::string getRowsToSynchronize() const;
		// DocString: FMTMatrixBuild::getRowsToSynchronize
		/**
		@brief Return a string of all the columns in the matrix cache, mainly for debugging.
		@return the columns to synchronize.
		*/
		std::string getColsToSynchronize() const;
		// DocString: FMTMatrixBuild::getRow
		/**
		@brief Fill up the bounds, indices and elements of a given row present in the cache.
		@param[in] whichRow the row index.
		@param[out] rowLower the row lower bound.
		@param[out] rowUpper the row upper bound.
		@param[out] indices the row indices.
		@param[out] elements the row elements.
		@return the number of elements in the row.
		*/
		int getRow(int whichRow, double &rowLower, double &rowUpper,
			std::vector<int>& indices, std::vector<double>&elements) const;
		// DocString: FMTMatrixBuild::getCol
		/**
		@brief Fill up the bounds, objective, indices and elements of a given column present in the cache.
		@param[in] whichCol the column index.
		@param[out] colLower the column lower bound.
		@param[out] colUpper the column upper bound.
		@param[out] objectiveValue the objective value.
		@param[out] indices the column indices.
		@param[out] elements the column elements.
		@return the number of elements in the column.
		*/
		int getCol(int whichCol, double &colLower, double &colUpper, double &objectiveValue,
			std::vector<int>& indices, std::vector<double>&elements) const;


	};
}

#endif
#endif