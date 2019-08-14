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

#include "FMTserializablematrix.h"


namespace Graph
{

FMTserializablematrix::FMTserializablematrix():
		collb(),
		colub(),
		obj(),
		rowlb(),
		rowub(),
		colsolution(),
		rowprice()
	{

	}

FMTserializablematrix::FMTserializablematrix(const FMTserializablematrix& rhs): CoinPackedMatrix(*this),
	collb(rhs.collb),
	colub(rhs.colub),
	obj(rhs.obj),
	rowlb(rhs.rowlb),
	rowub(rhs.rowub),
	colsolution(rhs.colsolution),
	rowprice(rhs.rowprice)
	{

	}
FMTserializablematrix& FMTserializablematrix::operator = (const FMTserializablematrix& rhs)
	{
	if (this!=&rhs)
		{
		CoinPackedMatrix::operator=(rhs);
		collb = rhs.collb;
		colub = rhs.colub;
		obj = rhs.obj;
		rowlb = rhs.rowlb;
		rowub = rhs.rowub;
		colsolution = rhs.colsolution;
		rowprice = rhs.rowprice;
		}
	return *this;
	}

FMTserializablematrix::FMTserializablematrix(const unique_ptr<OsiSolverInterface>& solverinterface):
	CoinPackedMatrix(),
	collb(),
	colub(),
	obj(),
	rowlb(),
	rowub(),
	colsolution(),
	rowprice()
	{
	if (solverinterface->getNumCols()>0)
		{
			const CoinPackedMatrix* matrix = solverinterface->getMatrixByRow();
			CoinPackedMatrix::operator=(*matrix);
			size_t ncols = solverinterface->getNumCols();
			const double* col_lower = solverinterface->getColLower();
			const double* col_upper = solverinterface->getColUpper();
			const double * col_obj = solverinterface->getObjCoefficients();
			const double* col_sol = solverinterface->getColSolution();
			collb.resize(ncols);
			colub.resize(ncols);
			obj.resize(ncols);
			colsolution.resize(ncols);
			for (size_t colid = 0; colid < ncols; ++colid)
				{
				collb[colid] = *(col_lower + colid);
				colub[colid] = *(col_upper + colid);
				obj[colid] = *(col_obj + colid);
				colsolution[colid] = *(col_sol + colid);
				}
			size_t nrows = solverinterface->getNumRows();
			rowlb.resize(nrows);
			rowub.resize(nrows);
			rowprice.resize(nrows);
			const double* row_lower = solverinterface->getRowLower();
			const double* row_upper = solverinterface->getRowUpper();
			const double* row_solution = solverinterface->getRowPrice();
			for (size_t rowid = 0; rowid < nrows; ++rowid)
				{
				rowlb[rowid] = *(row_lower + rowid);
				rowub[rowid] = *(row_upper + rowid);
				rowprice[rowid] = *(row_solution + rowid);
				}
		}
	}

void FMTserializablematrix::setmatrix(unique_ptr<OsiSolverInterface>& solverinterface) const
	{
	/*CoinPackedMatrix pcmat(*this);
	vector<double>tcollb(collb);
	vector<double>tcolub(collb);
	vector<double>tobj(collb);
	vector<double>trowlb(collb);
	vector<double>trowub(collb);
	solverinterface->loadProblem(pcmat, &tcollb[0], &tcolub[0], &tobj[0], &trowlb[0], &trowub[0]);*/
	if (!collb.empty())
		{
		solverinterface->loadProblem(*this, &collb[0], &colub[0], &obj[0], &rowlb[0], &rowub[0]);
		solverinterface->setColSolution(&colsolution[0]);
		solverinterface->setRowPrice(&rowprice[0]);
		}
	
	}


}
