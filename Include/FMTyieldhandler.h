/*
Copyright (c) 2019 Gouvernement du Qubec

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
class FMTyieldrequest;
class FMTspec;
class FMTdata;

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

class FMTEXPORT FMTyieldhandler : public FMTobject
{
public:
	virtual size_t getOverrideIndex() const;
	virtual std::vector<size_t> getTabous() const;
	virtual void setOverrideIndex(const size_t& newindex);
	virtual ~FMTyieldhandler() = default;
	FMTyieldhandler() = default;
	FMTyieldhandler(const FMTyieldhandler& rhs) = default;
	FMTyieldhandler& operator = (const FMTyieldhandler& rhs) = default;
	FMTyieldhandler(const FMTmask& lmask);
	double getLinearValue(const std::vector<double>& dls, const int& agetarget, bool allowoutofrange) const;
	virtual int getLastBase() const;
	const std::vector<int>& getBases() const;
	FMTmask getmask() const;
	void setMask(const FMTmask& p_mask);
	bool pushBase(const int& base);
	void setBase(const std::vector<int>& allvalues);
	bool inLookAt(const std::string& yld) const;
	virtual double getLastValue(const std::string yld) const;
	virtual double get(const std::string& yld, const FMTyieldrequest& request) const;
	virtual bool pushData(const std::string& yld, const double& value);
	virtual bool pushData(const std::string& yld, const FMTdata& data);
	virtual std::vector<std::string> indexes(const std::vector<std::string>& names) const;
	virtual  operator std::string() const;
	virtual std::vector<std::string> compare(const std::vector<std::string>& keys) const;
	virtual std::unique_ptr<FMTyieldhandler>clone() const;
	virtual bool operator == (const FMTyieldhandler& rhs) const;
	virtual bool empty() const;
	virtual size_t size() const;
	virtual FMTyldtype getType() const;
	virtual FMTdata& operator[](const std::string& yldname);
	virtual const FMTdata& at(const std::string& yldname) const;
	virtual bool containsYield(const std::string& yldname) const;
	virtual bool isNullYield(const std::string& yldname) const;
	virtual std::vector<std::string>getYieldNames() const;
	virtual void clearCache();
	virtual std::map<std::string, std::vector<double>>getAllYieldsData(const int& maxbase)const;
	virtual std::unique_ptr<FMTyieldhandler> presolve(const FMTmaskfilter& filter, const std::vector<FMTtheme>& newthemes) const;
	virtual std::unique_ptr<FMTyieldhandler> postsolve(const FMTmaskfilter& filter,const std::vector<FMTtheme>& basethemes) const;
	virtual std::unique_ptr<FMTyieldhandler> getFromFactor(const double& factor,
		std::vector<std::string>yieldnames = std::vector<std::string>()) const;
	virtual int getEndPoint(const std::string& yld, const int& lowerstep, const double& bound, const double& value) const;
	virtual double getPeak(const FMTyieldrequest& request,const std::string& yld, const int& targetage) const;
	virtual int getAge(const FMTyieldrequest& request, const FMTspec& spec) const;
	virtual double getYieldLinearValue(const std::string&yldname, const FMTyieldrequest& request, bool allowoutofrange=true) const;
protected:
	FMTmask mask;
	std::vector<int>bases;
	mutable std::unordered_set<std::string>lookat;
	static bool basePushData(std::map<std::string, FMTdata, cmpYieldString>& elements, const std::string& yld, const double& value);
	static bool basePushData(std::map<std::string, FMTdata, cmpYieldString>& elements, const std::string& yld, const FMTdata& data);
	double getChangesFrom(const int& targetage, const int& peakstep) const;
	int getMaxBase(const FMTyieldrequest& request) const;
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
