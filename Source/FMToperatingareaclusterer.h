/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTlpheuristic.h"
#include "FMToperatingareacluster.h"
#include <vector>

#ifdef FMTWITHOSI

#ifndef FMToperatingareaclusterer_H_INCLUDED
#define FMToperatingareaclusterer_H_INCLUDED
namespace Heuristics
{
class FMToperatingareaclusterer : public FMTlpheuristic
	{
	std::vector<FMToperatingareacluster>clusters;
	double minimalarea;
	double maximalarea;
	int minimalnumberofclusters;
	int maximalnumberofclusters;
	void addmaxminobjective(const FMToperatingareacluster& cluster, const FMToperatingareaclusterbinary& binary,
		const std::vector<FMToperatingareaclusterbinary>& binaries);
	void buildclustersvariables();
	void addobjective();
	void addareaconstraints();
	public:
		FMToperatingareaclusterer() = default;
		FMToperatingareaclusterer(const std::vector<FMToperatingareacluster>& lclusters,
								const double& lminimalarea, const double& lmaximalarea,
								const int& minimalnumberofclusters,const int& maximalnumberofclusters);
		FMToperatingareaclusterer(const FMToperatingareaclusterer&) = default;
		FMToperatingareaclusterer& operator = (const FMToperatingareaclusterer&) = default;
		~FMToperatingareaclusterer() = default;

	};
}


#endif
#endif
