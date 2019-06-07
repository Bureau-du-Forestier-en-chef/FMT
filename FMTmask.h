#ifndef FMTMASK_H_INCLUDED
#define FMTMASK_H_INCLUDED

#define BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS

#include <boost/python.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/unordered_map.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/python/stl_iterator.hpp>
#include <boost/functional/hash.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/nvp.hpp>
#include "FMTexception.h"
#include "FMTtheme.h"

using namespace boost::python;
using namespace std;
using namespace Exception;
/*
namespace std {

    template <typename Block, typename Alloc> struct hash<boost::dynamic_bitset<Block, Alloc> > {
        size_t operator()(boost::dynamic_bitset<Block, Alloc> const& bs) const {
            size_t seed = boost::hash_value(bs.size());

            std::vector<Block> blocks(bs.num_blocks());
            boost::hash_range(seed, blocks.begin(), blocks.end());

            return seed;
        }
    };

}*/

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
        bool setsubset(const FMTtheme& theme,boost::dynamic_bitset<>& subset);
        string name;
    public:
        boost::dynamic_bitset<> data;
        FMTmask();
        virtual ~FMTmask()=default;
        FMTmask(boost::dynamic_bitset<> bits);
		explicit operator bool() const;
        FMTmask(string mask,boost::dynamic_bitset<> bits);
        FMTmask(string mask,const vector<FMTtheme>& themes);
        FMTmask(const vector<string>& values,const vector<FMTtheme>& themes);
        string get(vector<FMTtheme>& themes) const;
        string getstr() const;
        bool set(const vector<FMTtheme>& themes,string value);
        string get(const FMTtheme& theme) const;
		bool empty() const;
        bool set(const FMTtheme& theme,string value);
        void update(const vector<FMTtheme>& themes);
        vector<FMTmask>decompose(const FMTtheme &theme) const;
        void append(boost::dynamic_bitset<> &bits);
        bool linkNvalidate(vector<FMTtheme>& themes);
        FMTmask(const FMTmask& rhs);
        FMTmask& operator = (const FMTmask& rhs);
        bool operator != (const FMTmask& rhs) const;
        bool operator == (const FMTmask& rhs) const;
        bool operator < (const FMTmask& rhs) const;
        FMTmask resume(const boost::dynamic_bitset<>& rhs) const;
        string to_string() const;
		operator string() const;
    };

}

namespace boost {
    template <typename Block, typename Alloc>
    std::size_t hash_value(const boost::dynamic_bitset<Block, Alloc>& bs)
        {
        /*size_t seed = boost::hash_value(bs.size());
        std::vector<Block> blocks(bs.num_blocks());
        boost::hash_range(seed, blocks.begin(), blocks.end());
        return seed;*/
		size_t seed = 0;// (1 << bs.size());
		for (size_t i = 0; i < bs.size(); i++)
			{
			boost::hash_combine(seed, bs[i]);
			//seed = seed | (bs[i] << (bs.size() - i - 1));
			}
		return seed;
        }
}

namespace boost {

  template <>
  struct hash<Core::FMTmask>
  {
    std::size_t operator()(const Core::FMTmask& mask) const
    {
      return (hash<boost::dynamic_bitset<>>()(mask.data));
    }
  };




}

#endif // FMTMASK_H_INCLUDED
