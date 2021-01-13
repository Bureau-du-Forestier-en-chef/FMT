/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTyields.h"

namespace Core{

FMTyields::FMTyields():FMTlist<FMTyieldhandler>(),names(), null_names()
        {
        names = getyldsnames();
        }
    FMTyields::FMTyields(const FMTyields& rhs): FMTlist<FMTyieldhandler>(rhs),names(rhs.names), null_names(rhs.null_names)
        {

        }
    FMTyields& FMTyields::operator = (const FMTyields& rhs)
        {
        if(this!=&rhs)
            {
			FMTlist<FMTyieldhandler>::operator = (rhs);
            names = rhs.names;
			null_names = rhs.null_names;
            }
        return *this;
        }
	std::vector<std::string>FMTyields::getstacked() const
        {
		std::vector<std::string>values;
		//std::vector<FMTyieldhandler>::const_iterator data_iterator = this->databegin();
        for(const auto& handlerobj : *this/*size_t id = 0 ; id < this->size(); ++id*/)
            {
			std::string value = "";
            value += std::string(handlerobj.second) + "\n";
            values.push_back(value);
			//++data_iterator;
            }
        return values;
        }


std::vector<std::string>FMTyields::getyldsnames() const
    {
	std::vector<std::string>fullylds;
    for(const auto& handlerobj : *this)
        {
        for(std::map<std::string,FMTdata>::const_iterator itd = handlerobj.second.elements.begin(); itd != handlerobj.second.elements.end(); ++itd)
             {
             if (std::find(fullylds.begin(),fullylds.end(), itd->first)==fullylds.end())
                 {
                 fullylds.push_back(itd->first);
                 }
             }
        }
    return fullylds;
    }

std::vector<std::string>FMTyields::getnullyldsnames() const
	{
	std::vector<std::string>nullyields;
	std::vector<std::string>nonnullyields;
	for (const auto& handlerobj : *this)
		{
		for (std::map<std::string, FMTdata>::const_iterator itd = handlerobj.second.elements.begin(); itd != handlerobj.second.elements.end(); ++itd)
			{
			std::vector<std::string>::iterator nullit = std::find(nullyields.begin(), nullyields.end(), itd->first);
			std::vector<std::string>::iterator nonnullit = std::find(nonnullyields.begin(), nonnullyields.end(), itd->first);
			if (itd->second.nulldata() && nonnullit == nonnullyields.end() && nullit == nullyields.end())
				{
				nullyields.push_back(itd->first);
			}else if (nullit!= nullyields.end())
				{
				nullyields.erase(nullit);
				nonnullyields.push_back(itd->first);
			}else {
				nonnullyields.push_back(itd->first);
				}
			}
		}
	return nullyields;
	}

bool FMTyields::isyld(const std::string& value) const
    {
    return (std::find(names.begin(),names.end(), value)!=names.end());
    }

bool FMTyields::isnullyld(const std::string& value) const
	{
	return (std::find(null_names.begin(), null_names.end(), value) != null_names.end());
	}

void FMTyields::update()
    {
	try {
		FMTlist<FMTyieldhandler>::update();
		names = getyldsnames();
		null_names = getnullyldsnames();
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTyields::update", __LINE__, __FILE__, _section);
		}
    }

void FMTyields::passinobject(const FMTobject& rhs)
	{
	try {
		FMTlist<FMTyieldhandler>::passinobject(rhs);
		for (auto&  handler : *this)
			{
			handler.second.passinobject(rhs);
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTyields::passinobject", __LINE__, __FILE__, _section);
		}

	}

FMTyields FMTyields::presolve(const FMTmask& basemask,
	const std::vector<FMTtheme>& originalthemes,
	const FMTmask& presolvedmask,
	const std::vector<FMTtheme>& newthemes) const
	{
	FMTyields newyields(*this);
	try {
		newyields.presolvelist(basemask, originalthemes, presolvedmask, newthemes);
		if (!presolvedmask.empty())
		{
			for (auto& yieldobject : newyields)
			{
				yieldobject.second = yieldobject.second.presolve(presolvedmask, newthemes);
			}
		}
		newyields.update();
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTyields::presolve", __LINE__, __FILE__, _section);
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
			handlerobj.second = handlerobj.second.getfromfactor(factor, yieldnames);
		}
	}catch (...)
		{
		_exhandler->raisefromcatch("for factor "+std::to_string(factor),
			"FMTyields::getfromfactor", __LINE__, __FILE__, _section);
		}
	return newyields;
	}

std::vector<double>FMTyields::get(const FMTdevelopment& dev,
	const std::vector<std::string>& targets) const
{
	std::vector<double>values;
	try {
		const std::vector<const FMTyieldhandler*>datas = this->findsets(dev.mask);
		values.reserve(targets.size());
		for (const std::string& name : targets)
		{
			bool gotyield = false;
			for (const FMTyieldhandler* data : datas)
			{
				if (data->elements.find(name) != data->elements.end())
				{
					values.push_back(data->get(datas, name, dev.age, dev.period, dev.mask));
					gotyield = true;
					break;
				}
			}
			if (!gotyield)
			{
				_exhandler->raise(Exception::FMTexc::FMTmissingyield,
					name + " for development type " + std::string(dev),
					"FMTyields::get", __LINE__, __FILE__);
			}
		}
			
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTyields::get", __LINE__, __FILE__, _section);
	}
	return values;
}

/*
std::map<std::string, double>FMTyields::get(const FMTdevelopment& dev,
	const std::vector<std::string>& targets) const
	{
	std::map<std::string, double>values;
	size_t actualyeildid = 0;
	try {
		const std::vector<const FMTyieldhandler*>datas = this->findsets(dev.mask);
		if (!datas.empty())
		{
			for (const std::string& name : targets)
			{
				for (const FMTyieldhandler* data : datas)
				{
					if (data->elements.find(name) != data->elements.end() && values.find(name) == values.end())
					{
						values[name] = data->get(datas, name, dev.age, dev.period, dev.mask);
					}
				}
			++actualyeildid;
			}
		}
		if (targets.size()!= values.size())
			{
			for (const std::string& yield : targets)
				{
				if (values.find(yield) == values.end())
					{
					_exhandler->raise(Exception::FMTexc::FMTmissingyield,
						yield + " for development type " + std::string(dev),
						"FMTyields::get", __LINE__, __FILE__);
					}
				}
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("for development "+std::string(dev)+" for yield("+ targets.at(actualyeildid) +")","FMTyields::get", __LINE__, __FILE__, _section);
		}
	return values;
	}*/

std::vector<double>FMTyields::getylds(const FMTdevelopment& dev, const FMTspec& spec) const
{
	try {
		const std::vector<std::string>& lnames = spec.getylds();
		return get(dev, lnames);
	}
	catch (...)
	{
		_exhandler->raisefromcatch("for development " + std::string(dev), "FMTyields::getylds", __LINE__, __FILE__, _section);
	}
	return std::vector<double>();
}

/*std::map<std::string,double>FMTyields::getylds(const FMTdevelopment& dev,const FMTspec& spec) const
    {
	try {
		const std::vector<std::string> lnames = spec.getylds();
		return get(dev, lnames);
	}catch (...)
		{
		_exhandler->raisefromcatch("for development " + std::string(dev),"FMTyields::getylds", __LINE__, __FILE__, _section);
		}
	return std::map<std::string, double>();
    }*/

bool FMTyields::operator == (const FMTyields& rhs) const
	{
	return (names == rhs.names && null_names == rhs.null_names &&
		FMTlist<FMTyieldhandler>::operator==(rhs));

	}

std::vector<const FMTyieldhandler*> FMTyields::gethandleroftype(FMTyldtype type) const
	{
	std::vector<const FMTyieldhandler*>selectedhandlers;
	for (const auto& handlerobj : *this)
		{
		if (handlerobj.second.gettype() == type)
			{
			selectedhandlers.push_back(&(handlerobj.second));
			}
		}
	return selectedhandlers;
	}

int FMTyields::getmaxbase(const std::vector<const FMTyieldhandler*>& handlers) const
	{
	int maxbase = 0;
	for (const FMTyieldhandler* handler : handlers)
		{
		const int lastbase = handler->getlastbase();
		if (lastbase > maxbase)
			{
			maxbase = lastbase;
			}
		}
	return maxbase;
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
			std::map<std::string, std::vector<double>>localstuff;
			if (type == FMTyldtype::FMTageyld)
			{
				const int lastbase = handler->getlastbase();
				std::vector<int>bases = handler->getbases();
				for (std::map<std::string, FMTdata>::const_iterator cit = handler->elements.begin(); cit != handler->elements.end(); cit++)
				{
					localstuff[cit->first] = std::vector<double>();
					for (int base = 0; base <= maxbase; ++base)
					{
						std::vector<int>::const_iterator baseit = std::find(bases.begin(), bases.end(), base);
						if (baseit != bases.end())
						{
							size_t index = std::distance<std::vector<int>::const_iterator>(bases.begin(), baseit);
							localstuff[cit->first].push_back(cit->second.data.at(index));
						}
						else if (base < lastbase)
						{
							localstuff[cit->first].push_back(0);
						}
						else {
							localstuff[cit->first].push_back(cit->second.data.back());
						}
					}
				}
			}
			else if (type == FMTyldtype::FMTtimeyld)
			{
				for (std::map<std::string, FMTdata>::const_iterator cit = handler->elements.begin(); cit != handler->elements.end(); cit++)
				{
					localstuff[cit->first] = cit->second.data;
				}
			}

			result[handler->getmask().get(target)] = localstuff;
		}
		}catch (...)
		{
			_exhandler->raisefromcatch("","FMTyields::getallyields", __LINE__, __FILE__, _section);
		}
	return result;
	}

int FMTyields::getage(const FMTdevelopment& dev,const FMTspec& spec) const
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
							return age;
						}
					}
					/*for (std::map<std::string, FMTyldbounds>::const_iterator it = spec.ylds.begin(); it != spec.ylds.end(); ++it)
					{
						if (data->elements.find(it->first) != data->elements.end())
						{
							const FMTyldbounds* bound = &it->second;
							const int new_age = data->getage(it->first, bound->getlower(), dev.age);
							if (new_age < age)
							{
								age = new_age;
							}
							return age;
						}
					}*/

				}
			}
		}
	}catch (...)
		{
		_exhandler->raisefromcatch("for developement "+std::string(dev),"FMTyields::getage", __LINE__, __FILE__, _section);
		}
	return age;
	}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTyields)