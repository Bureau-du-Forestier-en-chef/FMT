/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#include "FMTMaskFilter.h"
#include "FMTTheme.h"
#include "FMTMask.h"

namespace Core{


FMTMaskFilter::FMTMaskFilter():/*fullset(),*/selection(),flippedselection()/*,index()*/{}
FMTMaskFilter::FMTMaskFilter(const FMTMaskFilter& rhs) : /*fullset(rhs.fullset),*/selection(rhs.selection), flippedselection(rhs.flippedselection)//,index(rhs.index)
    {

    }

void FMTMaskFilter::swap(FMTMaskFilter& rhs)
{
	selection.swap(rhs.selection);
	flippedselection.swap(rhs.flippedselection);
}


FMTMaskFilter& FMTMaskFilter::operator = (const FMTMaskFilter& rhs)
    {
    if (this!=&rhs)
        {
		//fullset = rhs.fullset;
        selection = rhs.selection;
        flippedselection = rhs.flippedselection;
        //index = rhs.index;
        }
    return *this;
    }

FMTMaskFilter::FMTMaskFilter(const FMTMask& presolveselection, const FMTMask& buffermask):
	selection(presolveselection.getBitsetReference()),
	flippedselection(buffermask.getBitsetReference())
{

}

FMTMaskFilter::FMTMaskFilter(const FMTMask& presolveselection) :
	selection(presolveselection.getBitsetReference()),
	flippedselection( boost::dynamic_bitset<uint8_t>(presolveselection.getBitsetReference().size(), false))
{

}

bool FMTMaskFilter::canPresolve(const FMTMask& mask, const std::vector<const Core::FMTTheme*>& themes) const
	{
	return !FMTMask(selection).isNotThemesSubset(mask, themes);
	}

std::vector<const Core::FMTTheme*> FMTMaskFilter::getSelectedThemes(const std::vector<Core::FMTTheme>& themes) const
	{
	return FMTMask(selection).getSelectedThemes(themes);
	}

FMTMaskFilter FMTMaskFilter::presolve(const std::vector<FMTTheme>& themes) const
	{
	return FMTMaskFilter(FMTMask(selection).presolve(*this, themes));
	}

FMTMaskFilter::FMTMaskFilter(std::vector<FMTMask>& masks): /*fullset(),*/selection(), flippedselection()//, index()
        {
        flippedselection.resize(masks[0].getBitsetReference().size(),true);
        //std::cout<<"maskbitsize   "<<masks[0].getBitsetReference().size()<<std::endl;
        for(const FMTMask& bits : masks)
            {
            flippedselection &= bits.getBitsetReference();
            }
        selection = flippedselection;
        selection.flip();
        if(!selection.any())
        {
            flippedselection = boost::dynamic_bitset<uint8_t>(flippedselection.size(),false);
            selection = boost::dynamic_bitset<uint8_t>(flippedselection.size(),true);
			
        }
		/*int indexloc = 0;
		index.resize(selection.count());
		for (size_t location = 0; location < selection.size(); ++location)
		{
			if (selection[location])
			{
				index[indexloc] = location;
				++indexloc;
			}
		}*/
        /*fullset.resize(selection.count(), true);
        std::cout<<"Fullset   "<<fullset.count()<<" "<<fullset.size()<<std::endl;
        std::cout<<"Selection   "<<selection.count()<<" "<<selection.size()<<std::endl;
        std::cout<<"FLIPSelection   "<<flippedselection.count()<<" "<<flippedselection.size()<<std::endl;
        for (const FMTMask& mask : masks)
            {
            std::cout<<"MASK : "<<std::string(mask)<<std::endl;
            const FMTMask resumemask = filter(mask);
            std::cout<<"Resumemask : "<<std::string(resumemask)<<" "<<resumemask.count()<<" "<<resumemask.size()<<std::endl;
            fullset &= resumemask.getBitsetReference();
            }*/
        }

FMTMaskFilter::FMTMaskFilter(std::vector<FMTMask>& masks,const std::vector<FMTTheme>& themes):/*fullset(),*/ selection(), flippedselection()//, index()
    {
    *this = FMTMaskFilter(masks);
    /*index.resize(selection.count());
    //int id = 0;
    int indexloc = 0;
    //int start = 0;
	for (size_t location = 0;location<selection.size();++location)
		{
		if (selection[location])
			{
			index[indexloc] = location;
			++indexloc;
			}
		}*/
    /*for(const FMTTheme& the : themes)
        {
        for(int loc = start; loc <  (start + static_cast<int>(the.size()));++loc)
            {
            if (selection[loc])
                {
                index[indexloc] = id;
                ++indexloc;
                }
            }
        start+=static_cast<int>(the.size());
        ++id;
        }*/
    }
FMTMask FMTMaskFilter::filter(const FMTMask& devmask) const
        {
        if (!selection.empty())
            {
            const boost::dynamic_bitset<uint8_t> newkey = ((devmask.getBitsetReference() & selection) | flippedselection);
            return FMTMask(devmask.getStringReference(),newkey);
			//return newmask.resume(index);
            }
        return devmask;
        }

 /*bool FMTMaskFilter::within(const FMTMask& intersect) const
    {
    int location = 0;
    int lastid = 0;
    bool good = false;
    for(const int& in : index)
        {
        if (lastid != in)
            {
            if (!good)
                {
                return false;
                }else{
                good = false;
                }
            }
        if (intersect.getBitsetReference()[location])
            {
            good = true;
            }
        lastid = in;
        ++location;
        }
    return true;
    }*/


}


