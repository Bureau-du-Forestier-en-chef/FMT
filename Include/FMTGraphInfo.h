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
	// DocString: FMTGraphInfo
	/**
	@brief Information about a spatial line graph: its id and the interned constraint values shared across identical graphs.
	*/
	class FMTGraphInfo
		{
		public:
			// DocString: FMTGraphInfo()
			/**
			@brief Default constructor for FMTGraphInfo.
			*/
			FMTGraphInfo() = default;
			// DocString: FMTGraphInfo(size_t)
			/**
			@brief Construct a FMTGraphInfo from a graph id.
			@param[in] Id the graph id.
			*/
			FMTGraphInfo(size_t Id);
			// DocString: FMTGraphInfo::getValues
			/**
			@brief Return the values of a constraint for the graph.
			@param[in] p_ConstraintId the constraint id.
			@return the constraint values.
			*/
			const std::vector<double>& getValues(size_t p_ConstraintId) const;
			// DocString: FMTGraphInfo::getGraphId
			/**
			@brief Return the graph id.
			@return the graph id.
			*/
			inline size_t getGraphId() const
				{
				return m_GraphId;
				}
			// DocString: FMTGraphInfo::setValues
			/**
			@brief Set the interned values of a constraint for the graph.
			@param[in] p_ConstraintId the constraint id.
			@param[in] p_Values the interned values.
			*/
			void setValues(size_t p_ConstraintId,
							const std::shared_ptr<const FMTGraphValues>& p_Values);
			// DocString: FMTGraphInfo::setValuesSize
			/**
			@brief Reserve the number of constraint value vectors of the graph.
			@param[in] p_ValuesSize the number of constraints.
			*/
			void setValuesSize(size_t p_ValuesSize);
			// DocString: FMTGraphInfo::isEmpty
			/**
			@brief Return true if the graph info has no values.
			@return true if the graph info is empty else false.
			*/
			bool isEmpty() const;
		private:
			// Interned values are shared through shared_ptr (see FMTSpatialGraphs::_InternValues),
			// which replaces the former boost::flyweight process-wide static factory.
			std::vector<std::shared_ptr<const FMTGraphValues>> m_Values;
			size_t m_GraphId;
		};
	}


#endif 
