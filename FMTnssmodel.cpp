/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTnssmodel.h"
namespace Models
{
	FMTnssmodel::FMTnssmodel(const FMTmodel& rhs, unsigned int seed):FMTmodel(rhs),generator(seed) {}

	Core::FMTschedule FMTnssmodel::simulate(const std::vector<double>& actionsproportions,bool grow)
		{
		std::vector<int>shuffledactionsindex;
		int index = 0;
		const double initialarea = this->getinitialarea();
		std::vector<double>areastooperate;
		for (const double& proportion : actionsproportions)
			{
			if (proportion>0)
				{
				areastooperate.push_back(initialarea*proportion);
				shuffledactionsindex.push_back(index);
				}
			++index;
			}
		std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::vector<double>>>scheduledata;
		int period = 0;
		bool actionoperated = true;
		while (!shuffledactionsindex.empty() && actionoperated)
			{
			actionoperated = false;
			std::shuffle(shuffledactionsindex.begin(), shuffledactionsindex.end(), generator);
			std::vector<int>actiondone;
			for (const int& randomactionindex : shuffledactionsindex)
				{
				std::vector<Core::FMTactualdevelopment>operabledevs;
				std::vector<Core::FMTactualdevelopment>newarea;
				for (const Core::FMTactualdevelopment& dev : area)
					{
						if (dev.operable(actions.at(randomactionindex), yields))
							{
							operabledevs.push_back(dev);
						}else {
							newarea.push_back(dev);
							}
					}
				if (!operabledevs.empty())
					{
					
					if (scheduledata.find(actions.at(randomactionindex))==scheduledata.end())
						{
						scheduledata[actions.at(randomactionindex)] = std::map<Core::FMTdevelopment, std::vector<double>>();
						}
					std::shuffle(operabledevs.begin(), operabledevs.end(), generator);
					while (areastooperate.at(randomactionindex)>0 && !operabledevs.empty())
						{
						const double operatedarea = std::min(areastooperate.at(randomactionindex), operabledevs.begin()->getarea());
						if (scheduledata.at(actions.at(randomactionindex)).find(*operabledevs.begin()) == scheduledata.at(actions.at(randomactionindex)).end())
							{
							scheduledata[actions.at(randomactionindex)][*operabledevs.begin()] = std::vector<double>(1, operatedarea);
							}
						for (const Core::FMTdevelopmentpath& path : operabledevs.begin()->operate(actions.at(randomactionindex),
																			transitions.at(randomactionindex), yields, themes))
							{
							const double newareavalue = operatedarea*(path.proportion/100);
							const Core::FMTactualdevelopment newdev(*path.development, newareavalue);
							std::vector<Core::FMTactualdevelopment>::iterator restdev = std::find_if(newarea.begin(), newarea.end(), Core::FMTactualdevelopmentcomparator(&newdev));
							if (restdev!= newarea.end())
								{
								restdev->setarea(restdev->getarea() + newareavalue);
							}else {
								newarea.push_back(newdev);
								}
							}
						if (operatedarea!=operabledevs.begin()->getarea())
							{
							const Core::FMTactualdevelopment newdev(*operabledevs.begin(), operabledevs.begin()->getarea()- operatedarea);
							newarea.push_back(newdev);
							}
						period = operabledevs.begin()->period;
						areastooperate.at(randomactionindex)-= operatedarea;
						operabledevs.erase(operabledevs.begin());
						}
					if (!operabledevs.empty())
						{
						newarea.insert(newarea.end(), operabledevs.begin(), operabledevs.end());
						}
					actionoperated = true;
					}
				if (areastooperate.at(randomactionindex) <= 0)
					{
					actiondone.push_back(randomactionindex);
					}
				area = newarea;
				}
			std::vector<int>newactionatargets;
			for (const int& actionindex : shuffledactionsindex)
				{
				if (std::find(actiondone.begin(), actiondone.end(), actionindex)== actiondone.end())
					{
					newactionatargets.push_back(actionindex);
					}
				}
			shuffledactionsindex = newactionatargets;
			}
		if (grow)
			{
			for (Core::FMTactualdevelopment& dev : area)
				{
				dev = Core::FMTactualdevelopment(dev.grow(), dev.getarea());
				}
			}
		return Core::FMTschedule(period, scheduledata);
		}
}

BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTnssmodel);
