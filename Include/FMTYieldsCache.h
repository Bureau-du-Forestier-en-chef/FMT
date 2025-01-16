/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTYIELDSCACHE_H_INCLUDED
#define FMTYIELDSCACHE_H_INCLUDED


#include <boost/unordered/concurrent_flat_map.hpp>
#include <boost/dynamic_bitset.hpp>
#include <memory>
#include <string>
#include "FMTutility.h"

namespace Core
	{
	class FMTdevelopment;
	class FMTYieldDevelopment;
	class FMTyieldrequest;
	class FMTYieldsCache
		{
		public:
			FMTYieldsCache()=default;
			FMTYieldsCache(const FMTYieldsCache& rhs)=default;
			FMTYieldsCache& operator = (const FMTYieldsCache& rhs)=default;
			~FMTYieldsCache() = default;
			bool inCache(const FMTyieldrequest& p_request,
							const std::string& p_yield) const;
			double get(const FMTyieldrequest& p_request,
				const std::string& p_yield) const;
			void set(double p_value,
				const FMTyieldrequest& p_request,
				const std::string& p_yield);
			void reserve(const FMTyieldrequest& p_request);
		private:
			static std::unique_ptr<boost::concurrent_flat_map<FMTYieldDevelopment,double>> m_cache;
			FMTYieldDevelopment _getKey(const FMTyieldrequest& p_request,
										const std::string& p_yield) const;
			void _ClearIfTooBig();
		};
	}



#endif // FMTDATA_H_INCLUDED
