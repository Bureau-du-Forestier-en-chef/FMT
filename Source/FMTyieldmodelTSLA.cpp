/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTyieldmodelTSLA.hpp"
#include "FMTgraph.hpp"
#include "FMTlinegraph.hpp"
#include "FMTyieldrequest.hpp"
#include "FMTexceptionhandler.hpp"
#include "FMTgraphvertextoyield.hpp"



namespace Core {


	FMTyieldmodelTSLA::operator std::string() const
	{
		return "";
	}

	FMTyieldmodelTSLA::FMTyieldmodelTSLA() :FMTyieldmodel() 
	{
		modelName = FMTyieldmodelTSLA::GetModelType();
	}


	std::string FMTyieldmodelTSLA::GetModelType()
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
			_exhandler->raisefromcatch(GetModelType(), "FMTyieldmodelTSLA::Clone", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTyieldmodel>(nullptr);
	}

	const std::vector<double>FMTyieldmodelTSLA::Predict(const Core::FMTyieldrequest& request) const
	{
		std::vector<double>returned(1, std::numeric_limits<double>::max());
		try {
			const Graph::FMTgraphvertextoyield* graphinfo = request.getvertexgraphinfo();
			if (!graphinfo)
				{
				_exhandler->raise(Exception::FMTexc::FMTrangeerror, "Empty graph info info for " + GetModelType(),
					"FMTyieldmodelTSLA::Predict", __LINE__, __FILE__);
				}
			const Graph::FMTgraph<Graph::FMTbasevertexproperties, Graph::FMTbaseedgeproperties>* linegraph = graphinfo->getlinegraph();
			const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>* fullgraph = graphinfo->getfullgraph();
			size_t tsla = std::numeric_limits<size_t>::max();
			if (linegraph)
			{
				const Graph::FMTgraph<Graph::FMTbasevertexproperties, Graph::FMTbaseedgeproperties>::FMTvertex_descriptor* vertex = linegraph->getvertexfromvertexinfo(graphinfo);
				tsla = linegraph->timesincelastaction(*vertex);
			}else if (fullgraph)
			{
				const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor* vertex = fullgraph->getvertexfromvertexinfo(graphinfo);
				tsla = fullgraph->timesincelastaction(*vertex);
			}else {
				_exhandler->raise(Exception::FMTexc::FMTrangeerror, "No graph in graph info " + GetModelType(),
					"FMTyieldmodelTSLA::Predict", __LINE__, __FILE__);
			}
			if (tsla != std::numeric_limits<size_t>::max())
				{
				returned[0] = static_cast<double>(tsla);
				}

		}catch (...)
			{
			_exhandler->raisefromcatch(GetModelType(), "FMTyieldmodelTSLA::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
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
			_exhandler->raisefromcatch(GetModelType(), "FMTyieldmodelTSLA::presolve", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTyieldmodel>(nullptr);
	}

	std::unique_ptr<FMTyieldmodel> FMTyieldmodelTSLA::postsolve(const FMTmaskfilter& filter,
		const std::vector<FMTtheme>& basethemes) const
	{
		try {
			return Clone();
		}
		catch (...)
		{
			_exhandler->raisefromcatch(GetModelType(), "FMTyieldmodelTSLA::postsolve", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTyieldmodel>(nullptr);
	}



}