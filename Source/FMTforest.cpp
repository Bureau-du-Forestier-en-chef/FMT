/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTforest.h"
#include "FMTactualdevelopment.h"
#include "FMTfuturdevelopment.h"
#include "FMTtheme.h"
#include "FMTexceptionhandler.h"

namespace Spatial
{
FMTforest::FMTforest():FMTlayer<Core::FMTdevelopment>(){}
FMTforest::FMTforest(const FMTlayer<Core::FMTdevelopment>& rhs):FMTlayer<Core::FMTdevelopment>(rhs){}
FMTforest::FMTforest(const FMTforest& rhs):FMTlayer<Core::FMTdevelopment>(rhs){}
FMTforest& FMTforest::operator = (const FMTforest& rhs)
    {
    if (this!=&rhs)
        {
        FMTlayer<Core::FMTdevelopment>::operator = (rhs);
        }
    return *this;
    }

FMTlayer<std::string>FMTforest::getlock() const
    {
    FMTlayer<std::string>newlayer(this->geotransform,this->maxx,this->maxy,this->SRS_WKT,this->cellsize);
    for(std::map<FMTcoordinate,Core::FMTdevelopment>::const_iterator it = mapping.begin(); it != mapping.end(); ++it)
        {
        if (it->second.getlock() > 0 )
            {
            newlayer[it->first] = "_Lock " + std::to_string(it->second.getlock());
            }
        }
    return newlayer;
    }

void FMTforest::setperiod(int period)
	{
	for (std::map<FMTcoordinate, Core::FMTdevelopment>::iterator it = mapping.begin(); it != mapping.end(); ++it)
		{
		it->second.setperiod(period);
		}
	}


std::vector<Core::FMTactualdevelopment>FMTforest::getarea() const
            {
	std::vector<Core::FMTactualdevelopment>devs;
            for(std::map<FMTcoordinate,Core::FMTdevelopment>::const_iterator it = mapping.begin(); it != mapping.end(); ++it)
                {
                const Core::FMTactualdevelopment newadev(it->second,cellsize);
				std::vector<Core::FMTactualdevelopment>::iterator devit = std::find_if(devs.begin(),devs.end(), Core::FMTactualdevelopmentcomparator(&newadev));
                if (devit==devs.end())
                    {
                    devs.push_back(newadev);
                    }else{
                    devit->setarea(devit->getarea()+(cellsize));
                    }
                }
			
            return devs;
            }
FMTforest FMTforest::grow() const
            {
            FMTforest newforest(*this);
            for(std::map<FMTcoordinate,Core::FMTdevelopment>::const_iterator devit = mapping.begin(); devit != mapping.end(); ++devit)
                {
                const Core::FMTdevelopment* dev = &devit->second;
                const Core::FMTdevelopment newdev = dev->grow();
                newforest.mapping[devit->first] = newdev;
                }
            return newforest;
            }

std::vector<FMTlayer<std::string>> FMTforest::getthemes(const std::vector<Core::FMTtheme>& themes) const
            {
			std::vector<FMTlayer<std::string>> newlayers(themes.size(),FMTlayer<std::string>(this->geotransform,this->maxx,this->maxy,this->SRS_WKT,this->cellsize));
            for(std::map<FMTcoordinate,Core::FMTdevelopment>::const_iterator it = mapping.begin(); it != mapping.end(); ++it)
                {
                for(size_t id = 0 ; id < newlayers.size();++id)
                    {
					const std::string value = it->second.getmask().getAttribute(themes[id]);
                    newlayers[id][it->first] = value;
                    }
                }
            return newlayers;
            }
FMTlayer<int>FMTforest::getage() const
            {
            FMTlayer<int>newlayer(this->geotransform,this->maxx,this->maxy,this->SRS_WKT,this->cellsize);
            for(std::map<FMTcoordinate,Core::FMTdevelopment>::const_iterator it = mapping.begin(); it != mapping.end(); ++it)
                {
                newlayer[it->first] = it->second.getage();
                }
            return newlayer;
            }

FMTforest FMTforest::getcopy(bool copydata) const
    {
    FMTlayer<Core::FMTdevelopment> forest(this->geotransform,this->maxx,this->maxy,this->SRS_WKT,this->cellsize);
    if(copydata)
        {
        forest = *this;
        }
    return forest;
    }
void  FMTforest::SetLastThemeWithArea(const std::vector<Core::FMTactualdevelopment>& p_devs,
							const std::vector<Core::FMTtheme>& p_themes)
{
	try {
		std::map<std::string, std::string>new_Masks;
		for (const Core::FMTactualdevelopment& DEV : p_devs)
		{
			std::string subset;
			for (size_t i = 0; i < p_themes.size() - 1; ++i)
			{
				subset += DEV.getmask().getAttribute(p_themes.at(i))+ " ";
			}
			new_Masks[subset] = DEV.getmask().getAttribute(p_themes.back());
		}
		for (auto& DevWithLocation : *this)
			{
			std::string subset;
			for (size_t i = 0; i < p_themes.size() - 1; ++i)
				{
				subset += DevWithLocation.second.getmask().getAttribute(p_themes.at(i))+" ";
				}
			if (new_Masks.find(subset)== new_Masks.end())
				{
                _exhandler->raise(Exception::FMTexc::FMTinvalid_theme, "Missing mask " + subset +" in area",
                    "FMTforest::SetLastThemeWithArea", __LINE__, __FILE__);
				}
			subset += new_Masks[subset];
			DevWithLocation.second.setmask(Core::FMTmask(subset, p_themes));
			}
	}catch (...)
	{
		_exhandler->raisefromcatch("", "FMTforest::SetLastThemeWithArea", __LINE__, __FILE__);
	}

}

FMTforest FMTforest::presolve(const Core::FMTmaskfilter& filter, const std::vector<Core::FMTtheme>&presolvedthemes) const
	{
	FMTforest newforest(*this);
	try{
		for (std::map<FMTcoordinate, Core::FMTdevelopment>::iterator coordit = newforest.mapping.begin();
			coordit != newforest.mapping.end(); ++coordit)
			{
			coordit->second = Core::FMTactualdevelopment(coordit->second, 0.0).presolve(filter, presolvedthemes);
			}
	
	}catch (...)
	{
		_exhandler->raisefromcatch("", "FMTforest::presolve", __LINE__, __FILE__);
	}
	
	
	return newforest;
	}



FMTforest FMTforest::postsolve(const Core::FMTmaskfilter& filter, const std::vector<Core::FMTtheme>&originalbasethemes) const
	{
	FMTforest newforest(*this);
	for (std::map<FMTcoordinate, Core::FMTdevelopment>::iterator coordit = newforest.mapping.begin();
		coordit != newforest.mapping.end(); ++coordit)
		{
		coordit->second.setmask(coordit->second.getmask().postsolve(filter, originalbasethemes));
		}
	return newforest;
	}

}
