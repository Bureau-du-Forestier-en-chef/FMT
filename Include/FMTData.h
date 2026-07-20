/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTDATA_Hm_included
#define FMTDATA_Hm_included


#include <vector>
#include <string>
#include <map>
#include "FMTutility.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/export.hpp>
#include <memory>

namespace Core
{
class FMTDevelopment;
class FMTYieldRequest;
class FMTExpression;
class FMTMask;

// DocString: FMTData
/**
@brief Data of a yield holding the values, sources and operators used to compute complex yields.
*/
class FMTEXPORT FMTData
    {
    public:
		std::vector<double> data;
        // DocString: FMTData()
        /**
        @brief Default constructor for FMTData.
        */
        FMTData();
		// DocString: ~FMTData()
		/**
		@brief Default destructor for FMTData.
		*/
		~FMTData()=default;
        // DocString: FMTData(const FMTData&)
        /**
        @brief Copy constructor for FMTData.
        @param[in] rhs the FMTData to copy.
        */
        FMTData(const FMTData& rhs);
		// DocString: FMTData(const std::vector<double>&,const FMTyieldparserop&,const std::vector<std::string>&)
		/**
		@brief Construct a FMTData from values, an operator and sources.
		@param[in] lvalues the values.
		@param[in] lops the yield parser operator.
		@param[in] lsource the sources.
		*/
		FMTData(const std::vector<double>& lvalues,
			const FMTyieldparserop& lops,
			const std::vector<std::string>& lsource);
        // DocString: FMTData(const std::vector<double>&,const FMTyieldparserop&,const std::vector<std::string>&,const std::vector<bool>&)
        /**
        @brief Construct a FMTData from values, an operator, sources and a stacking.
        @param[in] lvalues the values.
        @param[in] lops the yield parser operator.
        @param[in] lsource the sources.
        @param[in] varstack the stacking of the sources (string = true, number = false).
        */
        FMTData(const std::vector<double>& lvalues,
                const FMTyieldparserop& lops,
                const std::vector<std::string>& lsource,
				const std::vector<bool>& varstack);
        // DocString: FMTData::operator=
        /**
        @brief Copy assignment operator for FMTData.
        @param[in] rhs the FMTData to copy.
        @return a reference to this FMTData.
        */
        FMTData& operator = (const FMTData& rhs);
        // DocString: FMTData::getOp
        /**
        @brief Return the yield parser operator of the data.
        @return the yield parser operator.
        */
        FMTyieldparserop getOp() const;
        // DocString: FMTData::clearCache
        /**
        @brief Clear the cached values of the data.
        */
        void clearCache();
		// DocString: FMTData::constant
		/**
		@brief Return true if the data is constant.
		@return true if the data is constant else false.
		*/
		bool constant() const;
		// DocString: FMTData::nullData
		/**
		@brief Return true if the data is null.
		@return true if the data is null else false.
		*/
		bool nullData() const;
		// DocString: FMTData::cacheValue
		/**
		@brief Return true if the value for a yield request is cached.
		@param[in] request the yield request.
		@return true if the value is cached else false.
		*/
		bool cacheValue(const FMTYieldRequest& request) const;
		// DocString: FMTData::get
		/**
		@brief Get the value of the data for a yield request.
		@param[in] request the yield request.
		@return the value of the data.
		*/
		double get(const FMTYieldRequest& request) const;
		// DocString: FMTData::set
		/**
		@brief Cache the value of the data for a yield request.
		@param[in] value the value to cache.
		@param[in] request the yield request.
		@param[in] age_only if true caches on age only.
		*/
		void set(const double& value, const FMTYieldRequest& request, const bool& age_only) const;
		// DocString: FMTData::getSources
		/**
		@brief Return pointers to the sources of the data.
		@return the sources of the data.
		*/
		std::vector<const std::string*> getSources() const;
		// DocString: FMTData::getSourcesCopy
		/**
		@brief Return a copy of the sources of the data.
		@return a copy of the sources of the data.
		*/
		std::vector<std::string> getSourcesCopy() const;
		// DocString: FMTData::getValues
		/**
		@brief Return pointers to the values of the data.
		@return the values of the data.
		*/
		std::vector<const double*>getValues() const;
		//std::vector<double>tovalues(const std::map<std::string, double>& sources) const;
		// DocString: FMTData::toExpression
		/**
		@brief Convert the data to an FMTExpression.
		@return the expression of the data.
		*/
		FMTExpression toExpression() const;
		// DocString: FMTData::operator==
		/**
		@brief Equality comparison operator of FMTData.
		@param[in] rhs the FMTData to compare with.
		@return true if both data are equal else false.
		*/
		bool operator == (const FMTData& rhs) const;
        // DocString: FMTData::operator std::string
        /**
        @brief Convert the data to its string representation.
        @return the string representation of the data.
        */
        operator std::string() const;
		// DocString: FMTData::operator*
		/**
		@brief Multiply the data values by a factor.
		@param[in] factor the factor to multiply by.
		@return the multiplied data.
		*/
		FMTData operator * (const double& factor) const;
	private:
		friend class boost::serialization::access;
		// DocString: FMTData::serialize
		/**
		@brief Serialize the FMTData for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& BOOST_SERIALIZATION_NVP(ops);
			ar& BOOST_SERIALIZATION_NVP(source);
			ar& BOOST_SERIALIZATION_NVP(stacking);
			ar& BOOST_SERIALIZATION_NVP(data);
		}
		FMTyieldparserop ops;
		std::vector<std::string>source;
		std::vector<bool>stacking;//Stacking a string = true stacking a number = false
		//mutable std::unique_ptr<boost::unordered_map<FMTDevelopment,double>> _cache; //hash base on (age and/or period and/or mask) //only for complex yield!!!!
		mutable std::unique_ptr<std::map<FMTDevelopment, double>> _cache; //hash base on (age and/or period and/or mask) //only for complex yield!!!!
		mutable bool _agebase;
		//size_t hashdata(const int& period, const int& age, const FMTMask& mask) const;
		// DocString: FMTData::getSummaryDevelopment
		/**
		@brief Return the summary development for a yield request.
		@param[in] request the yield request.
		@return the summary development.
		*/
		FMTDevelopment getSummaryDevelopment(const FMTYieldRequest& request) const;
		// DocString: FMTData::allocateCache
		/**
		@brief Allocate the cache of the data.
		*/
		void allocateCache() const;
		// DocString: FMTData::deAllocateCache
		/**
		@brief Deallocate the cache of the data.
		*/
		void deAllocateCache() const;
    };
}

BOOST_CLASS_EXPORT_KEY(Core::FMTData)


#endif // FMTDATA_Hm_included
