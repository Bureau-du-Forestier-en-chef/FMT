/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTcarbonpredictor.h"

namespace Graph
{

	std::vector<double>FMTcarbonpredictor::getyields(const FMTbasevertexproperties& vertex,const Core::FMTyields& yields, const std::vector<std::string>& yieldnames) const
	{
		return yields.get(vertex.get(), yieldnames);
	}


	FMTcarbonpredictor::FMTcarbonpredictor(const std::vector<std::string>& yieldnames, const Core::FMTyields& yields,
		const FMTbasevertexproperties& source, const FMTbasevertexproperties& target,
		const FMTbaseedgeproperties& edge,const FMTbaseedgeproperties* lastedge,int lperiodgap):
		source_edge(lastedge),
		target_edge(&edge),
		source_vertex(&source),
		target_vertex(&target),
		source_yields(getyields(source,yields,yieldnames)),
		target_yields(getyields(target, yields, yieldnames)),
		periodgap(lperiodgap)


	{

	}
	FMTcarbonpredictor::FMTcarbonpredictor(const FMTcarbonpredictor& rhs) :
		source_edge(rhs.source_edge),
		target_edge(rhs.target_edge),
		source_vertex(rhs.source_vertex),
		target_vertex(rhs.target_vertex),
		source_yields(rhs.source_yields),
		target_yields(rhs.target_yields),
		periodgap(rhs.periodgap)
	{

	}
	FMTcarbonpredictor& FMTcarbonpredictor::operator = (const FMTcarbonpredictor& rhs)
	{
		if (this!=&rhs)
		{
			source_edge = rhs.source_edge;
			target_edge = rhs.target_edge;
			source_vertex = rhs.source_vertex;
			target_vertex = rhs.target_vertex;
			source_yields = rhs.source_yields;
			target_yields = rhs.target_yields;
			periodgap = rhs.periodgap;
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
		std::vector<double>returned(5,0);
		double sourceedgevalue = -2;
		if (source_edge != nullptr)
			{
			sourceedgevalue = source_edge->getactionID();
			}
		returned[0] = static_cast<double>(periodgap);
		returned[1]= sourceedgevalue;
		returned[2]= static_cast<double>(source_vertex->get().age);
		returned[3] = static_cast<double>(source_vertex->get().lock);
		returned[4] = static_cast<double>(source_vertex->get().period);
		returned.insert(returned.end(), source_yields.begin(), source_yields.end());
		const size_t thesize = returned.size();
		returned.resize(thesize + 4);
		returned[thesize] = static_cast<double>(target_edge->getactionID());
		returned[thesize+1] = static_cast<double>(target_vertex->get().age);
		returned[thesize+2] = static_cast<double>(target_vertex->get().lock);
		returned[thesize+3] = static_cast<double>(target_vertex->get().period);
		returned.insert(returned.end(), target_yields.begin(), target_yields.end());
		return returned;
	}

	std::map<std::string, double>FMTcarbonpredictor::getpredictorsmap(const std::vector<std::string>& yieldnames)const
	{
		const std::vector<std::string>devpredictornames = { "disturbance","age","lock","period" };
		std::vector<std::string>predictornames;
		predictornames.push_back("source_distance");
		size_t id = 0;
		for (const std::string& name : devpredictornames)
			{
			predictornames.push_back(std::to_string(id)+"_source_"+name);
			++id;
			}
		for (const std::string& name : yieldnames)
			{
			predictornames.push_back(std::to_string(id) + "_source_" + name);
			++id;
			}
		for (const std::string& name : devpredictornames)
			{
			predictornames.push_back(std::to_string(id) + "_target_" + name);
			++id;
			}
		for (const std::string& name : yieldnames)
			{
			predictornames.push_back(std::to_string(id) + "_target_" + name);
			++id;
			}
		const std::vector<double>predictor_values= getpredictors();
		std::map<std::string, double>mapping;
		for (size_t predid = 0; predid < predictor_values.size();++predid)
			{
			mapping[predictornames.at(predid)] = predictor_values.at(predid);
			}
		return mapping;
	}

}