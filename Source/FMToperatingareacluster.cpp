#ifdef FMTWITHOSI
#include "FMToperatingareacluster.h"
#include <algorithm>
namespace Heuristics
{
	const FMToperatingareaclusterbinary& FMToperatingareacluster::getbinary(const Core::FMTmask& mask) const
			{
			return *std::find_if(binaries.begin(), binaries.end(), FMToperatingareacomparator(mask));
			}
}
#endif