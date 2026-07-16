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
#include "FMTmask.h"
#include <vector>
#include "FMTutility.h"
#include "FMTobject.h"
#include "FMTbounds.hpp"

namespace Core
{
class FMTYieldRequest;
class FMTSpec;
class FMTData;

struct cmpYieldString 
{
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

class FMTEXPORT FMTYieldHandler : public FMTObject
{
public:
	virtual size_t getOverrideIndex() const;
	virtual std::vector<size_t> getTabous() const;
	virtual void setOverrideIndex(const size_t& newindex);
	virtual ~FMTYieldHandler() = default;
	FMTYieldHandler() = default;
	FMTYieldHandler(const FMTYieldHandler& rhs) = default;
	FMTYieldHandler& operator = (const FMTYieldHandler& rhs) = default;
	FMTYieldHandler(const FMTMask& lmask);
	double getLinearValue(const std::vector<double>& dls, const int& agetarget, bool allowoutofrange) const;
	virtual int getLastBase() const;
	const std::vector<int>& getBases() const;
	FMTMask getMask() const;
	void setMask(const FMTMask& p_mask);
	bool pushBase(const int& base);
	void setBase(const std::vector<int>& allvalues);
	bool inLookAt(const std::string& yld) const;
	virtual double getLastValue(const std::string yld) const;
	virtual double get(const std::string& yld, const FMTYieldRequest& request) const;
	virtual bool pushData(const std::string& yld, const double& value);
	virtual bool pushData(const std::string& yld, const FMTData& data);
	virtual std::vector<std::string> indexes(const std::vector<std::string>& names) const;
	virtual  operator std::string() const;
	virtual std::vector<std::string> compare(const std::vector<std::string>& keys) const;
	virtual std::unique_ptr<FMTYieldHandler>clone() const;
	virtual bool operator == (const FMTYieldHandler& rhs) const;
	virtual bool empty() const;
	virtual size_t size() const;
	virtual FMTyldtype getType() const;
	virtual FMTData& operator[](const std::string& yldname);
	virtual const FMTData& at(const std::string& yldname) const;
	virtual bool containsYield(const std::string& yldname) const;
	virtual bool isNullYield(const std::string& yldname) const;
	virtual std::vector<std::string>getYieldNames() const;
	virtual void clearCache();
	virtual std::map<std::string, std::vector<double>>getAllYieldsData(const int& maxbase)const;
	virtual std::unique_ptr<FMTYieldHandler> presolve(const FMTMaskFilter& filter, const std::vector<FMTTheme>& newthemes) const;
	virtual std::unique_ptr<FMTYieldHandler> postSolve(const FMTMaskFilter& filter,const std::vector<FMTTheme>& basethemes) const;
	virtual std::unique_ptr<FMTYieldHandler> getFromFactor(const double& factor,
		std::vector<std::string>yieldnames = std::vector<std::string>()) const;
	virtual int getEndPoint(const std::string& yld, const int& lowerstep, const double& bound, const double& value) const;
	virtual double getPeak(const FMTYieldRequest& request,const std::string& yld, const int& targetage) const;
	virtual int getAge(const FMTYieldRequest& request, const FMTSpec& spec) const;
	virtual double getYieldLinearValue(const std::string&yldname, const FMTYieldRequest& request, bool allowoutofrange=true) const;
protected:
	FMTMask mask;
	std::vector<int>bases;
	mutable std::unordered_set<std::string>lookat;
	static bool basePushData(std::map<std::string, FMTData, cmpYieldString>& elements, const std::string& yld, const double& value);
	static bool basePushData(std::map<std::string, FMTData, cmpYieldString>& elements, const std::string& yld, const FMTData& data);
	double getChangesFrom(const int& targetage, const int& peakstep) const;
	int getMaxBase(const FMTYieldRequest& request) const;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& BOOST_SERIALIZATION_NVP(mask);
		ar& BOOST_SERIALIZATION_NVP(bases);
	}

	
};

}
#endif // FMTYLDDATA_Hm_included
