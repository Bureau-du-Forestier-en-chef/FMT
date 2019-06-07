#ifndef FMTserializablematrix_H_INCLUDED
#define FMTserializablematrix_H_INCLUDED

#include <CoinPackedMatrix.hpp>
#include <OsiSolverInterface.hpp>
#include <memory>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

using namespace std;

namespace Graph
{

class FMTserializablematrix : public CoinPackedMatrix
	{
	vector<double>collb;
	vector<double>colub;
	vector<double>obj;
	vector<double>rowlb;
	vector<double>rowub;
	vector<double>colsolution;
	vector<double>rowprice;
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

		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "load mat elements" << "\n";
		for (int indexid = 0; indexid < maxSize_; ++indexid)
			{
			ar & boost::serialization::make_nvp(("E" + to_string(indexid)).c_str(), element_[indexid]);
			ar & boost::serialization::make_nvp(("I"+to_string(indexid)).c_str(), index_[indexid]);
			}
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "load mat maj dim" << "\n";
		for (int id = 0 ; id < maxMajorDim_; ++id)
			{
			ar & boost::serialization::make_nvp(("L" + to_string(id)).c_str(), length_[id]);
			}
		if(maxMajorDim_ > 0)
			{
			for (int id = 0; id < maxMajorDim_ + 1; ++id)
				{
				ar & boost::serialization::make_nvp(("S" + to_string(id)).c_str(), start_[id]);
				}
			}
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "ALLDONE!" << "\n";
		
		}
	public:
		FMTserializablematrix();
		FMTserializablematrix(const FMTserializablematrix& rhs);
		FMTserializablematrix& operator = (const FMTserializablematrix& rhs);
		FMTserializablematrix(const unique_ptr<OsiSolverInterface>& solverinterface);
		void setmatrix(unique_ptr<OsiSolverInterface>& solverinterface) const;
		~FMTserializablematrix()=default;
	};

}

#endif