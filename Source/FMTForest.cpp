/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTForest.h"
#include "FMTActualDevelopment.h"
#include "FMTFuturDevelopment.h"
#include "FMTTheme.h"
#include "FMTExceptionHandler.h"

namespace Spatial
{
FMTForest::FMTForest():FMTLayer<Core::FMTDevelopment>(){}
FMTForest::FMTForest(const FMTLayer<Core::FMTDevelopment>& rhs):FMTLayer<Core::FMTDevelopment>(rhs){}
FMTForest::FMTForest(const FMTForest& rhs):FMTLayer<Core::FMTDevelopment>(rhs){}
FMTForest& FMTForest::operator = (const FMTForest& rhs)
    {
    if (this!=&rhs)
        {
        FMTLayer<Core::FMTDevelopment>::operator = (rhs);
        }
    return *this;
    }

FMTLayer<std::string>FMTForest::getLock() const
    {
    FMTLayer<std::string>newlayer(this->m_geotransform,this->m_maxx,this->m_maxy,this->m_SRS_WKT,this->m_cellsize);
    for(std::map<FMTCoordinate,Core::FMTDevelopment>::const_iterator it = m_mapping.begin(); it != m_mapping.end(); ++it)
        {
        if (it->second.getLock() > 0 )
            {
            newlayer[it->first] = "_Lock " + std::to_string(it->second.getLock());
            }
        }
    return newlayer;
    }

void FMTForest::setPeriod(int period)
	{
	for (std::map<FMTCoordinate, Core::FMTDevelopment>::iterator it = m_mapping.begin(); it != m_mapping.end(); ++it)
		{
		it->second.setPeriod(period);
		}
	}


std::vector<Core::FMTActualDevelopment>FMTForest::getArea() const
            {
	std::vector<Core::FMTActualDevelopment>devs;
            for(std::map<FMTCoordinate,Core::FMTDevelopment>::const_iterator it = m_mapping.begin(); it != m_mapping.end(); ++it)
                {
                const Core::FMTActualDevelopment newadev(it->second,m_cellsize);
				std::vector<Core::FMTActualDevelopment>::iterator devit = std::find_if(devs.begin(),devs.end(), Core::FMTActualDevelopmentComparator(&newadev));
                if (devit==devs.end())
                    {
                    devs.push_back(newadev);
                    }else{
                    devit->setArea(devit->getArea()+(m_cellsize));
                    }
                }
			
            return devs;
            }
FMTForest FMTForest::grow() const
            {
            FMTForest newforest(*this);
            for(std::map<FMTCoordinate,Core::FMTDevelopment>::const_iterator devit = m_mapping.begin(); devit != m_mapping.end(); ++devit)
                {
                const Core::FMTDevelopment* dev = &devit->second;
                const Core::FMTDevelopment newDev = dev->grow();
                newforest.m_mapping[devit->first] = newDev;
                }
            return newforest;
            }

std::vector<FMTLayer<std::string>> FMTForest::getThemes(const std::vector<Core::FMTTheme>& themes) const
            {
			std::vector<FMTLayer<std::string>> newlayers(themes.size(),FMTLayer<std::string>(this->m_geotransform,this->m_maxx,this->m_maxy,this->m_SRS_WKT,this->m_cellsize));
            for(std::map<FMTCoordinate,Core::FMTDevelopment>::const_iterator it = m_mapping.begin(); it != m_mapping.end(); ++it)
                {
                for(size_t id = 0 ; id < newlayers.size();++id)
                    {
					const std::string value = it->second.getMask().getAttribute(themes[id]);
                    newlayers[id][it->first] = value;
                    }
                }
            return newlayers;
            }
FMTLayer<int>FMTForest::getAge() const
            {
            FMTLayer<int>newlayer(this->m_geotransform,this->m_maxx,this->m_maxy,this->m_SRS_WKT,this->m_cellsize);
            for(std::map<FMTCoordinate,Core::FMTDevelopment>::const_iterator it = m_mapping.begin(); it != m_mapping.end(); ++it)
                {
                newlayer[it->first] = it->second.getAge();
                }
            return newlayer;
            }

FMTForest FMTForest::getCopy(bool copyData) const
    {
    FMTLayer<Core::FMTDevelopment> forest(this->m_geotransform,this->m_maxx,this->m_maxy,this->m_SRS_WKT,this->m_cellsize);
    if(copyData)
        {
        forest = *this;
        }
    return forest;
    }
void  FMTForest::setLastThemeWithArea(const std::vector<Core::FMTActualDevelopment>& p_devs,
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
                    "FMTForest::setLastThemeWithArea", __LINE__, __FILE__);
				}
			subset += new_Masks[subset];
			DevWithLocation.second.setMask(Core::FMTMask(subset, p_themes));
			}
	}catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTForest::setLastThemeWithArea", __LINE__, __FILE__);
	}

}

FMTForest FMTForest::presolve(const Core::FMTMaskFilter& filter, const std::vector<Core::FMTTheme>&presolvedthemes) const
	{
	FMTForest newforest(*this);
	try{
		for (std::map<FMTCoordinate, Core::FMTDevelopment>::iterator coordit = newforest.m_mapping.begin();
			coordit != newforest.m_mapping.end(); ++coordit)
			{
			coordit->second = Core::FMTActualDevelopment(coordit->second, 0.0).presolve(filter, presolvedthemes);
			}
	
	}catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTForest::presolve", __LINE__, __FILE__);
	}
	
	
	return newforest;
	}



FMTForest FMTForest::postSolve(const Core::FMTMaskFilter& filter, const std::vector<Core::FMTTheme>&originalbasethemes) const
	{
	FMTForest newforest(*this);
	for (std::map<FMTCoordinate, Core::FMTDevelopment>::iterator coordit = newforest.m_mapping.begin();
		coordit != newforest.m_mapping.end(); ++coordit)
		{
		coordit->second.setMask(coordit->second.getMask().postSolve(filter, originalbasethemes));
		}
	return newforest;
	}

}
