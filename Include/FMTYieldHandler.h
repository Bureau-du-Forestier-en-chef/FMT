/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTYLDDATA_Hm_included
#define FMTYLDDATA_Hm_included

#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include "FMTMask.h"
#include <vector>
#include "FMTutility.h"
#include "FMTObject.h"
#include "FMTBounds.hpp"

namespace Core
{
class FMTYieldRequest;
class FMTSpec;
class FMTData;

// DocString: cmpYieldString
/**
@brief Comparator ordering yield names by length then by content from the end.
*/
struct cmpYieldString 
{
	// DocString: cmpYieldString::operator()
	/**
	@brief Compare two yield names.
	@param[in] p_first the first yield name.
	@param[in] p_second the second yield name.
	@return true if the first is less than the second else false.
	*/
	inline bool operator()(const std::string& p_first, const std::string& p_second) const noexcept
		{
		const size_t FIRST_LENGTH = p_first.length();
		const size_t SECOND_LENGTH = p_second.length();
		bool lessThan = false;
		if (FIRST_LENGTH != SECOND_LENGTH)
		{
			lessThan = FIRST_LENGTH < SECOND_LENGTH;
		}
		else {
			size_t i = FIRST_LENGTH;
			bool gotValue = false;
			while (!gotValue && i > 0)
			{
				--i;
				if (p_first[i] != p_second[i])
				{
					if (p_first[i] < p_second[i])
					{
						lessThan = true;
					}
					else {
						lessThan = false;
					}
					gotValue = true;
				}
			}
		}
		return lessThan;
		}
};

// DocString: FMTYieldHandler
/**
@brief Base class for yield handlers, holding a mask and the base ages of the yields.
*/
class FMTEXPORT FMTYieldHandler : public FMTObject
{
public:
	// DocString: FMTYieldHandler::getOverrideIndex
	/**
	@brief Return the override index of the handler.
	@return the override index.
	*/
	virtual size_t getOverrideIndex() const;
	// DocString: FMTYieldHandler::getTabous
	/**
	@brief Return the tabou indexes of the handler.
	@return the tabou indexes.
	*/
	virtual std::vector<size_t> getTabous() const;
	// DocString: FMTYieldHandler::setOverrideIndex
	/**
	@brief Set the override index of the handler.
	@param[in] newindex the override index to set.
	*/
	virtual void setOverrideIndex(const size_t& newindex);
	// DocString: ~FMTYieldHandler()
	/**
	@brief Default virtual destructor for FMTYieldHandler.
	*/
	virtual ~FMTYieldHandler() = default;
	// DocString: FMTYieldHandler()
	/**
	@brief Default constructor for FMTYieldHandler.
	*/
	FMTYieldHandler() = default;
	// DocString: FMTYieldHandler(const FMTYieldHandler&)
	/**
	@brief Copy constructor for FMTYieldHandler.
	@param[in] rhs the FMTYieldHandler to copy.
	*/
	FMTYieldHandler(const FMTYieldHandler& rhs) = default;
	// DocString: FMTYieldHandler::operator=
	/**
	@brief Copy assignment operator for FMTYieldHandler.
	@param[in] rhs the FMTYieldHandler to copy.
	@return a reference to this FMTYieldHandler.
	*/
	FMTYieldHandler& operator = (const FMTYieldHandler& rhs) = default;
	// DocString: FMTYieldHandler(const FMTMask&)
	/**
	@brief Construct a yield handler from a mask.
	@param[in] lmask the mask.
	*/
	FMTYieldHandler(const FMTMask& lmask);
	// DocString: FMTYieldHandler::getLinearValue
	/**
	@brief Return the linearly interpolated value at a target age.
	@param[in] dls the values.
	@param[in] agetarget the target age.
	@param[in] allowoutofrange if true allows out of range values.
	@return the linear value.
	*/
	double getLinearValue(const std::vector<double>& dls, const int& agetarget, bool allowoutofrange) const;
	// DocString: FMTYieldHandler::getLastBase
	/**
	@brief Return the last base age of the handler.
	@return the last base age.
	*/
	virtual int getLastBase() const;
	// DocString: FMTYieldHandler::getBases
	/**
	@brief Return the base ages of the handler.
	@return a const reference to the base ages.
	*/
	const std::vector<int>& getBases() const;
	// DocString: FMTYieldHandler::getMask
	/**
	@brief Return the mask of the handler.
	@return the mask.
	*/
	FMTMask getMask() const;
	// DocString: FMTYieldHandler::setMask
	/**
	@brief Set the mask of the handler.
	@param[in] p_mask the mask to set.
	*/
	void setMask(const FMTMask& p_mask);
	// DocString: FMTYieldHandler::pushBase
	/**
	@brief Push a base age to the handler.
	@param[in] base the base age.
	@return true if the base age is pushed else false.
	*/
	bool pushBase(const int& base);
	// DocString: FMTYieldHandler::setBase
	/**
	@brief Set the base ages of the handler.
	@param[in] allvalues the base ages to set.
	*/
	void setBase(const std::vector<int>& allvalues);
	// DocString: FMTYieldHandler::inLookAt
	/**
	@brief Return true if a yield is in the look at set.
	@param[in] yld the yield name.
	@return true if the yield is in the look at set else false.
	*/
	bool inLookAt(const std::string& yld) const;
	// DocString: FMTYieldHandler::getLastValue
	/**
	@brief Return the last value of a yield.
	@param[in] yld the yield name.
	@return the last value of the yield.
	*/
	virtual double getLastValue(const std::string yld) const;
	// DocString: FMTYieldHandler::get
	/**
	@brief Get the value of a yield for a request.
	@param[in] yld the yield name.
	@param[in] request the yield request.
	@return the yield value.
	*/
	virtual double get(const std::string& yld, const FMTYieldRequest& request) const;
	// DocString: FMTYieldHandler::pushData(const std::string&, const double&)
	/**
	@brief Push a value for a yield.
	@param[in] yld the yield name.
	@param[in] value the value.
	@return true if the value is pushed else false.
	*/
	virtual bool pushData(const std::string& yld, const double& value);
	// DocString: FMTYieldHandler::pushData(const std::string&, const FMTData&)
	/**
	@brief Push data for a yield.
	@param[in] yld the yield name.
	@param[in] data the data.
	@return true if the data is pushed else false.
	*/
	virtual bool pushData(const std::string& yld, const FMTData& data);
	// DocString: FMTYieldHandler::indexes
	/**
	@brief Return the yield names that are indexes among the given names.
	@param[in] names the names.
	@return the index names.
	*/
	virtual std::vector<std::string> indexes(const std::vector<std::string>& names) const;
	// DocString: FMTYieldHandler::operator std::string
	/**
	@brief Return the string representation of the yield handler.
	@return the string representation.
	*/
	virtual  operator std::string() const;
	// DocString: FMTYieldHandler::compare
	/**
	@brief Return the keys that match the handler.
	@param[in] keys the keys.
	@return the matching keys.
	*/
	virtual std::vector<std::string> compare(const std::vector<std::string>& keys) const;
	// DocString: FMTYieldHandler::clone
	/**
	@brief Clone the yield handler.
	@return a unique pointer to the cloned yield handler.
	*/
	virtual std::unique_ptr<FMTYieldHandler>clone() const;
	// DocString: FMTYieldHandler::operator==
	/**
	@brief Equality comparison operator of FMTYieldHandler.
	@param[in] rhs the handler to compare with.
	@return true if both handlers are equal else false.
	*/
	virtual bool operator == (const FMTYieldHandler& rhs) const;
	// DocString: FMTYieldHandler::empty
	/**
	@brief Return true if the handler is empty.
	@return true if the handler is empty else false.
	*/
	virtual bool empty() const;
	// DocString: FMTYieldHandler::size
	/**
	@brief Return the number of yields in the handler.
	@return the number of yields.
	*/
	virtual size_t size() const;
	// DocString: FMTYieldHandler::getType
	/**
	@brief Return the type of the yield handler.
	@return the yield type.
	*/
	virtual FMTyldtype getType() const;
	// DocString: FMTYieldHandler::operator[]
	/**
	@brief Access the data of a yield.
	@param[in] yldname the yield name.
	@return a reference to the data of the yield.
	*/
	virtual FMTData& operator[](const std::string& yldname);
	// DocString: FMTYieldHandler::at
	/**
	@brief Access the data of a yield.
	@param[in] yldname the yield name.
	@return a const reference to the data of the yield.
	*/
	virtual const FMTData& at(const std::string& yldname) const;
	// DocString: FMTYieldHandler::containsYield
	/**
	@brief Return true if the handler contains a given yield.
	@param[in] yldname the yield name.
	@return true if the handler contains the yield else false.
	*/
	virtual bool containsYield(const std::string& yldname) const;
	// DocString: FMTYieldHandler::isNullYield
	/**
	@brief Return true if a yield is a null yield.
	@param[in] yldname the yield name.
	@return true if the yield is null else false.
	*/
	virtual bool isNullYield(const std::string& yldname) const;
	// DocString: FMTYieldHandler::getYieldNames
	/**
	@brief Return the yield names of the handler.
	@return the yield names.
	*/
	virtual std::vector<std::string>getYieldNames() const;
	// DocString: FMTYieldHandler::clearCache
	/**
	@brief Clear the cache of the handler.
	*/
	virtual void clearCache();
	// DocString: FMTYieldHandler::getAllYieldsData
	/**
	@brief Return all the yield data up to a maximum base age.
	@param[in] maxbase the maximum base age.
	@return a map of yield names to their values.
	*/
	virtual std::map<std::string, std::vector<double>>getAllYieldsData(const int& maxbase)const;
	// DocString: FMTYieldHandler::presolve
	/**
	@brief Return a presolved copy of the handler.
	@param[in] filter the mask filter.
	@param[in] newthemes the presolved themes.
	@return the presolved yield handler.
	*/
	virtual std::unique_ptr<FMTYieldHandler> presolve(const FMTMaskFilter& filter, const std::vector<FMTTheme>& newthemes) const;
	// DocString: FMTYieldHandler::postSolve
	/**
	@brief Return a postsolved copy of the handler.
	@param[in] filter the mask filter.
	@param[in] basethemes the original themes.
	@return the postsolved yield handler.
	*/
	virtual std::unique_ptr<FMTYieldHandler> postSolve(const FMTMaskFilter& filter,const std::vector<FMTTheme>& basethemes) const;
	// DocString: FMTYieldHandler::getFromFactor
	/**
	@brief Multiply the handler by a factor and return a new handler.
	@param[in] factor the factor.
	@param[in] yieldnames the yield names to multiply, all if empty.
	@return the multiplied yield handler.
	*/
	virtual std::unique_ptr<FMTYieldHandler> getFromFactor(const double& factor,
		std::vector<std::string>yieldnames = std::vector<std::string>()) const;
	// DocString: FMTYieldHandler::getEndPoint
	/**
	@brief Return the age at which a yield reaches a bound value.
	@param[in] yld the yield name.
	@param[in] lowerstep the lower step.
	@param[in] bound the bound.
	@param[in] value the value.
	@return the end point age.
	*/
	virtual int getEndPoint(const std::string& yld, const int& lowerstep, const double& bound, const double& value) const;
	// DocString: FMTYieldHandler::getPeak
	/**
	@brief Return the peak value of a yield for a request.
	@param[in] request the yield request.
	@param[in] yld the yield name.
	@param[in] targetage the target age.
	@return the peak value.
	*/
	virtual double getPeak(const FMTYieldRequest& request,const std::string& yld, const int& targetage) const;
	// DocString: FMTYieldHandler::getAge
	/**
	@brief Return the age of a development from a request and a specification.
	@param[in] request the yield request.
	@param[in] spec the specification.
	@return the age.
	*/
	virtual int getAge(const FMTYieldRequest& request, const FMTSpec& spec) const;
	// DocString: FMTYieldHandler::getYieldLinearValue
	/**
	@brief Return the linearly interpolated value of a yield for a request.
	@param[in] yldname the yield name.
	@param[in] request the yield request.
	@param[in] allowoutofrange if true allows out of range values.
	@return the linear yield value.
	*/
	virtual double getYieldLinearValue(const std::string&yldname, const FMTYieldRequest& request, bool allowoutofrange=true) const;
protected:
	FMTMask mask;
	std::vector<int>bases;
	mutable std::unordered_set<std::string>lookat;
	// DocString: FMTYieldHandler::basePushData(std::map<std::string, FMTData, cmpYieldString>&, const std::string&, const double&)
	/**
	@brief Push a value for a yield into the given elements.
	@param[in,out] elements the elements.
	@param[in] yld the yield name.
	@param[in] value the value.
	@return true if the value is pushed else false.
	*/
	static bool basePushData(std::map<std::string, FMTData, cmpYieldString>& elements, const std::string& yld, const double& value);
	// DocString: FMTYieldHandler::basePushData(std::map<std::string, FMTData, cmpYieldString>&, const std::string&, const FMTData&)
	/**
	@brief Push data for a yield into the given elements.
	@param[in,out] elements the elements.
	@param[in] yld the yield name.
	@param[in] data the data.
	@return true if the data is pushed else false.
	*/
	static bool basePushData(std::map<std::string, FMTData, cmpYieldString>& elements, const std::string& yld, const FMTData& data);
	// DocString: FMTYieldHandler::getChangesFrom
	/**
	@brief Return the change of value from a target age over a peak step.
	@param[in] targetage the target age.
	@param[in] peakstep the peak step.
	@return the change of value.
	*/
	double getChangesFrom(const int& targetage, const int& peakstep) const;
	// DocString: FMTYieldHandler::getMaxBase
	/**
	@brief Return the maximum base age for a request.
	@param[in] request the yield request.
	@return the maximum base age.
	*/
	int getMaxBase(const FMTYieldRequest& request) const;
private:
	friend class boost::serialization::access;
	// DocString: FMTYieldHandler::serialize
	/**
	@brief Serialize the FMTYieldHandler for multiprocessing across multiple cpus (pickle in Python).
	@tparam Archive the archive type.
	@param[in,out] ar the archive to serialize to or from.
	@param[in] version the serialization version.
	*/
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& BOOST_SERIALIZATION_NVP(mask);
		ar& BOOST_SERIALIZATION_NVP(bases);
	}

	
};

}
#endif // FMTYLDDATA_Hm_included
