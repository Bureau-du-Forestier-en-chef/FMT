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
	const FMTOperatingAreaClusterBinary& FMTOperatingAreaCluster::getBinary(const Core::FMTMask& mask) const
			{
			return *std::find_if(binaries.begin(), binaries.end(), FMTOperatingAreaComparator(mask));
			}

	FMTOperatingAreaCluster::FMTOperatingAreaCluster(const FMTOperatingAreaClusterBinary& localcentroid,
		const std::vector<FMTOperatingAreaClusterBinary>& neighborsbinaries) :
        binaries(neighborsbinaries),centroid(localcentroid),minimalarea(-1.0),maximalarea(-1.0)
	{

	}


    FMTOperatingAreaCluster::FMTOperatingAreaCluster(const FMTOperatingAreaCluster& basecluster,
                                                const double& lminimalarea,
								const double& lmaximalarea): binaries(basecluster.binaries),centroid(basecluster.centroid),
                                                        minimalarea(lminimalarea),maximalarea(lmaximalarea)
    {

    }

	double FMTOperatingAreaCluster::getTotalPotentialArea() const
        {
        double total = 0;
        total+=centroid.getArea();
        for (const FMTOperatingAreaClusterBinary& binary : binaries)
            {
            total+=binary.getArea();
            }
        return total;
        }

	double FMTOperatingAreaCluster::getMinimalArea() const
        {
        return std::max(centroid.getArea(),minimalarea);
        }

    double FMTOperatingAreaCluster::getMaximalArea() const
        {
        return std::min(maximalarea,getTotalPotentialArea());
        }

	double FMTOperatingAreaCluster::getMaximalStats() const
		{
		double maxincluster = 0;
		for (const FMTOperatingAreaClusterBinary& binary : getBinaries())
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

    bool FMTOperatingAreaCluster::isValidarea(const double& area) const
        {
        return (area >= getMinimalArea() && area <= getMaximalArea());
        }


	void FMTOperatingAreaCluster::setMaximalObjectiveVariable(const int& lmaxvar)
		{
		maxvar = lmaxvar;
		}
	void FMTOperatingAreaCluster::setMinimalObjectiveVariable(const int& lminvar)
		{
		minvar = lminvar;
		}

    void FMTOperatingAreaCluster::setMinimalArea(const double& newminimalarea)
        {
        minimalarea = newminimalarea;
        }

    void FMTOperatingAreaCluster::setMaximalArea(const double& newmaximalarea)
        {
        maximalarea = newmaximalarea;
        }

    bool FMTOperatingAreaCluster::isValidareabounds() const
        {
        return (getTotalPotentialArea() >= minimalarea && maximalarea >= minimalarea);
        }

    FMTOperatingAreaCluster FMTOperatingAreaCluster::getFilteredCluster(const Core::FMTMask& filterMask) const
        {
        FMTOperatingAreaCluster newcluster(*this);
        if (centroid.getMask().isSubsetOf(filterMask))
            {
            std::vector<FMTOperatingAreaClusterBinary>newbinaries;
            for (const FMTOperatingAreaClusterBinary& binary : binaries)
                {
                if (binary.getMask().isSubsetOf(filterMask))
                    {
                    FMTOperatingAreaClusterBinary newbinary(binary);
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

    std::vector<Core::FMTMask>FMTOperatingAreaCluster::getAllMasks() const
        {
        std::vector<Core::FMTMask>allmasks;
        allmasks.push_back(centroid.getMask());
        for (const FMTOperatingAreaClusterBinary& binary : binaries)
            {
            allmasks.push_back(binary.getMask());
            }
        return allmasks;
        }


}
#endif
