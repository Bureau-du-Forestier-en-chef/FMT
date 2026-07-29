/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
#include "FMTOperatingAreaCluster.h"
#include <algorithm>
namespace Heuristics

{
	const FMTOperatingAreaClusterBinary& FMTOperatingAreaCluster::getBinary(const Core::FMTMask& mask) const
			{
			return *std::find_if(m_binaries.begin(), m_binaries.end(), FMTOperatingAreaComparator(mask));
			}

	FMTOperatingAreaCluster::FMTOperatingAreaCluster(const FMTOperatingAreaClusterBinary& localcentroid,
		const std::vector<FMTOperatingAreaClusterBinary>& neighborsbinaries) :
        m_binaries(neighborsbinaries),m_centroid(localcentroid),m_minimalarea(-1.0),m_maximalarea(-1.0)
	{

	}


    FMTOperatingAreaCluster::FMTOperatingAreaCluster(const FMTOperatingAreaCluster& basecluster,
                                                const double& lminimalarea,
								const double& lmaximalarea): m_binaries(basecluster.m_binaries),m_centroid(basecluster.m_centroid),
                                                        m_minimalarea(lminimalarea),m_maximalarea(lmaximalarea)
    {

    }

	double FMTOperatingAreaCluster::getTotalPotentialArea() const
        {
        double total = 0;
        total+=m_centroid.getArea();
        for (const FMTOperatingAreaClusterBinary& binary : m_binaries)
            {
            total+=binary.getArea();
            }
        return total;
        }

	double FMTOperatingAreaCluster::getMinimalArea() const
        {
        return std::max(m_centroid.getArea(),m_minimalarea);
        }

    double FMTOperatingAreaCluster::getMaximalArea() const
        {
        return std::min(m_maximalarea,getTotalPotentialArea());
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

    bool FMTOperatingAreaCluster::isValidArea(const double& area) const
        {
        return (area >= getMinimalArea() && area <= getMaximalArea());
        }


	void FMTOperatingAreaCluster::setMaximalObjectiveVariable(const int& lmaxvar)
		{
		m_maxvar = lmaxvar;
		}
	void FMTOperatingAreaCluster::setMinimalObjectiveVariable(const int& lminvar)
		{
		m_minvar = lminvar;
		}

    void FMTOperatingAreaCluster::setMinimalArea(const double& newminimalarea)
        {
        m_minimalarea = newminimalarea;
        }

    void FMTOperatingAreaCluster::setMaximalArea(const double& newmaximalarea)
        {
        m_maximalarea = newmaximalarea;
        }

    bool FMTOperatingAreaCluster::isValidAreaBounds() const
        {
        return (getTotalPotentialArea() >= m_minimalarea && m_maximalarea >= m_minimalarea);
        }

    FMTOperatingAreaCluster FMTOperatingAreaCluster::getFilteredCluster(const Core::FMTMask& filterMask) const
        {
        FMTOperatingAreaCluster newcluster(*this);
        if (m_centroid.getMask().isSubsetOf(filterMask))
            {
            std::vector<FMTOperatingAreaClusterBinary>newbinaries;
            for (const FMTOperatingAreaClusterBinary& binary : m_binaries)
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
			newcluster.m_binaries = newbinaries;
			newcluster.m_centroid = m_centroid;
            }
        return newcluster;
        }

    std::vector<Core::FMTMask>FMTOperatingAreaCluster::getAllMasks() const
        {
        std::vector<Core::FMTMask>allmasks;
        allmasks.push_back(m_centroid.getMask());
        for (const FMTOperatingAreaClusterBinary& binary : m_binaries)
            {
            allmasks.push_back(binary.getMask());
            }
        return allmasks;
        }


}
#endif
