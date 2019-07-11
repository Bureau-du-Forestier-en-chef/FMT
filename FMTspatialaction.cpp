#include "FMTspatialaction.h"

namespace Spatial
{
    FMTspatialaction::FMTspatialaction() :
        FMTaction(),
        neighbors(),
        green_up(),
        adjacency(),
        minimal_size(),
        maximal_size(),
        neighbors_size(),
        greenup_weight(0),
        adjacency_weight(0),
        size_weight(0){}

    FMTspatialaction::FMTspatialaction(const FMTaction& action,
                     const vector<string>& lneighbors,
                     const size_t& lgreen_up,
                     const size_t& ladjacency,
                     const size_t& lminimal_size,
                     const size_t& lmaximal_size,
                     const size_t& lneighbors_size):
                         FMTaction(action),
                         neighbors(lneighbors),
                         green_up(lgreen_up),
                         adjacency(ladjacency),
                         minimal_size(lminimal_size),
                         maximal_size(lmaximal_size),
                         neighbors_size(lneighbors_size),
                         greenup_weight(0),
                         adjacency_weight(0),
                         size_weight(0){}

    FMTspatialaction::FMTspatialaction(const FMTaction& action,
                     const vector<string>& lneighbors,
                     const size_t& lgreen_up,
                     const size_t& ladjacency,
                     const size_t& lminimal_size,
                     const size_t& lmaximal_size,
                     const size_t& lneighbors_size,
                     double& lgreenup_weight,
                     double& ladjacency_weight,
                     double& lsize_weight):
                         FMTaction(action),
                         neighbors(lneighbors),
                         green_up(lgreen_up),
                         adjacency(ladjacency),
                         minimal_size(lminimal_size),
                         maximal_size(lmaximal_size),
                         neighbors_size(lneighbors_size),
                         greenup_weight(lgreenup_weight),
                         adjacency_weight(ladjacency_weight),
                         size_weight(lsize_weight){}

    FMTspatialaction::FMTspatialaction(const FMTaction& action) :
                FMTaction(action),
                neighbors(),
                green_up(),
                adjacency(),
                minimal_size(),
                maximal_size(),
                neighbors_size(4),
                greenup_weight(0),
                adjacency_weight(0),
                size_weight(0){}

    FMTspatialaction::FMTspatialaction(const FMTspatialaction& rhs) :
                FMTaction(rhs),
                neighbors(rhs.neighbors),
                green_up(rhs.green_up),
                adjacency(rhs.adjacency),
                minimal_size(rhs.minimal_size),
                maximal_size(rhs.maximal_size),
                neighbors_size(rhs.neighbors_size),
                greenup_weight(rhs.greenup_weight),
                adjacency_weight(rhs.adjacency_weight),
                size_weight(rhs.size_weight){}

    FMTspatialaction& FMTspatialaction::operator = (const FMTspatialaction& rhs)
        {
        if(this!=&rhs)
            {
            FMTaction::operator = (rhs);
            neighbors = rhs.neighbors;
            green_up = rhs.green_up;
            adjacency = rhs.adjacency;
            minimal_size = rhs.minimal_size;
            maximal_size = rhs.maximal_size;
            neighbors_size = rhs.neighbors_size;
            greenup_weight = rhs.greenup_weight;
            adjacency_weight = rhs.adjacency_weight;
            size_weight = rhs.size_weight;
            }
        return *this;
        }
    bool FMTspatialaction::simulated_annealing_valid()const
    {
        if (!( greenup_weight == 0 && adjacency_weight == 0 && size_weight == 0))
        {
            if  (
                    ((  minimal_size>0 || maximal_size>0 ) && size_weight !=0) ||
                    (   adjacency>0 && adjacency_weight!=0) ||
                    (   green_up>0 && greenup_weight!=0)
                )
            {
                return true;
            }
        }
        return false;
    }
}
