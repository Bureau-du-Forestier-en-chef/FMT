#include "FMTYieldDevelopment.h"

namespace Core {
	FMTYieldDevelopment::FMTYieldDevelopment(int p_age, int p_period,
		const boost::dynamic_bitset<uint8_t>& p_mask, const std::string& p_yield) :
		m_age(static_cast<uint8_t>(p_age)), m_period(static_cast<uint8_t>(p_period)),
		m_resumeMask(p_mask),
		m_yield(p_yield)
	{
	}

	bool FMTYieldDevelopment::operator == (const FMTYieldDevelopment& p_yieldDev) const
	{
		return	(p_yieldDev.m_age == m_age &&
			p_yieldDev.m_period == m_period &&
			p_yieldDev.m_resumeMask == m_resumeMask &&
			_equalYield(m_yield, p_yieldDev.m_yield));
	}
	size_t FMTYieldDevelopment::hash() const
	{
		size_t seed = 0;
		boost::hash_combine(seed, m_age);
		boost::hash_combine(seed, m_period);
		boost::hash_combine(seed, m_resumeMask);
		boost::hash_combine(seed, boost::hash<std::string>()(m_yield));
		return seed;
	}
	int FMTYieldDevelopment::getPeriod() const
	{
		return static_cast<int>(m_period);
	}
	bool FMTYieldDevelopment::_equalYield(const std::string& p_first, const std::string& p_second) const noexcept
	{

		const size_t FIRST_LENGTH = p_first.length();
		const size_t SECOND_LENGTH = p_second.length();
		bool equal = false;
		if (FIRST_LENGTH == SECOND_LENGTH)
		{
			size_t i = FIRST_LENGTH;
			while (!equal && i > 0)
			{
				--i;
				equal = (p_first[i] == p_second[i]);
			}
		}
		return equal;
	}
}
