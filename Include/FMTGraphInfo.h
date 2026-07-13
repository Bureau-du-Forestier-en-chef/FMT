/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifndef FMTGRAPHINFO_H
#define FMTGRAPHINFO_H


#include <vector>
#include <memory>
#include "FMTGraphValues.h"

namespace Spatial
	{
	class FMTGraphInfo
		{
		public:
			FMTGraphInfo() = default;
			FMTGraphInfo(size_t Id);
			const std::vector<double>& getValues(size_t p_ConstraintId) const;
			inline size_t getGraphId() const
				{
				return m_GraphId;
				}
			void setValues(size_t p_ConstraintId,
							const std::shared_ptr<const FMTGraphValues>& p_Values);
			void setValuesSize(size_t p_ValuesSize);
			bool isEmpty() const;
		private:
			// Interned values are shared through shared_ptr (see FMTSpatialGraphs::_InternValues),
			// which replaces the former boost::flyweight process-wide static factory.
			std::vector<std::shared_ptr<const FMTGraphValues>> m_Values;
			size_t m_GraphId;
		};
	}


#endif 
