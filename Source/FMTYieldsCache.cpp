/*
Copyright (c) 2019 Gouvernement du Qu�bec

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
#include "FMTYieldDevelopment.h"
#include <boost/dynamic_bitset.hpp>
#include <boost/flyweight.hpp>
#include <FMTobject.h>
#include <iostream>

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
		const size_t TABLE_SIZE = m_cache->size();
		bool needToClear = false;
		if (TABLE_SIZE > 0 && //works on 128 go
			(TABLE_SIZE % 10000) == 0) //Check each 10k yields set... maybe too much check
			{
			//boost::lock_guard<boost::mutex> memoryGuard(m_memoryMutex);
			//I realy dont trust GlobalMemoryStatusEx to be thread-safe even if ()
			needToClear = (Core::FMTobject::getavailablememory() / 1073741824) < 10;
			}
		if (needToClear)
			{
			m_cache->clear();
			}


		/*if (TABLE_SIZE > 0 && //works on 128 go
			(TABLE_SIZE % 100000) == 0 &&
			(Core::FMTobject::getavailablememory() / 1073741824) < 10) //If less then 10 Go then delete
			{
			m_cache->clear();
			}*/


		/*if ((Core::FMTobject::getavailablememory() / 1073741824) < 10000) //original
			{
			m_cache->clear();
			}*/
		
		
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

