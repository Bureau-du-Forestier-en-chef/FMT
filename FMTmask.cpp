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


#include "FMTmask.h"
#include "FMTtheme.h"

namespace Core{

FMTmask::FMTmask(): name(),data()
    {

    }

FMTmask::operator bool() const
	{
	return (!data.empty());
	}

FMTmask::FMTmask(const std::string& mask,const boost::dynamic_bitset<>& bits):name(mask),data(bits)
	{

	}
FMTmask::FMTmask(const boost::dynamic_bitset<>& bits): name(),data(bits)
	{

	}


FMTmask::FMTmask(const std::string& mask,const std::vector<FMTtheme>& themes):name(),data()
    {
	this->set(themes, mask);
    }
FMTmask::FMTmask(const std::vector<std::string>&values,const std::vector<FMTtheme>& themes):name(),data()
    {
    for(const std::string& el : values)
        {
        name+=el+" ";
        }
    name = name.substr(0, name.size()-1);
	this->set(themes, name);
    }
std::vector<FMTmask> FMTmask::decompose(const FMTtheme &theme) const
    {
	std::vector<std::string>bases;
    const std::string value = get(theme);
	const std::vector<std::string>atts = theme.getattributes(value);
	std::vector<FMTmask>newmasks;
    for(const std::string& att : atts)
        {
        FMTmask newmask = *this;
        newmask.set(theme,att);
        newmasks.push_back(newmask);
        }
    return newmasks;
    }
boost::dynamic_bitset<> FMTmask::subset(const FMTtheme& theme) const
    {
    boost::dynamic_bitset<> sub(theme.size());
    int locit = 0;
    for(int id = theme.start; id < (theme.start + static_cast<int>(theme.size())); ++id)
        {
        if (data[id])
            {
            sub[locit] = true;
            }else{
            sub[locit] = false;
            }
        ++locit;
        }
    return sub;
    }
void FMTmask::setsubset(const FMTtheme& theme,const boost::dynamic_bitset<>& subset)
    {
    int locit = 0;
    for(int id = theme.start; id < (theme.start+static_cast<int>(theme.size())); ++id)
        {
        if (subset[locit])
            {
            data[id] = true;
            }else{
            data[id] = false;
            }
        ++locit;
        }
    }

std::string FMTmask::get(const std::vector<FMTtheme>& themes) const
    {
	std::string value = "";
    for(const FMTtheme& theme : themes)
        {
        const boost::dynamic_bitset<>bits = this->subset(theme);
        value += theme.bitstostr(bits) + " ";
        }
    value = value.substr(0, value.size()-1);
    return value;
    }
void FMTmask::set(const std::vector<FMTtheme>& themes,const std::string& value)
    {
    name = value;
	std::vector<std::string>bases;
    boost::split(bases,value,boost::is_any_of(FMT_STR_SEPARATOR),boost::token_compress_on);
    size_t fullsize = 0;
    for(const FMTtheme& theme : themes)
        {
        fullsize+=theme.size();
        }
    data.resize(fullsize,false);
    for(const FMTtheme& theme : themes)
        {
        const boost::dynamic_bitset<>bits = theme.strtobits(bases[theme.id]);
		this->setsubset(theme, bits);
        }
    }
std::string FMTmask::get(const FMTtheme& theme) const
    {
    const boost::dynamic_bitset<>bits =  this->subset(theme);
    return theme.bitstostr(bits);
    }

bool FMTmask::empty() const
	{
	return data.empty();
	}

std::vector<FMTtheme>FMTmask::getstaticthemes(const std::vector<FMTtheme>& themes) const
	{
	std::vector<FMTtheme>staticths;
	for (const FMTtheme& theme : staticths)
		{
		const std::string value = this->get(theme);
		if (!theme.isaggregate(value) && !theme.isattribute(value))
			{
			staticths.push_back(theme);
			}
		}
	return staticths;
	}


void FMTmask::set(const FMTtheme& theme,const std::string& value)
    {
    const boost::dynamic_bitset<>sub=theme.strtobits(value);
    std::vector<std::string>bases;
    boost::split(bases,name,boost::is_any_of(FMT_STR_SEPARATOR), boost::token_compress_on);
    bases[theme.id] = value;
    name = boost::algorithm::join(bases," ");
    this->setsubset(theme,sub);
    }

void FMTmask::append(const boost::dynamic_bitset<> &bits)
            {
            const size_t thissize = data.size();
            data.resize(data.size() + bits.size());
            size_t bid = 0;
            for (size_t i=thissize; i<data.size(); ++i)
                {
                data[i] = bits[bid];
                ++bid;
                }
            }

void FMTmask::update(const std::vector<FMTtheme>& themes)
    {
    name = "";
    for(const FMTtheme& the : themes)
        {
        const boost::dynamic_bitset<>sub = subset(the);
        name+=the.bitstostr(sub)+" ";
        }
    name = name.substr(0, name.size()-1);
    }
/*bool FMTmask::linkNvalidate(const std::vector<FMTtheme>& themes)
            {
            std::vector<std::string>bases;
            boost::split(bases,name,boost::is_any_of(FMT_STR_SEPARATOR), boost::token_compress_on);
            int themeid = 0;
            boost::dynamic_bitset<> themebits;
            data.clear();
            if(themes.size()!=bases.size())
                {
                return false;
                }
            for(const std::string& value : bases)
                {
                const FMTtheme* theme = &themes[themeid];
                if((!theme->isaggregate(value))&&(!theme->isattribute(value))&&(!(value=="?")))
                    {
                    return false;
                    }
                themebits = theme->strtobits(value);
                this->append(themebits);
                ++themeid;
                }
            return true;
            }*/

FMTmask FMTmask::getunion(const FMTmask& rhs) const
	{
	FMTmask newmask(*this);
	newmask.name.clear();
	newmask.data |= rhs.data;
	return newmask;
	}

FMTmask::FMTmask(const FMTmask& rhs) : name(rhs.name),data(rhs.data)
            {


            }

FMTmask& FMTmask::operator = (const FMTmask& rhs)
            {
            if (this!=&rhs)
                {
                data = (rhs.data);
                name =rhs.name;
                }
            return *this;
            }

FMTmask FMTmask::resume(const boost::dynamic_bitset<>& rhs) const
            {
            FMTmask newmask;
            newmask.name = this->name;
			newmask.data.resize(rhs.size());
            for (boost::dynamic_bitset<>::size_type i = 0; i < rhs.size(); ++i)
                {
                if (rhs[i])
                    {
                    newmask.data[i]=(data[i]);
                    }
                }

            return newmask;
            }


bool FMTmask::operator == (const FMTmask& rhs) const
    {
    if(data.size() == rhs.data.size())
        {
        return data == rhs.data;
        }
    return false;
    }

bool FMTmask::operator != (const FMTmask& rhs) const
    {
    return !(*this==rhs);
    }

bool FMTmask::operator < (const FMTmask& rhs) const
    {
    if(data.size() == rhs.data.size())
        {
        return (data < rhs.data);
        }else if(data.size() < rhs.data.size())
            {
            return true;
            }
    return false;
    }

FMTmask FMTmask::removeaggregates(const std::vector<FMTtheme>& themes) const
	{
	FMTmask newmask(*this);
	newmask.name.clear();
	const boost::dynamic_bitset<> nullmask(data.size(), false);
	for (const FMTtheme& theme : themes)
		{
		const boost::dynamic_bitset<> localtheme = newmask.subset(theme);
		if (localtheme.count()>1 || (localtheme.count()==1 && localtheme.size() == 1))
			{
			newmask.setsubset(theme, nullmask);
			}
		}
	return newmask;
	}

void FMTmask::clear()
	{
	data.clear();
	name.clear();
	}
size_t FMTmask::size() const
	{
	return data.size();
	}

bool FMTmask::isnotthemessubset(const FMTmask& rhs, const std::vector<FMTtheme>& themes) const
	{
	const boost::dynamic_bitset<> intersection = (rhs.data & this->data);
	size_t thid = 0;
	size_t bitloc = 0;
	size_t totalthemelength = 0;
	bool founddifference = false;
	size_t falsefound = 0;
	while (!founddifference && bitloc < data.size())
		{
		if (!intersection[bitloc])
			{
			++falsefound;
			}
		const size_t themesize = themes.at(thid).size();
		if (bitloc == (themesize + totalthemelength)-1)
			{
			if (themesize == falsefound)//in that case we accept aggregate and "?")
				{
				founddifference = true;
				}
			++thid;
			falsefound = 0;
			totalthemelength += themesize;
			}
		++bitloc;
		}
	return founddifference;
	}

std::string FMTmask::getbitsstring() const
            {
            std::string bits = "";
            for (size_t i=0; i<data.size(); ++i)
                {
                if (data[i])
                    {
                    bits+="1 ";
                    }else{
                    bits+="0 ";
                    }
                }
            bits.pop_back();
            return bits;
            }

FMTmask FMTmask::presolve(const FMTmask& selectedmask, const std::vector<FMTtheme>&presolvedthemes) const
	{
	std::vector<std::string>bases;
	size_t basesize = 0;
	for (const FMTtheme& theme : presolvedthemes)
		{
		basesize += theme.size();
		}
	boost::dynamic_bitset<> newdata(basesize,false);
	size_t selectedloc = 0;
	for (size_t bitid = 0; bitid < selectedmask.data.size();++bitid)
		{
		if (selectedmask.data[bitid])
			{
			newdata[selectedloc] = data[bitid];
			++selectedloc;
			}
		}
	std::string newname;
	if (!name.empty())
		{
		FMTmask newmask(newdata);
		for (const FMTtheme& theme : presolvedthemes)
			{
			newname += theme.bitstostr(newmask.subset(theme)) + " ";
			}
		newname.pop_back();
		}
	return FMTmask(newname, newdata);
	}

}

