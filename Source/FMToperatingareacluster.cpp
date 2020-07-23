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
		const std::vector<FMToperatingareaclusterbinary>& neighborsbinaries) : binaries(neighborsbinaries),centroid(localcentroid)
	{

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

}
#endif