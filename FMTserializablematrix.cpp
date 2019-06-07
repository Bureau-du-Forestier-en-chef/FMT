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
