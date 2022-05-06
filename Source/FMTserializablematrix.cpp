/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
#include "FMTserializablematrix.hpp"
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

template<class Archive>
void FMTserializablematrix::gutsofserialize(Archive& ar, const unsigned int version)
{
	bool order;
	double extragap;
	double extramajor;
	int sizevector;
	int minordim;
	CoinBigIndex numelements;
	int majordim;
	CoinBigIndex maxsize;
	if (!Archive::is_loading::value)
		{
		order = matrix->isColOrdered();
		extragap = matrix->getExtraGap();
		extramajor = matrix->getExtraMajor();
		sizevector = matrix->getSizeVectorLengths();
		minordim = matrix->getMinorDim();
		numelements = matrix->getNumElements();
		majordim = matrix->getMaxMajorDim();
		maxsize = std::max(1,matrix->getNumElements());
		}
	ar & BOOST_SERIALIZATION_NVP(order);
	ar & BOOST_SERIALIZATION_NVP(extragap);
	ar & BOOST_SERIALIZATION_NVP(extramajor);
	ar & BOOST_SERIALIZATION_NVP(sizevector);
	ar & BOOST_SERIALIZATION_NVP(minordim);
	ar & BOOST_SERIALIZATION_NVP(numelements);
	ar & BOOST_SERIALIZATION_NVP(majordim);
	ar & BOOST_SERIALIZATION_NVP(maxsize);
	ar & BOOST_SERIALIZATION_NVP(collb);
	ar & BOOST_SERIALIZATION_NVP(colub);
	ar & BOOST_SERIALIZATION_NVP(obj);
	ar & BOOST_SERIALIZATION_NVP(rowlb);
	ar & BOOST_SERIALIZATION_NVP(rowub);
	ar & BOOST_SERIALIZATION_NVP(colsolution);
	ar & BOOST_SERIALIZATION_NVP(rowprice);
	double* element_;
	int *index_;
	int * length_;
	CoinBigIndex* start_;
	if (Archive::is_loading::value)
	{
		if (majordim > 0)
		{
			length_ = new int[majordim];
		}
		start_ = new CoinBigIndex[majordim + 1];
		if (maxsize > 0)
		{
			element_ = new double[maxsize];
			index_ = new int[maxsize];
		}
	}else {
		element_ = matrix->getMutableElements();
		index_ = matrix->getMutableIndices();
		length_ = matrix->getMutableVectorLengths();
		start_ = matrix->getMutableVectorStarts();
	}
	for (int indexid = 0; indexid < maxsize; ++indexid)
	{
		ar & boost::serialization::make_nvp(("E" + std::to_string(indexid)).c_str(), element_[indexid]);
		ar & boost::serialization::make_nvp(("I" + std::to_string(indexid)).c_str(), index_[indexid]);
	}
	for (int id = 0; id < majordim; ++id)
	{
		ar & boost::serialization::make_nvp(("L" + std::to_string(id)).c_str(), length_[id]);
	}
	if (majordim > 0)
	{
		for (int id = 0; id < majordim + 1; ++id)
		{
			ar & boost::serialization::make_nvp(("S" + std::to_string(id)).c_str(), start_[id]);
		}
	}
	if (Archive::is_loading::value)
		{
		matrix = std::unique_ptr<CoinPackedMatrix>(new CoinPackedMatrix(order,
			minordim, majordim, numelements,
			element_, index_,
			start_, length_,
			extramajor, extragap));
		delete length_;
		delete start_;
		delete element_;
		delete index_;
		}
}


template<> void FMTserializablematrix::serialize(boost::archive::binary_oarchive& ar, const unsigned int version)
	{
	gutsofserialize<boost::archive::binary_oarchive>(ar, version);
	}

template<> void FMTserializablematrix::serialize(boost::archive::binary_iarchive& ar, const unsigned int version)
	{
	gutsofserialize<boost::archive::binary_iarchive>(ar, version);
	}


}
#endif