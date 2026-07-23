/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifndef FMTPREDICTOR_H
#define FMTPREDICTOR_H


#include <vector>
#include <string>
#include <map>
#include "FMTutility.h"



namespace Core
{
	class FMTYields;
	class FMTAction;
}

namespace Graph
{
	class FMTBaseVertexProperties;
	class FMTBaseEdgeProperties;
	// DocString: FMTPredictor
	/**
	@brief Predictor gathering the source and target ages, yields, distances and disturbances of a graph transition, used to build the predictor variables of a machine learning yield model.
	*/
	class FMTEXPORT FMTPredictor
	{
	public:
		// DocString: FMTPredictor()
		/**
		@brief Default constructor for FMTPredictor.
		*/
		FMTPredictor();
		// DocString: ~FMTPredictor()
		/**
		@brief Default destructor for FMTPredictor.
		*/
		~FMTPredictor() = default;
		// DocString: FMTPredictor(...)
		/**
		@brief Construct a FMTPredictor from the source and target vertices of a graph transition and the edges between them.
		@param[in] actions the model actions.
		@param[in] yieldnames the yield names.
		@param[in] yields the yields.
		@param[in] source the source vertex.
		@param[in] target the target vertex.
		@param[in] edges the edges between the source and the target.
		@param[in] gaps the period gaps.
		@param[in] withGCBMid if true uses the GCBM id.
		*/
		FMTPredictor(const std::vector<Core::FMTAction>& actions, const std::vector<std::string>& yieldnames,const Core::FMTYields& yields,
			const FMTBaseVertexProperties& source, const FMTBaseVertexProperties& target, const std::vector<const FMTBaseEdgeProperties*>& edges, const std::vector<int>& gaps,bool withGCBMid = true);
		// DocString: FMTPredictor(const FMTPredictor&)
		/**
		@brief Copy constructor for FMTPredictor.
		@param[in] rhs the FMTPredictor to copy.
		*/
		FMTPredictor(const FMTPredictor& rhs);
		// DocString: FMTPredictor::operator=
		/**
		@brief Copy assignment operator for FMTPredictor.
		@param[in] rhs the FMTPredictor to copy.
		@return a reference to this FMTPredictor.
		*/
		FMTPredictor& operator = (const FMTPredictor& rhs);
		// DocString: FMTPredictor::operator==
		/**
		@brief Comparison operator for FMTPredictor.
		@param[in] rhs the FMTPredictor to compare to.
		@return true if both predictors are equal else false.
		*/
		bool operator==(const FMTPredictor& rhs) const;
		// DocString: FMTPredictor::operator<
		/**
		@brief Less than operator for FMTPredictor.
		@param[in] rhs the FMTPredictor to compare to.
		@return true if this predictor is less than the other else false.
		*/
		bool operator<(const FMTPredictor& rhs) const;
		// DocString: FMTPredictor::getPredictors
		/**
		@brief Return the predictor values.
		@return the predictor values.
		*/
		std::vector<double>getPredictors() const;
		// DocString: FMTPredictor::getDistance
		/**
		@brief Return the distance to an action.
		@param[in] actId the action id.
		@return the distance.
		*/
		double getDistance(size_t actId) const;
		// DocString: FMTPredictor::getDisturbance
		/**
		@brief Return the disturbance of an action.
		@param[in] actId the action id.
		@return the disturbance.
		*/
		double getDisturbance(size_t actId) const;
		// DocString: FMTPredictor::getSourceAge
		/**
		@brief Return the age of the source vertex.
		@return the source age.
		*/
		double getSourceAge() const;
		// DocString: FMTPredictor::getSourceYields
		/**
		@brief Return the yields of the source vertex.
		@return the source yields.
		*/
		std::vector<double> getSourceYields() const;
		// DocString: FMTPredictor::getTargetAge
		/**
		@brief Return the age of the target vertex.
		@return the target age.
		*/
		double getTargetAge() const;
		// DocString: FMTPredictor::getTargetYields
		/**
		@brief Return the yields of the target vertex.
		@return the target yields.
		*/
		std::vector<double> getTargetYields() const;
		// DocString: FMTPredictor::getPredictorNames
		/**
		@brief Return the names of the predictors.
		@param[in] yieldnames the yield names.
		@return the predictor names.
		*/
		std::vector<std::string>getPredictorNames(const std::vector<std::string>& yieldnames)const;
	private:
		const FMTBaseVertexProperties* m_source_vertex;
		const FMTBaseVertexProperties* m_target_vertex;
		std::vector<double>m_source_yields;
		std::vector<double>m_target_yields;
		std::vector<int>m_periodgaps;
		std::vector<int>m_sourceactions;
		// DocString: FMTPredictor::_getYields
		/**
		@brief Return the yields of a vertex.
		@param[in] vertex the vertex.
		@param[in] yields the yields.
		@param[in] yieldnames the yield names.
		@return the yields.
		*/
		std::vector<double>_getYields(const FMTBaseVertexProperties& vertex, const Core::FMTYields& yields, const std::vector<std::string>& yieldnames) const;
	};
}

#endif // predictor
