/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTYieldModelTsla.h"
#include "FMTGraph.hpp"
#include "FMTLineGraph.h"
#include "FMTYieldRequest.h"
#include "FMTExceptionHandler.h"
#include "FMTGraphVertexToYield.h"



namespace Core {


	FMTYieldModelTsla::operator std::string() const
	{
		return "";
	}

	FMTYieldModelTsla::FMTYieldModelTsla() :FMTYieldModel() 
	{
		modelName = FMTYieldModelTsla::getModelType();
	}


	std::string FMTYieldModelTsla::getModelType()
	{
		return "_TSLA";
	}



	std::unique_ptr<FMTYieldModel>FMTYieldModelTsla::Clone() const
	{
		try {
			return std::unique_ptr<FMTYieldModel>(new FMTYieldModelTsla());
		}
		catch (...)
		{
			_exhandler->raiseFromCatch(getModelType(), "FMTYieldModelTsla::Clone", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTYieldModel>(nullptr);
	}

	const std::vector<double>FMTYieldModelTsla::predict(const Core::FMTYieldRequest& request) const
	{
		std::vector<double>returned(1, std::numeric_limits<double>::max());
		try {
			const Graph::FMTGraphVertexToYield* graphinfo = request.getVertexGraphInfo();
			if (!graphinfo)
				{
				_exhandler->raise(Exception::FMTexc::FMTrangeerror, "Empty graph info info for " + getModelType(),
					"FMTYieldModelTsla::predict", __LINE__, __FILE__);
				}
			const Graph::FMTGraph<Graph::FMTBaseVertexProperties, Graph::FMTBaseEdgeProperties>* linegraph = graphinfo->getLineGraph();
			const Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>* fullgraph = graphinfo->getFullGraph();
			size_t tsla = std::numeric_limits<size_t>::max();
			if (linegraph)
			{
				const Graph::FMTGraph<Graph::FMTBaseVertexProperties, Graph::FMTBaseEdgeProperties>::FMTvertex_descriptor* vertex = linegraph->getVertexFromVertexInfo(graphinfo);
				tsla = linegraph->timeSinceLastAction(*vertex);
			}else if (fullgraph)
			{
				const Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor* vertex = fullgraph->getVertexFromVertexInfo(graphinfo);
				tsla = fullgraph->timeSinceLastAction(*vertex);
			}else {
				_exhandler->raise(Exception::FMTexc::FMTrangeerror, "No graph in graph info " + getModelType(),
					"FMTYieldModelTsla::predict", __LINE__, __FILE__);
			}
			if (tsla != std::numeric_limits<size_t>::max())
				{
				returned[0] = static_cast<double>(tsla);
			}

		}catch (...)
			{
			_exhandler->raiseFromCatch(getModelType(), "FMTYieldModelTsla::predict", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return returned;
	}

	std::unique_ptr<FMTYieldModel> FMTYieldModelTsla::presolve(const FMTMaskFilter& filter,
		const std::vector<FMTTheme>& newthemes) const
	{
		try {
			return Clone();
		}
		catch (...)
		{
			_exhandler->raiseFromCatch(getModelType(), "FMTYieldModelTsla::presolve", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTYieldModel>(nullptr);
	}

	std::unique_ptr<FMTYieldModel> FMTYieldModelTsla::postSolve(const FMTMaskFilter& filter,
		const std::vector<FMTTheme>& basethemes) const
	{
		try {
			return Clone();
		}
		catch (...)
		{
			_exhandler->raiseFromCatch(getModelType(), "FMTYieldModelTsla::postSolve", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTYieldModel>(nullptr);
	}



}