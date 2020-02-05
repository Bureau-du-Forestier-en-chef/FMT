/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "FMToutputnode.h"

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
        newnode.source.setbounds(FMTperbounds(FMTwssect::Optimize,period,period));
        newnode.factor.setbounds(FMTperbounds(FMTwssect::Optimize,period,period));
        return newnode;
        }

	FMToutputnode::FMToutputnode(const FMToutputsource& lsource, const FMToutputsource& lfactor, const double& lconstant) :
		source(lsource), factor(lfactor), constant(lconstant)
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

	bool FMToutputnode::issubsetof(const FMToutputnode& rhs, const std::map<std::string, std::vector<std::string>>& aggregates) const
		{
		return source.issubsetof(rhs.source,aggregates);
		}

	bool FMToutputnode::issamebutdifferentaction(const FMToutputnode& rhs) const
		{
		return source.issamebutdifferentaction(rhs.source);
		}

	bool FMToutputnode::canbeusedby(const FMToutputnode& rhs, const std::map<std::string, std::vector<std::string>>& aggregates) const
		{
		return source.canbeusedby(rhs.source, aggregates);
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
				if ((this->source.getperiodlowerbound() + period) >= 0)
				{
					const FMTperbounds perbound(FMTwssect::Optimize, node_period, node_period);
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
					const FMTperbounds perbound(FMTwssect::Optimize, node_period, node_period);
					this->source.setbounds(perbound);
					this->factor.setbounds(perbound);
				}
			}
		}
		if (this->multiperiod())
		{
			const int minperiod = std::max(this->source.getperiodlowerbound(), 1);
			const int maxperiod = std::min(this->source.getperiodupperbound(), maxperiod);
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

	FMToutputnodehashcomparator::FMToutputnodehashcomparator(size_t hash) : sourcehash(hash)
	{

	}

	bool FMToutputnodehashcomparator::operator()(const FMToutputnode& node) const
	{
		return (node.source.hash() == sourcehash);
	}

}
