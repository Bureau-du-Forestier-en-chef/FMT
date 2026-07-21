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
	// DocString: FMTYieldsCache
	/**
	@brief Cache of yield values keyed by development and yield name.
	*/
	class FMTYieldsCache
		{
		public:
			// DocString: FMTYieldsCache()
			/**
			@brief Default constructor for FMTYieldsCache.
			*/
			FMTYieldsCache()=default;
			// DocString: FMTYieldsCache(const FMTYieldsCache&)
			/**
			@brief Copy constructor for FMTYieldsCache.
			@param[in] rhs the FMTYieldsCache to copy.
			*/
			FMTYieldsCache(const FMTYieldsCache& rhs)=default;
			// DocString: FMTYieldsCache::operator=
			/**
			@brief Copy assignment operator for FMTYieldsCache.
			@param[in] rhs the FMTYieldsCache to copy.
			@return a reference to this FMTYieldsCache.
			*/
			FMTYieldsCache& operator = (const FMTYieldsCache& rhs)=default;
			// DocString: ~FMTYieldsCache()
			/**
			@brief Default destructor for FMTYieldsCache.
			*/
			~FMTYieldsCache() = default;
			// DocString: FMTYieldsCache::inCache
			/**
			@brief Return true if the value for a request and yield is cached.
			@param[in] p_request the yield request.
			@param[in] p_yield the yield name.
			@return true if the value is cached else false.
			*/
			bool inCache(const FMTYieldRequest& p_request,
							const std::string& p_yield) const;
			// DocString: FMTYieldsCache::get
			/**
			@brief Get the cached value for a request and yield.
			@param[in] p_request the yield request.
			@param[in] p_yield the yield name.
			@return the cached value.
			*/
			double get(const FMTYieldRequest& p_request,
				const std::string& p_yield) const;
			// DocString: FMTYieldsCache::set
			/**
			@brief Cache a value for a request and yield.
			@param[in] p_value the value.
			@param[in] p_request the yield request.
			@param[in] p_yield the yield name.
			*/
			void set(double p_value,
				const FMTYieldRequest& p_request,
				const std::string& p_yield);
			// DocString: FMTYieldsCache::reserve
			/**
			@brief Reserve memory in the cache for a request.
			@param[in] p_request the yield request.
			*/
			void reserve(const FMTYieldRequest& p_request);
		private:
			static std::unique_ptr<boost::concurrent_flat_map<FMTYieldDevelopment,double>> m_cache;
			// DocString: FMTYieldsCache::_getKey
			/**
			@brief Return the cache key for a request and yield.
			@param[in] p_request the yield request.
			@param[in] p_yield the yield name.
			@return the cache key.
			*/
			FMTYieldDevelopment _getKey(const FMTYieldRequest& p_request,
										const std::string& p_yield) const;
			// DocString: FMTYieldsCache::_clearIfTooBig
			/**
			@brief Clear the cache if it becomes too big.
			*/
			void _clearIfTooBig();
		};
	}



#endif // FMTDATA_Hm_included
