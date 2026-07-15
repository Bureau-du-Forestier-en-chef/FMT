/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTyieldmodelTSLA.h"
#include "FMTgraph.hpp"
#include "FMTlinegraph.h"
#include "FMTyieldrequest.h"
#include "FMTexceptionhandler.h"
#include "FMTgraphvertextoyield.h"



namespace Core {


	FMTyieldmodelTSLA::operator std::string() const
	{
		return "";
	}

	FMTyieldmodelTSLA::FMTyieldmodelTSLA() :FMTyieldmodel() 
	{
		modelName = FMTyieldmodelTSLA::getModelType();
	}


	std::string FMTyieldmodelTSLA::getModelType()
	{
		return "_TSLA";
	}



	std::unique_ptr<FMTyieldmodel>FMTyieldmodelTSLA::Clone() const
	{
		try {
			return std::unique_ptr<FMTyieldmodel>(new FMTyieldmodelTSLA());
		}
		catch (...)
		{
			_exhandler->raiseFromCatch(getModelType(), "FMTyieldmodelTSLA::Clone", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTyieldmodel>(nullptr);
	}

	const std::vector<double>FMTyieldmodelTSLA::predict(const Core::FMTyieldrequest& request) const
	{
		std::vector<double>returned(1, std::numeric_limits<double>::max());
		try {
			const Graph::FMTgraphvertextoyield* graphinfo = request.getVertexGraphInfo();
			if (!graphinfo)
				{
				_exhandler->raise(Exception::FMTexc::FMTrangeerror, "Empty graph info info for " + getModelType(),
					"FMTyieldmodelTSLA::predict", __LINE__, __FILE__);
				}
			const Graph::FMTgraph<Graph::FMTbasevertexproperties, Graph::FMTbaseedgeproperties>* linegraph = graphinfo->getLineGraph();
			const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>* fullgraph = graphinfo->getFullGraph();
			size_t tsla = std::numeric_limits<size_t>::max();
			if (linegraph)
			{
				const Graph::FMTgraph<Graph::FMTbasevertexproperties, Graph::FMTbaseedgeproperties>::FMTvertex_descriptor* vertex = linegraph->getVertexFromVertexInfo(graphinfo);
				tsla = linegraph->timeSinceLastAction(*vertex);
			}else if (fullgraph)
			{
				const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor* vertex = fullgraph->getVertexFromVertexInfo(graphinfo);
				tsla = fullgraph->timeSinceLastAction(*vertex);
			}else {
				_exhandler->raise(Exception::FMTexc::FMTrangeerror, "No graph in graph info " + getModelType(),
					"FMTyieldmodelTSLA::predict", __LINE__, __FILE__);
			}
			if (tsla != std::numeric_limits<size_t>::max())
				{
				returned[0] = static_cast<double>(tsla);
			}

		}catch (...)
			{
			_exhandler->raiseFromCatch(getModelType(), "FMTyieldmodelTSLA::predict", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return returned;
	}

	std::unique_ptr<FMTyieldmodel> FMTyieldmodelTSLA::presolve(const FMTmaskfilter& filter,
		const std::vector<FMTtheme>& newthemes) const
	{
		try {
			return Clone();
		}
		catch (...)
		{
			_exhandler->raiseFromCatch(getModelType(), "FMTyieldmodelTSLA::presolve", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTyieldmodel>(nullptr);
	}

	std::unique_ptr<FMTyieldmodel> FMTyieldmodelTSLA::postSolve(const FMTmaskfilter& filter,
		const std::vector<FMTtheme>& basethemes) const
	{
		try {
			return Clone();
		}
		catch (...)
		{
			_exhandler->raiseFromCatch(getModelType(), "FMTyieldmodelTSLA::postSolve", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTyieldmodel>(nullptr);
	}



}