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
	// DocString: FMTList
	/**
	The FMTList class is made to keep track of objects with a FMTMask tag like dictionary. The yield, action and transition class
	uses this class to list objects based on the tagged FMTMask and the position of the object in the list.
	FMTList uses caching and a mask filter to reduce the size of the list and gives a fast access to objects present in
	the list.
	*/
	template<typename T>
	class FMTList: public FMTObject
	{
	public:
		// DocString: FMTList::value_type
		///Value typedef of the FMTList
		typedef typename std::vector<std::pair<FMTMask, T>>::value_type value_type;
		// DocString: FMTList::iterator
		///Iterator typedef of the FMTList
		typedef typename std::vector<std::pair<FMTMask, T>>::iterator iterator;
		// DocString: FMTList::const_iterator
		///Const_Iterator typedef of the FMTList
		typedef typename std::vector<std::pair<FMTMask, T>>::const_iterator const_iterator;
		// DocString: FMTList::operator+=
		/**
		 * @brief append OtherList to this list actions both list had to be non shrinked, will throw exception if shrinked.
		 * @param OtherList the other list to append to this one.
		 * @returns A reference the the newly appended list.
		 */
		Core::FMTList<T>& operator+= (const Core::FMTList<T>& OtherList)
		{
			try {
				if (!this->canShrink()||!OtherList.canShrink())
					{
					_exhandler->raise(Exception::FMTexc::FMTinvalid_action, "Cant append list together",
						"FMTList::operator::+=", __LINE__, __FILE__);
					}
				for (const std::pair<FMTMask, T>& Object : OtherList.data)
					{
					push_back(Object.first, Object.second);
					}
			}catch (...) {
				_exhandler->raiseFromCatch("", "FMTList::operator::+=", __LINE__, __FILE__);

			}
			return *this;
		}

		// DocString: FMTList::swap
		/**
		Swap function for FMTList.
		*/
		void swap(Core::FMTList<T>& rhs)
		{
			data.swap(rhs.data);
			filter.swap(rhs.filter);
			fastpass.swap(rhs.fastpass);
		}
		// DocString: FMTList::getUnion
		/**
		Get a global union mask from all the masks of the FMTList.
		*/
		FMTMask getUnion(const std::vector<FMTTheme>& themes) const
		{
			Core::FMTMask testedMask(std::string(this->begin()->first), themes);
			for (const auto& object : *this)
			{
				const Core::FMTMask specifierMask(std::string(object.first), themes);
				testedMask = testedMask.getUnion(specifierMask);
			}
			return testedMask;
		}
		// DocString: FMTList()
		/**
		Default constructor for FMTList.
		*/
		FMTList() :
			FMTObject(),
			data(),
			filter(),
			fastpass() {};
		// DocString: FMTList(const FMTList&)
		/**
		Default copy constructor for FMTList.
		*/
		FMTList(const FMTList<T>& rhs) :
			FMTObject(rhs),
			data(),
			filter(rhs.filter),
			fastpass(rhs.fastpass)
		{
			copyData(rhs);
		}
		// DocString: FMTList::reserve
		/**
		@brief Reserve memory based on a other FMTList
		@param[in] p_other FMTList.
		*/
		void reserve(const FMTList<T>& p_other)
			{
			data.reserve(p_other.data.size());
			fastpass.reserve(p_other.data.size());
			}
		// DocString: FMTList::clear
		/**
		@brief clear the data
		*/
		void clear()
			{
			data.clear();
			fastpass.clear();
			}
		// DocString: FMTList::operator=
		/**
		Default copy assignment for FMTList.
		*/
		FMTList& operator = (const FMTList<T>& rhs)
		{
			if (this != &rhs)
			{
				FMTObject::operator=(rhs);
				copyData(rhs);
				filter = rhs.filter;
				fastpass = rhs.fastpass;
			}
			return *this;
		}
		// DocString: FMTList::operator==
		/**
		Comparison operator of FMTList.
		*/
		bool operator == (const FMTList<T>& rhs) const
		{
			return (data == rhs.data);
		}
		// DocString: ~FMTList()
		/**
		Default destructor for FMTList.
		*/
		~FMTList() = default;
		// DocString: FMTList::empty
		/**
		Returns true if the FMTList is empty else false.
		*/
		bool empty() const
		{
			return data.empty();
		}
		// DocString: FMTList::canShrink
		/**
		Returns true if the FMTList can be shrink else false.
		*/
		bool canShrink() const
		{
			return filter.empty();
		}
		// DocString: FMTList::clearCache
		/**
		Clear fastpass by swaping with an empty container.
		*/
		virtual void clearCache()
		{
			boost::unordered_map<FMTMask, std::vector<int>>().swap(fastpass);
		}
		// DocString: FMTList::size
		/**
		Returns the size of the FMTList
		*/
		size_t size() const
		{
			return data.size();
		}
		// DocString: FMTList::findSets
		/**
		@brief Here is the main function used on FMTList. Giving a global (mask) it will returns elements that are a subset of the global (mask), in the same order
		present in the FMTList. It will also use caching to try to get elements faster next time it's asked by the user.
		@param[in] p_mask the input mask
		@return a vector of subset of list
		*/
		std::vector<FMTList::const_iterator> findSets(const FMTMask& p_mask) const
		{
			std::vector<FMTList::const_iterator>subset;
			try {
				const FMTMask NEW_KEY = filter.filter(p_mask);
				subset = findSetsWithFiltered(NEW_KEY);
			}catch (...)
				{
				_exhandler->raiseFromCatch("for mask "+std::string(p_mask), "FMTList:: findsets", __LINE__, __FILE__);
				}
			return subset;
		}
		// DocString: FMTList::findSetsWithFiltered
		/**
		@brief Here is the main function used on FMTList. Giving a filtered mask (newkey) it will returns elements that are a subset of the global (mask), in the same order
		present in the FMTList. It will also use caching to try to get elements faster next time it's asked by the user.
		@param[in] p_newKey the mask to select subset
		@return a vector of const iterator on the data.
		*/
		std::vector<FMTList::const_iterator> findSetsWithFiltered(const FMTMask& p_newKey) const
		{
			std::vector<const_iterator>allhits;
			try {
				const_iterator BEGINNING = begin();
				boost::unordered_map<FMTMask, std::vector<int>>::const_iterator fast_it = fastpass.find(p_newKey);
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
								"INVALID FMTList cache", "FMTList::findSetsWithFiltered", __LINE__, __FILE__);
						}*/
					}
				}
				else {
					fastpass[p_newKey] = std::vector<int>();
					int location = 0;
					for (const std::pair<FMTMask, T>& object : data)
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
				_exhandler->raiseFromCatch("", "FMTList::findSetsWithFiltered", __LINE__, __FILE__);
				}
			return allhits;
		}
		// DocString: FMTList::filterMask
		/**
		Function used to shrink the FMTMask (baseMask) using the global filter of the FMTList.
		*/
		inline FMTMask filterMask(const FMTMask& baseMask) const
		{
			return filter.filter(baseMask);
		}
		// DocString: FMTList::shrink
		/**
		Reduce the size of the FMTMask used in the FMTList for less complexity.
		*/
		void shrink()
		{
			try {
				if (!data.empty())
				{
					fastpass.clear();
					std::vector<Core::FMTMask> filteredmasks;
					filteredmasks.reserve(data.size());
					for (const std::pair<FMTMask, T>& object : data)
					{
						filteredmasks.push_back(object.first);
					}
					if (filteredmasks.empty())
					{
						_exhandler->raise(Exception::FMTexc::FMTinvalid_maskrange, "Empty mask", "FMTActionParser::shrink", __LINE__, __FILE__);
					}
					filter = Core::FMTMaskFilter(filteredmasks);
					for (std::pair<FMTMask, T>& object : data)
					{
						object.first = filter.filter(object.first);
					}
					data.shrink_to_fit();
				}
				
			}
			catch (...) {

				_exhandler->raiseFromCatch("", "FMTList::shrink", __LINE__, __FILE__);

			}
		}
		// DocString: FMTList::unShrink
		/**
		Bring back the complexity of each FMTMask into the FMTList.
		*/
		void unShrink(const std::vector<FMTTheme>& themes)
		{
			fastpass.clear();
			filter = Core::FMTMaskFilter();
			for (std::pair<FMTMask, T>& object : data)
			{
				object.first = FMTMask(std::string(object.first), themes);
			}
		}
		// DocString: FMTList::apiPushBack
		/**
		For template specification to overcome the static_cast of function
		*/
		void apiPushBack(const FMTMask& mask, const T& value)
		{
			push_back(mask, value);
		}
		// DocString: FMTList::push_back
		/**
		Push back an element at the end of the FMTList.
		*/
		void push_back(const FMTMask& mask, const T& value)
		{
			data.emplace_back(mask, value);
		}
		// DocString: FMTList::update
		/**
		Update the FMTList filter and shrink it if any changes appened in the list.
		*/
		virtual void update()
		{
			if (canShrink())
			{
				shrink();
			}
		}
		// DocString: FMTList::push_back
		/**
		Push back a whole FMTList at the end of this FMTList.
		*/
		void push_back(const FMTList<T>& rhs)
		{
			if (this->canShrink() && rhs.canShrink())
			{
				copyData(rhs);
				this->shrink();
			}
		}
		// DocString: FMTList::push_front
		/**
		Push front an element at the beginning of the FMTList.
		*/
		void push_front(const FMTMask& mask, const T& value)
		{
			insert(0, mask, value);
		}
		// DocString: FMTList::pop_back
		/**
		Pop back an element at the end of the FMTList.
		*/
		void pop_back()
		{
			data.pop_back();
		}
		// DocString: FMTList::erase
		/**
		Erase a specific element in the FMTList.
		*/
		void erase(const size_t& location)
		{
			data.erase(data.begin() + location);
		}
		// DocString: FMTList::insert
		/**
		Insert an element in the FMTList at a specific location.
		*/
		void insert(const size_t& location, const FMTMask& mask, const T& value)
		{
			data.insert(data.begin() + location, std::pair<FMTMask, T>(mask, value));
		}
		
		// DocString: FMTList::begin
		/**
		Returns an iterator at the beginning of the FMTList.
		*/
		iterator begin()
		{
			return data.begin();
		}
		// DocString: FMTList::begin
		/**
		Returns an const iterator at the beginning of the FMTList.
		*/
		const_iterator begin() const
		{
			return data.begin();
		}
		// DocString: FMTList::end
		/**
		Returns an iterator at the end of the FMTList.
		*/
		iterator  end()
		{
			return data.end();
		}
		// DocString: FMTList::end
		/**
		Returns an const iterator at the end of the FMTList.
		*/
		const_iterator end() const
		{
			return data.end();
		}
	protected:
		// DocString: FMTList::compressMasks
		/**
		During pressolve you can compress multiple mask into single one if you have the same
		data. Usefull for actions and transitions. It will compress mask by generating aggregates
		and so make changes to themes.
		*/
		void compressMasks(std::vector<FMTTheme>& newthemes)
			{
			try {
				if (size()==1)
				{
					return;
				}
				unShrink(newthemes);
				size_t thstart = 0;
				for (FMTTheme& theme : newthemes)
				{
					std::vector<std::pair<FMTMask, T>>newvecdata;
					newvecdata.reserve(data.size());
					std::list<std::pair<FMTMask, T>>newData(data.begin(), data.end());
					boost::dynamic_bitset<uint8_t>selectedbits;
					selectedbits.resize(data.begin()->first.size(), true);
					for (size_t loc = thstart;loc < (theme.size()+ thstart);++loc)
						{
						selectedbits[loc] = false;
						}
					while (!newData.empty())
						{
						typename std::list<std::pair<FMTMask, T>>::iterator baseit = newData.begin();
						typename std::list<std::pair<FMTMask, T>>::iterator datait = newData.begin();
						++datait;
						std::vector<typename std::list<std::pair<FMTMask, T>>::iterator>toRemove;
						toRemove.reserve(newData.size());
						Core::FMTMask baseMask(baseit->first);
						const boost::dynamic_bitset<uint8_t> selecinter = selectedbits & baseit->first.getBitsetReference();
						boost::dynamic_bitset<uint8_t> reverSelect(selecinter);
						for (size_t loc = thstart; loc < (theme.size() + thstart); ++loc)
							{
							reverSelect[loc] = true;
							}
						/*Core::FMTMask testmask(baseit->first);
						testmask.set(theme, "?");*/
						while (datait!=newData.end())
							{
							/*Core::FMTMask datamask(datait->first);
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
						std::pair<FMTMask, T>newElement(baseMask,baseit->second);
						newData.erase(newData.begin());
						for (typename std::list<std::pair<FMTMask, T>>::iterator remove : toRemove)
							{
							newData.erase(remove);
							}
						if(!toRemove.empty())//aggregation done set new aggregate and refresh mask
							{
							std::string newmask;
							for (const FMTTheme& subtheme : newthemes)
								{
								if (subtheme==theme)
									{
									newmask +=theme.updateFromMask(baseMask) + " ";
								}else {
									newmask += baseMask.get(subtheme) + " ";
									}
								}
							newmask.pop_back();
							newElement.first = Core::FMTMask(newmask, newthemes);
							}
						newvecdata.push_back(newElement);
						}
					thstart += theme.size();
					data.swap(newvecdata);
				}
				shrink();
			}catch (...)
					{
				_exhandler->raiseFromCatch("", "compressMasks", __LINE__, __FILE__);
				}
			}
		// DocString: FMTList::presolveList
		/**
		Using a baseMask reprensenting the whole forest landscape this function will
		attempt to reduce the number of elements in the list knowing that if the element
		represent something that is not in the baseMask this element could be deleted.
		Also using a presolvecmask representing
		Use this function with care because it's going to change the stade of the list
		if user attempt to reference to a deleted element the model will seems broken.
		*/
		void presolveList(
			const FMTMaskFilter& filter,
			const std::vector<FMTTheme>& originalthemes,
			const std::vector<FMTTheme>& newthemes)
			{
			try {
				if (!canShrink())
				{
					unShrink(originalthemes);
				}
				const std::vector<const FMTTheme*>maskthemes = filter.getSelectedThemes(originalthemes);
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
				FMTList::update();
				//data.shrink_to_fit();
				/*
				std::vector<std::pair<FMTMask, T>>newData;
				newData.reserve(data.size());
				for (const std::pair<FMTMask, T>& object : data)
				{
					if (filter.canPresolve(object.first,maskthemes))
					{
						FMTMask mskkey = object.first;
						if (!filter.emptyFlipped())
						{
							mskkey = mskkey.presolve(filter, newthemes);
						}
						pushToData(newData, mskkey, object.second);
					}
				}
				data.swap(newData);*/
				//FMTList::update();
				//data.shrink_to_fit();
			}catch (...)
				{
				_exhandler->raiseFromCatch("","FMTList::presolveList", __LINE__, __FILE__);
				}
			}
		void copyData(const Core::FMTList<T>& rhs)
			{
			data = rhs.data;
			}
	private:
		// DocString: FMTList::save
		/**
		Save function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void save(Archive& ar, const unsigned int version) const
		{
			try {
				ar& boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTObject>(*this));
				ar& BOOST_SERIALIZATION_NVP(data);
				ar& BOOST_SERIALIZATION_NVP(filter);
				std::vector<std::pair<FMTMask, std::vector<int>>>vecFastPass(fastpass.begin(), fastpass.end());
				ar& BOOST_SERIALIZATION_NVP(vecFastPass);
			}
			catch (...)
			{
				_exhandler->printExceptions("", "FMTList::save", __LINE__, __FILE__);
			}
		}
		// DocString: FMTList::load
		/**
		Save function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		template<class Archive>
		void load(Archive& ar, const unsigned int version)
		{
			ar& BOOST_SERIALIZATION_NVP(data);
			ar& BOOST_SERIALIZATION_NVP(filter);
			std::vector<std::pair<FMTMask, std::vector<int>>>vecFastPass;
			ar& BOOST_SERIALIZATION_NVP(vecFastPass);
			for (const std::pair<FMTMask, std::vector<int>>& values : vecFastPass)
			{
				fastpass[values.first] = values.second;
			}
		}
		BOOST_SERIALIZATION_SPLIT_MEMBER()
			// DocString: FMTList::data
			///The container holding the data and the masks of the FMTList.
			std::vector<std::pair<FMTMask, T>>data;
		// DocString: FMTList::filter
		///The mask filter used by the FMTList to shrink the original FMTMask.
		FMTMaskFilter filter;
		// DocString: FMTList::fastpass
		///unordered_map used to do caching of mask subsets of the FMTList.
		mutable boost::unordered_map<FMTMask, std::vector<int>>fastpass;
		// DocString: FMTList::pushToData
		/**
		Push data in vector...
		*/
		void pushToData(std::vector<std::pair<FMTMask, T>>& datavector,
			const FMTMask& mask, const T& maskdata) const
		{
			datavector.push_back(std::pair<FMTMask, T>(mask, maskdata));
		}

	};

	template<> inline void FMTList<std::unique_ptr<Core::FMTYieldHandler>>::compressMasks(std::vector<FMTTheme>& newthemes)
	{

	}

	template<> inline void FMTList<std::unique_ptr<Core::FMTYieldHandler>>::insert(
		const size_t& location, 
		const FMTMask& mask, 
		const std::unique_ptr<Core::FMTYieldHandler>& value)
	{
		std::pair<Core::FMTMask, std::unique_ptr<Core::FMTYieldHandler>> newobject = std::make_pair(mask, std::move(value->clone()));
		data.insert(data.begin() + location, std::move(newobject));
	}

	template<> inline void FMTList<std::unique_ptr<Core::FMTYieldHandler>>::pushToData(
		std::vector<std::pair<FMTMask, 
		std::unique_ptr<Core::FMTYieldHandler>>>& datavector,
		const FMTMask& mask, 
		const std::unique_ptr<Core::FMTYieldHandler>& maskdata) const
	{
		std::pair<Core::FMTMask, std::unique_ptr<Core::FMTYieldHandler>> newobject = std::make_pair(mask, std::move(maskdata->clone()));
		datavector.push_back(std::move(newobject));
	}

	template<> inline void FMTList<std::unique_ptr<Core::FMTYieldHandler>>::push_back(
		const FMTMask& mask, 
		const std::unique_ptr<Core::FMTYieldHandler>& value)
	{
		pushToData(data, mask, value);
	}

	template<> inline void FMTList<std::unique_ptr<Core::FMTYieldHandler>>::copyData(
		const FMTList<std::unique_ptr<Core::FMTYieldHandler>>& rhs)
	{
		data.clear();
		data.reserve(rhs.data.size());
		for (const std::pair<Core::FMTMask, std::unique_ptr<Core::FMTYieldHandler>>& object : rhs.data)
		{
			pushToData(data, object.first, object.second);
		}
	}
}

#endif
