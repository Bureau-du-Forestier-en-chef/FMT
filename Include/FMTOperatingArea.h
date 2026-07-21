/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI

#ifndef FMToperatingarea_Hm_included
#define FMToperatingarea_Hm_included

#include "FMTMask.h"
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
	@brief Operating area giving information about its mask, neighbors and neighbors perimeter.
	@details The operating area has to be based on static themes not used within transitions.
	*/
	class FMTEXPORT FMTOperatingArea
		{
		// DocString: FMTOperatingArea::serialize
		/**
		@brief Serialize the FMTOperatingArea for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
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
			@brief Add area to the operating area.
			@param[in] value the area to add.
			@return a reference to this FMTOperatingArea.
			*/
			FMTOperatingArea& operator += (const double& value);
			// DocString: FMTOperatingArea::getNeighbors
			/**
			@brief Return a copy of the neighbors masks of the operating area.
			@return the neighbors masks.
			*/
			std::vector<Core::FMTMask> getNeighbors() const;
			// DocString: FMTOperatingArea::getArea
			/**
			@brief Get the area of the operating area.
			@return the area.
			*/
			double getArea() const;
			// DocString: FMTOperatingArea::getNeighborsPerimeter
			/**
			@brief Return the percentage of perimeter the operating area needs to share with another operating area to be considered a neighbor.
			@return the neighbors perimeter.
			*/
			double getNeighborsPerimeter() const;
			// DocString: FMTOperatingArea::getMask
			/**
			@brief Return the mask of the operating area.
			@return the mask.
			*/
			Core::FMTMask getMask() const;
			// DocString: FMTOperatingArea::setNeighbors
			/**
			@brief Set the neighbors masks of the operating area.
			@param[in] lneighbors the neighbors masks.
			*/
			void setNeighbors(const std::vector<Core::FMTMask>& lneighbors);
			// DocString: FMTOperatingArea(const Core::FMTMask,const double)
			/**
			@brief Main constructor for FMTOperatingArea from a mask and a neighbors perimeter.
			@details Before synchronizing to the solver interface, the user has to provide the green up, return time and other parameters for each operating area.
			@param[in] lmask the mask.
			@param[in] lneighborsperimeter the neighbors perimeter.
			*/
			FMTOperatingArea(const Core::FMTMask& lmask,const double& lneighborsperimeter);
			// DocString: FMTOperatingArea::setArea
			/**
			@brief Set the area of the operating area.
			@param[in] newarea the new area.
			*/
			void setArea(const double& newarea);

			// DocString: FMTOperatingArea()
			/**
			@brief Default constructor for FMTOperatingArea.
			*/
			FMTOperatingArea()=default;
			// DocString: FMTOperatingArea(const FMTOperatingArea)
			/**
			@brief Copy constructor for FMTOperatingArea.
			@param[in] rhs the FMTOperatingArea to copy.
			*/
			FMTOperatingArea(const FMTOperatingArea& rhs) = default;
			// DocString: FMTOperatingArea::operator=
			/**
			@brief Copy assignment operator for FMTOperatingArea.
			@param[in] rhs the FMTOperatingArea to copy.
			@return a reference to this FMTOperatingArea.
			*/
			FMTOperatingArea& operator = (const FMTOperatingArea& rhs)=default;
			// DocString: FMTOperatingArea::operator==
			/**
			@brief Comparison operator for FMTOperatingArea.
			@param[in] rhs the FMTOperatingArea to compare to.
			@return true if both operating areas are equal else false.
			*/
			bool operator == (const FMTOperatingArea& rhs) const;
			// DocString: FMTOperatingArea::operator!=
			/**
			@brief Comparison operator for FMTOperatingArea.
			@param[in] rhs the FMTOperatingArea to compare to.
			@return true if both operating areas are different else false.
			*/
			bool operator != (const FMTOperatingArea& rhs) const;
			// DocString: ~FMTOperatingArea()
			/**
			@brief Default destructor for FMTOperatingArea.
			*/
			virtual ~FMTOperatingArea()=default;
			// DocString: FMTOperatingArea::presolve
			/**
			@brief Return a presolved FMTOperatingArea using a mask filter and a subset of the original themes.
			@param[in] filter the mask filter.
			@param[in] presolvedthemes the presolved themes.
			@return the presolved operating area.
			*/
			FMTOperatingArea presolveOperatingArea(const Core::FMTMaskFilter& filter, const std::vector<Core::FMTTheme>&presolvedthemes) const;
			// DocString: FMTOperatingArea::postSolve
			/**
			@brief Return a postsolved FMTOperatingArea using a mask filter and the original themes.
			@param[in] filter the mask filter.
			@param[in] basethemes the base themes.
			@return the postsolved operating area.
			*/
			FMTOperatingArea postsolveOperatingArea(const Core::FMTMaskFilter& filter, const std::vector<Core::FMTTheme>&basethemes) const;

		};

	// DocString: FMTOperatingAreaComparator
	/**
	@brief Comparator to find a FMTOperatingArea with std::find_if when operating areas are in an stl container.
	*/
	class FMTEXPORT FMTOperatingAreaComparator
		{
			// DocString: FMTOperatingAreaComparator::mask
			///FMTMask of the operating area we wish to find.
			Core::FMTMask mask;
		public:
			// DocString: FMTOperatingAreaComparator(const Core::FMTMask)
			/**
			@brief Construct the comparator from a mask reference.
			@param[in] lmask the mask.
			*/
			FMTOperatingAreaComparator(const Core::FMTMask& lmask);
			// DocString: FMTOperatingAreaComparator(const FMTOperatingArea& oparea)
			/**
			@brief Construct the comparator from an operating area.
			@param[in] oparea the operating area.
			*/
			FMTOperatingAreaComparator(const FMTOperatingArea& oparea);
			// DocString: FMTOperatingAreaComparator::operator()
			/**
			@brief Comparator for FMTOperatingArea.
			@param[in] oparea the operating area to compare to.
			@return true if the operating area matches else false.
			*/
			bool operator()(const FMTOperatingArea& oparea) const;

		};

	}
	BOOST_CLASS_EXPORT_KEY(Heuristics::FMTOperatingArea)
#endif
#endif
