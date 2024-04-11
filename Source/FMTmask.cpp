/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#include "FMTmask.hpp"
#include "FMTtheme.hpp"
#include "FMTmaskfilter.hpp"

namespace Core{

FMTmask::FMTmask(): name(),data()
    {

    }

FMTmask::operator bool() const
	{
	return (!data.empty());
	}

FMTmask::FMTmask(const std::string& mask,const boost::dynamic_bitset<uint8_t>& bits):name(mask),data(bits)
	{
	//name.shrink_to_fit();
	}
FMTmask::FMTmask(const boost::dynamic_bitset<uint8_t>& bits): name(),data(bits)
	{

	}

FMTmask::FMTmask(const std::vector<FMTtheme>& themes)
	{
	size_t basesize = 0;
	for (const FMTtheme& theme : themes)
	{
		basesize += theme.size();
	}
	data.resize(basesize, false);
	}


FMTmask::FMTmask(const std::string& mask,const std::vector<FMTtheme>& themes):name(),data()
    {
	this->set(themes, mask);
	//name.shrink_to_fit();
    }
FMTmask::FMTmask(const std::vector<std::string>&values,const std::vector<FMTtheme>& themes):name(),data()
    {
    for(const std::string& el : values)
        {
        name+=el+" ";
        }
    //name = name.substr(0, name.size()-1);
	name.pop_back();
	this->set(themes, name);
	//name.shrink_to_fit();
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
 boost::dynamic_bitset<uint8_t> FMTmask::subset(const FMTtheme& theme) const
    {
    boost::dynamic_bitset<uint8_t> sub(theme.size(),false);
    int locit = 0;
    for(size_t id = theme.m_start; id < (theme.m_start + theme.size()); ++id)
        {
		sub[locit] = data[id];
        ++locit;
        }
    return sub;
    }

FMTmask FMTmask::getpostsolvemask(const FMTmask& mask, const std::vector<FMTtheme>& themes) const
	{
	FMTmask postsolvedmask(*this);
	for (const Core::FMTtheme & basetheme : themes)
		{
		if (mask.getsubsetcount(basetheme)!=0)//scrap this theme in the devmask
			{
			 boost::dynamic_bitset<uint8_t> sub(basetheme.size(),false);
			postsolvedmask.setsubset(basetheme, sub);
			}
		}
	return postsolvedmask;
	}

size_t FMTmask::getsubsetcount(const FMTtheme& theme) const
{
	return subset(theme).count();
}

void FMTmask::setsubset(const FMTtheme& theme,const boost::dynamic_bitset<uint8_t>& subset)
    {
    int locit = 0;
    for(size_t id = theme.m_start; id < (theme.m_start+theme.size()); ++id)
        {
        data[id] = subset[locit];
        ++locit;
        }
    }

std::string FMTmask::get(const std::vector<FMTtheme>& themes) const
    {
	std::string value = "";
    for(const FMTtheme& theme : themes)
        {
        const boost::dynamic_bitset<uint8_t>bits = this->subset(theme);
        value += theme.bitsToStr(bits) + " ";
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
        const boost::dynamic_bitset<uint8_t>bits = theme.strToBits(bases[theme.m_id]);
		this->setsubset(theme, bits);
        }
	//name.shrink_to_fit();
    }
std::string FMTmask::get(const FMTtheme& theme) const
    {
    const boost::dynamic_bitset<uint8_t>bits =  this->subset(theme);
    return theme.bitsToStr(bits);
    }

bool FMTmask::empty() const
	{
	return data.empty();
	}

std::vector<FMTtheme>FMTmask::getstaticthemes(const std::vector<FMTtheme>& themes) const
	{
	std::vector<FMTtheme>staticths;
	for (const FMTtheme& theme : themes)
		{
		const std::string value = this->get(theme);
		if (!theme.isaggregate(value) && !theme.isattribute(value))
			{
			staticths.push_back(theme);
			}
		}
	return staticths;
	}

std::vector<FMTtheme> FMTmask::getselectedthemes(const  std::vector<FMTtheme>& themes) const
{
	std::vector<FMTtheme>selected;
	for (const FMTtheme& theme : themes)
	{
		if (subset(theme).any())
		{
			selected.push_back(theme);
		}
	}
	return selected;
}


void FMTmask::set(const FMTtheme& theme,const std::string& value)
    {
    const boost::dynamic_bitset<uint8_t>sub=theme.strToBits(value);
    std::vector<std::string>bases;
    boost::split(bases,name,boost::is_any_of(FMT_STR_SEPARATOR), boost::token_compress_on);
    bases[theme.m_id] = value;
    name = boost::algorithm::join(bases," ");
	
    this->setsubset(theme,sub);

	//name.shrink_to_fit();
    }

void FMTmask::append(const boost::dynamic_bitset<uint8_t> &bits)
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
    //name = "";
	name.clear();
    for(const FMTtheme& the : themes)
        {
        const boost::dynamic_bitset<uint8_t>sub = subset(the);
        name+=the.bitsToStr(sub)+" ";
        }
    name = name.substr(0, name.size()-1);
	//name.shrink_to_fit();
    }

FMTmask FMTmask::getunion(const FMTmask& rhs) const
	{
	FMTmask newmask(data);
	newmask.data |= rhs.data;
	return newmask;
	}

FMTmask FMTmask::getintersect(const FMTmask& rhs) const
	{
	FMTmask newmask(data);
	newmask.data &= rhs.data;
	return newmask;
	}

 boost::dynamic_bitset<uint8_t> FMTmask::getbitsetintersect(const FMTmask& rhs) const
	{
	return data & rhs.data;
	}


FMTmask::FMTmask(const FMTmask& rhs) : name(rhs.name),data(rhs.data)
            {


            }

void FMTmask::swap(FMTmask& rhs)
	{
	data.swap(rhs.data);
	name.swap(rhs.name);
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

FMTmask FMTmask::resume(const boost::dynamic_bitset<uint8_t>& rhs) const
            {
			FMTmask newmask(name, boost::dynamic_bitset<uint8_t>(rhs.count()));
			size_t location = rhs.find_first();
			size_t newlocation = 0;
			while (location!=rhs.npos)
				{
				newmask.data[newlocation] = data[location];
				location = rhs.find_next(location);
				++newlocation;
				}
			return newmask;
            }

FMTmask FMTmask::resume(const std::vector<size_t>& indexes) const
		{
		FMTmask newmask(name, boost::dynamic_bitset<uint8_t>(indexes.size()));
		size_t baseid = 0;
		for (const size_t& id : indexes)
			{
			newmask.data[baseid] = data[id];
			++baseid;
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

FMTmask FMTmask::removeaggregates(const std::vector<FMTtheme>& themes, bool questionmarkonly) const
	{
	FMTmask newmask(*this);
	newmask.name.clear();
	const boost::dynamic_bitset<uint8_t> nullmask(data.size(), false);
	for (const FMTtheme& theme : themes)
		{
		const boost::dynamic_bitset<uint8_t> localtheme = newmask.subset(theme);
		if (!questionmarkonly&&(localtheme.count()>1 || (localtheme.count()==1 && localtheme.size() == 1))||
			questionmarkonly&&localtheme.count()==localtheme.size())
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

size_t FMTmask::count() const
	{
	return data.count();
	}


bool FMTmask::isnotthemessubset(const FMTmask& rhs, const std::vector<FMTtheme>& themes) const
	{
	for (const FMTtheme& theme : themes)
		{
		const size_t themestart = theme.getstart();
		const size_t themestop = themestart + theme.size()-1;
		if (!(rhs.data[themestart]&this->data[themestart])&&
			!(rhs.data[themestop]&this->data[themestop])&&
			!((subset(theme) & rhs.subset(theme)).any()))
			{
			return true;
			}
		}
	return false;
	}

std::string FMTmask::getbitsstring() const
            {
			std::string buffer;
			boost::to_string(data, buffer);
            return buffer;
            }

FMTmask FMTmask::refine(const FMTmask& mask,const std::vector<FMTtheme>& themes) const
	{
		if(size()==count())
		{
			return mask;
		}
		std::vector<std::string>bases;
		std::vector<std::string>maskbases;
		boost::split(bases,name,boost::is_any_of(FMT_STR_SEPARATOR),boost::token_compress_on);
		const std::string basemask = std::string(mask);
		boost::split(maskbases, basemask,boost::is_any_of(FMT_STR_SEPARATOR),boost::token_compress_on);
		for(const FMTtheme& theme : themes)
			{	
			const size_t thiscount = subset(theme).count();
			const size_t targetcount = mask.subset(theme).count();
			if (thiscount == theme.size() ||  targetcount < thiscount)
			{
				bases.at(theme.m_id) = maskbases.at(theme.m_id);
			}
			/*if (targetcount > 1 && !allow_aggregates)
			{
				//crash
			}else if (thiscount==theme.size()||((targetcount== 1||allow_aggregates) && targetcount < thiscount))
				{
				bases.at(theme.id) = maskbases.at(theme.id);
				}*/
			}
		return FMTmask(boost::algorithm::join(bases," "),themes);
	}	

FMTmask FMTmask::presolve(const FMTmaskfilter& filter, const std::vector<FMTtheme>&presolvedthemes) const
	{
	FMTmask newmask;
	newmask.data.resize(filter.flippedselection.count(), false);
	size_t selectedloc = 0;
	for (size_t bitid = 0; bitid < filter.flippedselection.size();++bitid)
		{
		if (filter.flippedselection[bitid])
			{
			newmask.data[selectedloc] = data[bitid];
			++selectedloc;
			}
		}
	if (!name.empty())
		{
		for (const FMTtheme& theme : presolvedthemes)
			{
			newmask.name += theme.bitsToStr(newmask.subset(theme)) + " ";
			}
		newmask.name.pop_back();
		//newmask.name.shrink_to_fit();
		}
	return newmask;
	}

FMTmask FMTmask::postsolve(const FMTmaskfilter& filter,
	const std::vector<FMTtheme>&basethemes) const
	{
	FMTmask newmask(filter.flippedselection);
	size_t presolvedid = 0;
	for (size_t mid = 0; mid < filter.selection.size();++mid)
		{
		if (filter.selection[mid])
			{
			newmask.data[mid] = data[presolvedid];
			++presolvedid;
			}
		}
	for (const FMTtheme& theme: basethemes)
		{
		newmask.name += theme.bitsToStr(newmask.subset(theme)) + " ";
		}
	newmask.name.pop_back();
	//newmask.name.shrink_to_fit();
	return newmask;
	}

FMTmaskcomparator::FMTmaskcomparator(const FMTmask& lbase_mask) : base_mask(lbase_mask)
    {

    }

bool FMTmaskcomparator::operator()(const FMTmask& mask) const
    {
    return (base_mask == mask);
    }



}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTmask)
