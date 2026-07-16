/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTmodelyieldhandler.h"
#include "FMTyieldrequest.h"
#include "FMTdevelopment.h"
#include "FMTyieldmodel.h"
#include "FMTexceptionhandler.h"
#include <boost\filesystem\path.hpp>
#include "FMTtimeyieldhandler.h"

namespace Core {


	FMTModelYieldHandler::operator std::string() const
	{
		std::string value;
		try {
			FMTTimeYieldHandler potentialtime = toTimeHandler();
			if (!potentialtime.empty())
				{
				return std::string(potentialtime);
			}
			else {
				bool dont_write = true;
				value += "*YM " + std::string(mask) + "\n";
				std::vector<std::string>modelslines(models.size());
				for (const auto& data : m_yldnames)
				{
					modelslines[data.second.first] += data.first + ",";
				}
				size_t modelid = 0;
				for (const std::unique_ptr<FMTYieldModel>& model : models)
				{
					const std::string model_data = std::string(*model);
					if (!model_data.empty())
					{
						modelslines[modelid].pop_back();
						value += modelslines[modelid] + model_data + "\n";
						dont_write = false;
					}
					
					++modelid;
				}
				if (dont_write)
					{
					value.clear();
					}
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTModelYieldHandler::std::string()", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return value;
	}

	void FMTModelYieldHandler::setModel(Models::FMTModel* p_modelPtr)
	{
		for (const std::unique_ptr<FMTYieldModel>& model : models)
		{
			model->setModel(p_modelPtr);
		}
	}


	FMTTimeYieldHandler FMTModelYieldHandler::toTimeHandler() const
		{
		try {
			FMTTimeYieldHandler newhandler(getMask());
			bool gotallmodel = true;
			size_t modelid = 0;
			for (const std::unique_ptr<FMTYieldModel>& model : models)
				{
				std::vector<std::vector<double>> values = model->getPeriodicValues();
				if (values.empty())
				{
					gotallmodel = false;
					break;
				}else {
					std::map<size_t, std::string>modelmapping;
					for (const auto& data : m_yldnames)
					{
						if (modelid == data.second.first)
						{
							modelmapping[data.second.second] = data.first;
						}
					}
					size_t yieldid = 0;
					for (const std::vector<double>& yield_values : values)
						{
						newhandler.setYieldValues(modelmapping.at(yieldid), 0, yield_values);
						++yieldid;
						}
					}
				++modelid;
				}
			if (gotallmodel)
			{
				return newhandler;
			}
		
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTModelYieldHandler::toTimeHandler", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return FMTTimeYieldHandler(getMask());
		}
	
	FMTyldtype FMTModelYieldHandler::getType() const
	{
		return FMTyldtype::FMTmodelyld;
	}

	FMTModelYieldHandler::FMTModelYieldHandler(const FMTMask& mask) :
		FMTYieldHandler(mask),models(),m_yldnames()
	{

	}

	FMTModelYieldHandler::FMTModelYieldHandler() :
		FMTYieldHandler(), models(), m_yldnames()
	{

	}
	FMTModelYieldHandler::FMTModelYieldHandler(const FMTModelYieldHandler& rhs) :
		FMTYieldHandler(rhs), models(), m_yldnames(rhs.m_yldnames)
	{
		for (const std::unique_ptr<FMTYieldModel>& model : rhs.models)
		{
			models.push_back(std::move(model->Clone()));
		}
	}

	FMTModelYieldHandler& FMTModelYieldHandler::operator = (const FMTModelYieldHandler& rhs)
	{
		if (this!=&rhs)
		{
			FMTYieldHandler::operator=(rhs);
			for (const std::unique_ptr<FMTYieldModel>& model : rhs.models)
				{
				models.push_back(std::move(model->Clone()));
				}
			m_yldnames = rhs.m_yldnames;

		}
		return *this;
	}


	std::map<std::string, size_t>FMTModelYieldHandler::getModelsNameByIndex() const
	{
		std::map<std::string, size_t>modelmapping;
		try {
			size_t location = 0;
			for (const std::unique_ptr<FMTYieldModel>& model : models)
			{
				modelmapping[model->getModelName()] = location;
				++location;
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTModelYieldHandler::getModelsNameByIndex", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return modelmapping;
	}


	bool FMTModelYieldHandler::operator == (const FMTModelYieldHandler& rhs) const
	{
		return (FMTYieldHandler::operator==(rhs));
	}


	double FMTModelYieldHandler::get(const std::string& yld, const FMTYieldRequest& request) const
	{
		try {
			const size_t modelid = m_yldnames.at(yld).first;
			const size_t yieldid = m_yldnames.at(yld).second;
			const std::unique_ptr<FMTYieldModel>&model = models.at(modelid);
			//const std::vector<std::string>sources = model->GetYieldsOutputs();
			/*if (lookat.find(yld) == lookat.end())
			{
				lookat.insert(yld);
			}
			else {
				_exhandler->raise(Exception::FMTexc::FMTinvalid_yield, "Recursivity detected for complexe yield " + yld,
					"FMTModelYieldHandler::get", __LINE__, __FILE__, Core::FMTsection::Yield);
			}*/
			const std::vector<double>predictions = model->predict(request);
			return (predictions.at(yieldid));
		}catch (...)
			{
			_exhandler->raiseFromCatch("at yield " + yld, "FMTModelYieldHandler::get", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return 0;
	}

	bool FMTModelYieldHandler::empty() const
	{
		return models.empty();
	}
	size_t FMTModelYieldHandler::size() const
	{
		return m_yldnames.size();
	}

	void FMTModelYieldHandler::pushBackModel(const std::unique_ptr<FMTYieldModel>& model)
	{
		models.push_back(std::move(model->Clone()));
	}
	void FMTModelYieldHandler::setYield(const size_t& modelid, const size_t& yieldid, const std::string& yldname)
	{
		m_yldnames[yldname] = std::pair<size_t,size_t>(modelid,yieldid);
	}
	
	bool FMTModelYieldHandler::containsYield(const std::string& yldname) const
	{
		return (m_yldnames.find(yldname) != m_yldnames.end());
	}

	bool FMTModelYieldHandler::isNullYield(const std::string& yldname) const
	{
		return false;
	}

	std::vector<std::string>FMTModelYieldHandler::getYieldNames() const
	{
		std::vector<std::string>results;
		results.reserve(m_yldnames.size());
		for (const auto& data : m_yldnames)
		{
			results.push_back(data.first);
		}
		return results;
	}

	void FMTModelYieldHandler::clearCache()
	{
		
	}

	std::unique_ptr<FMTYieldHandler>FMTModelYieldHandler::clone() const
	{
		return std::unique_ptr<FMTYieldHandler>(new FMTModelYieldHandler(*this));
	}


	std::unique_ptr<FMTYieldHandler> FMTModelYieldHandler::presolve(const FMTMaskFilter& filter, const std::vector<FMTTheme>& newthemes) const
	{
		try {
			FMTModelYieldHandler newhandler(*this);
			newhandler.models.clear();
			for (const std::unique_ptr<FMTYieldModel>& yieldmodelptr : models)
				{
				newhandler.models.push_back(yieldmodelptr->presolve(filter, newthemes));
				}
			return std::unique_ptr<FMTYieldHandler>(new FMTModelYieldHandler(newhandler));
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTModelYieldHandler::presolve", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTYieldHandler>(nullptr);
	}
	void FMTModelYieldHandler::clearRandomYieldsCache()
	{
		for (std::unique_ptr<FMTYieldModel>& yieldModelPtr : models)
			{
			yieldModelPtr->clearRandomYieldsCache();
			}
	}


	std::unique_ptr<FMTYieldHandler> FMTModelYieldHandler::postSolve(const FMTMaskFilter& filter, const std::vector<FMTTheme>& basethemes) const
	{
		try {
			FMTModelYieldHandler newhandler(*this);
			newhandler.models.clear();
			for (const std::unique_ptr<FMTYieldModel>& yieldmodelptr : models)
			{
				newhandler.models.push_back(yieldmodelptr->postSolve(filter, basethemes));
			}
			return newhandler.FMTYieldHandler::postSolve(filter, basethemes);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTModelYieldHandler::postSolve", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTYieldHandler>(nullptr);
	}


}

