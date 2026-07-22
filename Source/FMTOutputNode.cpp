/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTOutputNode.h"
#include "FMTAction.h"
#include <algorithm>

namespace Core

{

	FMTOutputNode::FMTOutputNode():source(),factor(),constant()
		{

		}

	bool FMTOutputNode::allowCashdeduction() const
		{
		return (!factor.isTimeYield());
		}

	bool FMTOutputNode::singlePeriod() const
		{
		return (!source.emptyPeriod() && source.getPeriodLowerBound() == source.getPeriodUpperBound());
		}

	bool FMTOutputNode::multiPeriod() const
		{
		return (!source.emptyPeriod() && source.getPeriodLowerBound() != source.getPeriodUpperBound());
		}

	bool FMTOutputNode::isNull() const
		{
		return ((factor.isConstant() && factor.getValue() == 0) || constant == 0);
		}

    bool FMTOutputNode::isPastPeriod() const
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

	FMTOutputNode::operator std::string() const
		{
		return (std::string(source) + " " + std::string(factor) + " *" + std::to_string(constant));
		}


    FMTOutputNode FMTOutputNode::setPeriod(int period) const
        {
        FMTOutputNode newnode(*this);
        newnode.source.setBounds(FMTPerBounds(FMTsection::Optimize,period,period));
        newnode.factor.setBounds(FMTPerBounds(FMTsection::Optimize,period,period));
        return newnode;
        }

	FMTOutputNode::FMTOutputNode(const FMTOutputSource& lsource, const FMTOutputSource& lfactor, const double& lconstant) :
		source(lsource), factor(lfactor), constant(lconstant)
		{
		
		}

	FMTOutputNode::FMTOutputNode(const Core::FMTMask& generalmask, const std::string& actionaggregate):
		source(FMTSpec(), generalmask, Core::FMTotar::actual, "", actionaggregate),
		factor(FMTotar::val, 1),
		constant(1.0)
	{
		
	}

	FMTOutputNode::FMTOutputNode(const FMTOutputNode& rhs) :
		source(rhs.source), factor(rhs.factor), constant(rhs.constant)
		{

		}

	size_t FMTOutputNode::hash() const
		{
		size_t seed = 0;
		boost::hash_combine(seed, source.hash());
		return seed;
		}

	size_t FMTOutputNode::hashForValue() const
		{
		size_t seed = 0;
		boost::hash_combine(seed, source.hash(-1,true));
		boost::hash_combine(seed, factor.hash(-1, true));
		boost::hash_combine(seed,constant);
		return seed;
		}

	bool FMTOutputNode::operator < (const FMTOutputNode& rhs) const
		{
		return (source<rhs.source);
		}

	bool FMTOutputNode::operator == (const FMTOutputNode& rhs) const
		{
		return (source == rhs.source);
		}

	bool FMTOutputNode::isSubsetOf(const FMTOutputNode& rhs) const
		{
		return source.isSubsetOf(rhs.source);
		}

	bool FMTOutputNode::isSubsetOf(const FMTOutputNode& rhs, const std::vector<Core::FMTAction>& actions) const
		{
		return source.isSubsetOf(rhs.source,actions);
		}

	bool FMTOutputNode::isSameButDifferentAction(const FMTOutputNode& rhs) const
		{
		return source.isSameButDifferentAction(rhs.source);
		}

	bool FMTOutputNode::isSameValues(const FMTOutputNode& rhs) const
		{
		return (constant == rhs.constant &&  factor.isEqualByValue(rhs.factor) && source.isEqualByValue(rhs.source));
		}


	FMTOutputNode& FMTOutputNode::operator = (const FMTOutputNode& rhs)
		{
		if (this!=&rhs)
			{
			source = rhs.source;
			factor = rhs.factor;
			constant = rhs.constant;
			}
		return *this;
		}

	int FMTOutputNode::setToGraph(std::vector<int>& targetedperiods, int period, int max_period)
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
					const FMTPerBounds perbound(FMTsection::Optimize, node_period, node_period);
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
					const FMTPerBounds perbound(FMTsection::Optimize, node_period, node_period);
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
		else if (this->source.emptyPeriod() && (this->source.isSum() || this->source.isAverage()))
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

	std::string FMTOutputNode::getHashString() const
	{
		/*std::string value(static_cast<const char*>(static_cast<const void*>(&source)));
		value += std::string(static_cast<const char*>(static_cast<const void*>(&factor)));
		value += std::string(static_cast<const char*>(static_cast<const void*>(&constant)));
		return value;*/
		//return FMTbinarizer().binarize<FMTOutputNode>(*this);
		return std::string(*this);
	}

	Core::FMTMask FMTOutputNode::getHashMask() const
	{
		Core::FMTMask baseMask;
		source.fillHashMask(baseMask);
		factor.fillHashMask(baseMask);
		baseMask.binarizedAppend<double>(constant);
		return baseMask;
	}

	void FMTOutputNode::fillHashMaskSpec(Core::FMTMask& baseMask) const
	{
		source.fillHashSpec(baseMask);
		factor.fillHashSpec(baseMask);
	}

	


	bool FMTOutputNodeValueComparator::operator()(const FMTOutputNode& node1, const FMTOutputNode& node2) const
		{
		return node1.isSameValues(node2);
		}

	bool FMTOutputNodeOriginComparator::operator()(const FMTOutputNode& node1, const FMTOutputNode& node2) const
		{
		return (node1.getOutputId()<node2.getOutputId());
		}


	size_t FMTOutputNodeHasher::operator()(const FMTOutputNode & node) const
		{
		return node.hashForValue();
		}

	void FMTOutputNode::fillUpEquation(std::map<std::string,std::vector<std::string>>& allequations,
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
BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTOutputNode)
