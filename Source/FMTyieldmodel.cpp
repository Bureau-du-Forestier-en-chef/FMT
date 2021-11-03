/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTyieldmodel.h"

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
	std::vector<std::string>FMTyieldmodel::GetYieldsSources() const
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


	std::vector<double>FMTyieldmodel::Predict(const Graph::FMTgraphvertextoyield& graphinfo,
											 const std::vector<double>& sourceyieldvalues) const
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTyieldmodel::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodel::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::vector<double>();
	}
	size_t FMTyieldmodel::GetOutputSize() const
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTyieldmodel::GetOutputSize", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodel::GetOutputSize", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return 0;
	}
}