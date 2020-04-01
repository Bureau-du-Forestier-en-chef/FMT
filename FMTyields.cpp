/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
	FMTlist<FMTyieldhandler>::update();
    names = getyldsnames();
	null_names = getnullyldsnames();
    }

FMTyields FMTyields::presolve(const FMTmask& basemask,
	const std::vector<FMTtheme>& originalthemes,
	const FMTmask& presolvedmask,
	const std::vector<FMTtheme>& newthemes) const
	{
	FMTyields newyields(*this);
	newyields.presolvelist(basemask, originalthemes, presolvedmask, newthemes);
	if (!presolvedmask.empty())
		{
		for (auto& yieldobject : newyields)
			{
			yieldobject.second = yieldobject.second.presolve(presolvedmask, newthemes);
			}
		}
	newyields.update();
	return newyields;
	}

std::map<std::string, double>FMTyields::get(const FMTdevelopment& dev,
	const std::vector<std::string>& targets) const
	{
	std::map<std::string, double>values;
	const std::vector<const FMTyieldhandler*>datas = this->findsets(dev.mask);
	if (!datas.empty() && !targets.empty())
	{
		for (const std::string& name : targets)
		{
			for (const FMTyieldhandler* data : datas)
			{
					if (data->elements.find(name) != data->elements.end() && values.find(name) == values.end())
					{
						values[name] = data->get(datas, name, dev.age, dev.period,dev.mask);
					}
			}

		}
	}
	return values;
	}

std::map<std::string,double>FMTyields::getylds(const FMTdevelopment& dev,const FMTspec& spec) const
    {
    const std::vector<std::string> lnames = spec.getylds();
	return get(dev,lnames);
    }

bool FMTyields::operator == (const FMTyields& rhs) const
	{
	return (names == rhs.names && null_names == rhs.null_names &&
		FMTlist<FMTyieldhandler>::operator==(rhs));

	}

std::vector<const FMTyieldhandler*> FMTyields::gethandleroftype(FMTyldwstype type) const
	{
	//std::vector<FMTyieldhandler>::const_iterator handlerit = this->databegin();
	//std::vector<FMTyieldhandler>::const_iterator handlerend = this->dataend();
	std::vector<const FMTyieldhandler*>selectedhandlers;
	for (const auto& handlerobj : *this)
		{
		if (handlerobj.second.gettype() == type)
			{
			selectedhandlers.push_back(&(handlerobj.second));
			}
		}
	/*while (handlerit!= handlerend)
		{
		if (handlerit->gettype()==type)
			{
			selectedhandlers.push_back(&(*handlerit));
			}

		++handlerit;
		}*/
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

std::map<std::string, std::map<std::string, std::vector<double>>>FMTyields::getallyields(const FMTtheme& target,FMTyldwstype type) const
	{
	std::map<std::string, std::map<std::string, std::vector<double>>>result;
	const std::vector<const FMTyieldhandler*> handlers = gethandleroftype(type);
	const int maxbase = getmaxbase(handlers);
	for (const FMTyieldhandler* handler : handlers)
		{
		std::map<std::string, std::vector<double>>localstuff;
		if (type == FMTyldwstype::FMTageyld)
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
		result[handler->getmask().get(target)] = localstuff;
		}
	return result;
	}

int FMTyields::getage(const FMTdevelopment& dev,const FMTspec& spec) const
    {
    const std::vector<const FMTyieldhandler*>datas = this->findsets(dev.mask);
    int age = dev.age;
    if (!datas.empty())
        {
            for(const FMTyieldhandler* data : datas)
                {
                if (data->gettype() == FMTyldwstype::FMTageyld)
                    {
                    for (std::map<std::string,FMTyldbounds>::const_iterator it = spec.ylds.begin(); it != spec.ylds.end();++it)
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
						}

					 }
				}
		}
	return age;
	}

}
