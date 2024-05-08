/*
Copyright (c) 2019 Gouvernement du Québec

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
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & BOOST_SERIALIZATION_NVP(inmemorynodes);
			//ar & BOOST_SERIALIZATION_NVP(basenode);
			ar & BOOST_SERIALIZATION_NVP(searchtree);
		}
		std::vector<tvdescriptor>inmemorynodes;
		titerator const * beginit;
		titerator const * endit;
		//boost::unordered_set<Core::FMTlookup<tvdescriptor, Core::FMTdevelopment>> const * basenode;
		mutable std::map<Core::FMToutputsource,std::vector<tvdescriptor>>searchtree;
        typedef typename std::map<Core::FMToutputsource,std::vector<tvdescriptor>>::const_iterator notecacheit;
		const std::vector<tvdescriptor>& getcleandescriptors(const Core::FMToutputnode& targetnode,const std::vector<Core::FMTaction>& actions,
										const std::vector<Core::FMTtheme>&themes, bool& exactnode) const
		{
			//bool exact = false;
			exactnode = false;
			typename std::map<Core::FMToutputsource, std::vector<tvdescriptor>>::const_iterator parent = this->getparentnode(targetnode, actions, exactnode);
			//Logging::FMTlogger() << "exact? " << exactnode << "\n";
			if (exactnode)
			{
				return parent->second;
			}

			//std::vector<tvdescriptor> cleaned(basenode);
			std::vector<tvdescriptor> cleaned;
			pushtovector(cleaned);
			//
			if (parent != searchtree.end())
			{
				cleaned = parent->second;
			}
			getactionrebuild(targetnode, actions, cleaned, exactnode); // should be able to find also exact!!!!!!!!
			if (!exactnode)
			{
				std::vector<tvdescriptor>toremove;
				/*size_t basenodesize = inmemorynodes.size();
				if (basenode != nullptr)
					{
					basenodesize =  basenode->size();
					}*/
				const Core::FMTmask& targetmask = targetnode.source.getmask();
				//Core::FMTmask unionmask(themes);
				for (typename std::map<Core::FMToutputsource, std::vector<tvdescriptor>>::const_reverse_iterator sit = searchtree.rbegin();
					sit != searchtree.rend(); sit++)
				{
					const Core::FMTmask& nodemask = sit->first.getmask();
					if (targetmask.isnotthemessubset(nodemask, themes))//deal only with mask
					{
						//unionmask = unionmask.getunion(nodemask);
						toremove.insert(toremove.end(), sit->second.begin(), sit->second.end());
					}
				}
				if (!toremove.empty())
				{
					//std::cout << "mask size of " << unionmask.size() << " " << unionmask.count() << "\n";
					std::vector<tvdescriptor>difference;
					std::sort(toremove.begin(), toremove.end());
					std::set_difference(cleaned.begin(), cleaned.end(),
						toremove.begin(), toremove.end(), std::inserter(difference, difference.begin()));
					cleaned = difference;
				}
			}
			std::pair<notecacheit, bool> returniterator;
			returniterator = searchtree.insert(std::pair<Core::FMToutputsource, std::vector<tvdescriptor>>(targetnode.source, cleaned));
			return (returniterator.first)->second;
		}
		void getactionrebuild(const Core::FMToutputnode& targetnode,
			const std::vector<Core::FMTaction>& actions,
			std::vector<tvdescriptor>& cleaned,
			bool& exactnode) const
		{
			const std::string actionname = targetnode.source.getaction();
			const std::vector<const Core::FMTaction*>aggregatesptr = Core::FMTactioncomparator(actionname).getallaggregates(actions, true);
			if (!actionname.empty() && !aggregatesptr.empty()) //so it's a aggregate!
			{
				std::map<std::string, std::vector< notecacheit>>potentials;
				for (const Core::FMTaction* attributeptr : aggregatesptr)
				{
					potentials[attributeptr->getname()] = std::vector< notecacheit>();
				}
				for (notecacheit sit = searchtree.begin();
					sit != searchtree.end(); sit++)
				{
					if (sit->first.issubsetof(targetnode.source, actions))
					{
						const std::string nodeaction = sit->first.getaction();
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
		notecacheit getparentnode(const Core::FMToutputnode& targetnode, const std::vector<Core::FMTaction>& actions, bool& exactnode) const
			{
				notecacheit parentit = searchtree.find(targetnode.source);
				if (parentit != searchtree.end())
				{
					exactnode = true;
					return parentit;
				}
				parentit = searchtree.begin();
				exactnode = false;
				while (parentit != searchtree.end())
				{
					if (targetnode.source.issubsetof(parentit->first, actions))
					{
						return parentit;
					}
					++parentit;
				}
				return searchtree.end();
			}
	public:
		FMToutputnodecache()=default;
		FMToutputnodecache(const FMToutputnodecache& rhs) = default;
		FMToutputnodecache& operator = (const FMToutputnodecache& rhs) = default;
		~FMToutputnodecache() = default;
		/*FMToutputnodecache(const boost::unordered_set<Core::FMTlookup<tvdescriptor,Core::FMTdevelopment>>& initialgraph) :
			inmemorynodes(),beginit(),endit(), searchtree()
			{
				//this->setinitialcache(initialgraph);
			}*/
		FMToutputnodecache(const std::vector<tvdescriptor>& initialnodes) :
			inmemorynodes(initialnodes), beginit(nullptr), endit(nullptr), searchtree()
		{
			inmemorynodes.shrink_to_fit();
			std::sort(inmemorynodes.begin(),inmemorynodes.end());
			//this->setinitialcache(initialnodes);
		}
		FMToutputnodecache(const titerator& first, const titerator& last) :
			inmemorynodes(), beginit(&first), endit(&last), searchtree()
		{
			//this->setinitialcache(initialnodes);
		}
		/*unsigned long long getpotentialsize() const
			{
			return static_cast<unsigned long long>(searchtree.size()) * static_cast<unsigned long long>(basenode->size()) * sizeof(tvdescriptor);
			}*/

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
				//std::vector<tvdescriptor>().swap(largestiterator->second);
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
			//basenode.clear();
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
					refvecs.push_back(*it);
				}
				refvecs.shrink_to_fit();
				std::sort(refvecs.begin(), refvecs.end());
			}
			else {
				refvecs = inmemorynodes;
			}
		}

	};

	
	/*template<> inline void FMToutputnodecache<FMTgraph<FMTvertexproperties,FMTedgeproperties>::FMTvertex_descriptor,FMTgraph<FMTvertexproperties,FMTedgeproperties>::FMTvertex_iterator>::pushtovector(std::vector<FMTgraph<FMTvertexproperties,FMTedgeproperties>::FMTvertex_descriptor>& refvecs) const
	{
			for (FMTgraph<FMTvertexproperties,FMTedgeproperties>::FMTvertex_iterator it = *beginit; it != *endit; ++it)
			{
				refvecs.push_back(*it);

			}
			refvecs.shrink_to_fit();
			std::sort(refvecs.begin(), refvecs.end());
	}

	template<> inline void FMToutputnodecache<FMTgraph<FMTbasevertexproperties,FMTbaseedgeproperties>::FMTvertex_descriptor,FMTgraph<FMTbasevertexproperties,FMTbaseedgeproperties>::FMTvertex_iterator>::pushtovector(std::vector<FMTgraph<FMTbasevertexproperties,FMTbaseedgeproperties>::FMTvertex_descriptor>& refvecs) const
	{
			for (FMTgraph<FMTbasevertexproperties,FMTbaseedgeproperties>::FMTvertex_iterator it = *beginit; it != *endit; ++it)
			{
				refvecs.push_back(*it);

			}
			refvecs.shrink_to_fit();
			std::sort(refvecs.begin(), refvecs.end());
	}*/

}

#endif
