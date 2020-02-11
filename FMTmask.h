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
#include "FMTexception.h"
#include <vector>
#include <string>
#include "FMTtheme.h"


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


class FMTmask
    {
    friend class FMTtheme;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		ar & BOOST_SERIALIZATION_NVP(data);
		ar & BOOST_SERIALIZATION_NVP(name);
		}
        boost::dynamic_bitset<> subset(const FMTtheme& theme) const;
        bool setsubset(const FMTtheme& theme,const boost::dynamic_bitset<>& subset);
		std::string name;
		boost::dynamic_bitset<> data;
    public:
        FMTmask();
        virtual ~FMTmask()=default;
        FMTmask(const boost::dynamic_bitset<>& bits);
		explicit operator bool() const;
        FMTmask(const std::string& mask,const boost::dynamic_bitset<>& bits);
        FMTmask(const std::string& mask,const std::vector<FMTtheme>& themes);
        FMTmask(const std::vector<std::string>& values,const std::vector<FMTtheme>& themes);
        std::string get(const std::vector<FMTtheme>& themes) const;
		std::string getstr() const;
        bool set(const std::vector<FMTtheme>& themes,const std::string& value);
		std::vector<FMTtheme>getstaticthemes(const std::vector<FMTtheme>& themes) const;
        std::string get(const FMTtheme& theme) const;
		bool isnotthemessubset(const FMTmask& rhs, const  std::vector<FMTtheme>& themes) const;
		bool empty() const;
        bool set(const FMTtheme& theme, const std::string& value);
        void update(const std::vector<FMTtheme>& themes);
		std::vector<FMTmask>decompose(const FMTtheme &theme) const;
        void append(const boost::dynamic_bitset<> &bits);
        bool linkNvalidate(const std::vector<FMTtheme>& themes);
        FMTmask(const FMTmask& rhs);
        FMTmask& operator = (const FMTmask& rhs);
        bool operator != (const FMTmask& rhs) const;
        bool operator == (const FMTmask& rhs) const;
        bool operator < (const FMTmask& rhs) const;
        FMTmask resume(const boost::dynamic_bitset<>& rhs) const;
		inline size_t hash() const
			{
			return boost::hash_value(data.m_bits);
			}
        std::string to_string() const;
		operator std::string() const;
		inline bool issubsetof(const boost::dynamic_bitset<>& rhs) const
			{
			return data.is_subset_of(rhs);
			}
		inline bool issubsetof(const FMTmask& rhs) const
			{
			return data.is_subset_of(rhs.data);
			}
		inline const boost::dynamic_bitset<>& getbitsetreference() const
			{
			return data;
			}
    };

}

namespace boost {

  template <>
  struct hash<Core::FMTmask>
  {
    std::size_t operator()(const Core::FMTmask& mask) const
    {
      return (mask.hash());
    }
  };




}

#endif // FMTMASK_H_INCLUDED
