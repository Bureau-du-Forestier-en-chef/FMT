/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMToutputnode.hpp"
#include "FMTaction.hpp"
#include <algorithm>

namespace Core

{

	FMToutputnode::FMToutputnode():source(),factor(),constant()
		{

		}

	bool FMToutputnode::allowcashdeduction() const
		{
		return (!factor.istimeyield());
		}

	bool FMToutputnode::singleperiod() const
		{
		return (!source.emptyperiod() && source.getperiodlowerbound() == source.getperiodupperbound());
		}

	bool FMToutputnode::multiperiod() const
		{
		return (!source.emptyperiod() && source.getperiodlowerbound() != source.getperiodupperbound());
		}

	bool FMToutputnode::isnull() const
		{
		return ((factor.isconstant() && factor.getvalue() == 0) || constant == 0);
		}

    bool FMToutputnode::ispastperiod() const
		{
		if (singleperiod())
			{
			if (source.getperiodlowerbound() < 0)
				{
				return true;
				}

			}
		return false;
		}

	FMToutputnode::operator std::string() const
		{
		return (std::string(source) + " " + std::string(factor) + " *" + std::to_string(constant));
		}


    FMToutputnode FMToutputnode::setperiod(int period) const
        {
        FMToutputnode newnode(*this);
        newnode.source.setbounds(FMTperbounds(FMTsection::Optimize,period,period));
        newnode.factor.setbounds(FMTperbounds(FMTsection::Optimize,period,period));
        return newnode;
        }

	FMToutputnode::FMToutputnode(const FMToutputsource& lsource, const FMToutputsource& lfactor, const double& lconstant) :
		source(lsource), factor(lfactor), constant(lconstant)
		{
		
		}

	FMToutputnode::FMToutputnode(const Core::FMTmask& generalmask, const std::string& actionaggregate):
		source(FMTspec(), generalmask, Core::FMTotar::actual, "", actionaggregate),
		factor(FMTotar::val, 1),
		constant(1.0)
	{
		
	}

	FMToutputnode::FMToutputnode(const FMToutputnode& rhs) :
		source(rhs.source), factor(rhs.factor), constant(rhs.constant)
		{

		}

	size_t FMToutputnode::hash() const
		{
		size_t seed = 0;
		boost::hash_combine(seed, source.hash());
		return seed;
		}

	size_t FMToutputnode::hashforvalue() const
		{
		size_t seed = 0;
		boost::hash_combine(seed, source.hash(-1,true));
		boost::hash_combine(seed, factor.hash(-1, true));
		boost::hash_combine(seed,constant);
		return seed;
		}

	bool FMToutputnode::operator < (const FMToutputnode& rhs) const
		{
		return (source<rhs.source);
		}

	bool FMToutputnode::operator == (const FMToutputnode& rhs) const
		{
		return (source == rhs.source);
		}

	bool FMToutputnode::issubsetof(const FMToutputnode& rhs) const
		{
		return source.issubsetof(rhs.source);
		}

	bool FMToutputnode::issubsetof(const FMToutputnode& rhs, const std::vector<Core::FMTaction>& actions) const
		{
		return source.issubsetof(rhs.source,actions);
		}

	bool FMToutputnode::issamebutdifferentaction(const FMToutputnode& rhs) const
		{
		return source.issamebutdifferentaction(rhs.source);
		}

	bool FMToutputnode::issamevalues(const FMToutputnode& rhs) const
		{
		return (constant == rhs.constant &&  factor.isequalbyvalue(rhs.factor) && source.isequalbyvalue(rhs.source));
		}


	FMToutputnode& FMToutputnode::operator = (const FMToutputnode& rhs)
		{
		if (this!=&rhs)
			{
			source = rhs.source;
			factor = rhs.factor;
			constant = rhs.constant;
			}
		return *this;
		}

	int FMToutputnode::settograph(std::vector<int>& targetedperiods, int period, int max_period)
		{
		int node_period = period;
		if (this->source.isnextperiod())
		{
			++node_period;
		}
		
		if (this->singleperiod())
		{
			if (this->ispastperiod())
			{
				if ((this->source.getperiodlowerbound() + node_period) >= 0)
				{
					node_period = (this->source.getperiodlowerbound() + node_period);
					const FMTperbounds perbound(FMTsection::Optimize, node_period, node_period);
					this->source.setbounds(perbound);
					this->factor.setbounds(perbound);
				}
				else {
					return -1;//dont need that node...
				}
			}
			else {
				node_period = this->source.getperiodlowerbound();
				if (this->source.isnextperiod())
				{
					++node_period;
					const FMTperbounds perbound(FMTsection::Optimize, node_period, node_period);
					this->source.setbounds(perbound);
					this->factor.setbounds(perbound);
				}
			}
		}
		if (this->multiperiod())
		{
			const int minperiod = std::max(this->source.getperiodlowerbound(), 1);
			const int maxperiod = std::min(this->source.getperiodupperbound(), max_period);
			for (int periodid = minperiod; periodid <= maxperiod; ++periodid)
			{
				int local_period = periodid;
				if (this->source.isnextperiod())
				{
					++local_period;
				}
				targetedperiods.push_back(local_period);
			}
		}
		else {
			targetedperiods.push_back(node_period);
		}
		return node_period;
		}

	std::string FMToutputnode::gethashstring() const
	{
		/*std::string value(static_cast<const char*>(static_cast<const void*>(&source)));
		value += std::string(static_cast<const char*>(static_cast<const void*>(&factor)));
		value += std::string(static_cast<const char*>(static_cast<const void*>(&constant)));
		return value;*/
		//return FMTbinarizer().binarize<FMToutputnode>(*this);
		return std::string(*this);
	}

	Core::FMTmask FMToutputnode::gethashmask() const
	{
		Core::FMTmask basemask;
		source.fillhashmask(basemask);
		factor.fillhashmask(basemask);
		basemask.binarizedappend<double>(constant);
		return basemask;
	}

	void FMToutputnode::fillhashmaskspec(Core::FMTmask& basemask) const
	{
		source.fillhashspec(basemask);
		factor.fillhashspec(basemask);
	}

	


	bool FMToutputnodevaluecomparator::operator()(const FMToutputnode& node1, const FMToutputnode& node2) const
		{
		return node1.issamevalues(node2);
		}

	bool FMToutputnodeorigincomparator::operator()(const FMToutputnode& node1, const FMToutputnode& node2) const
		{
		return (node1.getoutputid()<node2.getoutputid());
		}


	size_t FMToutputnodehasher::operator()(const FMToutputnode & node) const
		{
		return node.hashforvalue();
		}

	void FMToutputnode::fillupequation(std::map<std::string,std::vector<std::string>>& allequations,
		const std::map<std::string, double>& graphvalues,
		const std::vector<std::string>& equation, const size_t& nodeid) const
		{
		for (std::map<std::string, double>::const_iterator outit = graphvalues.begin(); outit != graphvalues.end(); outit++)
			{
				if (allequations.find(outit->first) == allequations.end())
				{
					allequations[outit->first] = equation;
				}
				std::vector<std::string>localequation(allequations.at(outit->first));
				const std::string strnode = "O" + std::to_string(nodeid);
				const std::string toreplace(std::to_string(outit->second));
				std::replace(localequation.begin(), localequation.end(), strnode, toreplace);
				allequations[outit->first] = localequation;
			}
		}


	

}
BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMToutputnode)
