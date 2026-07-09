/*
Copyright (c) 2019 Gouvernement du Qubec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMToutputnode.h"
#include "FMTaction.h"
#include <algorithm>

namespace Core

{

	FMToutputnode::FMToutputnode():source(),factor(),constant()
		{

		}

	bool FMToutputnode::allowCashdeduction() const
		{
		return (!factor.isTimeYield());
		}

	bool FMToutputnode::singlePeriod() const
		{
		return (!source.emptyperiod() && source.getPeriodLowerBound() == source.getPeriodUpperBound());
		}

	bool FMToutputnode::multiPeriod() const
		{
		return (!source.emptyperiod() && source.getPeriodLowerBound() != source.getPeriodUpperBound());
		}

	bool FMToutputnode::isNull() const
		{
		return ((factor.isConstant() && factor.getValue() == 0) || constant == 0);
		}

    bool FMToutputnode::isPastPeriod() const
		{
		if (singlePeriod())
			{
			if (source.getPeriodLowerBound() < 0)
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


    FMToutputnode FMToutputnode::setPeriod(int period) const
        {
        FMToutputnode newnode(*this);
        newnode.source.setBounds(FMTperbounds(FMTsection::Optimize,period,period));
        newnode.factor.setBounds(FMTperbounds(FMTsection::Optimize,period,period));
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

	size_t FMToutputnode::hashForValue() const
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

	bool FMToutputnode::isSubsetOf(const FMToutputnode& rhs) const
		{
		return source.isSubsetOf(rhs.source);
		}

	bool FMToutputnode::isSubsetOf(const FMToutputnode& rhs, const std::vector<Core::FMTaction>& actions) const
		{
		return source.isSubsetOf(rhs.source,actions);
		}

	bool FMToutputnode::isSameButDifferentAction(const FMToutputnode& rhs) const
		{
		return source.isSameButDifferentAction(rhs.source);
		}

	bool FMToutputnode::isSameValues(const FMToutputnode& rhs) const
		{
		return (constant == rhs.constant &&  factor.isEqualByValue(rhs.factor) && source.isEqualByValue(rhs.source));
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

	int FMToutputnode::setToGraph(std::vector<int>& targetedperiods, int period, int max_period)
		{
		int node_period = period;
		if (this->source.isNextPeriod())
		{
			++node_period;
		}
		
		if (this->singlePeriod())
		{
			if (this->isPastPeriod())
			{
				if ((this->source.getPeriodLowerBound() + node_period) >= 0)
				{
					node_period = (this->source.getPeriodLowerBound() + node_period);
					const FMTperbounds perbound(FMTsection::Optimize, node_period, node_period);
					this->source.setBounds(perbound);
					this->factor.setBounds(perbound);
				}
				else {
					return -1;//dont need that node...
				}
			}
			else {
				node_period = this->source.getPeriodLowerBound();
				if (this->source.isNextPeriod())
				{
					++node_period;
					const FMTperbounds perbound(FMTsection::Optimize, node_period, node_period);
					this->source.setBounds(perbound);
					this->factor.setBounds(perbound);
				}
			}
		}
		if (this->multiPeriod())
		{
			const int minperiod = std::max(this->source.getPeriodLowerBound(), 1);
			const int maxperiod = std::min(this->source.getPeriodUpperBound(), max_period);
			for (int periodid = minperiod; periodid <= maxperiod; ++periodid)
			{
				int local_period = periodid;
				if (this->source.isNextPeriod())
				{
					++local_period;
				}
				targetedperiods.push_back(local_period);
			}
		}
		else if (this->source.emptyperiod() && (this->source.isSum() || this->source.isAverage()))
		{	
			const int minperiod = 1;
			const int maxperiod = period;
			for (int periodid = minperiod; periodid <= maxperiod; ++periodid)
			{
				int local_period = periodid;
				if (this->source.isNextPeriod())
				{
					++local_period;
				}
				targetedperiods.push_back(local_period);
			}
		}
		else {
			targetedperiods.push_back(node_period);
		}
		if (this->source.isAverage())
		{
			constant *= 1/static_cast<double>(targetedperiods.size());//average factor = 1/sum(targeted_period)
		}
		return node_period;
		}

	std::string FMToutputnode::getHashString() const
	{
		/*std::string value(static_cast<const char*>(static_cast<const void*>(&source)));
		value += std::string(static_cast<const char*>(static_cast<const void*>(&factor)));
		value += std::string(static_cast<const char*>(static_cast<const void*>(&constant)));
		return value;*/
		//return FMTbinarizer().binarize<FMToutputnode>(*this);
		return std::string(*this);
	}

	Core::FMTmask FMToutputnode::getHashMask() const
	{
		Core::FMTmask basemask;
		source.fillHashMask(basemask);
		factor.fillHashMask(basemask);
		basemask.binarizedAppend<double>(constant);
		return basemask;
	}

	void FMToutputnode::fillHashMaskSpec(Core::FMTmask& basemask) const
	{
		source.fillHashSpec(basemask);
		factor.fillHashSpec(basemask);
	}

	


	bool FMToutputnodevaluecomparator::operator()(const FMToutputnode& node1, const FMToutputnode& node2) const
		{
		return node1.isSameValues(node2);
		}

	bool FMToutputnodeorigincomparator::operator()(const FMToutputnode& node1, const FMToutputnode& node2) const
		{
		return (node1.getOutputId()<node2.getOutputId());
		}


	size_t FMToutputnodehasher::operator()(const FMToutputnode & node) const
		{
		return node.hashForValue();
		}

	void FMToutputnode::fillUpEquation(std::map<std::string,std::vector<std::string>>& allequations,
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
