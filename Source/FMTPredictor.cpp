/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTPredictor.h"
#include "FMTModel.h"
#include "FMTDevelopment.h"
#include "FMTYieldRequest.h"
#include "FMTBaseVertexProperties.h"
#include "FMTBaseEdgeProperties.h"

namespace Graph
{

	FMTPredictor::FMTPredictor():
		m_source_vertex(nullptr),
		m_target_vertex(nullptr),
		m_source_yields(),
		m_target_yields(),
		m_periodgaps(),
		m_sourceactions()
	{

	}

	std::vector<double>FMTPredictor::_getYields(const FMTBaseVertexProperties& vertex,const Core::FMTYields& yields, const std::vector<std::string>& yieldnames) const
	{
		std::vector<double>values;
		values.reserve(yieldnames.size());
		const Core::FMTYieldRequest request = vertex.get().getYieldRequest();
		for (const std::string& yldname : yieldnames)
		{
			values.push_back(yields.get(request, yldname));
		}
		return values;
	}

	FMTPredictor::FMTPredictor(const std::vector<Core::FMTAction>& actions, const std::vector<std::string>& yieldnames,const Core::FMTYields& yields,
		const FMTBaseVertexProperties& source, const FMTBaseVertexProperties& target, const std::vector<const FMTBaseEdgeProperties*>& edges, const std::vector<int>& gaps,bool withGCBMid):
		m_source_vertex(&source),
		m_target_vertex(&target),
		m_source_yields(_getYields(source, yields, yieldnames)),
		m_target_yields(_getYields(target, yields, yieldnames)),
		m_periodgaps(gaps),
		m_sourceactions()
	{
		size_t location = 0;
		//bool lastnotedone = true;
		for (const FMTBaseEdgeProperties* edgeprop : edges)
		{
			if (edgeprop != nullptr)
			{
				if (edgeprop->getactionID() < 0)
				{
					if(withGCBMid)
					{
						m_sourceactions.push_back(FMTGCBMGROWTHID);
					}else{
						m_sourceactions.push_back(-1);
					}
				}
				else {
					if(withGCBMid)
					{
						m_sourceactions.push_back(actions.at(edgeprop->getactionID()).getGCBMActionId());
					}else{
						m_sourceactions.push_back(edgeprop->getactionID());
					}
					
				}
			}
			else {
				if(withGCBMid)
				{
					m_sourceactions.push_back(FMTGCBMWILDFIREID);//FMTGCBMUNKNOWNID);
				}else{
					m_sourceactions.push_back(-2);
				}
				
			}
			++location;
		}
	}


	FMTPredictor::FMTPredictor(const FMTPredictor& rhs) :
		m_source_vertex(rhs.m_source_vertex),
		m_target_vertex(rhs.m_target_vertex),
		m_source_yields(rhs.m_source_yields),
		m_target_yields(rhs.m_target_yields),
		m_periodgaps(rhs.m_periodgaps),
		m_sourceactions(rhs.m_sourceactions)
	{

	}
	FMTPredictor& FMTPredictor::operator = (const FMTPredictor& rhs)
	{
		if (this!=&rhs)
		{
			m_source_vertex = rhs.m_source_vertex;
			m_target_vertex = rhs.m_target_vertex;
			m_source_yields = rhs.m_source_yields;
			m_target_yields = rhs.m_target_yields;
			m_periodgaps = rhs.m_periodgaps;
			m_sourceactions=rhs.m_sourceactions;
		}
	return *this;
	}


	bool FMTPredictor::operator==(const FMTPredictor& rhs) const
		{
		return (getPredictors() == rhs.getPredictors());
		}
	bool FMTPredictor::operator<(const FMTPredictor& rhs) const
	{
		//strict ordering
		if (getPredictors() < rhs.getPredictors())
			return true;
		if (rhs.getPredictors() < getPredictors())
			return false;
		return false;
	}
	std::vector<double>FMTPredictor::getPredictors() const
	{
		std::vector<double>returned;
		for (size_t actid = 1; actid < m_sourceactions.size();++actid)
			{
			if (m_periodgaps.at(actid)<0)
				{
				returned.push_back(std::numeric_limits<double>::signaling_NaN());
				returned.push_back(std::numeric_limits<double>::signaling_NaN());
			}else {
				returned.push_back(static_cast<double>(m_periodgaps.at(actid)));
				returned.push_back(static_cast<double>(m_sourceactions.at(actid)));
				}
			}
		returned.push_back(static_cast<double>(m_source_vertex->get().getAge()));
		//returned.push_back(static_cast<double>(source_vertex->get().period));
		returned.insert(returned.end(), m_source_yields.begin(), m_source_yields.end());
		double gap = m_periodgaps.at(0);
		if (m_sourceactions.at(0)==-2)
		{
			gap = std::numeric_limits<double>::signaling_NaN();
		}
		returned.push_back(static_cast<double>(gap));
		returned.push_back(static_cast<double>(m_sourceactions.at(0)));
		returned.push_back(static_cast<double>(m_target_vertex->get().getAge()));
		//returned.push_back(static_cast<double>(target_vertex->get().period));
		returned.insert(returned.end(), m_target_yields.begin(), m_target_yields.end());
		returned.shrink_to_fit();
		return returned;
	}

	double FMTPredictor::getDistance(size_t actId) const
	{
		double gap = m_periodgaps.at(actId);
		if (actId == 0)
		{
			if (m_sourceactions.at(0) == -2)
				gap = std::numeric_limits<double>::signaling_NaN();
		}
		else if (gap < 0)
				gap = std::numeric_limits<double>::signaling_NaN();

		return gap;
	}

	double FMTPredictor::getDisturbance(size_t actId) const
	{
		double dist = m_sourceactions.at(actId);
		if(actId > 0 && m_periodgaps.at(actId) < 0)
		{
			dist = std::numeric_limits<double>::signaling_NaN();;
		}

		return dist;
	}

	double FMTPredictor::getSourceAge() const
	{
		return static_cast<double>(m_source_vertex->get().getAge());
	}

	std::vector<double> FMTPredictor::getSourceYields() const
	{
		return m_source_yields;
	}

	double FMTPredictor::getTargetAge() const
	{
		return static_cast<double>(m_target_vertex->get().getAge());
	}

	std::vector<double> FMTPredictor::getTargetYields() const
	{
		return m_target_yields;
	}

	std::vector<std::string>FMTPredictor::getPredictorNames(const std::vector<std::string>& yieldnames)const
	{
		std::vector<std::string>predictornames;
		const std::vector<std::string>devpredictornames = { "disturbance","age","period" };
		for (size_t actid = 1; actid < m_sourceactions.size(); ++actid)
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
