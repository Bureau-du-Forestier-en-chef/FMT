/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTspatialnodescache.hpp"
#include "FMToutputnodecache.hpp"
#include "FMTmodel.hpp"
#include "FMToutputnode.hpp"
#include "FMTcoordinate.hpp"

namespace Spatial
{
	FMTspatialnodescache::~FMTspatialnodescache()
		{
		if (staticnodes!=nullptr)
			{
			delete staticnodes;
			}
		}
	FMTspatialnodescache::FMTnodecache::FMTnodecache(const Core::FMToutputnode& node, const Models::FMTmodel& model):periodicvalues(),worthintersecting(true),patternvalues(), staticmask(), dynamicmask(),actions(node.source.targetsset(model.actions))
		{
		dynamicmask  = model.getdynamicmask(node);
		/*const Core::FMTmask dymask = model.getdynamicmask(node, true);
		const Core::FMTmask intersection = node.source.getmask();
		staticmask = dymask.getunion(intersection);*/
		staticmask = model.getstaticmask(node, true);
		}

	void FMTspatialnodescache::FMTnodecache::setvalue(const int& period, const double& value)
	{
		periodicvalues[period] = value;
	}
	void FMTspatialnodescache::FMTnodecache::removeperiod(const int& period)
	{
		periodicvalues.erase(period);
	}

	void FMTspatialnodescache::FMTnodecache::clearperiod()
		{
		periodicvalues.clear();
		}
	void FMTspatialnodescache::FMTnodecache::removevaluefromperiod(const int& period, const double& value)
	{
		periodicvalues[period] -= value;
	}
	bool FMTspatialnodescache::FMTnodecache::gotcachevalue(const int& period) const
	{
		return (periodicvalues.find(period) != periodicvalues.end());
	}
	double FMTspatialnodescache::FMTnodecache::getcachevalue(const int& period) const
	{
		return periodicvalues.at(period);
	}

	void FMTspatialnodescache::FMTnodecache::addvaluefromperiod(const int& period, const double& value)
	{
		periodicvalues[period] += value;
	}

	FMTspatialnodescache::FMTspatialnodescache(const std::vector<FMTcoordinate>& mapping):actualcache(), staticnodes(nullptr), patterncache()
		{
		staticnodes = new Graph::FMToutputnodecache<FMTcoordinate, FMTcoordinate*>(mapping);
		}

	FMTspatialnodescache::FMTspatialnodescache(const FMTspatialnodescache& rhs) : actualcache(nullptr), staticnodes(nullptr), patterncache(rhs.patterncache)
		{
			if(rhs.staticnodes != nullptr)
			{
				staticnodes = new Graph::FMToutputnodecache<FMTcoordinate, FMTcoordinate*>(*rhs.staticnodes);
			}
		}

	FMTspatialnodescache& FMTspatialnodescache::operator = (const FMTspatialnodescache& rhs)
		{
		if (this!=&rhs)
			{
			actualcache = nullptr;
			if (staticnodes != nullptr && rhs.staticnodes != nullptr)
			{
				*staticnodes = *rhs.staticnodes;
			}else {
				if (staticnodes != nullptr)
				{
					delete staticnodes;
				}
				staticnodes = nullptr;
				if (rhs.staticnodes != nullptr)
				{
					staticnodes = new Graph::FMToutputnodecache<FMTcoordinate, FMTcoordinate*>(*rhs.staticnodes);
				}
			}
			patterncache = rhs.patterncache;
			}
		return *this;
		}
	void FMTspatialnodescache::set(const Core::FMToutputnode& node)
		{
		ucaching::iterator nodecacheit = patterncache.find(node);
		actualcache = &nodecacheit->second;
		}

	const std::vector<FMTcoordinate>& FMTspatialnodescache::getnode(const Core::FMToutputnode& node, const Models::FMTmodel& model, bool& exactnode)
		{
		ucaching::iterator nodecacheit = patterncache.find(node);
		if (nodecacheit == patterncache.end())
		{
			std::pair<Core::FMToutputnode, FMTnodecache>newcache(node,FMTnodecache(node, model));
			nodecacheit = patterncache.insert(newcache).first;
		}
		actualcache = &nodecacheit->second;
		return staticnodes->getverticies(node, model.actions,model.themes, exactnode);
		}

	void FMTspatialnodescache::setnode(const Core::FMToutputnode& node, const std::vector<FMTcoordinate>& coordinates)
		{
		staticnodes->setvalidverticies(node, coordinates);
		}

	bool FMTspatialnodescache::isworthy(const Core::FMToutputnode& node) const
	{
		const size_t ucount = actualcache->staticmask.count();
		const size_t uwnode = node.source.getmask().getunion(actualcache->staticmask).count();
		return (uwnode > ucount);
	}

	void FMTspatialnodescache::erasenode(const Core::FMToutputnode& node)
		{
		staticnodes->erasenode(node);
		}

	void FMTspatialnodescache::swap(FMTspatialnodescache& rhs)
		{
		std::swap(actualcache,rhs.actualcache);
		std::swap(staticnodes, rhs.staticnodes);
		patterncache.swap(rhs.patterncache);
		}

	void FMTspatialnodescache::removeperiod(const int& period)
		{
		/*for (boost::unordered_map<Core::FMTmask, FMTnodecache>::iterator cit = patterncache.begin(); cit != patterncache.end(); cit++)
			{
			cit->second.removeperiod(period);
			}*/
		for (ucaching::iterator cit = patterncache.begin(); cit != patterncache.end(); cit++)
			{
			cit->second.removeperiod(period);
			}
		}

	FMTspatialnodescache::FMTnodecache* FMTspatialnodescache::getactualnodecache()
		{
		return actualcache;
		}

	void FMTspatialnodescache::setnodecache(FMTspatialnodescache::ucaching::iterator cashit)
		{
		actualcache = &cashit->second;
		}

	bool FMTspatialnodescache::empty() const
		{
		return patterncache.empty();
		}
	size_t FMTspatialnodescache::size() const
		{
		size_t totalsize = 0;
		/*for (boost::unordered_map<Core::FMTmask, FMTnodecache>::const_iterator cit = patterncache.begin();cit!=patterncache.end();cit++)
			{
			totalsize += cit->second.patternvalues.size();
			}*/
		for (ucaching::const_iterator cit = patterncache.begin(); cit != patterncache.end(); cit++)
		{
			totalsize += cit->second.patternvalues.size();
		}
		return totalsize;
		}

	void FMTspatialnodescache::insert(const FMTspatialnodescache& rhs)
		{
		/*for (boost::unordered_map<Core::FMTmask, FMTnodecache>::const_iterator cit = rhs.patterncache.begin(); cit != rhs.patterncache.end(); cit++)
			{
			if (patterncache.find(cit->first)!=patterncache.end())
				{
				patterncache[cit->first].worthintersecting= cit->second.worthintersecting;
				patterncache[cit->first].patternvalues.insert(cit->second.patternvalues.begin(), cit->second.patternvalues.end());
				patterncache[cit->first].staticmask= cit->second.staticmask;
				patterncache[cit->first].dynamicmask= cit->second.dynamicmask;
			}else {
				patterncache[cit->first] = cit->second;
				patterncache[cit->first].clearperiod();
				}
			}*/
		for (ucaching::const_iterator cit = rhs.patterncache.begin(); cit != rhs.patterncache.end(); cit++)
		{
			if (patterncache.find(cit->first) != patterncache.end())
			{
				patterncache[cit->first].worthintersecting = cit->second.worthintersecting;
				patterncache[cit->first].patternvalues.insert(cit->second.patternvalues.begin(), cit->second.patternvalues.end());
				patterncache[cit->first].staticmask = cit->second.staticmask;
				patterncache[cit->first].dynamicmask = cit->second.dynamicmask;
			}
			else {
				patterncache[cit->first] = cit->second;
				patterncache[cit->first].clearperiod();
			}
		}
		staticnodes->insert(*rhs.staticnodes);
		}

	FMTspatialnodescache::ucaching::iterator FMTspatialnodescache::begin()
		{
		return patterncache.begin();
		}
	FMTspatialnodescache::ucaching::iterator FMTspatialnodescache::end()
		{
		return patterncache.end();
		}

}