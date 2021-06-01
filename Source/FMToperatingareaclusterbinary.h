/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI

#ifndef FMToperatingareaclusterbinary_H_INCLUDED
#define FMToperatingareaclusterbinary_H_INCLUDED

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include "FMToperatingarea.h"
#include "FMToutput.h"

namespace Heuristics
{
// DocString: FMToperatingareaclusterbinary
/**
FMToperatingareaclusterbinary is the definition of a non clustered operating area this class is used by the FMToperatingareacluster.
The class contains extra informations for an FMToperating area. Eeach clusterbinary has a binary variable id, a maximal and minimal row
statistic and finaly the statistic varlue of the binary used to calculate the objective function.
*/
	class FMTEXPORT FMToperatingareaclusterbinary : public FMToperatingarea
	{
		friend class boost::serialization::access;
		// DocString: FMToperatingareaclusterbinary:serialize
        /**
        Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
        */
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
			{
			ar & boost::serialization::make_nvp("operatingarea", boost::serialization::base_object<FMToperatingarea>(*this));
			ar & BOOST_SERIALIZATION_NVP(variable);
			ar & BOOST_SERIALIZATION_NVP(statistic);
			}
        // DocString: FMToperatingareaclusterbinary::variable
        ///Binary variable set to 1 if binary in an active cluster else 0.
		int variable;
		// DocString: FMToperatingareaclusterbinary::statistic
        ///Value of the statistic we want to globaly minimize the heterogenity across the landscape3.
		double statistic;
	public:
	    // DocString: FMToperatingareaclusterbinary::FMToperatingareaclusterbinary()
		/**
        FMToperatingareaclusterbinary default constructor.
		*/
		FMToperatingareaclusterbinary() = default;
		 // DocString: FMToperatingareaclusterbinary::FMToperatingareaclusterbinary(const FMToperatingareaclusterbinary&)
		/**
        FMToperatingareaclusterbinary default copy constructor.
		*/
		FMToperatingareaclusterbinary(const FMToperatingareaclusterbinary&) = default;
		// DocString: FMToperatingareaclusterbinary::operator=
		/**
        FMToperatingareaclusterbinary default copy assignment.
		*/
		FMToperatingareaclusterbinary& operator = (const FMToperatingareaclusterbinary&) = default;
		 // DocString: FMToperatingareaclusterbinary::~FMToperatingareaclusterbinary()
		/**
        FMToperatingareaclusterbinary default destructor.
		*/
		~FMToperatingareaclusterbinary() = default;
		// DocString: FMToperatingareaclusterbinary::FMToperatingareaclusterbinary(const FMToperatingarea&)
		/**
        FMToperatingareaclusterbinary constructor using a base FMToperatingarea.
		*/
		FMToperatingareaclusterbinary(const FMToperatingarea& oparea);
		// DocString: FMToperatingareaclusterbinary::filterneighbors
		/**
        This function takes a vector of potential binary link and filter this list by looking at the neighbors of each binary
        to make sure that the linked list are real.y neighbors.
		*/
		std::vector<FMToperatingareaclusterbinary> filterneighbors(std::vector<FMToperatingareaclusterbinary> potentiallink) const;
		// DocString: FMToperatingareaclusterbinary::getoutputintersect
		/**
        Using an (output) this function is going to intersect the mask of the output with the mask of the operating area and it will result
        of an output targeting only the operating area.
		*/
		Core::FMToutput getoutputintersect(const Core::FMToutput& output,
			const std::vector<Core::FMTtheme>& themes) const;
		// DocString: FMToperatingareaclusterbinary::setstatistic
		/**
        Setter for the statistic of the clusterbinary that is going to be used in the global objective for minimizing heterogenity.
		*/
		void setstatistic(const double& statvalue);
		// DocString: FMToperatingareaclusterbinary::setstatistic
		/**
        Setter for the binary variable of the lpmodel that represent the operating area.
		*/
		void setvariable(const int& lvariable);
		// DocString: FMToperatingareaclusterbinary::getvariable
		/**
        Getter for the binary variable of the lpmodel that represent the operating area.
		*/
		inline const int& getvariable() const
			{
			return variable;
			}
        // DocString: FMToperatingareaclusterbinary::getstatistic
		/**
        Getter for the statistic value of the operating area.
		*/
		inline const double& getstatistic() const
			{
			return statistic;
			}

	};

}
BOOST_CLASS_EXPORT_KEY(Heuristics::FMToperatingareaclusterbinary)
#endif
#endif
