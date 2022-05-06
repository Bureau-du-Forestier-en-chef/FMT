/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
#ifndef FMTserializablematrix_H_INCLUDED
#define FMTserializablematrix_H_INCLUDED


#include <memory>
#include <vector>
#include "FMTutility.hpp"

class OsiSolverInterface;
class CoinPackedMatrix;

namespace Models
{
// DocString: FMTserializablematrix
/**
The FMTserializablematrix is made for the serialization of osisolverinterface matrix.
Osisolverinterface matrix is a abstract class pointing on multiple solvertype.
The goal of that class is to get the informations from osisolverinterface class into multiple
vectors (solutions,bounds,etc...) to permit the synchronization.
Also this class is usefull when copying osisolverinterface with the FMTsolverinterface type.
*/
class FMTserializablematrix
	{
	// DocString: FMTserializablematrix::matrix
	///The matrix pointer
	std::unique_ptr<CoinPackedMatrix> matrix;
	// DocString: FMTserializablematrix::collb
	///columns lower bound of the matrix
	std::vector<double>collb;
	// DocString: FMTserializablematrix::colub
	///columns upper bound of the matrix
	std::vector<double>colub;
	// DocString: FMTserializablematrix::obj
	///Objective coefficients for each column
	std::vector<double>obj;
	// DocString: FMTserializablematrix::rowlb
	///Rows lower bound of the marix
	std::vector<double>rowlb;
	// DocString: FMTserializablematrix::rowub
	///Rows upper bound of the marix
	std::vector<double>rowub;
	// DocString: FMTserializablematrix::colsolution
	///primal solution of the matrix
	std::vector<double>colsolution;
	// DocString: FMTserializablematrix::rowprice
	///dual solution of the matrix
	std::vector<double>rowprice;
	// DocString: FMTserializablematrix::serialize
	/**
	Save and load functions are for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version);
	// DocString: FMTserializablematrix::gutsofserialize
	/**
	The real serialization is here.
	*/
	template<class Archive>
	void gutsofserialize(Archive& ar, const unsigned int version);
	public:
		// DocString: FMTserializablematrix()
		/**
		Default constructor of FMTserializablematrix
		*/
		FMTserializablematrix();
		// DocString: FMTserializablematrix(const FMTserializablematrix)
		/**
		Copy constructor of FMTserializablematrix
		*/
		FMTserializablematrix(const FMTserializablematrix& rhs);
		// DocString: FMTserializablematrix::operator=
		/**
		Copy assignment of FMTserializablematrix
		*/
		FMTserializablematrix& operator = (const FMTserializablematrix& rhs);
		// DocString: FMTserializablematrix(const std::shared_ptr<OsiSolverInterface>,const FMTsolverinterface)
		/**
		Constructor of FMTserializablematrix with the solverinterface and the solvertype used.
		Normaly used during the saving part of serialization.
		*/
		FMTserializablematrix(const std::shared_ptr<OsiSolverInterface>& solverinterface);
		// DocString: FMTserializablematrix::setsolvertype
		/**
		Setter of the solvertype of the serializable matrix.
		*/
		//void setsolvertype(FMTsolverinterface& lsolvertype) const;
		// DocString: FMTserializablematrix::setmatrix
		/**
		This function will set it's contain to a solverinterface matrix, used during the loading part of serialization.
		*/
		void setmatrix(std::shared_ptr<OsiSolverInterface>& solverinterface) const;
		// DocString: FMTserializablematrix::buildsolverinterface
		/**
		Function used to build a shared pointer of a solverinterface passing the message handler to the pointer.
		*/
		//std::shared_ptr<OsiSolverInterface> buildsolverinterface(const FMTsolverinterface& lsolvertype, CoinMessageHandler* handler) const;
		// DocString: FMTserializablematrix::copysolverinterface
		/**
		Function used to copy a shared pointer of a solverinterface passing the message handler to the pointer to a other shared pointer.
		*/
		//std::shared_ptr<OsiSolverInterface> copysolverinterface(const std::shared_ptr<OsiSolverInterface>& solver_ptr, const FMTsolverinterface& lsolvertype, CoinMessageHandler* handler) const;
		// DocString: ~FMTserializablematrix()
		/**
		Default destructor of FMTserializablematrix
		*/
		~FMTserializablematrix();
	};

}

#endif
#endif