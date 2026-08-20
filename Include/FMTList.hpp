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

#include "FMTMask.h"
#include "FMTMaskFilter.h"
#include <boost/serialization/version.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/hash_collections_save_imp.hpp>
#include <boost/serialization/hash_collections_load_imp.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include "FMTExceptionHandler.h"
#include "FMTObject.h"
#include <iterator>
#include <memory>
#include "FMTYieldHandler.h"
#include "FMTTheme.h"


namespace Core
{
	// DocString: FMTList
	/**
	@brief Dictionary-like container keeping track of objects tagged with a FMTMask, used by the yield, action and transition classes.
	@details Uses caching and a mask filter to reduce the size of the list and provide fast access to the objects.
	@tparam T the type of the objects held by the list.
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
		@brief Append another list to this list; both lists must be non shrunk otherwise an exception is thrown.
		@param[in] OtherList the other list to append to this one.
		@return a reference to this appended list.
		*/
		Core::FMTList<T>& operator+= (const Core::FMTList<T>& OtherList)
		{
			try {
				if (!this->canShrink()||!OtherList.canShrink())
					{
					_exhandler->raise(Exception::FMTexc::FMTinvalid_action, "Cant append list together",
						"FMTList::operator::+=", __LINE__, __FILE__);
					}
				for (const std::pair<FMTMask, T>& Object : OtherList.m_data)
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
		@brief Swap this list with another.
		@param[in,out] rhs the list to swap with.
		*/
		void swap(Core::FMTList<T>& rhs)
		{
			m_data.swap(rhs.m_data);
			m_filter.swap(rhs.m_filter);
			m_fastpass.swap(rhs.m_fastpass);
		}
		// DocString: FMTList::getUnion
		/**
		@brief Return the union mask of all the masks of the list.
		@param[in] themes the themes.
		@return the union mask.
		*/
		FMTMask getUnion(const std::vector<FMTTheme>& themes) const
		{
			auto it = this->begin();
			FMTMask result(
				it->first.getStringReference(),
				themes);

			++it;
			for (; it != this->end(); ++it)
			{
				FMTMask Mask(
					it->first.getStringReference(),
					themes);
				result.unionWith(Mask);
			}
			return result;
		}
		// DocString: FMTList()
		/**
		@brief Default constructor for FMTList.
		*/
		FMTList() :
			FMTObject(),
			m_data(),
			m_filter(),
			m_fastpass() {};
		// DocString: FMTList(const FMTList&)
		/**
		@brief Copy constructor for FMTList.
		@param[in] rhs the FMTList to copy.
		*/
		FMTList(const FMTList<T>& rhs) :
			FMTObject(rhs),
			m_data(),
			m_filter(rhs.m_filter),
			m_fastpass(rhs.m_fastpass)
		{
			_copyData(rhs);
		}
		// DocString: FMTList::reserve
		/**
		@brief Reserve memory based on a other FMTList
		@param[in] p_other FMTList.
		*/
		void reserve(const FMTList<T>& p_other)
			{
			m_data.reserve(p_other.m_data.size());
			m_fastpass.reserve(p_other.m_data.size());
			}
		// DocString: FMTList::clear
		/**
		@brief clear the data
		*/
		void clear()
			{
			m_data.clear();
			m_fastpass.clear();
			}
		// DocString: FMTList::operator=
		/**
		@brief Copy assignment operator for FMTList.
		@param[in] rhs the FMTList to copy.
		@return a reference to this FMTList.
		*/
		FMTList& operator = (const FMTList<T>& rhs)
		{
			if (this != &rhs)
			{
				FMTObject::operator=(rhs);
				_copyData(rhs);
				m_filter = rhs.m_filter;
				m_fastpass = rhs.m_fastpass;
			}
			return *this;
		}
		// DocString: FMTList::operator==
		/**
		@brief Equality comparison operator of FMTList.
		@param[in] rhs the list to compare with.
		@return true if both lists are equal else false.
		*/
		bool operator == (const FMTList<T>& rhs) const
		{
			return (m_data == rhs.m_data);
		}
		// DocString: ~FMTList()
		/**
		@brief Default destructor for FMTList.
		*/
		~FMTList() = default;
		// DocString: FMTList::empty
		/**
		@brief Return true if the list is empty.
		@return true if the list is empty else false.
		*/
		bool empty() const
		{
			return m_data.empty();
		}
		// DocString: FMTList::canShrink
		/**
		@brief Return true if the list can be shrunk.
		@return true if the list can be shrunk else false.
		*/
		bool canShrink() const
		{
			return m_filter.empty();
		}
		// DocString: FMTList::clearCache
		/**
		@brief Clear the fast pass cache by swapping it with an empty container.
		*/
		virtual void clearCache()
		{
			boost::unordered_map<FMTMask, std::vector<int>>().swap(m_fastpass);
		}
		// DocString: FMTList::size
		/**
		@brief Return the size of the list.
		@return the size of the list.
		*/
		size_t size() const
		{
			return m_data.size();
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
				const FMTMask NEW_KEY = m_filter.filter(p_mask);
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
				boost::unordered_map<FMTMask, std::vector<int>>::const_iterator fast_it = m_fastpass.find(p_newKey);
				//const int SIZE_OF_DATE = static_cast<int>(size());
				if (fast_it != m_fastpass.end())
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
					m_fastpass[p_newKey] = std::vector<int>();
					int location = 0;
					for (const std::pair<FMTMask, T>& object : m_data)
					{
						if (p_newKey.isSubsetOf(object.first))
						{
							m_fastpass[p_newKey].push_back(location);
							allhits.push_back(BEGINNING + location);
						}
						++location;
					}
					m_fastpass[p_newKey].shrink_to_fit();
				}
			}catch (...) 
				{
				_exhandler->raiseFromCatch("", "FMTList::findSetsWithFiltered", __LINE__, __FILE__);
				}
			return allhits;
		}
		// DocString: FMTList::filterMask
		/**
		@brief Shrink a mask using the global filter of the list.
		@param[in] baseMask the mask to filter.
		@return the filtered mask.
		*/
		inline FMTMask filterMask(const FMTMask& baseMask) const
		{
			return m_filter.filter(baseMask);
		}
		// DocString: FMTList::shrink
		/**
		@brief Reduce the size of the masks used in the list to lower the complexity.
		*/
		void shrink()
		{
			try {
				if (!m_data.empty())
				{
					m_fastpass.clear();
					std::vector<Core::FMTMask> filteredmasks;
					filteredmasks.reserve(m_data.size());
					for (const std::pair<FMTMask, T>& object : m_data)
					{
						filteredmasks.push_back(object.first);
					}
					if (filteredmasks.empty())
					{
						_exhandler->raise(Exception::FMTexc::FMTinvalid_maskrange, "Empty mask", "FMTActionParser::shrink", __LINE__, __FILE__);
					}
					m_filter = Core::FMTMaskFilter(filteredmasks);
					for (std::pair<FMTMask, T>& object : m_data)
					{
						object.first = m_filter.filter(object.first);
					}
					m_data.shrink_to_fit();
				}
				
			}
			catch (...) {

				_exhandler->raiseFromCatch("", "FMTList::shrink", __LINE__, __FILE__);

			}
		}
		// DocString: FMTList::unShrink
		/**
		@brief Bring back the complexity of each mask into the list.
		@param[in] themes the themes.
		*/
		void unShrink(const std::vector<FMTTheme>& themes)
		{
			m_fastpass.clear();
			m_filter = Core::FMTMaskFilter();
			for (std::pair<FMTMask, T>& object : m_data)
			{
				object.first = FMTMask(std::string(object.first), themes);
			}
		}
		// DocString: FMTList::apiPushBack
		/**
		@brief Push back an element, used for template specification to overcome the static cast of the function.
		@param[in] mask the mask.
		@param[in] value the value.
		*/
		void apiPushBack(const FMTMask& mask, const T& value)
		{
			push_back(mask, value);
		}
		// DocString: FMTList::push_back
		/**
		@brief Push back an element at the end of the list.
		@param[in] mask the mask.
		@param[in] value the value.
		*/
		void push_back(const FMTMask& mask, const T& value)
		{
			m_data.emplace_back(mask, value);
		}
		// DocString: FMTList::update
		/**
		@brief Update the filter of the list and shrink it if any change occurred.
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
		@brief Push back a whole list at the end of this list.
		@param[in] rhs the list to append.
		*/
		void push_back(const FMTList<T>& rhs)
		{
			if (this->canShrink() && rhs.canShrink())
			{
				_copyData(rhs);
				this->shrink();
			}
		}
		// DocString: FMTList::push_front
		/**
		@brief Push an element at the beginning of the list.
		@param[in] mask the mask.
		@param[in] value the value.
		*/
		void push_front(const FMTMask& mask, const T& value)
		{
			insert(0, mask, value);
		}
		// DocString: FMTList::pop_back
		/**
		@brief Pop the element at the end of the list.
		*/
		void pop_back()
		{
			m_data.pop_back();
		}
		// DocString: FMTList::erase
		/**
		@brief Erase a specific element of the list.
		@param[in] location the location of the element to erase.
		*/
		void erase(const size_t& location)
		{
			m_data.erase(m_data.begin() + location);
		}
		// DocString: FMTList::insert
		/**
		@brief Insert an element in the list at a specific location.
		@param[in] location the location.
		@param[in] mask the mask.
		@param[in] value the value.
		*/
		void insert(const size_t& location, const FMTMask& mask, const T& value)
		{
			m_data.insert(m_data.begin() + location, std::pair<FMTMask, T>(mask, value));
		}
		
		// DocString: FMTList::begin
		/**
		@brief Return an iterator to the beginning of the list.
		@return an iterator to the beginning.
		*/
		iterator begin()
		{
			return m_data.begin();
		}
		// DocString: FMTList::begin
		/**
		@brief Return a const iterator to the beginning of the list.
		@return a const iterator to the beginning.
		*/
		const_iterator begin() const
		{
			return m_data.begin();
		}
		// DocString: FMTList::end
		/**
		@brief Return an iterator to the end of the list.
		@return an iterator to the end.
		*/
		iterator  end()
		{
			return m_data.end();
		}
		// DocString: FMTList::end
		/**
		@brief Return a const iterator to the end of the list.
		@return a const iterator to the end.
		*/
		const_iterator end() const
		{
			return m_data.end();
		}
	protected:
		// DocString: FMTList::_compressMasks
		/**
		@brief Compress multiple masks into a single one when they hold the same data, generating aggregates and changing the themes, used during preSolve for actions and transitions.
		@param[in,out] newthemes the themes, modified by the compression.
		*/
		void _compressMasks(std::vector<FMTTheme>& newthemes)
			{
			try {
				if (size()==1)
				{
					return;
				}
				unShrink(newthemes);
				boost::dynamic_bitset<uint8_t>selectedbits(m_data.begin()->first.size(), true);
				boost::dynamic_bitset<uint8_t>reverSelect(selectedbits.size());
				std::string newmask;
				newmask.reserve(newthemes.size() * 10);
				size_t thstart = 0;
				for (FMTTheme& theme : newthemes)
				{
					const size_t THEME_END = (theme.size() + thstart);
					std::vector<std::pair<FMTMask, T>>newvecdata;
					newvecdata.reserve(m_data.size());
					std::list<std::pair<FMTMask, T>>newData(m_data.begin(), m_data.end());
					selectedbits.set();
					for (size_t loc = thstart; loc < THEME_END; ++loc)
					{
						selectedbits[loc] = false;
					}
					std::vector<typename std::list<std::pair<FMTMask, T>>::iterator>toRemove;
					toRemove.reserve(newData.size());
					while (!newData.empty())
						{
						typename std::list<std::pair<FMTMask, T>>::iterator baseit = newData.begin();
						typename std::list<std::pair<FMTMask, T>>::iterator datait = newData.begin();
						++datait;
						toRemove.clear();
						Core::FMTMask baseMask(baseit->first);
						const auto& BASE_SELECT =
							baseit->first.getBitsetReference();
						reverSelect.reset();
						for (size_t bit = selectedbits.find_first();
							bit != boost::dynamic_bitset<uint8_t>::npos;
							bit = selectedbits.find_next(bit))
						{
							if (BASE_SELECT[bit])
							{
								reverSelect[bit] = true;
							}
						}
						for (size_t bit = thstart; bit < THEME_END; ++bit)
						{
							reverSelect[bit] = true;
						}
						while (datait!=newData.end())
							{
							if (_canAggregate(
								*baseit,
								*datait,
								selectedbits,
								reverSelect))
								{
								baseMask.unionWith(datait->first);
								toRemove.push_back(datait);
								}	
							++datait;
							}
						auto DataCopy = baseit->second;
						newData.erase(newData.begin());
						for (typename std::list<std::pair<FMTMask, T>>::iterator remove : toRemove)
							{
							newData.erase(remove);
							}
						if(!toRemove.empty())//aggregation done set new aggregate and refresh mask
							{
							newmask.clear();
							for (const FMTTheme& subtheme : newthemes)
								{
								if (subtheme==theme)
									{
									newmask += theme.updateFromMask(baseMask);
									newmask += ' ';
								}else {
									newmask += baseMask.get(subtheme);
									newmask += ' ';
									}
								}
							newmask.pop_back();
							baseMask = Core::FMTMask(newmask, newthemes);
							}
						newvecdata.emplace_back(
							std::move(baseMask),
							std::move(DataCopy));
						}
					thstart += theme.size();
					m_data.swap(newvecdata);
				}
				shrink();
			}catch (...)
					{
				_exhandler->raiseFromCatch("", "compressMasks", __LINE__, __FILE__);
				}
			}
		// DocString: FMTList::_preSolveList
		/**
		@brief Reduce the number of elements in the list by deleting those not represented in the base mask.
		@details Use with care because it changes the state of the list; referencing a deleted element makes the model seem broken.
		@param[in] filter the mask filter.
		@param[in] originalthemes the original themes.
		@param[in] newthemes the presolved themes.
		*/
		void _preSolveList(
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
				size_t dataId = m_data.size();
				while (dataId!=0)
					{
					--dataId;
					if (!filter.emptyFlipped() && filter.canPreSolve(m_data[dataId].first, maskthemes))
						{
						m_data[dataId].first.preSolveRef(filter, newthemes,false);
					}else {
						m_data.erase(m_data.begin() + dataId);
						}
					}
				FMTList::update();
				//data.shrink_to_fit();
				/*
				std::vector<std::pair<FMTMask, T>>newData;
				newData.reserve(m_data.size());
				for (const std::pair<FMTMask, T>& object : m_data)
				{
					if (filter.canPreSolve(object.first,maskthemes))
					{
						FMTMask mskkey = object.first;
						if (!filter.emptyFlipped())
						{
							mskkey = mskkey.preSolve(filter, newthemes);
						}
						pushToData(newData, mskkey, object.second);
					}
				}
				m_data.swap(newData);*/
				//FMTList::update();
				//data.shrink_to_fit();
			}catch (...)
				{
				_exhandler->raiseFromCatch("","FMTList::_preSolveList", __LINE__, __FILE__);
				}
			}
		// DocString: FMTList::_copyData
		/**
		@brief Copy the data from another list.
		@param[in] rhs the list to copy the data from.
		*/
		void _copyData(const Core::FMTList<T>& rhs)
			{
			m_data = rhs.m_data;
			}
	private:
		// DocString: FMTList::save
		/**
		@brief Save function used for serialization to do multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to save to.
		@param[in] version the serialization version.
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void save(Archive& ar, const unsigned int version) const
		{
			try {
				ar& boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTObject>(*this));
				ar& boost::serialization::make_nvp("data", m_data);
				ar& boost::serialization::make_nvp("filter", m_filter);
				std::vector<std::pair<FMTMask, std::vector<int>>>vecFastPass(m_fastpass.begin(), m_fastpass.end());
				ar& BOOST_SERIALIZATION_NVP(vecFastPass);
			}
			catch (...)
			{
				_exhandler->printExceptions("", "FMTList::save", __LINE__, __FILE__);
			}
		}
		// DocString: FMTList::load
		/**
		@brief Load function used for serialization to do multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to load from.
		@param[in] version the serialization version.
		*/
		template<class Archive>
		void load(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("data", m_data);
			ar& boost::serialization::make_nvp("filter", m_filter);
			std::vector<std::pair<FMTMask, std::vector<int>>>vecFastPass;
			ar& BOOST_SERIALIZATION_NVP(vecFastPass);
			for (const std::pair<FMTMask, std::vector<int>>& values : vecFastPass)
			{
				m_fastpass[values.first] = values.second;
			}
		}
		BOOST_SERIALIZATION_SPLIT_MEMBER()
			// DocString: FMTList::m_data
			///The container holding the data and the masks of the FMTList.
			std::vector<std::pair<FMTMask, T>>m_data;
		// DocString: FMTList::m_filter
		///The mask filter used by the FMTList to shrink the original FMTMask.
		FMTMaskFilter m_filter;
		// DocString: FMTList::m_fastpass
		///unordered_map used to do caching of mask subsets of the FMTList.
		mutable boost::unordered_map<FMTMask, std::vector<int>>m_fastpass;
		// DocString: FMTList::pushToData
		/**
		@brief Push data into a vector.
		@param[in,out] datavector the vector to push into.
		@param[in] mask the mask.
		@param[in] maskdata the data.
		*/
		void pushToData(std::vector<std::pair<FMTMask, T>>& datavector,
			const FMTMask& mask, const T& maskdata) const
		{
			datavector.push_back(std::pair<FMTMask, T>(mask, maskdata));
		}
		/**
		 * @brief Determines whether a candidate entry can be aggregated with a
		 *        base entry during mask compression.
		 *
		 * Two entries can be aggregated when:
		 * - The candidate mask is a subset of p_reverseSelect.
		 * - The candidate mask matches the base mask on all bits selected by
		 *   p_selectedBits.
		 * - Both entries have the same associated value.
		 *
		 * @param p_baseData Base entry used as the aggregation reference.
		 * @param p_candidateData Entry being evaluated for aggregation.
		 * @param p_selectedBits Bitset identifying the dimensions that must match.
		 * @param p_selectedIntersection Intersection of p_selectedBits and the
		 *        base mask bitset.
		 * @param p_reverseSelect Bitset used to validate the candidate mask
		 *        subset relationship.
		 *
		 * @return True if the candidate entry can be merged with the base entry;
		 *         otherwise false.
		 */
		bool _canAggregate(
			const std::pair<FMTMask, T>& p_baseData,
			const std::pair<FMTMask, T>& p_candidateData,
			const boost::dynamic_bitset<uint8_t>& p_selectedBits,
			const boost::dynamic_bitset<uint8_t>& p_reverseSelect) const
		{
			const auto& baseMask =
				p_baseData.first.getBitsetReference();

			const auto& candidateMask =
				p_candidateData.first.getBitsetReference();

			if (!candidateMask.is_subset_of(p_reverseSelect))
			{
				return false;
			}

			if (!(p_baseData.second == p_candidateData.second))
			{
				return false;
			}

			for (size_t bit = p_selectedBits.find_first();
				bit != boost::dynamic_bitset<uint8_t>::npos;
				bit = p_selectedBits.find_next(bit))
			{
				if (baseMask[bit] != candidateMask[bit])
				{
					return false;
				}
			}

			return true;
		}

	};

	template<> inline void FMTList<std::unique_ptr<Core::FMTYieldHandler>>::_compressMasks(std::vector<FMTTheme>& newthemes)
	{

	}

	template<> inline void FMTList<std::unique_ptr<Core::FMTYieldHandler>>::insert(
		const size_t& location, 
		const FMTMask& mask, 
		const std::unique_ptr<Core::FMTYieldHandler>& value)
	{
		std::pair<Core::FMTMask, std::unique_ptr<Core::FMTYieldHandler>> newobject = std::make_pair(mask, std::move(value->clone()));
		m_data.insert(m_data.begin() + location, std::move(newobject));
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
		pushToData(m_data, mask, value);
	}

	template<> inline void FMTList<std::unique_ptr<Core::FMTYieldHandler>>::_copyData(
		const FMTList<std::unique_ptr<Core::FMTYieldHandler>>& rhs)
	{
		m_data.clear();
		m_data.reserve(rhs.m_data.size());
		for (const std::pair<Core::FMTMask, std::unique_ptr<Core::FMTYieldHandler>>& object : rhs.m_data)
		{
			pushToData(m_data, object.first, object.second);
		}
	}
}

#endif
