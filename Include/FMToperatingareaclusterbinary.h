/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI

#ifndef FMToperatingareaclusterbinary_Hm_included
#define FMToperatingareaclusterbinary_Hm_included

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include "FMToperatingarea.h"


namespace Core
{
	class FMTOutput;
}

namespace Heuristics
{
// DocString: FMTOperatingAreaClusterBinary
/**
FMTOperatingAreaClusterBinary is the definition of a non clustered operating area this class is used by the FMTOperatingAreaCluster.
The class contains extra informations for an FMToperating area. Eeach clusterbinary has a binary variable id, a maximal and minimal row
statistic and finaly the statistic varlue of the binary used to calculate the objective function.
*/
	class FMTEXPORT FMTOperatingAreaClusterBinary : public FMTOperatingArea
	{
		friend class boost::serialization::access;
		// DocString: FMTOperatingAreaClusterBinary:serialize
        /**
        Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
        */
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
			{
			ar & boost::serialization::make_nvp("operatingarea", boost::serialization::base_object<FMTOperatingArea>(*this));
			ar & BOOST_SERIALIZATION_NVP(variable);
			ar & BOOST_SERIALIZATION_NVP(statistic);
			}
        // DocString: FMTOperatingAreaClusterBinary::variable
        ///Binary variable set to 1 if binary in an active cluster else 0.
		int variable;
		// DocString: FMTOperatingAreaClusterBinary::statistic
        ///Value of the statistic we want to globaly minimize the heterogenity across the landscape3.
		double statistic;
	public:
	    // DocString: FMTOperatingAreaClusterBinary::FMTOperatingAreaClusterBinary()
		/**
        FMTOperatingAreaClusterBinary default constructor.
		*/
		FMTOperatingAreaClusterBinary() = default;
		 // DocString: FMTOperatingAreaClusterBinary::FMTOperatingAreaClusterBinary(const FMTOperatingAreaClusterBinary&)
		/**
        FMTOperatingAreaClusterBinary default copy constructor.
		*/
		FMTOperatingAreaClusterBinary(const FMTOperatingAreaClusterBinary&) = default;
		// DocString: FMTOperatingAreaClusterBinary::operator=
		/**
        FMTOperatingAreaClusterBinary default copy assignment.
		*/
		FMTOperatingAreaClusterBinary& operator = (const FMTOperatingAreaClusterBinary&) = default;
		 // DocString: FMTOperatingAreaClusterBinary::~FMTOperatingAreaClusterBinary()
		/**
        FMTOperatingAreaClusterBinary default destructor.
		*/
		~FMTOperatingAreaClusterBinary() = default;
		// DocString: FMTOperatingAreaClusterBinary::FMTOperatingAreaClusterBinary(const FMTOperatingArea&)
		/**
        FMTOperatingAreaClusterBinary constructor using a base FMTOperatingArea.
		*/
		FMTOperatingAreaClusterBinary(const FMTOperatingArea& oparea);
		// DocString: FMTOperatingAreaClusterBinary::filterNeighbors
		/**
        This function takes a vector of potential binary link and filter this list by looking at the neighbors of each binary
        to make sure that the linked list are real.y neighbors.
		*/
		std::vector<FMTOperatingAreaClusterBinary> filterNeighbors(std::vector<FMTOperatingAreaClusterBinary> potentiallink) const;
		// DocString: FMTOperatingAreaClusterBinary::getOutputIntersect
		/**
        Using an (output) this function is going to intersect the mask of the output with the mask of the operating area and it will result
        of an output targeting only the operating area.
		*/
		Core::FMTOutput getOutputIntersect(const Core::FMTOutput& output,
			const std::vector<Core::FMTTheme>& themes) const;
		// DocString: FMTOperatingAreaClusterBinary::setStatistic
		/**
        Setter for the statistic of the clusterbinary that is going to be used in the global objective for minimizing heterogenity.
		*/
		void setStatistic(const double& statvalue);
		// DocString: FMTOperatingAreaClusterBinary::setStatistic
		/**
        Setter for the binary variable of the lpmodel that represent the operating area.
		*/
		void setVariable(const int& lvariable);
		// DocString: FMTOperatingAreaClusterBinary::getVariable
		/**
        Getter for the binary variable of the lpmodel that represent the operating area.
		*/
		inline const int& getVariable() const
			{
			return variable;
			}
        // DocString: FMTOperatingAreaClusterBinary::getStatistic
		/**
        Getter for the statistic value of the operating area.
		*/
		inline const double& getStatistic() const
			{
			return statistic;
			}

	};

}
BOOST_CLASS_EXPORT_KEY(Heuristics::FMTOperatingAreaClusterBinary)
#endif
#endif
