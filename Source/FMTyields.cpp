/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTyields.h"

namespace Core{

FMTyields::FMTyields():FMTlist<FMTyieldhandler>(), yieldpresence()
        {
		updateyieldpresence();
        }
    FMTyields::FMTyields(const FMTyields& rhs): FMTlist<FMTyieldhandler>(rhs),yieldpresence(rhs.yieldpresence)
        {

        }
    FMTyields& FMTyields::operator = (const FMTyields& rhs)
        {
        if(this!=&rhs)
            {
			FMTlist<FMTyieldhandler>::operator = (rhs);
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
				value += std::string(handlerobj.second) + "\n";
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
			for (std::map<std::string, FMTdata>::const_iterator itd = handlerobj.second.elements.begin(); itd != handlerobj.second.elements.end(); ++itd)
			{
				if (yieldpresence.find(itd->first) != yieldpresence.end())
				{
					if (!itd->second.nulldata())
					{
						yieldpresence[itd->first] = true;
					}
				}
				else {
					yieldpresence[itd->first] = !itd->second.nulldata();
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
			for (std::map<std::string, FMTdata>::const_iterator itd = handlerobj.second.elements.begin(); itd != handlerobj.second.elements.end(); ++itd)
				{
				if (std::find(alls.begin(),alls.end(), itd->first)== alls.end())
					{
					alls.push_back(itd->first);
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
		FMTlist<FMTyieldhandler>::clearcache();
		for (auto& yh:*this)
		{
			for (auto& el:yh.second.elements)
			{
				//Clearcache of FMTdata in FMTyieldhandler
				el.second.clearcache();
			}
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
			for (std::map<std::string, FMTdata>::const_iterator itd = handlerobj.second.elements.begin(); itd != handlerobj.second.elements.end(); ++itd)
			{
				if (itd->first==value)
					{
					return true;
					}
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
		FMTlist<FMTyieldhandler>::update();
		updateyieldpresence();
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
		_exhandler->raisefromcatch("for development type " + std::string(dev), "FMTyields::get", __LINE__, __FILE__, _section);
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
		_exhandler->raisefromcatch("for development " + std::string(dev), "FMTyields::getylds", __LINE__, __FILE__, _section);
	}
	return std::vector<double>();
}


bool FMTyields::operator == (const FMTyields& rhs) const
	{
	return (yieldpresence == rhs.yieldpresence &&
		FMTlist<FMTyieldhandler>::operator==(rhs));

	}

std::vector<const FMTyieldhandler*> FMTyields::gethandleroftype(FMTyldtype type) const
	{
	std::vector<const FMTyieldhandler*>selectedhandlers;
	try{
	for (const auto& handlerobj : *this)
		{
		if (handlerobj.second.gettype() == type)
			{
			selectedhandlers.push_back(&(handlerobj.second));
			}
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTyields::gethandleroftype", __LINE__, __FILE__, FMTsection::Yield);
	}
	return selectedhandlers;
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
