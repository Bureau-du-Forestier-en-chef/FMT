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

#ifndef FMTlist_H_INCLUDED
#define FMTlist_H_INCLUDED

#include <boost/unordered_map.hpp>
#include <functional>
#include <vector>
#include <utility>

#include "FMTmask.h"
#include "FMTmaskfilter.h"
#include <iterator>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/hash_collections_save_imp.hpp>
#include <boost/serialization/hash_collections_load_imp.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/nvp.hpp>


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
			std::vector<std::pair<FMTmask, std::vector<int>>>vecfastpass(fastpass.begin(), fastpass.end());
			ar & BOOST_SERIALIZATION_NVP(vecfastpass);
			}
		template<class Archive>
		void load(Archive& ar, const unsigned int version)
			{
			ar & BOOST_SERIALIZATION_NVP(data);
			ar & BOOST_SERIALIZATION_NVP(masks);
			ar & BOOST_SERIALIZATION_NVP(filter);
			std::vector<std::pair<FMTmask, std::vector<int>>>vecfastpass;
			ar & BOOST_SERIALIZATION_NVP(vecfastpass);
			for (const std::pair<FMTmask, std::vector<int>>& values : vecfastpass)
				{
				fastpass[values.first] = values.second;
				}
			}
		BOOST_SERIALIZATION_SPLIT_MEMBER()
		std::vector<T>data;
		std::vector<FMTmask>masks;
		FMTmaskfilter filter;
		mutable boost::unordered_map<FMTmask, std::vector<int>>fastpass;
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
		std::vector<FMTmask>getmasklist() const
		{
			return masks;
		}
		std::vector<T>getdatalist() const
		{
			return data;
		}
		size_t size() const
		{
			return data.size();
		}
		std::vector<const T*> findsets(const FMTmask& mask) const
		{
			std::vector<const T*>allhits;
			const FMTmask newkey = filter.filter(mask);
			if (!newkey.empty())
				{
				boost::unordered_map<FMTmask, std::vector<int>>::const_iterator fast_it = fastpass.find(newkey);
				if (fast_it != fastpass.end())
				{
					allhits.reserve(fast_it->second.size());
					for (const int& location : fast_it->second)
					{
						allhits.push_back(&data.at(location));
					}
				}
				else {
					fastpass[newkey] = std::vector<int>();
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
			std::vector<FMTmask>newmasks;
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

		typename std::vector<FMTmask>::const_iterator maskbegin() const
		{
			return masks.begin();
		}
		typename std::vector<FMTmask>::const_iterator maskend() const
		{
			return masks.end();
		}
		typename std::vector<T>::const_iterator databegin() const
		{
			return data.begin();
		}
		typename std::vector<T>::const_iterator dataend() const
		{
			return data.end();
		}
		typename std::vector<FMTmask>::iterator maskbegin()
		{
			return masks.begin();
		}
		typename std::vector<FMTmask>::iterator maskend()
		{
			return masks.end();
		}
		typename std::vector<T>::iterator databegin()
		{
			return data.begin();
		}
		typename std::vector<T>::iterator dataend()
		{
			return data.end();
		}
	};

}

#endif
