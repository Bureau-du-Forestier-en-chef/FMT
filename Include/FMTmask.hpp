/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTMASK_H_INCLUDED
#define FMTMASK_H_INCLUDED

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
#include "FMTexception.hpp"
#include <vector>
#include <string>


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

			ar & BOOST_SERIALIZATION_NVP(num_bits);
			ar & BOOST_SERIALIZATION_NVP(blocks);
		}

		template <typename Ar, typename Block, typename Alloc>
		void load(Ar& ar, dynamic_bitset<Block, Alloc>& bs, unsigned) {
			size_t num_bits;
			std::vector<Block> blocks;
			ar & BOOST_SERIALIZATION_NVP(num_bits);
			ar & BOOST_SERIALIZATION_NVP(blocks);

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
#if (BOOST_VERSION / 100 % 1000) < 71
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

class FMTtheme;
class FMTmaskfilter;
// DocString: FMTmask
/**
FMTmask class is one of the most important class in FMT. Alot of classes are bases on FMTmask.
data is boost:dynamic_bitset where if a bit is set that means that the attribute of a given theme
is part of the FMTmask. the data member size == the sum of the FMTthemes size.
The name data member hold the attribute value as a string. Sometime when the FMTmask is used for union
the name might not be set. Also if multiple bits are set for a given FMTtheme it means that the mask
contains a aggregate or a ?.
*/
class FMTEXPORT FMTmask
    {

    public:
		// DocString: swap()
		/**
		Swap for FMTmask.
		*/
		void swap(FMTmask& rhs);
		// DocString: FMTmask()
		/**
		Default constructor for FMTmask.
		*/
        FMTmask();
		// DocString: FMTmask(const FMTmask&)
		/**
		FMTmask copy constructor.
		*/
		FMTmask(const FMTmask& rhs);
		// DocString: ~FMTmask()
		/**
		Default destructor for FMTmask.
		*/
        virtual ~FMTmask()=default;
		// DocString: FMTmask(const boost::dynamic_bitset<uint8_t>&)
		/**
		Simple constructor for FMTmask that only sets the bitsets data member.
		*/
        FMTmask(const boost::dynamic_bitset<uint8_t>& bits);
		// DocString: FMTmask::operator bool
		/**
		Returns true if the FMTmask is not empty by looking at the data bitset.
		*/
		explicit operator bool() const;
		// DocString: FMTmask(const std::string&,const boost::dynamic_bitset<uint8_t>&)
		/**
		FMTmask constructor using a already sets bitset (bits) and a string mask to set as name.
		*/
        FMTmask(const std::string& mask,const boost::dynamic_bitset<uint8_t>& bits);
		// DocString: FMTmask(const std::string&,const std::vector<FMTtheme>&)
		/**
		FMTmask constructor using a string (mask) and sorted themes to generate a complete FMTmask.
		*/
        FMTmask(const std::string& mask,const std::vector<FMTtheme>& themes);
		// DocString: FMTmask(const std::vector<std::string>&,const std::vector<FMTtheme>&)
		/**
		FMTmask constructor using a vector of string for the attributes and a vector of sorted themes
		to generate a FMTmask.
		*/
        FMTmask(const std::vector<std::string>& values,const std::vector<FMTtheme>& themes);
		// DocString: FMTmask(const std::vector<FMTtheme>&,const std::vector<FMTtheme>&)
		/**
		FMTmask constructor using a vector of string for the attributes and a vector of sorted themes
		to generate a FMTmask.
		*/
		FMTmask(const std::vector<FMTtheme>& themes);
		// DocString: FMTmask::clear
		/**
		Clear the data and name members.
		*/
		void clear();
		// DocString: FMTmask::size
		/**
		Returns the size of the bitsets data member.
		*/
		size_t size() const;
		// DocString: FMTmask::count
		/**
		Get the number of bits set.
		*/
		size_t count() const;
		// DocString: FMTmask::get
		/**
		Gets the string mask name of the FMTmask based on the bitset data and the sorted themes.
		*/
        std::string get(const std::vector<FMTtheme>& themes) const;
		// DocString: FMTmask::get
		/**
		Get the corresponding attribute of the FMTmask for a given FMTtheme.
		*/
		std::string get(const FMTtheme& theme) const;
		// DocString: FMTmask::getsubsetcount
		/**
		Get the number of bits set for the given theme subset.
		*/
		size_t getsubsetcount(const FMTtheme& theme) const;
		// DocString: FMTmask::set
		/**
		Set the bitset data for some FMTthemes (themes) for a mask string (value).
		*/
        void set(const std::vector<FMTtheme>& themes,const std::string& value);
		// DocString: FMTmask::set
		/**
		Set the bitset data for a given FMTtheme (theme) for a attribute (value).
		*/
		void set(const FMTtheme& theme, const std::string& value);
		// DocString: FMTmask::getstaticthemes
		/**
		Filter FMTtheme from themes that are only using aggregates or ? for the FMTmask.
		*/
		std::vector<FMTtheme>getstaticthemes(const std::vector<FMTtheme>& themes) const;
		// DocString: FMTmask::getselectedthemes
		/**
		Takes vector of FMTtheme and return a vector of FMTtheme from which theme have any true bits.
		*/
		std::vector<FMTtheme> getselectedthemes(const  std::vector<FMTtheme>& themes) const;
		// DocString: FMTmask::isnotthemessubset
		/**
		By looking at each FMTtheme in the FMTmask return true if *this is not part of rhs base on sorted FMTtheme (themes).
		*/
		bool isnotthemessubset(const FMTmask& rhs, const  std::vector<FMTtheme>& themes) const;
		// DocString: FMTmask::removeaggregates
		/**
		For each FMTtheme in (themes) if the attribute is an aggregate or ? will unset all
		bits of the given aggregate or ? and return a new FMTmask.
		*/
		FMTmask removeaggregates(const std::vector<FMTtheme>& themes,bool questionmarkonly=false) const;
		// DocString: FMTmask::empty
		/**
		Returns true if the data member is empty.
		*/
		bool empty() const;
		// DocString: FMTmask::update
		/**
		Update the name data member bass on the dynamic bitset data member with a sorted list of FMTtheme (themes).
		*/
        void update(const std::vector<FMTtheme>& themes);
		// DocString: FMTmask::decompose
		/**
		Using a given (theme) this function desaggregate the attribute/aggregate/? of a theme from the FMTmask
		and returns a vector of each possible attribute as multiple masks.
		*/
		std::vector<FMTmask>decompose(const FMTtheme &theme) const;
		// DocString: FMTmask::append
		/**
		Append a bitsets to the bitset data member of the FMTmask.
		*/
        void append(const boost::dynamic_bitset<uint8_t> &bits);
		// DocString: FMTmask::binarizedappend
		/**
		Binarize any class and append it to the mask.
		*/
		template <class typetobinarize>
		void binarizedappend(const typetobinarize& element)
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
		// DocString: FMTmask::getunion
		/**
		Do a union operation on the dynamic bitset of the FMTmask and the given FMTmask (rhs).
		*/
		FMTmask getunion(const FMTmask& rhs) const;
		// DocString: FMTmask::getintersect
		/**
		Do a intersection operation on the dynamic bitset of the FMTmask and the given FMTmask (rhs).
		*/
		FMTmask getintersect(const FMTmask& rhs) const;
		// DocString: FMTmask::getbitsetintersect
		/**
		Do a intersection operation on the dynamic bitset of the FMTmask and the given FMTmask (rhs)
		but return the bitset.
		*/
		boost::dynamic_bitset<uint8_t> getbitsetintersect(const FMTmask& rhs) const;
		// DocString: FMTmask::operator=
		/**
		FMTmask copy assignment operator.
		 */
        FMTmask& operator = (const FMTmask& rhs);
		// DocString: FMTmask::operator!=
		/**
		FMTmask nonequality operator.
		*/
        bool operator != (const FMTmask& rhs) const;
		// DocString: FMTmask::operator==
		/**
		Comparison operator of FMTmask.
		*/
        bool operator == (const FMTmask& rhs) const;
		// DocString: FMTmask::operator<
		/**
		Less than operator of FMTmask.
		*/
        bool operator < (const FMTmask& rhs) const;
		// DocString: FMTmask::resume
		/**
		Using a mask (rhs) fix the corresponding resulting FMTmask with the selected name and data.
		*/
		FMTmask resume(const boost::dynamic_bitset<uint8_t>& rhs) const;
		// DocString: FMTmask::resume
		/**
		Using a mask indexes (indexes) fix the corresponding resulting FMTmask with the selected name and data.
		*/
		FMTmask resume(const std::vector<size_t>& indexes) const;
		// DocString: FMTmask::hash
		/**
		Hash function for the FMTmask class.
		*/
		inline size_t hash() const
			{
			return boost::hash<boost::dynamic_bitset<uint8_t>>()(data);
			}
		// DocString: FMTmask::getbitsstring
		/**
		Returns the binary representation of the dynamic bitset in a simple string.
		*/
        std::string getbitsstring() const;
		// DocString: FMTmask::operator std::string
		/**
		Returns the name of the FMTmask.
		*/
		inline operator std::string() const
			{
			return name;
			}
		// DocString: FMTmask::issubsetof
		/**
		Check if the bitset is a subset of the rhs bitset
		*/
		inline bool issubsetof(const boost::dynamic_bitset<uint8_t>& rhs) const
			{
			return data.is_subset_of(rhs);
			}
		// DocString: FMTmask::issubsetof
		/**
		Check if the FMTmask is a subset of a FMTmask (rhs).
		*/
		inline bool issubsetof(const FMTmask& rhs) const
			{
			return data.is_subset_of(rhs.data);
			}
		// DocString: FMTmask::getbitsetreference
		/**
		Get a const referencer ot the boost::dynamic_bitset data member.
		*/
		inline const boost::dynamic_bitset<uint8_t>& getbitsetreference() const
			{
			return data;
			}
		// DocString: FMTmask::getstringreference
		/**
		Get a const referencer ot the std::string data member.
		*/
		inline const std::string& getstringreference() const
		{
			return name;
		}
		// DocString: FMTmask::refine
		/**
		It will return a new FMTmask where the ? are completed with the attribute for the theme from (mask).
		If the attribute is not ? for this mask, the attribute will not change. 
		*/
		FMTmask refine(const FMTmask& mask, const std::vector<FMTtheme>& themes) const;
		// DocString: FMTmask::getpostsolvemask
		/**
		If this is a base postsolve mask it uses mask to turn off bits for themes that are still active.
		*/
		FMTmask getpostsolvemask(const FMTmask& mask,const std::vector<FMTtheme>& themes) const;
		// DocString: FMTmask::presolve
		/**
		Using a FMTmaskfilter (filter) and a subset of the original FMTthemes used to construct the FMTmask,
		it returns a presolved FMTmask with potentialy less data.
		*/
		FMTmask presolve(const FMTmaskfilter& filter,const std::vector<FMTtheme>&presolvedthemes) const;
		// DocString: FMTmask::postsolve
		/**
		Using aFMTmaskfilter (filter) and the original FMTthemes it returns a postsolved FMTmask.
		*/
		FMTmask postsolve(const FMTmaskfilter& filter, const std::vector<FMTtheme>&basethemes) const;
	private:
		friend class FMTtheme;
		// DocString: FMTmask::serialize
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
		// DocString: FMTmask::subset
		/**
		Get the data subset bits for a given (theme) on the mask.
		The subset is the length of the FMTtheme.
		*/
		boost::dynamic_bitset<uint8_t> subset(const FMTtheme& theme) const;
		// DocString: FMTmask::setsubset
		/**
		Set a given (subset) (theme size) for the FMTtheme (theme) to the mask
		*/
		void setsubset(const FMTtheme& theme, const boost::dynamic_bitset<uint8_t>& subset);
		// DocString: FMTmask::name
		///name of the FMTmask attributes or aggregates splitted by a space
		std::string name;
		// DocString: FMTmask::data
		///dynamic bitset holding the attributes information member.
		boost::dynamic_bitset<uint8_t> data;
    };


template<> inline void FMTmask::binarizedappend<std::string>(const std::string& element)
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

template<> inline void FMTmask::binarizedappend<double>(const double& element)
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

// DocString: FMTmaskcomparator
/**
FMTmaskcomparator to check if the mask data already exist in a std container.
*/
class FMTmaskcomparator
	{
	// DocString: FMTmaskcomparator::base_mask
	///The mask that we are looking for
	FMTmask base_mask;
	public:
		// DocString: FMTmaskcomparator:(const FMTmask&)
		/**
		FMTmaskcomparator constructor lbase_mask is the mask of that we want to match.
		*/
		FMTmaskcomparator(const FMTmask& lbase_mask);
		// DocString: FMTmaskcomparator::operator()(const FMTaction&)
		/**
		Matching test operator for FMTmaskcomparator.
		*/
		bool operator()(const FMTmask& mask) const;

	};

}

namespace boost {
/**
boost hash for FMTmask
*/
  template <>
  struct hash<Core::FMTmask>
  {
    std::size_t operator()(const Core::FMTmask& mask) const
    {
      return (mask.hash());
    }
  };

}

BOOST_CLASS_EXPORT_KEY(Core::FMTmask)

#endif // FMTMASK_H_INCLUDED
