#include "FMTspatialaction.h"
#include "FMToutput.h"
#include <iostream>
#include <limits>

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
                     const double& lgreenup_weight,
                     const double& ladjacency_weight,
                     const double& lsize_weight):
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
                    ((  minimal_size>0 || maximal_size>0 ) && size_weight>0) ||
                    (   adjacency>0 && adjacency_weight>0) ||
                    (   green_up>0 && greenup_weight>0)
                )
            {
                return true;
            }
        }
        return false;
    }

    vector<FMTconstraint> FMTspatialaction::to_constraints() const
    {
        vector<FMTconstraint> constraints;
        double max_all = std::numeric_limits<double>::infinity();
        int stopperiod = numeric_limits<int>::max();
        int startperiod = 1;
        if ( minimal_size>0 || maximal_size>0 )
        {
            FMTconstraint newconst(FMTconstrainttype::FMTspatialsize,FMToutput(name));
            string target = "RHS";
            double maxsize = static_cast<double>(maximal_size);
            double minsize = static_cast<double>(minimal_size);
            newconst.addbounds(FMTyldbounds(FMTwssect::Optimize, target, maxsize, minsize));
			if(size_weight>0)
            {
                string gtarget = "GOAL_"+name;
                double sizew = size_weight;
                newconst.addbounds(FMTyldbounds(FMTwssect::Optimize,gtarget,sizew,sizew));
            }
            newconst.setbounds(FMTperbounds(FMTwssect::Optimize, stopperiod, startperiod));
            constraints.push_back(newconst);
        }
        if (adjacency>0)
        {
            FMTconstraint newconst(FMTconstrainttype::FMTspatialadjacency,FMToutput(name));
            string target = "RHS";
            double adjac = static_cast<double>(adjacency);
			newconst.addbounds(FMTyldbounds(FMTwssect::Optimize,target,max_all,adjac));
            if (adjacency_weight>0)
            {
                string gtarget = "GOAL_"+name;
                double adjacw = adjacency_weight;
                newconst.addbounds(FMTyldbounds(FMTwssect::Optimize,gtarget,adjacw,adjacw));
            }
            newconst.setbounds(FMTperbounds(FMTwssect::Optimize, stopperiod, startperiod));
            constraints.push_back(newconst);
        }
        if ( green_up>0)
        {
            FMTconstraint newconst(FMTconstrainttype::FMTspatialgreenup,FMToutput(name));
            string target = "RHS";
            double gup = static_cast<double>(green_up);
			newconst.addbounds(FMTyldbounds(FMTwssect::Optimize, target, max_all, gup));
			if (greenup_weight>0)
			{
                string gtarget = "GOAL_"+name;
                double gupw = greenup_weight;
                newconst.addbounds(FMTyldbounds(FMTwssect::Optimize, gtarget,gupw,gupw));
			}
			newconst.setbounds(FMTperbounds(FMTwssect::Optimize, stopperiod, startperiod));
            constraints.push_back(newconst);
        }
        return constraints;
    }

}
