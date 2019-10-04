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
    vector<string>FMTyields::getstacked() const
        {
        vector<string>values;
		vector<FMTyieldhandler>::const_iterator data_iterator = this->databegin();
		//vector<FMTmask>::const_iterator mask_iterator = this->maskbegin();
        for(size_t id = 0 ; id < this->size(); ++id)
            {
            string value = "";
            value += string(*data_iterator) + "\n";
            values.push_back(value);
			++data_iterator;
            }
        return values;
        }

vector<string>FMTyields::getyldsnames() const
    {
    vector<string>fullylds;
	vector<FMTyieldhandler>::const_iterator data_iterator = this->databegin();
    for(size_t id = 0 ; id < this->size(); ++id)
        {
        for(map<string,FMTdata>::const_iterator itd = data_iterator->elements.begin(); itd != data_iterator->elements.end(); ++itd)
             {
             if (std::find(fullylds.begin(),fullylds.end(), itd->first)==fullylds.end())
                 {
                 fullylds.push_back(itd->first);
                 }
             }
		++data_iterator;
        }
    return fullylds;
    }

vector<string>FMTyields::getnullyldsnames() const
	{
	vector<string>nullyields;
	vector<string>nonnullyields;
	vector<FMTyieldhandler>::const_iterator data_iterator = this->databegin();
	for (size_t id = 0; id < this->size(); ++id)
		{
		for (map<string, FMTdata>::const_iterator itd = data_iterator->elements.begin(); itd != data_iterator->elements.end(); ++itd)
			{
			vector<string>::iterator nullit = find(nullyields.begin(), nullyields.end(), itd->first);
			vector<string>::iterator nonnullit = find(nonnullyields.begin(), nonnullyields.end(), itd->first);
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
		++data_iterator;
		}
	return nullyields;
	}

bool FMTyields::isyld(const string& value) const
    {
    return (std::find(names.begin(),names.end(), value)!=names.end());
    }

bool FMTyields::isnullyld(const string& value) const
	{
	return (std::find(null_names.begin(), null_names.end(), value) != null_names.end());
	}

void FMTyields::update()
    {
	this->shrink();
    names = getyldsnames();
	null_names = getnullyldsnames();
    }

map<string, double>FMTyields::get(const FMTdevelopment& dev,
	const vector<string>& targets) const
	{
	map<string, double>values;
	const vector<const FMTyieldhandler*>datas = this->findsets(dev.mask);
	//const FMTmask resumemask = this->filtermask(dev.mask);
	if (!datas.empty() && !targets.empty())
	{
		for (const string& name : targets)
		{
			for (const FMTyieldhandler* data : datas)
			{
					if (data->elements.find(name) != data->elements.end() && values.find(name) == values.end())
					{
						values[name] = data->get(datas, name, dev.age, dev.period,dev.mask); //recursive!!!
					}
			}

		}
	}
	return values;
	}

map<string,double>FMTyields::getylds(const FMTdevelopment& dev,const FMTspec& spec) const
    {
    //const vector<const FMTyieldhandler*>datas = this->findsets(dev.mask);
    const vector<string> lnames = spec.getylds();
	return get(dev,lnames);
    }

bool FMTyields::operator == (const FMTyields& rhs) const
	{
	return (names == rhs.names && null_names == rhs.null_names &&
		FMTlist<FMTyieldhandler>::operator==(rhs));

	}

vector<const FMTyieldhandler*> FMTyields::gethandleroftype(FMTyldwstype type) const
	{
	vector<FMTyieldhandler>::const_iterator handlerit = this->databegin();
	vector<FMTyieldhandler>::const_iterator handlerend = this->dataend();
	vector<const FMTyieldhandler*>selectedhandlers;
	while (handlerit!= handlerend)
		{
		if (handlerit->gettype()==type)
			{
			selectedhandlers.push_back(&(*handlerit));
			}

		++handlerit;
		}
	return selectedhandlers;
	}

int FMTyields::getmaxbase(const vector<const FMTyieldhandler*>& handlers) const
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

map<string, map<string, vector<double>>>FMTyields::getallyields(const FMTtheme& target,FMTyldwstype type) const
	{
	map<string, map<string, vector<double>>>result;
	vector<const FMTyieldhandler*> handlers = gethandleroftype(type);
	int maxbase = getmaxbase(handlers);
	for (const FMTyieldhandler* handler : handlers)
		{
		map<string, vector<double>>localstuff;
		if (type == FMTyldwstype::FMTageyld)
			{
			int lastbase = handler->getlastbase();
			vector<int>bases = handler->getbases();
			for (map<string, FMTdata>::const_iterator cit = handler->elements.begin(); cit != handler->elements.end(); cit++)
				{
				localstuff[cit->first] = vector<double>();
				for (int base = 0; base <= maxbase; ++base)
				{
					vector<int>::const_iterator baseit = std::find(bases.begin(), bases.end(), base);
					if (baseit != bases.end())
					{
						size_t index = std::distance<vector<int>::const_iterator>(bases.begin(), baseit);
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
    const vector<const FMTyieldhandler*>datas = this->findsets(dev.mask);
    //const vector<string> lnames = spec.getylds();
    int age = dev.age;
    if (!datas.empty())
        {
            for(const FMTyieldhandler* data : datas)
                {
                if (data->gettype() == FMTyldwstype::FMTageyld)
                    {
                    for (map<string,FMTyldbounds>::const_iterator it = spec.ylds.begin(); it != spec.ylds.end();++it)
                        {
						if (data->elements.find(it->first) != data->elements.end())
							{
							const FMTyldbounds* bound = &it->second;
							int new_age = data->getage(it->first, bound->getlower(), dev.age);
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
