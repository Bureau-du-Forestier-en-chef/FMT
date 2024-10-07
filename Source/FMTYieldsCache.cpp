/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTYieldsCache.h"
#include "FMTdevelopment.h"
#include "FMTyieldrequest.h"
#include "FMTgraphvertextoyield.h"
#include "FMTyields.h"
#include "FMTmodel.h"
#include "FMTyieldhandler.h"
#include <boost/dynamic_bitset.hpp>
#include <boost/flyweight.hpp>





namespace Core
{
	class FMTYieldDevelopment
	{
	public:
		FMTYieldDevelopment(int p_age,int p_period,
			const boost::dynamic_bitset<uint8_t>& p_mask, const std::string& p_yield) :
			m_age(static_cast<uint8_t>(p_age)), m_period(static_cast<uint8_t>(p_period)),
			m_resumeMask(p_mask),
			m_yield(p_yield)
		{

		}
		bool operator == (const FMTYieldDevelopment& p_yieldDev) const
		{
			return	(p_yieldDev.m_age == m_age &&
				p_yieldDev.m_period == m_period &&
				p_yieldDev.m_resumeMask == m_resumeMask &&
				equalYield(m_yield,p_yieldDev.m_yield));
		}

		bool operator < (const FMTYieldDevelopment& p_yieldDev) const
			{
			if (m_age < p_yieldDev.m_age)
				return true;
			if (p_yieldDev.m_age < m_age)
				return false;
			if (m_period < p_yieldDev.m_period)
				return true;
			if (p_yieldDev.m_period < m_period)
				return false;
			if (m_resumeMask < p_yieldDev.m_resumeMask)
				return true;
			if (p_yieldDev.m_resumeMask < m_resumeMask)
				return false;
			if (lessThenYield(m_yield, p_yieldDev.m_yield))
				return true;
			if (lessThenYield(p_yieldDev.m_yield,m_yield))
				return false;
			return false;
			}


		size_t hash() const
		{
			size_t seed = 0;
			boost::hash_combine(seed, m_age);
			boost::hash_combine(seed, m_period);
			boost::hash_combine(seed, m_resumeMask);
			boost::hash_combine(seed, boost::hash<std::string>()(m_yield));
			return seed;
		}
		int getPeriod() const
			{
			return static_cast<int>(m_period);
			}
	private:
		uint8_t m_age;
		uint8_t m_period;
		boost::dynamic_bitset<uint8_t> m_resumeMask;
		std::string m_yield;

	};
}

namespace boost {
	template <>
	struct hash<Core::FMTYieldDevelopment>
	{
		std::size_t operator()(const Core::FMTYieldDevelopment& p_yield) const
		{
			return (p_yield.hash());
		}
	};

}
namespace Core
{
	std::unique_ptr<boost::concurrent_flat_map<FMTYieldDevelopment, double>> FMTYieldsCache::m_cache =
		static_cast<std::unique_ptr<boost::concurrent_flat_map<FMTYieldDevelopment, double>>>(new boost::concurrent_flat_map<FMTYieldDevelopment, double>());

	bool FMTYieldsCache::inCache(const FMTyieldrequest& p_request,
		const std::string& p_yield) const
		{
		return m_cache->contains(_getKey(p_request, p_yield));
		}
	void FMTYieldsCache::reserve(const FMTyieldrequest& p_request)
		{
		if (m_cache->empty())
			{
			const size_t NUMBER_OF_ATTRIBUTS = p_request.getresumemask().size();
			const Graph::FMTgraphvertextoyield* VERTEX_PTR = p_request.getvertexgraphinfo();
			size_t toReserve = 0;
			if (VERTEX_PTR!=nullptr)
				{
				const Models::FMTmodel* MODEL_Ptr = VERTEX_PTR->getmodel();
				const size_t LENGTH = static_cast<size_t>(MODEL_Ptr->getparameter(Models::FMTintmodelparameters::LENGTH));
				const FMTyields YIELDS = MODEL_Ptr->getyields();
				for (const auto& HANDLER : YIELDS)
					{
					if (HANDLER.second->gettype()==Core::FMTyldtype::FMTcomplexyld)
						{
						toReserve += (HANDLER.second->size() * NUMBER_OF_ATTRIBUTS * LENGTH * 2);
						}
					}
				}
			m_cache->reserve(toReserve*3);
			}
		}

	double FMTYieldsCache::get(const FMTyieldrequest& p_request,
		const std::string& p_yield) const
		{
		double value = 0;
		m_cache->visit(_getKey(p_request, p_yield),[&](const std::pair<FMTYieldDevelopment, double>& data)
			{
				value = data.second;
			});
		return value;
		}

	void FMTYieldsCache::_ClearIfTooBig()
		{
		if (((sizeof(FMTYieldDevelopment) * m_cache->size()) / 1073741824) > 10) //delete larger then 20 Go
			{
			m_cache->clear();
			}
		}
	void FMTYieldsCache::set(double p_value,
		const FMTyieldrequest& p_request,
		const std::string& p_yield)
		{
		_ClearIfTooBig();
		const std::pair<FMTYieldDevelopment, double> DATA(_getKey(p_request, p_yield), p_value);
		m_cache->insert(DATA);
		}

	FMTYieldDevelopment FMTYieldsCache::_getKey(const FMTyieldrequest& p_request,
									const std::string& p_yield) const
		{
		const Core::FMTdevelopment& REF_DEV = p_request.getdevelopment();
		const FMTYieldDevelopment KEY(REF_DEV.getage(), REF_DEV.getperiod(),p_request.getresumemask().getbitsetreference(), p_yield);
		return KEY;
		}

	}

