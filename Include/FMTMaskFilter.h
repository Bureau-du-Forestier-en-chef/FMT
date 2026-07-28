/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTMASKFILTER_Hm_included
#define FMTMASKFILTER_Hm_included


#include <boost/dynamic_bitset.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <vector>
#include  <boost/functional/hash.hpp>
#include "FMTutility.h"


namespace Core
{
	class FMTMask;
	class FMTTheme;


// DocString: FMTMaskFilter
/**
@brief Filter built from masks used to presolve and postsolve FMTMask objects.
*/
class FMTEXPORT FMTMaskFilter
    {
    public:
        // DocString: FMTMaskFilter()
        /**
        @brief Default constructor for FMTMaskFilter.
        */
        FMTMaskFilter();
        // DocString: ~FMTMaskFilter()
        /**
        @brief Default virtual destructor for FMTMaskFilter.
        */
        virtual ~FMTMaskFilter()=default;
        // DocString: FMTMaskFilter(const FMTMaskFilter&)
        /**
        @brief Copy constructor for FMTMaskFilter.
        @param[in] rhs the FMTMaskFilter to copy.
        */
        FMTMaskFilter(const FMTMaskFilter& rhs);
        // DocString: FMTMaskFilter::operator=
        /**
        @brief Copy assignment operator for FMTMaskFilter.
        @param[in] rhs the FMTMaskFilter to copy.
        @return a reference to this FMTMaskFilter.
        */
        FMTMaskFilter& operator = (const FMTMaskFilter& rhs);
		// DocString: FMTMaskFilter(const FMTMask&, const FMTMask&)
		/**
		@brief Construct a filter from a presolve selection mask and a buffer mask.
		@param[in] presolveselection the presolve selection mask.
		@param[in] buffermask the buffer mask.
		*/
		FMTMaskFilter(const FMTMask& presolveselection, const FMTMask& buffermask);
		// DocString: FMTMaskFilter(const FMTMask&)
		/**
		@brief Construct a filter from a presolve selection mask.
		@param[in] presolveselection the presolve selection mask.
		*/
		FMTMaskFilter(const FMTMask& presolveselection);
        // DocString: FMTMaskFilter(std::vector<FMTMask>&)
        /**
        @brief Construct a filter from a vector of masks.
        @param[in] masks the masks to build the filter from.
        */
        FMTMaskFilter(std::vector<FMTMask>& masks);
        // DocString: FMTMaskFilter(std::vector<FMTMask>&,const std::vector<FMTTheme>&)
        /**
        @brief Construct a filter from a vector of masks and the themes.
        @param[in] masks the masks to build the filter from.
        @param[in] themes the themes.
        */
        FMTMaskFilter(std::vector<FMTMask>& masks,const std::vector<FMTTheme>& themes);
        // DocString: FMTMaskFilter::filter
        /**
        @brief Filter a development mask.
        @param[in] devmask the development mask to filter.
        @return the filtered mask.
        */
        FMTMask filter(const FMTMask& devmask) const;
		// DocString: FMTMaskFilter::swap
		/**
		@brief Swap this filter with another.
		@param[in,out] rhs the filter to swap with.
		*/
		void swap(FMTMaskFilter& rhs);
		// DocString: FMTMaskFilter::presolve
		/**
		@brief Return a presolved copy of the filter.
		@param[in] themes the themes.
		@return the presolved filter.
		*/
		FMTMaskFilter presolve(const std::vector<FMTTheme>& themes) const;
		// DocString: FMTMaskFilter::canPresolve
		/**
		@brief Return true if the mask can be presolved with the filter.
		@param[in] mask the mask to check.
		@param[in] themes the themes.
		@return true if the mask can be presolved else false.
		*/
		bool canPresolve(const FMTMask& mask, const std::vector<const Core::FMTTheme*>& themes) const;
		// DocString: FMTMaskFilter::getSelectedThemes
		/**
		@brief Return the themes selected by the filter.
		@param[in] themes the themes.
		@return the selected themes.
		*/
		std::vector<const Core::FMTTheme*> getSelectedThemes(const std::vector<Core::FMTTheme>& themes) const;
		// DocString: FMTMaskFilter::emptyFlipped
		/**
		@brief Return true if the flipped selection is empty.
		@return true if the flipped selection is empty else false.
		*/
		inline bool emptyFlipped() const
			{
			return flippedselection.empty();
			}
		// DocString: FMTMaskFilter::empty
		/**
		@brief Return true if the selection is empty.
		@return true if the selection is empty else false.
		*/
		inline bool empty() const
			{
			return selection.empty();
			}
		// DocString: FMTMaskFilter::hash
		/**
		@brief Return the hash of the filter.
		@return the hash value.
		*/
		inline size_t hash() const
			{
			return (boost::hash<boost::dynamic_bitset<uint8_t>>()(selection) ^ boost::hash<boost::dynamic_bitset<uint8_t>>()(flippedselection));
			}
    protected:
		/*boost::dynamic_bitset<> fullset;*/
        boost::dynamic_bitset<uint8_t> selection;
        boost::dynamic_bitset<uint8_t> flippedselection;
		//std::vector<size_t>index;
    private:
	friend class boost::serialization::access;
	friend class FMTTheme;
	friend class FMTMask;
	// DocString: FMTMaskFilter::serialize
	/**
	@brief Serialize the FMTMaskFilter for multiprocessing across multiple cpus (pickle in Python).
	@tparam Archive the archive type.
	@param[in,out] ar the archive to serialize to or from.
	@param[in] version the serialization version.
	*/
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		/*ar & BOOST_SERIALIZATION_NVP(fullset);*/
		ar & BOOST_SERIALIZATION_NVP(selection);
		ar & BOOST_SERIALIZATION_NVP(flippedselection);
		//ar & BOOST_SERIALIZATION_NVP(index);
		}
    };



}

namespace boost {

	template <>
	struct hash<Core::FMTMaskFilter>
	{
		std::size_t operator()(const Core::FMTMaskFilter& filter) const
		{

			return (filter.hash());
		}
	};


}

#endif // FMTMASKFILTER_Hm_included
