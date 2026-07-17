/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI

#ifndef FMToperatingarea_Hm_included
#define FMToperatingarea_Hm_included

#include "FMTmask.h"
#include <vector>
#include <map>
#include <memory>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>

namespace Heuristics
	{
	// DocString: FMTOperatingArea
	/**
	FMTOperatingArea gives informations about a given operating area it's mask,neighbors,neighborsperimeter.
	FMToperating area have to be based on static themes (not used within transitions).
	*/
	class FMTEXPORT FMTOperatingArea
		{
		// DocString: FMTOperatingArea::serialize
		/**
		serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
			{
			ar & BOOST_SERIALIZATION_NVP(mask);
			ar & BOOST_SERIALIZATION_NVP(neighbors);
			ar & BOOST_SERIALIZATION_NVP(neighborsperimeter);
			ar & BOOST_SERIALIZATION_NVP(_area);
			}
		protected:
		// DocString: FMTOperatingArea::mask
		///The mask describing the operating area
		Core::FMTMask mask;
		// DocString: FMTOperatingArea::neighbors
		///Neighbors mask of the operating area
		std::vector<Core::FMTMask>neighbors;
		// DocString: FMTOperatingArea::neighborsperimeter
		///neighborsperimeter is the ratio a operatingarea needs to share to a other operatingarea to be considered neighbor.
		double neighborsperimeter;
		// DocString: FMTOperatingArea::_area
		///The initial area of the operating area used as big M for the MIP.
		double _area;
		public:
			// DocString: FMTOperatingArea::operator+=
			/**
			Add area to the operating area.
			*/
			FMTOperatingArea& operator += (const double& value);
			// DocString: FMTOperatingArea::getNeighbors
			/**
			Getter returning a copy of the neighbors masks data of the operatingarea.
			*/
			std::vector<Core::FMTMask> getNeighbors() const;
			// DocString: FMTOperatingArea::getArea
			/**
				Get the area of the operating area.
			*/
			double getArea() const;
			// DocString: FMTOperatingArea::getNeighborsPerimeter
			/**
			Getter returning the percentage of perimeter the oprating area needs to share with a other operating area,
			to be considered neighbor.
			*/
			double getNeighborsPerimeter() const;
			// DocString: FMTOperatingArea::getMask
			/**
			Getter returning mask of the operating area.
			*/
			Core::FMTMask getMask() const;
			// DocString: FMTOperatingArea::setNeighbors
			/**
			Setter for the neighbors mask member data.
			*/
			void setNeighbors(const std::vector<Core::FMTMask>& lneighbors);
			// DocString: FMTOperatingArea(const Core::FMTMask,const double)
			/**
			Main FMTOperatingArea constructor targeting the user. Before synchronizing everything to the solverinterface,
			the user has to provide to the heuristics all the green-up, returntime etc.... for each operating area.
			*/
			FMTOperatingArea(const Core::FMTMask& lmask,const double& lneighborsperimeter);
			// DocString: FMTOperatingArea::setArea
			/**
			Set the area of the operating area.
			*/
			void setArea(const double& newarea);

			// DocString: FMTOperatingArea()
			/**
			Default FMTOperatingArea constructor
			*/
			FMTOperatingArea()=default;
			// DocString: FMTOperatingArea(const FMTOperatingArea)
			/**
			FMTOperatingArea copy constructor
			*/
			FMTOperatingArea(const FMTOperatingArea& rhs) = default;
			// DocString: FMTOperatingArea::operator=
			/**
			FMTOperatingArea copy assignment
			*/
			FMTOperatingArea& operator = (const FMTOperatingArea& rhs)=default;
			// DocString: FMTOperatingArea::operator==
			/**
			Comparison operator of FMTOperatingArea
			*/
			bool operator == (const FMTOperatingArea& rhs) const;
			// DocString: FMTOperatingArea::operator!=
			/**
			Comparison operator of FMTOperatingArea
			*/
			bool operator != (const FMTOperatingArea& rhs) const;
			// DocString: ~FMTOperatingArea()
			/**
			Default FMTOperatingArea destructor
			*/
			virtual ~FMTOperatingArea()=default;
			// DocString: FMTOperatingArea::presolve
			/**
			Using a FMTMask (selectedmask) and a subset of the original FMTthemes used to construct the FMTMask,
			it returns a presolved FMTMask with potentialy less data.
			*/
			FMTOperatingArea presolveOperatingArea(const Core::FMTMaskFilter& filter, const std::vector<Core::FMTTheme>&presolvedthemes) const;
			// DocString: FMTOperatingArea::postSolve
			/**
			Using a FMTMask (selectedmask) and the original FMTthemes it returns a postsolved FMTMask.
			*/
			FMTOperatingArea postsolveOperatingArea(const Core::FMTMaskFilter& filter, const std::vector<Core::FMTTheme>&basethemes) const;

		};

	// DocString: FMTOperatingAreaComparator
	/**
	This class is made to compare FMTOperatingArea using the std::find_if() function when FMTOperatingArea are in a stl container.
	*/
	class FMTEXPORT FMTOperatingAreaComparator
		{
			// DocString: FMTOperatingAreaComparator::mask
			///FMTMask of the operating area we wish to find.
			Core::FMTMask mask;
		public:
			// DocString: FMTOperatingAreaComparator(const Core::FMTMask)
			/**
			Constructor of the comparator using the FMTMask as reference.
			*/
			FMTOperatingAreaComparator(const Core::FMTMask& lmask);
			// DocString: FMTOperatingAreaComparator(const FMTOperatingArea& oparea)
			/**
			Constructor of the comparator using the operating area.
			*/
			FMTOperatingAreaComparator(const FMTOperatingArea& oparea);
			// DocString: FMTOperatingAreaComparator::operator()
			/**
			Comparator for FMToperating area.
			*/
			bool operator()(const FMTOperatingArea& oparea) const;

		};

	}
	BOOST_CLASS_EXPORT_KEY(Heuristics::FMTOperatingArea)
#endif
#endif
