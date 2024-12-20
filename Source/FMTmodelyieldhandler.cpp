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


	FMTmodelyieldhandler::operator std::string() const
	{
		std::string value;
		try {
			FMTtimeyieldhandler potentialtime = totimehandler();
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
				for (const std::unique_ptr<FMTyieldmodel>& model : models)
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
			_exhandler->raisefromcatch("", "FMTmodelyieldhandler::std::string()", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return value;
	}

	void FMTmodelyieldhandler::setModel(Models::FMTmodel* p_modelPtr)
	{
		for (const std::unique_ptr<FMTyieldmodel>& model : models)
		{
			model->setModel(p_modelPtr);
		}
	}


	FMTtimeyieldhandler FMTmodelyieldhandler::totimehandler() const
		{
		try {
			FMTtimeyieldhandler newhandler(getmask());
			bool gotallmodel = true;
			size_t modelid = 0;
			for (const std::unique_ptr<FMTyieldmodel>& model : models)
				{
				std::vector<std::vector<double>> values = model->getperiodicvalues();
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
						newhandler.setyieldvalues(modelmapping.at(yieldid), 0, yield_values);
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
			_exhandler->raisefromcatch("", "FMTmodelyieldhandler::totimehandler", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return FMTtimeyieldhandler(getmask());
		}
	
	FMTyldtype FMTmodelyieldhandler::gettype() const
	{
		return FMTyldtype::FMTmodelyld;
	}

	FMTmodelyieldhandler::FMTmodelyieldhandler(const FMTmask& mask) :
		FMTyieldhandler(mask),models(),m_yldnames()
	{

	}

	FMTmodelyieldhandler::FMTmodelyieldhandler() :
		FMTyieldhandler(), models(), m_yldnames()
	{

	}
	FMTmodelyieldhandler::FMTmodelyieldhandler(const FMTmodelyieldhandler& rhs) :
		FMTyieldhandler(rhs), models(), m_yldnames(rhs.m_yldnames)
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
			m_yldnames = rhs.m_yldnames;

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
			const size_t modelid = m_yldnames.at(yld).first;
			const size_t yieldid = m_yldnames.at(yld).second;
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
		return m_yldnames.size();
	}

	void FMTmodelyieldhandler::push_backmodel(const std::unique_ptr<FMTyieldmodel>& model)
	{
		models.push_back(std::move(model->Clone()));
	}
	void FMTmodelyieldhandler::setyield(const size_t& modelid, const size_t& yieldid, const std::string& yldname)
	{
		m_yldnames[yldname] = std::pair<size_t,size_t>(modelid,yieldid);
	}
	
	bool FMTmodelyieldhandler::containsyield(const std::string& yldname) const
	{
		return (m_yldnames.find(yldname) != m_yldnames.end());
	}

	bool FMTmodelyieldhandler::isnullyield(const std::string& yldname) const
	{
		return false;
	}

	std::vector<std::string>FMTmodelyieldhandler::getyieldnames() const
	{
		std::vector<std::string>results;
		results.reserve(m_yldnames.size());
		for (const auto& data : m_yldnames)
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


	std::unique_ptr<FMTyieldhandler> FMTmodelyieldhandler::presolve(const FMTmaskfilter& filter, const std::vector<FMTtheme>& newthemes) const
	{
		try {
			FMTmodelyieldhandler newhandler(*this);
			newhandler.models.clear();
			for (const std::unique_ptr<FMTyieldmodel>& yieldmodelptr : models)
				{
				newhandler.models.push_back(yieldmodelptr->presolve(filter, newthemes));
				}
			return std::unique_ptr<FMTyieldhandler>(new FMTmodelyieldhandler(newhandler));
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTmodelyieldhandler::presolve", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTyieldhandler>(nullptr);
	}
	void FMTmodelyieldhandler::clearRandomYieldsCache()
	{
		for (std::unique_ptr<FMTyieldmodel>& yieldModelPtr : models)
			{
			yieldModelPtr->clearRandomYieldsCache();
			}
	}


	std::unique_ptr<FMTyieldhandler> FMTmodelyieldhandler::postsolve(const FMTmaskfilter& filter, const std::vector<FMTtheme>& basethemes) const
	{
		try {
			FMTmodelyieldhandler newhandler(*this);
			newhandler.models.clear();
			for (const std::unique_ptr<FMTyieldmodel>& yieldmodelptr : models)
			{
				newhandler.models.push_back(yieldmodelptr->postsolve(filter, basethemes));
			}
			return newhandler.FMTyieldhandler::postsolve(filter, basethemes);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTmodelyieldhandler::postsolve", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTyieldhandler>(nullptr);
	}


}

