/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTyields.h"
#include "FMTyieldrequest.h"
#include "FMTexceptionhandler.h"
#include "FMTageyieldhandler.h"
#include "FMTtimeyieldhandler.h"
#include "FMTcomplexyieldhandler.h"
#include "FMTmodelyieldhandler.h"
#include "FMTyieldmodelTSLA.h"
#include <boost/algorithm/string.hpp> 
#include <memory>

namespace Core{

	const std::string FMTyields::m_nullYield = "~FMTNULLYIELD";

	const std::string& FMTyields::getNullYield()
		{
		return FMTyields::m_nullYield;
		}

	void FMTyields::push_backagehandler(const FMTmask& mask, const FMTageyieldhandler& value)
	{
		try {
			std::unique_ptr<Core::FMTyieldhandler>handlerptr(new FMTageyieldhandler(value));
			push_back(mask, handlerptr);
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyields::push_back", __LINE__, __FILE__, FMTsection::Yield);
		}
	}

	void FMTyields::push_backtimehandler(const FMTmask& mask, const FMTtimeyieldhandler& value)
	{
		try {
			std::unique_ptr<Core::FMTyieldhandler>handlerptr(new FMTtimeyieldhandler(value));
			push_back(mask, handlerptr);
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyields::push_back", __LINE__, __FILE__, FMTsection::Yield);
		}
	}




FMTyields::FMTyields():FMTlist<std::unique_ptr<FMTyieldhandler>>(), m_yieldsLocations()
        {
		_updateYieldLocations();
        }
    FMTyields::FMTyields(const FMTyields& rhs): FMTlist<std::unique_ptr<FMTyieldhandler>>(rhs), m_yieldsLocations(rhs.m_yieldsLocations)
        {

        }

	void FMTyields::swap(FMTyields& rhs)
		{
		FMTlist<std::unique_ptr<FMTyieldhandler>>::swap(rhs);
		m_yieldsLocations.swap(rhs.m_yieldsLocations);
		}

    FMTyields& FMTyields::operator = (const FMTyields& rhs)
        {
        if(this!=&rhs)
            {
			FMTlist<std::unique_ptr<FMTyieldhandler>>::operator = (rhs);
			m_yieldsLocations = rhs.m_yieldsLocations;
            }
        return *this;
        }

	void FMTyields::generatedefaultyields(const std::vector<Core::FMTtheme>& themes)
	{
		try {
			std::string general_mask;
			for (const Core::FMTtheme& theme : themes)
				{
				general_mask += "? ";
				}
			general_mask.pop_back();
			const Core::FMTmask base_mask(general_mask, themes);
			FMTmodelyieldhandler newhandler(base_mask);
			const std::unique_ptr<Core::FMTyieldmodel>TSLA(new FMTyieldmodelTSLA());
			newhandler.push_backmodel(TSLA);
			newhandler.setyield(0, 0, TSLA->GetModelName());
			const std::unique_ptr<Core::FMTyieldhandler>TSLAhandler(new FMTmodelyieldhandler(newhandler));
			push_back(base_mask,TSLAhandler);
		}catch (...)
			{
				_exhandler->raisefromcatch("", "FMTyields::generatedefaultyields", __LINE__, __FILE__, FMTsection::Yield);
			}
	}


	std::vector<std::string>FMTyields::getstacked() const
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
				const size_t canbeoverride = handlerobj.second->getoverrideindex();
				std::map<size_t, std::string>::const_iterator overfind = overrided.find(canbeoverride);
				const bool inoverridesection = (overrided.find(canbeoverride) == overrided.end());
				if (inoverridesection&&
					canbeoverride>0)
					{
					overrided[canbeoverride] = value;
				}else {
					const size_t position = values.size()+1;
					for (const size_t& tab : handlerobj.second->gettabous())
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
			_exhandler->raisefromcatch("", "FMTyields::getstacked", __LINE__, __FILE__,FMTsection::Yield);
		}
       
        return values;
        }


void FMTyields::_updateYieldLocations()
    {
	try {
		m_yieldsLocations.clear();
		m_yieldsLocations[getNullYield()] = 0;
		size_t SIZE_OF = FMTlist::size();
		size_t Id = 0;
		for (const auto& handlerObj : *this)
		{
			for (const std::string& yldName : handlerObj.second->getyieldnames())
			{
				std::pair<std::unordered_map<std::string, size_t>::iterator,bool>inserted = m_yieldsLocations.insert(std::pair<std::string, size_t>(yldName, SIZE_OF));
				if (!handlerObj.second->isnullyield(yldName)&&
					inserted.first->second == SIZE_OF)
					{
					inserted.first->second = Id;
					}
			}
			++Id;
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTyields::updateyieldpresence", __LINE__, __FILE__, FMTsection::Yield);
	}

    }

std::vector<std::string> FMTyields::getallyieldnames() const
{
	std::vector<std::string>alls;
	try {
		alls.push_back(getNullYield());
		for (const auto& handlerobj : *this)
			{
			for (const std::string& yldname : handlerobj.second->getyieldnames())
				{
				if (std::find(alls.begin(),alls.end(), yldname)== alls.end())
					{
					alls.push_back(yldname);
					}
				}
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTyields::getallyieldnames", __LINE__, __FILE__, FMTsection::Yield);
		}
	return alls;
	}

void FMTyields::clearcache()
	{
		//Clearcache of FMTlist
		FMTlist<std::unique_ptr<FMTyieldhandler>>::clearcache();
		for (auto& yh:*this)
		{
			yh.second->clearcache();
		}
	}

bool FMTyields::isyld(const std::string& value, bool fromsource) const
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
			if (handlerobj.second->containsyield(value))
				{
				return true;
				}
		}
	}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTyields::isyld", __LINE__, __FILE__, FMTsection::Yield);
	}
	return returnvalue;
    }

bool FMTyields::isnullyld(const std::string& value) const
	{
	bool gotNull = true;
	std::unordered_map<std::string, size_t>::const_iterator presenceIt = m_yieldsLocations.find(value);
	if (presenceIt != m_yieldsLocations.end()&&
		presenceIt->second<FMTlist::size())
		{
		gotNull = false;
		}
	return gotNull;
	}

void FMTyields::update()
    {
	try {
		FMTlist<std::unique_ptr<FMTyieldhandler>>::update();
		_updateYieldLocations();
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTyields::update", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
    }

void FMTyields::reserve(const FMTyields& p_other)
	{
	FMTlist<std::unique_ptr<FMTyieldhandler>>::reserve(p_other);
	m_yieldsLocations.reserve(p_other.m_yieldsLocations.size());
	}


FMTyields FMTyields::presolve(const FMTmaskfilter& filter,
	const std::vector<FMTtheme>& originalthemes,
	const std::vector<FMTtheme>& newthemes) const
	{
	FMTyields newyields(*this);
	try {
		
		newyields.presolvelist(filter, originalthemes, newthemes);
		if (!filter.emptyflipped())
		{
			for (auto& yieldObject : newyields)
			{
				yieldObject.second = yieldObject.second->presolve(filter, newthemes);
				
			}
		}
		
		newyields.update();
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTyields::presolve", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
	return newyields;
	}

void FMTyields::presolveRef(const FMTmaskfilter& p_filter,
	const std::vector<FMTtheme>& p_originalThemes,
	const std::vector<FMTtheme>& p_newThemes)
{
	try {
		presolvelist(p_filter, p_originalThemes, p_newThemes);
		if (!p_filter.emptyflipped())
		{
			for (auto& yieldobject : *this)
			{
				yieldobject.second = yieldobject.second->presolve(p_filter, p_newThemes);
			}
		}
		update();
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTyields::presolveRef", __LINE__, __FILE__, Core::FMTsection::Yield);
	}
}


FMTyields FMTyields::getfromfactor(const double& factor,
	std::vector<std::string>yieldnames) const
	{
	FMTyields newyields(*this);
	try {
		for (auto& handlerobj : newyields)
		{
			bool gotyield = false;
			for (const std::string& yield : yieldnames)
			{
				if (handlerobj.second->containsyield(yield))
					{
					gotyield = true;
					break;
					}
			}
			if (gotyield)
				{
				handlerobj.second = handlerobj.second->getfromfactor(factor, yieldnames);
				}
		}
	}catch (...)
		{
		_exhandler->printexceptions("for factor "+std::to_string(factor),
			"FMTyields::getfromfactor", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
	return newyields;
	}

double FMTyields::get(const FMTyieldrequest& request, const std::string& yld) const
{
	try {
		bool gotYield = false;
		request._updateData(*this);
		const const_iterator FIRST_IT = _getFirstSeen(yld);
		if (FIRST_IT == end())
		{
			gotYield = true;
		}else {
			for (const const_iterator IT : request.getdatas())
			{
				if (IT >= FIRST_IT &&
					(IT->second)->containsyield(yld))
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
				yld + " for developement " + std::string(request.getdevelopment()),
				"FMTyields::get", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("for development type " + std::string(request.getdevelopment()), "FMTyields::get", __LINE__, __FILE__);
	}
	return 0;
}

/*double FMTyields::getsingle(const FMTdevelopment& dev,
	const std::string& target) const
{
	try {
		const Core::FMTmask filteredmask = this->filtermask(dev.getmask());
		const std::vector<const FMTyieldhandler*>datas = this->findsetswithfiltered(filteredmask);
		for (const FMTyieldhandler* data : datas)
		{
			if (data->elements.find(target) != data->elements.end())
			{
				return data->get(datas, target, dev.getage(), dev.getperiod(), filteredmask);
			}
		}

	_exhandler->raise(Exception::FMTexc::FMTmissingyield,
			target + " for development type " + std::string(dev),
					"FMTyields::get", __LINE__, __FILE__, Core::FMTsection::Yield);
	}catch (...)
		{
		_exhandler->raisefromcatch("for development type " + std::string(dev), "FMTyields::get", __LINE__, __FILE__);
		}
	return 0;
}*/

/*
std::vector<double>FMTyields::get(const std::vector<FMTyieldrequest>& requests) const
{
	std::vector<double>values(requests.size());
	try {
		//const Core::FMTmask filteredmask = this->filtermask(dev.getmask());
		//const std::vector<const FMTyieldhandler*>datas = this->findsetswithfiltered(filteredmask);
		size_t location = 0;
		for (const FMTyieldrequest& request : requests)
		{
			bool gotyield = false;
			for (const FMTyieldhandler* data : request.getdatas())
			{
				if (data->elements.find(request.getyld()) != data->elements.end())
				{
					values[location] = data->get(request);
					gotyield = true;
					break;
				}
			}
			if (!gotyield)
			{
				_exhandler->raise(Exception::FMTexc::FMTmissingyield,
					request.getyld() + " for yield request " + std::string(request),
					"FMTyields::get", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
			++location;
		}
			
	}
	catch (...)
	{
		_exhandler->raisefromcatch("for yield request " + std::string(*requests.begin()), "FMTyields::get", __LINE__, __FILE__);
	}
	return values;
}

std::vector<double>FMTyields::getylds(const FMTdevelopment& dev, const FMTspec& spec) const
{
	try {
		const std::vector<std::string>& lnames = spec.getylds();
		return get(dev, lnames);
	}
	catch (...)
	{
		_exhandler->raisefromcatch("for development " + std::string(dev), "FMTyields::getylds", __LINE__, __FILE__);
	}
	return std::vector<double>();
}*/


bool FMTyields::operator == (const FMTyields& rhs) const
	{
	return (m_yieldsLocations == rhs.m_yieldsLocations &&
		FMTlist<std::unique_ptr<FMTyieldhandler>>::operator==(rhs));

	}

bool FMTyields::operator != (const FMTyields& rhs) const
	{
	return !(*this == rhs);
	}

std::vector<const FMTyieldhandler*> FMTyields::gethandleroftype(FMTyldtype type) const
	{
	std::vector<const FMTyieldhandler*>selectedhandlers;
	try{
	for (const auto& handlerobj : *this)
		{
		if (handlerobj.second->gettype() == type)
			{
			selectedhandlers.push_back(handlerobj.second.get());
			}
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTyields::gethandleroftype", __LINE__, __FILE__, FMTsection::Yield);
	}
	return selectedhandlers;
	}

void FMTyields::setModel(Models::FMTmodel* p_modelPtr)
{
	const std::vector<FMTyieldhandler*> HANDLERS = gethandlers(FMTyldtype::FMTmodelyld);
	for (FMTyieldhandler* handler : HANDLERS)
		{
		FMTmodelyieldhandler* ModelHandler = dynamic_cast<FMTmodelyieldhandler*>(handler);
		ModelHandler->setModel(p_modelPtr);
		}
}

FMTyields::const_iterator  FMTyields::_getFirstSeen(const std::string& p_yield) const
	{
	FMTyields::const_iterator Iterator = end();
	std::unordered_map<std::string, size_t>::const_iterator it = m_yieldsLocations.find(p_yield);
	if (it!= m_yieldsLocations.end())
		{
		Iterator = begin() + it->second;
		}
	return Iterator;
	}


std::vector<FMTyieldhandler*> FMTyields::gethandlers(FMTyldtype type)
	{
	std::vector<FMTyieldhandler*>selectedhandlers;
	try {
		for (auto& handlerobj : *this)
		{
			if (handlerobj.second->gettype() == type)
			{
				selectedhandlers.push_back(handlerobj.second.get());
			}
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTyields::gethandlers", __LINE__, __FILE__, FMTsection::Yield);
	}
	return selectedhandlers;
	}

bool FMTyields::gotyieldtype(FMTyldtype type) const
{
	try {
		return !gethandleroftype(type).empty();
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTyields::gotyieldtype", __LINE__, __FILE__, FMTsection::Yield);
	}
	return false;
}

void FMTyields::setactionsmappingtomodelhandlers(const std::vector<int>& actionids)
{
	
	try {
		for (FMTyieldhandler* handler : gethandlers(FMTyldtype::FMTmodelyld))
		{
			handler->setbase(actionids);
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTyields::setactionsmappingtomodelhandlers", __LINE__, __FILE__, FMTsection::Yield);
	}
}


int FMTyields::getmaxbase(const std::vector<const FMTyieldhandler*>& handlers)
	{
	int maxbase = 0;
	try{
	for (const FMTyieldhandler* handler : handlers)
		{
		const int lastbase = handler->getlastbase();
		if (lastbase > maxbase)
			{
			maxbase = lastbase;
			}
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTyields::getmaxbase", __LINE__, __FILE__, FMTsection::Yield);
	}
	return maxbase;
	}

void FMTyields::clear()
	{
	FMTlist< std::unique_ptr<FMTyieldhandler>>::clear();
	m_yieldsLocations.clear();
	}

#include "FMTlogger.h"

std::map<std::string, std::map<std::string, std::vector<double>>>FMTyields::getallyields(const FMTtheme& target,FMTyldtype type) const
	{
	std::map<std::string, std::map<std::string, std::vector<double>>>result;
	try {
		const std::vector<const FMTyieldhandler*> handlers = gethandleroftype(type);
		const int maxbase = getmaxbase(handlers);
		for (const FMTyieldhandler* handler : handlers)
		{
			const std::map<std::string, std::vector<double>>localstuff = handler->getallyieldsdata(maxbase);
			std::string strtarget = handler->getmask().get(target);
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
			_exhandler->raisefromcatch("","FMTyields::getallyields", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
	return result;
	}

//Old function before converting complexyield
/*int FMTyields::getage(const FMTdevelopment& dev,const FMTspec& spec) const
    {
	int age = dev.age;
	try {
		const std::vector<const FMTyieldhandler*>datas = this->findsets(dev.mask);
		if (!datas.empty())
		{
			for (const FMTyieldhandler* data : datas)
			{
				if (data->gettype() == FMTyldtype::FMTageyld)
				{
					for (size_t id = 0; id < spec.yieldnames.size();++id)
					{
						if (data->elements.find(spec.yieldnames.at(id)) != data->elements.end())
						{
							const FMTyldbounds* bound = &spec.yieldbounds.at(id);
							const int new_age = data->getage(spec.yieldnames.at(id), bound->getlower(), dev.age);
							if (new_age < age)
							{
								age = new_age;
							}
							*_logger<<"Return age "+std::to_string(age)+" for yield "+std::string(*data) <<"\n";
							return age;
						}
					}
				}else if(data->gettype() == FMTyldtype::FMTcomplexyld){
					*_logger<<"Not an age yield "+std::string(*data)<<"\n";
				}
			}
		}
	}catch (...)
		{
		_exhandler->raisefromcatch("for developement "+std::string(dev),"FMTyields::getage", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
	return age;
	}
	
FMTyieldhandler FMTyields::complexyldtoageyld(const FMTyieldhandler* complexyld, const FMTyieldrequest& request,const FMTspec& lspec) const
	{
	const FMTmask cplxmask = complexyld->getmask();
	FMTyieldhandler nhandler(FMTyldtype::FMTageyld, cplxmask);
	try {
		const std::vector<std::string>& yldnames = lspec.getylds();
		for (size_t id = 0; id < yldnames.size(); ++id)
		{
			if (complexyld->containsyield(yldnames.at(id)))
			{
				Core::FMTdevelopment newdev(request.getdevelopment());
				for (int age = 0; age <= request.getdevelopment().getage(); ++age)
				{
					newdev.setage(age);
					const FMTyieldrequest newrequest(newdev);
					nhandler.push_base(age);
					nhandler.push_data(yldnames.at(id), complexyld->get(yldnames.at(id), newrequest));
				}
			}
		}
	}
	catch (...) {
		_exhandler->raisefromcatch("Error in converting complexyield to ageyield for yieldhandler " + std::string(*complexyld), "FMTyieldrequest::complexyldtoageyld", __LINE__, __FILE__);
	}
	return nhandler;
	}*/

int FMTyields::getage(const FMTyieldrequest& request,const FMTspec& spec) const
    {
	int age = request.getdevelopment().getage();
	try {
		request._updateData(*this);
		if (!request.getdatas().empty())
		{
			for (const const_iterator data : request.getdatas())
			{
				for (const std::string& yldname : spec.yieldnames)
				{
					if ((data->second)->containsyield(yldname))
					{
						return (data->second)->getage(request, spec);
					}
				}
				
				
			}
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("for developement " + std::string(request.getdevelopment()), "FMTyield::getage", __LINE__, __FILE__, Core::FMTsection::Yield);
	}
	return age;
	}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTyields)
