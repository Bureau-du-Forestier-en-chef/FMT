/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI

#ifndef FMToperatingarea_H_INCLUDED
#define FMToperatingarea_H_INCLUDED

#include "FMTgraphdescription.h"
#include "FMTgraph.h"
#include "FMTmask.h"
#include "OsiSolverInterface.hpp"
#include <vector>
#include <map>
#include <memory>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include "FMTlpsolver.h"

namespace Heuristics
	{
	// DocString: FMToperatingarea
	/**
	FMToperatingarea gives informations about a given operating area it's mask,neighbors,neighborsperimeter.
	FMToperating area have to be based on static themes (not used within transitions).
	*/
	class FMTEXPORT FMToperatingarea
		{
		// DocString: FMToperatingarea::serialize
		/**
		serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
			{
			ar & BOOST_SERIALIZATION_NVP(mask);
			ar & BOOST_SERIALIZATION_NVP(neighbors);
			ar & BOOST_SERIALIZATION_NVP(neihgborsperimeter);
			ar & BOOST_SERIALIZATION_NVP(_area);
			}
		protected:
		// DocString: FMToperatingarea::mask
		///The mask describing the operating area
		Core::FMTmask mask;
		// DocString: FMToperatingarea::neighbors
		///Neighbors mask of the operating area
		std::vector<Core::FMTmask>neighbors;
		// DocString: FMToperatingarea::neihgborsperimeter
		///neighborsperimeter is the ratio a operatingarea needs to share to a other operatingarea to be considered neighbor.
		double neihgborsperimeter;
		// DocString: FMToperatingarea::_area
		///The initial area of the operating area used as big M for the MIP.
		double _area;
		public:
			// DocString: FMToperatingarea::operator+=
			/**
			Add area to the operating area.
			*/
			FMToperatingarea& operator += (const double& value);
			// DocString: FMToperatingarea::getneighbors
			/**
			Getter returning a copy of the neighbors masks data of the operatingarea.
			*/
			std::vector<Core::FMTmask> getneighbors() const;
			// DocString: FMToperatingarea::getarea
			/**
				Get the area of the operating area.
			*/
			double getarea() const;
			// DocString: FMToperatingarea::getneihgborsperimeter
			/**
			Getter returning the percentage of perimeter the oprating area needs to share with a other operating area,
			to be considered neighbor.
			*/
			double getneihgborsperimeter() const;
			// DocString: FMToperatingarea::getmask
			/**
			Getter returning mask of the operating area.
			*/
			Core::FMTmask getmask() const;
			// DocString: FMToperatingarea::setneighbors
			/**
			Setter for the neighbors mask member data.
			*/
			void setneighbors(const std::vector<Core::FMTmask>& lneighbors);
			// DocString: FMToperatingarea(const Core::FMTmask,const double)
			/**
			Main FMToperatingarea constructor targeting the user. Before synchronizing everything to the solverinterface,
			the user has to provide to the heuristics all the green-up, returntime etc.... for each operating area.
			*/
			FMToperatingarea(const Core::FMTmask& lmask,const double& lneihgborsperimeter);
			// DocString: FMToperatingarea::setarea
			/**
			Set the area of the operating area.
			*/
			void setarea(const double& newarea);

			// DocString: FMToperatingarea()
			/**
			Default FMToperatingarea constructor
			*/
			FMToperatingarea()=default;
			// DocString: FMToperatingarea(const FMToperatingarea)
			/**
			FMToperatingarea copy constructor
			*/
			FMToperatingarea(const FMToperatingarea& rhs) = default;
			// DocString: FMToperatingarea::operator=
			/**
			FMToperatingarea copy assignment
			*/
			FMToperatingarea& operator = (const FMToperatingarea& rhs)=default;
			// DocString: FMToperatingarea::operator==
			/**
			Comparison operator of FMToperatingarea
			*/
			bool operator == (const FMToperatingarea& rhs) const;
			// DocString: FMToperatingarea::operator!=
			/**
			Comparison operator of FMToperatingarea
			*/
			bool operator != (const FMToperatingarea& rhs) const;
			// DocString: ~FMToperatingarea()
			/**
			Default FMToperatingarea destructor
			*/
			virtual ~FMToperatingarea()=default;
			// DocString: FMToperatingarea::presolve
			/**
			Using a FMTmask (selectedmask) and a subset of the original FMTthemes used to construct the FMTmask,
			it returns a presolved FMTmask with potentialy less data.
			*/
			FMToperatingarea presolveoperatingarea(const Core::FMTmask& selectedmask,const std::vector<Core::FMTtheme>&presolvedthemes) const;
			// DocString: FMToperatingarea::postsolve
			/**
			Using a FMTmask (selectedmask) and the original FMTthemes it returns a postsolved FMTmask.
			*/
			FMToperatingarea postsolveoperatingarea(const Core::FMTmask& selectedmask, const std::vector<Core::FMTtheme>&basethemes) const;

		};

	// DocString: FMToperatingareacomparator
	/**
	This class is made to compare FMToperatingarea using the std::find_if() function when FMToperatingarea are in a stl container.
	*/
	class FMToperatingareacomparator
		{
			// DocString: FMToperatingareacomparator::mask
			///FMTmask of the operating area we wish to find.
			Core::FMTmask mask;
		public:
			// DocString: FMToperatingareacomparator(const Core::FMTmask)
			/**
			Constructor of the comparator using the FMTmask as reference.
			*/
			FMToperatingareacomparator(const Core::FMTmask& lmask);
			// DocString: FMToperatingareacomparator(const FMToperatingarea& oparea)
			/**
			Constructor of the comparator using the operating area.
			*/
			FMToperatingareacomparator(const FMToperatingarea& oparea);
			// DocString: FMToperatingareacomparator::operator()
			/**
			Comparator for FMToperating area.
			*/
			bool operator()(const FMToperatingarea& oparea) const;

		};

	}
	BOOST_CLASS_EXPORT_KEY(Heuristics::FMToperatingarea)
#endif
#endif
