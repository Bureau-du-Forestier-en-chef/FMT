#ifndef FMTlist_H_INCLUDED
#define FMTlist_H_INCLUDED
/*
#define BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS
#include <boost/python.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/python/stl_iterator.hpp>
#include <boost/dynamic_bitset.hpp>*/

#include <boost/unordered_map.hpp>
#include <functional>
#include <vector>
#include <utility>

#include "FMTmask.h"
#include "FMTmaskfilter.h"
#include <iterator>
#include <boost/serialization/serialization.hpp>
//#include <boost/serialization/hash_map.hpp>
#include <boost/serialization/hash_collections_save_imp.hpp>
#include <boost/serialization/hash_collections_load_imp.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/nvp.hpp>



using namespace boost::python;
using namespace std;
using namespace Exception;



namespace Core
{

	template<typename T>
	class FMTlist
	{
		friend class boost::serialization::access;
		template<class Archive>
		void save(Archive& ar, const unsigned int version) const
			{
			ar & BOOST_SERIALIZATION_NVP(data);
			ar & BOOST_SERIALIZATION_NVP(masks);
			ar & BOOST_SERIALIZATION_NVP(filter);
			vector<pair<FMTmask, vector<int>>>vecfastpass(fastpass.begin(), fastpass.end());
			ar & BOOST_SERIALIZATION_NVP(vecfastpass);
			}
		template<class Archive>
		void load(Archive& ar, const unsigned int version)
			{
			ar & BOOST_SERIALIZATION_NVP(data);
			ar & BOOST_SERIALIZATION_NVP(masks);
			ar & BOOST_SERIALIZATION_NVP(filter);
			vector<pair<FMTmask, vector<int>>>vecfastpass;
			ar & BOOST_SERIALIZATION_NVP(vecfastpass);
			for (const pair<FMTmask, vector<int>>& values : vecfastpass)
				{
				fastpass[values.first] = values.second;
				}
			}
		BOOST_SERIALIZATION_SPLIT_MEMBER()
		vector<T>data;
		vector<FMTmask>masks;
		FMTmaskfilter filter;
		mutable boost::unordered_map<FMTmask, vector<int>>fastpass;
	public:
		FMTlist() :
			data(),
			masks(),
			filter(),
			fastpass() {};
		FMTlist(const FMTlist<T>& rhs) :
			data(rhs.data),
			masks(rhs.masks),
			filter(rhs.filter),
			fastpass(rhs.fastpass)
		{

		}
		FMTlist& operator = (const FMTlist<T>& rhs)
		{
			if (this != &rhs)
			{
				data = rhs.data;
				masks = rhs.masks;
				filter = rhs.filter;
				fastpass = rhs.fastpass;
			}
			return *this;
		}

		bool operator == (const FMTlist<T>& rhs) const
			{
			return (data == rhs.data &&
				masks == rhs.masks);
			}

		virtual ~FMTlist() = default;
		bool empty() const
		{
			return data.empty();
		}
		vector<FMTmask>getmasklist() const
		{
			return masks;
		}
		vector<T>getdatalist() const
		{
			return data;
		}
		size_t size() const
		{
			return data.size();
		}
		vector<const T*> findsets(const FMTmask& mask) const
		{
			vector<const T*>allhits;
			FMTmask newkey = filter.filter(mask);
			if (!newkey.empty())
				{
				boost::unordered_map<FMTmask, vector<int>>::const_iterator fast_it = fastpass.find(newkey);
				if (fast_it != fastpass.end())
				{
					//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " using fast pass!! " << "\n";
					allhits.reserve(fast_it->second.size());
					for (const int& location : fast_it->second)
					{
						allhits.push_back(&data.at(location));
					}
				}
				else {
					fastpass[newkey] = vector<int>();
					int location = 0;
					for (const FMTmask& mask : masks)
					{
						if (newkey.data.is_subset_of(mask.data))
						{
							fastpass[newkey].push_back(location);
							allhits.push_back(&data.at(location));
						}
						++location;
					}
					}
				}
			return allhits;
		}

		FMTmask filtermask(const FMTmask& basemask) const
			{
			return filter.filter(basemask);
			}

		void shrink()
		{
			vector<FMTmask>newmasks;
			fastpass.clear();
			for (const FMTmask& mask : masks)
			{
				newmasks.push_back(filter.filter(mask));
			}
			masks = newmasks;
		}
		void push_back(const FMTmask& mask, const T& value)
		{
			data.push_back(value);
			masks.push_back(mask);
		}
		void push_front(const FMTmask& mask, const T& value)
		{
			data.insert(data.begin(),value);
			masks.insert(masks.begin(),mask);
		}
		void pop_back()
            {
			data.pop_back();
			masks.pop_back();
            }
		void erase(const size_t& location)
            {
            data.erase(data.begin()+location);
			masks.erase(masks.begin()+location);
            }

        void insert(const size_t& location,const FMTmask& mask, const T& value)
            {
            data.insert(data.begin()+location,value);
            masks.insert(masks.begin()+location,mask);
            }

		typename vector<FMTmask>::const_iterator maskbegin() const
		{
			return masks.begin();
		}
		typename vector<FMTmask>::const_iterator maskend() const
		{
			return masks.end();
		}
		typename vector<T>::const_iterator databegin() const
		{
			return data.begin();
		}
		typename vector<T>::const_iterator dataend() const
		{
			return data.end();
		}
		typename vector<FMTmask>::iterator maskbegin()
		{
			return masks.begin();
		}
		typename vector<FMTmask>::iterator maskend()
		{
			return masks.end();
		}
		typename vector<T>::iterator databegin()
		{
			return data.begin();
		}
		typename vector<T>::iterator dataend()
		{
			return data.end();
		}
	};

}

#endif
