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

#include "FMTtransitionmask.h"

namespace Core{

void FMTtransitionmask::build(const std::string& lmask, const std::vector<FMTtheme>& themes )
    {
    int id = 0;
	std::vector<std::string>bases;
    boost::split(bases,lmask,boost::is_any_of(FMT_STR_SEPARATOR), boost::token_compress_on);
    for(const FMTtheme& theme : themes)
        {
        if (theme.isattribute(bases[id]))
            {
            flippedselection.resize(theme.size()+flippedselection.size(),false);
            }else{
            flippedselection.resize(theme.size()+flippedselection.size(),true);
            }
        ++id;
        }
    selection = (flippedselection ^ mask.getbitsetreference());
    }

FMTtransitionmask::FMTtransitionmask():FMTmaskfilter(),FMTspec(),mask(),proportion()
    {

    }
FMTtransitionmask::FMTtransitionmask(const std::string& lmask,const std::vector<FMTtheme>& themes,const double& lproportion):FMTmaskfilter(),FMTspec(),
        mask(lmask,themes),proportion(lproportion)
        {
        this->build(lmask,themes);
        }
    FMTmask FMTtransitionmask::trans(const FMTmask& basemask) const
        {
        return FMTmask(((flippedselection & basemask.getbitsetreference()) | selection));
        }
    FMTtransitionmask::FMTtransitionmask(const FMTtransitionmask& rhs) :FMTmaskfilter(rhs),FMTspec(rhs) , mask(rhs.mask),proportion(rhs.proportion)
        {

        }
    FMTtransitionmask::FMTtransitionmask(const FMTtransitionmask& rhs,const FMTmask& lmask,const std::vector<FMTtheme>& themes):FMTmaskfilter(),
                FMTspec(rhs),mask(lmask),proportion(rhs.proportion)
        {
        this->build(std::string(lmask),themes);
        }
    FMTtransitionmask& FMTtransitionmask::operator = (const FMTtransitionmask& rhs)
        {
        if (this!=&rhs)
            {
            proportion = rhs.proportion;
            mask = rhs.mask;
            FMTspec::operator = (rhs);
            FMTmaskfilter::operator = (rhs);
            }
        return *this;
        }
    FMTtransitionmask::operator std::string() const
        {
		std::string line;
        line += "*TARGET " + std::string(mask) + " " + std::to_string(proportion) + " " + FMTspec::operator std::string();
        return line;
        }
    FMTdevelopment FMTtransitionmask::disturb(const FMTdevelopment& dev,const FMTyields& yields,const std::vector<FMTtheme>& themes, const bool& reset_age) const
        {
        FMTdevelopment newdev(dev);
        newdev.mask = this->trans(dev.mask);
        newdev.lock = 0;
		bool age_change = false;
        if(!lock.empty())
            {
			const int lower_lock = lock.getlower();
            newdev.lock = lower_lock;
            }
        if(!age.empty())
            {
            newdev.age = age.getlower();
			age_change = true;
            }else if(!ylds.empty())
                {
                newdev.age = yields.getage(newdev,*this);
				age_change = true;
                }
        if(!themes.empty())
            {
            newdev.mask.update(themes);
            }
		if (reset_age && !age_change)
			{
			newdev.age = 0;
			}
        return newdev;
        }

    void FMTtransitionmask::setproportion(double newproportion)
        {
        proportion  = std::min(newproportion,100.00);
        }

	FMTtransitionmask FMTtransitionmask::presolve(const FMTmask& selectedmask, const std::vector<FMTtheme>&presolvedthemes) const
		{
		FMTtransitionmask newtransitionmask(*this);
		newtransitionmask.mask = newtransitionmask.mask.presolve(selectedmask, presolvedthemes);
		newtransitionmask.flippedselection.clear();
		newtransitionmask.build(newtransitionmask.mask, presolvedthemes);
		return newtransitionmask;
		}

    double FMTtransitionmask::getproportion() const
        {
            return proportion;
        }

    FMTmask FMTtransitionmask::getmask() const
        {
            return mask;
        }

	std::map<std::string, std::string>FMTtransitionmask::get(const std::vector<FMTtheme>& themes) const
        {
		std::map<std::string, std::string>mapping;
        int id = 1;
        for(const FMTtheme& theme : themes)
            {
            const std::string value = mask.get(theme);
			std::string name = theme.getname();
            if (name.empty())
                {
                name = "THEME_"+ std::to_string(id);
                }
            mapping[name] = value;
            ++id;
            }
        return mapping;
        }

	bool FMTtransitionmask::operator == (const FMTtransitionmask& rhs) const
		{
		return (FMTspec::operator== (rhs) &&
			mask == rhs.mask &&
			proportion == rhs.proportion);
		}
}
