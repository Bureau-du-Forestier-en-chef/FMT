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
	const FMToperatingareaclusterbinary& FMToperatingareacluster::getbinary(const Core::FMTmask& mask) const
			{
			return *std::find_if(binaries.begin(), binaries.end(), FMToperatingareacomparator(mask));
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

	double FMToperatingareacluster::gettotalpotentialarea() const
        {
        double total = 0;
        total+=centroid.getarea();
        for (const FMToperatingareaclusterbinary& binary : binaries)
            {
            total+=binary.getarea();
            }
        return total;
        }

	double FMToperatingareacluster::getminimalarea() const
        {
        return std::max(centroid.getarea(),minimalarea);
        }

    double FMToperatingareacluster::getmaximalarea() const
        {
        return std::min(maximalarea,gettotalpotentialarea());
        }

    bool FMToperatingareacluster::isvalidarea(const double& area) const
        {
        return (area >= getminimalarea() && area <= getmaximalarea());
        }


	void FMToperatingareacluster::setmaximalobjectivevariable(const int& lmaxvar)
		{
		maxvar = lmaxvar;
		}
	void FMToperatingareacluster::setminimalobjectivevariable(const int& lminvar)
		{
		minvar = lminvar;
		}

    void FMToperatingareacluster::setminimalarea(const double& newminimalarea)
        {
        minimalarea = newminimalarea;
        }

    void FMToperatingareacluster::setmaximalarea(const double& newmaximalarea)
        {
        maximalarea = newmaximalarea;
        }

    bool FMToperatingareacluster::isvalidareabounds() const
        {
        return (gettotalpotentialarea() >= minimalarea && maximalarea >= minimalarea);
        }

    FMToperatingareacluster FMToperatingareacluster::getfilteredcluster(const Core::FMTmask& filtermask) const
        {
        FMToperatingareacluster newcluster;
        if (centroid.getmask().issubsetof(filtermask))
            {
            std::vector<FMToperatingareaclusterbinary>newbinaries;
            for (const FMToperatingareaclusterbinary& binary : binaries)
                {
                if (binary.getmask().issubsetof(filtermask))
                    {
                    FMToperatingareaclusterbinary newbinary(binary);
                    std::vector<Core::FMTmask>newneighbors;
                    for (const Core::FMTmask& link : binary.getneighbors())
                        {
                        if (link.issubsetof(filtermask))
                            {
                            newneighbors.push_back(link);
                            }

                        }
                    newbinary.setneighbors(newneighbors);
                    newbinaries.push_back(newbinary);
                    }
                }
            newcluster = FMToperatingareacluster(FMToperatingareacluster(centroid,newbinaries),minimalarea,maximalarea);
            }
        return newcluster;
        }

    std::vector<Core::FMTmask>FMToperatingareacluster::getallmasks() const
        {
        std::vector<Core::FMTmask>allmasks;
        allmasks.push_back(centroid.getmask());
        for (const FMToperatingareaclusterbinary& binary : binaries)
            {
            allmasks.push_back(binary.getmask());
            }
        return allmasks;
        }


}
#endif
