/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTlist_Hm_included
#define FMTlist_Hm_included

#include <boost/unordered_map.hpp>
#include <functional>
#include <vector>
#include <utility>

#include "FMTmask.h"
#include "FMTmaskfilter.h"
#include <boost/serialization/version.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/hash_collections_save_imp.hpp>
#include <boost/serialization/hash_collections_load_imp.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include "FMTexceptionhandler.h"
#include "FMTobject.h"
#include <iterator>
#include <memory>
#include "FMTyieldhandler.h"
#include "FMTtheme.h"


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
	public:
		// DocString: FMTlist::value_type
		///Value typedef of the FMTlist
		typedef typename std::vector<std::pair<FMTmask, T>>::value_type value_type;
		// DocString: FMTlist::iterator
		///Iterator typedef of the FMTlist
		typedef typename std::vector<std::pair<FMTmask, T>>::iterator iterator;
		// DocString: FMTlist::const_iterator
		///Const_Iterator typedef of the FMTlist
		typedef typename std::vector<std::pair<FMTmask, T>>::const_iterator const_iterator;
		// DocString: FMTlist::operator+=
		/**
		 * @brief append OtherList to this list actions both list had to be non shrinked, will throw exception if shrinked.
		 * @param OtherList the other list to append to this one.
		 * @returns A reference the the newly appended list.
		 */
		Core::FMTlist<T>& operator+= (const Core::FMTlist<T>& OtherList)
		{
			try {
				if (!this->canShrink()||!OtherList.canShrink())
					{
					_exhandler->raise(Exception::FMTexc::FMTinvalid_action, "Cant append list together",
						"FMTlist::operator::+=", __LINE__, __FILE__);
					}
				for (const std::pair<FMTmask, T>& Object : OtherList.data)
					{
					push_back(Object.first, Object.second);
					}
			}catch (...) {
				_exhandler->raisefromcatch("", "FMTlist::operator::+=", __LINE__, __FILE__);

			}
			return *this;
		}

		// DocString: FMTlist::swap
		/**
		Swap function for FMTlist.
		*/
		void swap(Core::FMTlist<T>& rhs)
		{
			data.swap(rhs.data);
			filter.swap(rhs.filter);
			fastpass.swap(rhs.fastpass);
		}
		// DocString: FMTlist::getUnion
		/**
		Get a global union mask from all the masks of the FMTlist.
		*/
		FMTmask getUnion(const std::vector<FMTtheme>& themes) const
		{
			Core::FMTmask testedMask(std::string(this->begin()->first), themes);
			for (const auto& object : *this)
			{
				const Core::FMTmask specifierMask(std::string(object.first), themes);
				testedMask = testedMask.getUnion(specifierMask);
			}
			return testedMask;
		}
		// DocString: FMTlist()
		/**
		Default constructor for FMTlist.
		*/
		FMTlist() :
			FMTobject(),
			data(),
			filter(),
			fastpass() {};
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
			copyData(rhs);
		}
		// DocString: FMTlist::reserve
		/**
		@brief Reserve memory based on a other FMTlist
		@param[in] p_other FMTlist.
		*/
		void reserve(const FMTlist<T>& p_other)
			{
			data.reserve(p_other.data.size());
			fastpass.reserve(p_other.data.size());
			}
		// DocString: FMTlist::clear
		/**
		@brief clear the data
		*/
		void clear()
			{
			data.clear();
			fastpass.clear();
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
				copyData(rhs);
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
		// DocString: FMTlist::canShrink
		/**
		Returns true if the FMTlist can be shrink else false.
		*/
		bool canShrink() const
		{
			return filter.empty();
		}
		// DocString: FMTlist::clearCache
		/**
		Clear fastpass by swaping with an empty container.
		*/
		virtual void clearCache()
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
		// DocString: FMTlist::findSets
		/**
		@brief Here is the main function used on FMTlist. Giving a global (mask) it will returns elements that are a subset of the global (mask), in the same order
		present in the FMTlist. It will also use caching to try to get elements faster next time it's asked by the user.
		@param[in] p_mask the input mask
		@return a vector of subset of list
		*/
		std::vector<FMTlist::const_iterator> findSets(const FMTmask& p_mask) const
		{
			std::vector<FMTlist::const_iterator>subset;
			try {
				const FMTmask NEW_KEY = filter.filter(p_mask);
				subset = findSetsWithFiltered(NEW_KEY);
			}catch (...)
				{
				_exhandler->raisefromcatch("for mask "+std::string(p_mask), "FMTlist:: findsets", __LINE__, __FILE__);
				}
			return subset;
		}
		// DocString: FMTlist::findSetsWithFiltered
		/**
		@brief Here is the main function used on FMTlist. Giving a filtered mask (newkey) it will returns elements that are a subset of the global (mask), in the same order
		present in the FMTlist. It will also use caching to try to get elements faster next time it's asked by the user.
		@param[in] p_newKey the mask to select subset
		@return a vector of const iterator on the data.
		*/
		std::vector<FMTlist::const_iterator> findSetsWithFiltered(const FMTmask& p_newKey) const
		{
			std::vector<const_iterator>allhits;
			try {
				const_iterator BEGINNING = begin();
				boost::unordered_map<FMTmask, std::vector<int>>::const_iterator fast_it = fastpass.find(p_newKey);
				//const int SIZE_OF_DATE = static_cast<int>(size());
				if (fast_it != fastpass.end())
				{
					allhits.reserve(fast_it->second.size());
					for (const int& location : fast_it->second)
					{
						allhits.push_back(BEGINNING + location);
						/*if (location >= SIZE_OF_DATE)
						{
							_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
								"INVALID FMTlist cache", "FMTlist::findSetsWithFiltered", __LINE__, __FILE__);
						}*/
					}
				}
				else {
					fastpass[p_newKey] = std::vector<int>();
					int location = 0;
					for (const std::pair<FMTmask, T>& object : data)
					{
						if (p_newKey.isSubsetOf(object.first))
						{
							fastpass[p_newKey].push_back(location);
							allhits.push_back(BEGINNING + location);
						}
						++location;
					}
					fastpass[p_newKey].shrink_to_fit();
				}
			}catch (...) 
				{
				_exhandler->raisefromcatch("", "FMTlist::findSetsWithFiltered", __LINE__, __FILE__);
				}
			return allhits;
		}
		// DocString: FMTlist::filterMask
		/**
		Function used to shrink the FMTmask (baseMask) using the global filter of the FMTlist.
		*/
		inline FMTmask filterMask(const FMTmask& baseMask) const
		{
			return filter.filter(baseMask);
		}
		// DocString: FMTlist::shrink
		/**
		Reduce the size of the FMTmask used in the FMTlist for less complexity.
		*/
		void shrink()
		{
			try {
				if (!data.empty())
				{
					fastpass.clear();
					std::vector<Core::FMTmask> filteredmasks;
					filteredmasks.reserve(data.size());
					for (const std::pair<FMTmask, T>& object : data)
					{
						filteredmasks.push_back(object.first);
					}
					if (filteredmasks.empty())
					{
						_exhandler->raise(Exception::FMTexc::FMTinvalid_maskrange, "Empty mask", "FMTactionparser::shrink", __LINE__, __FILE__);
					}
					filter = Core::FMTmaskfilter(filteredmasks);
					for (std::pair<FMTmask, T>& object : data)
					{
						object.first = filter.filter(object.first);
					}
					data.shrink_to_fit();
				}
				
			}
			catch (...) {

				_exhandler->raisefromcatch("", "FMTlist::shrink", __LINE__, __FILE__);

			}
		}
		// DocString: FMTlist::unShrink
		/**
		Bring back the complexity of each FMTmask into the FMTlist.
		*/
		void unShrink(const std::vector<FMTtheme>& themes)
		{
			fastpass.clear();
			filter = Core::FMTmaskfilter();
			for (std::pair<FMTmask, T>& object : data)
			{
				object.first = FMTmask(std::string(object.first), themes);
			}
		}
		// DocString: FMTlist::apiPushBack
		/**
		For template specification to overcome the static_cast of function
		*/
		void apiPushBack(const FMTmask& mask, const T& value)
		{
			push_back(mask, value);
		}
		// DocString: FMTlist::push_back
		/**
		Push back an element at the end of the FMTlist.
		*/
		void push_back(const FMTmask& mask, const T& value)
		{
			data.emplace_back(mask, value);
		}
		// DocString: FMTlist::update
		/**
		Update the FMTlist filter and shrink it if any changes appened in the list.
		*/
		virtual void update()
		{
			if (canShrink())
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
			if (this->canShrink() && rhs.canShrink())
			{
				copyData(rhs);
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
		void insert(const size_t& location, const FMTmask& mask, const T& value)
		{
			data.insert(data.begin() + location, std::pair<FMTmask, T>(mask, value));
		}
		
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
	protected:
		// DocString: FMTlist::compressMasks
		/**
		During pressolve you can compress multiple mask into single one if you have the same
		data. Usefull for actions and transitions. It will compress mask by generating aggregates
		and so make changes to themes.
		*/
		void compressMasks(std::vector<FMTtheme>& newthemes)
			{
			try {
				if (size()==1)
				{
					return;
				}
				unShrink(newthemes);
				size_t thstart = 0;
				for (FMTtheme& theme : newthemes)
				{
					std::vector<std::pair<FMTmask, T>>newvecdata;
					newvecdata.reserve(data.size());
					std::list<std::pair<FMTmask, T>>newData(data.begin(), data.end());
					boost::dynamic_bitset<uint8_t>selectedbits;
					selectedbits.resize(data.begin()->first.size(), true);
					for (size_t loc = thstart;loc < (theme.size()+ thstart);++loc)
						{
						selectedbits[loc] = false;
						}
					while (!newData.empty())
						{
						typename std::list<std::pair<FMTmask, T>>::iterator baseit = newData.begin();
						typename std::list<std::pair<FMTmask, T>>::iterator datait = newData.begin();
						++datait;
						std::vector<typename std::list<std::pair<FMTmask, T>>::iterator>toRemove;
						toRemove.reserve(newData.size());
						Core::FMTmask baseMask(baseit->first);
						const boost::dynamic_bitset<uint8_t> selecinter = selectedbits & baseit->first.getBitsetReference();
						boost::dynamic_bitset<uint8_t> reverSelect(selecinter);
						for (size_t loc = thstart; loc < (theme.size() + thstart); ++loc)
							{
							reverSelect[loc] = true;
							}
						/*Core::FMTmask testmask(baseit->first);
						testmask.set(theme, "?");*/
						while (datait!=newData.end())
							{
							/*Core::FMTmask datamask(datait->first);
							datamask.set(theme, "?");*/
							const boost::dynamic_bitset<uint8_t>&dataref = datait->first.getBitsetReference();
							if (dataref.is_subset_of(reverSelect))
							{
								const boost::dynamic_bitset<uint8_t> datainter = selectedbits & dataref;
								if (datainter == selecinter &&
									baseit->second == datait->second)
								{
									baseMask = baseMask.getUnion(datait->first);
									toRemove.push_back(datait);
								}
							}
							++datait;
							}
						std::pair<FMTmask, T>newElement(baseMask,baseit->second);
						newData.erase(newData.begin());
						for (typename std::list<std::pair<FMTmask, T>>::iterator remove : toRemove)
							{
							newData.erase(remove);
							}
						if(!toRemove.empty())//aggregation done set new aggregate and refresh mask
							{
							std::string newmask;
							for (const FMTtheme& subtheme : newthemes)
								{
								if (subtheme==theme)
									{
									newmask +=theme.updateFromMask(baseMask) + " ";
								}else {
									newmask += baseMask.get(subtheme) + " ";
									}
								}
							newmask.pop_back();
							newElement.first = Core::FMTmask(newmask, newthemes);
							}
						newvecdata.push_back(newElement);
						}
					thstart += theme.size();
					data.swap(newvecdata);
				}
				shrink();
			}catch (...)
					{
				_exhandler->raisefromcatch("", "compressMasks", __LINE__, __FILE__);
				}
			}
		// DocString: FMTlist::presolveList
		/**
		Using a baseMask reprensenting the whole forest landscape this function will
		attempt to reduce the number of elements in the list knowing that if the element
		represent something that is not in the baseMask this element could be deleted.
		Also using a presolvecmask representing
		Use this function with care because it's going to change the stade of the list
		if user attempt to reference to a deleted element the model will seems broken.
		*/
		void presolveList(
			const FMTmaskfilter& filter,
			const std::vector<FMTtheme>& originalthemes,
			const std::vector<FMTtheme>& newthemes)
			{
			try {
				if (!canShrink())
				{
					unShrink(originalthemes);
				}
				const std::vector<const FMTtheme*>maskthemes = filter.getSelectedThemes(originalthemes);
				size_t dataId = data.size();
				while (dataId!=0)
					{
					--dataId;
					if (!filter.emptyFlipped() && filter.canPresolve(data[dataId].first, maskthemes))
						{
						data[dataId].first.presolveRef(filter, newthemes,false);
					}else {
						data.erase(data.begin() + dataId);
						}
					}
				FMTlist::update();
				//data.shrink_to_fit();
				/*
				std::vector<std::pair<FMTmask, T>>newData;
				newData.reserve(data.size());
				for (const std::pair<FMTmask, T>& object : data)
				{
					if (filter.canPresolve(object.first,maskthemes))
					{
						FMTmask mskkey = object.first;
						if (!filter.emptyFlipped())
						{
							mskkey = mskkey.presolve(filter, newthemes);
						}
						pushToData(newData, mskkey, object.second);
					}
				}
				data.swap(newData);*/
				//FMTlist::update();
				//data.shrink_to_fit();
			}catch (...)
				{
				_exhandler->raisefromcatch("","FMTlist::presolveList", __LINE__, __FILE__);
				}
			}
		void copyData(const Core::FMTlist<T>& rhs)
			{
			data = rhs.data;
			}
	private:
		// DocString: FMTlist::save
		/**
		Save function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void save(Archive& ar, const unsigned int version) const
		{
			try {
				ar& boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTobject>(*this));
				ar& BOOST_SERIALIZATION_NVP(data);
				ar& BOOST_SERIALIZATION_NVP(filter);
				std::vector<std::pair<FMTmask, std::vector<int>>>vecFastPass(fastpass.begin(), fastpass.end());
				ar& BOOST_SERIALIZATION_NVP(vecFastPass);
			}
			catch (...)
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
			ar& BOOST_SERIALIZATION_NVP(data);
			ar& BOOST_SERIALIZATION_NVP(filter);
			std::vector<std::pair<FMTmask, std::vector<int>>>vecFastPass;
			ar& BOOST_SERIALIZATION_NVP(vecFastPass);
			for (const std::pair<FMTmask, std::vector<int>>& values : vecFastPass)
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
		// DocString: FMTlist::pushToData
		/**
		Push data in vector...
		*/
		void pushToData(std::vector<std::pair<FMTmask, T>>& datavector,
			const FMTmask& mask, const T& maskdata) const
		{
			datavector.push_back(std::pair<FMTmask, T>(mask, maskdata));
		}

	};

	template<> inline void FMTlist<std::unique_ptr<Core::FMTyieldhandler>>::compressMasks(std::vector<FMTtheme>& newthemes)
	{

	}

	template<> inline void FMTlist<std::unique_ptr<Core::FMTyieldhandler>>::insert(
		const size_t& location, 
		const FMTmask& mask, 
		const std::unique_ptr<Core::FMTyieldhandler>& value)
	{
		std::pair<Core::FMTmask, std::unique_ptr<Core::FMTyieldhandler>> newobject = std::make_pair(mask, std::move(value->clone()));
		data.insert(data.begin() + location, std::move(newobject));
	}

	template<> inline void FMTlist<std::unique_ptr<Core::FMTyieldhandler>>::pushToData(
		std::vector<std::pair<FMTmask, 
		std::unique_ptr<Core::FMTyieldhandler>>>& datavector,
		const FMTmask& mask, 
		const std::unique_ptr<Core::FMTyieldhandler>& maskdata) const
	{
		std::pair<Core::FMTmask, std::unique_ptr<Core::FMTyieldhandler>> newobject = std::make_pair(mask, std::move(maskdata->clone()));
		datavector.push_back(std::move(newobject));
	}

	template<> inline void FMTlist<std::unique_ptr<Core::FMTyieldhandler>>::push_back(
		const FMTmask& mask, 
		const std::unique_ptr<Core::FMTyieldhandler>& value)
	{
		pushToData(data, mask, value);
	}

	template<> inline void FMTlist<std::unique_ptr<Core::FMTyieldhandler>>::copyData(
		const FMTlist<std::unique_ptr<Core::FMTyieldhandler>>& rhs)
	{
		data.clear();
		data.reserve(rhs.data.size());
		for (const std::pair<Core::FMTmask, std::unique_ptr<Core::FMTyieldhandler>>& object : rhs.data)
		{
			pushToData(data, object.first, object.second);
		}
	}
}

#endif
