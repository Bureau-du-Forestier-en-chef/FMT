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
FMTforest::FMTforest():FMTlayer<Core::FMTDevelopment>(){}
FMTforest::FMTforest(const FMTlayer<Core::FMTDevelopment>& rhs):FMTlayer<Core::FMTDevelopment>(rhs){}
FMTforest::FMTforest(const FMTforest& rhs):FMTlayer<Core::FMTDevelopment>(rhs){}
FMTforest& FMTforest::operator = (const FMTforest& rhs)
    {
    if (this!=&rhs)
        {
        FMTlayer<Core::FMTDevelopment>::operator = (rhs);
        }
    return *this;
    }

FMTlayer<std::string>FMTforest::getLock() const
    {
    FMTlayer<std::string>newlayer(this->geotransform,this->maxx,this->maxy,this->SRS_WKT,this->cellsize);
    for(std::map<FMTcoordinate,Core::FMTDevelopment>::const_iterator it = mapping.begin(); it != mapping.end(); ++it)
        {
        if (it->second.getLock() > 0 )
            {
            newlayer[it->first] = "_Lock " + std::to_string(it->second.getLock());
            }
        }
    return newlayer;
    }

void FMTforest::setPeriod(int period)
	{
	for (std::map<FMTcoordinate, Core::FMTDevelopment>::iterator it = mapping.begin(); it != mapping.end(); ++it)
		{
		it->second.setPeriod(period);
		}
	}


std::vector<Core::FMTActualDevelopment>FMTforest::getArea() const
            {
	std::vector<Core::FMTActualDevelopment>devs;
            for(std::map<FMTcoordinate,Core::FMTDevelopment>::const_iterator it = mapping.begin(); it != mapping.end(); ++it)
                {
                const Core::FMTActualDevelopment newadev(it->second,cellsize);
				std::vector<Core::FMTActualDevelopment>::iterator devit = std::find_if(devs.begin(),devs.end(), Core::FMTActualDevelopmentComparator(&newadev));
                if (devit==devs.end())
                    {
                    devs.push_back(newadev);
                    }else{
                    devit->setArea(devit->getArea()+(cellsize));
                    }
                }
			
            return devs;
            }
FMTforest FMTforest::grow() const
            {
            FMTforest newforest(*this);
            for(std::map<FMTcoordinate,Core::FMTDevelopment>::const_iterator devit = mapping.begin(); devit != mapping.end(); ++devit)
                {
                const Core::FMTDevelopment* dev = &devit->second;
                const Core::FMTDevelopment newDev = dev->grow();
                newforest.mapping[devit->first] = newDev;
                }
            return newforest;
            }

std::vector<FMTlayer<std::string>> FMTforest::getThemes(const std::vector<Core::FMTTheme>& themes) const
            {
			std::vector<FMTlayer<std::string>> newlayers(themes.size(),FMTlayer<std::string>(this->geotransform,this->maxx,this->maxy,this->SRS_WKT,this->cellsize));
            for(std::map<FMTcoordinate,Core::FMTDevelopment>::const_iterator it = mapping.begin(); it != mapping.end(); ++it)
                {
                for(size_t id = 0 ; id < newlayers.size();++id)
                    {
					const std::string value = it->second.getMask().getAttribute(themes[id]);
                    newlayers[id][it->first] = value;
                    }
                }
            return newlayers;
            }
FMTlayer<int>FMTforest::getAge() const
            {
            FMTlayer<int>newlayer(this->geotransform,this->maxx,this->maxy,this->SRS_WKT,this->cellsize);
            for(std::map<FMTcoordinate,Core::FMTDevelopment>::const_iterator it = mapping.begin(); it != mapping.end(); ++it)
                {
                newlayer[it->first] = it->second.getAge();
                }
            return newlayer;
            }

FMTforest FMTforest::getCopy(bool copyData) const
    {
    FMTlayer<Core::FMTDevelopment> forest(this->geotransform,this->maxx,this->maxy,this->SRS_WKT,this->cellsize);
    if(copyData)
        {
        forest = *this;
        }
    return forest;
    }
void  FMTforest::setLastThemeWithArea(const std::vector<Core::FMTActualDevelopment>& p_devs,
							const std::vector<Core::FMTTheme>& p_themes)
{
	try {
		std::map<std::string, std::string>new_Masks;
		for (const Core::FMTActualDevelopment& DEV : p_devs)
		{
			std::string subset;
			for (size_t i = 0; i < p_themes.size() - 1; ++i)
			{
				subset += DEV.getMask().getAttribute(p_themes.at(i))+ " ";
			}
			new_Masks[subset] = DEV.getMask().getAttribute(p_themes.back());
		}
		for (auto& DevWithLocation : *this)
			{
			std::string subset;
			for (size_t i = 0; i < p_themes.size() - 1; ++i)
				{
				subset += DevWithLocation.second.getMask().getAttribute(p_themes.at(i))+" ";
				}
			if (new_Masks.find(subset)== new_Masks.end())
				{
                _exhandler->raise(Exception::FMTexc::FMTinvalid_theme, "Missing mask " + subset +" in area",
                    "FMTforest::setLastThemeWithArea", __LINE__, __FILE__);
				}
			subset += new_Masks[subset];
			DevWithLocation.second.setMask(Core::FMTMask(subset, p_themes));
			}
	}catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTforest::setLastThemeWithArea", __LINE__, __FILE__);
	}

}

FMTforest FMTforest::presolve(const Core::FMTMaskFilter& filter, const std::vector<Core::FMTTheme>&presolvedthemes) const
	{
	FMTforest newforest(*this);
	try{
		for (std::map<FMTcoordinate, Core::FMTDevelopment>::iterator coordit = newforest.mapping.begin();
			coordit != newforest.mapping.end(); ++coordit)
			{
			coordit->second = Core::FMTActualDevelopment(coordit->second, 0.0).presolve(filter, presolvedthemes);
			}
	
	}catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTforest::presolve", __LINE__, __FILE__);
	}
	
	
	return newforest;
	}



FMTforest FMTforest::postSolve(const Core::FMTMaskFilter& filter, const std::vector<Core::FMTTheme>&originalbasethemes) const
	{
	FMTforest newforest(*this);
	for (std::map<FMTcoordinate, Core::FMTDevelopment>::iterator coordit = newforest.mapping.begin();
		coordit != newforest.mapping.end(); ++coordit)
		{
		coordit->second.setMask(coordit->second.getMask().postSolve(filter, originalbasethemes));
		}
	return newforest;
	}

}
