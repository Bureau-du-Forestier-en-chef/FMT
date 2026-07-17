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
    FMTOperatingAreaCluster represent a potential cluster of multiple operating area. Each operating area is represented by a FMTOperatingAreaClusterBinary (binaries).
    The centroid of the potential cluster is also represented by a FMTOperatingAreaClusterBinary (centroid). If the centroid clusterbinary is active then the cluster
    is considered active.
    */
	class FMTEXPORT FMTOperatingAreaCluster
	{
		friend class boost::serialization::access;
		// DocString: FMTOperatingAreaCluster:serialize
        /**
        Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
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
		FMTOperatingAreaCluster default constructor.
		*/
		FMTOperatingAreaCluster() = default;
		// DocString: FMTOperatingAreaCluster::FMTOperatingAreaCluster(const FMTOperatingAreaClusterBinary&,const std::vector<FMTOperatingAreaClusterBinary>&)
		/**
		FMTOperatingAreaCluster main constructor taking the (localcentroid) and (neighborsbinaries) has arguments, it's a non complete constructor.
		*/
		FMTOperatingAreaCluster(const FMTOperatingAreaClusterBinary& localcentroid,
								const std::vector<FMTOperatingAreaClusterBinary>& neighborsbinaries);
        // DocString: FMTOperatingAreaCluster::FMTOperatingAreaCluster(const FMTOperatingAreaCluster&,const double&,const double&)
		/**
		FMTOperatingAreaCluster constructor taking an already builded FMTOperatingAreaCluster (basecluster) and an minimal area (lminimalarea) and
		a maximal area (lmaximalarea), this is a complete constructor.
		*/
        FMTOperatingAreaCluster(const FMTOperatingAreaCluster& basecluster,
								const double& lminimalarea,
								const double& lmaximalarea);
        // DocString: FMTOperatingAreaCluster::FMTOperatingAreaCluster(const FMTOperatingAreaCluster&)
		/**
		FMTOperatingAreaCluster default copy constructor.
		*/
		FMTOperatingAreaCluster(const FMTOperatingAreaCluster&) = default;
		// DocString: FMTOperatingAreaCluster::operator=
		/**
		FMTOperatingAreaCluster default copy assignment.
		*/
		FMTOperatingAreaCluster& operator = (const FMTOperatingAreaCluster&) = default;
		// DocString: FMTOperatingAreaCluster::~FMTOperatingAreaCluster()
		/**
		FMTOperatingAreaCluster default destructor.
		*/
		~FMTOperatingAreaCluster() = default;
		// DocString: FMTOperatingAreaCluster::getBinary
		/**
		Returns the corresponding binary const reference of the cluster corresponding to the given (mask).
		*/
		const FMTOperatingAreaClusterBinary& getBinary(const Core::FMTMask& mask) const;
		// DocString: FMTOperatingAreaCluster::getCentroid
		/**
		Getter for cluster centroid binary.
		*/
		inline FMTOperatingAreaClusterBinary getCentroid() const
            {
            return centroid;
            }
		// DocString: FMTOperatingAreaCluster::getBinaries
		/**
		Getter for the binaries vector member data.
		*/
		inline std::vector<FMTOperatingAreaClusterBinary> getBinaries() const
            {
            return binaries;
            }
		// DocString: FMTOperatingAreaCluster::getMaximalObjectiveVariable
		/**
		Getter for the maximal objective variable.
		*/
		inline int getMaximalObjectiveVariable() const
            {
            return maxvar;
            }
        // DocString: FMTOperatingAreaCluster::getMinimalObjectiveVariable
		/**
		Getter for the minimal objective variable.
		*/
		inline int getMinimalObjectiveVariable() const
            {
            return minvar;
            }
        // DocString: FMTOperatingAreaCluster::getMinimalArea
		/**
		Get the minimal area of the potential cluster considering the number of binaries in the cluster.
		*/
		double getMinimalArea() const;
		// DocString: FMTOperatingAreaCluster::getMaximalArea
		/**
		Get the maximal area of the potential cluster considering the number of binaries in the cluster.
		*/
		double getMaximalArea() const;
		// DocString: FMTOperatingAreaCluster::getMaximalStats
		/**
		Get maximal stats for the whole cluster.
		*/
		double getMaximalStats() const;
		// DocString: FMTOperatingAreaCluster::getRealMinimalArea
		/**
		Getter for the minimalarea.
		*/
		inline double getRealMinimalArea() const
            {
            return minimalarea;
            }
        // DocString: FMTOperatingAreaCluster::getRealMaximalArea
		/**
		Getter for the maximalarea.
		*/
		inline double getRealMaximalArea() const
            {
            return maximalarea;
            }
        // DocString: FMTOperatingAreaCluster::getTotalPotentialArea
		/**
		Get the maximal potential area of the cluster.
		*/
		double getTotalPotentialArea() const;
		// DocString: FMTOperatingAreaCluster::isValidarea
		/**
		Check if the (area) is a valid area for the cluster (between bounds).
		*/
		bool isValidarea(const double& area) const;
		// DocString: FMTOperatingAreaCluster::setMaximalObjectiveVariable
		/**
		Set the maximal objective variable of the cluster.
		*/
		void setMaximalObjectiveVariable(const int& lmaxvar);
		// DocString: FMTOperatingAreaCluster::setMinimalObjectiveVariable
		/**
		Set the minimal objective variable of the cluster.
		*/
		void setMinimalObjectiveVariable(const int& lminvar);
		// DocString: FMTOperatingAreaCluster::setMinimalArea
		/**
		Set the minimal area of the cluster.
		*/
		void setMinimalArea(const double& newminimalarea);
		// DocString: FMTOperatingAreaCluster::setMaximalArea
		/**
		Set the potential maximal area of the cluster.
		*/
		void setMaximalArea(const double& newmaximalarea);
		// DocString: FMTOperatingAreaCluster::isValidareabounds
		/**
		Validate is the minimal and maximal area used by the user respect the binaries present in the cluster.
		*/
		bool isValidareabounds() const;
		// DocString: FMTOperatingAreaCluster::getFilteredCluster
		/**
		Base on a given mask the function is going to remove any binary that does not fell in the mask definition.
		It can reduce the size of the binaries.
		*/
		FMTOperatingAreaCluster getFilteredCluster(const Core::FMTMask& filterMask) const;
		// DocString: FMTOperatingAreaCluster::getAllMasks
		/**
		Returns all the mask contained by the cluster including the operating area centroid.
		*/
		std::vector<Core::FMTMask>getAllMasks() const;
	};
}

BOOST_CLASS_EXPORT_KEY(Heuristics::FMTOperatingAreaCluster)
#endif
#endif
