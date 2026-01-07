/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifndef FMTGRAPHINFO_H
#define FMTGRAPHINFO_H


#include <vector>
#include "FMTGraphValues.h"
#include <boost/flyweight.hpp>

namespace Spatial
	{
	class FMTGraphInfo
		{
		public:
			FMTGraphInfo() = default;
			FMTGraphInfo(size_t Id);
			const std::vector<double>& GetValues(size_t p_ConstraintId) const;
			size_t GetGraphId() const;
			void SetValues(size_t p_ConstraintId,
							const std::vector<double>& p_Values);
			void SetValuesSize(size_t p_ValuesSize);
			bool IsEmpty() const;
		private:
			std::vector<boost::flyweight<FMTGraphValues>> m_Values;
			size_t m_GraphId;
		};
	}


#endif 
