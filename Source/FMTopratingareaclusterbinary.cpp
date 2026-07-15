/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI

#include "FMToperatingareaclusterbinary.h"
#include <vector>
#include <algorithm>
#include "FMToutput.h"

namespace Heuristics
{

	FMToperatingareaclusterbinary::FMToperatingareaclusterbinary(const FMToperatingarea& oparea) :
		FMToperatingarea(oparea), statistic()
		{

		}

	void FMToperatingareaclusterbinary::setStatistic(const double& statvalue)
		{
		statistic = statvalue;
		}

	void FMToperatingareaclusterbinary::setVariable(const int& lvariable)
		{
		variable = lvariable;
		}


	Core::FMToutput FMToperatingareaclusterbinary::getOutputIntersect(const Core::FMToutput& output,const std::vector<Core::FMTtheme>& themes) const
		{
		return output.intersectWithMask(getMask(),themes);
		}

	std::vector<FMToperatingareaclusterbinary> FMToperatingareaclusterbinary::filterNeighbors(std::vector<FMToperatingareaclusterbinary> potentiallink) const
		{
		std::vector<FMToperatingareaclusterbinary>finalbinaries;
		while (finalbinaries.size()!= potentiallink.size())
			{
			if (!finalbinaries.empty())
				{
				potentiallink = finalbinaries;
				finalbinaries.clear();
				}
			std::map<Core::FMTmask, std::vector<Core::FMTmask>>neighbors;
			for (const FMToperatingareaclusterbinary& mainbinary : potentiallink)
				{
				neighbors[mainbinary.getMask()] = std::vector<Core::FMTmask>(1, this->getMask());
				}
			for (std::map<Core::FMTmask, std::vector<Core::FMTmask>>::iterator dcit = neighbors.begin();
				dcit != neighbors.end(); dcit++)
				{
					for (const FMToperatingareaclusterbinary& mainbinary : potentiallink)
					{
						if (dcit->first != mainbinary.getMask())
						{
							const std::vector<Core::FMTmask> baseneighbors = mainbinary.getNeighbors();
							dcit->second.insert(dcit->second.end(), baseneighbors.begin(), baseneighbors.end());
						}
					}
				}
			for (const FMToperatingareaclusterbinary& mainbinary : potentiallink)
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
