/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTmodelyieldhandler.hpp"
#include "FMTyieldrequest.hpp"
#include "FMTdevelopment.hpp"
#include "FMTyieldmodel.hpp"
#include "FMTexceptionhandler.hpp"

namespace Core {


	FMTmodelyieldhandler::operator std::string() const
	{
		std::string value;
		try {
			value += "*YM " + std::string(mask) + "\n";
			std::vector<std::string>modelslines(models.size());
			for (const auto& data : yldnames)
				{
				modelslines[data.second.first] += data.first + ",";
				}
			size_t modelid = 0;
			for(std::string& line : modelslines)
				{
				line.pop_back();
				value += line + " _PRED(" + models.at(modelid)->GetModelName() + ")\n";
				++modelid;
				}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTmodelyieldhandler::std::string()", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return value;
	}


	
	FMTyldtype FMTmodelyieldhandler::gettype() const
	{
		return FMTyldtype::FMTmodelyld;
	}

	FMTmodelyieldhandler::FMTmodelyieldhandler(const FMTmask& mask) :
		FMTyieldhandler(mask),models(),yldnames()
	{

	}

	FMTmodelyieldhandler::FMTmodelyieldhandler() :
		FMTyieldhandler(), models(), yldnames()
	{

	}
	FMTmodelyieldhandler::FMTmodelyieldhandler(const FMTmodelyieldhandler& rhs) :
		FMTyieldhandler(rhs), models(), yldnames(rhs.yldnames)
	{
		for (const std::unique_ptr<FMTyieldmodel>& model : rhs.models)
		{
			models.push_back(std::move(model->Clone()));
		}
	}

	FMTmodelyieldhandler& FMTmodelyieldhandler::operator = (const FMTmodelyieldhandler& rhs)
	{
		if (this!=&rhs)
		{
			FMTyieldhandler::operator=(rhs);
			for (const std::unique_ptr<FMTyieldmodel>& model : rhs.models)
				{
				models.push_back(std::move(model->Clone()));
				}
			yldnames = rhs.yldnames;

		}
		return *this;
	}


	std::map<std::string, size_t>FMTmodelyieldhandler::getmodelsnamebyindex() const
	{
		std::map<std::string, size_t>modelmapping;
		try {
			size_t location = 0;
			for (const std::unique_ptr<FMTyieldmodel>& model : models)
			{
				modelmapping[model->GetModelName()] = location;
				++location;
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTmodelyieldhandler::getmodelsnamebyindex", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return modelmapping;
	}


	bool FMTmodelyieldhandler::operator == (const FMTmodelyieldhandler& rhs) const
	{
		return (FMTyieldhandler::operator==(rhs));
	}


	double FMTmodelyieldhandler::get(const std::string& yld, const FMTyieldrequest& request) const
	{
		try {
			const size_t modelid = yldnames.at(yld).first;
			const size_t yieldid = yldnames.at(yld).second;
			const std::unique_ptr<FMTyieldmodel>&model = models.at(modelid);
			//const std::vector<std::string>sources = model->GetYieldsOutputs();
			/*if (lookat.find(yld) == lookat.end())
			{
				lookat.insert(yld);
			}
			else {
				_exhandler->raise(Exception::FMTexc::FMTinvalid_yield, "Recursivity detected for complexe yield " + yld,
					"FMTmodelyieldhandler::get", __LINE__, __FILE__, Core::FMTsection::Yield);
			}*/
			const std::vector<double>predictions = model->Predict(request);
			return (predictions.at(yieldid));
		}catch (...)
			{
			_exhandler->raisefromcatch("at yield " + yld, "FMTmodelyieldhandler::get", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return 0;
	}

	bool FMTmodelyieldhandler::empty() const
	{
		return models.empty();
	}
	size_t FMTmodelyieldhandler::size() const
	{
		return yldnames.size();
	}

	void FMTmodelyieldhandler::push_backmodel(const std::unique_ptr<FMTyieldmodel>& model)
	{
		models.push_back(std::move(model->Clone()));
	}
	void FMTmodelyieldhandler::setyield(const size_t& modelid, const size_t& yieldid, const std::string& yldname)
	{
		yldnames[yldname] = std::pair<size_t,size_t>(modelid,yieldid);
	}
	
	bool FMTmodelyieldhandler::containsyield(const std::string& yldname) const
	{
		return (yldnames.find(yldname) != yldnames.end());
	}

	bool FMTmodelyieldhandler::isnullyield(const std::string& yldname) const
	{
		return false;
	}

	std::vector<std::string>FMTmodelyieldhandler::getyieldnames() const
	{
		std::vector<std::string>results;
		results.reserve(yldnames.size());
		for (const auto& data : yldnames)
		{
			results.push_back(data.first);
		}
		return results;
	}

	void FMTmodelyieldhandler::clearcache()
	{
		
	}

	std::unique_ptr<FMTyieldhandler>FMTmodelyieldhandler::clone() const
	{
		return std::unique_ptr<FMTyieldhandler>(new FMTmodelyieldhandler(*this));
	}


}

