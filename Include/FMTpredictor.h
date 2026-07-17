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
	class FMTYields;
	class FMTAction;
}

namespace Graph
{
	class FMTBaseVertexProperties;
	class FMTBaseEdgeProperties;
	class FMTEXPORT FMTPredictor
	{
	public:
		FMTPredictor();
		~FMTPredictor() = default;
		FMTPredictor(const std::vector<Core::FMTAction>& actions, const std::vector<std::string>& yieldnames,const Core::FMTYields& yields,
			const FMTBaseVertexProperties& source, const FMTBaseVertexProperties& target, const std::vector<const FMTBaseEdgeProperties*>& edges, const std::vector<int>& gaps,bool withGCBMid = true);
		FMTPredictor(const FMTPredictor& rhs);
		FMTPredictor& operator = (const FMTPredictor& rhs);
		bool operator==(const FMTPredictor& rhs) const;
		bool operator<(const FMTPredictor& rhs) const;
		std::vector<double>getPredictors() const;
		double getDistance(size_t actId) const;
		double getDisturbance(size_t actId) const;
		double getSourceAge() const;
		std::vector<double> getSourceYields() const;
		double getTargetAge() const;
		std::vector<double> getTargetYields() const;
		std::vector<std::string>getPredictorNames(const std::vector<std::string>& yieldnames)const;
	private:
		const FMTBaseVertexProperties* source_vertex;
		const FMTBaseVertexProperties* target_vertex;
		std::vector<double>source_yields;
		std::vector<double>target_yields;
		std::vector<int>periodgaps;
		std::vector<int>sourceactions;
		std::vector<double>getYields(const FMTBaseVertexProperties& vertex, const Core::FMTYields& yields, const std::vector<std::string>& yieldnames) const;
	};
}

#endif // predictor
