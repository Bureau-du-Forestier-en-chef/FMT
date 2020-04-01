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
#ifdef FMTWITHOSI
#ifndef FMTserializablematrix_H_INCLUDED
#define FMTserializablematrix_H_INCLUDED

#include <CoinPackedMatrix.hpp>
#include <OsiSolverInterface.hpp>
#include <memory>
#include "FMTsolverinterface.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>


namespace Models
{
/**
The FMTserializablematrix is made for the serialization of osisolverinterface matrix.
Osisolverinterface matrix is a abstract class pointing on multiple solvertype.
The goal of that class is to get the informations from osisolverinterface class into multiple
vectors (solutions,bounds,etc...) to permit the synchronization.
Also this class is usefull when copying osisolverinterface with the FMTsolverinterface type.
*/

class FMTserializablematrix : public CoinPackedMatrix
	{
	///columns lower bound of the matrix
	std::vector<double>collb;
	///columns upper bound of the matrix
	std::vector<double>colub;
	///Objective coefficients for each column
	std::vector<double>obj;
	///Rows lower bound of the marix
	std::vector<double>rowlb;
	///Rows upper bound of the marix
	std::vector<double>rowub;
	///primal solution of the matrix
	std::vector<double>colsolution;
	///dual solution of the matrix
	std::vector<double>rowprice;
	///solver type used for abstraction.
	FMTsolverinterface solvertype;
	/**
	Save and load functions are for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		ar & BOOST_SERIALIZATION_NVP(colOrdered_);
		ar & BOOST_SERIALIZATION_NVP(extraGap_);
		ar & BOOST_SERIALIZATION_NVP(extraMajor_);
		ar & BOOST_SERIALIZATION_NVP(majorDim_);
		ar & BOOST_SERIALIZATION_NVP(minorDim_);
		ar & BOOST_SERIALIZATION_NVP(size_);
		ar & BOOST_SERIALIZATION_NVP(maxMajorDim_);
		ar & BOOST_SERIALIZATION_NVP(maxSize_);
		ar & BOOST_SERIALIZATION_NVP(collb);
		ar & BOOST_SERIALIZATION_NVP(colub);
		ar & BOOST_SERIALIZATION_NVP(obj);
		ar & BOOST_SERIALIZATION_NVP(rowlb);
		ar & BOOST_SERIALIZATION_NVP(rowub);
		ar & BOOST_SERIALIZATION_NVP(colsolution);
		ar & BOOST_SERIALIZATION_NVP(rowprice);
		ar & BOOST_SERIALIZATION_NVP(solvertype);
		if (Archive::is_loading::value)
			{
			if (maxMajorDim_ > 0)
				{
				length_ = new int[maxMajorDim_];
				}
			start_ = new CoinBigIndex[maxMajorDim_ + 1];
			if (maxSize_ > 0)
				{
				element_ = new double[maxSize_];
				index_ = new int[maxSize_];
				}
			}
		for (int indexid = 0; indexid < maxSize_; ++indexid)
			{
			ar & boost::serialization::make_nvp(("E" + std::to_string(indexid)).c_str(), element_[indexid]);
			ar & boost::serialization::make_nvp(("I"+ std::to_string(indexid)).c_str(), index_[indexid]);
			}
		for (int id = 0 ; id < maxMajorDim_; ++id)
			{
			ar & boost::serialization::make_nvp(("L" + std::to_string(id)).c_str(), length_[id]);
			}
		if(maxMajorDim_ > 0)
			{
			for (int id = 0; id < maxMajorDim_ + 1; ++id)
				{
				ar & boost::serialization::make_nvp(("S" + std::to_string(id)).c_str(), start_[id]);
				}
			}
		}
	public:
		/**
		Default constructor of FMTserializablematrix
		*/
		FMTserializablematrix();
		/**
		Copy constructor of FMTserializablematrix
		*/
		FMTserializablematrix(const FMTserializablematrix& rhs);
		/**
		Copy assignment of FMTserializablematrix
		*/
		FMTserializablematrix& operator = (const FMTserializablematrix& rhs);
		/**
		Constructor of FMTserializablematrix with the solverinterface and the solvertype used.
		Normaly used during the saving part of serialization.
		*/
		FMTserializablematrix(const std::shared_ptr<OsiSolverInterface>& solverinterface,const FMTsolverinterface& lsolvertype);
		/**
		Setter of the solvertype of the serializable matrix.
		*/
		void setsolvertype(FMTsolverinterface& lsolvertype) const;
		/**
		This function will set it's contain to a solverinterface matrix, used during the loading part of serialization.
		*/
		void setmatrix(std::shared_ptr<OsiSolverInterface>& solverinterface) const;
		/**
		Function used to build a shared pointer of a solverinterface passing the message handler to the pointer.
		*/
		std::shared_ptr<OsiSolverInterface> buildsolverinterface(const FMTsolverinterface& lsolvertype, CoinMessageHandler* handler) const;
		/**
		Function used to copy a shared pointer of a solverinterface passing the message handler to the pointer to a other shared pointer.
		*/
		std::shared_ptr<OsiSolverInterface> copysolverinterface(const std::shared_ptr<OsiSolverInterface>& solver_ptr, const FMTsolverinterface& lsolvertype, CoinMessageHandler* handler) const;
		/**
		Default destructor of FMTserializablematrix
		*/
		~FMTserializablematrix()=default;
	};

}

#endif
#endif