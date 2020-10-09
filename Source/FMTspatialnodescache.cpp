/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTspatialnodescache.h"
#include "FMToutputnodecache.h"
#include "FMTmodel.h"
#include "FMToutputnode.h"

namespace Spatial
{


	FMTspatialnodescache::~FMTspatialnodescache()
		{
		if (staticnodes!=nullptr)
			{
			delete staticnodes;
			}
		}
	FMTspatialnodescache::FMTnodecache::FMTnodecache(const Core::FMToutputnode& node, const Models::FMTmodel& model):periodicvalues(),worthintersecting(true),patternvalues(), staticmask(), dynamicmask()
		{
		dynamicmask  = model.getdynamicmask(node);
		const Core::FMTmask dymask = model.getdynamicmask(node, true);
		const Core::FMTmask intersection = node.source.getmask();
		staticmask = dymask.getunion(intersection);
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
		staticnodes = new Graph::FMToutputnodecache<FMTcoordinate>(mapping);
		}

	FMTspatialnodescache::FMTspatialnodescache(const FMTspatialnodescache& rhs) : actualcache(nullptr), staticnodes(nullptr), patterncache(rhs.patterncache)
		{
		staticnodes = new Graph::FMToutputnodecache<FMTcoordinate>(*rhs.staticnodes);
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
					staticnodes = new Graph::FMToutputnodecache<FMTcoordinate>(*rhs.staticnodes);
				}
			}
			patterncache = rhs.patterncache;
			}
		return *this;
		}

	const std::vector<FMTcoordinate>& FMTspatialnodescache::getnode(const Core::FMToutputnode& node, const Models::FMTmodel& model, bool& exactnode)
		{
		const size_t hashvalue = node.hashforvalue();
		std::unordered_map<size_t, FMTnodecache>::iterator nodecacheit = patterncache.find(hashvalue);
		if (nodecacheit == patterncache.end())
		{
			patterncache[hashvalue] = FMTnodecache(node, model);
		}
		actualcache = &patterncache[hashvalue];
		return staticnodes->getverticies(node, model.actions,model.themes, exactnode);
		}

	void FMTspatialnodescache::setnode(const Core::FMToutputnode& node, const std::vector<FMTcoordinate>& coordinates)
		{
		staticnodes->setvalidverticies(node, coordinates);
		}

	void FMTspatialnodescache::removeperiod(const int& period)
		{
		for (std::unordered_map<size_t, FMTnodecache>::iterator cit = patterncache.begin(); cit != patterncache.end(); cit++)
			{
			cit->second.removeperiod(period);
			}
		}

	FMTspatialnodescache::FMTnodecache* FMTspatialnodescache::getactualnodecache()
		{
		return actualcache;
		}

	bool FMTspatialnodescache::empty() const
		{
		return patterncache.empty();
		}
	size_t FMTspatialnodescache::size() const
		{
		size_t totalsize = 0;
		for (std::unordered_map<size_t,FMTnodecache>::const_iterator cit = patterncache.begin();cit!=patterncache.end();cit++)
			{
			totalsize += cit->second.patternvalues.size();
			}
		return totalsize;
		}

	void FMTspatialnodescache::insert(const FMTspatialnodescache& rhs)
		{
		for (std::unordered_map<size_t, FMTnodecache>::const_iterator cit = rhs.patterncache.begin(); cit != rhs.patterncache.end(); cit++)
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
			}
		staticnodes->insert(*rhs.staticnodes);
		}

}