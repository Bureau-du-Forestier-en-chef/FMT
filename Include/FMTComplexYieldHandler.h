/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTCOMPLEXYIELDHANDLER_Hm_included
#define FMTCOMPLEXYIELDHANDLER_Hm_included

#include "FMTYieldHandler.h"
#include "FMTData.h"
#include <string>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include "FMTYieldsCache.h"
#include <memory>

namespace Core
{
	// DocString: FMTComplexYieldHandler
	/**
	@brief Yield handler for complex yields computed from other yields using operators, as seen in the yield section.
	*/
	class FMTEXPORT FMTComplexYieldHandler final : public FMTYieldHandler
	{
	public:
		// DocString: FMTComplexYieldHandler::setTabou(const size_t&)
		/**
		@brief Add an override index to the tabou set.
		@param[in] index the index to add.
		*/
		void setTabou(const size_t& index);
		// DocString: FMTComplexYieldHandler::setTabou(const FMTComplexYieldHandler&)
		/**
		@brief Copy the tabou set from another complex yield handler.
		@param[in] rhs the complex yield handler.
		*/
		void setTabou(const FMTComplexYieldHandler& rhs);
		// DocString: FMTComplexYieldHandler::getTabous
		/**
		@brief Return the tabou indexes of the handler.
		@return the tabou indexes.
		*/
		std::vector<size_t>getTabous() const override;
		// DocString: FMTComplexYieldHandler::setOverrideIndex
		/**
		@brief Set the override index of the handler.
		@param[in] newindex the override index to set.
		*/
		virtual void setOverrideIndex(const size_t& newindex);
		// DocString: FMTComplexYieldHandler::getOverrideIndex
		/**
		@brief Return the override index of the handler.
		@return the override index.
		*/
		virtual size_t  getOverrideIndex() const;
		// DocString: FMTComplexYieldHandler::getLastBase
		/**
		@brief Return the last base age of the handler.
		@return the last base age.
		*/
		virtual int getLastBase() const;
		// DocString: FMTComplexYieldHandler::get
		/**
		@brief Get the value of a yield for a request.
		@param[in] yld the yield name.
		@param[in] request the yield request.
		@return the yield value.
		*/
		virtual double get(const std::string& yld, const FMTYieldRequest& request) const;
		// DocString: FMTComplexYieldHandler::compareSources
		/**
		@brief Compare the sources of a yield with those of an overriding handler.
		@param[in] yield the yield name.
		@param[in] overridedyield the overriding handler.
		@return true if the sources are equal else false.
		*/
		bool compareSources(const std::string& yield,const FMTComplexYieldHandler& overridedyield) const;
		// DocString: FMTComplexYieldHandler::operator std::string
		/**
		@brief Return the string representation of the complex yield handler.
		@return the string representation.
		*/
		virtual  operator std::string() const;
		// DocString: ~FMTComplexYieldHandler()
		/**
		@brief Destructor for FMTComplexYieldHandler.
		*/
		~FMTComplexYieldHandler();
		// DocString: FMTComplexYieldHandler()
		/**
		@brief Default constructor for FMTComplexYieldHandler.
		*/
		FMTComplexYieldHandler();
		// DocString: FMTComplexYieldHandler(const FMTComplexYieldHandler&)
		/**
		@brief Copy constructor for FMTComplexYieldHandler.
		@param[in] rhs the FMTComplexYieldHandler to copy.
		*/
		FMTComplexYieldHandler(const FMTComplexYieldHandler& rhs) = default;
		// DocString: FMTComplexYieldHandler::operator=
		/**
		@brief Copy assignment operator for FMTComplexYieldHandler.
		@param[in] rhs the FMTComplexYieldHandler to copy.
		@return a reference to this FMTComplexYieldHandler.
		*/
		FMTComplexYieldHandler& operator = (const FMTComplexYieldHandler& rhs) = default;
		// DocString: FMTComplexYieldHandler(const FMTMask&)
		/**
		@brief Construct a complex yield handler from a mask.
		@param[in] mask the mask.
		*/
		FMTComplexYieldHandler(const FMTMask& mask);
		// DocString: FMTComplexYieldHandler::getDataElements
		/**
		@brief Return the data elements of the handler.
		@return a const reference to the data elements.
		*/
		const std::map<std::string, FMTData,cmpYieldString>& getDataElements() const;
		// DocString: FMTComplexYieldHandler::indexes
		/**
		@brief Return the yield names that are indexes among the given names.
		@param[in] names the names.
		@return the index names.
		*/
		virtual std::vector<std::string> indexes(const std::vector<std::string>& names) const;
		// DocString: FMTComplexYieldHandler::getPeak
		/**
		@brief Return the peak value of a yield for a request.
		@param[in] request the yield request.
		@param[in] yld the yield name.
		@param[in] targetage the target age.
		@return the peak value.
		*/
		virtual double getPeak(const FMTYieldRequest& request, const std::string& yld, const int& targetage) const;
		// DocString: FMTComplexYieldHandler::complexYldToAgeYld
		/**
		@brief Convert the complex yield to an age yield handler for a request and specification.
		@param[in] request the yield request.
		@param[in] lspec the specification.
		@return the age yield handler.
		*/
		std::unique_ptr<FMTYieldHandler>complexYldToAgeYld(const FMTYieldRequest& request, const FMTSpec& lspec) const;
		// DocString: FMTComplexYieldHandler::pushData(const std::string&, const double&)
		/**
		@brief Push a value for a yield.
		@param[in] yld the yield name.
		@param[in] value the value.
		@return true if the value is pushed else false.
		*/
		virtual bool pushData(const std::string& yld, const double& value);
		// DocString: FMTComplexYieldHandler::pushData(const std::string&, const FMTData&)
		/**
		@brief Push data for a yield.
		@param[in] yld the yield name.
		@param[in] data the data.
		@return true if the data is pushed else false.
		*/
		virtual bool pushData(const std::string& yld, const FMTData& data);
		// DocString: FMTComplexYieldHandler::clone
		/**
		@brief Clone the complex yield handler.
		@return a unique pointer to the cloned yield handler.
		*/
		virtual std::unique_ptr<FMTYieldHandler>clone() const;
		// DocString: FMTComplexYieldHandler::operator==
		/**
		@brief Equality comparison operator of FMTComplexYieldHandler.
		@param[in] rhs the handler to compare with.
		@return true if both handlers are equal else false.
		*/
		virtual bool operator == (const FMTComplexYieldHandler& rhs) const;
		// DocString: FMTComplexYieldHandler::empty
		/**
		@brief Return true if the handler is empty.
		@return true if the handler is empty else false.
		*/
		virtual bool empty() const;
		// DocString: FMTComplexYieldHandler::size
		/**
		@brief Return the number of yields in the handler.
		@return the number of yields.
		*/
		virtual size_t size() const;
		// DocString: FMTComplexYieldHandler::getType
		/**
		@brief Return the type of the yield handler.
		@return the yield type.
		*/
		virtual FMTyldtype getType() const;
		// DocString: FMTComplexYieldHandler::operator[]
		/**
		@brief Access the data of a yield.
		@param[in] yldname the yield name.
		@return a reference to the data of the yield.
		*/
		virtual FMTData& operator[](const std::string& yldname);
		// DocString: FMTComplexYieldHandler::at
		/**
		@brief Access the data of a yield.
		@param[in] yldname the yield name.
		@return a const reference to the data of the yield.
		*/
		virtual const FMTData& at(const std::string& yldname) const;
		// DocString: FMTComplexYieldHandler::containsYield
		/**
		@brief Return true if the handler contains a given yield.
		@param[in] yldname the yield name.
		@return true if the handler contains the yield else false.
		*/
		virtual bool containsYield(const std::string& yldname) const;
		// DocString: FMTComplexYieldHandler::getYieldNames
		/**
		@brief Return the yield names of the handler.
		@return the yield names.
		*/
		virtual std::vector<std::string>getYieldNames() const;
		// DocString: FMTComplexYieldHandler::clearCache
		/**
		@brief Clear the cache of the handler.
		*/
		virtual void clearCache();
		// DocString: FMTComplexYieldHandler::getAge
		/**
		@brief Return the age of a development from a request and a specification.
		@param[in] request the yield request.
		@param[in] spec the specification.
		@return the age.
		*/
		virtual int getAge(const FMTYieldRequest& request, const FMTSpec& spec) const;
		// DocString: FMTComplexYieldHandler::getYieldLinearValue
		/**
		@brief Return the linearly interpolated value of a yield for a request.
		@param[in] yldname the yield name.
		@param[in] request the yield request.
		@param[in] allowoutofrange if true allows out of range values.
		@return the linear yield value.
		*/
		virtual double getYieldLinearValue(const std::string& yldname, const FMTYieldRequest& request, bool allowoutofrange=true) const;
	private:
		friend class boost::serialization::access;
		// DocString: FMTComplexYieldHandler::serialize
		/**
		@brief Serialize the FMTComplexYieldHandler through its base FMTYieldHandler for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("FMTyieldhandler", boost::serialization::base_object<FMTYieldHandler>(*this));
			ar& BOOST_SERIALIZATION_NVP(m_elements);
		}
		std::map<std::string, FMTData, cmpYieldString>m_elements;
		// DocString: FMTComplexYieldHandler::_getSources
		/**
		@brief Return the source values of the complex yield.
		@param[in] srcdata the source data.
		@param[in] request the yield request.
		@param[out] age_only true if the sources depend only on age.
		@return the source values.
		*/
		std::map<std::string, double> _getSources(const std::map<std::string, const std::unique_ptr<FMTYieldHandler>*>& srcdata,
			const FMTYieldRequest& request, bool& age_only) const;

		// DocString: FMTComplexYieldHandler::_getData
		/**
		@brief Return the source yield handlers for a yield.
		@param[in] request the yield request.
		@param[in] names the source names.
		@param[in] original the original yield.
		@return the source yield handlers.
		*/
		std::vector<const std::unique_ptr<FMTYieldHandler>*>_getData(const FMTYieldRequest& request,
			const std::vector<const std::string*>& names, const std::string& original) const;

		// DocString: FMTComplexYieldHandler::_toMap
		/**
		@brief Convert the source data to a map of yield name to value.
		@param[in] p_request the yield request.
		@param[in] p_names the source names.
		@param[in] p_data the source data.
		@return a map of yield name to value.
		*/
		static std::map<std::string, double>_toMap(const FMTYieldRequest& p_request,
											const std::vector<const std::string*>& p_names,
											const std::vector<const std::unique_ptr<FMTYieldHandler>*>& p_data);

		// DocString: FMTComplexYieldHandler::_getSourcesArray
		/**
		@brief Return the source values of the complex yield as an array.
		@param[in] srcdata the source data.
		@param[in] request the yield request.
		@param[out] age_only true if the sources depend only on age.
		@return the source values array.
		*/
		std::vector<double>_getSourcesArray(const std::map<std::string, const std::unique_ptr<FMTYieldHandler>*>& srcdata,
			const FMTYieldRequest& request, bool& age_only) const;
		// DocString: FMTComplexYieldHandler::_toAgeYld
		/**
		@brief Convert the yields to an age yield handler over an age range.
		@param[in] request the yield request.
		@param[in] yieldnames the yield names.
		@param[in] minage the minimum age.
		@param[in] maxage the maximum age.
		@return the age yield handler.
		*/
		std::unique_ptr<FMTYieldHandler>_toAgeYld(const FMTYieldRequest& request,
			const std::vector<std::string>& yieldnames, const int& minage, const int& maxage) const;
		// DocString: FMTComplexYieldHandler::_getRange
		/**
		@brief Compute the Range operator value for a yield.
		@param[in] p_data the data.
		@param[in] p_yld the yield name.
		@param[in] p_request the yield request.
		@return the computed value.
		*/
		double _getRange(const FMTData* p_data,const std::string& p_yld, const FMTYieldRequest& p_request) const;
		// DocString: FMTComplexYieldHandler::_getMultiply
		/**
		@brief Compute the Multiply operator value for a yield.
		@param[in] p_data the data.
		@param[in] p_yld the yield name.
		@param[in] p_request the yield request.
		@return the computed value.
		*/
		double _getMultiply(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const;
		// DocString: FMTComplexYieldHandler::_getSum
		/**
		@brief Compute the Sum operator value for a yield.
		@param[in] p_data the data.
		@param[in] p_yld the yield name.
		@param[in] p_request the yield request.
		@return the computed value.
		*/
		double _getSum(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const;
		// DocString: FMTComplexYieldHandler::_getSubstract
		/**
		@brief Compute the Substract operator value for a yield.
		@param[in] p_data the data.
		@param[in] p_yld the yield name.
		@param[in] p_request the yield request.
		@return the computed value.
		*/
		double _getSubstract(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const;
		// DocString: FMTComplexYieldHandler::_getDivide
		/**
		@brief Compute the Divide operator value for a yield.
		@param[in] p_data the data.
		@param[in] p_yld the yield name.
		@param[in] p_request the yield request.
		@return the computed value.
		*/
		double _getDivide(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const;
		// DocString: FMTComplexYieldHandler::_getYTP
		/**
		@brief Compute the YTP operator value for a yield.
		@param[in] p_data the data.
		@param[in] p_yld the yield name.
		@param[in] p_request the yield request.
		@return the computed value.
		*/
		double _getYTP(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const;
		// DocString: FMTComplexYieldHandler::_getMAI
		/**
		@brief Compute the MAI operator value for a yield.
		@param[in] p_data the data.
		@param[in] p_yld the yield name.
		@param[in] p_request the yield request.
		@return the computed value.
		*/
		double _getMAI(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const;
		// DocString: FMTComplexYieldHandler::_getCAI
		/**
		@brief Compute the CAI operator value for a yield.
		@param[in] p_data the data.
		@param[in] p_yld the yield name.
		@param[in] p_request the yield request.
		@return the computed value.
		*/
		double _getCAI(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const;
		// DocString: FMTComplexYieldHandler::_getEquation
		/**
		@brief Compute the Equation operator value for a yield.
		@param[in] p_data the data.
		@param[in] p_yld the yield name.
		@param[in] p_request the yield request.
		@return the computed value.
		*/
		double _getEquation(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const;
		// DocString: FMTComplexYieldHandler::_getEndPoint
		/**
		@brief Compute the EndPoint operator value for a yield.
		@param[in] p_data the data.
		@param[in] p_yld the yield name.
		@param[in] p_request the yield request.
		@return the computed value.
		*/
		double _getEndPoint(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const;
		// DocString: FMTComplexYieldHandler::_getDelta
		/**
		@brief Compute the Delta operator value for a yield.
		@param[in] p_data the data.
		@param[in] p_yld the yield name.
		@param[in] p_request the yield request.
		@return the computed value.
		*/
		double _getDelta(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const;
		// DocString: FMTComplexYieldHandler::_getDistance
		/**
		@brief Compute the Distance operator value for a yield.
		@param[in] p_data the data.
		@param[in] p_yld the yield name.
		@param[in] p_request the yield request.
		@return the computed value.
		*/
		double _getDistance(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const;
		// DocString: FMTComplexYieldHandler::_getMax
		/**
		@brief Compute the Max operator value for a yield.
		@param[in] p_data the data.
		@param[in] p_yld the yield name.
		@param[in] p_request the yield request.
		@return the computed value.
		*/
		double _getMax(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const;
		// DocString: FMTComplexYieldHandler::_getMin
		/**
		@brief Compute the Min operator value for a yield.
		@param[in] p_data the data.
		@param[in] p_yld the yield name.
		@param[in] p_request the yield request.
		@return the computed value.
		*/
		double _getMin(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const;
		std::unordered_set<size_t>m_overridetabou;
		size_t m_overrideindex;
		mutable FMTYieldsCache _cache;
	};

}

#endif 


