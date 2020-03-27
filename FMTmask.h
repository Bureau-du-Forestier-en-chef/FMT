/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
#include "FMTexception.h"
#include <vector>
#include <string>


/**
Function part of boost and serialization needed to serialize boost::dynamic_bitset<>.
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
The boost::dynamic_bitset<> hashing was missing the boost version older thant 1.71
*/
#if (BOOST_VERSION / 100 % 1000) < 71
namespace boost {
	template <typename Block, typename Alloc>
	std::size_t hash_value(const boost::dynamic_bitset<Block, Alloc>& bs)
	{
		return boost::hash_value(bs.m_bits);
	}
}
#endif

namespace Core
{

class FMTtheme;

/**
FMTmask class is one of the most important class in FMT. Alot of classes are bases on FMTmask.
data is boost:dynamic_bitset where if a bit is set that means that the attribute of a given theme
is part of the FMTmask. the data member size == the sum of the FMTthemes size.
The name data member hold the attribute value as a string. Sometime when the FMTmask is used for union
the name might not be set. Also if multiple bits are set for a given FMTtheme it means that the mask
contains a aggregate or a ?.
*/
class FMTmask
    {
    friend class FMTtheme;
	/**
	Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		ar & BOOST_SERIALIZATION_NVP(data);
		ar & BOOST_SERIALIZATION_NVP(name);
		}
		/**
		Get the data subset bits for a given (theme) on the mask.
		The subset is the length of the FMTtheme.
		*/
        boost::dynamic_bitset<> subset(const FMTtheme& theme) const;
		/**
		Set a given (subset) (theme size) for the FMTtheme (theme) to the mask
		*/
        void setsubset(const FMTtheme& theme,const boost::dynamic_bitset<>& subset);
		///name of the FMTmask attributes or aggregates splitted by a space
		std::string name;
		///dynamic bitset holding the attributes information member.
		boost::dynamic_bitset<> data;
    public:
		/**
		Default constructor for FMTmask.
		*/
        FMTmask();
		/**
		FMTmask copy constructor.
		*/
		FMTmask(const FMTmask& rhs);
		/**
		Default destructor for FMTmask.
		*/
        virtual ~FMTmask()=default;
		/**
		Simple constructor for FMTmask that only sets the bitsets data member.
		*/
        FMTmask(const boost::dynamic_bitset<>& bits);
		/**
		Returns true if the FMTmask is not empty by looking at the data bitset.
		*/
		explicit operator bool() const;
		/**
		FMTmask constructor using a already sets bitset (bits) and a string mask to set as name.
		*/
        FMTmask(const std::string& mask,const boost::dynamic_bitset<>& bits);
		/**
		FMTmask constructor using a string (mask) and sorted themes to generate a complete FMTmask.
		*/
        FMTmask(const std::string& mask,const std::vector<FMTtheme>& themes);
		/**
		FMTmask constructor using a vector of string for the attributes and a vector of sorted themes
		to generate a FMTmask.
		*/
        FMTmask(const std::vector<std::string>& values,const std::vector<FMTtheme>& themes);
		/**
		Clear the data and name members.
		*/
		void clear();
		/**
		Returns the size of the bitsets data member.
		*/
		size_t size() const;
		/**
		Gets the string mask name of the FMTmask based on the bitset data and the sorted themes.
		*/
        std::string get(const std::vector<FMTtheme>& themes) const;
		/**
		Get the corresponding attribute of the FMTmask for a given FMTtheme.
		*/
		std::string get(const FMTtheme& theme) const;
		/**
		Set the bitset data for some FMTthemes (themes) for a mask string (value).
		*/
        void set(const std::vector<FMTtheme>& themes,const std::string& value);
		/**
		Set the bitset data for a given FMTtheme (theme) for a attribute (value).
		*/
		void set(const FMTtheme& theme, const std::string& value);
		/**
		Filter FMTtheme from themes that are only using aggregates or ? for the FMTmask.
		*/
		std::vector<FMTtheme>getstaticthemes(const std::vector<FMTtheme>& themes) const;
		/**
		By looking at each FMTtheme in the FMTmask return true if *this is not part of rhs base on sorted FMTtheme (themes).
		*/
		bool isnotthemessubset(const FMTmask& rhs, const  std::vector<FMTtheme>& themes) const;
		/**
		For each FMTtheme in (themes) if the attribute is an aggregate or ? will unset all
		bits of the given aggregate or ? and return a new FMTmask.
		*/
		FMTmask removeaggregates(const std::vector<FMTtheme>& themes) const;
		/**
		Returns true if the data member is empty.
		*/
		bool empty() const;
		/**
		Update the name data member bass on the dynamic bitset data member with a sorted list of FMTtheme (themes).
		*/
        void update(const std::vector<FMTtheme>& themes);
		/**
		Using a given (theme) this function desaggregate the attribute/aggregate/? of a theme from the FMTmask
		and returns a vector of each possible attribute as multiple masks.
		*/
		std::vector<FMTmask>decompose(const FMTtheme &theme) const;
		/**
		Append a bitsets to the bitset data member of the FMTmask.
		*/
        void append(const boost::dynamic_bitset<> &bits);
       // bool linkNvalidate(const std::vector<FMTtheme>& themes);
		/**
		Do a union operation on the dynamic bitset of the FMTmask and the given FMTmask (rhs).
		*/
		FMTmask getunion(const FMTmask& rhs) const;
		/**
		FMTmask copy assignment operator.
		 */
        FMTmask& operator = (const FMTmask& rhs);
		/**
		FMTmask nonequality operator.
		*/
        bool operator != (const FMTmask& rhs) const;
		/**
		Comparison operator of FMTmask.
		*/
        bool operator == (const FMTmask& rhs) const;
		/**
		Less than operator of FMTmask.
		*/
        bool operator < (const FMTmask& rhs) const;
		/**
		Using a mask (rhs) returns the corresponding resulting FMTmask with the selected name and data.
		*/
        FMTmask resume(const boost::dynamic_bitset<>& rhs) const;
		/**
		Hash function for the FMTmask class.
		*/
		inline size_t hash() const
			{
			return boost::hash_value(data.m_bits);
			}
		/**
		Returns the binary representation of the dynamic bitset in a simple string.
		*/
        std::string getbitsstring() const;
		/**
		Returns the name of the FMTmask.
		*/
		inline operator std::string() const
			{
			return name;
			}
		/**
		Check if the bitset is a subset of the rhs bitset
		*/
		inline bool issubsetof(const boost::dynamic_bitset<>& rhs) const
			{
			return data.is_subset_of(rhs);
			}
		/**
		Check if the FMTmask is a subset of a FMTmask (rhs).
		*/
		inline bool issubsetof(const FMTmask& rhs) const
			{
			return data.is_subset_of(rhs.data);
			}
		/**
		Get a const referencer ot the boost::dynamic_bitset data member.
		*/
		inline const boost::dynamic_bitset<>& getbitsetreference() const
			{
			return data;
			}
		/**
		Using a FMTmask (selectedmask) and a subset of the original FMTthemes used to construct the FMTmask,
		it returns a presolved FMTmask with potentialy less data. 
		*/
		FMTmask presolve(const FMTmask& selectedmask,const std::vector<FMTtheme>&presolvedthemes) const;
		/**
		Using a FMTmask (selectedmask) and the original FMTthemes it returns a postsolved FMTmask.
		*/
		FMTmask postsolve(const FMTmask& selectedmask, const std::vector<FMTtheme>&basethemes) const;
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

BOOST_CLASS_EXPORT_KEY(Core::FMTmask);

#endif // FMTMASK_H_INCLUDED
