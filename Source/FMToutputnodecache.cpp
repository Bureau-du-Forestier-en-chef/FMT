/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
/*
#include "FMTOutputNodeCache.h"
#include <algorithm>

namespace Graph
{

	void FMTOutputNodeCache::setinitialcache(const std::unordered_map<size_t, FMTvertex_descriptor>& initialgraph)
			{
			searchtree.clear();
			baseNode.resize(initialgraph.size());
			size_t idv = 0;
			for (std::unordered_map<size_t, FMTvertex_descriptor>::const_iterator itgraph = initialgraph.begin();
				itgraph!= initialgraph.end();itgraph++)
				{
				baseNode[idv] = itgraph->second;
				++idv;
				}
			std::sort(baseNode.begin(), baseNode.end());
			}

	FMTOutputNodeCache::FMTOutputNodeCache(const std::unordered_map<size_t, FMTvertex_descriptor>& initialgraph):
		baseNode(), searchtree()
		{
		this->setinitialcache(initialgraph);
		}

	std::map<Core::FMTOutputNode, std::vector<FMTvertex_descriptor>>::const_iterator FMTOutputNodeCache::getparentnode(const Core::FMTOutputNode& targetnode, const std::vector<Core::FMTAction>& actions, bool& exactnode) const
		{
		std::map<Core::FMTOutputNode, std::vector<FMTvertex_descriptor>>::const_iterator parentit = searchtree.find(targetnode);
		if (parentit!=searchtree.end())
			{
			exactnode = true;
			return parentit;
			}
		parentit = searchtree.begin();
		exactnode = false;
		while (parentit != searchtree.end())
			{
			if (targetnode.isSubsetOf(parentit->first, actions))
				{
				return parentit;
				}
			++parentit;
			}
		return searchtree.end();
		}

	void FMTOutputNodeCache::getActionRebuild(const Core::FMTOutputNode& targetnode,
											const std::vector<Core::FMTAction>& actions,
											std::vector<FMTvertex_descriptor>& cleaned,
											bool& exactnode) const
		{
		const std::string actionname = targetnode.source.getAction();
		const std::vector<const Core::FMTAction*>aggregatesptr = Core::FMTActionComparator(actionname).getAllAggregates(actions,true);
		if (!actionname.empty() && !aggregatesptr.empty()) //so it's a aggregate!
			{
			std::map<std::string,std::vector< std::map<Core::FMTOutputNode, std::vector<FMTvertex_descriptor>>::const_iterator>>potentials;
			for (const Core::FMTAction* attributeptr : aggregatesptr)
				{
				potentials[attributeptr->getName()] = std::vector< std::map<Core::FMTOutputNode, std::vector<FMTvertex_descriptor>>::const_iterator>();
				}
			for (std::map<Core::FMTOutputNode, std::vector<FMTvertex_descriptor>>::const_iterator sit = searchtree.begin();
					sit != searchtree.end(); sit++) 
				{
				if (sit->first.isSubsetOf(targetnode,actions))
					{
					const std::string nodeaction= sit->first.source.getAction();
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
			std::vector< std::map<Core::FMTOutputNode, std::vector<FMTvertex_descriptor>>::const_iterator>::const_iterator testting = potentials.begin()->second.begin();
			while (testting!= potentials.begin()->second.end())
				{
				size_t attid = 0;
				std::vector<FMTvertex_descriptor>finalSelection((*testting)->second);
				size_t insertingdone = 1;
				for (const auto& attribute : potentials)
					{
					if (attid!=0)
						{
						for (std::map<Core::FMTOutputNode, std::vector<FMTvertex_descriptor>>::const_iterator it : potentials.at(attribute.first))
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
					if ((*testting)->first.isSameButDifferentAction(targetnode)) //we got a exact match!!!
						{
						exactnode = true;
						cleaned = finalSelection;
					}else {
						std::vector<FMTvertex_descriptor>intersection;
						std::set_intersection(cleaned.begin(), cleaned.end(),
							finalSelection.begin(), finalSelection.end(), std::inserter(intersection, intersection.begin()));
						cleaned = intersection;
						}
					}
				++testting;
				}


			}
		}

	const std::vector<FMTvertex_descriptor>& FMTOutputNodeCache::getCleanDescriptors(const Core::FMTOutputNode& targetnode,
																					const std::vector<Core::FMTAction>& actions,
																					const std::vector<Core::FMTTheme>&themes,
																					bool& exactnode) const
		{
		bool exact = false;
		std::map<Core::FMTOutputNode, std::vector<FMTvertex_descriptor>>::const_iterator parent = this->getparentnode(targetnode,actions, exact);
		if (exact)
			{
			return parent->second;
			}
		std::vector<FMTvertex_descriptor> cleaned = baseNode;
		if (parent!=searchtree.end())
			{
			cleaned = parent->second;
			}
		getActionRebuild(targetnode, actions, cleaned,exactnode); // should be able to find also exact!!!!!!!!
		if (!exact)
			{
			std::vector<FMTvertex_descriptor>toRemove;
			for (std::map<Core::FMTOutputNode, std::vector<FMTvertex_descriptor>>::const_reverse_iterator sit = searchtree.rbegin();
				sit != searchtree.rend(); sit++) 
				{
					
					if (targetnode.source.getmask().isNotThemesSubset(sit->first.source.getmask(),themes))//deal only with mask
					{
						toRemove.insert(toRemove.end(), sit->second.begin(), sit->second.end());
					}
				}
			if (!toRemove.empty())
				{
				std::vector<FMTvertex_descriptor>difference;
				std::sort(toRemove.begin(), toRemove.end());
				std::set_difference(cleaned.begin(), cleaned.end(),
					toRemove.begin(), toRemove.end(), std::inserter(difference, difference.begin()));
				cleaned = difference;
				}
			}
		std::pair<std::map<Core::FMTOutputNode, std::vector<FMTvertex_descriptor>>::const_iterator, bool> returniterator;
		returniterator = searchtree.insert(std::pair<Core::FMTOutputNode, std::vector<FMTvertex_descriptor>>(targetnode, cleaned));
		return (returniterator.first)->second;
		}

	FMTOutputNodeCache::FMTOutputNodeCache() : baseNode(), searchtree()
		{

		}


	FMTOutputNodeCache::FMTOutputNodeCache(const FMTOutputNodeCache& rhs) : baseNode(rhs.baseNode),searchtree(rhs.searchtree)
		{

		}

	FMTOutputNodeCache& FMTOutputNodeCache::operator = (const FMTOutputNodeCache& rhs)
		{
		if (this!=&rhs)
			{
			baseNode = rhs.baseNode;
			searchtree = rhs.searchtree;
			}
		return *this;
		}


	const std::vector<FMTvertex_descriptor>& FMTOutputNodeCache::getVertices(const Core::FMTOutputNode& targetnode, const std::vector<Core::FMTAction>& actions,
																const std::vector<Core::FMTTheme>&themes, bool& exactvecticies) const
		{
		return this->getCleanDescriptors(targetnode,actions,themes, exactvecticies);
		}

	void FMTOutputNodeCache::setValidVertices(const Core::FMTOutputNode& targetnode,const std::vector<FMTvertex_descriptor>& vertices) const
		{
		searchtree[targetnode] = vertices;
		}

	void FMTOutputNodeCache::clear()
		{
		baseNode.clear();
		searchtree.clear();
		}


}
*/