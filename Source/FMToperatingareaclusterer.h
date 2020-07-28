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
	int numberofsimulationpass;
	std::map<Core::FMTmask,std::vector<FMToperatingareaclusterbinary>>getallbinaries() const;
	void addmaxminobjective(const FMToperatingareacluster& cluster, const FMToperatingareaclusterbinary& binary,
		const std::vector<FMToperatingareaclusterbinary>& binaries);
	void buildclustersvariables();
	void addobjective();
	void addlinksrows();
	void addforcingrows();
	void addareaconstraints();
	void buildproblem();
	void setallinteger();
	double getspreadprobability(const std::vector<FMToperatingareaclusterbinary>& incluster,const FMToperatingareaclusterbinary& target) const;
	bool spread(const FMToperatingareacluster& ignition,std::vector<FMToperatingareaclusterbinary>& assigned);
	double gettargetedoperatingareasize(const FMToperatingareacluster& target);
	void simulatespread();
	void unboundall();
	std::vector<int>getbinariesvariables() const;
	public:
		FMToperatingareaclusterer() = default;
		FMToperatingareaclusterer(const Models::FMTsolverinterface& interfacetype,const size_t& lseed,const std::vector<FMToperatingareacluster>& lclusters);
		FMToperatingareaclusterer(const FMToperatingareaclusterer&) = default;
		FMToperatingareaclusterer& operator = (const FMToperatingareaclusterer&) = default;
		void setnumberofsimulationpass(const int& pass);
		~FMToperatingareaclusterer() = default;
		bool initialsolve() final;
		// DocString: FMToperatingareaclusterer::branchnboundsolve
		/**
		Solve problem using Branch and bound on the primal formulation. If the function is called after a call to initialsolve()
		it's going to use the heuristic solution has a starting MIP solution, if not it's going to directly use the BnB on the formulated problem.
		*/
		void branchnboundsolve() final;
		std::vector<FMToperatingareacluster>getsolution() const;

	};
}


#endif
#endif
