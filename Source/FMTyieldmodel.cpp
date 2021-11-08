/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTyieldmodel.h"
#include "FMTyieldrequest.h"
#include "FMTgraph.h"
#include "FMTlinegraph.h"
#include "FMTyields.h"

namespace Core {

	std::string FMTyieldmodel::GetModelName() const
	{
		return modelname;
	}
	FMTyieldmodel::FMTyieldmodel(const std::string& name):
		modelname(name)
	{

	}
	std::unique_ptr<FMTyieldmodel>FMTyieldmodel::Clone() const
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTyieldmodel::Clone", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodel::Clone", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTyieldmodel>(nullptr);
	}
	std::string FMTyieldmodel::GetModelInfo() const
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTyieldmodel::GetModelInfo", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodel::GetModelInfo", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::string();
	}
	bool FMTyieldmodel::Validate(const std::vector<std::string>& YieldsAvailable) const
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTyieldmodel::Validate", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodel::Validate", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return false;
	}
	std::vector<std::string>FMTyieldmodel::GetYieldsOutputs() const
	{
		try {
			//_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
			//	"FMTyieldmodel::GetYieldsSources", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodel::GetYieldsSources", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::vector<std::string>();
	}


	std::vector<double>FMTyieldmodel::Predict(const Core::FMTyieldrequest& request) const
	{
		try {
			const Graph::FMTgraphvertextoyield* graphinfo = request.getvertexgraphinfo();
			const Graph::FMTlinegraph* linegraph = dynamic_cast<const Graph::FMTlinegraph*>(graphinfo->getlinegraph());
			if (linegraph!=nullptr)//Im a linegraph
				{
				const Graph::FMTgraph<Graph::FMTbasevertexproperties, Graph::FMTbaseedgeproperties>::FMTvertex_descriptor* vertex = linegraph->getvertexfromvertexinfo(graphinfo);
				const Models::FMTmodel& modelref = graphinfo->getmodel();
				//const Graph::FMTcarbonpredictor predictor = linegraph->getcarbonpredictors()
				}

			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTyieldmodel::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodel::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::vector<double>();
	}
	
}