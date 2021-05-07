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
#include "FMTdata.h"
#include "FMTmask.h"
#include "FMTtheme.h"
#include <vector>
#include "FMTutility.h"
#include "FMTobject.h"

namespace Core
{

class FMTyieldhandler : public FMTobject
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{

		ar & BOOST_SERIALIZATION_NVP(yldtype);
		ar & BOOST_SERIALIZATION_NVP(mask);
		ar & BOOST_SERIALIZATION_NVP(bases);
		ar & BOOST_SERIALIZATION_NVP(elements);
	}
        FMTyldtype yldtype;
        FMTmask mask;
		std::vector<int>bases;
		mutable std::unordered_set<std::string>lookat;
    public:
		std::map<std::string,FMTdata>elements;
        operator std::string() const;
		FMTyieldhandler()=default;
		~FMTyieldhandler() = default;
        FMTyieldhandler(FMTyldtype ltype,const FMTmask& lmask);
        FMTyieldhandler(const FMTyieldhandler& rhs);
        FMTyieldhandler& operator = (const FMTyieldhandler& rhs);
		std::vector<std::string> compare(const std::vector<std::string>& keys) const;
        bool push_base(const int& base);
        bool push_data(const std::string& yld,const double& value);
        bool push_data(const std::string& yld,const FMTdata& data);
		std::vector<std::string> indexes(const std::vector<std::string>& names) const;
		double get(const std::vector<const FMTyieldhandler*>& datas,
                   const std::string yld,const int& age,const int& period,
					const FMTmask& resume_mask) const;
		int getlastbase() const;
		const std::vector<int>& getbases() const;
		double getlinearvalue(const std::vector<double>& dls, const int& agetarget) const;
		double getlastvalue(const std::string yld) const;
		std::map<std::string,double> getsources(const std::map<std::string, const FMTyieldhandler*>& srcdata, const std::vector<const FMTyieldhandler*>& datas,
										const int& age, const int& period ,const FMTmask& resume_mask, bool& age_only) const;
		std::vector<double>getsourcesarray(const std::map<std::string, const FMTyieldhandler*>& srcdata, const std::vector<const FMTyieldhandler*>& datas,
			const int& age, const int& period, const FMTmask& resume_mask, bool& age_only) const;
		std::map<std::string,const FMTyieldhandler*> getdata(const std::vector<const FMTyieldhandler*>& datas,
                                    const std::vector<std::string>& names, const std::string& original) const;
		bool operator == (const FMTyieldhandler& rhs) const;
		double getchangesfrom(const int& targetage,const int& peakstep) const;
		int getendpoint(const std::string& yld, const int& lowerstep, const double& bound, const double& value) const;
		double getpeakfrom(const std::string& yld,double maxvalue = std::numeric_limits<double>::lowest()) const;
        double getpeak(const std::string& yld, const int& targetage) const;
        int getage(const std::string yld, const double& value,const int& starting_age) const;
		std::map<std::string,FMTdata>getdataelements() const;
        FMTyldtype gettype() const;
		FMTmask getmask() const;
		FMTyieldhandler presolve(const FMTmask& presolvedmask, const std::vector<FMTtheme>& newthemes) const;
		FMTyieldhandler postsolve(const FMTmask& selectedmask, const std::vector<FMTtheme>& basethemes) const;
		FMTyieldhandler getfromfactor(const double& factor,
			std::vector<std::string>yieldnames = std::vector<std::string>()) const;
    };
}
#endif // FMTYLDDATA_H_INCLUDED
