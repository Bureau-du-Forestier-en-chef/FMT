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
@brief Helper for the serialization of an OSI solver interface matrix.
@details Extracts the information from the OSI solver interface matrix into vectors (solutions, bounds, etc.) to allow synchronization, also useful when copying an OSI solver interface with the FMTSolverInterface type.
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
	@brief Serialize the matrix by extracting the OSI solver interface matrix into vectors, for multiprocessing across multiple cpus (pickle in Python).
	@tparam Archive the archive type.
	@param[in,out] ar the archive to serialize to or from.
	@param[in] version the serialization version.
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
	// DocString: FMTSerializableMatrix::getSetMatrixElements
	/**
	@brief Get or set the matrix elements to or from the given variables depending on the loading flag.
	@param[in] loading if true loads the elements, otherwise gets them.
	@param[in,out] order the storage order.
	@param[in,out] extragap the extra gap.
	@param[in,out] extramajor the extra major.
	@param[in,out] sizevector the size of the vectors.
	@param[in,out] minordim the minor dimension.
	@param[in,out] numelements the number of elements.
	@param[in,out] majordim the major dimension.
	@param[in,out] maxsize the maximum size.
	@param[in,out] lelement the elements.
	@param[in,out] lindex the indices.
	@param[in,out] llength the lengths.
	@param[in,out] lstart the starts.
	*/
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
	// DocString: FMTSerializableMatrix::getSetMemberElements
	/**
	@brief Get or set the member elements to or from the given vectors depending on the loading flag.
	@param[in] loading if true loads the elements, otherwise gets them.
	@param[in,out] lcollb the columns lower bounds.
	@param[in,out] lcolub the columns upper bounds.
	@param[in,out] lobj the objective coefficients.
	@param[in,out] lrowlb the rows lower bounds.
	@param[in,out] lrowub the rows upper bounds.
	@param[in,out] lcolsolution the primal solution.
	@param[in,out] lrowprice the dual solution.
	*/
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
		@brief Default constructor for FMTSerializableMatrix.
		*/
		FMTSerializableMatrix();
		// DocString: FMTSerializableMatrix(const FMTSerializableMatrix)
		/**
		@brief Copy constructor for FMTSerializableMatrix.
		@param[in] rhs the FMTSerializableMatrix to copy.
		*/
		FMTSerializableMatrix(const FMTSerializableMatrix& rhs);
		// DocString: FMTSerializableMatrix::operator=
		/**
		@brief Copy assignment operator for FMTSerializableMatrix.
		@param[in] rhs the FMTSerializableMatrix to copy.
		@return a reference to this FMTSerializableMatrix.
		*/
		FMTSerializableMatrix& operator = (const FMTSerializableMatrix& rhs);
		// DocString: FMTSerializableMatrix(const std::shared_ptr<OsiSolverInterface>,const FMTSolverInterface)
		/**
		@brief Construct a serializable matrix from a solver interface, normally used during the saving part of serialization.
		@param[in] solverinterface the solver interface.
		*/
		FMTSerializableMatrix(const std::shared_ptr<OsiSolverInterface>& solverinterface);
		// DocString: FMTSerializableMatrix::setSolverType
		/**
		Setter of the solvertype of the serializable matrix.
		*/
		//void setSolverType(FMTSolverInterface& lsolvertype) const;
		// DocString: FMTSerializableMatrix::setMatrix
		/**
		@brief Set the content of a solver interface matrix, used during the loading part of serialization.
		@param[in] solverinterface the solver interface.
		*/
		void setMatrix(std::shared_ptr<OsiSolverInterface>& solverinterface) const;
		// DocString: FMTSerializableMatrix::buildSolverInterface
		/**
		Function used to build a shared pointer of a solverinterface passing the message handler to the pointer.
		*/
		//std::shared_ptr<OsiSolverInterface> buildSolverInterface(const FMTSolverInterface& lsolvertype, CoinMessageHandler* handler) const;
		// DocString: FMTSerializableMatrix::copySolverInterface
		/**
		Function used to copy a shared pointer of a solverinterface passing the message handler to the pointer to a other shared pointer.
		*/
		//std::shared_ptr<OsiSolverInterface> copySolverInterface(const std::shared_ptr<OsiSolverInterface>& solver_ptr, const FMTSolverInterface& lsolvertype, CoinMessageHandler* handler) const;
		// DocString: ~FMTSerializableMatrix()
		/**
		@brief Destructor for FMTSerializableMatrix.
		*/
		~FMTSerializableMatrix();
	};

}

#endif
#endif