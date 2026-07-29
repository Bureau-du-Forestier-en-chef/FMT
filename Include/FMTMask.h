/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTMASK_Hm_included
#define FMTMASK_Hm_included

#ifndef BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS
#define BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS
#endif

#include <boost/dynamic_bitset.hpp>
#include <boost/unordered_map.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/functional/hash.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/string.hpp>
#include <boost/functional/hash.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/dynamic_bitset/serialization.hpp>
#include "FMTException.h"
#include <vector>
#include <string>




#if (BOOST_VERSION / 100 % 1000) < 71
/**
Function part of boost and serialization needed to serialize boost::dynamic_bitset<uint8_t>.
*/
namespace boost {
	namespace serialization {

		template <typename Ar, typename Block, typename Alloc>
		void save(Ar& ar, dynamic_bitset<Block, Alloc> const& bs, unsigned) {
			size_t num_bits = bs.size();
			std::vector<Block> blocks(bs.num_blocks());
			to_block_range(bs, blocks.begin());

			ar& BOOST_SERIALIZATION_NVP(num_bits);
			ar& BOOST_SERIALIZATION_NVP(blocks);
		}

		template <typename Ar, typename Block, typename Alloc>
		void load(Ar& ar, dynamic_bitset<Block, Alloc>& bs, unsigned) {
			size_t num_bits;
			std::vector<Block> blocks;
			ar& BOOST_SERIALIZATION_NVP(num_bits);
			ar& BOOST_SERIALIZATION_NVP(blocks);

			bs.resize(num_bits);
			from_block_range(blocks.begin(), blocks.end(), bs);
			bs.resize(num_bits);
		}

		template <typename Ar, typename Block, typename Alloc>
		void serialize(Ar& ar, dynamic_bitset<Block, Alloc>& bs, unsigned version) {
			split_free(ar, bs, version);
		}

	}
}
/**
The boost::dynamic_bitset<uint8_t> hashing was missing the boost version older thant 1.71
*/
namespace boost {
	template <typename Block, typename Alloc>
	std::size_t hash_value(const boost::dynamic_bitset<Block, Alloc>& bs)
	{
	std::size_t hash = hash_value(bs.m_num_bits);
	boost::hash_combine(hash, bs.m_bits); 
	return hash;
	}
}
#endif

namespace Core
{

class FMTTheme;
class FMTMaskFilter;
// DocString: FMTMask
/**
@brief Core class holding, as a boost dynamic bitset, which theme attributes are part of a mask.
@details A set bit means the attribute of a given theme is part of the mask; the data size equals the sum of the theme sizes. The name member holds the attribute values as a string. Multiple bits set for a theme mean the mask contains an aggregate or a question mark.
*/
class FMTEXPORT FMTMask
    {

    public:
		// DocString: FMTMask::(FMTMask&& rhs)
		/**
		@brief Move copy constructor
		@param[in] rhs development to swap
		*/
		FMTMask(FMTMask&& rhs) noexcept;
		// DocString: FMTMask::operator=(FMTMask&& rhs)
		/**
		@brief Move assignment operator for FMTMask.
		@param[in] rhs the mask to move from.
		@return a reference to this FMTMask.
		*/
		FMTMask& operator=(FMTMask&& rhs) noexcept;
		// DocString: swap()
		/**
		@brief Swap this mask with another.
		@param[in,out] rhs the mask to swap with.
		*/
		void swap(FMTMask& rhs);
		// DocString: FMTMask()
		/**
		@brief Default constructor for FMTMask.
		*/
        FMTMask();
		// DocString: FMTMask(const FMTMask&)
		/**
		@brief Copy constructor for FMTMask.
		@param[in] rhs the FMTMask to copy.
		*/
		FMTMask(const FMTMask& rhs);
		// DocString: ~FMTMask()
		/**
		@brief Default virtual destructor for FMTMask.
		*/
        virtual ~FMTMask()=default;
		// DocString: FMTMask(const boost::dynamic_bitset<uint8_t>&)
		/**
		@brief Construct a mask setting only the bitset data member.
		@param[in] bits the bitset data.
		*/
        FMTMask(const boost::dynamic_bitset<uint8_t>& bits);
		// DocString: FMTMask::operator bool
		/**
		@brief Return true if the mask is not empty by looking at the data bitset.
		@return true if the mask is not empty else false.
		*/
		explicit operator bool() const;
		// DocString: FMTMask(const std::string&,const boost::dynamic_bitset<uint8_t>&)
		/**
		@brief Construct a mask from an already set bitset and a string mask used as name.
		@param[in] mask the string mask used as name.
		@param[in] bits the bitset data.
		*/
        FMTMask(const std::string& mask,const boost::dynamic_bitset<uint8_t>& bits);
		// DocString: FMTMask(const std::string&,const std::vector<FMTTheme>&)
		/**
		@brief Construct a complete mask from a string and sorted themes.
		@param[in] mask the string mask.
		@param[in] themes the sorted themes.
		*/
        FMTMask(const std::string& mask,const std::vector<FMTTheme>& themes);
		// DocString: FMTMask(const std::vector<std::string>&,const std::vector<FMTTheme>&)
		/**
		@brief Construct a mask from a vector of attribute strings and sorted themes.
		@param[in] values the attribute strings.
		@param[in] themes the sorted themes.
		*/
        FMTMask(const std::vector<std::string>& values,const std::vector<FMTTheme>& themes);
		// DocString: FMTMask(const std::vector<FMTTheme>&,const std::vector<FMTTheme>&)
		/**
		@brief Construct an empty mask sized for the given themes.
		@param[in] themes the sorted themes.
		*/
		FMTMask(const std::vector<FMTTheme>& themes);
		// DocString: FMTMask::clear
		/**
		@brief Clear the data and name members.
		*/
		void clear();
		// DocString: FMTMask::size
		/**
		@brief Return the size of the bitset data member.
		@return the size of the bitset.
		*/
		size_t size() const;
		// DocString: FMTMask::count
		/**
		@brief Return the number of bits set.
		@return the number of bits set.
		*/
		size_t count() const;
		// DocString: FMTMask::get
		/**
		@brief Get the string mask name based on the bitset data and sorted themes.
		@param[in] themes the sorted themes.
		@return the string mask name.
		*/
        std::string get(const std::vector<FMTTheme>& themes) const;
		// DocString: FMTMask::get
		/**
		@brief Get the attribute of the mask for a given theme.
		@param[in] theme the theme.
		@return the attribute of the theme.
		*/
		std::string get(const FMTTheme& theme) const;
		// DocString: FMTMask::getAttribute
		/**
		@brief When you deal with FMTDevelopment you can directly call this to get a reference to the attribute string
		@param[in] p_theme the theme we target
		@return a const reference to the attribute.
		*/
		const std::string& getAttribute(const FMTTheme& p_theme) const;
		// DocString: FMTMask::getSubsetCount
		/**
		@brief Return the number of bits set for the given theme subset.
		@param[in] theme the theme.
		@return the number of bits set for the theme subset.
		*/
		size_t getSubsetCount(const FMTTheme& theme) const;
		// DocString: FMTMask::set
		/**
		@brief Set the bitset data for some themes from a mask string.
		@param[in] themes the themes.
		@param[in] value the mask string.
		*/
        void set(const std::vector<FMTTheme>& themes,const std::string& value);
		// DocString: FMTMask::setExclusiveBits
		/**
		@brief Flip only the theme subset and clear the mask of the intersection with another mask.
		@param[in] p_mask the mask to intersect with.
		@param[in] p_theme the theme to subset on.
		*/
		void setExclusiveBits(const FMTMask& p_mask,const FMTTheme& p_theme);
		// DocString: FMTMask::getNonFullBlocks
		/**
		@brief Return the indexes of the blocks that contain a 0.
		@return a vector of indexes of the blocks that contain a 0.
		*/
		std::vector<size_t>getNonFullBlocks() const;
		// DocString: FMTMask::isSubsetOf
		/**
		@brief Check if this mask is a subset of another mask by looking at a block subset.
		@param[in] p_mask the other mask.
		@param[in] p_subset the block subset.
		@return true if all blocks are subset of this else false.
		*/
		bool isSubsetOf(const FMTMask& p_mask,const std::vector<size_t>& p_subset) const;
		// DocString: FMTMask::set
		/**
		@brief Set the bitset data for a given theme from an attribute.
		@param[in] theme the theme.
		@param[in] value the attribute.
		*/
		void set(const FMTTheme& theme, const std::string& value);
		// DocString: FMTMask::getStaticThemes
		/**
		@brief Return the themes that only use aggregates or a question mark for the mask.
		@param[in] themes the themes to filter.
		@return the static themes.
		*/
		std::vector<const Core::FMTTheme*>getStaticThemes(const std::vector<const Core::FMTTheme*>& themes) const;
		// DocString: FMTMask::getSelectedThemes
		/**
		@brief Return the themes that have any bit set in the mask.
		@param[in] themes the themes to filter.
		@return the selected themes.
		*/
		std::vector<const Core::FMTTheme*> getSelectedThemes(const  std::vector<Core::FMTTheme>& themes) const;
		// DocString: FMTMask::isNotThemesSubset
		/**
		@brief Return true if this mask is not part of rhs based on the sorted themes.
		@param[in] rhs the other mask.
		@param[in] themes the sorted themes.
		@return true if this mask is not a subset of rhs else false.
		*/
		bool isNotThemesSubset(const FMTMask& rhs, const  std::vector<const Core::FMTTheme*>& themes) const;
		// DocString: FMTMask::isNotThemesSubset
		/**
		@brief Return true if this mask is not part of rhs based on the sorted themes.
		@param[in] rhs the other mask.
		@param[in] themes the sorted themes.
		@return true if this mask is not a subset of rhs else false.
		*/
		bool isNotThemesSubset(const FMTMask& rhs, const  std::vector<Core::FMTTheme>& themes) const;
		// DocString: FMTMask::removeAggregates
		/**
		@brief Return a new mask with the bits of aggregates or question marks unset for the given themes.
		@param[in] themes the themes.
		@param[in] questionmarkonly if true only removes question marks.
		@return the mask without aggregates.
		*/
		FMTMask removeAggregates(const std::vector<FMTTheme>& themes,bool questionmarkonly=false) const;
		// DocString: FMTMask::empty
		/**
		@brief Return true if the data member is empty.
		@return true if the mask is empty else false.
		*/
		bool empty() const;
		// DocString: FMTMask::update
		/**
		@brief Update the name member from the bitset data using a sorted list of themes.
		@param[in] themes the sorted themes.
		*/
        void update(const std::vector<FMTTheme>& themes);
		// DocString: FMTMask::decompose
		/**
		@brief Disaggregate the attribute of a theme from the mask into a vector of masks, one per possible attribute.
		@param[in] theme the theme to decompose.
		@return a vector of masks, one per possible attribute.
		*/
		std::vector<FMTMask>decompose(const FMTTheme &theme) const;
		// DocString: FMTMask::append
		/**
		@brief Append a bitset to the bitset data member of the mask.
		@param[in] bits the bitset to append.
		*/
        void append(const boost::dynamic_bitset<uint8_t> &bits);

		// DocString: FMTMask::binarizedAppend
		/**
		@brief Binarize an element and append it to the mask.
		@tparam typetobinarize the type of the element.
		@param[in] element the element to binarize and append.
		*/
		template <class typetobinarize>
		void binarizedAppend(const typetobinarize& element)
		{
			size_t  location = m_data.size();
			m_data.resize(m_data.size() + (sizeof(typetobinarize) * 8));
			const char* charelements = reinterpret_cast<const char*>(&element);
			for (size_t charit = 0; charit < sizeof(typetobinarize); ++charit)
			{
				const char value = charelements[charit];
				for (int i = 7; i >= 0; --i)
				{
					m_data[location] = ((value & (1 << i)));
					++location;
				}
			}
		}
		// DocString: FMTMask::getUnion
		/**
		@brief Return the union of this mask and another mask.
		@param[in] rhs the other mask.
		@return the union mask.
		*/
		FMTMask getUnion(const FMTMask& rhs) const;
		// DocString: FMTMask::getIntersect
		/**
		@brief Return the intersection of this mask and another mask.
		@param[in] rhs the other mask.
		@return the intersection mask.
		*/
		FMTMask getIntersect(const FMTMask& rhs) const;
		// DocString: FMTMask::setIntersect
		/**
		@brief set the intersect with this mask using rhs mask.
		@param[in] rhs the mask we intersect with.
		*/
		void setIntersect(const FMTMask& rhs);
		// DocString: FMTMask::getBitsetIntersect
		/**
		@brief Return the bitset of the intersection of this mask and another mask.
		@param[in] rhs the other mask.
		@return the intersection bitset.
		*/
		boost::dynamic_bitset<uint8_t> getBitsetIntersect(const FMTMask& rhs) const;
		// DocString: FMTMask::operator=
		/**
		@brief Copy assignment operator for FMTMask.
		@param[in] rhs the mask to copy.
		@return a reference to this FMTMask.
		*/
        FMTMask& operator = (const FMTMask& rhs);
		// DocString: FMTMask::operator!=
		/**
		@brief Inequality comparison operator of FMTMask.
		@param[in] rhs the mask to compare with.
		@return true if both masks are different else false.
		*/
        bool operator != (const FMTMask& rhs) const;
		// DocString: FMTMask::operator==
		/**
		@brief Equality comparison operator of FMTMask.
		@param[in] rhs the mask to compare with.
		@return true if both masks are equal else false.
		*/
        bool operator == (const FMTMask& rhs) const;
		// DocString: FMTMask::operator<
		/**
		@brief Less than comparison operator of FMTMask.
		@param[in] rhs the mask to compare with.
		@return true if this mask is less than rhs else false.
		*/
        bool operator < (const FMTMask& rhs) const;
		// DocString: FMTMask::resume
		/**
		@brief Return a mask with the selected name and data using a bitset.
		@param[in] rhs the selection bitset.
		@return the resumed mask.
		*/
		FMTMask resume(const boost::dynamic_bitset<uint8_t>& rhs) const;
		// DocString: FMTMask::resume
		/**
		@brief Return a mask with the selected name and data using mask indexes.
		@param[in] indexes the selection indexes.
		@return the resumed mask.
		*/
		FMTMask resume(const std::vector<size_t>& indexes) const;
		// DocString: FMTMask::hash
		/**
		@brief Return the hash of the mask.
		@return the hash value.
		*/
		inline size_t hash() const
			{
			return boost::hash<boost::dynamic_bitset<uint8_t>>()(m_data);
			}
		// DocString: FMTMask::getBitsString
		/**
		@brief Return the binary representation of the bitset as a string.
		@return the binary representation of the bitset.
		*/
        std::string getBitsString() const;
		// DocString: FMTMask::operator std::string
		/**
		@brief Return the name of the mask.
		@return the name of the mask.
		*/
		inline operator std::string() const
			{
			return m_name;
			}
		// DocString: FMTMask::isSubsetOf
		/**
		@brief Check if this mask is a subset of another mask.
		@param[in] p_rhs the mask to check against.
		@return true if this mask is a subset of p_rhs else false.
		*/
		bool isSubsetOf(const FMTMask& p_rhs) const;
		// DocString: FMTMask::getBitsetReference
		/**
		@brief Return a const reference to the bitset data member.
		@return a const reference to the bitset.
		*/
		inline const boost::dynamic_bitset<uint8_t>& getBitsetReference() const
			{
			return m_data;
			}
		// DocString: FMTMask::getStringReference
		/**
		@brief Return a const reference to the name string member.
		@return a const reference to the name string.
		*/
		inline const std::string& getStringReference() const
		{
			return m_name;
		}
		// DocString: FMTMask::reserve
		/**
		@brief reserve memory for the boost dynamicbitset.
		@param[in] p_size number of bits to reserve.
		*/
		void reserve(size_t p_size);
		// DocString: FMTMask::refine
		/**
		@brief Return a new mask where the question marks are completed with the attributes of another mask.
		@param[in] mask the mask to refine from.
		@param[in] themes the themes.
		@return the refined mask.
		*/
		FMTMask refine(const FMTMask& mask, const std::vector<FMTTheme>& themes) const;
		// DocString: FMTMask::getPostsolveMask
		/**
		@brief Turn off the bits of themes that are still active on a base postsolve mask.
		@param[in] mask the mask used to turn off bits.
		@param[in] themes the themes.
		@return the postsolve mask.
		*/
		FMTMask getPostsolveMask(const FMTMask& mask,const std::vector<FMTTheme>& themes) const;
		// DocString: FMTMask::presolve
		/**
		@brief Return a presolved mask with potentially less data using a filter and a subset of the original themes.
		@param[in] filter the mask filter.
		@param[in] presolvedthemes the presolved themes.
		@return the presolved mask.
		*/
		FMTMask presolve(const FMTMaskFilter& filter,const std::vector<FMTTheme>&presolvedthemes) const;
		// DocString: FMTMask::presolveRef
		/**
		@brief Presolve this mask in place with potentially less data using a filter and a subset of the original themes.
		@param[in] p_filter the mask filter.
		@param[in] p_presolvedThemes the presolved themes.
		@param[in] p_allowReallocation if true allows reallocation of the data.
		*/
		void presolveRef(const FMTMaskFilter& p_filter, const std::vector<FMTTheme>& p_presolvedThemes,bool p_allowReallocation = true);

		// DocString: FMTMask::canPresolve
		/**
		@brief Return true if the mask can be presolved with the given filter and themes.
		@param[in] p_filter the mask filter.
		@param[in] p_presolvedThemes the presolved themes.
		@return true if the mask can be presolved else false.
		*/
		bool canPresolve(const FMTMaskFilter& p_filter, const std::vector<FMTTheme>& p_presolvedThemes) const;
		// DocString: FMTMask::postSolve
		/**
		@brief Return a postsolved mask using a filter and the original themes.
		@param[in] filter the mask filter.
		@param[in] basethemes the original themes.
		@return the postsolved mask.
		*/
		FMTMask postSolve(const FMTMaskFilter& filter, const std::vector<FMTTheme>&basethemes) const;
	private:
		friend class FMTTheme;
		// DocString: FMTMask::serialize
		/**
		@brief Serialize the FMTMask for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("data", m_data);
			ar& boost::serialization::make_nvp("name", m_name);
		}
		// DocString: FMTMask::_subset
		/**
		@brief Return the data subset bits for a given theme, of the length of the theme.
		@param[in] theme the theme.
		@return the subset bits of the theme.
		*/
		boost::dynamic_bitset<uint8_t> _subset(const FMTTheme& theme) const;
		// DocString: FMTMask::operator []
		/**
		@brief get bit value at i position.
		@param[in] i position
		@return true if on false if off
		*/
		bool operator [](int i) const;
		// DocString: FMTMask::_anyIntersect
		/**
		@brief check if two subset of mask intersect each other.
		@param[in] p_MASK the mask we check with
		@param[in] p_THEME the theme we subset on
		@return true if intersect else false.
		*/
		bool _anyIntersect(const FMTMask& p_MASK, const FMTTheme& p_THEME) const;
		// DocString: FMTMask::_countTheme
		/**
		@brief count number of bit set in the theme subset.
		@param[in] p_THEME the theme we subset on
		@return number of time the bits are on
		*/
		size_t _countTheme(const FMTTheme& p_Theme) const;
		// DocString: FMTMask::_getPresolveMas
		/**
		@brief Return the bitset used to presolve the mask.
		@param[in] p_filter the mask filter.
		@param[in] p_presolvedThemes the presolved themes.
		@return the presolve bitset.
		*/
		boost::dynamic_bitset<uint8_t> _getPresolveMask(const FMTMaskFilter& p_filter,
								const std::vector<FMTTheme>& p_presolvedThemes) const;
		// DocString: FMTMask::_setSubset
		/**
		@brief Set a given subset of the length of a theme for that theme in the mask.
		@param[in] theme the theme.
		@param[in] subset the subset bits to set.
		*/
		void _setSubset(const FMTTheme& theme, const boost::dynamic_bitset<uint8_t>& subset);
		// DocString: FMTMask::m_name
		///name of the FMTMask attributes or aggregates splitted by a space
		std::string m_name;
		// DocString: FMTMask::m_data
		///dynamic bitset holding the attributes information member.
		boost::dynamic_bitset<uint8_t> m_data;
    };


template<> inline void FMTMask::binarizedAppend<std::string>(const std::string& element)
	{
		size_t  location = m_data.size();
		m_data.resize(m_data.size() + (element.size() * 8));
		const char* charelements = element.c_str();
		for (size_t charit = 0; charit < element.size(); ++charit)
		{
			const char value = charelements[charit];
			for (int i = 7; i >= 0; --i)
			{
				m_data[location] = ((value & (1 << i)));
				++location;
			}
		}
	}

template<> inline void FMTMask::binarizedAppend<double>(const double& element)
{
	size_t  location = m_data.size();
	const int corrected = static_cast<int>(element*(1 / FMT_DBL_TOLERANCE));
	m_data.resize(m_data.size() + (sizeof(int) * 8));
	const char* charelements = reinterpret_cast<const char*>(&corrected);
	for (size_t charit = 0; charit < sizeof(int); ++charit)
	{
		const char value = charelements[charit];
		for (int i = 7; i >= 0; --i)
		{
			m_data[location] = ((value & (1 << i)));
			++location;
		}
	}
}

// DocString: FMTMaskComparator
/**
@brief Comparator used to check if a mask already exists in a std container.
*/
class FMTMaskComparator
	{
	public:
		// DocString: FMTMaskComparator:(const FMTMask&)
		/**
		@brief Construct the comparator from the mask to match.
		@param[in] lbase_mask the mask to match.
		*/
		FMTMaskComparator(const FMTMask& lbase_mask);
		// DocString: FMTMaskComparator::operator()(const FMTAction&)
		/**
		@brief Matching test operator for the comparator.
		@param[in] mask the mask to test.
		@return true if the mask matches else false.
		*/
		bool operator()(const FMTMask& mask) const;

	private:
	// DocString: FMTMaskComparator::m_baseMask
	///The mask that we are looking for
	FMTMask m_baseMask;
	};

}

namespace boost {
/**
boost hash for FMTMask
*/
  template <>
  struct hash<Core::FMTMask>
  {
    std::size_t operator()(const Core::FMTMask& mask) const
    {
      return (mask.hash());
    }
  };

}

BOOST_CLASS_EXPORT_KEY(Core::FMTMask)

#endif // FMTMASK_Hm_included
