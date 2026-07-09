/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI

#ifndef FMToperatingareacluster_Hm_included
#define FMToperatingareacluster_Hm_included

#include "FMToperatingareaclusterbinary.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <vector>

namespace Heuristics
{
    // DocString: FMToperatingareacluster
    /**
    FMToperatingareacluster represent a potential cluster of multiple operating area. Each operating area is represented by a FMToperatingareaclusterbinary (binaries).
    The centroid of the potential cluster is also represented by a FMToperatingareaclusterbinary (centroid). If the centroid clusterbinary is active then the cluster
    is considered active.
    */
	class FMTEXPORT FMToperatingareacluster
	{
		friend class boost::serialization::access;
		// DocString: FMToperatingareacluster:serialize
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
		// DocString: FMToperatingareacluster::maxvar
        ///Linear variable representing the maximal value of the statistic present in the whole cluster, if active.
		int maxvar;
		// DocString: FMToperatingareacluster::minvar
        ///Linear variable representing the minimal value of the statistic present in the whole cluster, if active.
		int minvar;
		// DocString: FMToperatingareacluster::minimalarea
        ///Minimal area of the cluster,if smaller then the minimalarea the problem is infeasible.
		double minimalarea;
		// DocString: FMToperatingareacluster::maximalarea
        ///Maximal area of the cluster,if greater then the maximal the problem is infeasible.
        double maximalarea;
        // DocString: FMToperatingareacluster::binaries
        ///All the potential clusterbinaries that the cluster can potentialy have.
		std::vector<FMToperatingareaclusterbinary>binaries;
		// DocString: FMToperatingareacluster::centroid
        ///Centroid binary of the cluster, it's the non aggregated Operating area in the middle of the cluster.
		FMToperatingareaclusterbinary centroid;
	public:
	    // DocString: FMToperatingareacluster::FMToperatingareacluster()
		/**
		FMToperatingareacluster default constructor.
		*/
		FMToperatingareacluster() = default;
		// DocString: FMToperatingareacluster::FMToperatingareacluster(const FMToperatingareaclusterbinary&,const std::vector<FMToperatingareaclusterbinary>&)
		/**
		FMToperatingareacluster main constructor taking the (localcentroid) and (neighborsbinaries) has arguments, it's a non complete constructor.
		*/
		FMToperatingareacluster(const FMToperatingareaclusterbinary& localcentroid,
								const std::vector<FMToperatingareaclusterbinary>& neighborsbinaries);
        // DocString: FMToperatingareacluster::FMToperatingareacluster(const FMToperatingareacluster&,const double&,const double&)
		/**
		FMToperatingareacluster constructor taking an already builded FMToperatingareacluster (basecluster) and an minimal area (lminimalarea) and
		a maximal area (lmaximalarea), this is a complete constructor.
		*/
        FMToperatingareacluster(const FMToperatingareacluster& basecluster,
								const double& lminimalarea,
								const double& lmaximalarea);
        // DocString: FMToperatingareacluster::FMToperatingareacluster(const FMToperatingareacluster&)
		/**
		FMToperatingareacluster default copy constructor.
		*/
		FMToperatingareacluster(const FMToperatingareacluster&) = default;
		// DocString: FMToperatingareacluster::operator=
		/**
		FMToperatingareacluster default copy assignment.
		*/
		FMToperatingareacluster& operator = (const FMToperatingareacluster&) = default;
		// DocString: FMToperatingareacluster::~FMToperatingareacluster()
		/**
		FMToperatingareacluster default destructor.
		*/
		~FMToperatingareacluster() = default;
		// DocString: FMToperatingareacluster::getBinary
		/**
		Returns the corresponding binary const reference of the cluster corresponding to the given (mask).
		*/
		const FMToperatingareaclusterbinary& getBinary(const Core::FMTmask& mask) const;
		// DocString: FMToperatingareacluster::getCentroid
		/**
		Getter for cluster centroid binary.
		*/
		inline FMToperatingareaclusterbinary getCentroid() const
            {
            return centroid;
            }
		// DocString: FMToperatingareacluster::getBinaries
		/**
		Getter for the binaries vector member data.
		*/
		inline std::vector<FMToperatingareaclusterbinary> getBinaries() const
            {
            return binaries;
            }
		// DocString: FMToperatingareacluster::getMaximalObjectiveVariable
		/**
		Getter for the maximal objective variable.
		*/
		inline int getMaximalObjectiveVariable() const
            {
            return maxvar;
            }
        // DocString: FMToperatingareacluster::getMinimalObjectiveVariable
		/**
		Getter for the minimal objective variable.
		*/
		inline int getMinimalObjectiveVariable() const
            {
            return minvar;
            }
        // DocString: FMToperatingareacluster::getMinimalArea
		/**
		Get the minimal area of the potential cluster considering the number of binaries in the cluster.
		*/
		double getMinimalArea() const;
		// DocString: FMToperatingareacluster::getMaximalArea
		/**
		Get the maximal area of the potential cluster considering the number of binaries in the cluster.
		*/
		double getMaximalArea() const;
		// DocString: FMToperatingareacluster::getMaximalStats
		/**
		Get maximal stats for the whole cluster.
		*/
		double getMaximalStats() const;
		// DocString: FMToperatingareacluster::getRealMinimalArea
		/**
		Getter for the minimalarea.
		*/
		inline double getRealMinimalArea() const
            {
            return minimalarea;
            }
        // DocString: FMToperatingareacluster::getRealMaximalArea
		/**
		Getter for the maximalarea.
		*/
		inline double getRealMaximalArea() const
            {
            return maximalarea;
            }
        // DocString: FMToperatingareacluster::getTotalPotentialArea
		/**
		Get the maximal potential area of the cluster.
		*/
		double getTotalPotentialArea() const;
		// DocString: FMToperatingareacluster::isValidarea
		/**
		Check if the (area) is a valid area for the cluster (between bounds).
		*/
		bool isValidarea(const double& area) const;
		// DocString: FMToperatingareacluster::setMaximalObjectiveVariable
		/**
		Set the maximal objective variable of the cluster.
		*/
		void setMaximalObjectiveVariable(const int& lmaxvar);
		// DocString: FMToperatingareacluster::setMinimalObjectiveVariable
		/**
		Set the minimal objective variable of the cluster.
		*/
		void setMinimalObjectiveVariable(const int& lminvar);
		// DocString: FMToperatingareacluster::setMinimalArea
		/**
		Set the minimal area of the cluster.
		*/
		void setMinimalArea(const double& newminimalarea);
		// DocString: FMToperatingareacluster::setMaximalArea
		/**
		Set the potential maximal area of the cluster.
		*/
		void setMaximalArea(const double& newmaximalarea);
		// DocString: FMToperatingareacluster::isValidareabounds
		/**
		Validate is the minimal and maximal area used by the user respect the binaries present in the cluster.
		*/
		bool isValidareabounds() const;
		// DocString: FMToperatingareacluster::getFilteredCluster
		/**
		Base on a given mask the function is going to remove any binary that does not fell in the mask definition.
		It can reduce the size of the binaries.
		*/
		FMToperatingareacluster getFilteredCluster(const Core::FMTmask& filterMask) const;
		// DocString: FMToperatingareacluster::getAllMasks
		/**
		Returns all the mask contained by the cluster including the operating area centroid.
		*/
		std::vector<Core::FMTmask>getAllMasks() const;
	};
}

BOOST_CLASS_EXPORT_KEY(Heuristics::FMToperatingareacluster)
#endif
#endif
