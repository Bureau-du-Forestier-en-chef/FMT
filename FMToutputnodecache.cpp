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

	std::map<FMToutputnode, std::vector<FMTvertex_descriptor>>::const_iterator FMToutputnodecache::getparentnode(const FMToutputnode& targetnode, const std::map<string, vector<string>>& actionaggregates, bool& exactnode) const
		{
		std::map<FMToutputnode, std::vector<FMTvertex_descriptor>>::const_iterator parentit = searchtree.find(targetnode);
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

	void FMToutputnodecache::getactionrebuild(const FMToutputnode& targetnode, 
											 const std::map<string, vector<string>>& aggregates,
											std::vector<FMTvertex_descriptor>& cleaned,
											bool& exactnode) const
		{
		const string actionname = targetnode.source.getaction();
		map<string, vector<string>>::const_iterator aggregateit = aggregates.find(actionname);
		if (!actionname.empty() && aggregateit != aggregates.end()) //so it's a aggregate!
			{
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "rebuilding aggregate for  " << string(targetnode) << "\n";
			std::map<string,std::vector< std::map<FMToutputnode, std::vector<FMTvertex_descriptor>>::const_iterator>>potentials;
			for (const string& attribute : aggregateit->second)
				{
				potentials[attribute] = std::vector< std::map<FMToutputnode, std::vector<FMTvertex_descriptor>>::const_iterator>();
				}
			for (std::map<FMToutputnode, std::vector<FMTvertex_descriptor>>::const_iterator sit = searchtree.begin();
					sit != searchtree.end(); sit++) // start by the back to scrap cleaned stuff fast!
				{
				if (sit->first.canbeusedby(targetnode,aggregates))
					{
					//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "got node subset  " << string(sit->first) << "\n";
					const string nodeaction= sit->first.source.getaction();
					potentials[nodeaction].push_back(sit);
					}
				}
			for (const string& attribute : aggregateit->second)
				{
				if (potentials.at(attribute).empty())
					{
					return; //not a perfect rebuilt need to be complete!!
					}
				}
			std::vector< std::map<FMToutputnode, std::vector<FMTvertex_descriptor>>::const_iterator>::const_iterator testting = potentials.begin()->second.begin();
			while (testting!= potentials.begin()->second.end())
				{
				size_t attid = 0;
				std::vector<FMTvertex_descriptor>finalselection((*testting)->second);
				size_t insertingdone = 1;
				for (const auto& attribute : potentials)
					{
					if (attid!=0)
						{
						for (std::map<FMToutputnode, std::vector<FMTvertex_descriptor>>::const_iterator it : potentials.at(attribute.first))
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
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "inserting done:  " << insertingdone << "\n";
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "potential size:  " << potentials.size() << "\n";
				if (insertingdone == potentials.size())
					{
					//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "before rebuilt size:  " << cleaned.size() << "\n";
					std::sort(finalselection.begin(), finalselection.end());
					if ((*testting)->first.issamebutdifferentaction(targetnode)) //we got a exact match!!!
						{
						exactnode = true;
						cleaned = finalselection;
					}else {
						//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "final size of:  " << finalselection.size() << "\n";
						vector<FMTvertex_descriptor>intersection;
						std::set_intersection(cleaned.begin(), cleaned.end(),
							finalselection.begin(), finalselection.end(), std::inserter(intersection, intersection.begin()));
						cleaned = intersection;
						}
					//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "after rebuilt size:  " << cleaned.size() << "\n";
					}
				++testting;
				}


			}
		}

	const std::vector<FMTvertex_descriptor>& FMToutputnodecache::getcleandescriptors(const FMToutputnode& targetnode,
																					const std::map<string,vector<string>>& actionaggregates,
																					const std::vector<FMTtheme>&themes,
																					bool& exactnode) const
		{
		bool exact = false;
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "for: " << string(targetnode) << "\n";
		std::map<FMToutputnode, std::vector<FMTvertex_descriptor>>::const_iterator parent = this->getparentnode(targetnode,actionaggregates, exact);
		if (exact)
			{
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "deduced: " << 100 << "\n";
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "found exact!" << string(targetnode) << "\n";
			return parent->second;
			}
		std::vector<FMTvertex_descriptor> cleaned = basenode;
		if (parent!=searchtree.end())
			{
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "got parent :  " << string(parent->first) << "\n";
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "for  :  " << string(targetnode) << "\n";
			cleaned = parent->second;
			}
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "node of:  " << string(targetnode) << "\n";
		getactionrebuild(targetnode, actionaggregates, cleaned,exactnode); // should be able to find also exact!!!!!!!!
		if (!exact)
			{
			//If node contain a action aggregate try to look for perfect reconstruction of the parent???
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "before last clean:  " << cleaned.size() << "\n";
			std::vector<FMTvertex_descriptor>toremove;
			for (std::map<FMToutputnode, std::vector<FMTvertex_descriptor>>::const_reverse_iterator sit = searchtree.rbegin();
				sit != searchtree.rend(); sit++) // start by the back to scrap cleaned stuff fast!
				{
					//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "scanning on " << string(sit->first.source.getmask()) << "\n";
					if (targetnode.source.getmask().isnotthemessubset(sit->first.source.getmask(),themes))//deal only with mask
					{
						toremove.insert(toremove.end(), sit->second.begin(), sit->second.end());
						//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " getting out " << string(sit->first)<< "\n";
					}
				}
			if (!toremove.empty())
				{
				vector<FMTvertex_descriptor>difference;
				std::sort(toremove.begin(), toremove.end());
				std::set_difference(cleaned.begin(), cleaned.end(),
					toremove.begin(), toremove.end(), std::inserter(difference, difference.begin()));
				cleaned = difference;
				}
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "after last clean:  " << cleaned.size() << "\n";
			}
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "deduction of :"<<(1-(double(cleaned.size())/double(basenode.size())))*100 << "\n";
		std::pair<std::map<FMToutputnode, std::vector<FMTvertex_descriptor>>::const_iterator, bool> returniterator;
		returniterator = searchtree.insert(std::pair<FMToutputnode, std::vector<FMTvertex_descriptor>>(targetnode, cleaned));
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


	const std::vector<FMTvertex_descriptor>& FMToutputnodecache::getverticies(const FMToutputnode& targetnode, const std::map<string, vector<string>>& actionaggregates, 
																const std::vector<FMTtheme>&themes, bool& exactvecticies) const
		{
		/*std::map<FMToutputnode, std::vector<FMTvertex_descriptor>>::const_iterator parent = getparentnode(targetnode, exactvecticies);
		if (exactvecticies)
			{
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "got exact for" << string(targetnode) << "\n";
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "with " << string(parent->first) << "\n";
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "with size: " << parent->second.size() << "\n";
			return parent->second;
			}
		return getpotentialfromchildren(targetnode, parent);*/
		return this->getcleandescriptors(targetnode,actionaggregates,themes, exactvecticies);
		//return basenode;
		}

	void FMToutputnodecache::setvalidverticies(const FMToutputnode& targetnode,const std::vector<FMTvertex_descriptor>& verticies) const
		{
		searchtree[targetnode] = verticies;
		}

	void FMToutputnodecache::clear()
		{
		basenode.clear();
		searchtree.clear();
		}


}
