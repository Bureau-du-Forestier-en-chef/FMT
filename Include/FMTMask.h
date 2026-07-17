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
FMTMask class is one of the most important class in FMT. Alot of classes are bases on FMTMask.
data is boost:dynamic_bitset where if a bit is set that means that the attribute of a given theme
is part of the FMTMask. the data member size == the sum of the FMTthemes size.
The name data member hold the attribute value as a string. Sometime when the FMTMask is used for union
the name might not be set. Also if multiple bits are set for a given FMTTheme it means that the mask
contains a aggregate or a ?.
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
		@brief Move copy assignement
		@param[in] rhs development to swap
		*/
		FMTMask& operator=(FMTMask&& rhs) noexcept;
		// DocString: swap()
		/**
		Swap for FMTMask.
		*/
		void swap(FMTMask& rhs);
		// DocString: FMTMask()
		/**
		Default constructor for FMTMask.
		*/
        FMTMask();
		// DocString: FMTMask(const FMTMask&)
		/**
		FMTMask copy constructor.
		*/
		FMTMask(const FMTMask& rhs);
		// DocString: ~FMTMask()
		/**
		Default destructor for FMTMask.
		*/
        virtual ~FMTMask()=default;
		// DocString: FMTMask(const boost::dynamic_bitset<uint8_t>&)
		/**
		Simple constructor for FMTMask that only sets the bitsets data member.
		*/
        FMTMask(const boost::dynamic_bitset<uint8_t>& bits);
		// DocString: FMTMask::operator bool
		/**
		Returns true if the FMTMask is not empty by looking at the data bitset.
		*/
		explicit operator bool() const;
		// DocString: FMTMask(const std::string&,const boost::dynamic_bitset<uint8_t>&)
		/**
		FMTMask constructor using a already sets bitset (bits) and a string mask to set as name.
		*/
        FMTMask(const std::string& mask,const boost::dynamic_bitset<uint8_t>& bits);
		// DocString: FMTMask(const std::string&,const std::vector<FMTTheme>&)
		/**
		FMTMask constructor using a string (mask) and sorted themes to generate a complete FMTMask.
		*/
        FMTMask(const std::string& mask,const std::vector<FMTTheme>& themes);
		// DocString: FMTMask(const std::vector<std::string>&,const std::vector<FMTTheme>&)
		/**
		FMTMask constructor using a vector of string for the attributes and a vector of sorted themes
		to generate a FMTMask.
		*/
        FMTMask(const std::vector<std::string>& values,const std::vector<FMTTheme>& themes);
		// DocString: FMTMask(const std::vector<FMTTheme>&,const std::vector<FMTTheme>&)
		/**
		FMTMask constructor using a vector of string for the attributes and a vector of sorted themes
		to generate a FMTMask.
		*/
		FMTMask(const std::vector<FMTTheme>& themes);
		// DocString: FMTMask::clear
		/**
		Clear the data and name members.
		*/
		void clear();
		// DocString: FMTMask::size
		/**
		Returns the size of the bitsets data member.
		*/
		size_t size() const;
		// DocString: FMTMask::count
		/**
		Get the number of bits set.
		*/
		size_t count() const;
		// DocString: FMTMask::get
		/**
		Gets the string mask name of the FMTMask based on the bitset data and the sorted themes.
		*/
        std::string get(const std::vector<FMTTheme>& themes) const;
		// DocString: FMTMask::get
		/**
		Get the corresponding attribute of the FMTMask for a given FMTTheme.
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
		Get the number of bits set for the given theme subset.
		*/
		size_t getSubsetCount(const FMTTheme& theme) const;
		// DocString: FMTMask::set
		/**
		Set the bitset data for some FMTthemes (themes) for a mask string (value).
		*/
        void set(const std::vector<FMTTheme>& themes,const std::string& value);
		// DocString: FMTMask::setExclusiveBits
		/**
		@brief flip only the theme subset and clean the mask of the intersect with p_mask.
		@param[in] p_mask
		@param[in] p_theme
		*/
		void setExclusiveBits(const FMTMask& p_mask,const FMTTheme& p_theme);
		// DocString: FMTMask::getNonFullBlocks
		/**
		@brief flip only the theme subset and clean the mask of the intersect with p_mask.
		@return a vector of index of blocks that avec 0 in it.
		*/
		std::vector<size_t>getNonFullBlocks() const;
		// DocString: FMTMask::isSubsetOf
		/**
		@brief Check if this is subset of p_mask by locking at block subset.
		@param[in] p_subset Block Subset.
		@param[in] p_mask the oter mask
		@return true if all blocks are subset of this.
		*/
		bool isSubsetOf(const FMTMask& p_mask,const std::vector<size_t>& p_subset) const;
		// DocString: FMTMask::set
		/**
		Set the bitset data for a given FMTTheme (theme) for a attribute (value).
		*/
		void set(const FMTTheme& theme, const std::string& value);
		// DocString: FMTMask::getStaticThemes
		/**
		Filter FMTTheme from themes that are only using aggregates or ? for the FMTMask.
		*/
		std::vector<const Core::FMTTheme*>getStaticThemes(const std::vector<const Core::FMTTheme*>& themes) const;
		// DocString: FMTMask::getSelectedThemes
		/**
		Takes vector of FMTTheme and return a vector of FMTTheme from which theme have any true bits.
		*/
		std::vector<const Core::FMTTheme*> getSelectedThemes(const  std::vector<Core::FMTTheme>& themes) const;
		// DocString: FMTMask::isNotThemesSubset
		/**
		By looking at each FMTTheme in the FMTMask return true if *this is not part of rhs base on sorted FMTTheme (themes).
		*/
		bool isNotThemesSubset(const FMTMask& rhs, const  std::vector<const Core::FMTTheme*>& themes) const;
		// DocString: FMTMask::isNotThemesSubset
		/**
		By looking at each FMTTheme in the FMTMask return true if *this is not part of rhs base on sorted FMTTheme (themes).
		*/
		bool isNotThemesSubset(const FMTMask& rhs, const  std::vector<Core::FMTTheme>& themes) const;
		// DocString: FMTMask::removeAggregates
		/**
		For each FMTTheme in (themes) if the attribute is an aggregate or ? will unset all
		bits of the given aggregate or ? and return a new FMTMask.
		*/
		FMTMask removeAggregates(const std::vector<FMTTheme>& themes,bool questionmarkonly=false) const;
		// DocString: FMTMask::empty
		/**
		Returns true if the data member is empty.
		*/
		bool empty() const;
		// DocString: FMTMask::update
		/**
		Update the name data member bass on the dynamic bitset data member with a sorted list of FMTTheme (themes).
		*/
        void update(const std::vector<FMTTheme>& themes);
		// DocString: FMTMask::decompose
		/**
		Using a given (theme) this function desaggregate the attribute/aggregate/? of a theme from the FMTMask
		and returns a vector of each possible attribute as multiple masks.
		*/
		std::vector<FMTMask>decompose(const FMTTheme &theme) const;
		// DocString: FMTMask::append
		/**
		Append a bitsets to the bitset data member of the FMTMask.
		*/
        void append(const boost::dynamic_bitset<uint8_t> &bits);

		// DocString: FMTMask::binarizedAppend
		/**
		Binarize any class and append it to the mask.
		*/
		template <class typetobinarize>
		void binarizedAppend(const typetobinarize& element)
		{
			size_t  location = data.size();
			data.resize(data.size() + (sizeof(typetobinarize) * 8));
			const char* charelements = reinterpret_cast<const char*>(&element);
			for (size_t charit = 0; charit < sizeof(typetobinarize); ++charit)
			{
				const char value = charelements[charit];
				for (int i = 7; i >= 0; --i)
				{
					data[location] = ((value & (1 << i)));
					++location;
				}
			}
		}
		// DocString: FMTMask::getUnion
		/**
		Do a union operation on the dynamic bitset of the FMTMask and the given FMTMask (rhs).
		*/
		FMTMask getUnion(const FMTMask& rhs) const;
		// DocString: FMTMask::getIntersect
		/**
		Do a intersection operation on the dynamic bitset of the FMTMask and the given FMTMask (rhs).
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
		Do a intersection operation on the dynamic bitset of the FMTMask and the given FMTMask (rhs)
		but return the bitset.
		*/
		boost::dynamic_bitset<uint8_t> getBitsetIntersect(const FMTMask& rhs) const;
		// DocString: FMTMask::operator=
		/**
		FMTMask copy assignment operator.
		 */
        FMTMask& operator = (const FMTMask& rhs);
		// DocString: FMTMask::operator!=
		/**
		FMTMask nonequality operator.
		*/
        bool operator != (const FMTMask& rhs) const;
		// DocString: FMTMask::operator==
		/**
		Comparison operator of FMTMask.
		*/
        bool operator == (const FMTMask& rhs) const;
		// DocString: FMTMask::operator<
		/**
		Less than operator of FMTMask.
		*/
        bool operator < (const FMTMask& rhs) const;
		// DocString: FMTMask::resume
		/**
		Using a mask (rhs) fix the corresponding resulting FMTMask with the selected name and data.
		*/
		FMTMask resume(const boost::dynamic_bitset<uint8_t>& rhs) const;
		// DocString: FMTMask::resume
		/**
		Using a mask indexes (indexes) fix the corresponding resulting FMTMask with the selected name and data.
		*/
		FMTMask resume(const std::vector<size_t>& indexes) const;
		// DocString: FMTMask::hash
		/**
		Hash function for the FMTMask class.
		*/
		inline size_t hash() const
			{
			return boost::hash<boost::dynamic_bitset<uint8_t>>()(data);
			}
		// DocString: FMTMask::getBitsString
		/**
		Returns the binary representation of the dynamic bitset in a simple string.
		*/
        std::string getBitsString() const;
		// DocString: FMTMask::operator std::string
		/**
		Returns the name of the FMTMask.
		*/
		inline operator std::string() const
			{
			return name;
			}
		// DocString: FMTMask::isSubsetOf
		/**
		@brief Check if the FMTMask is a subset of a FMTMask (rhs).
		@param[in] rhs the model to check if subset
		@return true if subset or false.
		*/
		bool isSubsetOf(const FMTMask& p_rhs) const;
		// DocString: FMTMask::getBitsetReference
		/**
		Get a const referencer ot the boost::dynamic_bitset data member.
		*/
		inline const boost::dynamic_bitset<uint8_t>& getBitsetReference() const
			{
			return data;
			}
		// DocString: FMTMask::getStringReference
		/**
		Get a const referencer ot the std::string data member.
		*/
		inline const std::string& getStringReference() const
		{
			return name;
		}
		// DocString: FMTMask::reserve
		/**
		@brief reserve memory for the boost dynamicbitset.
		@param[in] p_size number of bits to reserve.
		*/
		void reserve(size_t p_size);
		// DocString: FMTMask::refine
		/**
		It will return a new FMTMask where the ? are completed with the attribute for the theme from (mask).
		If the attribute is not ? for this mask, the attribute will not change. 
		*/
		FMTMask refine(const FMTMask& mask, const std::vector<FMTTheme>& themes) const;
		// DocString: FMTMask::getPostsolveMask
		/**
		If this is a base postSolve mask it uses mask to turn off bits for themes that are still active.
		*/
		FMTMask getPostsolveMask(const FMTMask& mask,const std::vector<FMTTheme>& themes) const;
		// DocString: FMTMask::presolve
		/**
		Using a FMTMaskFilter (filter) and a subset of the original FMTthemes used to construct the FMTMask,
		it returns a presolved FMTMask with potentialy less data.
		*/
		FMTMask presolve(const FMTMaskFilter& filter,const std::vector<FMTTheme>&presolvedthemes) const;
		// DocString: FMTMask::presolveRef
		/**
		@brief Using a FMTMaskFilter (filter) and a subset of the original FMTthemes used to construct the FMTMask,
		it returns a presolved FMTMask with potentialy less data.
		@param[in] p_filter
		@param[in] p_presolvedThemes
		@param[in] p_allowReallocation
		*/
		void presolveRef(const FMTMaskFilter& p_filter, const std::vector<FMTTheme>& p_presolvedThemes,bool p_allowReallocation = true);

		// DocString: FMTMask::canPresolve
		/**
		@brief Check if the mask can be presolved
		@param[in] p_filter
		@param[in] p_presolvedThemes
		@return true if you can presolve
		*/
		bool canPresolve(const FMTMaskFilter& p_filter, const std::vector<FMTTheme>& p_presolvedThemes) const;
		// DocString: FMTMask::postSolve
		/**
		Using aFMTmaskfilter (filter) and the original FMTthemes it returns a postsolved FMTMask.
		*/
		FMTMask postSolve(const FMTMaskFilter& filter, const std::vector<FMTTheme>&basethemes) const;
	private:
		friend class FMTTheme;
		// DocString: FMTMask::serialize
		/**
		Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& BOOST_SERIALIZATION_NVP(data);
			ar& BOOST_SERIALIZATION_NVP(name);
		}
		// DocString: FMTMask::subset
		/**
		Get the data subset bits for a given (theme) on the mask.
		The subset is the length of the FMTTheme.
		*/
		boost::dynamic_bitset<uint8_t> subset(const FMTTheme& theme) const;
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
		@brief Get a presolve mask
		@param[in] filter
		@param[in] p_presolvedThemes
		@return mask to presolve
		*/
		boost::dynamic_bitset<uint8_t> _getPresolveMask(const FMTMaskFilter& p_filter,
								const std::vector<FMTTheme>& p_presolvedThemes) const;
		// DocString: FMTMask::setSubset
		/**
		Set a given (subset) (theme size) for the FMTTheme (theme) to the mask
		*/
		void setSubset(const FMTTheme& theme, const boost::dynamic_bitset<uint8_t>& subset);
		// DocString: FMTMask::name
		///name of the FMTMask attributes or aggregates splitted by a space
		std::string name;
		// DocString: FMTMask::data
		///dynamic bitset holding the attributes information member.
		boost::dynamic_bitset<uint8_t> data;
    };


template<> inline void FMTMask::binarizedAppend<std::string>(const std::string& element)
	{
		size_t  location = data.size();
		data.resize(data.size() + (element.size() * 8));
		const char* charelements = element.c_str();
		for (size_t charit = 0; charit < element.size(); ++charit)
		{
			const char value = charelements[charit];
			for (int i = 7; i >= 0; --i)
			{
				data[location] = ((value & (1 << i)));
				++location;
			}
		}
	}

template<> inline void FMTMask::binarizedAppend<double>(const double& element)
{
	size_t  location = data.size();
	const int corrected = static_cast<int>(element*(1 / FMT_DBL_TOLERANCE));
	data.resize(data.size() + (sizeof(int) * 8));
	const char* charelements = reinterpret_cast<const char*>(&corrected);
	for (size_t charit = 0; charit < sizeof(int); ++charit)
	{
		const char value = charelements[charit];
		for (int i = 7; i >= 0; --i)
		{
			data[location] = ((value & (1 << i)));
			++location;
		}
	}
}

// DocString: FMTMaskComparator
/**
FMTMaskComparator to check if the mask data already exist in a std container.
*/
class FMTMaskComparator
	{
	// DocString: FMTMaskComparator::base_mask
	///The mask that we are looking for
	FMTMask base_mask;
	public:
		// DocString: FMTMaskComparator:(const FMTMask&)
		/**
		FMTMaskComparator constructor lbase_mask is the mask of that we want to match.
		*/
		FMTMaskComparator(const FMTMask& lbase_mask);
		// DocString: FMTMaskComparator::operator()(const FMTAction&)
		/**
		Matching test operator for FMTMaskComparator.
		*/
		bool operator()(const FMTMask& mask) const;

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
