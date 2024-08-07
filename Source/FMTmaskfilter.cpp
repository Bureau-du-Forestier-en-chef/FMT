/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#include "FMTmaskfilter.h"
#include "FMTtheme.h"
#include "FMTmask.h"

namespace Core{


FMTmaskfilter::FMTmaskfilter():/*fullset(),*/selection(),flippedselection()/*,index()*/{}
FMTmaskfilter::FMTmaskfilter(const FMTmaskfilter& rhs) : /*fullset(rhs.fullset),*/selection(rhs.selection), flippedselection(rhs.flippedselection)//,index(rhs.index)
    {

    }

void FMTmaskfilter::swap(FMTmaskfilter& rhs)
{
	selection.swap(rhs.selection);
	flippedselection.swap(rhs.flippedselection);
}


FMTmaskfilter& FMTmaskfilter::operator = (const FMTmaskfilter& rhs)
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

FMTmaskfilter::FMTmaskfilter(const FMTmask& presolveselection, const FMTmask& buffermask):
	selection(presolveselection.getbitsetreference()),
	flippedselection(buffermask.getbitsetreference())
{

}

FMTmaskfilter::FMTmaskfilter(const FMTmask& presolveselection) :
	selection(presolveselection.getbitsetreference()),
	flippedselection( boost::dynamic_bitset<uint8_t>(presolveselection.getbitsetreference().size(), false))
{

}

bool FMTmaskfilter::canpresolve(const FMTmask& mask, const std::vector<const Core::FMTtheme*>& themes) const
	{
	return !FMTmask(selection).isnotthemessubset(mask, themes);
	}

std::vector<const Core::FMTtheme*> FMTmaskfilter::getselectedthemes(const std::vector<Core::FMTtheme>& themes) const
	{
	return FMTmask(selection).getselectedthemes(themes);
	}

FMTmaskfilter FMTmaskfilter::presolve(const std::vector<FMTtheme>& themes) const
	{
	return FMTmaskfilter(FMTmask(selection).presolve(*this, themes));
	}

FMTmaskfilter::FMTmaskfilter(std::vector<FMTmask>& masks): /*fullset(),*/selection(), flippedselection()//, index()
        {
        flippedselection.resize(masks[0].getbitsetreference().size(),true);
        //std::cout<<"maskbitsize   "<<masks[0].getbitsetreference().size()<<std::endl;
        for(const FMTmask& bits : masks)
            {
            flippedselection &= bits.getbitsetreference();
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
        for (const FMTmask& mask : masks)
            {
            std::cout<<"MASK : "<<std::string(mask)<<std::endl;
            const FMTmask resumemask = filter(mask);
            std::cout<<"Resumemask : "<<std::string(resumemask)<<" "<<resumemask.count()<<" "<<resumemask.size()<<std::endl;
            fullset &= resumemask.getbitsetreference();
            }*/
        }

FMTmaskfilter::FMTmaskfilter(std::vector<FMTmask>& masks,const std::vector<FMTtheme>& themes):/*fullset(),*/ selection(), flippedselection()//, index()
    {
    *this = FMTmaskfilter(masks);
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
    /*for(const FMTtheme& the : themes)
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
FMTmask FMTmaskfilter::filter(const FMTmask& devmask) const
        {
        if (!selection.empty())
            {
            const boost::dynamic_bitset<uint8_t> newkey = ((devmask.getbitsetreference() & selection) | flippedselection);
            return FMTmask(devmask.getstringreference(),newkey);
			//return newmask.resume(index);
            }
        return devmask;
        }

 /*bool FMTmaskfilter::within(const FMTmask& intersect) const
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
        if (intersect.getbitsetreference()[location])
            {
            good = true;
            }
        lastid = in;
        ++location;
        }
    return true;
    }*/


}


