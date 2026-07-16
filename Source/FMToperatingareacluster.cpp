/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
#include "FMToperatingareacluster.h"
#include <algorithm>
namespace Heuristics

{
	const FMToperatingareaclusterbinary& FMToperatingareacluster::getBinary(const Core::FMTMask& mask) const
			{
			return *std::find_if(binaries.begin(), binaries.end(), FMTOperatingAreaComparator(mask));
			}

	FMToperatingareacluster::FMToperatingareacluster(const FMToperatingareaclusterbinary& localcentroid,
		const std::vector<FMToperatingareaclusterbinary>& neighborsbinaries) :
        binaries(neighborsbinaries),centroid(localcentroid),minimalarea(-1.0),maximalarea(-1.0)
	{

	}


    FMToperatingareacluster::FMToperatingareacluster(const FMToperatingareacluster& basecluster,
                                                const double& lminimalarea,
								const double& lmaximalarea): binaries(basecluster.binaries),centroid(basecluster.centroid),
                                                        minimalarea(lminimalarea),maximalarea(lmaximalarea)
    {

    }

	double FMToperatingareacluster::getTotalPotentialArea() const
        {
        double total = 0;
        total+=centroid.getArea();
        for (const FMToperatingareaclusterbinary& binary : binaries)
            {
            total+=binary.getArea();
            }
        return total;
        }

	double FMToperatingareacluster::getMinimalArea() const
        {
        return std::max(centroid.getArea(),minimalarea);
        }

    double FMToperatingareacluster::getMaximalArea() const
        {
        return std::min(maximalarea,getTotalPotentialArea());
        }

	double FMToperatingareacluster::getMaximalStats() const
		{
		double maxincluster = 0;
		for (const FMToperatingareaclusterbinary& binary : getBinaries())
			{
				if (binary.getStatistic() > maxincluster)
				{
					maxincluster = binary.getStatistic();
				}
			}
		if (getCentroid().getStatistic() > maxincluster)
			{
			maxincluster = getCentroid().getStatistic();
			}
		return maxincluster;
		}

    bool FMToperatingareacluster::isValidarea(const double& area) const
        {
        return (area >= getMinimalArea() && area <= getMaximalArea());
        }


	void FMToperatingareacluster::setMaximalObjectiveVariable(const int& lmaxvar)
		{
		maxvar = lmaxvar;
		}
	void FMToperatingareacluster::setMinimalObjectiveVariable(const int& lminvar)
		{
		minvar = lminvar;
		}

    void FMToperatingareacluster::setMinimalArea(const double& newminimalarea)
        {
        minimalarea = newminimalarea;
        }

    void FMToperatingareacluster::setMaximalArea(const double& newmaximalarea)
        {
        maximalarea = newmaximalarea;
        }

    bool FMToperatingareacluster::isValidareabounds() const
        {
        return (getTotalPotentialArea() >= minimalarea && maximalarea >= minimalarea);
        }

    FMToperatingareacluster FMToperatingareacluster::getFilteredCluster(const Core::FMTMask& filterMask) const
        {
        FMToperatingareacluster newcluster(*this);
        if (centroid.getMask().isSubsetOf(filterMask))
            {
            std::vector<FMToperatingareaclusterbinary>newbinaries;
            for (const FMToperatingareaclusterbinary& binary : binaries)
                {
                if (binary.getMask().isSubsetOf(filterMask))
                    {
                    FMToperatingareaclusterbinary newbinary(binary);
                    std::vector<Core::FMTMask>newneighbors;
                    for (const Core::FMTMask& link : binary.getNeighbors())
                        {
                        if (link.isSubsetOf(filterMask))
                            {
                            newneighbors.push_back(link);
                            }

                        }
                    newbinary.setNeighbors(newneighbors);
                    newbinaries.push_back(newbinary);
                    }
                }
			newcluster.binaries = newbinaries;
			newcluster.centroid = centroid;
            }
        return newcluster;
        }

    std::vector<Core::FMTMask>FMToperatingareacluster::getAllMasks() const
        {
        std::vector<Core::FMTMask>allmasks;
        allmasks.push_back(centroid.getMask());
        for (const FMToperatingareaclusterbinary& binary : binaries)
            {
            allmasks.push_back(binary.getMask());
            }
        return allmasks;
        }


}
#endif
