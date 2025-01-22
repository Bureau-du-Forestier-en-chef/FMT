/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTYIELDSDEVELOPMENT_H_INCLUDED
#define FMTYIELDSDEVELOPMENT_H_INCLUDED

#include <boost/unordered/concurrent_flat_map.hpp>
#include <boost/dynamic_bitset.hpp>
#include <string>


namespace Core
{
	class FMTYieldDevelopment
	{
	public:
		FMTYieldDevelopment(int p_age, int p_period,
			const boost::dynamic_bitset<uint8_t>& p_mask, const std::string& p_yield);
		bool operator == (const FMTYieldDevelopment& p_yieldDev) const;
		size_t hash() const;
		int getPeriod() const;
	private:
		uint8_t m_age;
		uint8_t m_period;
		boost::dynamic_bitset<uint8_t> m_resumeMask;
		std::string m_yield;
		bool _equalYield(const std::string& p_first, const std::string& p_second) const noexcept;
	};
}
namespace boost {
	template <>
	struct hash< Core::FMTYieldDevelopment>
	{
		std::size_t operator()(const Core::FMTYieldDevelopment& p_yield) const
		{
			return (p_yield.hash());
		}
	};
}
#endif // FMTDATA_H_INCLUDED
