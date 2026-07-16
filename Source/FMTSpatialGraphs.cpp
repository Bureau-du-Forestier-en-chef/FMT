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
#include "FMTGraphValues.h"
#include <future>
#include <list>
#include <memory>
#include <boost/functional/hash.hpp>


namespace Spatial
{

	
	std::vector<double> FMTSpatialGraphs::_getConstraintsValueFromSolution(size_t p_ConstraintId,
		const FMTSolutionTracker& p_solution) const
	{
		std::vector<double>results;
		try {
			for (size_t GraphFamily : m_Constraints.at(p_ConstraintId))
			{
				for (const auto& GRAPH : m_AllGraphs.at(GraphFamily))
				{
					_fillConstraintResults(p_ConstraintId, GRAPH.second,
						results, p_solution);
				}
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTSpatialGraphs::getConstraintsValues",
				__LINE__, __FILE__);
			}
		return results;
	}
	std::vector<double> FMTSpatialGraphs::_getConstraintsValueFromTracker(size_t p_ConstraintId,
		const FMTSolutionTracker& p_solution) const
	{
		std::vector<double> results;
		try {
			results = p_solution.getConstraintsValues(p_ConstraintId);
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTSpatialGraphs::_getConstraintsValueFromTracker",
				__LINE__, __FILE__);
			}
		return results;
	}

	void FMTSpatialGraphs::enableSolutionTracker(FMTSolutionTracker& p_tracker) const
		{
		try {
			if (!p_tracker.isTrackingEnable())
				{
				p_tracker.setConstraintValues(_getConstraintsValues(p_tracker),
									m_Constraints,m_AllGraphs.size());
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTSpatialGraphs::enableSolutionTracker",
				__LINE__, __FILE__);
			}
		}


	double FMTSpatialGraphs::getConstraintsValue(
		size_t p_ConstraintId,
		const FMTSolutionTracker& p_solution) const
	{
		double value = 0.0;
		try {
			const size_t NON_SPATIAL_ID  = _getNonSpatialId(p_ConstraintId);
			std::vector<double>results;
			if (p_solution.isTrackingEnable())
			{
			results = _getConstraintsValueFromTracker(NON_SPATIAL_ID, p_solution);
			}else {
				results = _getConstraintsValueFromSolution(NON_SPATIAL_ID, p_solution);
				}
			if (!results.empty())
			{
				value = m_Model->constraints.at(p_ConstraintId).evaluate(results);
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialGraphs::getConstraintsValues",
				__LINE__, __FILE__);
		}
		return value;
	}

	FMTVirtualLineGraph FMTSpatialGraphs::getVirtualGraph(const Graph::FMTlinegraph& p_LineGraph)
	{
		size_t Family = 0;
		const_iterator Iterator = m_AllGraphs.begin()->end();
		try {
			Family = _getFamily(p_LineGraph);
			Iterator = getIterator(p_LineGraph, Family);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialGraphs::getVirtualGraph",
				__LINE__, __FILE__);
		}
		return FMTVirtualLineGraph(*this, Iterator, Family);
	}

	FMTVirtualLineGraph FMTSpatialGraphs::setVirtualGraph(const Graph::FMTlinegraph& p_LineGraph)
	{
		size_t Family = 0;
		const_iterator Iterator = m_AllGraphs.begin()->end();
		try {
			Family = _getFamily(p_LineGraph);
			Iterator = setIterator(p_LineGraph, Family);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialGraphs::setVirtualGraph",
				__LINE__, __FILE__);
		}
		return FMTVirtualLineGraph(*this, Iterator, Family);
	}

	FMTSpatialGraphs::const_iterator FMTSpatialGraphs::getBaseIterator(size_t p_family) const
	{
		const_iterator Iterator = m_AllGraphs.at(p_family).end();
		try {
			size_t MinId = m_LastGraphId;
			for (const_iterator It = m_AllGraphs.at(p_family).begin();
				It != m_AllGraphs.at(p_family).end();++It)
				{
				if (It->second.getGraphId()<MinId)
					{
					MinId = It->second.getGraphId();
					Iterator = It;
					}
				}
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialGraphs::getBaseIterator",
				__LINE__, __FILE__);
		}
		return Iterator;
	}

	FMTSpatialGraphs::const_iterator FMTSpatialGraphs::getLastPeriodIterator(
		size_t p_family,
		FMTSpatialGraphs::const_iterator p_iterator) const
	{
		FMTSpatialGraphs::const_iterator Iterator = m_AllGraphs.at(p_family).end();
		try {
			for (FMTSpatialGraphs::const_iterator It = m_AllGraphs.at(p_family).begin();
				It != m_AllGraphs.at(p_family).end(); ++It)
			{
				if (p_iterator->first.isLessPeriod(It->first))
				{
					return It;
				}
			}
			/*_exhandler->raise(Exception::FMTexc::FMTrangeerror,
				"Cant Get Less period for graph id " + std::to_string(p_iterator->second.getGraphId())
				, "FMTSpatialGraphs::getLastPeriodIterator", __LINE__, __FILE__);*/
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialGraphs::getLastPeriodIterator",
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

	size_t FMTSpatialGraphs::getNumberOfCategories(int p_themeId) const
	{
		return m_Model->themes.at(p_themeId).size(); 
	}

	size_t FMTSpatialGraphs::getCategoryOf(int p_themeId, size_t p_family) const
	{
		const Core::FMTMask& MASK = m_AllGraphs.at(p_family).begin()->first.getBaseDevelopment().getMask();
		return std::distance(&*m_Model->themes.at(p_themeId).getBaseAttributes().begin(),
			&MASK.getAttribute(m_Model->themes.at(p_themeId)));
	}

	void FMTSpatialGraphs::deleteNonCompleteGraphs()
	{
		const size_t MAX_GRAPH_LENGTH = _getMaxGraphLength();
		_removeGraphsShorterThan(MAX_GRAPH_LENGTH);
	}





	FMTSolutionTracker FMTSpatialGraphs::getBaseSolution() const
	{
		return m_BaseSolution;
	}

	bool FMTSpatialGraphs::isNotNull(size_t p_family,
		FMTSpatialGraphs::const_iterator p_iterator) const
	{
		return (p_iterator != m_AllGraphs.at(p_family).end());
	}

	std::vector<Core::FMTSchedule> FMTSpatialGraphs::getSchedules(const FMTSolutionTracker& p_Solution,
		bool WithLock) const
	{
		std::vector<Core::FMTSchedule> Schedules(m_Model->getParameter(Models::FMTintmodelparameters::LENGTH));
		try {
			int SchedulePeriod = 1;
			for (Core::FMTSchedule& Schedule : Schedules)
			{
				Schedule.setPeriod(SchedulePeriod);
				++SchedulePeriod;
			}
			for (const auto& FAMILY : m_AllGraphs)
			{
				for (const auto& GRAPH : FAMILY)
				{
					const double CELLS = _getGraphCells(p_Solution, GRAPH.second);
					if (CELLS > FMT_DBL_TOLERANCE)
					{
						const double GRAPH_AREA = CELLS * GRAPH.first.getBaseDevelopment().getArea();
						for (int period = 1; period < GRAPH.first.getPeriod(); ++period)
						{
							const Core::FMTSchedule SCHEDULE = GRAPH.first.getSchedule(getModel().actions,
								&GRAPH_AREA, period, WithLock);
							Schedules[period - 1] += SCHEDULE;
							
						}
					}
				}
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialGraphs::getSchedule",
				__LINE__, __FILE__);
		}
		return  Schedules;
	}

	void FMTSpatialGraphs::addToSolution(FMTSolutionTracker& p_solution,
		size_t p_family,
		FMTSpatialGraphs::const_iterator p_iterator)
	{
		if (isNotNull(p_family, p_iterator))
			{
			p_solution.addToSolution(p_family, 
				p_iterator,m_LastGraphId);
			}
	}

	void FMTSpatialGraphs::removeToSolution(FMTSolutionTracker& p_solution,
		size_t p_family,
		FMTSpatialGraphs::const_iterator p_iterator)
	{
		if (isNotNull(p_family, p_iterator))
		{
			p_solution.removeFromSolution(p_family, p_iterator);
		}
	}

	FMTSpatialGraphs::const_iterator FMTSpatialGraphs::getIterator(
		const Graph::FMTlinegraph& p_Graph,
		size_t p_family) const
	{
		FMTSpatialGraphs::const_iterator It = m_AllGraphs.at(p_family).find(p_Graph);
		if (It == m_AllGraphs.at(p_family).end())
		{
			_exhandler->raise(Exception::FMTexc::FMTrangeerror,
				"Empty Iterator for Graph Family " + std::to_string(p_family) +
				" Family size of " + std::to_string(m_AllGraphs.at(p_family).size())
				, "FMTSpatialGraphs::getIterator", __LINE__, __FILE__);
		}
		return It;
	}

	FMTSpatialGraphs::const_iterator FMTSpatialGraphs::setIterator(
		const Graph::FMTlinegraph& p_Graph,
		size_t p_family)
	{
		const_iterator Iterator = m_AllGraphs.at(p_family).end();
		try {
			std::pair<FMTSpatialGraphs::iterator, bool> inserted = m_AllGraphs.at(p_family).insert(
				std::pair<Graph::FMTlinegraph, FMTGraphInfo>(p_Graph, FMTGraphInfo(m_LastGraphId)));
			if (inserted.second)//Calculate constraints
			{
				_buildConstraintsValues(inserted.first, p_family);
				++m_LastGraphId;
			}
			Iterator = inserted.first;
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialGraphs::setIterator",
				__LINE__, __FILE__);
		}
		return Iterator;

	}


	FMTSpatialGraphs::const_iterator  FMTSpatialGraphs::setNaturalGrowthIterator(size_t p_family)
	{
		const_iterator Iterator = m_AllGraphs.at(p_family).end();
		try {
			Iterator = _getNaturalGrowthIterator(p_family);
			if (Iterator==m_AllGraphs.at(p_family).end())
				{
				const int LENGTH = m_Model->getParameter(Models::FMTintmodelparameters::LENGTH);
				const Core::FMTActualDevelopment* BASE = dynamic_cast<const Core::FMTActualDevelopment*>(
					&m_AllGraphs.at(p_family).begin()->first.getBaseDevelopment());
				Graph::FMTlinegraph NewGraph(static_cast<size_t>(LENGTH), *BASE);
				NewGraph.grow(LENGTH);
				Iterator = setIterator(NewGraph, p_family);
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "SetNaturalGrowthIterator",
				__LINE__, __FILE__);
			}
		return Iterator;
	}




	void FMTSpatialGraphs::setModel(const Models::FMTmodel& p_model)
	{
		m_Model = &p_model;
	}


	std::map<std::string, double> FMTSpatialGraphs::getOutput(
		const FMTSolutionTracker& p_Solution,
		const Core::FMTOutput& p_output,
		int p_period, Core::FMToutputlevel level) const
	{
		std::map<std::string, double> results;
		try {
			for (const auto& FAMILY : m_AllGraphs)
			{
				for (const auto& GRAPH : FAMILY)
				{
					const double CELLS = _getGraphCells(p_Solution, GRAPH.second);
					if (CELLS > FMT_DBL_TOLERANCE)
					{
						const double GRAPH_AREA = GRAPH.first.getBaseDevelopment().getArea();
						const double VALUE = CELLS * GRAPH_AREA;
						const double* SOLUTION = &VALUE;
						for (const auto& DATA : GRAPH.first.getOutput(getModel(), p_output, p_period, SOLUTION, level))
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
			_exhandler->raiseFromCatch("", "FMTSpatialGraphs::getOutput",
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
		const std::vector<size_t> SOLUTION = _buildGraphs(p_model, p_CellSize);
		_buildConstraintsLocator(p_model);
		_buildConstraintsValues(m_AllGraphs.begin(), m_AllGraphs.end());
		_buildTacker(SOLUTION);
	}

	Core::FMTMask FMTSpatialGraphs::_getUseFullBits(const Models::FMTmodel& p_model)
	{
		Core::FMTMask usefullBits(p_model.themes);
		try {
			const std::vector<const Core::FMTTheme*> STATIC_THEMES = p_model.locateStaticTransitionsThemes();
			for (const Core::FMTTheme& THEME : p_model.themes)
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
			_exhandler->raiseFromCatch("", "FMTSpatialGraphs::_getUseFullBits",
				__LINE__, __FILE__);
		}
		return usefullBits;
	}

	std::vector<size_t> FMTSpatialGraphs::_buildGraphs(const Models::FMTmodel& p_model, double p_CellSize)
	{
		std::vector<size_t>BaseSolution;
		try {
			const size_t LENGTH = static_cast<size_t>(p_model.getParameter(Models::FMTintmodelparameters::LENGTH));
			const Core::FMTMask USEFULL_BITS = _getUseFullBits(p_model);
			const std::vector<Core::FMTActualDevelopment>AREAS = p_model.Models::FMTmodel::getArea();
			BaseSolution = std::vector<size_t>(AREAS.size());
			for (const auto& DEV : AREAS)
			{
				const Core::FMTMask& DEV_MASK = DEV.getMask();
				Core::FMTMask SORT_MASK = DEV_MASK.getIntersect(USEFULL_BITS);
				std::pair<boost::unordered_map<Core::FMTMask, size_t>::iterator, bool> InSertedMask =
					m_GraphsMasks.insert(std::pair<Core::FMTMask, size_t>(SORT_MASK, m_AllGraphs.size()));
				const size_t GRAPHS_LOCATION = InSertedMask.first->second;
				if (GRAPHS_LOCATION == m_AllGraphs.size())
				{
					m_AllGraphs.push_back(std::map<Graph::FMTlinegraph,
											FMTGraphInfo>());
				}
				Graph::FMTlinegraph local_graph(LENGTH);
				std::vector<Core::FMTActualDevelopment> NewDevs(1, DEV);

				const size_t NUMBER_OF_CELLS = static_cast<size_t> (std::round(NewDevs.begin()->getArea() / p_CellSize));
				NewDevs.begin()->setArea(p_CellSize);
				BaseSolution[m_LastGraphId] = NUMBER_OF_CELLS;
				local_graph.initialize(NewDevs);
				//local_graph.grow(LENGTH);
				m_AllGraphs[GRAPHS_LOCATION][local_graph] = FMTGraphInfo(m_LastGraphId);
				++m_LastGraphId;
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialGraphs::_buildGraphs",
				__LINE__, __FILE__);
		}
		return BaseSolution;
	}

	std::vector<std::vector<double>> FMTSpatialGraphs::_getConstraintsValues(const FMTSolutionTracker& p_solution) const
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
						_fillConstraintResults(ConstraintId,
							GRAPH.second,
							values,
							p_solution);
					}
				}
				ConstraintsValues.push_back(values);
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTSpatialGraphs::_getConstraintsValues",
				__LINE__, __FILE__);
			}
		return ConstraintsValues;
	}

	void FMTSpatialGraphs::_buildTacker(const std::vector<size_t>& p_solution)
	{
		try {
			m_BaseSolution = FMTSolutionTracker(p_solution,m_Constraints, m_AllGraphs.size());
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTSpatialGraphs::_buildTacker",
				__LINE__, __FILE__);
			}
	}


	void FMTSpatialGraphs::_buildConstraintsLocator(const Models::FMTmodel& p_model)
	{
		try {
			const Core::FMTMask USEFULL_BITS = _getUseFullBits(p_model);
			m_ConstraintsId.clear();
			size_t Id = 0;
			for (const Core::FMTConstraint& CONSTRAINT : p_model.constraints)
			{
				m_ConstraintsId.push_back(Id);
				if (!CONSTRAINT.isSpatial())
				{
					std::vector<size_t>selectedGraphs;
					const Core::FMTMask UNION_MASK = CONSTRAINT.getMasksUnion();
					Core::FMTMask Intersect = UNION_MASK.getIntersect(UNION_MASK);
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
			_exhandler->raiseFromCatch("", "FMTSpatialGraphs::_buildConstraintsLocator",
				__LINE__, __FILE__);
		}
	}

	void FMTSpatialGraphs::_buildConstraintsValues(
		FMTSpatialGraphs::iterator p_Graph,
		size_t p_family)
	{
		try {
			size_t ConstraintId = 0;
			for (const Core::FMTConstraint& CONSTRAINT : getModel().constraints)
			{
				if (!CONSTRAINT.isSpatial())
				{
					if (std::find(m_Constraints.at(ConstraintId).begin(),
						m_Constraints.at(ConstraintId).end(),
						p_family) != m_Constraints.at(ConstraintId).end())
					{
						int LowestPeriod = CONSTRAINT.getPeriodLowerBound();
						int UpperPeriod = CONSTRAINT.getPeriodUpperBound();
						if (p_Graph->second.isEmpty())
						{
							p_Graph->second.setValuesSize(getModel().constraints.size());
						}
						if (p_Graph->first.constraintLenght(CONSTRAINT, LowestPeriod, UpperPeriod))
						{
							if (CONSTRAINT.acrossPeriod())
							{
								++UpperPeriod;
							}
							const double GRAPH_AREA = p_Graph->first.getBaseDevelopment().getArea();
							const double* SOLUTION = &GRAPH_AREA;
							std::vector<double>Values((UpperPeriod - LowestPeriod) + 1, 0.0);
							size_t i  = _fillValuesFromLastPeriod(p_family, p_Graph, ConstraintId,
								Values);
							LowestPeriod += static_cast<int>(i);
							for (int period = LowestPeriod; period <= UpperPeriod; ++period)
							{
								Values.at(i) = p_Graph->first.getOutput(getModel(),
									CONSTRAINT, period, SOLUTION, Core::FMToutputlevel::totalonly).at("Total");
								++i;
							}
							//Values are interned through a run-scoped pool (see _InternValues) so that
							//identical value vectors are shared without any process-wide static factory.
							p_Graph->second.setValues(ConstraintId, _internValues(Values));
						}
					}

					++ConstraintId;
				}

			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialGraphs::_buildConstraintsValues",
				__LINE__, __FILE__);
		}
	}

	std::shared_ptr<const FMTGraphValues> FMTSpatialGraphs::_internValues(
		const std::vector<double>& p_Values)
	{
		//Run-scoped deduplication pool. Replaces the former process-wide boost::flyweight
		//static factory: identical value vectors are shared through weak_ptr, so a single
		//copy of each distinct vector is kept alive by the graphs referencing it, and the
		//whole pool dies with this FMTSpatialGraphs instance at the end of the run. This
		//removes the shared static state that could be corrupted between successive runs.
		std::shared_ptr<const FMTGraphValues> interned;
		try {
			std::size_t hashvalue = 0;
			boost::hash_range(hashvalue, p_Values.begin(), p_Values.end());
			std::vector<std::weak_ptr<const FMTGraphValues>>& bucket = m_ValuesPool[hashvalue];
			for (std::vector<std::weak_ptr<const FMTGraphValues>>::iterator it = bucket.begin();
				it != bucket.end(); )
			{
				if (std::shared_ptr<const FMTGraphValues> existing = it->lock())
				{
					if (existing->getValues() == p_Values)
					{
						return existing;
					}
					++it;
				}
				else
				{
					it = bucket.erase(it);//prune expired entries
				}
			}
			interned = std::make_shared<const FMTGraphValues>(p_Values);
			bucket.push_back(interned);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialGraphs::_internValues",
				__LINE__, __FILE__);
		}
		return interned;
	}

	size_t FMTSpatialGraphs::_fillValuesFromLastPeriod(
		size_t p_family,
		FMTSpatialGraphs::iterator p_Graph,
		size_t p_ConstraintId,
		std::vector<double>& p_constraintValues) const
	{
		size_t i = 0;
		try {
			if(p_constraintValues.size() > 1)
			{
				FMTSpatialGraphs::const_iterator LAST_GRAPH = getLastPeriodIterator(p_family, p_Graph);
				if (isNotNull(p_family, LAST_GRAPH))
				{

					for (const double& VALUE : LAST_GRAPH->second.getValues(p_ConstraintId))
					{
						p_constraintValues.at(i) = VALUE;
						++i;
					}
					
				}
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTSpatialGraphs::_fillValuesFromLastPeriod",
				__LINE__, __FILE__);
			}
		return i;
	}



	void FMTSpatialGraphs::_buildConstraintsValues(
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
					_buildConstraintsValues(GraphIt, Family);
				}
				++Family;
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialGraphs::_buildConstraintsValues",
				__LINE__, __FILE__);
		}
	}

	size_t FMTSpatialGraphs::_getFamily(const Graph::FMTlinegraph& p_Graph) const
	{
		try {
			const Core::FMTMask USEFULL_BITS = _getUseFullBits(getModel());
			const Core::FMTDevelopment& DEVELOPPEMENT = p_Graph.getBaseDevelopment();
			Core::FMTMask DEV_MASK = DEVELOPPEMENT.getMask();
			DEV_MASK = DEV_MASK.getIntersect(USEFULL_BITS);
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
			_exhandler->raiseFromCatch("", "FMTSpatialGraphs::_buildConstraintsLocator",
				__LINE__, __FILE__);
		}
		return 0;
	}

	const Models::FMTmodel& FMTSpatialGraphs::getModel() const
	{
		return *m_Model;
	}

	size_t FMTSpatialGraphs::_getNonSpatialId(size_t p_Constraint) const
	{
		try {
			p_Constraint = m_ConstraintsId.at(p_Constraint);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialGraphs::_getNonSpatialId",
				__LINE__, __FILE__);
		}
		return p_Constraint;
	}

	void FMTSpatialGraphs::_fillConstraintResults(size_t p_Constraint,
		const FMTGraphInfo& p_GraphInfo,
		std::vector<double>& p_results,
		const FMTSolutionTracker& p_solution) const
	{
		try {
			const double CELLS = _getGraphCells(p_solution, p_GraphInfo);
			if (CELLS > FMT_DBL_TOLERANCE)
			{
				if (p_results.empty())
				{
					p_results = std::vector<double>(p_GraphInfo.getValues(p_Constraint).size(), 0.0);
				}
				size_t i = 0;
				for (double VALUE : p_GraphInfo.getValues(p_Constraint))
				{
					p_results.at(i) += VALUE * CELLS;
					++i;
				}
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("For constraint Id " + std::to_string(p_Constraint),
				"FMTSpatialGraphs::_fillConstraintResults",
				__LINE__, __FILE__);
			}
	}



	size_t FMTSpatialGraphs::_getMaxGraphLength() const
	{
		size_t maxLength = 0;
		for (const auto& FAMILY : m_AllGraphs)
		{
			for (const auto& GRAPH : FAMILY)
			{
				maxLength = std::max(maxLength, 
						static_cast<size_t>(GRAPH.first.getPeriod()));
			}
		}
		return maxLength;
	}
	size_t FMTSpatialGraphs::_getMinGraphLength() const
	{
		size_t minLength = std::numeric_limits<size_t>::max();
		for (const auto& FAMILY : m_AllGraphs)
		{
			for (const auto& GRAPH : FAMILY)
			{
				minLength = std::min(minLength,
					static_cast<size_t>(GRAPH.first.getPeriod()));
			}
		}
		return minLength;
	}



	double FMTSpatialGraphs::_getGraphCells(const FMTSolutionTracker& p_solution,
													const FMTGraphInfo& p_GraphInfo) const
	{
		double area = 0.0;
		try {
			area = static_cast<double>(p_solution.getNumberOfCells(p_GraphInfo.getGraphId()));
		}catch (...)
			{
			_exhandler->raiseFromCatch("For Graph id "+std::to_string(p_GraphInfo.getGraphId())+
													" with solution size of "+ std::to_string(p_solution.size()) +
													" last graph id of "+ std::to_string(m_LastGraphId),
																	"FMTSpatialGraphs::_getGraphCells",
				__LINE__, __FILE__);
			}
		return area;
	}

	void FMTSpatialGraphs::_removeGraphsShorterThan(size_t p_GraphSize)
	{
		for (auto& FAMILY : m_AllGraphs)
		{
			for (auto GraphIt = FAMILY.begin(); GraphIt != FAMILY.end();)
				{
				const size_t GRAPH_SIZE = static_cast<size_t>(GraphIt->first.getPeriod());
				if (GRAPH_SIZE < p_GraphSize)
				{
					GraphIt = FAMILY.erase(GraphIt);
				}else {
					++GraphIt;
					}
				}
		}
	}

	FMTSpatialGraphs::const_iterator FMTSpatialGraphs::_getNaturalGrowthIterator(size_t p_family) const
	{
		const int LENGTH = m_Model->getParameter(Models::FMTintmodelparameters::LENGTH);
		for (const_iterator It = m_AllGraphs.at(p_family).begin();
				It != m_AllGraphs.at(p_family).end(); ++It)
			{
				if (It->first.getPeriod() - 1 == LENGTH &&
					It->first.isOnlyGrow())
				{
					return It;
				}

			}
		return m_AllGraphs.at(p_family).end();
	}


	

}


