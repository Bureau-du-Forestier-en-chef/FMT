/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTlist_H_INCLUDED
#define FMTlist_H_INCLUDED

#include <boost/unordered_map.hpp>
#include <functional>
#include <vector>
#include <utility>

#include "FMTmask.hpp"
#include "FMTmaskfilter.hpp"
#include <boost/serialization/version.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/hash_collections_save_imp.hpp>
#include <boost/serialization/hash_collections_load_imp.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include "FMTobject.hpp"
#include <iterator>
#include <memory>
#include "FMTyieldhandler.hpp"

namespace Core
{
	// DocString: FMTlist
	/**
	The FMTlist class is made to keep track of objects with a FMTmask tag like dictionary. The yield, action and transition class
	uses this class to list objects based on the tagged FMTmask and the position of the object in the list.
	FMTlist uses caching and a mask filter to reduce the size of the list and gives a fast access to objects present in
	the list.
	*/
	template<typename T>
	class FMTlist: public FMTobject
	{
		// DocString: FMTlist::save
		/**
		Save function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void save(Archive& ar, const unsigned int version) const
			{
			try {
			ar & boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTobject>(*this));
			ar & BOOST_SERIALIZATION_NVP(data);
			ar & BOOST_SERIALIZATION_NVP(filter);
			std::vector<std::pair<FMTmask, std::vector<int>>>vecfastpass(fastpass.begin(), fastpass.end());
			ar & BOOST_SERIALIZATION_NVP(vecfastpass);
			}catch (...)
				{
				_exhandler->printexceptions("", "FMTlist::save", __LINE__, __FILE__);
				}
			}
		// DocString: FMTlist::load
		/**
		Save function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
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
		// DocString: FMTlist::data
		///The container holding the data and the masks of the FMTlist.
		std::vector<std::pair<FMTmask, T>>data;
		// DocString: FMTlist::filter
		///The mask filter used by the FMTlist to shrink the original FMTmask.
		FMTmaskfilter filter;
		// DocString: FMTlist::fastpass
		///unordered_map used to do caching of mask subsets of the FMTlist.
		mutable boost::unordered_map<FMTmask, std::vector<int>>fastpass;
		// DocString: FMTlist::pushtodata
		/**
		Push data in vector...
		*/
		void pushtodata(std::vector<std::pair<FMTmask, T>>& datavector,
			const FMTmask& mask, const T& maskdata) const
		{
			datavector.push_back(std::pair<FMTmask, T>(mask, maskdata));
		}
	protected:
		// DocString: FMTlist::presolvelist
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
			try {
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
						pushtodata(newdata, mskkey, object.second);
					}
				}
				data.swap(newdata);
				//data = newdata;
				this->update();
				data.shrink_to_fit();
			}catch (...)
				{
				_exhandler->raisefromcatch("","FMTlist::presolvelist", __LINE__, __FILE__);
				}
			}
		void copydata(const Core::FMTlist<T>& rhs)
			{
			data = rhs.data;
			}
	public:
		// DocString: FMTlist::getunion
		/**
		Get a global union mask from all the masks of the FMTlist.
		*/
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
		// DocString: FMTlist()
		/**
		Default constructor for FMTlist.
		*/
		FMTlist() :
			FMTobject(),
			data(),
			filter(),
			fastpass(){};
		// DocString: FMTlist(const FMTlist&)
		/**
		Default copy constructor for FMTlist.
		*/
		FMTlist(const FMTlist<T>& rhs) :
			FMTobject(rhs),
			data(),
			filter(rhs.filter),
			fastpass(rhs.fastpass)
		{
			copydata(rhs);
		}
		// DocString: FMTlist::operator=
		/**
		Default copy assignment for FMTlist.
		*/
		FMTlist& operator = (const FMTlist<T>& rhs)
		{
			if (this != &rhs)
			{
				FMTobject::operator=(rhs);
				copydata(rhs);
				filter = rhs.filter;
				fastpass = rhs.fastpass;
			}
			return *this;
		}
		// DocString: FMTlist::operator==
		/**
		Comparison operator of FMTlist.
		*/
		bool operator == (const FMTlist<T>& rhs) const
			{
			return (data == rhs.data);
			}
		// DocString: ~FMTlist()
		/**
		Default destructor for FMTlist.
		*/
		~FMTlist() = default;
		// DocString: FMTlist::empty
		/**
		Returns true if the FMTlist is empty else false.
		*/
		bool empty() const
			{
			return data.empty();
			}
		// DocString: FMTlist::canshrink
		/**
		Returns true if the FMTlist can be shrink else false.
		*/
		bool canshrink() const
			{
			return filter.empty();
			}
		// DocString: FMTlist::clearcache
		/**
		Clear fastpass by swaping with an empty container.
		*/
		virtual void clearcache()
			{
				boost::unordered_map<FMTmask, std::vector<int>>().swap(fastpass);
			}
		// DocString: FMTlist::size
		/**
		Returns the size of the FMTlist
		*/
		size_t size() const
		{
			return data.size();
		}
		// DocString: FMTlist::findsets
		/**
		Here is the main function used on FMTlist. Giving a global (mask) it will returns elements that are a subset of the global (mask), in the same order
		present in the FMTlist. It will also use caching to try to get elements faster next time it's asked by the user.
		*/
		std::vector<const T*> findsets(const FMTmask& mask) const
			{
			const FMTmask newkey = filter.filter(mask);
			return findsetswithfiltered(newkey);
			}
		// DocString: FMTlist::findsetswithfiltered
		/**
		Here is the main function used on FMTlist. Giving a filtered mask (newkey) it will returns elements that are a subset of the global (mask), in the same order
		present in the FMTlist. It will also use caching to try to get elements faster next time it's asked by the user.
		*/
		std::vector<const T*> findsetswithfiltered(const FMTmask& newkey) const
		{
			std::vector<const T*>allhits;
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
				for (const std::pair<FMTmask, T>& object : data)
				{
					if (newkey.issubsetof(object.first))
					{
						fastpass[newkey].push_back(location);
						allhits.push_back(&object.second);
					}
					++location;
				}
				fastpass[newkey].shrink_to_fit();
			}
			return allhits;
		}
		// DocString: FMTlist::filtermask
		/**
		Function used to shrink the FMTmask (basemask) using the global filter of the FMTlist.
		*/
		inline FMTmask filtermask(const FMTmask& basemask) const
			{
			return filter.filter(basemask);
			}
		// DocString: FMTlist::shrink
		/**
		Reduce the size of the FMTmask used in the FMTlist for less complexity.
		*/
		void shrink()
		{
				fastpass.clear();
				std::vector<Core::FMTmask> filteredmasks;
				for (const std::pair<FMTmask, T>& object : data)
				{
					filteredmasks.push_back(object.first);
				}
				filter=Core::FMTmaskfilter(filteredmasks);
				for (std::pair<FMTmask, T>& object : data)
				{
					object.first = filter.filter(object.first);
				}
				data.shrink_to_fit();
		}
		// DocString: FMTlist::unshrink
		/**
		Bring back the complexity of each FMTmask into the FMTlist.
		*/
		void unshrink(const std::vector<FMTtheme>& themes)
		{
			fastpass.clear();
			filter = Core::FMTmaskfilter();
			for (std::pair<FMTmask, T>& object : data)
				{
				object.first = FMTmask(std::string(object.first), themes);
				}
		}
		// DocString: FMTlist::push_back
		/**
		Push back an element at the end of the FMTlist.
		*/
		void push_back(const FMTmask& mask, const T& value)
		{
			data.emplace_back(mask,value);
		}
		// DocString: FMTlist::update
		/**
		Update the FMTlist filter and shrink it if any changes appened in the list.
		*/
		virtual void update()
			{
				if (canshrink())
					{
					shrink();
					}
			}
		// DocString: FMTlist::push_back
		/**
		Push back a whole FMTlist at the end of this FMTlist.
		*/
		void push_back(const FMTlist<T>& rhs)
		{
			if (this->canshrink() && rhs.canshrink())
			{
				copydata(rhs);
				this->shrink();
			}
		}
		// DocString: FMTlist::push_front
		/**
		Push front an element at the beginning of the FMTlist.
		*/
		void push_front(const FMTmask& mask, const T& value)
		{
			insert(0, mask, value);
		}
		// DocString: FMTlist::pop_back
		/**
		Pop back an element at the end of the FMTlist.
		*/
		void pop_back()
            {
			data.pop_back();
            }
		// DocString: FMTlist::erase
		/**
		Erase a specific element in the FMTlist.
		*/
		void erase(const size_t& location)
            {
			data.erase(data.begin() + location);
            }
		// DocString: FMTlist::insert
		/**
		Insert an element in the FMTlist at a specific location.
		*/
        void insert(const size_t& location,const FMTmask& mask, const T& value)
            {
			data.insert(data.begin() + location,std::pair<FMTmask,T>(mask,value));
            }
		// DocString: FMTlist::value_type
		///Value typedef of the FMTlist
		typedef typename std::vector<std::pair<FMTmask, T>>::value_type value_type;
		// DocString: FMTlist::iterator
		///Iterator typedef of the FMTlist
		typedef typename std::vector<std::pair<FMTmask, T>>::iterator iterator;
		// DocString: FMTlist::const_iterator
		///Const_Iterator typedef of the FMTlist
		typedef typename std::vector<std::pair<FMTmask, T>>::const_iterator const_iterator;
		// DocString: FMTlist::begin
		/**
		Returns an iterator at the beginning of the FMTlist.
		*/
		iterator begin()
			{
			return data.begin();
			}
		// DocString: FMTlist::begin
		/**
		Returns an const iterator at the beginning of the FMTlist.
		*/
		const_iterator begin() const
			{
			return data.begin();
			}
		// DocString: FMTlist::end
		/**
		Returns an iterator at the end of the FMTlist.
		*/
		iterator  end()
			{
			return data.end();
			}
		// DocString: FMTlist::end
		/**
		Returns an const iterator at the end of the FMTlist.
		*/
		const_iterator end() const
			{
			return data.end();
			}
		

	};


	

	template<> inline void FMTlist<std::unique_ptr<Core::FMTyieldhandler>>::insert(const size_t& location, const FMTmask& mask, const std::unique_ptr<Core::FMTyieldhandler>& value)
	{
		std::pair<Core::FMTmask, std::unique_ptr<Core::FMTyieldhandler>> newobject = std::make_pair(mask, std::move(value->clone()));
		data.insert(data.begin() + location, std::move(newobject));
	}

	template<> inline void FMTlist<std::unique_ptr<Core::FMTyieldhandler>>::pushtodata(std::vector<std::pair<FMTmask, std::unique_ptr<Core::FMTyieldhandler>>>& datavector,
		const FMTmask& mask, const std::unique_ptr<Core::FMTyieldhandler>& maskdata) const
	{
		std::pair<Core::FMTmask, std::unique_ptr<Core::FMTyieldhandler>> newobject = std::make_pair(mask, std::move(maskdata->clone()));
		datavector.push_back(std::move(newobject));
	}

	template<> inline void FMTlist<std::unique_ptr<Core::FMTyieldhandler>>::push_back(const FMTmask& mask, const std::unique_ptr<Core::FMTyieldhandler>& value)
	{
		pushtodata(data, mask, value);
	}

	template<> inline void FMTlist<std::unique_ptr<Core::FMTyieldhandler>>::copydata(const FMTlist<std::unique_ptr<Core::FMTyieldhandler>>& rhs)
	{
		data.clear();
		data.reserve(rhs.data.size());
		for (const std::pair<Core::FMTmask, std::unique_ptr<Core::FMTyieldhandler>>& object : rhs.data)
		{
			pushtodata(data, object.first, object.second);
		}
	}


}


#endif