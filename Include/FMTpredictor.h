/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifndef FMTPREDICTOR_H
#define FMTPREDICTOR_H


#include <vector>
#include <string>
#include <map>
#include "FMTutility.h"



namespace Core
{
	class FMTyields;
	class FMTaction;
}

namespace Graph
{
	class FMTbasevertexproperties;
	class FMTbaseedgeproperties;
	class FMTEXPORT FMTpredictor
	{
		const FMTbasevertexproperties* source_vertex;
		const FMTbasevertexproperties* target_vertex;
		std::vector<double>source_yields;
		std::vector<double>target_yields;
		std::vector<int>periodgaps;
		std::vector<int>sourceactions;
		std::vector<double>getyields(const FMTbasevertexproperties& vertex,const Core::FMTyields& yields, const std::vector<std::string>& yieldnames) const;
	public:
		FMTpredictor() = default;
		~FMTpredictor() = default;
		FMTpredictor(const std::vector<Core::FMTaction>& actions, const std::vector<std::string>& yieldnames,const Core::FMTyields& yields,
			const FMTbasevertexproperties& source, const FMTbasevertexproperties& target, const std::vector<const FMTbaseedgeproperties*>& edges, const std::vector<int>& gaps,bool withGCBMid = true);
		FMTpredictor(const FMTpredictor& rhs);
		FMTpredictor& operator = (const FMTpredictor& rhs);
		bool operator==(const FMTpredictor& rhs) const;
		bool operator<(const FMTpredictor& rhs) const;
		std::vector<double>getpredictors() const;
		double getDistance(size_t actId) const;
		double getDisturbance(size_t actId) const;
		double getSourceAge() const;
		std::vector<double> getSourceYields() const;
		double getTargetAge() const;
		std::vector<double> getTargetYields() const;
		std::vector<std::string>getpredictornames(const std::vector<std::string>& yieldnames)const;
	};
}

#endif // predictor
