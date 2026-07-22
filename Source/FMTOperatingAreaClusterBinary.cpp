/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI

#include "FMTOperatingAreaClusterBinary.h"
#include <vector>
#include <algorithm>
#include "FMTOutput.h"

namespace Heuristics
{

	FMTOperatingAreaClusterBinary::FMTOperatingAreaClusterBinary(const FMTOperatingArea& oparea) :
		FMTOperatingArea(oparea), statistic()
		{

		}

	void FMTOperatingAreaClusterBinary::setStatistic(const double& statvalue)
		{
		statistic = statvalue;
		}

	void FMTOperatingAreaClusterBinary::setVariable(const int& lvariable)
		{
		variable = lvariable;
		}


	Core::FMTOutput FMTOperatingAreaClusterBinary::getOutputIntersect(const Core::FMTOutput& output,const std::vector<Core::FMTTheme>& themes) const
		{
		return output.intersectWithMask(getMask(),themes);
		}

	std::vector<FMTOperatingAreaClusterBinary> FMTOperatingAreaClusterBinary::filterNeighbors(std::vector<FMTOperatingAreaClusterBinary> potentiallink) const
		{
		std::vector<FMTOperatingAreaClusterBinary>finalbinaries;
		while (finalbinaries.size()!= potentiallink.size())
			{
			if (!finalbinaries.empty())
				{
				potentiallink = finalbinaries;
				finalbinaries.clear();
				}
			std::map<Core::FMTMask, std::vector<Core::FMTMask>>neighbors;
			for (const FMTOperatingAreaClusterBinary& mainbinary : potentiallink)
				{
				neighbors[mainbinary.getMask()] = std::vector<Core::FMTMask>(1, this->getMask());
				}
			for (std::map<Core::FMTMask, std::vector<Core::FMTMask>>::iterator dcit = neighbors.begin();
				dcit != neighbors.end(); dcit++)
				{
					for (const FMTOperatingAreaClusterBinary& mainbinary : potentiallink)
					{
						if (dcit->first != mainbinary.getMask())
						{
							const std::vector<Core::FMTMask> baseneighbors = mainbinary.getNeighbors();
							dcit->second.insert(dcit->second.end(), baseneighbors.begin(), baseneighbors.end());
						}
					}
				}
			for (const FMTOperatingAreaClusterBinary& mainbinary : potentiallink)
				{
				if (std::find(neighbors[mainbinary.getMask()].begin(), neighbors[mainbinary.getMask()].end(), mainbinary.getMask()) != neighbors[mainbinary.getMask()].end()||
                    std::find(neighbors[mainbinary.getMask()].begin(), neighbors[mainbinary.getMask()].end(), this->getMask()) != neighbors[mainbinary.getMask()].end())
					{
					finalbinaries.push_back(mainbinary);
					}
				}
			}
		return finalbinaries;
		}
}

#endif
