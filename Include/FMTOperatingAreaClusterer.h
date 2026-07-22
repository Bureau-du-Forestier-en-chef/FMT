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
@brief Heuristic to do spatial clustering of operating areas across a landscape from a shapefile.
@details The user can define the minimal and maximal area of the resulting clusters and minimize the heterogeneity of a statistic output over the clustered map. initialSolve finds a good initial solution and branchNBoundSolve tries to get an optimal one.
*/
class FMTEXPORT FMTOperatingAreaClusterer : public FMTLpHeuristic
	{
    friend class boost::serialization::access;
		// DocString: FMTOperatingAreaClusterer::serialize
		/**
		@brief Serialize the FMTOperatingAreaClusterer for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::make_nvp("FMTlpheuristic", boost::serialization::base_object<FMTLpHeuristic>(*this));
			ar & BOOST_SERIALIZATION_NVP(clusters);
			ar & BOOST_SERIALIZATION_NVP(numberofsimulationpass);

		}
    // DocString: FMTOperatingAreaClusterer::clusters
    /**
    @brief Operating area clusters enumeration containing the information for the LP formulation.
    */
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
        @brief Return all the binary decisions contained in the clusters, mapped by mask.
        @return the binaries mapped by mask.
        */
        std::map<Core::FMTMask,std::vector<FMTOperatingAreaClusterBinary>>getAllBinaries() const;
		// DocString: FMTOperatingAreaClusterer::getTotalArea()
		/**
		@brief Compute the total area of the full map.
		@return the total area.
		*/
		double calculateTotalArea() const;
		// DocString: FMTOperatingAreaClusterer::getTotalArea
		/**
		@brief Return the total area of the full map.
		@return the total area.
		*/
		inline const double& getTotalArea() const
			{
			return totalarea;
			}
        // DocString: FMTOperatingAreaClusterer::addMaxMinObjective
        /**
        @brief Add the objective rows to the LP model for a given cluster.
        @param[in] cluster the cluster.
        @param[in] binary the cluster binary.
        @param[in] binaries the cluster binaries.
        @param[in] clusterid the cluster id.
        */
        void addMaxMinObjective(const FMTOperatingAreaCluster& cluster, const FMTOperatingAreaClusterBinary& binary,
            const std::vector<FMTOperatingAreaClusterBinary>& binaries,
			const int& clusterid);
        // DocString: FMTOperatingAreaClusterer::buildClustersVariables
        /**
        @brief Add and map the variables for each operating area binary, the first step of the LP formulation.
        */
        void buildClustersVariables();
         // DocString: FMTOperatingAreaClusterer::addObjective
         /**
         @brief Build the objective function of the LP formulation, generating rows in the matrix, based on minimizing the heterogeneity found in the cluster.
         */
        void addObjective();
         // DocString: FMTOperatingAreaClusterer::addLinksRows
         /**
         @brief Add link rows so that the operating areas located between the cluster centroid and a selected operating area are also part of the cluster.
         */
        void addLinksRows();
        // DocString: FMTOperatingAreaClusterer::addForcingRows
        /**
        @brief Add rows so that an operating area is part of only one cluster, forcing the solver to choose between clusters.
        */
        void addForcingRows();
		// DocString: FMTOperatingAreaClusterer::addNumberOfClusterRows
		/**
		@brief Add the minimal and maximal number of clusters constraints.
		*/
		void addNumberOfClusterRows();
        // DocString: FMTOperatingAreaClusterer::addAreaConstraints
        /**
        @brief Add the minimal and maximal area constraints for each potential cluster.
        */
        void addAreaConstraints();
       
        // DocString: FMTOperatingAreaClusterer::setAllInteger
        /**
        @brief Set the binary variables as integer, normally used before the branch and bound solve.
        */
        void setAllInteger();
        // DocString: FMTOperatingAreaClusterer::getSpreadProbability
        /**
        @brief Compute the probability of the fire to spread to a target binary from the binaries present in the cluster.
        @param[in] incluster the binaries present in the cluster.
        @param[in] target the target binary.
        @return the spread probability.
        */
        double getSpreadProbability(const std::vector<FMTOperatingAreaClusterBinary>& incluster,const FMTOperatingAreaClusterBinary& target) const;
        // DocString: FMTOperatingAreaClusterer::spread
        /**
        @brief Spread the fire from an ignition cluster to its neighbors and update the assigned binaries.
        @details Returns true if the generated fire patch is between the area bounds of the cluster.
        @param[in] ignition the ignition cluster.
        @param[in,out] assigned the assigned binaries.
        @return true if the patch is within the area bounds else false.
        */
        bool spread(const FMTOperatingAreaCluster& ignition,std::vector<FMTOperatingAreaClusterBinary>& assigned);
         // DocString: FMTOperatingAreaClusterer::getTargetedOperatingAreaSize
         /**
         @brief Generate a random targeted cluster size within the minimal and maximal area of a cluster.
         @param[in] target the target cluster.
         @return the targeted operating area size.
         */
        double getTargetedOperatingAreaSize(const FMTOperatingAreaCluster& target);
		// DocString: FMTOperatingAreaClusterer::isValidarea
		/**
		@brief Check if a computed area is valid for a cluster.
		@param[in] cluster the cluster.
		@param[in] area the area.
		@param[in] actives the number of active binaries.
		@return true if the area is valid else false.
		*/
		bool isValidarea(const FMTOperatingAreaCluster& cluster,
			const double& area, const size_t& actives) const;
         // DocString: FMTOperatingAreaClusterer::unboundAll
         /**
         @brief Release the bounds set by the heuristic on the binary decision variables for the whole model.
         */
        void unboundAll();
         // DocString: FMTOperatingAreaClusterer::getBinariesVariables
         /**
         @brief Return the LP problem variables of the cluster binaries of the whole problem.
         @return the binaries variables.
         */
        std::vector<int>getBinariesVariables() const;
	public:
	    // DocString: FMTOperatingAreaClusterer::FMTOperatingAreaClusterer()
	    /**
	    @brief Default constructor for FMTOperatingAreaClusterer.
	    */
		FMTOperatingAreaClusterer() = default;
		// DocString: FMTOperatingAreaClusterer::FMTOperatingAreaClusterer(const Models::FMTsolverinterface&,const size_t&,const std::vector<FMTOperatingAreaCluster>&)
		/**
		@brief Construct a FMTOperatingAreaClusterer from a solver interface, a seed and a vector of clusters to aggregate.
		@param[in] interfacetype the solver interface type.
		@param[in] lseed the seed.
		@param[in] lclusters the clusters.
		@param[in] minimalnumberofclusters the minimal number of clusters.
		@param[in] maximalnumberofclusters the maximal number of clusters.
		*/
		FMTOperatingAreaClusterer(const Models::FMTsolverinterface& interfacetype,
			const size_t& lseed,const std::vector<FMTOperatingAreaCluster>& lclusters,
			int minimalnumberofclusters = -1,int maximalnumberofclusters = -1);
		 // DocString: FMTOperatingAreaClusterer::FMTOperatingAreaClusterer(const FMTOperatingAreaClusterer&)
		 /**
		 @brief Default copy constructor for FMTOperatingAreaClusterer.
		 */
		FMTOperatingAreaClusterer(const FMTOperatingAreaClusterer&);
		 // DocString: FMTOperatingAreaClusterer::operator =
		 /**
		 @brief Default copy assignment operator for FMTOperatingAreaClusterer.
		 @return a reference to this FMTOperatingAreaClusterer.
		 */
		FMTOperatingAreaClusterer& operator = (const FMTOperatingAreaClusterer&) = default;
		 // DocString: FMTOperatingAreaClusterer::setNumberOfSimulationPass
		 /**
		 @brief Set the number of passes made by the heuristic to get a better objective or a feasible solution during initialSolve.
		 @param[in] pass the number of passes.
		 */
		void setNumberOfSimulationPass(const int& pass);
		 // DocString: FMTOperatingAreaClusterer::~FMTOperatingAreaClusterer()
		 /**
		 @brief Default destructor for FMTOperatingAreaClusterer.
		 */
		~FMTOperatingAreaClusterer() = default;
		// DocString: FMTOperatingAreaClusterer::getBinariesCount
		/**
		@brief Get the number of binaries in the whole problem.
		@return the number of binaries.
		*/
		size_t getBinariesCount() const;
		// DocString: FMTOperatingAreaClusterer::initialSolve
		/**
		@brief Solve the heuristic problem using the fire spread heuristic to get a good initial clustering solution for each operating area.
		@return true if an initial solution is found else false.
		*/
		bool initialSolve() final;
		// DocString: FMTOperatingAreaClusterer::branchNBoundSolve
		/**
		@brief Solve the problem using branch and bound on the primal formulation, using the heuristic solution as a starting MIP solution when initialSolve has been called.
		@return true if a solution is found else false.
		*/
		bool branchNBoundSolve() final;
		 // DocString: FMTOperatingAreaClusterer::getSolution
		 /**
		 @brief Return the clustering solution with cleaned operating clusters.
		 @return the clustering solution.
		 */
		std::vector<FMTOperatingAreaCluster>getSolution() const;
		// DocString: FMTOperatingAreaClusterer::buildProblem
		/**
		@brief Build the whole problem by calling buildClustersVariables, addObjective, addLinksRows, addForcingRows and addAreaConstraints to minimize the heterogeneity.
		*/
		void buildProblem();

	};
}

BOOST_CLASS_EXPORT_KEY(Heuristics::FMTOperatingAreaClusterer)


#endif
#endif
