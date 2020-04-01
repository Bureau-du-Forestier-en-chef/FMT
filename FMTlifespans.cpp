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

#include "FMTlifespans.h"

namespace Core{

FMTlifespans::FMTlifespans() : FMTlist<int>(){}
FMTlifespans::FMTlifespans(const FMTlifespans& rhs) : FMTlist<int>(rhs){}
FMTlifespans& FMTlifespans::operator = (const FMTlifespans& rhs)
    {
    if (this!=&rhs)
        {
		FMTlist<int>::operator = (rhs);
        }
    return *this;
    }

void FMTlifespans::update()
	{
	FMTlist<int>::update();
	}

FMTlifespans FMTlifespans::presolve(const FMTmask& basemask,
	const std::vector<FMTtheme>& originalthemes,
	const FMTmask& presolvedmask,
	const std::vector<FMTtheme>& newthemes) const
	{
	FMTlifespans newlifespans(*this);
	newlifespans.presolvelist(basemask, originalthemes, presolvedmask, newthemes);
	newlifespans.update();
	return newlifespans;
	}

bool FMTlifespans::operator == (const FMTlifespans& rhs) const
	{
	return (FMTlist<int>::operator ==(rhs));
	}

FMTlifespans::operator std::string() const
    {
	std::string line;
    for(const auto& lfobject : *this)
        {
        line+= std::string(lfobject.first)+" ";
        line+= std::to_string(lfobject.second);
        line+="\n";
        }
    return line;
    }
}
