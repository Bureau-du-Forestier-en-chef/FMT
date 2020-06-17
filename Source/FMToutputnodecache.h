/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMToutputnodecache_H
#define FMToutputnodecache_H

#include<map>
#include<vector>
#include <unordered_map>
#include <FMTgraphdescription.h>
#include "FMTtheme.h"
#include "FMTaction.h"
#include "FMToutputnode.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

namespace Graph
{

	class FMToutputnodecache
	{
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & BOOST_SERIALIZATION_NVP(basenode);
			ar & BOOST_SERIALIZATION_NVP(searchtree);
		}
		std::vector<FMTvertex_descriptor>basenode;
		mutable std::map<Core::FMToutputnode, std::vector<FMTvertex_descriptor>>searchtree;
		void setinitialcache(const std::unordered_map<size_t, FMTvertex_descriptor>& initialgraph);
		const std::vector<FMTvertex_descriptor>& getcleandescriptors(const Core::FMToutputnode& targetnode,const std::vector<Core::FMTaction>& actions,
										const std::vector<Core::FMTtheme>&themes, bool& exactnode) const;
		void getactionrebuild(const Core::FMToutputnode& targetnode,
			const std::vector<Core::FMTaction>& actions,
			std::vector<FMTvertex_descriptor>& cleaned,
			bool& exactnode) const;
		std::map<Core::FMToutputnode, std::vector<FMTvertex_descriptor>>::const_iterator getparentnode(const Core::FMToutputnode& targetnode, const std::vector<Core::FMTaction>& actions, bool& exactnode) const;
	public:
		FMToutputnodecache();
		FMToutputnodecache(const FMToutputnodecache& rhs);
		FMToutputnodecache& operator = (const FMToutputnodecache& rhs);
		~FMToutputnodecache() = default;
		FMToutputnodecache(const std::unordered_map<size_t, FMTvertex_descriptor>& initialgraph);
		const std::vector<FMTvertex_descriptor>& getverticies(const Core::FMToutputnode& targetnode, const std::vector<Core::FMTaction>& actions,
			const std::vector<Core::FMTtheme>&themes, bool& exactvecticies) const;
		void setvalidverticies(const Core::FMToutputnode& targetnode,const std::vector<FMTvertex_descriptor>& verticies) const;
		void clear();
	};

}

#endif