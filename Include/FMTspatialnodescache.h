/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#include <unordered_map>
#include <map>
#include "FMTmask.h"
#include "FMToutputnode.h"
#include "FMTutility.h"


#ifndef FMTSPATIALNODESCACHE_H
#define FMTSPATIALNODESCACHE_H
namespace Graph
{
	template<class T1,class T2>
	class FMToutputnodecache;
}

namespace Models
{
	class FMTmodel;
}

namespace Spatial
{
	class FMTcoordinate;
	class FMTEXPORT FMTspatialnodescache
	{
	class FMTEXPORT FMTnodecache
		{
		public:
			std::unordered_map<int, double>periodicvalues;
			bool worthintersecting;
			boost::unordered_map<Core::FMTmask, double>patternvalues;
			Core::FMTmask staticmask;
			Core::FMTmask dynamicmask;
			std::unordered_set<int>actions;
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
	Graph::FMToutputnodecache<FMTcoordinate,FMTcoordinate*>* staticnodes;
	//boost::unordered_map<Core::FMTmask, FMTnodecache>patterncache;
	public:
		typedef std::unordered_map<Core::FMToutputnode, FMTnodecache, Core::FMToutputnodehasher, Core::FMToutputnodevaluecomparator> ucaching;
	private:
		ucaching patterncache;
	public:
		FMTspatialnodescache() = default;
		~FMTspatialnodescache();
		FMTspatialnodescache(const std::vector<FMTcoordinate>& mapping);
		FMTspatialnodescache(const FMTspatialnodescache& rhs);
		FMTspatialnodescache& operator = (const FMTspatialnodescache& rhs);
		const std::vector<FMTcoordinate>& getnode(const Core::FMToutputnode& node, const Models::FMTmodel& model,bool& exactnode);
		void setnode(const Core::FMToutputnode& node, const std::vector<FMTcoordinate>& coordinates);
		void removeperiod(const int& period);
		void set(const Core::FMToutputnode& node);
		FMTnodecache* getactualnodecache();
		void setnodecache(ucaching::iterator cashit);
		void swap(FMTspatialnodescache& rhs);
		void erasenode(const Core::FMToutputnode& node);
		bool isworthy(const Core::FMToutputnode& node) const;
		bool empty() const;
		size_t size() const;
		void insert(const FMTspatialnodescache& rhs);
		ucaching::iterator begin();
		ucaching::iterator end();

	};

}

#endif // FMTLINEGRAPH_H
