/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTcarbonpredictor.h"
#include "FMTaction.h"
#include "FMTdevelopment.h"
#include "FMTyieldrequest.h"
#include "FMTyields.h"

namespace Graph
{

	std::vector<double>FMTcarbonpredictor::getyields(const FMTbasevertexproperties& vertex,const Core::FMTyields& yields, const std::vector<std::string>& yieldnames) const
	{
		std::vector<double>values;
		values.reserve(yieldnames.size());
		const Core::FMTyieldrequest request = vertex.get().getyieldrequest();
		for (const std::string& yldname : yieldnames)
		{
			values.push_back(yields.get(request, yldname));
		}
		return values;
	}

	FMTcarbonpredictor::FMTcarbonpredictor(const std::vector<int>& actionsindex, const std::vector<std::string>& yieldnames, const Core::FMTyields& yields,
		const FMTbasevertexproperties& source, const FMTbasevertexproperties& target, const std::vector<const FMTbaseedgeproperties*>& edges, const std::vector<int>& gaps):
		source_vertex(&source),
		target_vertex(&target),
		source_yields(getyields(source, yields, yieldnames)),
		target_yields(getyields(target, yields, yieldnames)),
		periodgaps(gaps),
		sourceactions()
	{
		size_t location = 0;
		//bool lastnotedone = true;
		for (const FMTbaseedgeproperties* edgeprop : edges)
		{
			if (edgeprop != nullptr)
			{
				if (edgeprop->getactionID()<0)
				{
					sourceactions.push_back(FMTGCBMGROWTHID);
				}else {
					sourceactions.push_back(actionsindex.at(edgeprop->getactionID()));
					}
			}else {
				sourceactions.push_back(FMTGCBMUNKNOWNID);
			}
			++location;
		}

	}

	FMTcarbonpredictor::FMTcarbonpredictor(const FMTcarbonpredictor& rhs) :
		source_vertex(rhs.source_vertex),
		target_vertex(rhs.target_vertex),
		source_yields(rhs.source_yields),
		target_yields(rhs.target_yields),
		periodgaps(rhs.periodgaps),
		sourceactions(rhs.sourceactions)
	{

	}
	FMTcarbonpredictor& FMTcarbonpredictor::operator = (const FMTcarbonpredictor& rhs)
	{
		if (this!=&rhs)
		{
			source_vertex = rhs.source_vertex;
			target_vertex = rhs.target_vertex;
			source_yields = rhs.source_yields;
			target_yields = rhs.target_yields;
			periodgaps = rhs.periodgaps;
			sourceactions=rhs.sourceactions;
		}
	return *this;
	}


	bool FMTcarbonpredictor::operator==(const FMTcarbonpredictor& rhs) const
		{
		return (getpredictors() == rhs.getpredictors());
		}
	bool FMTcarbonpredictor::operator<(const FMTcarbonpredictor& rhs) const
	{
		//strict ordering
		if (getpredictors() < rhs.getpredictors())
			return true;
		if (rhs.getpredictors() < getpredictors())
			return false;
		return false;
	}
	std::vector<double>FMTcarbonpredictor::getpredictors() const
	{
		std::vector<double>returned;
		for (size_t actid = 1; actid < sourceactions.size();++actid)
			{
			if (periodgaps.at(actid)<0)
				{
				returned.push_back(std::numeric_limits<double>::signaling_NaN());
				returned.push_back(std::numeric_limits<double>::signaling_NaN());
			}else {
				returned.push_back(static_cast<double>(periodgaps.at(actid)));
				returned.push_back(static_cast<double>(sourceactions.at(actid)));
				}
			}
		returned.push_back(static_cast<double>(source_vertex->get().getage()));
		//returned.push_back(static_cast<double>(source_vertex->get().period));
		returned.insert(returned.end(), source_yields.begin(), source_yields.end());
		double gap = periodgaps.at(0);
		if (sourceactions.at(0)==-2)
		{
			gap = std::numeric_limits<double>::signaling_NaN();
		}
		returned.push_back(static_cast<double>(gap));
		returned.push_back(static_cast<double>(sourceactions.at(0)));
		returned.push_back(static_cast<double>(target_vertex->get().getage()));
		//returned.push_back(static_cast<double>(target_vertex->get().period));
		returned.insert(returned.end(), target_yields.begin(), target_yields.end());
		returned.shrink_to_fit();
		return returned;
	}

	std::vector<std::string>FMTcarbonpredictor::getpredictornames(const std::vector<std::string>& yieldnames)const
	{
		std::vector<std::string>predictornames;
		const std::vector<std::string>devpredictornames = { "disturbance","age","period" };
		for (size_t actid = 1; actid < sourceactions.size(); ++actid)
			{
			const std::string distname = "s" + std::to_string(actid) + "_disturbance";
			const std::string distgap = "s" + std::to_string(actid) + "_distance";
			predictornames.push_back(distgap);
			predictornames.push_back(distname);
			
			}
		predictornames.push_back("source_age");
		//predictornames.push_back("source_period");
		for (const std::string& name : yieldnames)
			{
			predictornames.push_back("source_" + name);
			}
		predictornames.push_back("target_distance");
		predictornames.push_back("target_disturbance");
		predictornames.push_back("target_age");
		//predictornames.push_back("target_period");
		for (const std::string& name : yieldnames)
		{
			predictornames.push_back("target_" + name);
		}
		predictornames.shrink_to_fit();
		return predictornames;
	}

}
