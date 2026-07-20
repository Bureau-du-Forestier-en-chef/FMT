/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTYIELDSDEVELOPMENT_Hm_included
#define FMTYIELDSDEVELOPMENT_Hm_included

#include <boost/unordered/concurrent_flat_map.hpp>
#include <boost/dynamic_bitset.hpp>
#include <string>


namespace Core
{
	// DocString: FMTYieldDevelopment
	/**
	@brief Development keyed by age, period, mask and yield, used to cache yield values.
	*/
	class FMTYieldDevelopment
	{
	public:
		// DocString: FMTYieldDevelopment(int,int,const boost::dynamic_bitset<uint8_t>&,const std::string&)
		/**
		@brief Construct a yield development from an age, a period, a mask and a yield.
		@param[in] p_age the age.
		@param[in] p_period the period.
		@param[in] p_mask the mask.
		@param[in] p_yield the yield.
		*/
		FMTYieldDevelopment(int p_age, int p_period,
			const boost::dynamic_bitset<uint8_t>& p_mask, const std::string& p_yield);
		// DocString: FMTYieldDevelopment::operator==
		/**
		@brief Equality comparison operator of FMTYieldDevelopment.
		@param[in] p_yieldDev the yield development to compare with.
		@return true if both are equal else false.
		*/
		bool operator == (const FMTYieldDevelopment& p_yieldDev) const;
		// DocString: FMTYieldDevelopment::hash
		/**
		@brief Return the hash of the yield development.
		@return the hash value.
		*/
		size_t hash() const;
		// DocString: FMTYieldDevelopment::getPeriod
		/**
		@brief Return the period of the yield development.
		@return the period.
		*/
		int getPeriod() const;
	private:
		uint8_t m_age;
		uint8_t m_period;
		boost::dynamic_bitset<uint8_t> m_resumeMask;
		std::string m_yield;
		// DocString: FMTYieldDevelopment::_equalYield
		/**
		@brief Return true if two yields are equal.
		@param[in] p_first the first yield.
		@param[in] p_second the second yield.
		@return true if the yields are equal else false.
		*/
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
#endif // FMTDATA_Hm_included
