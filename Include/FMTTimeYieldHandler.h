/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTTIMEYIELDHANDLER_Hm_included
#define FMTTIMEYIELDHANDLER_Hm_included

#include "FMTYieldHandler.h"
#include "FMTData.h"
#include <string>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

namespace Core
{
	
	// DocString: FMTTimeYieldHandler
	/**
	@brief Yield handler for time based yields, indexed by period.
	*/
	class FMTEXPORT FMTTimeYieldHandler final : public FMTYieldHandler
	{
	public:
		// DocString: FMTTimeYieldHandler::get
		/**
		@brief Get the value of a yield for a request.
		@param[in] yld the yield name.
		@param[in] request the yield request.
		@return the yield value.
		*/
		virtual double get(const std::string& yld, const FMTYieldRequest& request) const;
		// DocString: FMTTimeYieldHandler::operator std::string
		/**
		@brief Return the string representation of the time yield handler.
		@return the string representation.
		*/
		virtual  operator std::string() const;
		// DocString: ~FMTTimeYieldHandler()
		/**
		@brief Default destructor for FMTTimeYieldHandler.
		*/
		~FMTTimeYieldHandler() = default;
		// DocString: FMTTimeYieldHandler()
		/**
		@brief Default constructor for FMTTimeYieldHandler.
		*/
		FMTTimeYieldHandler() = default;
		// DocString: FMTTimeYieldHandler(const FMTTimeYieldHandler&)
		/**
		@brief Copy constructor for FMTTimeYieldHandler.
		@param[in] rhs the FMTTimeYieldHandler to copy.
		*/
		FMTTimeYieldHandler(const FMTTimeYieldHandler& rhs) = default;
		// DocString: FMTTimeYieldHandler::operator=
		/**
		@brief Copy assignment operator for FMTTimeYieldHandler.
		@param[in] rhs the FMTTimeYieldHandler to copy.
		@return a reference to this FMTTimeYieldHandler.
		*/
		FMTTimeYieldHandler& operator = (const FMTTimeYieldHandler& rhs) = default;
		// DocString: FMTTimeYieldHandler(const FMTMask&)
		/**
		@brief Construct a time yield handler from a mask.
		@param[in] mask the mask.
		*/
		FMTTimeYieldHandler(const FMTMask& p_mask);
		// DocString: FMTTimeYieldHandler::setYieldValues
		/**
		@brief Set the values for a given yield name.
		@param[in] yldname the yield name.
		@param[in] startingperiod the starting period.
		@param[in] values the values.
		*/
		virtual void setYieldValues(const std::string& yldname,const int& startingperiod,const std::vector<double>& values);	
		// DocString: FMTTimeYieldHandler::getLastValue
		/**
		@brief Return the last value of a yield.
		@param[in] yld the yield name.
		@return the last value of the yield.
		*/
		virtual double getLastValue(const std::string yld) const;
		// DocString: FMTTimeYieldHandler::pushData(const std::string&, const double&)
		/**
		@brief Push a value for a yield.
		@param[in] yld the yield name.
		@param[in] value the value.
		@return true if the value is pushed else false.
		*/
		virtual bool pushData(const std::string& yld, const double& value);
		// DocString: FMTTimeYieldHandler::pushData(const std::string&, const FMTData&)
		/**
		@brief Push data for a yield.
		@param[in] yld the yield name.
		@param[in] data the data.
		@return true if the data is pushed else false.
		*/
		virtual bool pushData(const std::string& yld, const FMTData& data);
		// DocString: FMTTimeYieldHandler::clone
		/**
		@brief Clone the time yield handler.
		@return a unique pointer to the cloned yield handler.
		*/
		virtual std::unique_ptr<FMTYieldHandler>clone() const;
		// DocString: FMTTimeYieldHandler::operator==
		/**
		@brief Equality comparison operator of FMTTimeYieldHandler.
		@param[in] rhs the handler to compare with.
		@return true if both handlers are equal else false.
		*/
		virtual bool operator == (const FMTTimeYieldHandler& rhs) const;
		// DocString: FMTTimeYieldHandler::empty
		/**
		@brief Return true if the handler is empty.
		@return true if the handler is empty else false.
		*/
		virtual bool empty() const;
		// DocString: FMTTimeYieldHandler::size
		/**
		@brief Return the number of yields in the handler.
		@return the number of yields.
		*/
		virtual size_t size() const;
		// DocString: FMTTimeYieldHandler::getType
		/**
		@brief Return the type of the yield handler.
		@return the yield type.
		*/
		virtual FMTyldtype getType() const;
		// DocString: FMTTimeYieldHandler::operator[]
		/**
		@brief Access the data of a yield.
		@param[in] yldname the yield name.
		@return a reference to the data of the yield.
		*/
		virtual FMTData& operator[](const std::string& yldname);
		// DocString: FMTTimeYieldHandler::at
		/**
		@brief Access the data of a yield.
		@param[in] yldname the yield name.
		@return a const reference to the data of the yield.
		*/
		virtual const FMTData& at(const std::string& yldname) const;
		// DocString: FMTTimeYieldHandler::containsYield
		/**
		@brief Return true if the handler contains a given yield.
		@param[in] yldname the yield name.
		@return true if the handler contains the yield else false.
		*/
		virtual bool containsYield(const std::string& yldname) const;
		// DocString: FMTTimeYieldHandler::getYieldNames
		/**
		@brief Return the yield names of the handler.
		@return the yield names.
		*/
		virtual std::vector<std::string>getYieldNames() const;
		// DocString: FMTTimeYieldHandler::clearCache
		/**
		@brief Clear the cache of the handler.
		*/
		virtual void clearCache();
		// DocString: FMTTimeYieldHandler::getAllYieldsData
		/**
		@brief Return all the yield data up to a maximum base age.
		@param[in] maxbase the maximum base age.
		@return a map of yield names to their values.
		*/
		virtual std::map<std::string, std::vector<double>>getAllYieldsData(const int& maxbase)const;
		// DocString: FMTTimeYieldHandler::getFromFactor
		/**
		@brief Multiply the handler by a factor and return a new handler.
		@param[in] factor the factor.
		@param[in] yieldnames the yield names to multiply, all if empty.
		@return the multiplied yield handler.
		*/
		virtual std::unique_ptr<FMTYieldHandler> getFromFactor(const double& factor,
			std::vector<std::string>yieldnames = std::vector<std::string>()) const;
	private:
		friend class boost::serialization::access;
		// DocString: FMTTimeYieldHandler::serialize
		/**
		@brief Serialize the FMTTimeYieldHandler through its base FMTYieldHandler for multiprocessing across multiple cpus (pickle in Python).
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
	};

}

#endif 
