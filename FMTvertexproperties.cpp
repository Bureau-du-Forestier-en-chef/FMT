#include "FMTvertexproperties.h"

namespace Graph
{
	FMTvertexproperties::FMTvertexproperties() :development(), constraintID() {}

	bool FMTvertexproperties::operator < (const FMTvertexproperties& rhs) const
		{
		if (this->development->period < rhs.development->period)
			{
			return true;
			}
		if (*(this->development) < *(rhs.development))
			{
			return true;
			}
		return false;
		}

	FMTvertexproperties::FMTvertexproperties(const FMTvertexproperties& rhs):
		development(), constraintID(rhs.constraintID)
			{
			//development = make_unique<FMTdevelopment>(*rhs.development);
			development = rhs.development->Clone();
			}
	FMTvertexproperties::FMTvertexproperties(const FMTfuturdevelopment& ldevelopment,
		int& lconstraintID):development(new FMTfuturdevelopment(ldevelopment)),constraintID(lconstraintID)
		{
		//development = make_unique<FMTdevelopment>(ldevelopment);
		}
	FMTvertexproperties& FMTvertexproperties::operator = (const FMTvertexproperties& rhs)
		{
		if (this!=&rhs)
			{
			constraintID = rhs.constraintID;
			development = rhs.development->Clone();
			//development.reset(new FMTdevelopment(*rhs.development)); ///naaaa
			//development = make_unique<FMTdevelopment>(*rhs.development);
			}
		return *this;
		}
	FMTvertexproperties::FMTvertexproperties(const FMTactualdevelopment& ldevelopment,
		int& lconstraintID): development(new FMTactualdevelopment(ldevelopment)), constraintID(lconstraintID)
		{
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << development->getarea() << "actual! " << "\n";
		}
    FMTvertexproperties::FMTvertexproperties(const FMTdevelopment& ldevelopment,int& lconstraintID):
        development(new FMTdevelopment(ldevelopment)), constraintID(lconstraintID)
		{

		}
	double FMTvertexproperties::getbaseRHS() const
		{
		return development->getarea();
		}
	bool FMTvertexproperties::operator == (const FMTvertexproperties& rhs) const
		{
		return (*development == *rhs.development);
		}
	bool FMTvertexproperties::operator != (const FMTvertexproperties& rhs) const
		{
		return (!(*this==rhs));
		}
	int FMTvertexproperties::getconstraintID() const
		{
		return constraintID;
		}
	void FMTvertexproperties::setconstraintID(const int& ID)
		{
		constraintID = ID;
		}
	const FMTdevelopment& FMTvertexproperties::get() const
		{
		return *development;
		}

	size_t FMTvertexproperties::hash() const
		{
		return boost::hash<Core::FMTdevelopment>()(*development);
		}

	/*boost::dynamic_bitset<>FMTvertexproperties::hash(const int& maxperiod, const int& maxage,
		const boost::dynamic_bitset<>& actions, bool periodstart) const
		{
		boost::dynamic_bitset<>bits(maxperiod+1 + actions.size() + 2);

		int bitid = 0;
		for (int id = 0; id < maxperiod+1; ++id)
		{
			bool period = false;
			if (id == development->period)
			{
				period = true;
			}
			bits[bitid] = (period);
			++bitid;
		}

		for (int actionid = 0; actionid < actions.size(); ++actionid)
		{
			bits[bitid] = actions[actionid];
			++bitid;
		}

		bits[bitid] = (periodstart);
		++bitid;
		bool locked = false;
		if (development->lock > 0)
		{
			locked = true;
		}
		bits[bitid] = (locked);
		++bitid;

		return bits;
		}*/

	string FMTvertexproperties::constraintname() const
		{
		return "D" + to_string(constraintID);
		}
}
