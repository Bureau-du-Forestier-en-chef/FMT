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


#include "FMTmaskfilter.h"
#include "FMTtheme.h"

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
FMTmaskfilter::FMTmaskfilter(std::vector<FMTmask>& masks): fullset(),selection(), flippedselection(), index()
        {
        flippedselection.resize(masks[0].getbitsetreference().size(),true);
        for(const FMTmask& bits : masks)
            {
            flippedselection.swap(flippedselection &= bits.getbitsetreference());

            }
        selection = flippedselection;
        selection.flip();
        }

FMTmaskfilter::FMTmaskfilter(std::vector<FMTmask>& masks,const std::vector<FMTtheme>& themes):fullset(), selection(), flippedselection(), index()
    {
    *this = FMTmaskfilter(masks);
    index.resize(selection.count());
    int id = 0;
    int indexloc = 0;
    int start = 0;
    for(const FMTtheme& the : themes)
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
        }
	fullset.resize(selection.count(), true);
	for (const FMTmask& mask : masks)
		{
		const FMTmask resumemask = filter(mask);
		fullset &= resumemask.getbitsetreference();
		}
    }
FMTmask FMTmaskfilter::filter(const FMTmask& devmask) const
        {
        if (!selection.empty())
            {
            const boost::dynamic_bitset<> newkey = ((devmask.getbitsetreference() & selection) | flippedselection);
            const FMTmask newmask(std::string(devmask),newkey);
            const FMTmask resumemask = newmask.resume(selection);
			if (resumemask.issubsetof(fullset))
				{
				return resumemask;
			}else {
				return FMTmask();
				}
            }
        return devmask;
        }

 bool FMTmaskfilter::within(const FMTmask& intersect) const
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
    }


}


