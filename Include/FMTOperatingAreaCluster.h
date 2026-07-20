/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI

#ifndef FMToperatingareacluster_Hm_included
#define FMToperatingareacluster_Hm_included

#include "FMTOperatingAreaClusterBinary.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <vector>

namespace Heuristics
{
    // DocString: FMTOperatingAreaCluster
    /**
    @brief Potential cluster of multiple operating areas.
    @details Each operating area is represented by a FMTOperatingAreaClusterBinary and the centroid of the cluster is also a FMTOperatingAreaClusterBinary; the cluster is active if the centroid binary is active.
    */
	class FMTEXPORT FMTOperatingAreaCluster
	{
		friend class boost::serialization::access;
		// DocString: FMTOperatingAreaCluster::serialize
		/**
		@brief Serialize the FMTOperatingAreaCluster for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & BOOST_SERIALIZATION_NVP(maxvar);
			ar & BOOST_SERIALIZATION_NVP(minvar);
			ar & BOOST_SERIALIZATION_NVP(minimalarea);
			ar & BOOST_SERIALIZATION_NVP(maximalarea);
			ar & BOOST_SERIALIZATION_NVP(binaries);
			ar & BOOST_SERIALIZATION_NVP(centroid);
		}
		// DocString: FMTOperatingAreaCluster::maxvar
        ///Linear variable representing the maximal value of the statistic present in the whole cluster, if active.
		int maxvar;
		// DocString: FMTOperatingAreaCluster::minvar
        ///Linear variable representing the minimal value of the statistic present in the whole cluster, if active.
		int minvar;
		// DocString: FMTOperatingAreaCluster::minimalarea
        ///Minimal area of the cluster,if smaller then the minimalarea the problem is infeasible.
		double minimalarea;
		// DocString: FMTOperatingAreaCluster::maximalarea
        ///Maximal area of the cluster,if greater then the maximal the problem is infeasible.
        double maximalarea;
        // DocString: FMTOperatingAreaCluster::binaries
        ///All the potential clusterbinaries that the cluster can potentialy have.
		std::vector<FMTOperatingAreaClusterBinary>binaries;
		// DocString: FMTOperatingAreaCluster::centroid
        ///Centroid binary of the cluster, it's the non aggregated Operating area in the middle of the cluster.
		FMTOperatingAreaClusterBinary centroid;
	public:
	    // DocString: FMTOperatingAreaCluster::FMTOperatingAreaCluster()
	    /**
	    @brief Default constructor for FMTOperatingAreaCluster.
	    */
		FMTOperatingAreaCluster() = default;
		// DocString: FMTOperatingAreaCluster::FMTOperatingAreaCluster(const FMTOperatingAreaClusterBinary&,const std::vector<FMTOperatingAreaClusterBinary>&)
		/**
		@brief Incomplete constructor for FMTOperatingAreaCluster from a centroid and its neighbors binaries.
		@param[in] localcentroid the centroid binary.
		@param[in] neighborsbinaries the neighbors binaries.
		*/
		FMTOperatingAreaCluster(const FMTOperatingAreaClusterBinary& localcentroid,
								const std::vector<FMTOperatingAreaClusterBinary>& neighborsbinaries);
        // DocString: FMTOperatingAreaCluster::FMTOperatingAreaCluster(const FMTOperatingAreaCluster&,const double&,const double&)
        /**
        @brief Complete constructor for FMTOperatingAreaCluster from a base cluster and its minimal and maximal area.
        @param[in] basecluster the base cluster.
        @param[in] lminimalarea the minimal area.
        @param[in] lmaximalarea the maximal area.
        */
        FMTOperatingAreaCluster(const FMTOperatingAreaCluster& basecluster,
								const double& lminimalarea,
								const double& lmaximalarea);
        // DocString: FMTOperatingAreaCluster::FMTOperatingAreaCluster(const FMTOperatingAreaCluster&)
        /**
        @brief Default copy constructor for FMTOperatingAreaCluster.
        */
		FMTOperatingAreaCluster(const FMTOperatingAreaCluster&) = default;
		// DocString: FMTOperatingAreaCluster::operator=
		/**
		@brief Default copy assignment operator for FMTOperatingAreaCluster.
		@return a reference to this FMTOperatingAreaCluster.
		*/
		FMTOperatingAreaCluster& operator = (const FMTOperatingAreaCluster&) = default;
		// DocString: FMTOperatingAreaCluster::~FMTOperatingAreaCluster()
		/**
		@brief Default destructor for FMTOperatingAreaCluster.
		*/
		~FMTOperatingAreaCluster() = default;
		// DocString: FMTOperatingAreaCluster::getBinary
		/**
		@brief Return the binary of the cluster corresponding to a given mask.
		@param[in] mask the mask.
		@return the corresponding binary.
		*/
		const FMTOperatingAreaClusterBinary& getBinary(const Core::FMTMask& mask) const;
		// DocString: FMTOperatingAreaCluster::getCentroid
		/**
		@brief Return the centroid binary of the cluster.
		@return the centroid binary.
		*/
		inline FMTOperatingAreaClusterBinary getCentroid() const
            {
            return centroid;
            }
		// DocString: FMTOperatingAreaCluster::getBinaries
		/**
		@brief Return the binaries of the cluster.
		@return the binaries.
		*/
		inline std::vector<FMTOperatingAreaClusterBinary> getBinaries() const
            {
            return binaries;
            }
		// DocString: FMTOperatingAreaCluster::getMaximalObjectiveVariable
		/**
		@brief Return the maximal objective variable.
		@return the maximal objective variable.
		*/
		inline int getMaximalObjectiveVariable() const
            {
            return maxvar;
            }
        // DocString: FMTOperatingAreaCluster::getMinimalObjectiveVariable
        /**
        @brief Return the minimal objective variable.
        @return the minimal objective variable.
        */
		inline int getMinimalObjectiveVariable() const
            {
            return minvar;
            }
        // DocString: FMTOperatingAreaCluster::getMinimalArea
        /**
        @brief Get the minimal area of the potential cluster considering the number of binaries in the cluster.
        @return the minimal area.
        */
		double getMinimalArea() const;
		// DocString: FMTOperatingAreaCluster::getMaximalArea
		/**
		@brief Get the maximal area of the potential cluster considering the number of binaries in the cluster.
		@return the maximal area.
		*/
		double getMaximalArea() const;
		// DocString: FMTOperatingAreaCluster::getMaximalStats
		/**
		@brief Get the maximal statistic for the whole cluster.
		@return the maximal statistic.
		*/
		double getMaximalStats() const;
		// DocString: FMTOperatingAreaCluster::getRealMinimalArea
		/**
		@brief Return the minimal area member of the cluster.
		@return the real minimal area.
		*/
		inline double getRealMinimalArea() const
            {
            return minimalarea;
            }
        // DocString: FMTOperatingAreaCluster::getRealMaximalArea
        /**
        @brief Return the maximal area member of the cluster.
        @return the real maximal area.
        */
		inline double getRealMaximalArea() const
            {
            return maximalarea;
            }
        // DocString: FMTOperatingAreaCluster::getTotalPotentialArea
        /**
        @brief Get the maximal potential area of the cluster.
        @return the total potential area.
        */
		double getTotalPotentialArea() const;
		// DocString: FMTOperatingAreaCluster::isValidarea
		/**
		@brief Check if an area is valid for the cluster (between the bounds).
		@param[in] area the area.
		@return true if the area is valid else false.
		*/
		bool isValidarea(const double& area) const;
		// DocString: FMTOperatingAreaCluster::setMaximalObjectiveVariable
		/**
		@brief Set the maximal objective variable of the cluster.
		@param[in] lmaxvar the maximal objective variable.
		*/
		void setMaximalObjectiveVariable(const int& lmaxvar);
		// DocString: FMTOperatingAreaCluster::setMinimalObjectiveVariable
		/**
		@brief Set the minimal objective variable of the cluster.
		@param[in] lminvar the minimal objective variable.
		*/
		void setMinimalObjectiveVariable(const int& lminvar);
		// DocString: FMTOperatingAreaCluster::setMinimalArea
		/**
		@brief Set the minimal area of the cluster.
		@param[in] newminimalarea the new minimal area.
		*/
		void setMinimalArea(const double& newminimalarea);
		// DocString: FMTOperatingAreaCluster::setMaximalArea
		/**
		@brief Set the potential maximal area of the cluster.
		@param[in] newmaximalarea the new maximal area.
		*/
		void setMaximalArea(const double& newmaximalarea);
		// DocString: FMTOperatingAreaCluster::isValidareabounds
		/**
		@brief Validate that the minimal and maximal area set by the user respect the binaries present in the cluster.
		@return true if the area bounds are valid else false.
		*/
		bool isValidareabounds() const;
		// DocString: FMTOperatingAreaCluster::getFilteredCluster
		/**
		@brief Return a cluster with the binaries not falling in a mask definition removed, which can reduce the number of binaries.
		@param[in] filterMask the filter mask.
		@return the filtered cluster.
		*/
		FMTOperatingAreaCluster getFilteredCluster(const Core::FMTMask& filterMask) const;
		// DocString: FMTOperatingAreaCluster::getAllMasks
		/**
		@brief Return all the masks contained by the cluster, including the operating area centroid.
		@return the masks.
		*/
		std::vector<Core::FMTMask>getAllMasks() const;
	};
}

BOOST_CLASS_EXPORT_KEY(Heuristics::FMTOperatingAreaCluster)
#endif
#endif
