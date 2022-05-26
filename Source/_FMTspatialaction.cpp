/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTspatialaction.hpp"
#include "FMToutput.hpp"
#include <iostream>
#include <limits>
#include "FMTconstraint.hpp"

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
                     const std::vector<std::string>& lneighbors,
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
                     const std::vector<std::string>& lneighbors,
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

	std::vector<Core::FMTconstraint> FMTspatialaction::to_constraints() const
    {
		std::vector<Core::FMTconstraint> constraints;
        constexpr double max_all = std::numeric_limits<double>::infinity();
        constexpr int stopperiod = std::numeric_limits<int>::max();
        int startperiod = 1;
        if ( minimal_size>0)
        {
            Core::FMTconstraint newconst(Core::FMTconstrainttype::FMTspatialsize,Core::FMToutput(name));
			std::string target = "RHS";
            double minsize = static_cast<double>(minimal_size);
            newconst.addbounds(Core::FMTyldbounds(Core::FMTsection::Optimize, target, max_all, minsize));
			if(size_weight>0)
            {
				std::string gtarget = "GOAL_"+name;
                double sizew = size_weight;
                newconst.addbounds(Core::FMTyldbounds(Core::FMTsection::Optimize,gtarget,sizew,sizew));
            }
            newconst.setbounds(Core::FMTperbounds(Core::FMTsection::Optimize, stopperiod, startperiod));
            constraints.push_back(newconst);
        }
        if (maximal_size>0)
        {
            Core::FMTconstraint newconst(Core::FMTconstrainttype::FMTspatialsize,Core::FMToutput(name));
			std::string target = "RHS";
            double maxsize = static_cast<double>(maximal_size);
            double minsize = 0;
            newconst.addbounds(Core::FMTyldbounds(Core::FMTsection::Optimize, target, maxsize, minsize));
			if(size_weight>0)
            {
				std::string gtarget = "GOAL_"+name;
                double sizew = size_weight;
                newconst.addbounds(Core::FMTyldbounds(Core::FMTsection::Optimize,gtarget,sizew,sizew));
            }
            newconst.setbounds(Core::FMTperbounds(Core::FMTsection::Optimize, stopperiod, startperiod));
            constraints.push_back(newconst);
        }
        if (adjacency>0)
        {
            Core::FMTconstraint newconst(Core::FMTconstrainttype::FMTspatialadjacency,Core::FMToutput(name));
			std::string target = "RHS";
            double adjac = static_cast<double>(adjacency);
			newconst.addbounds(Core::FMTyldbounds(Core::FMTsection::Optimize,target,max_all,adjac));
            if (adjacency_weight>0)
            {
				std::string gtarget = "GOAL_"+name;
                double adjacw = adjacency_weight;
                newconst.addbounds(Core::FMTyldbounds(Core::FMTsection::Optimize,gtarget,adjacw,adjacw));
            }
            newconst.setbounds(Core::FMTperbounds(Core::FMTsection::Optimize, stopperiod, startperiod));
            constraints.push_back(newconst);
        }
       /* if ( green_up>0)
        {
            Core::FMTconstraint newconst(Core::FMTconstrainttype::FMTspatialgreenup,Core::FMToutput(name));
			std::string target = "RHS";
            double gup = static_cast<double>(green_up);
			newconst.addbounds(Core::FMTyldbounds(Core::FMTsection::Optimize, target, max_all, gup));
			if (greenup_weight>0)
			{
				std::string gtarget = "GOAL_"+name;
                double gupw = greenup_weight;
                newconst.addbounds(Core::FMTyldbounds(Core::FMTsection::Optimize, gtarget,gupw,gupw));
			}
			newconst.setbounds(Core::FMTperbounds(Core::FMTsection::Optimize, stopperiod, startperiod));
            constraints.push_back(newconst);
        }*/
        return constraints;
    }

}
