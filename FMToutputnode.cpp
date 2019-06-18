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

	size_t FMToutputnode::hash(const int& period) const
		{
		size_t seed = 0;
		boost::hash_combine(seed, source.hash(period));
		boost::hash_combine(seed, factor.hash(period));
		boost::hash_combine(seed, constant);
		return seed;
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

}
