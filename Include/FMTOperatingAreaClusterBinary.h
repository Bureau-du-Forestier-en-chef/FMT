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
#include "FMTOperatingArea.h"


namespace Core
{
	class FMTOutput;
}

namespace Heuristics
{
// DocString: FMTOperatingAreaClusterBinary
/**
@brief Definition of a non clustered operating area, used by the FMTOperatingAreaCluster.
@details Each cluster binary has a binary variable id, a maximal and minimal row statistic and the statistic value of the binary used to compute the objective function.
*/
	class FMTEXPORT FMTOperatingAreaClusterBinary : public FMTOperatingArea
	{
		friend class boost::serialization::access;
		// DocString: FMTOperatingAreaClusterBinary::serialize
		/**
		@brief Serialize the FMTOperatingAreaClusterBinary for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
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
	    @brief Default constructor for FMTOperatingAreaClusterBinary.
	    */
		FMTOperatingAreaClusterBinary() = default;
		 // DocString: FMTOperatingAreaClusterBinary::FMTOperatingAreaClusterBinary(const FMTOperatingAreaClusterBinary&)
		 /**
		 @brief Default copy constructor for FMTOperatingAreaClusterBinary.
		 */
		FMTOperatingAreaClusterBinary(const FMTOperatingAreaClusterBinary&) = default;
		// DocString: FMTOperatingAreaClusterBinary::operator=
		/**
		@brief Default copy assignment operator for FMTOperatingAreaClusterBinary.
		@return a reference to this FMTOperatingAreaClusterBinary.
		*/
		FMTOperatingAreaClusterBinary& operator = (const FMTOperatingAreaClusterBinary&) = default;
		 // DocString: FMTOperatingAreaClusterBinary::~FMTOperatingAreaClusterBinary()
		 /**
		 @brief Default destructor for FMTOperatingAreaClusterBinary.
		 */
		~FMTOperatingAreaClusterBinary() = default;
		// DocString: FMTOperatingAreaClusterBinary::FMTOperatingAreaClusterBinary(const FMTOperatingArea&)
		/**
		@brief Construct a FMTOperatingAreaClusterBinary from a base operating area.
		@param[in] oparea the operating area.
		*/
		FMTOperatingAreaClusterBinary(const FMTOperatingArea& oparea);
		// DocString: FMTOperatingAreaClusterBinary::filterNeighbors
		/**
		@brief Filter a list of potential binary links by looking at the neighbors of each binary to keep only the real neighbors.
		@param[in] potentiallink the potential binary links.
		@return the filtered neighbors.
		*/
		std::vector<FMTOperatingAreaClusterBinary> filterNeighbors(std::vector<FMTOperatingAreaClusterBinary> potentiallink) const;
		// DocString: FMTOperatingAreaClusterBinary::getOutputIntersect
		/**
		@brief Intersect the mask of an output with the mask of the operating area to get an output targeting only the operating area.
		@param[in] output the output.
		@param[in] themes the themes.
		@return the intersected output.
		*/
		Core::FMTOutput getOutputIntersect(const Core::FMTOutput& output,
			const std::vector<Core::FMTTheme>& themes) const;
		// DocString: FMTOperatingAreaClusterBinary::setStatistic
		/**
		@brief Set the statistic of the cluster binary used in the global objective to minimize heterogeneity.
		@param[in] statvalue the statistic value.
		*/
		void setStatistic(const double& statvalue);
		// DocString: FMTOperatingAreaClusterBinary::setVariable
		/**
		@brief Set the binary variable of the LP model representing the operating area.
		@param[in] lvariable the binary variable.
		*/
		void setVariable(const int& lvariable);
		// DocString: FMTOperatingAreaClusterBinary::getVariable
		/**
		@brief Return the binary variable of the LP model representing the operating area.
		@return the binary variable.
		*/
		inline const int& getVariable() const
			{
			return variable;
			}
        // DocString: FMTOperatingAreaClusterBinary::getStatistic
        /**
        @brief Return the statistic value of the operating area.
        @return the statistic value.
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
