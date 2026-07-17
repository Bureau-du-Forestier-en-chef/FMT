/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTLpHeuristic.h"
#include "FMTOperatingAreaCluster.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>
#include <vector>

#ifdef FMTWITHOSI

#ifndef FMToperatingareaclusterer_Hm_included
#define FMToperatingareaclusterer_Hm_included
namespace Heuristics
{
// DocString: FMTOperatingAreaClusterer
/**
FMTOperatingAreaClusterer is made to do spatial clustering of operatingarea across a defined landscape from
a shapefile. You can define the minimal and maximal area of the resulting clusters and minimize the heteroginity
of a given statistic output of the whole clustered map. The initialSolve will attend to find a good initialsolution
then the user could try to get an optimal solution with the branchandbound().
*/
class FMTEXPORT FMTOperatingAreaClusterer : public FMTLpHeuristic
	{
    friend class boost::serialization::access;
		template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::make_nvp("FMTlpheuristic", boost::serialization::base_object<FMTLpHeuristic>(*this));
			ar & BOOST_SERIALIZATION_NVP(clusters);
			ar & BOOST_SERIALIZATION_NVP(numberofsimulationpass);

		}
    // DocString: FMTOperatingAreaClusterer::clusters
    ///Operating area clusters enumeration containing informations for the lpformulation.
	std::vector<FMTOperatingAreaCluster>clusters;
	// DocString: FMTOperatingAreaClusterer::numberofsimulationpass
    ///Number of tries made by the heuristic to find a feasible and/or a better solution.
	int numberofsimulationpass;
	// DocString: FMTOperatingAreaClusterer::minimalnumberofclusters
	///Minimal number of cluster to generate. If -1 none
	int minimalnumberofclusters;
	// DocString: FMTOperatingAreaClusterer::maximalnumberofclusters
	///Maximal number of cluster to generate. If -1 none
	int maximalnumberofclusters;
	// DocString: FMTOperatingAreaClusterer::totalarea
	///total area of the full map
	double totalarea;
        // DocString: FMTOperatingAreaClusterer::getAllBinaries
		/**
		Returns all the binary decicions contained in the clusters data member, mapped by the mask.
		*/
        std::map<Core::FMTMask,std::vector<FMTOperatingAreaClusterBinary>>getAllBinaries() const;
		// DocString: FMTOperatingAreaClusterer::getTotalArea()
		/**
		Get total area.
		*/
		double calculateTotalArea() const;
		// DocString: FMTOperatingAreaClusterer::getTotalArea
		/**
		Get total area.
		*/
		inline const double& getTotalArea() const
			{
			return totalarea;
			}
        // DocString: FMTOperatingAreaClusterer::addMaxMinObjective
		/**
		This function adds objective rows to the lpmodel for a given cluster. Called by add objective.
		*/
        void addMaxMinObjective(const FMTOperatingAreaCluster& cluster, const FMTOperatingAreaClusterBinary& binary,
            const std::vector<FMTOperatingAreaClusterBinary>& binaries,
			const int& clusterid);
        // DocString: FMTOperatingAreaClusterer::buildClustersVariables
		/**
		First function to call to construct the lp formulation used by the heuristic, the function add and map variables for
		each operating area binaries.
		*/
        void buildClustersVariables();
         // DocString: FMTOperatingAreaClusterer::addObjective
		/**
		This function build the objective functions for the whole lp formulation. It will also generates rows in the matrix.
		The objective function is based on a minimization of the heterogenity found in the cluster.
		*/
        void addObjective();
         // DocString: FMTOperatingAreaClusterer::addLinksRows
		/**
		This function adds link rows when an OParea is selected to be part of a given cluster the opareas located between the
		cluster centroid and the selected OParea has to be also part of the oparea.
		*/
        void addLinksRows();
        // DocString: FMTOperatingAreaClusterer::addLinksRows
		/**
		This function adds rows to make sure that an OParea is part of only one OParea cluster. An Oparea cannot be part of
		two Clusters at the same time we force the solver to choose between those two.
		*/
        void addForcingRows();
		// DocString: FMTOperatingAreaClusterer::addNumberOfClusterRows
		/**
		Add the minimal and maximal number of clusters.
		*/
		void addNumberOfClusterRows();
        // DocString: FMTOperatingAreaClusterer::addAreaConstraints
		/**
		This function adds minimal and maximal area constraints for each potential cluster.
		*/
        void addAreaConstraints();
       
        // DocString: FMTOperatingAreaClusterer::setAllInteger
		/**
		This functions sets the binary variable has integer, normaly used before sending the problem to the branchandbound solve.
		*/
        void setAllInteger();
        // DocString: FMTOperatingAreaClusterer::getSpreadProbability
		/**
		For a given potential (target) binarycluster and all the binarycluster present in the actual cluster (incluster), it calculates the probability
        of the fire to spread to a binary present in the cluster.
		*/
        double getSpreadProbability(const std::vector<FMTOperatingAreaClusterBinary>& incluster,const FMTOperatingAreaClusterBinary& target) const;
        // DocString: FMTOperatingAreaClusterer::spread
		/**
		For a (ignition) binary this function makes spread the fire to its neighbors and update the (assigned) binaries vector. If the generated fire patch
		is between the area bounds for the cluster it will return true else false and the problem will be ready to be optimized.
		*/
        bool spread(const FMTOperatingAreaCluster& ignition,std::vector<FMTOperatingAreaClusterBinary>& assigned);
         // DocString: FMTOperatingAreaClusterer::getTargetedOperatingAreaSize
		/**
		For a given cluster (target) this function generates a random targeted cluster size within the minimal and maximal area of the cluster.
		*/
        double getTargetedOperatingAreaSize(const FMTOperatingAreaCluster& target);
		// DocString: FMTOperatingAreaClusterer::isValidarea
		/**
		Check if the area calculated is valid.
		*/
		bool isValidarea(const FMTOperatingAreaCluster& cluster,
			const double& area, const size_t& actives) const;
         // DocString: FMTOperatingAreaClusterer::unboundAll
		/**
		This function release the bounds set by the heuristic on the binaries decision variables for the whole model.
		*/
        void unboundAll();
         // DocString: FMTOperatingAreaClusterer::getBinariesVariables
		/**
		This function returns the Lp problem variables of the cluster binaries of the whole problem.
		*/
        std::vector<int>getBinariesVariables() const;
	public:
	    // DocString: FMTOperatingAreaClusterer::FMTOperatingAreaClusterer()
		/**
		Default constructor for FMTOperatingAreaClusterer.
		*/
		FMTOperatingAreaClusterer() = default;
		// DocString: FMTOperatingAreaClusterer::FMTOperatingAreaClusterer(const Models::FMTsolverinterface&,const size_t&,const std::vector<FMTOperatingAreaCluster>&)
		/**
		Constructor for FMTOperatingAreaClusterer taking a solver (interfacetype) a seed (lseed) an a vector of (clusters) to aggregate.
		*/
		FMTOperatingAreaClusterer(const Models::FMTsolverinterface& interfacetype,
			const size_t& lseed,const std::vector<FMTOperatingAreaCluster>& lclusters,
			int minimalnumberofclusters = -1,int maximalnumberofclusters = -1);
		 // DocString: FMTOperatingAreaClusterer::FMTOperatingAreaClusterer(const FMTOperatingAreaClusterer&)
		/**
		Default copy constructor for FMTOperatingAreaClusterer.
		*/
		FMTOperatingAreaClusterer(const FMTOperatingAreaClusterer&);
		 // DocString: FMTOperatingAreaClusterer::operator =
		/**
		Default copy assignement for FMTOperatingAreaClusterer.
		*/
		FMTOperatingAreaClusterer& operator = (const FMTOperatingAreaClusterer&) = default;
		 // DocString: FMTOperatingAreaClusterer::setNumberOfSimulationPass
		/**
		Sets the number of pass that will be made in the heuristics. It's the number of try made to get a better objective and/or a feasible solution
        value during the initialSolve phase.
		*/
		void setNumberOfSimulationPass(const int& pass);
		 // DocString: FMTOperatingAreaClusterer::~FMTOperatingAreaClusterer()
		/**
		Default destructor for FMTOperatingAreaClusterer.
		*/
		~FMTOperatingAreaClusterer() = default;
		// DocString: FMTOperatingAreaClusterer::getBinariesCount
		/**
		Get number of binaries in the whole problem.
		*/
		size_t getBinariesCount() const;
		// DocString: FMTOperatingAreaClusterer::initialSolve
		/**
		Solve the heuristic problem using the original heuristic resolving the problem till finding a initial solution
		for each operating area.The fire spread heuristic will try to get a good initial clustering solution.
		*/
		bool initialSolve() final;
		// DocString: FMTOperatingAreaClusterer::branchNBoundSolve
		/**
		Solve problem using Branch and bound on the primal formulation. If the function is called after a call to initialSolve()
		it's going to use the heuristic solution has a starting MIP solution, if not it's going to directly use the BnB on the formulated problem.
		*/
		bool branchNBoundSolve() final;
		 // DocString: FMTOperatingAreaClusterer::getSolution
		/**
		Returns the clustering solution with cleaned operatingclusters, the user can then use the solution.
		*/
		std::vector<FMTOperatingAreaCluster>getSolution() const;
		// DocString: FMTOperatingAreaClusterer::buildProblem
	   /**
	   This function build the whole problem by calling in this order buildclustersvariable,addObjective,addLinksRows,addForcingRows,addAreaConstraints.
	   to generate a complete formulation for minimizing the heterogenity.
	   */
		void buildProblem();

	};
}

BOOST_CLASS_EXPORT_KEY(Heuristics::FMTOperatingAreaClusterer)


#endif
#endif
