/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTEVENTSGRAPH_H
#define FMTEVENTSGRAPH_H

#include "FMTevent.h"
#include <boost/graph/adjacency_list.hpp>
#include <unordered_set>
#include <boost/unordered_set.hpp>
#include "FMTobject.h"

namespace Core
{
	class FMTconstraint;
}


namespace Spatial
{

class FMTbindingspatialaction;

// DocString: FMTEventsGraph
/**
This class is a container for FMTevent. It's build to handle event
comparison or distance calculation in an efficient way by allowing the
user to get events by period, action id and/or coordinates.
One container can hold all events for a model. They are sorted by
period, centroid, action_id and finally by size.
You can  remove FMTcoordinate and add FMTcoordinate by period and action.
*/
class FMTEventsGraph : public Core::FMTobject
{
public:
	typedef std::vector<std::vector<Spatial::FMTbindingspatialaction>> p_ActionsBindings;
	void AddEvent(const FMTevent& p_event);
	double Evaluate(const std::vector<Core::FMTconstraint>& p_SpatialConstraints) const;
	int GetPeriodWithMaximalEvents(const std::vector<bool>& actions) const;
	std::vector<std::vector<Spatial::FMTcoordinate>>GetAreaConflictCoordinates(
		const p_ActionsBindings& p_bindingActions, int p_period, bool p_conflictOnly) const;
	std::vector<std::vector<Spatial::FMTcoordinate>>getAdjacencyConflictCoordinates(
		const p_ActionsBindings& p_bindingActions, int p_period, bool p_conflictOnly) const;



private:
	class EventNode
	{
	public:
		EventNode(const FMTevent& p_event);
		const FMTevent& GetEvent() const;
	private:
		FMTevent m_event;

	};
	class EventEdge
	{
	public:
		EventEdge(double p_distance, 
					bool p_TooClose);
	private:
		double m_distance;
		bool m_TooClose;
	};

	typedef boost::adjacency_list<boost::listS,
		boost::listS,
		boost::bidirectionalS,
		EventNode,
		EventEdge,
		boost::no_property,
		boost::listS>EventsGraph;

	EventsGraph m_Graph;

    
};


}
#endif // FMTEVENTSGRAPH
