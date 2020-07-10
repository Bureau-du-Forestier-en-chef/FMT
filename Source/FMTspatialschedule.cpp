#include "FMTspatialschedule.h"

namespace Spatial
{
    FMTspatialschedule::FMTspatialschedule():FMTlayer<Graph::FMTlinegraph>(),outputscache(),events()
    {
        //ctor
    }

    FMTspatialschedule::FMTspatialschedule(const FMTforest& initialmap):outputscache(),events()
    {
        FMTlayer<Graph::FMTlinegraph>::operator = (initialmap.copyextent<Graph::FMTlinegraph>());//Setting layer information
        for(std::map<FMTcoordinate,Core::FMTdevelopment>::const_iterator devit = initialmap.mapping.begin(); devit != initialmap.mapping.end(); ++devit)
        {
			std::vector<Core::FMTactualdevelopment> actdevelopment;
            actdevelopment.push_back(Core::FMTactualdevelopment (devit->second,initialmap.getcellsize()));
            Graph::FMTlinegraph local_graph(Graph::FMTgraphbuild::schedulebuild);
            std::queue<Graph::FMTvertex_descriptor> actives = local_graph.initialize(actdevelopment);
            mapping[devit->first] = local_graph;
        }
    }

    FMTspatialschedule::FMTspatialschedule(const FMTspatialschedule& other):
            FMTlayer<Graph::FMTlinegraph>(other),
            outputscache(other.outputscache),
            events(other.events)
    {
        //copy ctor
    }

    FMTspatialschedule& FMTspatialschedule::operator=(const FMTspatialschedule& rhs)
    {
        if (this == &rhs) return *this; // handle self assignment
        //assignment operator
        FMTlayer<Graph::FMTlinegraph>::operator = (rhs);
        events = rhs.events;
        if (outputscache.size() < rhs.outputscache.size())
			{
			outputscache = rhs.outputscache;
			}
        return *this;
    }

    bool FMTspatialschedule::operator == (const FMTspatialschedule& rhs)const
    {
        if (!(events==rhs.events)) return false;
        for (std::map<FMTcoordinate,Graph::FMTlinegraph>::const_iterator coordit = this->mapping.begin();
                                                                         coordit!= this->mapping.end(); ++coordit)
        {
            if (coordit->second != rhs.mapping.at(coordit->first))
            {
                return false;
            }
        }
    return true;
    }

    bool FMTspatialschedule::operator != (const FMTspatialschedule& rhs)const
    {
        return (!(*this==rhs));
    }

    bool FMTspatialschedule::copyfromselected(const FMTspatialschedule& rhs, const std::vector<size_t>& selected)
    {
		std::map<FMTcoordinate, Graph::FMTlinegraph>::iterator baseit;
		std::map<FMTcoordinate, Graph::FMTlinegraph>::const_iterator newvalueit;
		//No location check sooo make sure you copy the same kind of solution...
		events = rhs.events;
		if (outputscache.size()<rhs.outputscache.size())
			{
			outputscache = rhs.outputscache;
			}
		if (this->size() == rhs.size())
			{
			for (const size_t& selection : selected)
				{
				baseit = std::next(this->mapping.begin(), selection);
				newvalueit = std::next(rhs.mapping.begin(), selection);
				baseit->second = newvalueit->second;
				}
			return true;
			}
		return false;
    }

	bool FMTspatialschedule::swapfromselected(FMTspatialschedule& rhs, const std::vector<size_t>& selected)
    {
		std::map<FMTcoordinate, Graph::FMTlinegraph>::iterator baseit;
		std::map<FMTcoordinate, Graph::FMTlinegraph>::iterator newvalueit;
		//No location check sooo make sure you copy the same kind of solution...
		events.swap(rhs.events);
		if (outputscache.size() < rhs.outputscache.size())
		{
			outputscache.swap(rhs.outputscache);
		}
		if (this->size() == rhs.size())
		{
			for (const size_t& selection : selected)
			{
				baseit = std::next(this->mapping.begin(), selection);
				newvalueit = std::next(rhs.mapping.begin(), selection);
				baseit->second.swap(newvalueit->second);
			}
			return true;
		}
		return false;
    }

    FMTforest FMTspatialschedule::getforestperiod(const int& period) const
    {
        FMTforest forest(this->copyextent<Core::FMTdevelopment>());//Setting layer information
        for(std::map<FMTcoordinate,Graph::FMTlinegraph>::const_iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
        {
            const Graph::FMTlinegraph* local_graph = &graphit->second;
            const std::vector<double> solutions(1,this->getcellsize());
			std::vector<Core::FMTactualdevelopment> actdev = local_graph->getperiodstopdev(period,&solutions[0]);//
            forest.mapping[graphit->first]=Core::FMTdevelopment(actdev.front());
        }
        return forest;
    }

    bool FMTspatialschedule::allow_action(const FMTspatialaction& targetaction,const std::vector<Core::FMTaction>& modelactions,
                                          const FMTcoordinate& location, const int& period) const
    {
        int MINGU = static_cast<int>((period - targetaction.green_up));
        for(size_t green_up = std::max(0,MINGU); green_up < static_cast<size_t>(period); ++green_up)
        {
            int naction_id = 0;
            for (const Core::FMTaction& mact : modelactions)
            {
                if (std::find(targetaction.neighbors.begin(), targetaction.neighbors.end(), mact.getname()) != targetaction.neighbors.end())
                {
                    std::vector<FMTsaeventcontainer::const_iterator> eventsatgupaid = events.getevents(green_up,naction_id);
                    if(!eventsatgupaid.empty())
                    {
                        for(const auto eventit : eventsatgupaid)
                        {
                            if(eventit->withinc(static_cast<unsigned int>(targetaction.adjacency),location))
                            {
                            return false;
                            }
                        }
                    }
                }
                naction_id++;
            }
        }
        return true;
    }

    std::set<FMTcoordinate> FMTspatialschedule::getallowable(const FMTspatialaction& targetaction,
                                                             const std::vector<Core::FMTaction>& modelactions, const int& period) const
    {
    std::set<FMTcoordinate> allowable;
    for(std::map<FMTcoordinate,Graph::FMTlinegraph>::const_iterator itc = mapping.begin(); itc != mapping.end(); ++itc)
        {
        if (allow_action(targetaction,modelactions,itc->first,period))
            {
            allowable.insert(itc->first);
            }
        }
    return allowable;
    }

    int FMTspatialschedule::lastperiod() const
    {
        Graph::FMTlinegraph flgraph = mapping.begin()->second;
        return flgraph.getlastperiod();
    }
}
