/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMToutputnodecache_H
#define FMToutputnodecache_H

#include<map>
#include<vector>
#include <unordered_map>
#include "FMTOutputNode.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include "FMTTheme.h"
#include "FMTAction.h"
#include "FMTGraph.hpp"



namespace Graph
{
	// DocString: FMTOutputNodeCache
	/**
	@brief Cache of graph vertices for output nodes, keyed by output source, used to speed up output computation on the graph.
	@tparam tvdescriptor the vertex descriptor type.
	@tparam titerator the iterator type over the vertices.
	*/
	template <class tvdescriptor,class titerator>
	class FMTOutputNodeCache
	{
		
	public:
		// DocString: FMTOutputNodeCache()
		/**
		@brief Default constructor for FMTOutputNodeCache.
		*/
		FMTOutputNodeCache()=default;
		// DocString: FMTOutputNodeCache(const FMTOutputNodeCache&)
		/**
		@brief Copy constructor for FMTOutputNodeCache.
		@param[in] rhs the FMTOutputNodeCache to copy.
		*/
		FMTOutputNodeCache(const FMTOutputNodeCache& rhs) = default;
		// DocString: FMTOutputNodeCache::operator=
		/**
		@brief Copy assignment operator for FMTOutputNodeCache.
		@param[in] rhs the FMTOutputNodeCache to copy.
		@return a reference to this FMTOutputNodeCache.
		*/
		FMTOutputNodeCache& operator = (const FMTOutputNodeCache& rhs) = default;
		// DocString: ~FMTOutputNodeCache()
		/**
		@brief Default destructor for FMTOutputNodeCache.
		*/
		~FMTOutputNodeCache() = default;
		// DocString: FMTOutputNodeCache(const std::vector<tvdescriptor>&)
		/**
		@brief Construct a cache from an initial set of nodes.
		@param[in] initialnodes the initial nodes.
		*/
		FMTOutputNodeCache(const std::vector<tvdescriptor>& initialnodes) :
			m_inmemorynodes(initialnodes), m_beginit(nullptr), m_endit(nullptr), m_searchtree(), m_allocator(), m_reserve()
		{
			m_inmemorynodes.shrink_to_fit();
			std::sort(m_inmemorynodes.begin(),m_inmemorynodes.end());
		}
		// DocString: FMTOutputNodeCache(const titerator&, const titerator&, std::allocator<tvdescriptor>&, const size_t&)
		/**
		@brief Construct a cache from a range of nodes, an allocator and a reserve size.
		@param[in] first the first iterator.
		@param[in] last the last iterator.
		@param[in] p_allocator the allocator.
		@param[in] p_reserve the reserve size.
		*/
		FMTOutputNodeCache(const titerator& first, const titerator& last,std::allocator<tvdescriptor>& p_allocator,const size_t& p_reserve) :
			m_inmemorynodes(), m_beginit(&first), m_endit(&last), m_searchtree(),m_allocator(&p_allocator), m_reserve(p_reserve)
		{
			if (m_reserve>0)
				{
				std::vector<tvdescriptor>allocated(*m_allocator);
				allocated.reserve(m_reserve);
				for (titerator it = *m_beginit; it != *m_endit; ++it)
					{
					allocated.push_back(*it);
					}
				m_beginit = nullptr;
				m_endit = nullptr;
				std::sort(allocated.begin(), allocated.end());
				allocated.shrink_to_fit();
				m_inmemorynodes.swap(allocated);
				}

		}
		// DocString: FMTOutputNodeCache::eraseNode
		/**
		@brief Erase a node from the cache.
		@param[in] node the node to erase.
		*/
		void eraseNode(const Core::FMTOutputNode& node)
			{
			m_searchtree.erase(node.source);
			}

		// DocString: FMTOutputNodeCache::contains
		/**
		@brief Return true if the cache contains a node.
		@param[in] node the node.
		@return true if the cache contains the node else false.
		*/
		bool contains(const Core::FMTOutputNode& node) const
			{
			return m_searchtree.find(node.source) != m_searchtree.end();
			}

		// DocString: FMTOutputNodeCache::removeLargest
		/**
		@brief Remove the largest entry of the cache and return the freed memory.
		@return the amount of freed memory in bytes.
		*/
		unsigned long long removeLargest()
		{
			size_t largestsize = 0;
			unsigned long long  removedmemory = 0;
			notecacheit largestiterator = m_searchtree.end();
			for (typename std::map<Core::FMTOutputSource, std::vector<tvdescriptor>>::iterator mapit = m_searchtree.begin(); mapit != m_searchtree.end(); mapit++)
			{
				size_t sizeofvec = mapit->second.size();
				if (sizeofvec > largestsize)
				{
					largestsize = mapit->second.size();
					largestiterator = mapit;
				}

			}
			if (largestiterator != m_searchtree.end())
			{
				removedmemory = largestsize * sizeof(tvdescriptor);
				m_searchtree.erase(largestiterator);
			}
			return removedmemory;
		}
		// DocString: FMTOutputNodeCache::getVertices
		/**
		@brief Return the vertices for a target node, using the cache.
		@param[in] targetnode the target node.
		@param[in] actions the actions.
		@param[in] themes the themes.
		@param[out] exactvecticies true if the returned vertices are an exact match.
		@return the vertices for the target node.
		*/
		const std::vector<tvdescriptor>& getVertices(const Core::FMTOutputNode& targetnode, const std::vector<Core::FMTAction>& actions,
			const std::vector<Core::FMTTheme>&themes, bool& exactvecticies) const
			{
			return this->_getCleanDescriptors(targetnode, actions, themes, exactvecticies);
			}
		// DocString: FMTOutputNodeCache::setValidVertices
		/**
		@brief Set the valid vertices for a target node in the cache.
		@param[in] targetnode the target node.
		@param[in] vertices the vertices to set.
		*/
		void setValidVertices(const Core::FMTOutputNode& targetnode,const std::vector<tvdescriptor>& vertices) const
			{
			m_searchtree[targetnode.source] = vertices;
			m_searchtree[targetnode.source].shrink_to_fit();
			}
		// DocString: FMTOutputNodeCache::clear
		/**
		@brief Clear the cache.
		*/
		void clear()
			{
			m_beginit = nullptr;
			m_endit = nullptr;
			m_inmemorynodes.clear();
			m_searchtree.clear();
			}
		// DocString: FMTOutputNodeCache::rebase
		/**
		@brief Rebase the cache on a new range of developments.
		@param[in] beginofdevs the first iterator.
		@param[in] endofdevs the last iterator.
		*/
		void rebase(const titerator& beginofdevs, const titerator& endofdevs)
			{
			m_beginit = &beginofdevs;
			m_endit = &endofdevs;
			}
		// DocString: FMTOutputNodeCache::insert
		/**
		@brief Insert the content of another cache into this cache.
		@param[in] rhs the cache to insert.
		*/
		void insert(const FMTOutputNodeCache& rhs)
			{
			if (m_beginit==nullptr)
			{
				if (m_inmemorynodes.size() < rhs.m_inmemorynodes.size())
				{
					m_inmemorynodes = rhs.m_inmemorynodes;
				}
			}
			
			m_searchtree.insert(rhs.m_searchtree.begin(), rhs.m_searchtree.end());
			}

		// DocString: FMTOutputNodeCache::_pushToVector
		/**
		@brief Push the nodes of the cache into a vector.
		@param[in,out] refvecs the vector to push into.
		*/
		void _pushToVector(std::vector<tvdescriptor>& refvecs) const
		{
			if (m_beginit!=nullptr)
			{
				for (titerator it = *m_beginit; it != *m_endit; ++it)
				{
					/*if (refvecs.capacity() <= (refvecs.size() + 1))
					{
						std::cout << "problem! "<< refvecs.capacity() <<"\n";
					}*/
					refvecs.push_back(*it);
				}
				refvecs.shrink_to_fit();
				std::sort(refvecs.begin(), refvecs.end());
			}
			else {
				refvecs = m_inmemorynodes;
			}
		}
	private:
		friend class boost::serialization::access;
		// DocString: FMTOutputNodeCache::serialize
		/**
		@brief Serialize the FMTOutputNodeCache for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::make_nvp("inmemorynodes", m_inmemorynodes);
			ar & boost::serialization::make_nvp("searchtree", m_searchtree);
		}
		std::vector<tvdescriptor>m_inmemorynodes;
		titerator const * m_beginit;
		titerator const * m_endit;
		mutable std::map<Core::FMTOutputSource,std::vector<tvdescriptor>>m_searchtree;
		std::allocator<tvdescriptor>* m_allocator;
		size_t m_reserve;
        typedef typename std::map<Core::FMTOutputSource,std::vector<tvdescriptor>>::const_iterator notecacheit;
		
		// DocString: FMTOutputNodeCache::_getCleanDescriptors
		/**
		@brief Return the clean vertex descriptors for a target node, rebuilding from the cache and parent nodes.
		@param[in] targetnode the target node.
		@param[in] actions the actions.
		@param[in] themes the themes.
		@param[out] exactnode true if the returned descriptors are an exact match.
		@return the clean vertex descriptors.
		*/
		const std::vector<tvdescriptor>& _getCleanDescriptors(const Core::FMTOutputNode& targetnode,const std::vector<Core::FMTAction>& actions,
										const std::vector<Core::FMTTheme>&themes, bool& exactnode) const
		{
			exactnode = false;
			bool foundSubset = false;
			typename std::map<Core::FMTOutputSource, std::vector<tvdescriptor>>::const_iterator parent = this->_getParentNode(targetnode,
																						actions, exactnode, foundSubset);
			if (exactnode)
			{
				return parent->second;
			}
			//std::vector<tvdescriptor> cleaned(*m_allocator);
			m_searchtree[targetnode.source] = std::vector<tvdescriptor>(*m_allocator);
			std::vector<tvdescriptor>& cleaned = m_searchtree[targetnode.source];
			if (foundSubset)
			{
				cleaned = parent->second;
			}else {
				cleaned.reserve(m_reserve);
				_pushToVector(cleaned);
			}
			_getActionRebuild(targetnode, actions, cleaned, exactnode);// , TO_RESERVE); // should be able to find also exact!!!!!!!!
			if (!exactnode)
			{
				std::vector<tvdescriptor>toRemove(*m_allocator);
				bool gotSomething = false;
				const Core::FMTMask& targetmask = targetnode.source.getMask();
				for (typename std::map<Core::FMTOutputSource, std::vector<tvdescriptor>>::const_reverse_iterator sit = m_searchtree.rbegin();
					sit != m_searchtree.rend(); sit++)
				{
					const Core::FMTMask& nodemask = sit->first.getMask();
					if (targetmask.isNotThemesSubset(nodemask, themes))//deal only with mask
					{
						if (!gotSomething)
							{
							toRemove.reserve(cleaned.size());
							}
						toRemove.insert(toRemove.end(), sit->second.begin(), sit->second.end());
						gotSomething = true;

					}
				}
				if (!toRemove.empty())
				{
					std::vector<tvdescriptor>difference(*m_allocator);
					difference.reserve(cleaned.size());
					std::sort(toRemove.begin(), toRemove.end());
					std::set_difference(cleaned.begin(), cleaned.end(),
						toRemove.begin(), toRemove.end(), std::inserter(difference, difference.begin()));
					cleaned.swap(difference);
				}
			}
			//std::pair<notecacheit, bool> returniterator;
			//returniterator = searchtree.insert(std::pair<Core::FMTOutputSource, std::vector<tvdescriptor>>(targetnode.source, cleaned));
			//return (returniterator.first)->second;
			return cleaned;
		}
		// DocString: FMTOutputNodeCache::_getActionRebuild
		/**
		@brief Rebuild the descriptors for an aggregate action from the cached descriptors of its member actions.
		@param[in] targetnode the target node.
		@param[in] actions the actions.
		@param[in,out] cleaned the descriptors to rebuild.
		@param[out] exactnode true if an exact match is found.
		*/
		void _getActionRebuild(const Core::FMTOutputNode& targetnode,
			const std::vector<Core::FMTAction>& actions,
			std::vector<tvdescriptor>& cleaned,
			bool& exactnode/*, const size_t& p_reserve*/) const
		{
			const std::string actionname = targetnode.source.getAction();
			const std::vector<const Core::FMTAction*>aggregatesptr = Core::FMTActionComparator(actionname).getAllAggregates(actions, true);
			if (!actionname.empty() && !aggregatesptr.empty()) //so it's a aggregate!
			{
				std::map<std::string, std::vector< notecacheit>>potentials;
				for (const Core::FMTAction* attributeptr : aggregatesptr)
				{
					potentials[attributeptr->getName()] = std::vector< notecacheit>();
					potentials[attributeptr->getName()].reserve(m_reserve);
				}
				for (notecacheit sit = m_searchtree.begin();
					sit != m_searchtree.end(); sit++)
				{
					if (sit->first.isSubsetOf(targetnode.source, actions) && 
						(sit->first != targetnode.source))
					{
						const std::string nodeaction = sit->first.getAction();
						/*if (potentials[nodeaction].capacity() <= (potentials[nodeaction].size() + 1))
						{
							std::cout << "problem!/n";
						}*/
						potentials[nodeaction].push_back(sit);
					}
				}
				for (const Core::FMTAction* attributeptr : aggregatesptr)
				{
					if (potentials.at(attributeptr->getName()).empty())
					{
						return; //not a perfect rebuilt need to be complete!!
					}
				}
				typename std::vector< notecacheit>::const_iterator testting = potentials.begin()->second.begin();
				while (testting != potentials.begin()->second.end())
				{
					size_t attid = 0;
					std::vector<tvdescriptor>finalSelection((*testting)->second);
					size_t insertingdone = 1;
					for (const auto& attribute : potentials)
					{
						if (attid != 0)
						{
							for (notecacheit it : potentials.at(attribute.first))
							{
								if ((*testting)->first.isSameButDifferentAction(it->first))
								{
									finalSelection.insert(finalSelection.end(), it->second.begin(), it->second.end());
									++insertingdone;
									break;
								}

							}

						}
						++attid;
					}
					if (insertingdone == potentials.size())
					{
						std::sort(finalSelection.begin(), finalSelection.end());
						//Weird fix BF
						finalSelection.erase(std::unique(finalSelection.begin(), finalSelection.end()),finalSelection.end());
						if ((*testting)->first.isSameButDifferentAction(targetnode.source)) //we got a exact match!!!
						{
							exactnode = true;
							cleaned = finalSelection;
						}
						else {
							std::vector<tvdescriptor>intersection;
							std::set_intersection(cleaned.begin(), cleaned.end(),
								finalSelection.begin(), finalSelection.end(), std::inserter(intersection, intersection.begin()));
							cleaned = intersection;
						}
					}
					++testting;
				}


			}
		}
		// DocString: FMTOutputNodeCache::_getParentNode
		/**
		@brief Return the parent node of a target node in the cache, an exact match or a subset.
		@param[in] m_targetNode the target node.
		@param[in] m_actions the actions.
		@param[out] m_exactNode true if an exact match is found.
		@param[in] m_foundSubset whether a subset was found.
		@return an iterator to the parent node, or the end of the search tree.
		*/
		notecacheit _getParentNode(const Core::FMTOutputNode& m_targetNode,
							const std::vector<Core::FMTAction>& m_actions,
						bool& m_exactNode, bool m_foundSubset) const
			{
				notecacheit parentit = m_searchtree.find(m_targetNode.source);
				if (parentit != m_searchtree.end())
				{
					m_exactNode = true;
					return parentit;
				}
				parentit = m_searchtree.begin();
				m_exactNode = false;
				m_foundSubset = false;
				while (parentit != m_searchtree.end())
				{
					if (m_targetNode.source.isSubsetOf(parentit->first, m_actions))
					{
						m_foundSubset = true;
						return parentit;
					}
					++parentit;
				}
				return m_searchtree.end();
			}
	};


	
}

#endif
