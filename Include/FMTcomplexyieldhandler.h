/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTCOMPLEXYIELDHANDLER_Hm_included
#define FMTCOMPLEXYIELDHANDLER_Hm_included

#include "FMTyieldhandler.h"
#include "FMTdata.h"
#include <string>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include "FMTYieldsCache.h"
#include <memory>

namespace Core
{
	class FMTEXPORT FMTComplexYieldHandler final : public FMTYieldHandler
	{
	public:
		void setTabou(const size_t& index);
		void setTabou(const FMTComplexYieldHandler& rhs);
		std::vector<size_t>getTabous() const override;
		virtual void setOverrideIndex(const size_t& newindex);
		virtual size_t  getOverrideIndex() const;
		virtual int getLastBase() const;
		virtual double get(const std::string& yld, const FMTYieldRequest& request) const;
		bool compareSources(const std::string& yield,const FMTComplexYieldHandler& overridedyield) const;
		virtual  operator std::string() const;
		~FMTComplexYieldHandler();
		FMTComplexYieldHandler();
		FMTComplexYieldHandler(const FMTComplexYieldHandler& rhs) = default;
		FMTComplexYieldHandler& operator = (const FMTComplexYieldHandler& rhs) = default;
		FMTComplexYieldHandler(const FMTMask& mask);
		const std::map<std::string, FMTData,cmpYieldString>& getDataElements() const;
		virtual std::vector<std::string> indexes(const std::vector<std::string>& names) const;
		virtual double getPeak(const FMTYieldRequest& request, const std::string& yld, const int& targetage) const;
		std::unique_ptr<FMTYieldHandler>complexYldToAgeYld(const FMTYieldRequest& request, const FMTSpec& lspec) const;
		virtual bool pushData(const std::string& yld, const double& value);
		virtual bool pushData(const std::string& yld, const FMTData& data);
		virtual std::unique_ptr<FMTYieldHandler>clone() const;
		virtual bool operator == (const FMTComplexYieldHandler& rhs) const;
		virtual bool empty() const;
		virtual size_t size() const;
		virtual FMTyldtype getType() const;
		virtual FMTData& operator[](const std::string& yldname);
		virtual const FMTData& at(const std::string& yldname) const;
		virtual bool containsYield(const std::string& yldname) const;
		virtual std::vector<std::string>getYieldNames() const;
		virtual void clearCache();
		virtual int getAge(const FMTYieldRequest& request, const FMTSpec& spec) const;
		virtual double getYieldLinearValue(const std::string& yldname, const FMTYieldRequest& request, bool allowoutofrange=true) const;
	private:
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("FMTyieldhandler", boost::serialization::base_object<FMTYieldHandler>(*this));
			ar& BOOST_SERIALIZATION_NVP(m_elements);
		}
		std::map<std::string, FMTData, cmpYieldString>m_elements;
		std::map<std::string, double> getSources(const std::map<std::string, const std::unique_ptr<FMTYieldHandler>*>& srcdata,
			const FMTYieldRequest& request, bool& age_only) const;

		std::vector<const std::unique_ptr<FMTYieldHandler>*>_getData(const FMTYieldRequest& request,
			const std::vector<const std::string*>& names, const std::string& original) const;

		static std::map<std::string, double>_toMap(const FMTYieldRequest& p_request,
											const std::vector<const std::string*>& p_names,
											const std::vector<const std::unique_ptr<FMTYieldHandler>*>& p_data);

		std::vector<double>getSourcesArray(const std::map<std::string, const std::unique_ptr<FMTYieldHandler>*>& srcdata,
			const FMTYieldRequest& request, bool& age_only) const;
		std::unique_ptr<FMTYieldHandler>toAgeYld(const FMTYieldRequest& request,
			const std::vector<std::string>& yieldnames, const int& minage, const int& maxage) const;
		double _getRange(const FMTData* p_data,const std::string& p_yld, const FMTYieldRequest& p_request) const;
		double _getMultiply(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const;
		double _getSum(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const;
		double _getSubstract(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const;
		double _getDivide(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const;
		double _getYTP(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const;
		double _getMAI(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const;
		double _getCAI(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const;
		double _getEquation(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const;
		double _getEndPoint(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const;
		double _getDelta(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const;
		double _getDistance(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const;
		double _getMax(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const;
		double _getMin(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const;
		std::unordered_set<size_t>overridetabou;
		size_t overrideindex;
		mutable FMTYieldsCache _cache;
	};

}

#endif 


