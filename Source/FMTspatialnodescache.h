/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#include <unordered_map>
#include <map>
#include "FMTmask.h"
#include "FMToutputnode.h"
#include "FMTcoordinate.h"
//#include "FMToutputnodecache.h"
//#include "FMTmodel.h"

#ifndef FMTSPATIALNODESCACHE_H
#define FMTSPATIALNODESCACHE_H
namespace Graph
{
	template<class T1>
	class FMToutputnodecache;
}

namespace Models
{
	class FMTmodel;
}

namespace Spatial
{
	class FMTspatialnodescache
	{
	class FMTnodecache
		{
		std::map<int, double>periodicvalues;
		public:
			bool worthintersecting;
			boost::unordered_map<Core::FMTmask, double>patternvalues;
			Core::FMTmask staticmask;
			Core::FMTmask dynamicmask;
			FMTnodecache() = default;
			FMTnodecache(const Core::FMToutputnode& node, const Models::FMTmodel& model);
			~FMTnodecache() = default;
			FMTnodecache(const FMTnodecache& rhs) = default;
			FMTnodecache& operator = (const FMTnodecache& rhs) = default;
			void setvalue(const int& period, const double& value);
			void removeperiod(const int& period);
			void clearperiod();
			void removevaluefromperiod(const int& period, const double& value);
			void addvaluefromperiod(const int& period, const double& value);
			bool gotcachevalue(const int& period) const;
			double getcachevalue(const int& period) const;
		};
	FMTnodecache* actualcache;
	Graph::FMToutputnodecache<FMTcoordinate>* staticnodes;
	boost::unordered_map<Core::FMTmask, FMTnodecache>patterncache;
	public:
		FMTspatialnodescache() = default;
		~FMTspatialnodescache();
		FMTspatialnodescache(const std::vector<FMTcoordinate>& mapping);
		FMTspatialnodescache(const FMTspatialnodescache& rhs);
		FMTspatialnodescache& operator = (const FMTspatialnodescache& rhs);
		const std::vector<FMTcoordinate>& getnode(const Core::FMToutputnode& node, const Models::FMTmodel& model,bool& exactnode, const Core::FMTmask& mask);
		void setnode(const Core::FMToutputnode& node, const std::vector<FMTcoordinate>& coordinates);
		void removeperiod(const int& period);
		FMTnodecache* getactualnodecache();
		void swap(FMTspatialnodescache& rhs);
		bool empty() const;
		size_t size() const;
		void insert(const FMTspatialnodescache& rhs);

	};

}

#endif // FMTLINEGRAPH_H
