/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
#ifndef FMTserializablematrix_Hm_included
#define FMTserializablematrix_Hm_included


#include <memory>
#include <vector>
#include "FMTutility.h"
#include <boost/serialization/vector.hpp>

class OsiSolverInterface;
class CoinPackedMatrix;

namespace Models
{
// DocString: FMTSerializableMatrix
/**
The FMTSerializableMatrix is made for the serialization of osisolverinterface matrix.
Osisolverinterface matrix is a abstract class pointing on multiple solvertype.
The goal of that class is to get the informations from osisolverinterface class into multiple
vectors (solutions,bounds,etc...) to permit the synchronization.
Also this class is usefull when copying osisolverinterface with the FMTsolverinterface type.
*/
class FMTEXPORT FMTSerializableMatrix
	{
	// DocString: FMTSerializableMatrix::matrix
	///The matrix pointer
	std::unique_ptr<CoinPackedMatrix> matrix;
	// DocString: FMTSerializableMatrix::collb
	///columns lower bound of the matrix
	std::vector<double>collb;
	// DocString: FMTSerializableMatrix::colub
	///columns upper bound of the matrix
	std::vector<double>colub;
	// DocString: FMTSerializableMatrix::obj
	///Objective coefficients for each column
	std::vector<double>obj;
	// DocString: FMTSerializableMatrix::rowlb
	///Rows lower bound of the marix
	std::vector<double>rowlb;
	// DocString: FMTSerializableMatrix::rowub
	///Rows upper bound of the marix
	std::vector<double>rowub;
	// DocString: FMTSerializableMatrix::colsolution
	///primal solution of the matrix
	std::vector<double>colsolution;
	// DocString: FMTSerializableMatrix::rowprice
	///dual solution of the matrix
	std::vector<double>rowprice;
	// DocString: FMTSerializableMatrix::serialize
	/**
	Save and load functions are for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		const bool loading = Archive::is_loading::value;
		bool order;
		double extragap;
		double extramajor;
		int sizevector;
		int minordim;
		int numelements;
		int majordim;
		int maxsize;
		std::vector<double> lelement;
		std::vector<int> lindex;
		std::vector<int> llength;
		std::vector<int> lstart;
		//members
		std::vector<double> lcollb;
		std::vector<double> lcolub;
		std::vector<double> lobj;
		std::vector<double> lrowlb;
		std::vector<double> lrowub;
		std::vector<double> lcolsolution;
		std::vector<double> lrowprice;
		if (!loading)
		{
			getSetMatrixElements(false,
				order,
				extragap,
				extramajor,
				sizevector,
				minordim,
				numelements,
				majordim,
				maxsize,
				lelement,
				lindex,
				llength,
				lstart);
			getSetMemberElements(false,
				lcollb,
				lcolub,
				lobj,
				lrowlb,
				lrowub,
				lcolsolution,
				lrowprice);
		}
		ar&order;
		ar& extragap;
		ar& extramajor;
		ar& sizevector;
		ar& minordim;
		ar& numelements;
		ar& majordim;
		ar& maxsize;
		ar& lelement;
		ar& lindex;
		ar& llength;
		ar& lstart;
		//members
		ar& lcollb;
		ar& lcolub;
		ar& lobj;
		ar& lrowlb;
		ar& lrowub;
		ar& lcolsolution;
		ar& lrowprice;
		if (loading)
			{
			getSetMatrixElements(true,
				order,
				extragap,
				extramajor,
				sizevector,
				minordim,
				numelements,
				majordim,
				maxsize,
				lelement,
				lindex,
				llength,
				lstart);
			getSetMemberElements(true,
				lcollb,
				lcolub,
				lobj,
				lrowlb,
				lrowub,
				lcolsolution,
				lrowprice);
			}
		

		}
	void getSetMatrixElements(bool loading,
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
		std::vector<int>& lstart);
	void getSetMemberElements(bool loading,
		std::vector<double>&lcollb,
		std::vector<double>&lcolub,
		std::vector<double>&lobj,
		std::vector<double>&lrowlb,
		std::vector<double>&lrowub,
		std::vector<double>&lcolsolution,
		std::vector<double>&lrowprice);
	public:
		// DocString: FMTSerializableMatrix()
		/**
		Default constructor of FMTSerializableMatrix
		*/
		FMTSerializableMatrix();
		// DocString: FMTSerializableMatrix(const FMTSerializableMatrix)
		/**
		Copy constructor of FMTSerializableMatrix
		*/
		FMTSerializableMatrix(const FMTSerializableMatrix& rhs);
		// DocString: FMTSerializableMatrix::operator=
		/**
		Copy assignment of FMTSerializableMatrix
		*/
		FMTSerializableMatrix& operator = (const FMTSerializableMatrix& rhs);
		// DocString: FMTSerializableMatrix(const std::shared_ptr<OsiSolverInterface>,const FMTsolverinterface)
		/**
		Constructor of FMTSerializableMatrix with the solverinterface and the solvertype used.
		Normaly used during the saving part of serialization.
		*/
		FMTSerializableMatrix(const std::shared_ptr<OsiSolverInterface>& solverinterface);
		// DocString: FMTSerializableMatrix::setSolverType
		/**
		Setter of the solvertype of the serializable matrix.
		*/
		//void setSolverType(FMTsolverinterface& lsolvertype) const;
		// DocString: FMTSerializableMatrix::setMatrix
		/**
		This function will set it's contain to a solverinterface matrix, used during the loading part of serialization.
		*/
		void setMatrix(std::shared_ptr<OsiSolverInterface>& solverinterface) const;
		// DocString: FMTSerializableMatrix::buildSolverInterface
		/**
		Function used to build a shared pointer of a solverinterface passing the message handler to the pointer.
		*/
		//std::shared_ptr<OsiSolverInterface> buildSolverInterface(const FMTsolverinterface& lsolvertype, CoinMessageHandler* handler) const;
		// DocString: FMTSerializableMatrix::copySolverInterface
		/**
		Function used to copy a shared pointer of a solverinterface passing the message handler to the pointer to a other shared pointer.
		*/
		//std::shared_ptr<OsiSolverInterface> copySolverInterface(const std::shared_ptr<OsiSolverInterface>& solver_ptr, const FMTsolverinterface& lsolvertype, CoinMessageHandler* handler) const;
		// DocString: ~FMTSerializableMatrix()
		/**
		Default destructor of FMTSerializableMatrix
		*/
		~FMTSerializableMatrix();
	};

}

#endif
#endif