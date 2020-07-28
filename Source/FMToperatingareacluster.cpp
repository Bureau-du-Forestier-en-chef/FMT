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

	FMToperatingareaclusterbinary FMToperatingareacluster::getcentroid() const
			{
			return centroid;
			}
	std::vector<FMToperatingareaclusterbinary> FMToperatingareacluster::getbinaries() const
			{
			return binaries;
			}

	FMToperatingareacluster::FMToperatingareacluster(const FMToperatingareaclusterbinary& localcentroid,
		const std::vector<FMToperatingareaclusterbinary>& neighborsbinaries,
		const double& lminimalarea,
        const double& lmaximalarea) : binaries(neighborsbinaries),centroid(localcentroid),
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

    double FMToperatingareacluster::getrealminimalarea() const
        {
        return minimalarea;
        }

    double FMToperatingareacluster::getrealmaximalarea() const
		{
        return maximalarea;
		}

    bool FMToperatingareacluster::isvalidarea(const double& area) const
        {
        return (area >= getminimalarea() && area <= getmaximalarea());
        }

	int FMToperatingareacluster::getmaximalobjectivevariable() const
		{
		return maxvar;
		}
	int FMToperatingareacluster::getminimalobjectivevariable() const
		{
		return minvar;
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


}
#endif
