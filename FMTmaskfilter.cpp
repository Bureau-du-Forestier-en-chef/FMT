
#include "FMTmaskfilter.h"
/*
namespace Core{


FMTmaskfilter::FMTmaskfilter():selection(),flippedselection(),index(){}
FMTmaskfilter::FMTmaskfilter(const FMTmaskfilter& rhs) : selection(rhs.selection), flippedselection(rhs.flippedselection), index(rhs.index)
    {

    }


FMTmaskfilter& FMTmaskfilter::operator = (const FMTmaskfilter& rhs)
    {
    if (this!=&rhs)
        {
        selection = rhs.selection;
        flippedselection = rhs.flippedselection;
        index = rhs.index;
        }
    return *this;
    }
FMTmaskfilter::FMTmaskfilter(vector<FMTmask>& masks): selection(), flippedselection(), index()
        {

        flippedselection.resize(masks[0].size(),true);
        for(FMTmask bits : masks)
            {
            flippedselection.swap(flippedselection &= bits);
            }

        selection = flippedselection;
        selection.flip();
        }

FMTmaskfilter::FMTmaskfilter(vector<FMTmask>& masks,const vector<FMTtheme>& themes): selection(), flippedselection(), index()
    {
    *this = FMTmaskfilter(masks);
    index.resize(selection.count());
    int id = 0;
    int indexloc = 0;
    int start = 0;
    for(const FMTtheme& the : themes)
        {
        for(int loc = start; loc <  (start + int(the.size()));++loc)
            {
            if (selection[loc])
                {
                index[indexloc] = id;
                ++indexloc;
                }
            }
        start+=int(the.size());
        ++id;
        }
    }
FMTmask FMTmaskfilter::filter(const FMTmask& devmask) const
        {
        if (!selection.empty())
            {
            boost::dynamic_bitset<> newkey = ((devmask & selection) | flippedselection);
            FMTmask newmask(devmask.getstr(),newkey);
            FMTmask resumemask = newmask.resume(selection);
            return resumemask;
            }
        return devmask;
        }

 bool FMTmaskfilter::within(const FMTmask& intersect)
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
        if (intersect[location])
            {
            good = true;
            }
        lastid = in;
        ++location;
        }
    return true;
    }

}
*/
namespace Core{


FMTmaskfilter::FMTmaskfilter():fullset(),selection(),flippedselection(),index(){}
FMTmaskfilter::FMTmaskfilter(const FMTmaskfilter& rhs) : fullset(rhs.fullset),selection(rhs.selection), flippedselection(rhs.flippedselection), index(rhs.index)
    {

    }


FMTmaskfilter& FMTmaskfilter::operator = (const FMTmaskfilter& rhs)
    {
    if (this!=&rhs)
        {
		fullset = rhs.fullset;
        selection = rhs.selection;
        flippedselection = rhs.flippedselection;
        index = rhs.index;
        }
    return *this;
    }
FMTmaskfilter::FMTmaskfilter(vector<FMTmask>& masks): fullset(),selection(), flippedselection(), index()
        {
        flippedselection.resize(masks[0].data.size(),true);
        for(FMTmask bits : masks)
            {
            flippedselection.swap(flippedselection &= bits.data);

            }

        selection = flippedselection;
        selection.flip();
        }

FMTmaskfilter::FMTmaskfilter(vector<FMTmask>& masks,const vector<FMTtheme>& themes):fullset(), selection(), flippedselection(), index()
    {
    *this = FMTmaskfilter(masks);
    index.resize(selection.count());
    int id = 0;
    int indexloc = 0;
    int start = 0;
    for(const FMTtheme& the : themes)
        {
        for(int loc = start; loc <  (start + int(the.size()));++loc)
            {
            if (selection[loc])
                {
                index[indexloc] = id;
                ++indexloc;
                }
            }
        start+=int(the.size());
        ++id;
        }
	fullset.resize(selection.count(), true);
	for (const FMTmask& mask : masks)
		{
		FMTmask resumemask = filter(mask);
		fullset &= resumemask.data;
		}
    }
FMTmask FMTmaskfilter::filter(const FMTmask& devmask) const
        {
        if (!selection.empty())
            {
            boost::dynamic_bitset<> newkey = ((devmask.data & selection) | flippedselection);
            FMTmask newmask(string(devmask),newkey);
            FMTmask resumemask = newmask.resume(selection);
			if (resumemask.data.is_subset_of(fullset))
				{
				return resumemask;
			}else {
				return FMTmask();
				}
            }
        return devmask;
        }

 bool FMTmaskfilter::within(const FMTmask& intersect)
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
        if (intersect.data[location])
            {
            good = true;
            }
        lastid = in;
        ++location;
        }
    return true;
    }

 size_t FMTmaskfilter::hash() const
	{
	return (boost::hash<boost::dynamic_bitset<>>()(selection) ^ boost::hash<boost::dynamic_bitset<>>()(flippedselection));
	}

}


