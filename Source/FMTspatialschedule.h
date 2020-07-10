#ifndef FMTSPATIALSCHEDULE_H
#define FMTSPATIALSCHEDULE_H

#include "FMTlayer.h"
#include "FMTforest.h"
#include "FMTlinegraph.h"

namespace Spatial
{
// DocString: FMTspatialschedule
/**
This class is a map containing a linear graph for each cell. The graph represent the stand,
the action and the transition at each each period for the cell. Can be build randomly or
with a schedule.
*/
class FMTspatialschedule : public FMTlayer<Graph::FMTlinegraph>
{
    mutable std::unordered_map<size_t, std::vector<double>>outputscache;
    public:
        // DocString: FMTspatialschedule()
		/**
		Default constructor of FMTspatialschedule
		*/
        FMTspatialschedule();
        // DocString: FMTspatialschedule(const FMTforest)
		/**
		Constructor of FMTspatialschedule based on FMTforest. It's initializing
		every graph in the map base on developments types in each cell.
		*/
        FMTspatialschedule(const FMTforest& initialmap);
        // DocString: ~FMTspatialschedule()
		/**
		Default destructor of FMTspatialschedule
		*/
        virtual ~FMTspatialschedule() = default;
        // DocString: FMTspatialschedule(const FMTspatialschedule)
		/**
		Copy constructor of FMTspatialschedule
		*/
        FMTspatialschedule(const FMTspatialschedule& other);
        // DocString: FMTspatialschedule::=
		/**
		Copy assignment of FMTspatialschedule
		*/
        FMTspatialschedule& operator=(const FMTspatialschedule& rhs);
        // DocString: FMTspatialschedule::==
		/**
		Comparison operator equal to
		*/
        bool operator == (const FMTspatialschedule& rhs)const;
        // DocString: FMTspatialschedule::!=
		/**
		Comparison operator different than
		*/
        bool operator != (const FMTspatialschedule& rhs)const;
        // DocString: FMTspatialschedule::empty
        /**
        Test whether the map is empty.
        */
        bool empty() const {return mapping.empty();};
        // DocString: FMTspatialschedule::copyfromselected(const FMTspatialschedule, const std::vector<size_t>)
        /**

        */
        bool copyfromselected(const FMTspatialschedule& rhs, const std::vector<size_t>& selected);
        // DocString: FMTspatialschedule::swapfromselected(const FMTspatialschedule, const std::vector<size_t>)
        /**

        */
		bool swapfromselected(FMTspatialschedule& rhs, const std::vector<size_t>& selected);
        // DocString: FMTspatialschedule::getforestperiod(const int)
        /**

        */
        FMTforest getforestperiod(const int& period) const;
		// DocString: FMTspatialschedule::allow_action
        /**
        Check in all events around the location during periods corresponding to green up delay
        if an action present in neighbors is in the adjacency limit.
        */
        bool allow_action(const FMTspatialaction& targetaction, const std::vector<Core::FMTaction>& modelactions,
                          const FMTcoordinate& location,const int& period) const;
		//
		/*

		*/
		std::set<FMTcoordinate> getallowable(const FMTspatialaction& targetaction, const std::vector<Core::FMTaction>& modelactions, const int& period) const;

        int lastperiod() const;
        //Dont forget to implement FMTforest functions needed plus modify FMTevent to have only one class
        //Modification must replace FMTsaeventcontainer as the equivalent as disturbancestacks for all FMTspatialschedule
        //When build the actions must be in order to fill the graphs and the event container. At the end of the period we must implement a grow for every cell
    protected:
        FMTsaeventcontainer events;
    private:
};
}

#endif // FMTSPATIALSCHEDULE_H
