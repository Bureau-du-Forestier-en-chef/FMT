/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTYIELDSCACHE_Hm_included
#define FMTYIELDSCACHE_Hm_included


#include <boost/unordered/concurrent_flat_map.hpp>
#include <boost/dynamic_bitset.hpp>
#include <memory>
#include <string>
#include "FMTutility.h"

namespace Core
	{
	class FMTDevelopment;
	class FMTYieldDevelopment;
	class FMTYieldRequest;
	class FMTYieldsCache
		{
		public:
			FMTYieldsCache()=default;
			FMTYieldsCache(const FMTYieldsCache& rhs)=default;
			FMTYieldsCache& operator = (const FMTYieldsCache& rhs)=default;
			~FMTYieldsCache() = default;
			bool inCache(const FMTYieldRequest& p_request,
							const std::string& p_yield) const;
			double get(const FMTYieldRequest& p_request,
				const std::string& p_yield) const;
			void set(double p_value,
				const FMTYieldRequest& p_request,
				const std::string& p_yield);
			void reserve(const FMTYieldRequest& p_request);
		private:
			static std::unique_ptr<boost::concurrent_flat_map<FMTYieldDevelopment,double>> m_cache;
			FMTYieldDevelopment _getKey(const FMTYieldRequest& p_request,
										const std::string& p_yield) const;
			void _clearIfTooBig();
		};
	}



#endif // FMTDATA_Hm_included
