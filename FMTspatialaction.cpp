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
        neighbors_size(){}

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
                         neighbors_size(lneighbors_size){}
    FMTspatialaction::FMTspatialaction(const FMTaction& action) :
                FMTaction(action),
                neighbors(),
                green_up(),
                adjacency(),
                minimal_size(),
                maximal_size(),
                neighbors_size(4){}

    FMTspatialaction::FMTspatialaction(const FMTspatialaction& rhs) :
                FMTaction(rhs),
                neighbors(rhs.neighbors),
                green_up(rhs.green_up),
                adjacency(rhs.adjacency),
                minimal_size(rhs.minimal_size),
                maximal_size(rhs.maximal_size),
                neighbors_size(rhs.neighbors_size){}

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
            }
        return *this;
        }

}
