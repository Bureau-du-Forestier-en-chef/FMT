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
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/hash_collections_save_imp.hpp>
#include <boost/serialization/hash_collections_load_imp.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/nvp.hpp>
#include <iterator>

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
			ar & BOOST_SERIALIZATION_NVP(filter);
			std::vector<std::pair<FMTmask, std::vector<int>>>vecfastpass(fastpass.begin(), fastpass.end());
			ar & BOOST_SERIALIZATION_NVP(vecfastpass);
			}
		template<class Archive>
		void load(Archive& ar, const unsigned int version)
			{
			ar & BOOST_SERIALIZATION_NVP(data);
			ar & BOOST_SERIALIZATION_NVP(filter);
			std::vector<std::pair<FMTmask, std::vector<int>>>vecfastpass;
			ar & BOOST_SERIALIZATION_NVP(vecfastpass);
			for (const std::pair<FMTmask, std::vector<int>>& values : vecfastpass)
				{
				fastpass[values.first] = values.second;
				}
			}
		BOOST_SERIALIZATION_SPLIT_MEMBER()
		std::vector<std::pair<FMTmask, T>>data;
		FMTmaskfilter filter;
		mutable boost::unordered_map<FMTmask, std::vector<int>>fastpass;
	protected:
		/**
		Using a basemask reprensenting the whole forest landscape this function will
		attempt to reduce the number of elements in the list knowing that if the element
		represent something that is not in the basemask this element could be deleted.
		Also using a presolvecmask representing
		Use this function with care because it's going to change the stade of the list
		if user attempt to reference to a deleted element the model will seems broken.
		*/
		void presolvelist(const FMTmask& basemask,
			const std::vector<FMTtheme>& originalthemes,
			const FMTmask& presolvedmask,
			const std::vector<FMTtheme>& newthemes)
			{
				if (!canshrink())
					{
					unshrink(originalthemes);
					}
				std::vector<std::pair<FMTmask, T>>newdata;
				for (const std::pair<FMTmask, T>& object : data)
					{
					if (!object.first.isnotthemessubset(basemask, originalthemes))
						{
						FMTmask mskkey = object.first;
						if (!presolvedmask.empty())
							{
							mskkey = mskkey.presolve(presolvedmask, newthemes);
							}
						newdata.push_back(std::pair<FMTmask,T>(mskkey, object.second));
						}
					}
				data = newdata;
				this->update();
			}
	public:
		FMTmask getunion(const std::vector<FMTtheme>& themes) const
			{
				Core::FMTmask testedmask(std::string(this->begin()->first), themes);
				for (const auto& object : *this)
				{
					const Core::FMTmask specificiermask(std::string(object.first), themes);
					testedmask = testedmask.getunion(specificiermask);
				}
				return testedmask;
			}
		FMTlist() :
			data(),
			filter(),
			fastpass(){};
		FMTlist(const FMTlist<T>& rhs) :
			data(rhs.data),
			filter(rhs.filter),
			fastpass(rhs.fastpass)
		{

		}
		FMTlist& operator = (const FMTlist<T>& rhs)
		{
			if (this != &rhs)
			{
				data = rhs.data;
				filter = rhs.filter;
				fastpass = rhs.fastpass;
			}
			return *this;
		}

		bool operator == (const FMTlist<T>& rhs) const
			{
			return (data == rhs.data);
			}

		virtual ~FMTlist() = default;
		bool empty() const
			{
			return data.empty();
			}
		bool canshrink() const
			{
			return filter.empty();
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
						allhits.push_back(&data.at(location).second);
					}
				}
				else {
					fastpass[newkey] = std::vector<int>();
					int location = 0;
					for (const std::pair<FMTmask,T>& object : data)
					{
						if (newkey.issubsetof(object.first))
						{
							fastpass[newkey].push_back(location);
							allhits.push_back(&object.second);
						}
						++location;
					}
					}
				}
			return allhits;
		}

		inline FMTmask filtermask(const FMTmask& basemask) const
			{
			return filter.filter(basemask);
			}

		void shrink()
		{
				std::vector<std::pair<FMTmask, T>>newdata;
				fastpass.clear();
				for (const std::pair<FMTmask, T>& object : data)
				{
					newdata.push_back(std::pair<FMTmask, T>(filter.filter(object.first), object.second));
				}
				data = newdata;
		}

		void unshrink(const std::vector<FMTtheme>& themes)
		{
			std::vector<std::pair<FMTmask, T>>newdata;
			fastpass.clear();
			filter = FMTmaskfilter();
			for (const std::pair<FMTmask, T>& object : data)
				{
				newdata.push_back(std::pair<FMTmask,T>(FMTmask(std::string(object.first), themes), object.second));
				}
			data = newdata;

		}

		

		void push_back(const FMTmask& mask, const T& value)
		{
			data.emplace_back(mask,value);
		}
		virtual void update()
			{
				if (canshrink())
					{
					shrink();
					}
			}
		
		void push_back(const std::pair<FMTmask,T>& value)
			{
			data.emplace_back(value);
			}

		void push_back(const FMTlist<T>& rhs)
		{
			if (this->canshrink() && rhs.canshrink())
			{
				for (const std::pair<FMTmask, T>& object : rhs.data)
				{
					this->push_back(object);
				}
				this->shrink();
			}
		}

		void push_front(const FMTmask& mask, const T& value)
		{
			data.emplace(data.begin(), mask, value);
		}
		void push_front(const std::pair<FMTmask, T>& value)
		{
			data.emplace(data.begin(), value);
		}

		void pop_back()
            {
			data.pop_back();
            }
		void erase(const size_t& location)
            {
			data.erase(data.begin() + location);
            }

        void insert(const size_t& location,const FMTmask& mask, const T& value)
            {
			data.insert(data.begin() + location,std::pair<FMTmask,T>(mask,value));
            }

		void insert(const size_t& location,const std::pair<FMTmask, T>& value)
		{
			data.insert(data.begin() + location, value);
		}
		typedef typename std::vector<std::pair<FMTmask, T>>::value_type value_type;
		typedef typename std::vector<std::pair<FMTmask, T>>::iterator iterator;
		typedef typename std::vector<std::pair<FMTmask, T>>::const_iterator const_iterator;
		void append(FMTlist<T>::value_type element)
			{
			data.push_back(filtermask(element->first), element->second);
			}
		iterator begin()
			{
			return data.begin();
			}

		const_iterator begin() const
			{
			return data.begin();
			}

		iterator  end()
			{
			return data.end();
			}

		const_iterator end() const
			{
			return data.end();
			}

	};

}

#endif
