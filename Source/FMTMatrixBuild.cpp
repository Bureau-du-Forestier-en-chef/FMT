/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI

#include "FMTMatrixBuild.h"
#include "FMTDefaultExceptionHandler.h"
#include "FMTError.h"
#include <algorithm>
#include <boost/algorithm/string.hpp> 
#include "OsiSolverInterface.hpp"
#include "CoinBuild.hpp"


namespace Models

{
	FMTMatrixBuild::~FMTMatrixBuild() = default;

	std::string FMTMatrixBuild::_formatForMatrixName(std::string name,bool shortformat) const
	{
		std::string to = "_";
		if (shortformat)
			{
			to = "";
			}
		boost::replace_all(name," ",to);
		return name;
	}

	void FMTMatrixBuild::_sortElementsAndClean(std::vector<int>& elements) const
		{
		std::sort(elements.begin(), elements.end());
		elements.erase(std::unique(elements.begin(), elements.end()), elements.end());
		}

	void FMTMatrixBuild::sortAndCleanDeleted()
		{
		_sortElementsAndClean(m_deletedconstraints);
		_sortElementsAndClean(m_deletedvariables);
		}

	int FMTMatrixBuild::getRow(int whichRow, double &rowLower, double &rowUpper,
		std::vector<int>& indices, std::vector<double>&elements) const 
		{
		const int * indicesarray;
		const double * elementsarray;
		int sizeofrow = m_rowsbuild->row(whichRow, rowLower, rowUpper, indicesarray, elementsarray);
		indices.reserve(sizeofrow);
		elements.reserve(sizeofrow);
		indices.insert(indices.end(), indicesarray, indicesarray + sizeofrow);
		elements.insert(elements.end(), elementsarray, elementsarray + sizeofrow);
		//need to delete arrays?
		return sizeofrow;
		}

	int FMTMatrixBuild::getCol(int whichCol, double &colLower, double &colUpper, double &objectiveValue,
		std::vector<int>& indices, std::vector<double>&elements) const
		{
		const int * indicesarray;
		const double * elementsarray;
		int sizeofcol = m_colsbuild->column(whichCol, colLower, colUpper, objectiveValue, indicesarray, elementsarray);
		indices.reserve(sizeofcol);
		elements.reserve(sizeofcol);
		indices.insert(indices.end(), indicesarray, indicesarray + sizeofcol);
		elements.insert(elements.end(), elementsarray, elementsarray + sizeofcol);
		//need to delete arrays?
		return sizeofcol;
		}

	void FMTMatrixBuild::formatAllNames(bool shortformat)
		{
		int colid = 0;
		for (std::string& colname : m_columnnames)
			{
			if (colname.empty())
				{
				colname = "C" + std::to_string(colid);
				}
			colname = _formatForMatrixName(colname, shortformat);
			++colid;
			}
		int rowid = 0;
		for (std::string& rowname : m_rownames)
			{
			if (rowname.empty())
				{
				rowname = "R" + std::to_string(rowid);
				}
			rowname = _formatForMatrixName(rowname, shortformat);
			++rowid;
			}

		}


	void FMTMatrixBuild::setColName(const std::string& name, const int& columnid)
		{
		const size_t sizeofcontainer = static_cast<size_t>(columnid + 1);
		if (sizeofcontainer>m_columnnames.size())
			{
			m_columnnames.resize(sizeofcontainer);
			}
		m_columnnames[columnid] = name;
		}


	void FMTMatrixBuild::setRowName(const std::string& name, const int& rowid)
		{
		const size_t sizeofcontainer = static_cast<size_t>(rowid + 1);
		if (sizeofcontainer > m_rownames.size())
		{
			m_rownames.resize(sizeofcontainer);
		}
		m_rownames[rowid] = name;
		}




	void FMTMatrixBuild::synchronize(std::shared_ptr<OsiSolverInterface> solver)
		{
		try {
			
			if (!m_deletedconstraints.empty())
			{
				_sortElementsAndClean(m_deletedconstraints);
				solver->deleteRows(static_cast<int>(m_deletedconstraints.size()), &m_deletedconstraints[0]);
				m_deletedconstraints.clear();
			}
			if (!m_deletedvariables.empty())
			{
				_sortElementsAndClean(m_deletedvariables);
				solver->deleteCols(static_cast<int>(m_deletedvariables.size()), &m_deletedvariables[0]);
				m_deletedvariables.clear();
			}
			if (m_colsbuild->numberColumns() > 0)
			{
				solver->addCols(*m_colsbuild);
				*m_colsbuild = CoinBuild(1);
				m_colscount = 0;
			}
			if (m_rowsbuild->numberRows() > 0)
			{
				solver->addRows(*m_rowsbuild);
				*m_rowsbuild = CoinBuild(0);
				m_rowscount = 0;
			}
			}catch (...)
			{
				Exception::FMTDefaultExceptionHandler().raiseFromCatch("", "FMTMatrixBuild::synchronize", __LINE__, __FILE__);
			}


		}

	void FMTMatrixBuild::deleteRow(const int& rowindex)
		{
		m_deletedconstraints.push_back(rowindex);
		}
	void FMTMatrixBuild::deleteCol(const int& colindex)
		{
		m_deletedvariables.push_back(colindex);
		}


	FMTMatrixBuild::FMTMatrixBuild() : m_colscount(0), m_rowscount(0), m_colsbuild( new CoinBuild(1)),
		m_rowsbuild(new CoinBuild(0)), m_deletedconstraints(),m_deletedvariables()
		{

		}

	FMTMatrixBuild::FMTMatrixBuild(const FMTMatrixBuild& rhs) : m_colscount(rhs.m_colscount), m_rowscount(rhs.m_rowscount), m_colsbuild(new CoinBuild(*rhs.m_colsbuild)),
		m_rowsbuild(new CoinBuild(*rhs.m_rowsbuild)), m_deletedconstraints(rhs.m_deletedconstraints), m_deletedvariables(rhs.m_deletedvariables)
		{

		}

	void FMTMatrixBuild::swap(FMTMatrixBuild& rhs)
	{
		m_colscount=rhs.m_colscount;
		m_rowscount = rhs.m_rowscount;
		m_colsbuild.swap(rhs.m_colsbuild);
		m_rowsbuild.swap(rhs.m_rowsbuild);
		m_deletedconstraints.swap(rhs.m_deletedconstraints);
		m_deletedvariables.swap(rhs.m_deletedvariables);
		m_columnnames.swap(rhs.m_columnnames);
		m_rownames.swap(rhs.m_rownames);
	}


	FMTMatrixBuild& FMTMatrixBuild::operator = (const FMTMatrixBuild& rhs)
		{
		if (this!=&rhs)
			{
			m_colscount = rhs.m_colscount;
			m_rowscount = rhs.m_rowscount;
			m_colsbuild.reset(new CoinBuild(*rhs.m_colsbuild));
			m_rowsbuild.reset(new CoinBuild(*rhs.m_rowsbuild));
			m_deletedconstraints = rhs.m_deletedconstraints;
			m_deletedvariables = rhs.m_deletedvariables;
			m_columnnames = rhs.m_columnnames;
			m_rownames = rhs.m_rownames;
			}
		return *this;
		}

	std::string FMTMatrixBuild::getRowsToSynchronize() const
		{
		std::string rows;
		for (int irow =0; irow< m_rowsbuild->numberRows();++irow)
			{
			double lowerbound,upperbound;
			const int* columns;
			const double* elements;
			int numberofelements = m_rowsbuild->row(irow, lowerbound, upperbound, columns, elements);
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
	std::string FMTMatrixBuild::getColsToSynchronize() const
		{
		std::string cols;
		for (int icol = 0; icol < m_colsbuild->numberColumns(); ++icol)
		{
			double lowerbound, upperbound,objective;
			const int* rows;
			const double* elements;
			int numberofelements = m_colsbuild->column(icol, lowerbound, upperbound,objective,rows, elements);
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

	void FMTMatrixBuild::addCol(int numberInColumn, const int * rows, const double * elements, double columnLower,
		double columnUpper, double objectiveValue)
	{
		m_colsbuild->addCol(numberInColumn, rows, elements, columnLower,
			columnUpper, objectiveValue);
		++m_colscount;
	}

	void FMTMatrixBuild::addRow(int numberInRow, const int * columns, const double * elements, double rowLower, double rowUpper)
	{
		m_rowsbuild->addRow(numberInRow, columns, elements, rowLower, rowUpper);
		++m_rowscount;
	}


}

#endif