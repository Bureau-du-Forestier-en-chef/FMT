/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTYLDDATA_H_INCLUDED
#define FMTYLDDATA_H_INCLUDED

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

class FMTEXPORT FMTyieldhandler : public FMTobject
{
public:
	virtual size_t getoverrideindex() const;
	virtual std::vector<size_t> gettabous() const;
	virtual void setoverrideindex(const size_t& newindex);
	virtual ~FMTyieldhandler() = default;
	FMTyieldhandler() = default;
	FMTyieldhandler(const FMTyieldhandler& rhs) = default;
	FMTyieldhandler& operator = (const FMTyieldhandler& rhs) = default;
	FMTyieldhandler(const FMTmask& lmask);
	double getlinearvalue(const std::vector<double>& dls, const int& agetarget, bool allowoutofrange) const;
	virtual int getlastbase() const;
	const std::vector<int>& getbases() const;
	FMTmask getmask() const;
	void setMask(const FMTmask& p_mask);
	bool push_base(const int& base);
	void setbase(const std::vector<int>& allvalues);
	bool inlookat(const std::string& yld) const;
	virtual double getlastvalue(const std::string yld) const;
	virtual double get(const std::string& yld, const FMTyieldrequest& request) const;
	virtual bool push_data(const std::string& yld, const double& value);
	virtual bool push_data(const std::string& yld, const FMTdata& data);
	virtual std::vector<std::string> indexes(const std::vector<std::string>& names) const;
	virtual  operator std::string() const;
	virtual std::vector<std::string> compare(const std::vector<std::string>& keys) const;
	virtual std::unique_ptr<FMTyieldhandler>clone() const;
	virtual bool operator == (const FMTyieldhandler& rhs) const;
	virtual bool empty() const;
	virtual size_t size() const;
	virtual FMTyldtype gettype() const;
	virtual FMTdata& operator[](const std::string& yldname);
	virtual const FMTdata& at(const std::string& yldname) const;
	virtual bool containsyield(const std::string& yldname) const;
	virtual bool isnullyield(const std::string& yldname) const;
	virtual std::vector<std::string>getyieldnames() const;
	virtual void clearcache();
	virtual std::map<std::string, std::vector<double>>getallyieldsdata(const int& maxbase)const;
	virtual std::unique_ptr<FMTyieldhandler> presolve(const FMTmaskfilter& filter, const std::vector<FMTtheme>& newthemes) const;
	virtual std::unique_ptr<FMTyieldhandler> postsolve(const FMTmaskfilter& filter,const std::vector<FMTtheme>& basethemes) const;
	virtual std::unique_ptr<FMTyieldhandler> getfromfactor(const double& factor,
		std::vector<std::string>yieldnames = std::vector<std::string>()) const;
	virtual int getendpoint(const std::string& yld, const int& lowerstep, const double& bound, const double& value) const;
	virtual double getpeak(const FMTyieldrequest& request,const std::string& yld, const int& targetage) const;
	virtual int getage(const FMTyieldrequest& request, const FMTspec& spec) const;
	virtual double getyieldlinearvalue(const std::string&yldname, const FMTyieldrequest& request, bool allowoutofrange=true) const;
protected:
	FMTmask mask;
	std::vector<int>bases;
	mutable std::unordered_set<std::string>lookat;
	static bool basepush_data(std::map<std::string, FMTdata>& elements, const std::string& yld, const double& value);
	static bool basepush_data(std::map<std::string, FMTdata>& elements, const std::string& yld, const FMTdata& data);
	double getchangesfrom(const int& targetage, const int& peakstep) const;
	int getmaxbase(const FMTyieldrequest& request) const;
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
#endif // FMTYLDDATA_H_INCLUDED
