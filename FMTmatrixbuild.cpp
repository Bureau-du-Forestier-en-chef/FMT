/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI

#include "FMTmatrixbuild.h"
#include <algorithm>

namespace Models

{

	void FMTmatrixbuild::sortelementsandclean(std::vector<int>& elements) const
		{
		std::sort(elements.begin(), elements.end());
		elements.erase(std::unique(elements.begin(), elements.end()), elements.end());
		}

	void FMTmatrixbuild::sortandcleandeleted()
		{
		sortelementsandclean(deletedconstraints);
		sortelementsandclean(deletedvariables);
		}

	int FMTmatrixbuild::getrow(int whichRow, double &rowLower, double &rowUpper,
		std::vector<int>& indices, std::vector<double>&elements) const 
		{
		const int * indicesarray;
		const double * elementsarray;
		int sizeofrow = rowsbuild.row(whichRow, rowLower, rowUpper, indicesarray, elementsarray);
		indices.reserve(sizeofrow);
		elements.reserve(sizeofrow);
		indices.insert(indices.end(), indicesarray, indicesarray + sizeofrow);
		elements.insert(elements.end(), elementsarray, elementsarray + sizeofrow);
		//need to delete arrays?
		return sizeofrow;
		}

	int FMTmatrixbuild::getcol(int whichCol, double &colLower, double &colUpper, double &objectiveValue,
		std::vector<int>& indices, std::vector<double>&elements) const
		{
		const int * indicesarray;
		const double * elementsarray;
		int sizeofcol = rowsbuild.column(whichCol, colLower, colUpper, objectiveValue, indicesarray, elementsarray);
		indices.reserve(sizeofcol);
		elements.reserve(sizeofcol);
		indices.insert(indices.end(), indicesarray, indicesarray + sizeofcol);
		elements.insert(elements.end(), elementsarray, elementsarray + sizeofcol);
		//need to delete arrays?
		return sizeofcol;
		}

	void FMTmatrixbuild::synchronize(std::shared_ptr<OsiSolverInterface> solver)
		{
		if (!deletedconstraints.empty())
			{
			sortelementsandclean(deletedconstraints);
			solver->deleteRows(static_cast<int>(deletedconstraints.size()), &deletedconstraints[0]);
			deletedconstraints.clear();
			}
		if (!deletedvariables.empty())
			{
			sortelementsandclean(deletedvariables);
			solver->deleteCols(static_cast<int>(deletedvariables.size()), &deletedvariables[0]);
			deletedvariables.clear();
			}
		if (colsbuild.numberColumns()>0)
			{
			solver->addCols(colsbuild);
			colsbuild = CoinBuild(1);
			colscount = 0;
			}
		if (rowsbuild.numberRows()>0)
			{
			solver->addRows(rowsbuild);
			rowsbuild = CoinBuild(0);
			rowscount = 0;
			}
		}

	void FMTmatrixbuild::deleteRow(const int& rowindex)
		{
		deletedconstraints.push_back(rowindex);
		}
	void FMTmatrixbuild::deleteCol(const int& colindex)
		{
		deletedvariables.push_back(colindex);
		}


	FMTmatrixbuild::FMTmatrixbuild() : colscount(0), rowscount(0), colsbuild(1),
		rowsbuild(0), deletedconstraints(),deletedvariables()
		{

		}

	FMTmatrixbuild::FMTmatrixbuild(const FMTmatrixbuild& rhs) : colscount(rhs.colscount), rowscount(rhs.rowscount), colsbuild(rhs.colsbuild),
		rowsbuild(rhs.rowsbuild), deletedconstraints(rhs.deletedconstraints), deletedvariables(rhs.deletedvariables)
		{

		}


	FMTmatrixbuild& FMTmatrixbuild::operator = (const FMTmatrixbuild& rhs)
		{
		if (this!=&rhs)
			{
			colscount = rhs.colscount;
			rowscount = rhs.rowscount;
			colsbuild = rhs.colsbuild;
			rowsbuild = rhs.rowsbuild;
			deletedconstraints = rhs.deletedconstraints;
			deletedvariables = rhs.deletedvariables;
			}
		return *this;
		}

	std::string FMTmatrixbuild::getrowstosynchronize() const
		{
		std::string rows;
		for (int irow =0; irow< rowsbuild.numberRows();++irow)
			{
			double lowerbound,upperbound;
			const int* columns;
			const double* elements;
			int numberofelements = rowsbuild.row(irow, lowerbound, upperbound, columns, elements);
			double maximumcolid = 0;
			double minimumcolid = COIN_INT_MAX;
			double maximumparemeter=0;
			double minimumparemeter= COIN_INT_MAX;
			for (int nelement = 0; nelement < numberofelements;++nelement)
				{
				if (columns[nelement]> maximumcolid)
					{
					maximumcolid = columns[nelement];
					}
				if (columns[nelement] < minimumcolid)
					{
					minimumcolid = columns[nelement];
					}
				if (elements[nelement] > maximumparemeter)
					{
					maximumparemeter = elements[nelement];
					}

				if (elements[nelement] < minimumparemeter)
					{
					minimumparemeter = elements[nelement];
					}
				}
			rows += "maxi"+ std::to_string(maximumcolid) +
				" mini" + std::to_string(minimumcolid) +
				" maxp" + std::to_string(maximumparemeter) +
				" minp" + std::to_string(minimumparemeter) +
				" L" + std::to_string(lowerbound) +
				" U" + std::to_string(upperbound);
			}
		rows += "\n";
		return rows;
		}
	std::string FMTmatrixbuild::getcolstosynchronize() const
		{
		std::string cols;
		for (int icol = 0; icol < colsbuild.numberColumns(); ++icol)
		{
			double lowerbound, upperbound,objective;
			const int* rows;
			const double* elements;
			int numberofelements = colsbuild.column(icol, lowerbound, upperbound,objective,rows, elements);
			for (int nelement = 0; nelement < numberofelements; ++nelement)
			{
				double maximumrowid = 0;
				double minimumrowid = COIN_INT_MAX;
				double maximumparemeter = 0;
				double minimumparemeter = COIN_INT_MAX;
				for (int nelement = 0; nelement < numberofelements; ++nelement)
				{
					if (rows[nelement] > maximumrowid)
					{
						maximumrowid = rows[nelement];
					}
					if (rows[nelement] < minimumrowid)
					{
						minimumrowid = rows[nelement];
					}
					if (elements[nelement] > maximumparemeter)
					{
						maximumparemeter = elements[nelement];
					}

					if (elements[nelement] < minimumparemeter)
					{
						minimumparemeter = elements[nelement];
					}
				}
				cols += "maxi" + std::to_string(maximumrowid) +
					" mini" + std::to_string(minimumrowid) +
					" maxp" + std::to_string(maximumparemeter) +
					" minp" + std::to_string(minimumparemeter) +
					" L" + std::to_string(lowerbound) +
					" U" + std::to_string(upperbound)+
					" O" + std::to_string(objective);
			}
		}
		cols += "\n";
		return cols;
		}


}

#endif