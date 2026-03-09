/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTtransitionmask.h"
#include "FMTyieldrequest.h"
#include "FMTtheme.h"
#include "FMTyields.h"
#include "FMTdevelopmentpath.h"

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
    FMTmask FMTtransitionmask::trans(const FMTmask& p_baseMask,
                            const std::vector<FMTtheme>& p_themes) const
        {
        FMTmask newMask(((flippedselection & p_baseMask.getbitsetreference()) | selection));
        if (!p_themes.empty())
            {
            newMask.update(p_themes);
            }
        return newMask;
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
    FMTdevelopmentpath FMTtransitionmask::disturb(const Core::FMTdevelopment& dev,const FMTyields& yields,
                                                const std::vector<FMTtheme>& themes, const bool& reset_age) const
        {
        FMTdevelopmentpath newPath(this->trans(dev.getmask(), themes),
                    dev.getage(), dev.getlock(), dev.getperiod(), proportion);
        FMTdevelopment& newdev = newPath.getDevelopmentReference();
		bool age_change = false;
        if (!lock.empty())
            {
			const int lower_lock = lock.getlower();
            newdev.setlock(std::max(lower_lock, newdev.getlock()));
            }
        if(!age.empty())
            {
            newdev.setage(age.getlower());
			age_change = true;
            }else if(!yieldnames.empty())
                {
				const FMTyieldrequest newrequest = newdev.getyieldrequest();
                newdev.setage(yields.getage(newrequest, *this));
				age_change = true;
                }
		if (reset_age && !age_change)
			{
			newdev.setage(0);
			}
        return newPath;
        }

    void FMTtransitionmask::setproportion(double newproportion)
        {
        proportion  = std::min(newproportion,100.00);
        }

	FMTtransitionmask FMTtransitionmask::presolve(const FMTmaskfilter& filter, const std::vector<FMTtheme>&presolvedthemes) const
		{
		FMTtransitionmask newtransitionmask(*this);
        newtransitionmask.presolveRef(filter, presolvedthemes);
		return newtransitionmask;
		}

    void FMTtransitionmask::presolveRef(const FMTmaskfilter& filter, const std::vector<FMTtheme>& presolvedthemes)
        {
        mask.presolveRef(filter, presolvedthemes, false);
        flippedselection.clear();
        build(mask, presolvedthemes);
        }

    double FMTtransitionmask::getproportion() const
        {
            return proportion;
        }

    FMTmask FMTtransitionmask::getmask() const
        {
            return mask;
        }

    void FMTtransitionmask::setmask(const Core::FMTmask& mtmask)
        {
        mask = mtmask;
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
