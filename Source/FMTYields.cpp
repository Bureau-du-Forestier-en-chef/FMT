/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTYields.h"
#include "FMTYieldRequest.h"
#include "FMTExceptionHandler.h"
#include "FMTAgeYieldHandler.h"
#include "FMTTimeYieldHandler.h"
#include "FMTComplexYieldHandler.h"
#include "FMTModelYieldHandler.h"
#include "FMTYieldModelTsla.h"
#include <boost/algorithm/string.hpp> 
#include <memory>
#include "FMTSpec.h"

namespace Core{

	const std::string FMTYields::m_nullYield = "~FMTNULLYIELD";

	const std::string& FMTYields::getNullYield()
		{
		return FMTYields::m_nullYield;
		}

	void FMTYields::pushBackAgeHandler(const FMTMask& mask, const FMTAgeYieldHandler& value)
	{
		try {
			std::unique_ptr<Core::FMTYieldHandler>handlerptr(new FMTAgeYieldHandler(value));
			push_back(mask, handlerptr);
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYields::push_back", __LINE__, __FILE__, FMTsection::Yield);
		}
	}

	void FMTYields::pushBackTimeHandler(const FMTMask& mask, const FMTTimeYieldHandler& value)
	{
		try {
			std::unique_ptr<Core::FMTYieldHandler>handlerptr(new FMTTimeYieldHandler(value));
			push_back(mask, handlerptr);
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYields::push_back", __LINE__, __FILE__, FMTsection::Yield);
		}
	}




FMTYields::FMTYields():FMTList<std::unique_ptr<FMTYieldHandler>>(), 
	m_yieldsLocations(), m_yieldsHash()
        {
		_updateYieldLocations();
		_updateHash();
        }
    FMTYields::FMTYields(const FMTYields& rhs): 
		FMTList<std::unique_ptr<FMTYieldHandler>>(rhs), 
		m_yieldsLocations(rhs.m_yieldsLocations),
		m_yieldsHash(rhs.m_yieldsHash)
        {

        }

	void FMTYields::swap(FMTYields& rhs)
		{
		FMTList<std::unique_ptr<FMTYieldHandler>>::swap(rhs);
		m_yieldsLocations.swap(rhs.m_yieldsLocations);
		std::swap(m_yieldsHash, rhs.m_yieldsHash);
		}

    FMTYields& FMTYields::operator = (const FMTYields& rhs)
        {
        if(this!=&rhs)
            {
			FMTList<std::unique_ptr<FMTYieldHandler>>::operator = (rhs);
			m_yieldsLocations = rhs.m_yieldsLocations;
			m_yieldsHash = rhs.m_yieldsHash;
            }
        return *this;
        }

	void FMTYields::generateDefaultYields(const std::vector<Core::FMTTheme>& themes)
	{
		try {
			std::string general_mask;
			for (const Core::FMTTheme& theme : themes)
				{
				general_mask += "? ";
				}
			general_mask.pop_back();
			const Core::FMTMask base_mask(general_mask, themes);
			FMTModelYieldHandler newhandler(base_mask);
			const std::unique_ptr<Core::FMTYieldModel>TSLA(new FMTYieldModelTsla());
			newhandler.pushBackModel(TSLA);
			newhandler.setYield(0, 0, TSLA->getModelName());
			const std::unique_ptr<Core::FMTYieldHandler>TSLAhandler(new FMTModelYieldHandler(newhandler));
			push_back(base_mask,TSLAhandler);
		}catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTYields::generateDefaultYields", __LINE__, __FILE__, FMTsection::Yield);
			}
	}


	std::vector<std::string>FMTYields::getStacked() const
        {
		std::vector<std::string>values;
		try {
			std::map<size_t, std::string>overrided;
			std::map<size_t, size_t>positions;
			bool inoverridesection = true;
			for (const auto& handlerobj : *this)
			{
				std::string value = std::string(*handlerobj.second);
				if (value.empty())
					{
					continue;
					}
				value += "\n";
				const size_t canbeoverride = handlerobj.second->getOverrideIndex();
				std::map<size_t, std::string>::const_iterator overfind = overrided.find(canbeoverride);
				const bool inoverridesection = (overrided.find(canbeoverride) == overrided.end());
				if (inoverridesection&&
					canbeoverride>0)
					{
					overrided[canbeoverride] = value;
				}else {
					const size_t position = values.size()+1;
					for (const size_t& tab : handlerobj.second->getTabous())
						{
						if (positions.find(tab)== positions.end())
							{
							positions[tab] = 0;
							}
						positions[tab] = std::max(positions.at(tab), position);
						}
					values.push_back(value);
				}
			}
			std::map<size_t,std::vector<std::string>>finaloverided;
			for (std::map<size_t, std::string>::const_iterator oit = overrided.begin(); oit!= overrided.end();++oit)
				{
				size_t location = values.size();
				if (positions.find(oit->first)!=positions.end())
				{
					location = positions.at(oit->first);
				}
				if (finaloverided.find(oit->first) == finaloverided.end())
				{
					finaloverided[location] = std::vector<std::string>();
				}
				finaloverided[location].push_back(oit->second);

				}
			for (std::map<size_t, std::vector<std::string>>::const_reverse_iterator oit = finaloverided.rbegin(); oit != finaloverided.rend(); ++oit)
			{
				values.insert(values.begin() + oit->first, oit->second.begin(), oit->second.end());
			}
			
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYields::getStacked", __LINE__, __FILE__,FMTsection::Yield);
		}
       
        return values;
        }

void FMTYields::_updateHash()
	{
	m_yieldsHash = 0;
	for (const auto& handlerObj : *this)
		{
		boost::hash_combine(m_yieldsHash, handlerObj.first.hash());
		boost::hash_combine(m_yieldsHash, std::string(*handlerObj.second));
		}
	}


void FMTYields::_updateYieldLocations()
    {
	try {
		m_yieldsLocations.clear();
		m_yieldsLocations[getNullYield()] = 0;
		size_t SIZE_OF = FMTList::size();
		size_t Id = 0;
		for (const auto& handlerObj : *this)
		{
			for (const std::string& yldName : handlerObj.second->getYieldNames())
			{
				std::pair<std::unordered_map<std::string, size_t>::iterator,bool>inserted = m_yieldsLocations.insert(std::pair<std::string, size_t>(yldName, SIZE_OF));
				if (inserted.first->second == SIZE_OF)
					{
					inserted.first->second = Id;
					}
			}
			++Id;
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTYields::updateyieldpresence", __LINE__, __FILE__, FMTsection::Yield);
	}

    }

std::vector<std::string> FMTYields::getAllYieldNames() const
{
	std::vector<std::string>alls;
	try {
		alls.push_back(getNullYield());
		for (const auto& handlerobj : *this)
			{
			for (const std::string& yldname : handlerobj.second->getYieldNames())
				{
				if (std::find(alls.begin(),alls.end(), yldname)== alls.end())
					{
					alls.push_back(yldname);
					}
				}
			}
	}catch (...)
		{
		_exhandler->raiseFromCatch("", "FMTYields::getAllYieldNames", __LINE__, __FILE__, FMTsection::Yield);
		}
	return alls;
	}

void FMTYields::clearCache()
	{
		//Clearcache of FMTList
		FMTList<std::unique_ptr<FMTYieldHandler>>::clearCache();
		for (auto& yh:*this)
		{
			yh.second->clearCache();
		}
	}

void FMTYields::clearRandomYieldsCache()
	{
	for (FMTYieldHandler*  handler : _getHandlers(FMTyldtype::FMTmodelyld))
		{
		dynamic_cast<FMTModelYieldHandler*>(handler)->clearRandomYieldsCache();
		}
	}

bool FMTYields::isYld(const std::string& value, bool fromsource) const
    {
	bool returnvalue = false;
	try{
	if (!fromsource)
	{
		std::unordered_map<std::string, size_t>::const_iterator presenceit = m_yieldsLocations.find(value);
		returnvalue = (presenceit != m_yieldsLocations.end());
	}else {
		for (const auto& handlerobj : *this)
		{
			if (handlerobj.second->containsYield(value))
				{
				return true;
				}
		}
	}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTYields::isYld", __LINE__, __FILE__, FMTsection::Yield);
	}
	return returnvalue;
    }

bool FMTYields::isNullYld(const std::string& value) const
	{
	bool gotNull = true;
	std::unordered_map<std::string, size_t>::const_iterator presenceIt = m_yieldsLocations.find(value);
	if (presenceIt != m_yieldsLocations.end()&&
		presenceIt->second<FMTList::size())
		{
		gotNull = false;
		}
	return gotNull;
	}

void FMTYields::update()
    {
	try {
		FMTList<std::unique_ptr<FMTYieldHandler>>::update();
		_updateYieldLocations();
		_updateHash();
	}catch (...)
		{
		_exhandler->raiseFromCatch("","FMTYields::update", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
    }

void FMTYields::reserve(const FMTYields& p_other)
	{
	FMTList<std::unique_ptr<FMTYieldHandler>>::reserve(p_other);
	m_yieldsLocations.reserve(p_other.m_yieldsLocations.size());
	}


FMTYields FMTYields::preSolve(const FMTMaskFilter& filter,
	const std::vector<FMTTheme>& originalthemes,
	const std::vector<FMTTheme>& newthemes) const
	{
	FMTYields newyields(*this);
	try {
		
		newyields._preSolveList(filter, originalthemes, newthemes);
		if (!filter.emptyFlipped())
		{
			for (auto& yieldObject : newyields)
			{
				yieldObject.second = yieldObject.second->preSolve(filter, newthemes);
				
			}
		}
		
		newyields.update();
	}catch (...)
		{
		_exhandler->raiseFromCatch("","FMTYields::preSolve", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
	return newyields;
	}

void FMTYields::preSolveRef(const FMTMaskFilter& p_filter,
	const std::vector<FMTTheme>& p_originalThemes,
	const std::vector<FMTTheme>& p_newThemes)
{
	try {
		_preSolveList(p_filter, p_originalThemes, p_newThemes);
		if (!p_filter.emptyFlipped())
		{
			for (auto& yieldobject : *this)
			{
				yieldobject.second = yieldobject.second->preSolve(p_filter, p_newThemes);
			}
		}
		update();
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTYields::preSolveRef", __LINE__, __FILE__, Core::FMTsection::Yield);
	}
}


FMTYields FMTYields::getFromFactor(const double& factor,
	std::vector<std::string>yieldnames) const
	{
	FMTYields newyields(*this);
	try {
		for (auto& handlerobj : newyields)
		{
			bool gotyield = false;
			for (const std::string& yield : yieldnames)
			{
				if (handlerobj.second->containsYield(yield))
					{
					gotyield = true;
					break;
					}
			}
			if (gotyield)
				{
				handlerobj.second = handlerobj.second->getFromFactor(factor, yieldnames);
				}
		}
	}catch (...)
		{
		_exhandler->printExceptions("for factor "+std::to_string(factor),
			"FMTYields::getFromFactor", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
	return newyields;
	}

double FMTYields::get(const FMTYieldRequest& request, const std::string& yld) const
{
	try {
		bool gotYield = false;
		request._updateData(*this);
		const const_iterator FIRST_IT = _getFirstSeen(yld);
		if (FIRST_IT == end())
		{
			gotYield = true;
		}else {
			for (const const_iterator IT : request.getDatas())
			{
				if (IT >= FIRST_IT &&
					(IT->second)->containsYield(yld))
				{
					return (IT->second)->get(yld, request);
					gotYield = true;
					break;
				}
			}
		}
		if (!gotYield)
		{
			_exhandler->raise(Exception::FMTexc::FMTmissingyield,
				yld + " for developement " + std::string(request.getDevelopment()),
				"FMTYields::get", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("for development type " + std::string(request.getDevelopment()), "FMTYields::get", __LINE__, __FILE__);
	}
	return 0;
}


bool FMTYields::operator == (const FMTYields& rhs) const
	{
	if (m_yieldsLocations == rhs.m_yieldsLocations &&
		size() == rhs.size())
		{
		auto& firstId = begin();
		auto& secondId = rhs.begin();
		while (firstId!=end())
			{
			if (!(firstId->second.get()->operator==(*secondId->second.get()))||
				!(firstId->first==secondId->first))
				{
				return false;
				}
			++firstId;
			++secondId;
			}
		return true;
		}
	return false;
	}

bool FMTYields::operator != (const FMTYields& rhs) const
	{
	return !(*this == rhs);
	}

std::vector<const FMTYieldHandler*> FMTYields::_getHandlerOfType(FMTyldtype type) const
	{
	std::vector<const FMTYieldHandler*>selectedhandlers;
	try{
	for (const auto& handlerobj : *this)
		{
		if (handlerobj.second->getType() == type)
			{
			selectedhandlers.push_back(handlerobj.second.get());
			}
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTYields::_getHandlerOfType", __LINE__, __FILE__, FMTsection::Yield);
	}
	return selectedhandlers;
	}

void FMTYields::_setModel(Models::FMTModel* p_modelPtr)
{
	const std::vector<FMTYieldHandler*> HANDLERS = _getHandlers(FMTyldtype::FMTmodelyld);
	for (FMTYieldHandler* handler : HANDLERS)
		{
		FMTModelYieldHandler* ModelHandler = dynamic_cast<FMTModelYieldHandler*>(handler);
		ModelHandler->setModel(p_modelPtr);
		}
}

FMTYields::const_iterator  FMTYields::_getFirstSeen(const std::string& p_yield) const
	{
	FMTYields::const_iterator Iterator = end();
	std::unordered_map<std::string, size_t>::const_iterator it = m_yieldsLocations.find(p_yield);
	if (it!= m_yieldsLocations.end())
		{
		Iterator = begin() + it->second;
		}
	return Iterator;
	}

size_t FMTYields::_getYieldsHash() const
	{
	return m_yieldsHash;
	}


std::vector<FMTYieldHandler*> FMTYields::_getHandlers(FMTyldtype type)
	{
	std::vector<FMTYieldHandler*>selectedhandlers;
	try {
		for (auto& handlerobj : *this)
		{
			if (handlerobj.second->getType() == type)
			{
				selectedhandlers.push_back(handlerobj.second.get());
			}
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTYields::_getHandlers", __LINE__, __FILE__, FMTsection::Yield);
	}
	return selectedhandlers;
	}

bool FMTYields::gotYieldType(FMTyldtype type) const
{
	try {
		return !_getHandlerOfType(type).empty();
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTYields::gotYieldType", __LINE__, __FILE__, FMTsection::Yield);
	}
	return false;
}

void FMTYields::setActionsMappingToModelHandlers(const std::vector<int>& actionids)
{
	
	try {
		for (FMTYieldHandler* handler : _getHandlers(FMTyldtype::FMTmodelyld))
		{
			handler->setBase(actionids);
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTYields::setActionsMappingToModelHandlers", __LINE__, __FILE__, FMTsection::Yield);
	}
}


int FMTYields::getMaxBase(const std::vector<const FMTYieldHandler*>& handlers)
	{
	int maxbase = 0;
	try{
	for (const FMTYieldHandler* handler : handlers)
		{
		const int lastbase = handler->getLastBase();
		if (lastbase > maxbase)
			{
			maxbase = lastbase;
			}
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTYields::getMaxBase", __LINE__, __FILE__, FMTsection::Yield);
	}
	return maxbase;
	}

void FMTYields::clear()
	{
	FMTList< std::unique_ptr<FMTYieldHandler>>::clear();
	m_yieldsLocations.clear();
	}

#include "FMTLogger.h"

std::map<std::string, std::map<std::string, std::vector<double>>>FMTYields::getAllYields(const FMTTheme& target,FMTyldtype type) const
	{
	std::map<std::string, std::map<std::string, std::vector<double>>>result;
	try {
		const std::vector<const FMTYieldHandler*> handlers = _getHandlerOfType(type);
		const int maxbase = getMaxBase(handlers);
		for (const FMTYieldHandler* handler : handlers)
		{
			const std::map<std::string, std::vector<double>>localstuff = handler->getAllYieldsData(maxbase);
			std::string strtarget = handler->getMask().get(target);
			if (result.find(strtarget) != result.end())
			{
				//Validate the logic for overrided yield ? ... Maybe we should iter from end to begin ... 
				result.at(strtarget).insert(localstuff.begin(),localstuff.end());
			}else{
				result[strtarget] = localstuff;
			}
			
		}
		}catch (...)
		{
			_exhandler->raiseFromCatch("","FMTYields::getAllYields", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
	return result;
	}


int FMTYields::getAge(const FMTYieldRequest& request,const FMTSpec& spec) const
    {
	int age = request.getDevelopment().getAge();
	try {
		request._updateData(*this);
		if (!request.getDatas().empty())
		{
			for (const const_iterator data : request.getDatas())
			{
				for (const std::string& yldname : spec.yieldnames)
				{
					if ((data->second)->containsYield(yldname))
					{
						return (data->second)->getAge(request, spec);
					}
				}
				
				
			}
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("for developement " + std::string(request.getDevelopment()), "FMTyield::getAge", __LINE__, __FILE__, Core::FMTsection::Yield);
	}
	return age;
	}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTYields)
