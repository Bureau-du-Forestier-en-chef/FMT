/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTDATA_H_INCLUDED
#define FMTDATA_H_INCLUDED


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
class FMTdevelopment;
class FMTyieldrequest;
class FMTexpression;
class FMTmask;

class FMTEXPORT FMTdata
    {
    public:
		std::vector<double> data;
        FMTdata();
		~FMTdata()=default;
        FMTdata(const FMTdata& rhs);
		FMTdata(const std::vector<double>& lvalues,
			const FMTyieldparserop& lops,
			const std::vector<std::string>& lsource);
        FMTdata(const std::vector<double>& lvalues,
                const FMTyieldparserop& lops,
                const std::vector<std::string>& lsource,
				const std::vector<bool>& varstack);
        FMTdata& operator = (const FMTdata& rhs);
        FMTyieldparserop getop() const;
        void clearcache();
		bool constant() const;
		bool nulldata() const;
		bool cachevalue(const FMTyieldrequest& request) const;
		double get(const FMTyieldrequest& request) const;
		void set(const double& value, const FMTyieldrequest& request, const bool& age_only) const;
		std::vector<std::string> getsource() const;
		std::vector<double>tovalues(const std::map<std::string, double>& sources) const;
		FMTexpression toexpression() const;
		bool operator == (const FMTdata& rhs) const;
        operator std::string() const;
		FMTdata operator * (const double& factor) const;
	private:
		friend class boost::serialization::access;
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
		//mutable std::unique_ptr<boost::unordered_map<FMTdevelopment,double>> _cache; //hash base on (age and/or period and/or mask) //only for complex yield!!!!
		mutable std::unique_ptr<std::map<FMTdevelopment, double>> _cache; //hash base on (age and/or period and/or mask) //only for complex yield!!!!
		mutable bool _agebase;
		//size_t hashdata(const int& period, const int& age, const FMTmask& mask) const;
		FMTdevelopment getsummarydevelopment(const FMTyieldrequest& request) const;
		void allocateCache() const;
		void deAllocateCache() const;
    };
}

BOOST_CLASS_EXPORT_KEY(Core::FMTdata)


#endif // FMTDATA_H_INCLUDED
