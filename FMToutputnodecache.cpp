#include "FMToutputnodecache.h"
#include <algorithm>

namespace Graph
{

	void FMToutputnodecache::setinitialcache(const std::unordered_map<size_t, FMTvertex_descriptor>& initialgraph)
			{
			searchtree.clear();
			basenode.resize(initialgraph.size());
			size_t idv = 0;
			for (std::unordered_map<size_t, FMTvertex_descriptor>::const_iterator itgraph = initialgraph.begin();
				itgraph!= initialgraph.end();itgraph++)
				{
				basenode[idv] = itgraph->second;
				++idv;
				}
			std::sort(basenode.begin(), basenode.end());
			}

	FMToutputnodecache::FMToutputnodecache(const std::unordered_map<size_t, FMTvertex_descriptor>& initialgraph):
		basenode(), searchtree()
		{
		this->setinitialcache(initialgraph);
		}

	std::map<Core::FMToutputnode, std::vector<FMTvertex_descriptor>>::const_iterator FMToutputnodecache::getparentnode(const Core::FMToutputnode& targetnode, const std::map<std::string, std::vector<std::string>>& actionaggregates, bool& exactnode) const
		{
		std::map<Core::FMToutputnode, std::vector<FMTvertex_descriptor>>::const_iterator parentit = searchtree.find(targetnode);
		if (parentit!=searchtree.end())
			{
			exactnode = true;
			return parentit;
			}
		parentit = searchtree.begin();
		exactnode = false;
		while (parentit != searchtree.end())
			{
			if (targetnode.issubsetof(parentit->first, actionaggregates))
				{
				return parentit;
				}
			++parentit;
			}
		return searchtree.end();
		}

	void FMToutputnodecache::getactionrebuild(const Core::FMToutputnode& targetnode,
											 const std::map<std::string, std::vector<std::string>>& aggregates,
											std::vector<FMTvertex_descriptor>& cleaned,
											bool& exactnode) const
		{
		const std::string actionname = targetnode.source.getaction();
		std::map<std::string, std::vector<std::string>>::const_iterator aggregateit = aggregates.find(actionname);
		if (!actionname.empty() && aggregateit != aggregates.end()) //so it's a aggregate!
			{
			std::map<std::string,std::vector< std::map<Core::FMToutputnode, std::vector<FMTvertex_descriptor>>::const_iterator>>potentials;
			for (const std::string& attribute : aggregateit->second)
				{
				potentials[attribute] = std::vector< std::map<Core::FMToutputnode, std::vector<FMTvertex_descriptor>>::const_iterator>();
				}
			for (std::map<Core::FMToutputnode, std::vector<FMTvertex_descriptor>>::const_iterator sit = searchtree.begin();
					sit != searchtree.end(); sit++) 
				{
				if (sit->first.canbeusedby(targetnode,aggregates))
					{
					const std::string nodeaction= sit->first.source.getaction();
					potentials[nodeaction].push_back(sit);
					}
				}
			for (const std::string& attribute : aggregateit->second)
				{
				if (potentials.at(attribute).empty())
					{
					return; //not a perfect rebuilt need to be complete!!
					}
				}
			std::vector< std::map<Core::FMToutputnode, std::vector<FMTvertex_descriptor>>::const_iterator>::const_iterator testting = potentials.begin()->second.begin();
			while (testting!= potentials.begin()->second.end())
				{
				size_t attid = 0;
				std::vector<FMTvertex_descriptor>finalselection((*testting)->second);
				size_t insertingdone = 1;
				for (const auto& attribute : potentials)
					{
					if (attid!=0)
						{
						for (std::map<Core::FMToutputnode, std::vector<FMTvertex_descriptor>>::const_iterator it : potentials.at(attribute.first))
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
					if ((*testting)->first.issamebutdifferentaction(targetnode)) //we got a exact match!!!
						{
						exactnode = true;
						cleaned = finalselection;
					}else {
						std::vector<FMTvertex_descriptor>intersection;
						std::set_intersection(cleaned.begin(), cleaned.end(),
							finalselection.begin(), finalselection.end(), std::inserter(intersection, intersection.begin()));
						cleaned = intersection;
						}
					}
				++testting;
				}


			}
		}

	const std::vector<FMTvertex_descriptor>& FMToutputnodecache::getcleandescriptors(const Core::FMToutputnode& targetnode,
																					const std::map<std::string,std::vector<std::string>>& actionaggregates,
																					const std::vector<Core::FMTtheme>&themes,
																					bool& exactnode) const
		{
		bool exact = false;
		std::map<Core::FMToutputnode, std::vector<FMTvertex_descriptor>>::const_iterator parent = this->getparentnode(targetnode,actionaggregates, exact);
		if (exact)
			{
			return parent->second;
			}
		std::vector<FMTvertex_descriptor> cleaned = basenode;
		if (parent!=searchtree.end())
			{
			cleaned = parent->second;
			}
		getactionrebuild(targetnode, actionaggregates, cleaned,exactnode); // should be able to find also exact!!!!!!!!
		if (!exact)
			{
			std::vector<FMTvertex_descriptor>toremove;
			for (std::map<Core::FMToutputnode, std::vector<FMTvertex_descriptor>>::const_reverse_iterator sit = searchtree.rbegin();
				sit != searchtree.rend(); sit++) 
				{
					
					if (targetnode.source.getmask().isnotthemessubset(sit->first.source.getmask(),themes))//deal only with mask
					{
						toremove.insert(toremove.end(), sit->second.begin(), sit->second.end());
					}
				}
			if (!toremove.empty())
				{
				std::vector<FMTvertex_descriptor>difference;
				std::sort(toremove.begin(), toremove.end());
				std::set_difference(cleaned.begin(), cleaned.end(),
					toremove.begin(), toremove.end(), std::inserter(difference, difference.begin()));
				cleaned = difference;
				}
			}
		std::pair<std::map<Core::FMToutputnode, std::vector<FMTvertex_descriptor>>::const_iterator, bool> returniterator;
		returniterator = searchtree.insert(std::pair<Core::FMToutputnode, std::vector<FMTvertex_descriptor>>(targetnode, cleaned));
		return (returniterator.first)->second;
		}

	FMToutputnodecache::FMToutputnodecache() : basenode(), searchtree()
		{

		}


	FMToutputnodecache::FMToutputnodecache(const FMToutputnodecache& rhs) : basenode(rhs.basenode),searchtree(rhs.searchtree)
		{

		}

	FMToutputnodecache& FMToutputnodecache::operator = (const FMToutputnodecache& rhs)
		{
		if (this!=&rhs)
			{
			basenode = rhs.basenode;
			searchtree = rhs.searchtree;
			}
		return *this;
		}


	const std::vector<FMTvertex_descriptor>& FMToutputnodecache::getverticies(const Core::FMToutputnode& targetnode, const std::map<std::string, std::vector<std::string>>& actionaggregates,
																const std::vector<Core::FMTtheme>&themes, bool& exactvecticies) const
		{
		return this->getcleandescriptors(targetnode,actionaggregates,themes, exactvecticies);
		}

	void FMToutputnodecache::setvalidverticies(const Core::FMToutputnode& targetnode,const std::vector<FMTvertex_descriptor>& verticies) const
		{
		searchtree[targetnode] = verticies;
		}

	void FMToutputnodecache::clear()
		{
		basenode.clear();
		searchtree.clear();
		}


}
