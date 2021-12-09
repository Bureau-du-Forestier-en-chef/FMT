/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifndef FMTCARBONPREDICTOR_H
#define FMTCARBONPREDICTOR_H


#include <vector>
#include <string>
#include <map>
#include "FMTutility.hpp"



namespace Core
{
	class FMTyields;
	class FMTaction;
}

namespace Graph
{
	class FMTbasevertexproperties;
	class FMTbaseedgeproperties;
	class FMTEXPORT FMTcarbonpredictor
	{
		const FMTbasevertexproperties* source_vertex;
		const FMTbasevertexproperties* target_vertex;
		std::vector<double>source_yields;
		std::vector<double>target_yields;
		std::vector<int>periodgaps;
		std::vector<int>sourceactions;
		std::vector<double>getyields(const FMTbasevertexproperties& vertex,const Core::FMTyields& yields, const std::vector<std::string>& yieldnames) const;
	public:
		FMTcarbonpredictor() = default;
		~FMTcarbonpredictor() = default;
		FMTcarbonpredictor(const std::vector<Core::FMTaction>& actions, const std::vector<std::string>& yieldnames,const Core::FMTyields& yields,
			const FMTbasevertexproperties& source, const FMTbasevertexproperties& target, const std::vector<const FMTbaseedgeproperties*>& edges, const std::vector<int>& gaps,bool withGCBMid = true);
		FMTcarbonpredictor(const FMTcarbonpredictor& rhs);
		FMTcarbonpredictor& operator = (const FMTcarbonpredictor& rhs);
		bool operator==(const FMTcarbonpredictor& rhs) const;
		bool operator<(const FMTcarbonpredictor& rhs) const;
		std::vector<double>getpredictors() const;
		std::vector<std::string>getpredictornames(const std::vector<std::string>& yieldnames)const;
	};
}

#endif // carbonpredictor
