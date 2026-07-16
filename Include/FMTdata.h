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

class FMTEXPORT FMTData
    {
    public:
		std::vector<double> data;
        FMTData();
		~FMTData()=default;
        FMTData(const FMTData& rhs);
		FMTData(const std::vector<double>& lvalues,
			const FMTyieldparserop& lops,
			const std::vector<std::string>& lsource);
        FMTData(const std::vector<double>& lvalues,
                const FMTyieldparserop& lops,
                const std::vector<std::string>& lsource,
				const std::vector<bool>& varstack);
        FMTData& operator = (const FMTData& rhs);
        FMTyieldparserop getOp() const;
        void clearCache();
		bool constant() const;
		bool nullData() const;
		bool cacheValue(const FMTYieldRequest& request) const;
		double get(const FMTYieldRequest& request) const;
		void set(const double& value, const FMTYieldRequest& request, const bool& age_only) const;
		std::vector<const std::string*> getSources() const;
		std::vector<std::string> getSourcesCopy() const;
		std::vector<const double*>getValues() const;
		//std::vector<double>tovalues(const std::map<std::string, double>& sources) const;
		FMTExpression toExpression() const;
		bool operator == (const FMTData& rhs) const;
        operator std::string() const;
		FMTData operator * (const double& factor) const;
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
		//mutable std::unique_ptr<boost::unordered_map<FMTDevelopment,double>> _cache; //hash base on (age and/or period and/or mask) //only for complex yield!!!!
		mutable std::unique_ptr<std::map<FMTDevelopment, double>> _cache; //hash base on (age and/or period and/or mask) //only for complex yield!!!!
		mutable bool _agebase;
		//size_t hashdata(const int& period, const int& age, const FMTMask& mask) const;
		FMTDevelopment getSummaryDevelopment(const FMTYieldRequest& request) const;
		void allocateCache() const;
		void deAllocateCache() const;
    };
}

BOOST_CLASS_EXPORT_KEY(Core::FMTData)


#endif // FMTDATA_Hm_included
