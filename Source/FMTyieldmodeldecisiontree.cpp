/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTyieldmodeldecisiontree.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include "FMTyieldrequest.hpp"
#include "FMToutputparser.cpp"



namespace Core {

	FMTyieldmodeldecisiontree::FMTyieldmodeldecisiontree(const boost::property_tree::ptree& jsonProps,const std::vector<std::string>& inputYields, const Core::FMTmask& mainmask):
		FMTyieldmodel()
	{
		try {
			const boost::filesystem::path fmtdll(getruntimelocation());
			boost::property_tree::ptree::const_assoc_iterator modelNameIt = jsonProps.find(JSON_PROP_MODEL_NAME);
			const boost::filesystem::path filenamepath(modelNameIt->second.data());
			modelName = (fmtdll / filenamepath).string();
			
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodeldecisiontree::FMTyieldmodeldecisiontree()", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
	}

	std::unique_ptr<FMTyieldmodel>FMTyieldmodeldecisiontree::Clone() const
	{
		try {
			return std::unique_ptr<FMTyieldmodel>(new FMTyieldmodeldecisiontree(*this));
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodeldecisiontree::Clone", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTyieldmodel>(nullptr);
	}

	const std::vector<double>FMTyieldmodeldecisiontree::Predict(const Core::FMTyieldrequest& request) const
	{
		std::vector<double>values;
		try {
			
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodeldecisiontree::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return values;
	}



}