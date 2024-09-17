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
#include "FMToutputnode.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include "FMTtheme.h"
#include "FMTaction.h"
#include "FMTgraph.hpp"



namespace Graph
{
	template <class tvdescriptor,class titerator>
	class FMToutputnodecache
	{
		
	public:
		FMToutputnodecache()=default;
		FMToutputnodecache(const FMToutputnodecache& rhs) = default;
		FMToutputnodecache& operator = (const FMToutputnodecache& rhs) = default;
		~FMToutputnodecache() = default;
		FMToutputnodecache(const std::vector<tvdescriptor>& initialnodes) :
			inmemorynodes(initialnodes), beginit(nullptr), endit(nullptr), searchtree(), m_allocator(), m_reserve()
		{
			inmemorynodes.shrink_to_fit();
			std::sort(inmemorynodes.begin(),inmemorynodes.end());
		}
		FMToutputnodecache(const titerator& first, const titerator& last,std::allocator<tvdescriptor>& p_allocator,const size_t& p_reserve) :
			inmemorynodes(), beginit(&first), endit(&last), searchtree(),m_allocator(&p_allocator), m_reserve(p_reserve)
		{
			if (m_reserve>0)
				{
				std::vector<tvdescriptor>allocated(*m_allocator);
				allocated.reserve(m_reserve);
				for (titerator it = *beginit; it != *endit; ++it)
					{
					allocated.push_back(*it);
					}
				beginit = nullptr;
				endit = nullptr;
				std::sort(allocated.begin(), allocated.end());
				allocated.shrink_to_fit();
				inmemorynodes.swap(allocated);
				}

		}
		void erasenode(const Core::FMToutputnode& node)
			{
			searchtree.erase(node.source);
			}

		bool contains(const Core::FMToutputnode& node) const
			{
			return searchtree.find(node.source) != searchtree.end();
			}

		unsigned long long removelargest()
		{
			size_t largestsize = 0;
			unsigned long long  removedmemory = 0;
			notecacheit largestiterator = searchtree.end();
			for (typename std::map<Core::FMToutputsource, std::vector<tvdescriptor>>::iterator mapit = searchtree.begin(); mapit != searchtree.end(); mapit++)
			{
				size_t sizeofvec = mapit->second.size();
				if (sizeofvec > largestsize)
				{
					largestsize = mapit->second.size();
					largestiterator = mapit;
				}

			}
			if (largestiterator != searchtree.end())
			{
				removedmemory = largestsize * sizeof(tvdescriptor);
				searchtree.erase(largestiterator);
			}
			return removedmemory;
		}
		const std::vector<tvdescriptor>& getverticies(const Core::FMToutputnode& targetnode, const std::vector<Core::FMTaction>& actions,
			const std::vector<Core::FMTtheme>&themes, bool& exactvecticies) const
			{
			return this->getcleandescriptors(targetnode, actions, themes, exactvecticies);
			}
		void setvalidverticies(const Core::FMToutputnode& targetnode,const std::vector<tvdescriptor>& verticies) const
			{
			searchtree[targetnode.source] = verticies;
			searchtree[targetnode.source].shrink_to_fit();
			}
		void clear()
			{
			beginit = nullptr;
			endit = nullptr;
			inmemorynodes.clear();
			searchtree.clear();
			}
		void rebase(const titerator& beginofdevs, const titerator& endofdevs)
			{
			beginit = &beginofdevs;
			endit = &endofdevs;
			}
		void insert(const FMToutputnodecache& rhs)
			{
			if (beginit==nullptr)
			{
				if (inmemorynodes.size() < rhs.inmemorynodes.size())
				{
					inmemorynodes = rhs.inmemorynodes;
				}
			}
			
			searchtree.insert(rhs.searchtree.begin(), rhs.searchtree.end());
			}

		void pushtovector(std::vector<tvdescriptor>& refvecs) const
		{
			if (beginit!=nullptr)
			{
				for (titerator it = *beginit; it != *endit; ++it)
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
				refvecs = inmemorynodes;
			}
		}
	private:
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & BOOST_SERIALIZATION_NVP(inmemorynodes);
			ar & BOOST_SERIALIZATION_NVP(searchtree);
		}
		std::vector<tvdescriptor>inmemorynodes;
		titerator const * beginit;
		titerator const * endit;
		mutable std::map<Core::FMToutputsource,std::vector<tvdescriptor>>searchtree;
		std::allocator<tvdescriptor>* m_allocator;
		size_t m_reserve;
        typedef typename std::map<Core::FMToutputsource,std::vector<tvdescriptor>>::const_iterator notecacheit;
		
		const std::vector<tvdescriptor>& getcleandescriptors(const Core::FMToutputnode& targetnode,const std::vector<Core::FMTaction>& actions,
										const std::vector<Core::FMTtheme>&themes, bool& exactnode) const
		{
			exactnode = false;
			bool foundSubset = false;
			typename std::map<Core::FMToutputsource, std::vector<tvdescriptor>>::const_iterator parent = this->_getParentNode(targetnode,
																						actions, exactnode, foundSubset);
			if (exactnode)
			{
				return parent->second;
			}
			//std::vector<tvdescriptor> cleaned(*m_allocator);
			searchtree[targetnode.source] = std::vector<tvdescriptor>(*m_allocator);
			std::vector<tvdescriptor>& cleaned = searchtree[targetnode.source];
			if (foundSubset)
			{
				cleaned = parent->second;
			}else {
				cleaned.reserve(m_reserve);
				pushtovector(cleaned);
			}
			getactionrebuild(targetnode, actions, cleaned, exactnode);// , TO_RESERVE); // should be able to find also exact!!!!!!!!
			if (!exactnode)
			{
				std::vector<tvdescriptor>toremove(*m_allocator);
				toremove.reserve(m_reserve);
				const Core::FMTmask& targetmask = targetnode.source.getmask();
				for (typename std::map<Core::FMToutputsource, std::vector<tvdescriptor>>::const_reverse_iterator sit = searchtree.rbegin();
					sit != searchtree.rend(); sit++)
				{
					const Core::FMTmask& nodemask = sit->first.getmask();
					if (targetmask.isnotthemessubset(nodemask, themes))//deal only with mask
					{
						toremove.insert(toremove.end(), sit->second.begin(), sit->second.end());

					}
				}
				if (!toremove.empty())
				{
					std::vector<tvdescriptor>difference(*m_allocator);
					difference.reserve(m_reserve);
					std::sort(toremove.begin(), toremove.end());
					std::set_difference(cleaned.begin(), cleaned.end(),
						toremove.begin(), toremove.end(), std::inserter(difference, difference.begin()));
					cleaned.swap(difference);
				}
			}
			//std::pair<notecacheit, bool> returniterator;
			//returniterator = searchtree.insert(std::pair<Core::FMToutputsource, std::vector<tvdescriptor>>(targetnode.source, cleaned));
			//return (returniterator.first)->second;
			return cleaned;
		}
		void getactionrebuild(const Core::FMToutputnode& targetnode,
			const std::vector<Core::FMTaction>& actions,
			std::vector<tvdescriptor>& cleaned,
			bool& exactnode/*, const size_t& p_reserve*/) const
		{
			const std::string actionname = targetnode.source.getaction();
			const std::vector<const Core::FMTaction*>aggregatesptr = Core::FMTactioncomparator(actionname).getallaggregates(actions, true);
			if (!actionname.empty() && !aggregatesptr.empty()) //so it's a aggregate!
			{
				std::map<std::string, std::vector< notecacheit>>potentials;
				for (const Core::FMTaction* attributeptr : aggregatesptr)
				{
					potentials[attributeptr->getname()] = std::vector< notecacheit>();
					potentials[attributeptr->getname()].reserve(m_reserve);
				}
				for (notecacheit sit = searchtree.begin();
					sit != searchtree.end(); sit++)
				{
					if (sit->first.issubsetof(targetnode.source, actions) && 
						(sit->first != targetnode.source))
					{
						const std::string nodeaction = sit->first.getaction();
						/*if (potentials[nodeaction].capacity() <= (potentials[nodeaction].size() + 1))
						{
							std::cout << "problem!/n";
						}*/
						potentials[nodeaction].push_back(sit);
					}
				}
				for (const Core::FMTaction* attributeptr : aggregatesptr)
				{
					if (potentials.at(attributeptr->getname()).empty())
					{
						return; //not a perfect rebuilt need to be complete!!
					}
				}
				typename std::vector< notecacheit>::const_iterator testting = potentials.begin()->second.begin();
				while (testting != potentials.begin()->second.end())
				{
					size_t attid = 0;
					std::vector<tvdescriptor>finalselection((*testting)->second);
					size_t insertingdone = 1;
					for (const auto& attribute : potentials)
					{
						if (attid != 0)
						{
							for (notecacheit it : potentials.at(attribute.first))
							{
								if ((*testting)->first.issamebutdifferentaction(it->first))
								{
									finalselection.insert(finalselection.end(), it->second.begin(), it->second.end());
									++insertingdone;
									break;
								}

							}

						}
						++attid;
					}
					if (insertingdone == potentials.size())
					{
						std::sort(finalselection.begin(), finalselection.end());
						//Weird fix BF
						finalselection.erase(std::unique(finalselection.begin(), finalselection.end()),finalselection.end());
						if ((*testting)->first.issamebutdifferentaction(targetnode.source)) //we got a exact match!!!
						{
							exactnode = true;
							cleaned = finalselection;
						}
						else {
							std::vector<tvdescriptor>intersection;
							std::set_intersection(cleaned.begin(), cleaned.end(),
								finalselection.begin(), finalselection.end(), std::inserter(intersection, intersection.begin()));
							cleaned = intersection;
						}
					}
					++testting;
				}


			}
		}
		notecacheit _getParentNode(const Core::FMToutputnode& m_targetNode,
							const std::vector<Core::FMTaction>& m_actions,
						bool& m_exactNode, bool m_foundSubset) const
			{
				notecacheit parentit = searchtree.find(m_targetNode.source);
				if (parentit != searchtree.end())
				{
					m_exactNode = true;
					return parentit;
				}
				parentit = searchtree.begin();
				m_exactNode = false;
				m_foundSubset = false;
				while (parentit != searchtree.end())
				{
					if (m_targetNode.source.issubsetof(parentit->first, m_actions))
					{
						m_foundSubset = true;
						return parentit;
					}
					++parentit;
				}
				return searchtree.end();
			}
	};


	
}

#endif
