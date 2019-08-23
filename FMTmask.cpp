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
/*
namespace Core{

FMTmask::FMTmask(): boost::dynamic_bitset<>(1,false),name()
    {

    }

FMTmask::FMTmask(string mask,boost::dynamic_bitset<> bits): boost::dynamic_bitset<>(bits),name(mask){}
FMTmask::FMTmask(boost::dynamic_bitset<> bits): boost::dynamic_bitset<>(bits),name(){}
string FMTmask::getstr() const
    {
    return name;
    }
FMTmask::FMTmask(string mask,const vector<FMTtheme>& themes):boost::dynamic_bitset<>(),name()
        {
        if(!this->set(themes,mask))
            {
            //crash here for this mask..
            }
        }
FMTmask::FMTmask(vector<string>values,vector<FMTtheme>& themes):boost::dynamic_bitset<>(),name()
    {
    for(string& el : values)
        {
        name+=el+" ";
        }
    name = name.substr(0, name.size()-1);
    if(!this->set(themes,name))
        {
        //crash here for this mask..
        }
    }
vector<FMTmask> FMTmask::decompose(const FMTtheme &theme) const
    {
    vector<string>bases;
    string value = get(theme);
    vector<string>atts = theme.getattributes(value);
    vector<FMTmask>newmasks;
    for(string& att : atts)
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
    for(int id = theme.start; id < (theme.start + int(theme.size())); ++id)
        {
        if (boost::dynamic_bitset<>::operator[](id))
            {
            sub[locit] = true;
            }else{
            sub[locit] = false;
            }
        ++locit;
        }
    return sub;
    }
bool FMTmask::setsubset(const FMTtheme& theme,boost::dynamic_bitset<>& subset)
    {
    int locit = 0;
    for(int id = theme.start; id < (theme.start+int(theme.size())); ++id)
        {
        if (subset[locit])
            {
            boost::dynamic_bitset<>::operator[](id) = true;
            }else{
            boost::dynamic_bitset<>::operator[](id) = false;
            }
        ++locit;
        }
    return true;
    }
string FMTmask::get(vector<FMTtheme>& themes) const
    {
    string value = "";
    for(FMTtheme& theme : themes)
        {
        boost::dynamic_bitset<>bits = this->subset(theme);
        value += theme.bitstostr(bits) + " ";
        }
    value = value.substr(0, value.size()-1);
    return value;
    }
bool FMTmask::set(const vector<FMTtheme>& themes,string value)
    {
    name = value;
    vector<string>bases;
    boost::split(bases,value,boost::is_any_of("\t "));
    size_t fullsize = 0;
    for(const FMTtheme& theme : themes)
        {
        fullsize+=theme.size();
        }
    this->resize(fullsize,false);
    for(const FMTtheme& theme : themes)
        {
        boost::dynamic_bitset<>bits = theme.strtobits(bases[theme.id]);
        if (!this->setsubset(theme,bits))
            {
            return false;
            }
        }
    return true;
    }
string FMTmask::get(const FMTtheme& theme) const
    {
    boost::dynamic_bitset<>bits =  this->subset(theme);
    return theme.bitstostr(bits);
    }
bool FMTmask::set(const FMTtheme& theme,string value)
    {
    boost::dynamic_bitset<>sub=theme.strtobits(value);
    vector<string>bases;
    boost::split(bases,name,boost::is_any_of("\t "));
    bases[theme.id] = value;
    name = boost::algorithm::join(bases," ");
    return (this->setsubset(theme,sub));
    }

void FMTmask::append(boost::dynamic_bitset<> &bits)
            {
            size_t thissize = this->size();
            this->resize(this->size() + bits.size());
            size_t bid = 0;
            for (size_t i=thissize; i<this->size(); ++i)
                {
                boost::dynamic_bitset<>::operator[](i) = bits[bid];
                ++bid;
                }
            }

void FMTmask::update(const vector<FMTtheme>& themes)
    {
    name = "";
    for(const FMTtheme& the : themes)
        {
        boost::dynamic_bitset<>sub = subset(the);
        name+=the.bitstostr(sub)+" ";
        }
    name = name.substr(0, name.size()-1);
    }
bool FMTmask::linkNvalidate(vector<FMTtheme>& themes)
            {
            vector<string>bases;
            boost::split(bases,name,boost::is_any_of("\t "));
            int themeid = 0;
            FMTtheme* theme;
            boost::dynamic_bitset<> themebits;
            this->clear();
            if(themes.size()!=bases.size())
                {
                return false;
                }
            for(string& value : bases)
                {
                theme = &themes[themeid];
                if((!theme->isaggregate(value))&&(!theme->isattribute(value))&&(!(value=="?")))
                    {
                    return false;
                    }
                themebits = theme->strtobits(value);
                this->append(themebits);
                ++themeid;
                }
            return true;
            }
FMTmask::FMTmask(const FMTmask& rhs) : boost::dynamic_bitset<>(rhs),name(rhs.name)
            {
            }

FMTmask& FMTmask::operator = (const FMTmask& rhs)
            {
            if (this!=&rhs)
                {
                boost::dynamic_bitset<>::operator = (rhs);
                name =rhs.name;
                }
            return *this;
            }

FMTmask FMTmask::resume(const boost::dynamic_bitset<>& rhs) const
            {
            FMTmask newmask;
            newmask.name = this->name;
            for (boost::dynamic_bitset<>::size_type i = 0; i < rhs.size(); ++i)
                {
                if (rhs[i])
                    {
                    newmask.push_back(boost::dynamic_bitset<>::operator[](i));
                    }
                }

            return newmask;
            }

bool FMTmask::operator == (const FMTmask& rhs) const
    {
    if(this->size() == rhs.size())
        {
        return boost::operator==(*this,rhs);
        }
    return false;
    }

bool FMTmask::operator != (const FMTmask& rhs) const
    {
    return !(*this==rhs);
    }

bool FMTmask::operator < (const FMTmask& rhs) const
    {
    if(this->size() == rhs.size())
        {
        return boost::operator<(*this,rhs);
        }else if(this->size() < rhs.size())
            {
            return true;
            }
    return false;
    }
string FMTmask::to_string() const
            {
            string bits = "";
            for (size_t i=0; i<this->size(); ++i)
                {
                if (boost::dynamic_bitset<>::operator[](i))
                    {
                    bits+="1 ";
                    }else{
                    bits+="0 ";
                    }
                }
            bits.pop_back();
            return bits;
            }

}

*/
namespace Core{

FMTmask::FMTmask(): name(),data()//data(1,false)
    {

    }

FMTmask::FMTmask(FMTmask&& rhs) noexcept : name(std::move(rhs.name)),data(std::move(rhs.data))
	{
	//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "MOVVINGGGGGG "<< "\n";
	}

FMTmask& FMTmask::operator = (FMTmask&& rhs)
	{
	name = std::move(rhs.name);
	data = std::move(rhs.data);
	return *this;
	}

FMTmask::operator bool() const
	{
	return (!data.empty());
	}

FMTmask::FMTmask(string mask,boost::dynamic_bitset<> bits):name(mask),data(bits){}
FMTmask::FMTmask(boost::dynamic_bitset<> bits): name(),data(bits){}
string FMTmask::getstr() const
    {
    return name;
    }
FMTmask::operator string() const
	{
	return name;
	}
FMTmask::FMTmask(string mask,const vector<FMTtheme>& themes):name(),data()
        {
        if(!this->set(themes,mask))
            {
            //crash here for this mask..
            }
        }
FMTmask::FMTmask(const vector<string>&values,const vector<FMTtheme>& themes):name(),data()
    {
    for(const string& el : values)
        {
        name+=el+" ";
        }
    name = name.substr(0, name.size()-1);
    if(!this->set(themes,name))
        {
        //crash here for this mask..
        }
    }
vector<FMTmask> FMTmask::decompose(const FMTtheme &theme) const
    {
    vector<string>bases;
    string value = get(theme);
    vector<string>atts = theme.getattributes(value);
    vector<FMTmask>newmasks;
    for(string& att : atts)
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
    for(int id = theme.start; id < (theme.start + int(theme.size())); ++id)
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
bool FMTmask::setsubset(const FMTtheme& theme,boost::dynamic_bitset<>& subset)
    {
    int locit = 0;
    for(int id = theme.start; id < (theme.start+int(theme.size())); ++id)
        {
        if (subset[locit])
            {
            data[id] = true;
            }else{
            data[id] = false;
            }
        ++locit;
        }
    return true;
    }
string FMTmask::get(vector<FMTtheme>& themes) const
    {
    string value = "";
    for(FMTtheme& theme : themes)
        {
        boost::dynamic_bitset<>bits = this->subset(theme);
        value += theme.bitstostr(bits) + " ";
        }
    value = value.substr(0, value.size()-1);
    return value;
    }
bool FMTmask::set(const vector<FMTtheme>& themes,string value)
    {
    name = value;
    vector<string>bases;
    boost::split(bases,value,boost::is_any_of("\t "),boost::token_compress_on);
    size_t fullsize = 0;
    for(const FMTtheme& theme : themes)
        {
        fullsize+=theme.size();
        }
    data.resize(fullsize,false);
    for(const FMTtheme& theme : themes)
        {
        boost::dynamic_bitset<>bits = theme.strtobits(bases[theme.id]);
        if (!this->setsubset(theme,bits))
            {
            return false;
            }
        }
    return true;
    }
string FMTmask::get(const FMTtheme& theme) const
    {
    boost::dynamic_bitset<>bits =  this->subset(theme);
    return theme.bitstostr(bits);
    }

bool FMTmask::empty() const
	{
	return data.empty();
	}

vector<FMTtheme>FMTmask::getstaticthemes(const vector<FMTtheme>& themes) const
	{
	vector<FMTtheme>staticths;
	for (const FMTtheme& theme : staticths)
		{
		string value = this->get(theme);
		if (!theme.isaggregate(value) && !theme.isattribute(value))
			{
			staticths.push_back(theme);
			}
		}
	return staticths;
	}


bool FMTmask::set(const FMTtheme& theme,string value)
    {
    boost::dynamic_bitset<>sub=theme.strtobits(value);
    vector<string>bases;
    boost::split(bases,name,boost::is_any_of("\t "), boost::token_compress_on);
    bases[theme.id] = value;
    name = boost::algorithm::join(bases," ");
    return (this->setsubset(theme,sub));
    }

void FMTmask::append(boost::dynamic_bitset<> &bits)
            {
            size_t thissize = data.size();
            data.resize(data.size() + bits.size());
            size_t bid = 0;
            for (size_t i=thissize; i<data.size(); ++i)
                {
                data[i] = bits[bid];
                ++bid;
                }
            }

void FMTmask::update(const vector<FMTtheme>& themes)
    {
    name = "";
    for(const FMTtheme& the : themes)
        {
        boost::dynamic_bitset<>sub = subset(the);
        name+=the.bitstostr(sub)+" ";
        }
    name = name.substr(0, name.size()-1);
    }
bool FMTmask::linkNvalidate(vector<FMTtheme>& themes)
            {
            vector<string>bases;
            boost::split(bases,name,boost::is_any_of("\t "), boost::token_compress_on);
            int themeid = 0;
            FMTtheme* theme;
            boost::dynamic_bitset<> themebits;
            data.clear();
            if(themes.size()!=bases.size())
                {
                return false;
                }
            for(string& value : bases)
                {
                theme = &themes[themeid];
                if((!theme->isaggregate(value))&&(!theme->isattribute(value))&&(!(value=="?")))
                    {
                    return false;
                    }
                themebits = theme->strtobits(value);
                this->append(themebits);
                ++themeid;
                }
            return true;
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
string FMTmask::to_string() const
            {
            string bits = "";
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

}

