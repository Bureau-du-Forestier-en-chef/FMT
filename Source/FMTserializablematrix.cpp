/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
#include "FMTserializablematrix.h"
#include <OsiSolverInterface.hpp>
#include <CoinPackedMatrix.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>

namespace Models
{

FMTserializablematrix::~FMTserializablematrix() = default;

FMTserializablematrix::FMTserializablematrix():
		matrix(new CoinPackedMatrix()),
		collb(),
		colub(),
		obj(),
		rowlb(),
		rowub(),
		colsolution(),
		rowprice()
	{

	}

FMTserializablematrix::FMTserializablematrix(const FMTserializablematrix& rhs): 
	matrix(new CoinPackedMatrix(*rhs.matrix)),
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
		matrix.reset(new CoinPackedMatrix(*rhs.matrix));
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

FMTserializablematrix::FMTserializablematrix(const std::shared_ptr<OsiSolverInterface>& solverinterface):
	matrix(new CoinPackedMatrix()),
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
			const CoinPackedMatrix* newmatrix = solverinterface->getMatrixByRow();
			*matrix = *newmatrix;
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


void FMTserializablematrix::setmatrix(std::shared_ptr<OsiSolverInterface>& solverinterface) const
	{
	if (!collb.empty())
		{
		solverinterface->loadProblem(*matrix, &collb[0], &colub[0], &obj[0], &rowlb[0], &rowub[0]);
		solverinterface->setColSolution(&colsolution[0]);
		solverinterface->setRowPrice(&rowprice[0]);
		}
	
	}

void FMTserializablematrix::getsetmatrixelements(bool loading,
	bool& order,
	double& extragap,
	double& extramajor,
	int& sizevector,
	int& minordim,
	int& numelements,
	int& majordim,
	int& maxsize,
	std::vector<double>& lelement,
	std::vector<int>& lindex,
	std::vector<int>& llength,
	std::vector<int>& lstart)
{
	if (!loading)
	{
		order = matrix->isColOrdered();
		extragap = matrix->getExtraGap();
		extramajor = matrix->getExtraMajor();
		sizevector = matrix->getSizeVectorLengths();
		minordim = matrix->getMinorDim();
		numelements = matrix->getNumElements();
		majordim = matrix->getMaxMajorDim();
		maxsize = std::max(1, matrix->getNumElements());
		lelement.resize(maxsize);
		lindex.resize(maxsize);
		llength.resize(majordim);
		lstart.resize(majordim + 1);
		const double* lelement_i = matrix->getMutableElements();
		const int*  lindex_i = matrix->getMutableIndices();
		const int*  llength_i = matrix->getMutableVectorLengths();
		const CoinBigIndex*  lstart_i = matrix->getMutableVectorStarts();
		size_t index = 0;
		for (double& le :lelement)
		{
			le = *(lelement_i + index);
			++index;
		}
		index = 0;
		for (int& li : lindex)
		{
			li = *(lindex_i + index);
			++index;
		}
		index = 0;
		for (int& ll : llength)
		{
			ll = *(llength_i + index);
			++index;
		}
		index = 0;
		for (int& ls : lstart)
		{
			ls = *(lstart_i + index);
			++index;
		}
	}else {
		matrix = std::unique_ptr<CoinPackedMatrix>(new CoinPackedMatrix(order,
			minordim, majordim, numelements,
			&lelement[0], &lindex[0],
			&lstart[0], &llength[0],
			extramajor, extragap));

	}

}


void FMTserializablematrix::getsetmemberelements(bool loading,
	std::vector<double>& lcollb,
	std::vector<double>& lcolub,
	std::vector<double>& lobj,
	std::vector<double>& lrowlb,
	std::vector<double>& lrowub,
	std::vector<double>& lcolsolution,
	std::vector<double>& lrowprice)
{
	if (!loading)
	{
		lcollb = collb;
		lcolub = colub;
		lobj = obj;
		lrowlb = rowlb;
		lrowub = rowub;
		lcolsolution = colsolution;
		lrowprice = rowprice;
	}else {
		collb = lcollb;
		colub = lcolub;
		obj = lobj;
		rowlb =lrowlb;
		rowub = lrowub;
		colsolution = lcolsolution;
		rowprice = lrowprice;
		}

}

}
#endif