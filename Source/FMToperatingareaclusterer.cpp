#include "FMToperatingareaclusterer.h"


namespace Heuristics
{
	FMToperatingareacluster::FMToperatingareacluster(const FMToperatingareaclusterbinary& localcentroid,
		const std::vector<FMToperatingareaclusterbinary>& neighborsbinaries):
		binaries(neighborsbinaries), centroid(localcentroid)
	{

	}

}