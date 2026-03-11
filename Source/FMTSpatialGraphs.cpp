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
#include "FMTSolutionTracker.h"
#include "FMTschedule.h"
#include <future>
#include <list>


namespace Spatial
{

	
	std::vector<double> FMTSpatialGraphs::_GetConstraintsValueFromSolution(size_t p_ConstraintId,
		const FMTSolutionTracker& p_solution) const
	{
		std::vector<double>results;
		try {
			for (size_t GraphFamily : m_Constraints.at(p_ConstraintId))
			{
				for (const auto& GRAPH : m_AllGraphs.at(GraphFamily))
				{
					_FillConstraintResults(p_ConstraintId, GRAPH.second,
						results, p_solution);
				}
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTSpatialGraphs::GetConstraintsValues",
				__LINE__, __FILE__);
			}
		return results;
	}
	std::vector<double> FMTSpatialGraphs::_GetConstraintsValueFromTracker(size_t p_ConstraintId,
		const FMTSolutionTracker& p_solution) const
	{
		std::vector<double> results;
		try {
			results = p_solution.GetConstraintsValues(p_ConstraintId);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTSpatialGraphs::_GetConstraintsValueFromTracker",
				__LINE__, __FILE__);
			}
		return results;
	}

	void FMTSpatialGraphs::EnableSolutionTracker(FMTSolutionTracker& p_tracker) const
		{
		try {
			if (!p_tracker.IsTrackingEnable())
				{
				p_tracker.SetConstraintValues(_GetConstraintsValues(p_tracker),
									m_Constraints,m_AllGraphs.size());
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTSpatialGraphs::EnableSolutionTracler",
				__LINE__, __FILE__);
			}
		}


	double FMTSpatialGraphs::GetConstraintsValue(
		size_t p_ConstraintId,
		const FMTSolutionTracker& p_solution) const
	{
		double value = 0.0;
		try {
			const size_t NON_SPATIAL_ID  = _GetNonSpatialId(p_ConstraintId);
			std::vector<double>results;
			if (p_solution.IsTrackingEnable())
			{
			results = _GetConstraintsValueFromTracker(NON_SPATIAL_ID, p_solution);
			}else {
				results = _GetConstraintsValueFromSolution(NON_SPATIAL_ID, p_solution);
				}
			if (!results.empty())
			{
				value = m_Model->constraints.at(p_ConstraintId).evaluate(results);
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
		const_iterator Iterator = m_AllGraphs.begin()->end();
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
		const_iterator Iterator = m_AllGraphs.begin()->end();
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

	FMTSpatialGraphs::const_iterator FMTSpatialGraphs::GetBaseIterator(size_t p_family) const
	{
		const_iterator Iterator = m_AllGraphs.at(p_family).end();
		try {
			size_t MinId = m_LastGraphId;
			for (const_iterator It = m_AllGraphs.at(p_family).begin();
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

	FMTSpatialGraphs::const_iterator FMTSpatialGraphs::GetLastPeriodIterator(
		size_t p_family,
		FMTSpatialGraphs::const_iterator p_iterator) const
	{
		FMTSpatialGraphs::const_iterator Iterator = m_AllGraphs.at(p_family).end();
		try {
			for (FMTSpatialGraphs::const_iterator It = m_AllGraphs.at(p_family).begin();
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

	size_t FMTSpatialGraphs::GetNumberOfCategories(int p_themeId) const
	{
		return m_Model->themes.at(p_themeId).size(); 
	}

	size_t FMTSpatialGraphs::GetCategoryOf(int p_themeId, size_t p_family) const
	{
		const Core::FMTmask& MASK = m_AllGraphs.at(p_family).begin()->first.getbasedevelopment().getmask();
		return std::distance(&*m_Model->themes.at(p_themeId).getbaseattributes().begin(),
			&MASK.getAttribute(m_Model->themes.at(p_themeId)));
	}

	void FMTSpatialGraphs::DeleteNonCompleteGraphs()
	{
		const size_t MAX_GRAPH_LENGTH = _GetMaxGraphLength();
		_RemoveGraphsShorterThan(MAX_GRAPH_LENGTH);
	}





	FMTSolutionTracker FMTSpatialGraphs::GetBaseSolution() const
	{
		return m_BaseSolution;
	}

	bool FMTSpatialGraphs::IsNotNull(size_t p_family,
		FMTSpatialGraphs::const_iterator p_iterator) const
	{
		return (p_iterator != m_AllGraphs.at(p_family).end());
	}

	std::vector<Core::FMTschedule> FMTSpatialGraphs::GetSchedules(const FMTSolutionTracker& p_Solution,
		bool WithLock) const
	{
		std::vector<Core::FMTschedule> Schedules(m_Model->getparameter(Models::FMTintmodelparameters::LENGTH));
		try {
			int SchedulePeriod = 1;
			for (Core::FMTschedule& Schedule : Schedules)
			{
				Schedule.setperiod(SchedulePeriod);
				++SchedulePeriod;
			}
			for (const auto& FAMILY : m_AllGraphs)
			{
				for (const auto& GRAPH : FAMILY)
				{
					const double CELLS = _GetGraphCells(p_Solution, GRAPH.second);
					if (CELLS > FMT_DBL_TOLERANCE)
					{
						const double GRAPH_AREA = CELLS * GRAPH.first.getbasedevelopment().getarea();
						for (int period = 1; period < GRAPH.first.getperiod(); ++period)
						{
							const Core::FMTschedule SCHEDULE = GRAPH.first.getschedule(GetModel().actions,
								&GRAPH_AREA, period, WithLock);
							Schedules[period - 1] += SCHEDULE;
							
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

	void FMTSpatialGraphs::AddToSolution(FMTSolutionTracker& p_solution,
		size_t p_family,
		FMTSpatialGraphs::const_iterator p_iterator)
	{
		if (IsNotNull(p_family, p_iterator))
			{
			p_solution.AddToSolution(p_family, 
				p_iterator,m_LastGraphId);
			}
	}

	void FMTSpatialGraphs::RemoveToSolution(FMTSolutionTracker& p_solution,
		size_t p_family,
		FMTSpatialGraphs::const_iterator p_iterator)
	{
		if (IsNotNull(p_family, p_iterator))
		{
			p_solution.RemoveFromSolution(p_family, p_iterator);
		}
	}

	FMTSpatialGraphs::const_iterator FMTSpatialGraphs::GetIterator(
		const Graph::FMTlinegraph& p_Graph,
		size_t p_family) const
	{
		FMTSpatialGraphs::const_iterator It = m_AllGraphs.at(p_family).find(p_Graph);
		if (It == m_AllGraphs.at(p_family).end())
		{
			_exhandler->raise(Exception::FMTexc::FMTrangeerror,
				"Empty Iterator for Graph Family " + std::to_string(p_family) +
				" Family size of " + std::to_string(m_AllGraphs.at(p_family).size())
				, "FMTSpatialGraphs::GetIterator", __LINE__, __FILE__);
		}
		return It;
	}

	FMTSpatialGraphs::const_iterator FMTSpatialGraphs::SetIterator(
		const Graph::FMTlinegraph& p_Graph,
		size_t p_family)
	{
		const_iterator Iterator = m_AllGraphs.at(p_family).end();
		try {
			std::pair<FMTSpatialGraphs::iterator, bool> inserted = m_AllGraphs.at(p_family).insert(
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


	FMTSpatialGraphs::const_iterator  FMTSpatialGraphs::SetNaturalGrowthIterator(size_t p_family)
	{
		const_iterator Iterator = m_AllGraphs.at(p_family).end();
		try {
			Iterator = _GetNaturalGrowthIterator(p_family);
			if (Iterator==m_AllGraphs.at(p_family).end())
				{
				const int LENGTH = m_Model->getparameter(Models::FMTintmodelparameters::LENGTH);
				const Core::FMTactualdevelopment* BASE = dynamic_cast<const Core::FMTactualdevelopment*>(
					&m_AllGraphs.at(p_family).begin()->first.getbasedevelopment());
				Graph::FMTlinegraph NewGraph(static_cast<size_t>(LENGTH), *BASE);
				NewGraph.grow(LENGTH);
				Iterator = SetIterator(NewGraph, p_family);
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "SetNaturalGrowthIterator",
				__LINE__, __FILE__);
			}
		return Iterator;
	}




	void FMTSpatialGraphs::SetModel(const Models::FMTmodel& p_model)
	{
		m_Model = &p_model;
	}


	std::map<std::string, double> FMTSpatialGraphs::GetOutput(
		const FMTSolutionTracker& p_Solution,
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
		const std::vector<size_t> SOLUTION = _BuildGraphs(p_model, p_CellSize);
		_BuildConstraintsLocator(p_model);
		_BuildConstraintsValues(m_AllGraphs.begin(), m_AllGraphs.end());
		_BuildTacker(SOLUTION);
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

	std::vector<size_t> FMTSpatialGraphs::_BuildGraphs(const Models::FMTmodel& p_model, double p_CellSize)
	{
		std::vector<size_t>BaseSolution;
		try {
			const size_t LENGTH = static_cast<size_t>(p_model.getparameter(Models::FMTintmodelparameters::LENGTH));
			const Core::FMTmask USEFULL_BITS = _GetUseFullBits(p_model);
			const std::vector<Core::FMTactualdevelopment>AREAS = p_model.Models::FMTmodel::getarea();
			BaseSolution = std::vector<size_t>(AREAS.size());
			for (const auto& DEV : AREAS)
			{
				const Core::FMTmask& DEV_MASK = DEV.getmask();
				Core::FMTmask SORT_MASK = DEV_MASK.getintersect(USEFULL_BITS);
				std::pair<boost::unordered_map<Core::FMTmask, size_t>::iterator, bool> InSertedMask =
					m_GraphsMasks.insert(std::pair<Core::FMTmask, size_t>(SORT_MASK, m_AllGraphs.size()));
				const size_t GRAPHS_LOCATION = InSertedMask.first->second;
				if (GRAPHS_LOCATION == m_AllGraphs.size())
				{
					m_AllGraphs.push_back(std::map<Graph::FMTlinegraph,
											FMTGraphInfo>());
				}
				Graph::FMTlinegraph local_graph(LENGTH);
				std::vector<Core::FMTactualdevelopment> NewDevs(1, DEV);

				const size_t NUMBER_OF_CELLS = static_cast<size_t> (std::round(NewDevs.begin()->getarea() / p_CellSize));
				NewDevs.begin()->setarea(p_CellSize);
				BaseSolution[m_LastGraphId] = NUMBER_OF_CELLS;
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
		return BaseSolution;
	}

	std::vector<std::vector<double>> FMTSpatialGraphs::_GetConstraintsValues(const FMTSolutionTracker& p_solution) const
	{
		std::vector<std::vector<double>>ConstraintsValues;
		try {
			for (size_t ConstraintId = 0; ConstraintId < m_Constraints.size(); ++ConstraintId)
			{
				std::vector<double>values;
				for (size_t GraphFamily : m_Constraints.at(ConstraintId))
				{
					for (const auto& GRAPH : m_AllGraphs.at(GraphFamily))
					{
						_FillConstraintResults(ConstraintId,
							GRAPH.second,
							values,
							p_solution);
					}
				}
				ConstraintsValues.push_back(values);
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTSpatialGraphs::_GetConstraintsValues",
				__LINE__, __FILE__);
			}
		return ConstraintsValues;
	}

	void FMTSpatialGraphs::_BuildTacker(const std::vector<size_t>& p_solution)
	{
		try {
			m_BaseSolution = FMTSolutionTracker(p_solution,m_Constraints, m_AllGraphs.size());
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTSpatialGraphs::_BuildTacker",
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
		FMTSpatialGraphs::iterator p_Graph,
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
		FMTSpatialGraphs::family_iterator FirstGraphFamily,
		FMTSpatialGraphs::family_iterator GraphFamilyEnd)
	{
		try {
			size_t Family = 0;
			for (family_iterator It = FirstGraphFamily; It != GraphFamilyEnd; ++It)
			{
				for (iterator GraphIt = It->begin();
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
		const FMTSolutionTracker& p_solution) const
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
		}catch (...)
			{
			_exhandler->raisefromcatch("For constraint Id " + std::to_string(p_Constraint),
				"FMTSpatialGraphs::_FillConstraintResults",
				__LINE__, __FILE__);
			}
	}



	size_t FMTSpatialGraphs::_GetMaxGraphLength() const
	{
		size_t maxLength = 0;
		for (const auto& FAMILY : m_AllGraphs)
		{
			for (const auto& GRAPH : FAMILY)
			{
				maxLength = std::max(maxLength, 
						static_cast<size_t>(GRAPH.first.getperiod()));
			}
		}
		return maxLength;
	}
	size_t FMTSpatialGraphs::_GetMinGraphLength() const
	{
		size_t minLength = std::numeric_limits<size_t>::max();
		for (const auto& FAMILY : m_AllGraphs)
		{
			for (const auto& GRAPH : FAMILY)
			{
				minLength = std::min(minLength,
					static_cast<size_t>(GRAPH.first.getperiod()));
			}
		}
		return minLength;
	}



	double FMTSpatialGraphs::_GetGraphCells(const FMTSolutionTracker& p_solution,
													const FMTGraphInfo& p_GraphInfo) const
	{
		double area = 0.0;
		try {
			area = static_cast<double>(p_solution.GetNumberOfCells(p_GraphInfo.GetGraphId()));
		}catch (...)
			{
			_exhandler->raisefromcatch("For Graph id "+std::to_string(p_GraphInfo.GetGraphId())+
													" with solution size of "+ std::to_string(p_solution.size()) +
													" last graph id of "+ std::to_string(m_LastGraphId),
																	"FMTSpatialGraphs::_GetGraphCells",
				__LINE__, __FILE__);
			}
		return area;
	}

	void FMTSpatialGraphs::_RemoveGraphsShorterThan(size_t p_GraphSize)
	{
		for (auto& FAMILY : m_AllGraphs)
		{
			for (auto GraphIt = FAMILY.begin(); GraphIt != FAMILY.end();)
				{
				const size_t GRAPH_SIZE = static_cast<size_t>(GraphIt->first.getperiod());
				if (GRAPH_SIZE < p_GraphSize)
				{
					GraphIt = FAMILY.erase(GraphIt);
				}else {
					++GraphIt;
					}
				}
		}
	}

	FMTSpatialGraphs::const_iterator FMTSpatialGraphs::_GetNaturalGrowthIterator(size_t p_family) const
	{
		const int LENGTH = m_Model->getparameter(Models::FMTintmodelparameters::LENGTH);
		for (const_iterator It = m_AllGraphs.at(p_family).begin();
				It != m_AllGraphs.at(p_family).end(); ++It)
			{
				if (It->first.getperiod() - 1 == LENGTH &&
					It->first.isonlygrow())
				{
					return It;
				}

			}
		return m_AllGraphs.at(p_family).end();
	}


	

}


