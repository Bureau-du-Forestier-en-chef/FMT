/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTAGEYIELDHANDLER_Hm_included
#define FMTAGEYIELDHANDLER_Hm_included

#include "FMTYieldHandler.h"
#include <string>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include "FMTData.h"

namespace Core
{
	// DocString: FMTAgeYieldHandler
	/**
	@brief Yield handler for age based yields, as seen in the yield section with _AGE.
	*/
	class FMTEXPORT FMTAgeYieldHandler : public FMTYieldHandler
	{
	public:
		// DocString: FMTAgeYieldHandler::get
		/**
		@brief Get the value of a yield for a request.
		@param[in] yld the yield name.
		@param[in] request the yield request.
		@return the yield value.
		*/
		virtual double get(const std::string& yld, const FMTYieldRequest& request) const;
		// DocString: FMTAgeYieldHandler::operator std::string
		/**
		@brief Return the string representation of the age yield handler.
		@return the string representation.
		*/
		virtual  operator std::string() const;
		// DocString: ~FMTAgeYieldHandler()
		/**
		@brief Default destructor for FMTAgeYieldHandler.
		*/
		~FMTAgeYieldHandler() = default;
		// DocString: FMTAgeYieldHandler()
		/**
		@brief Default constructor for FMTAgeYieldHandler.
		*/
		FMTAgeYieldHandler() = default;
		// DocString: FMTAgeYieldHandler(const FMTAgeYieldHandler&)
		/**
		@brief Copy constructor for FMTAgeYieldHandler.
		@param[in] rhs the FMTAgeYieldHandler to copy.
		*/
		FMTAgeYieldHandler(const FMTAgeYieldHandler& rhs) = default;
		// DocString: FMTAgeYieldHandler::operator=
		/**
		@brief Copy assignment operator for FMTAgeYieldHandler.
		@param[in] rhs the FMTAgeYieldHandler to copy.
		@return a reference to this FMTAgeYieldHandler.
		*/
		FMTAgeYieldHandler& operator = (const FMTAgeYieldHandler& rhs) = default;
		// DocString: FMTAgeYieldHandler(const FMTYieldHandler&)
		/**
		@brief Construct an age yield handler from a yield handler.
		@param[in] rhs the yield handler.
		*/
		FMTAgeYieldHandler(const FMTYieldHandler& rhs);
		// DocString: FMTAgeYieldHandler(const FMTMask&)
		/**
		@brief Construct an age yield handler from a mask.
		@param[in] mask the mask.
		*/
		FMTAgeYieldHandler(const FMTMask& mask);
		// DocString: FMTAgeYieldHandler::setYieldValues
		/**
		@brief Set the values for a given yield name.
		@param[in] yldname the yield name.
		@param[in] baseages the base ages used to determine the age of the values, same size as values.
		@param[in] values the values.
		*/
		virtual void setYieldValues(const std::string& yldname,const std::vector<int>& baseages,const std::vector<double>& values);	
		// DocString: FMTAgeYieldHandler::getLastValue
		/**
		@brief Return the last value of a yield.
		@param[in] yld the yield name.
		@return the last value of the yield.
		*/
		virtual double getLastValue(const std::string yld) const;
		// DocString: FMTAgeYieldHandler::pushData(const std::string&, const double&)
		/**
		@brief Push a value for a yield.
		@param[in] yld the yield name.
		@param[in] value the value.
		@return true if the value is pushed else false.
		*/
		virtual bool pushData(const std::string& yld, const double& value);
		// DocString: FMTAgeYieldHandler::pushData(const std::string&, const FMTData&)
		/**
		@brief Push data for a yield.
		@param[in] yld the yield name.
		@param[in] data the data.
		@return true if the data is pushed else false.
		*/
		virtual bool pushData(const std::string& yld, const FMTData& data);
		// DocString: FMTAgeYieldHandler::clone
		/**
		@brief Clone the age yield handler.
		@return a unique pointer to the cloned yield handler.
		*/
		virtual std::unique_ptr<FMTYieldHandler>clone() const;
		// DocString: FMTAgeYieldHandler::operator==
		/**
		@brief Equality comparison operator of FMTAgeYieldHandler.
		@param[in] rhs the handler to compare with.
		@return true if both handlers are equal else false.
		*/
		virtual bool operator == (const FMTAgeYieldHandler& rhs) const;
		// DocString: FMTAgeYieldHandler::empty
		/**
		@brief Return true if the handler is empty.
		@return true if the handler is empty else false.
		*/
		virtual bool empty() const;
		// DocString: FMTAgeYieldHandler::size
		/**
		@brief Return the number of yields in the handler.
		@return the number of yields.
		*/
		virtual size_t size() const;
		// DocString: FMTAgeYieldHandler::getType
		/**
		@brief Return the type of the yield handler.
		@return the yield type.
		*/
		virtual FMTyldtype getType() const;
		// DocString: FMTAgeYieldHandler::operator[]
		/**
		@brief Access the data of a yield.
		@param[in] yldname the yield name.
		@return a reference to the data of the yield.
		*/
		virtual FMTData& operator[](const std::string& yldname);
		// DocString: FMTAgeYieldHandler::at
		/**
		@brief Access the data of a yield.
		@param[in] yldname the yield name.
		@return a const reference to the data of the yield.
		*/
		virtual const FMTData& at(const std::string& yldname) const;
		// DocString: FMTAgeYieldHandler::containsYield
		/**
		@brief Return true if the handler contains a given yield.
		@param[in] yldname the yield name.
		@return true if the handler contains the yield else false.
		*/
		virtual bool containsYield(const std::string& yldname) const;
		// DocString: FMTAgeYieldHandler::getYieldNames
		/**
		@brief Return the yield names of the handler.
		@return the yield names.
		*/
		virtual std::vector<std::string>getYieldNames() const;
		// DocString: FMTAgeYieldHandler::clearCache
		/**
		@brief Clear the cache of the handler.
		*/
		virtual void clearCache();
		// DocString: FMTAgeYieldHandler::getAllYieldsData
		/**
		@brief Return all the yield data up to a maximum base age.
		@param[in] maxbase the maximum base age.
		@return a map of yield names to their values.
		*/
		virtual std::map<std::string, std::vector<double>>getAllYieldsData(const int& maxbase)const;
		// DocString: FMTAgeYieldHandler::getFromFactor
		/**
		@brief Multiply the handler by a factor and return a new handler.
		@param[in] factor the factor.
		@param[in] yieldnames the yield names to multiply, all if empty.
		@return the multiplied yield handler.
		*/
		virtual std::unique_ptr<FMTYieldHandler> getFromFactor(const double& factor,
			std::vector<std::string>yieldnames = std::vector<std::string>()) const;
		// DocString: FMTAgeYieldHandler::getEndPoint
		/**
		@brief Return the age at which a yield reaches a bound value.
		@param[in] yld the yield name.
		@param[in] lowerstep the lower step.
		@param[in] bound the bound.
		@param[in] value the value.
		@return the end point age.
		*/
		virtual int getEndPoint(const std::string& yld, const int& lowerstep, const double& bound, const double& value) const;
		// DocString: FMTAgeYieldHandler::getPeak
		/**
		@brief Return the peak value of a yield for a request.
		@param[in] request the yield request.
		@param[in] yld the yield name.
		@param[in] targetage the target age.
		@return the peak value.
		*/
		virtual double getPeak(const FMTYieldRequest& request,const std::string& yld, const int& targetage) const;
		// DocString: FMTAgeYieldHandler::getPeakfrom
		/**
		@brief Return the peak value of a yield from a maximum value.
		@param[in] yld the yield name.
		@param[in] maxvalue the maximum value.
		@return the peak value.
		*/
		virtual double getPeakfrom(const std::string& yld, double maxvalue = std::numeric_limits<double>::lowest()) const;
		// DocString: FMTAgeYieldHandler::getAge(const FMTYieldRequest&, const FMTSpec&)
		/**
		@brief Return the age of a development from a request and a specification.
		@param[in] request the yield request.
		@param[in] spec the specification.
		@return the age.
		*/
		virtual int getAge(const FMTYieldRequest& request, const FMTSpec& spec) const;
		// DocString: FMTAgeYieldHandler::getYieldLinearValue
		/**
		@brief Return the linearly interpolated value of a yield for a request.
		@param[in] yldname the yield name.
		@param[in] request the yield request.
		@param[in] allowoutofrange if true allows out of range values.
		@return the linear yield value.
		*/
		virtual double getYieldLinearValue(const std::string&yldname, const FMTYieldRequest& request, bool allowoutofrange=true) const;
	private:
		friend class boost::serialization::access;
		// DocString: FMTAgeYieldHandler::serialize
		/**
		@brief Serialize the FMTAgeYieldHandler through its base FMTYieldHandler for multiprocessing across multiple cpus (pickle in Python).
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
		// DocString: FMTAgeYieldHandler::getAge(const std::string, const double&, const int&)
		/**
		@brief Return the age at which a yield reaches a value from a starting age.
		@param[in] yld the yield name.
		@param[in] value the value.
		@param[in] starting_age the starting age.
		@return the age.
		*/
		int getAge(const std::string yld, const double& value, const int& starting_age) const;
	};

}

#endif 

