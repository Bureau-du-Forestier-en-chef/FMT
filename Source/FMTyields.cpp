/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTyields.hpp"
#include "FMTyieldrequest.hpp"

namespace Core{




FMTyields::FMTyields():FMTlist<std::unique_ptr<FMTyieldhandler>>(), yieldpresence()
        {
		updateyieldpresence();
        }
    FMTyields::FMTyields(const FMTyields& rhs): FMTlist<std::unique_ptr<FMTyieldhandler>>(rhs),yieldpresence(rhs.yieldpresence)
        {

        }

	void FMTyields::swap(FMTyields& rhs)
		{
		FMTlist<std::unique_ptr<FMTyieldhandler>>::swap(rhs);
		yieldpresence.swap(rhs.yieldpresence);
		}

    FMTyields& FMTyields::operator = (const FMTyields& rhs)
        {
        if(this!=&rhs)
            {
			FMTlist<std::unique_ptr<FMTyieldhandler>>::operator = (rhs);
			yieldpresence = rhs.yieldpresence;
            }
        return *this;
        }
	std::vector<std::string>FMTyields::getstacked() const
        {
		std::vector<std::string>values;
		try {
			for (const auto& handlerobj : *this)
			{
				std::string value = "";
				value += std::string(*handlerobj.second) + "\n";
				values.push_back(value);
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyields::getstacked", __LINE__, __FILE__,FMTsection::Yield);
		}
       
        return values;
        }


void FMTyields::updateyieldpresence()
    {
	try {
		yieldpresence.clear();
		for (const auto& handlerobj : *this)
		{
			for (const std::string& yldname : handlerobj.second->getyieldnames())
			{
				//const FMTdata& data = handlerobj.second->at(yldname);
				const bool nulldata = handlerobj.second->isnullyield(yldname);
				if (yieldpresence.find(yldname) != yieldpresence.end())
				{
					if (!nulldata)
					{
						yieldpresence[yldname] = true;
					}
				}
				else {
					yieldpresence[yldname] = !nulldata;
				}
			}
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
		std::unordered_map<std::string, bool>::const_iterator presenceit = yieldpresence.find(value);
		returnvalue = (presenceit != yieldpresence.end());
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
	std::unordered_map<std::string, bool>::const_iterator presenceit = yieldpresence.find(value);
	return (presenceit == yieldpresence.end() || (presenceit != yieldpresence.end() && !presenceit->second));
	}

void FMTyields::update()
    {
	try {
		FMTlist<std::unique_ptr<FMTyieldhandler>>::update();
		updateyieldpresence();
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTyields::update", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
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
			for (auto& yieldobject : newyields)
			{
				yieldobject.second = yieldobject.second->presolve(filter, newthemes);
			}
		}
		
		newyields.update();
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTyields::presolve", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
	return newyields;
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
		bool gotyield = false;
		request.updatedata(*this);
		for (const std::unique_ptr<FMTyieldhandler>* data : request.getdatas())
		{
			if ((*data)->containsyield(yld))
			{
				return (*data)->get(yld,request);
				gotyield = true;
				break;
			}
		}
		if (!gotyield)
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
	return (yieldpresence == rhs.yieldpresence &&
		FMTlist<std::unique_ptr<FMTyieldhandler>>::operator==(rhs));

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


int FMTyields::getmaxbase(const std::vector<const FMTyieldhandler*>& handlers) const
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

#include "FMTlogger.hpp"

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
		request.updatedata(*this);
		if (!request.getdatas().empty())
		{
			for (const std::unique_ptr<FMTyieldhandler>* data : request.getdatas())
			{
				for (const std::string& yldname : spec.yieldnames)
				{
					if ((*data)->containsyield(yldname))
					{
						return (*data)->getage(request, spec);
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
