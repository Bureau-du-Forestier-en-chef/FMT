/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#include <map>
#include <vector>
#include "FMTSpatialGraphs.h"
#include "FMTlinegraph.h"
#include "FMTconstraint.h"
#include "FMTmodel.h"
#include "FMTmask.h"
#include "FMTtheme.h"
#include "FMTGraphInfo.h"
#include "FMTVirtualLineGraph.h"
#include "FMTschedule.h"
#include <future>
#include <list>


namespace Spatial
{


	double FMTSpatialGraphs::GetConstraintsValue(
		size_t ConstraintId,
		const std::vector<size_t>& p_solution) const
	{
		double value = 0.0;
		try {
			std::vector<double>results;
			ConstraintId = _GetNonSpatialId(ConstraintId);
#ifndef NDEBUG
			assert(p_solution.size() >= m_LastGraphId);
			assert(ConstraintId < m_Constraints.size());
#endif
			for (size_t GraphFamily : m_Constraints.at(ConstraintId))
			{
				for (const auto& GRAPH : m_AllGraphs.at(GraphFamily))
				{
					_FillConstraintResults(ConstraintId, GRAPH.second,
						results, p_solution);
				}
			}
			if (!results.empty())
			{
				value = m_Model->constraints.at(ConstraintId).evaluate(results);
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialGraphs::GetConstraintsValues",
				__LINE__, __FILE__);
		}
		return value;
	}

	FMTVirtualLineGraph FMTSpatialGraphs::GetVirtualGraph(const Graph::FMTlinegraph& p_LineGraph)
	{
		size_t Family = 0;
		std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator Iterator = m_AllGraphs.begin()->end();
		try {
			Family = _GetFamily(p_LineGraph);
			Iterator = GetIterator(p_LineGraph, Family);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialGraphs::GetVirtualGraph",
				__LINE__, __FILE__);
		}
		return FMTVirtualLineGraph(*this, Iterator, Family);
	}

	FMTVirtualLineGraph FMTSpatialGraphs::SetVirtualGraph(const Graph::FMTlinegraph& p_LineGraph)
	{
		size_t Family = 0;
		std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator Iterator = m_AllGraphs.begin()->end();
		try {
			Family = _GetFamily(p_LineGraph);
			Iterator = SetIterator(p_LineGraph, Family);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialGraphs::SetVirtualGraph",
				__LINE__, __FILE__);
		}
		return FMTVirtualLineGraph(*this, Iterator, Family);
	}

	std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator FMTSpatialGraphs::GetBaseIterator(size_t p_family) const
	{
		std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator Iterator = m_AllGraphs.at(p_family).end();
		try {
			size_t MinId = m_LastGraphId;
			for (std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator It = m_AllGraphs.at(p_family).begin();
				It != m_AllGraphs.at(p_family).end();++It)
				{
				if (It->second.GetGraphId()<MinId)
					{
					MinId = It->second.GetGraphId();
					Iterator = It;
					}
				}
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialGraphs::GetBaseIterator",
				__LINE__, __FILE__);
		}
		return Iterator;
	}

	std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator FMTSpatialGraphs::GetLastPeriodIterator(
		size_t p_family,
		std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator p_iterator) const
	{
		std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator Iterator = m_AllGraphs.at(p_family).end();
		try {
			for (std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator It = m_AllGraphs.at(p_family).begin();
				It != m_AllGraphs.at(p_family).end(); ++It)
			{
				if (p_iterator->first.IsLessPeriod(It->first))
				{
					return It;
				}
			}
			_exhandler->raise(Exception::FMTexc::FMTrangeerror,
				"Cant Get Less period for graph id " + std::to_string(p_iterator->second.GetGraphId())
				, "FMTSpatialGraphs::GetLastPeriodIteratorr", __LINE__, __FILE__);
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialGraphs::GetLastPeriodIterator",
				__LINE__, __FILE__);
		}
		return Iterator;
	}

	void FMTSpatialGraphs::swap(FMTSpatialGraphs& p_Other)
	{
		m_AllGraphs.swap(p_Other.m_AllGraphs);
		m_LastGraphId = p_Other.m_LastGraphId;
		m_Constraints.swap(p_Other.m_Constraints);
		m_GraphsMasks.swap(p_Other.m_GraphsMasks);
		m_BaseSolution.swap(p_Other.m_BaseSolution);
		m_ConstraintsId.swap(p_Other.m_ConstraintsId);
		m_Model = p_Other.m_Model;
	}


	std::vector<size_t>FMTSpatialGraphs::GetBaseSolution() const
	{
		return m_BaseSolution;
	}

	bool FMTSpatialGraphs::IsNotNull(size_t p_family,
		std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator p_iterator) const
	{
		return (p_iterator != m_AllGraphs.at(p_family).end());
	}

	std::vector<Core::FMTschedule> FMTSpatialGraphs::GetSchedules(const std::vector<size_t>& p_Solution,
		bool WithLock) const
	{
		std::vector<Core::FMTschedule> Schedules;
		try {
			for (const auto& FAMILY : m_AllGraphs)
			{
				for (const auto& GRAPH : FAMILY)
				{
					const double CELLS = _GetGraphCells(p_Solution, GRAPH.second);
					if (CELLS > FMT_DBL_TOLERANCE)
					{
						for (int period = 1; period < GRAPH.first.getperiod(); ++period)
						{
							const Core::FMTschedule SCHEDULE = GRAPH.first.getschedule(GetModel().actions,
								&CELLS, period, WithLock);
							if (!SCHEDULE.empty())
							{
								if (period > Schedules.size())
								{
									Schedules.push_back(SCHEDULE);
								}
								else {
									Schedules[period - 1] += SCHEDULE;
								}
							}
							
						}
					}
				}
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialGraphs::GetSchedule",
				__LINE__, __FILE__);
		}
		return  Schedules;
	}

	void FMTSpatialGraphs::AddToSolution(std::vector<size_t>& p_solution,
		size_t p_family,
		std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator p_iterator)
	{
		if (IsNotNull(p_family, p_iterator))
		{
			if (m_LastGraphId > p_solution.size())
			{
				p_solution.resize(m_LastGraphId * SOLUTION_RESIZE_FACTOR, 0);
			}
			++p_solution.at(p_iterator->second.GetGraphId());
		}
	}

	void FMTSpatialGraphs::RemoveToSolution(std::vector<size_t>& p_solution,
		size_t p_family,
		std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator p_iterator)
	{
		if (IsNotNull(p_family, p_iterator))
		{
			--p_solution.at(p_iterator->second.GetGraphId());
		}
	}

	std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator FMTSpatialGraphs::GetIterator(
		const Graph::FMTlinegraph& p_Graph,
		size_t p_family) const
	{
		std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator It = m_AllGraphs.at(p_family).find(p_Graph);
		if (It == m_AllGraphs.at(p_family).end())
		{
			_exhandler->raise(Exception::FMTexc::FMTrangeerror,
				"Empty Iterator for Graph Family " + std::to_string(p_family) +
				" Family size of " + std::to_string(m_AllGraphs.at(p_family).size())
				, "FMTSpatialGraphs::GetIterator", __LINE__, __FILE__);
		}
		return It;
	}

	std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator FMTSpatialGraphs::SetIterator(
		const Graph::FMTlinegraph& p_Graph,
		size_t p_family)
	{
		std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator Iterator = m_AllGraphs.at(p_family).end();
		try {
			std::pair<std::map<Graph::FMTlinegraph, FMTGraphInfo>::iterator, bool> inserted = m_AllGraphs.at(p_family).insert(
				std::pair<Graph::FMTlinegraph, FMTGraphInfo>(p_Graph, FMTGraphInfo(m_LastGraphId)));
			if (inserted.second)//Calculate constraints
			{
				_BuildConstraintsValues(inserted.first, p_family);
				++m_LastGraphId;
			}
			Iterator = inserted.first;
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialGraphs::SetIterator",
				__LINE__, __FILE__);
		}
		return Iterator;

	}




	void FMTSpatialGraphs::SetModel(const Models::FMTmodel& p_model)
	{
		m_Model = &p_model;
	}


	std::map<std::string, double> FMTSpatialGraphs::GetOutput(
		const std::vector<size_t>& p_Solution,
		const Core::FMToutput& p_output,
		int p_period, Core::FMToutputlevel level) const
	{
		std::map<std::string, double> results;
		try {
			for (const auto& FAMILY : m_AllGraphs)
			{
				for (const auto& GRAPH : FAMILY)
				{
					const double CELLS = _GetGraphCells(p_Solution, GRAPH.second);
					if (CELLS > FMT_DBL_TOLERANCE)
					{
						const double GRAPH_AREA = GRAPH.first.getbasedevelopment().getarea();
						const double VALUE = CELLS * GRAPH_AREA;
						const double* SOLUTION = &VALUE;
						for (const auto& DATA : GRAPH.first.getoutput(GetModel(), p_output, p_period, SOLUTION, level))
						{
							if (results.find(DATA.first) == results.end())
							{
								results[DATA.first] = 0.0;
							}
							results[DATA.first] += DATA.second;
						}
					}
				}
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialGraphs::GetOutput",
				__LINE__, __FILE__);
		}
		return results;
	}




	FMTSpatialGraphs::FMTSpatialGraphs(const Models::FMTmodel& p_model, double p_CellSize) :
		m_AllGraphs(),
		m_LastGraphId(0),
		m_Constraints(),
		m_GraphsMasks(),
		m_BaseSolution(),
		m_Model(&p_model)
	{
		_BuildGraphs(p_model, p_CellSize);
		_BuildConstraintsLocator(p_model);
		_BuildConstraintsValues(m_AllGraphs.begin(), m_AllGraphs.end());
	}

	Core::FMTmask FMTSpatialGraphs::_GetUseFullBits(const Models::FMTmodel& p_model)
	{
		Core::FMTmask usefullBits(p_model.themes);
		try {
			const std::vector<const Core::FMTtheme*> STATIC_THEMES = p_model.locatestatictransitionsthemes();
			for (const Core::FMTtheme& THEME : p_model.themes)
			{
				if (std::find(STATIC_THEMES.begin(),
					STATIC_THEMES.end(), &THEME) != STATIC_THEMES.end())
				{
					usefullBits.set(THEME, "?");
				}
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialGraphs::_GetUseFullBits",
				__LINE__, __FILE__);
		}
		return usefullBits;
	}

	void FMTSpatialGraphs::_BuildGraphs(const Models::FMTmodel& p_model, double p_CellSize)
	{
		try {
			const size_t LENGTH = static_cast<size_t>(p_model.getparameter(Models::FMTintmodelparameters::LENGTH));
			const Core::FMTmask USEFULL_BITS = _GetUseFullBits(p_model);
			const std::vector<Core::FMTactualdevelopment>AREAS = p_model.Models::FMTmodel::getarea();
			m_BaseSolution = std::vector<size_t>(AREAS.size());
			for (const auto& DEV : AREAS)
			{
				const Core::FMTmask& DEV_MASK = DEV.getmask();
				Core::FMTmask SORT_MASK = DEV_MASK.getintersect(USEFULL_BITS);
				std::pair<boost::unordered_map<Core::FMTmask, size_t>::iterator, bool> InSertedMask =
					m_GraphsMasks.insert(std::pair<Core::FMTmask, size_t>(SORT_MASK, m_AllGraphs.size()));
				const size_t GRAPHS_LOCATION = InSertedMask.first->second;
				if (GRAPHS_LOCATION == m_AllGraphs.size())
				{
					m_AllGraphs.push_back(std::map<Graph::FMTlinegraph, FMTGraphInfo>());
				}
				Graph::FMTlinegraph local_graph(LENGTH);
				std::vector<Core::FMTactualdevelopment> NewDevs(1, DEV);

				const size_t NUMBER_OF_CELLS = static_cast<size_t> (std::round(NewDevs.begin()->getarea() / p_CellSize));
				NewDevs.begin()->setarea(p_CellSize);
				m_BaseSolution[m_LastGraphId] = NUMBER_OF_CELLS;
				local_graph.initialize(NewDevs);
				//local_graph.grow(LENGTH);
				m_AllGraphs[GRAPHS_LOCATION][local_graph] = FMTGraphInfo(m_LastGraphId);
				++m_LastGraphId;
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialGraphs::_BuildGraphs",
				__LINE__, __FILE__);
		}
	}

	void FMTSpatialGraphs::_BuildConstraintsLocator(const Models::FMTmodel& p_model)
	{
		try {
			const Core::FMTmask USEFULL_BITS = _GetUseFullBits(p_model);
			m_ConstraintsId.clear();
			size_t Id = 0;
			for (const Core::FMTconstraint& CONSTRAINT : p_model.constraints)
			{
				m_ConstraintsId.push_back(Id);
				if (!CONSTRAINT.isspatial())
				{
					std::vector<size_t>selectedGraphs;
					const Core::FMTmask UNION_MASK = CONSTRAINT.getMasksUnion();
					Core::FMTmask Intersect = UNION_MASK.getintersect(UNION_MASK);
					for (const auto& MASKS : m_GraphsMasks)
					{
						if (MASKS.first.isSubsetOf(Intersect))
						{
							selectedGraphs.push_back(MASKS.second);
						}
					}
					std::sort(selectedGraphs.begin(), selectedGraphs.end());
					m_Constraints.push_back(selectedGraphs);
					++Id;
				}
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialGraphs::_BuildConstraintsLocator",
				__LINE__, __FILE__);
		}
	}

	void FMTSpatialGraphs::_BuildConstraintsValues(
		std::map<Graph::FMTlinegraph, FMTGraphInfo>::iterator p_Graph,
		size_t p_family)
	{
		try {
			size_t ConstraintId = 0;
			for (const Core::FMTconstraint& CONSTRAINT : GetModel().constraints)
			{
				if (!CONSTRAINT.isspatial())
				{
					if (std::find(m_Constraints.at(ConstraintId).begin(),
						m_Constraints.at(ConstraintId).end(),
						p_family) != m_Constraints.at(ConstraintId).end())
					{
						int LowestPeriod = CONSTRAINT.getperiodlowerbound();
						int UpperPeriod = CONSTRAINT.getperiodupperbound();
						if (p_Graph->second.IsEmpty())
						{
							p_Graph->second.SetValuesSize(GetModel().constraints.size());
						}
						if (p_Graph->first.constraintlenght(CONSTRAINT, LowestPeriod, UpperPeriod))
						{
							if (CONSTRAINT.acrossperiod())
							{
								++UpperPeriod;
							}
							const double GRAPH_AREA = p_Graph->first.getbasedevelopment().getarea();
							const double* SOLUTION = &GRAPH_AREA;
							size_t i = 0;
							std::vector<double>Values((UpperPeriod - LowestPeriod) + 1, 0.0);
							for (int period = LowestPeriod; period <= UpperPeriod; ++period)
							{
								Values.at(i) = p_Graph->first.getoutput(GetModel(),
									CONSTRAINT, period, SOLUTION, Core::FMToutputlevel::totalonly).at("Total");
								++i;
							}
							p_Graph->second.SetValues(ConstraintId, Values);
						}
					}

					++ConstraintId;
				}

			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialGraphs::_BuildConstraintsValues",
				__LINE__, __FILE__);
		}
	}


	void FMTSpatialGraphs::_BuildConstraintsValues(
		std::vector<std::map<Graph::FMTlinegraph, FMTGraphInfo>>::iterator FirstGraphFamily,
		std::vector<std::map<Graph::FMTlinegraph, FMTGraphInfo>>::iterator GraphFamilyEnd)
	{
		try {
			size_t Family = 0;
			for (std::vector<std::map<Graph::FMTlinegraph, FMTGraphInfo>>::iterator It = FirstGraphFamily; It != GraphFamilyEnd; ++It)
			{
				for (std::map<Graph::FMTlinegraph, FMTGraphInfo>::iterator GraphIt = It->begin();
					GraphIt != It->end(); ++GraphIt)
				{
					_BuildConstraintsValues(GraphIt, Family);
				}
				++Family;
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialGraphs::_BuildConstraintsValues",
				__LINE__, __FILE__);
		}
	}

	size_t FMTSpatialGraphs::_GetFamily(const Graph::FMTlinegraph& p_Graph) const
	{
		try {
			const Core::FMTmask USEFULL_BITS = _GetUseFullBits(GetModel());
			const Core::FMTdevelopment& DEVELOPPEMENT = p_Graph.getbasedevelopment();
			Core::FMTmask DEV_MASK = DEVELOPPEMENT.getmask();
			DEV_MASK = DEV_MASK.getintersect(USEFULL_BITS);
			for (const auto& MASK : m_GraphsMasks)
			{
				if (DEV_MASK == MASK.first)
				{
					return MASK.second;
				}
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialGraphs::_BuildConstraintsLocator",
				__LINE__, __FILE__);
		}
		return 0;
	}

	const Models::FMTmodel& FMTSpatialGraphs::GetModel() const
	{
		return *m_Model;
	}

	size_t FMTSpatialGraphs::_GetNonSpatialId(size_t p_Constraint) const
	{
		try {
			p_Constraint = m_ConstraintsId.at(p_Constraint);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialGraphs::_GetNonSpatialId",
				__LINE__, __FILE__);
		}
		return p_Constraint;
	}

	void FMTSpatialGraphs::_FillConstraintResults(size_t p_Constraint,
		const FMTGraphInfo& p_GraphInfo,
		std::vector<double>& p_results,
		const std::vector<size_t>& p_solution) const
	{
		try {
			const double CELLS = _GetGraphCells(p_solution, p_GraphInfo);
			if (CELLS > FMT_DBL_TOLERANCE)
			{
				if (p_results.empty())
				{
					p_results = std::vector<double>(p_GraphInfo.GetValues(p_Constraint).size(), 0.0);
				}
				size_t i = 0;
				for (double VALUE : p_GraphInfo.GetValues(p_Constraint))
				{
					p_results.at(i) += VALUE * CELLS;
					++i;
				}
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("For constraint Id " + std::to_string(p_Constraint),
				"FMTSpatialGraphs::_FillConstraintResults",
				__LINE__, __FILE__);
		}
	}

	size_t FMTSpatialGraphs::_GetMaxGraphLength(const std::vector<size_t>& p_solution) const
	{
		size_t maxLength = 0;
		for (const auto& FAMILY : m_AllGraphs)
		{
			for (const auto& GRAPH : FAMILY)
			{
				const double CELLS = _GetGraphCells(p_solution, GRAPH.second);
				if (CELLS > FMT_DBL_TOLERANCE)
				{
					maxLength = std::max(maxLength, 
						static_cast<size_t>(GRAPH.first.getperiod()));
				}
			}
		}
		return maxLength;
	}
	size_t FMTSpatialGraphs::_GetMinGraphLength(const std::vector<size_t>& p_solution) const
	{
		size_t minLength = std::numeric_limits<size_t>::max();
		for (const auto& FAMILY : m_AllGraphs)
		{
			for (const auto& GRAPH : FAMILY)
			{
				const double CELLS = _GetGraphCells(p_solution, GRAPH.second);
				if (CELLS > FMT_DBL_TOLERANCE)
				{
					minLength = std::min(minLength,
						static_cast<size_t>(GRAPH.first.getperiod()));
				}
			}
		}
		return minLength;
	}



	double FMTSpatialGraphs::_GetGraphCells(const std::vector<size_t>& p_solution,
													const FMTGraphInfo& p_GraphInfo) const
	{
		double area = 0.0;
		try {
			if (p_GraphInfo.GetGraphId() < p_solution.size() && //Sometime a solution can have a smaller solution vector
				p_solution.at(p_GraphInfo.GetGraphId()) > 0)
			{
				area = static_cast<double>(p_solution.at(p_GraphInfo.GetGraphId()));
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("For Graph id "+std::to_string(p_GraphInfo.GetGraphId())+
													" with solution size of "+ std::to_string(p_solution.size()) +
													" last graph id of "+ std::to_string(m_LastGraphId),
																	"FMTSpatialGraphs::_GetGraphCells",
				__LINE__, __FILE__);
		}
		return area;
	}
	

}


