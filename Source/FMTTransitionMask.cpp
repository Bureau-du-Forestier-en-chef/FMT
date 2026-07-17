/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTTransitionMask.h"
#include "FMTYieldRequest.h"
#include "FMTTheme.h"
#include "FMTYields.h"
#include "FMTDevelopmentPath.h"

namespace Core{

void FMTTransitionMask::build(const std::string& lmask, const std::vector<FMTTheme>& themes )
    {
    int id = 0;
	std::vector<std::string>bases;
    boost::split(bases,lmask,boost::is_any_of(FMT_STR_SEPARATOR), boost::token_compress_on);
    for(const FMTTheme& theme : themes)
        {
        if (theme.isAttribute(bases[id]))
            {
            flippedselection.resize(theme.size()+flippedselection.size(),false);
            }else{
            flippedselection.resize(theme.size()+flippedselection.size(),true);
            }
        ++id;
        }
    selection = (flippedselection ^ mask.getBitsetReference());
    }

FMTTransitionMask::FMTTransitionMask():FMTMaskFilter(),FMTSpec(),mask(),proportion()
    {

    }
FMTTransitionMask::FMTTransitionMask(const std::string& lmask,const std::vector<FMTTheme>& themes,const double& lproportion):FMTMaskFilter(),FMTSpec(),
        mask(lmask,themes),proportion(lproportion)
        {
        this->build(lmask,themes);
        }
    FMTMask FMTTransitionMask::trans(const FMTMask& p_baseMask,
                            const std::vector<FMTTheme>& p_themes) const
        {
        FMTMask newMask(((flippedselection & p_baseMask.getBitsetReference()) | selection));
        if (!p_themes.empty())
            {
            newMask.update(p_themes);
            }
        return newMask;
        }
    FMTTransitionMask::FMTTransitionMask(const FMTTransitionMask& rhs) :FMTMaskFilter(rhs),FMTSpec(rhs) , mask(rhs.mask),proportion(rhs.proportion)
        {

        }
    FMTTransitionMask::FMTTransitionMask(const FMTTransitionMask& rhs,const FMTMask& lmask,const std::vector<FMTTheme>& themes):FMTMaskFilter(),
                FMTSpec(rhs),mask(lmask),proportion(rhs.proportion)
        {
        this->build(std::string(lmask),themes);
        }
    FMTTransitionMask& FMTTransitionMask::operator = (const FMTTransitionMask& rhs)
        {
        if (this!=&rhs)
            {
            proportion = rhs.proportion;
            mask = rhs.mask;
            FMTSpec::operator = (rhs);
            FMTMaskFilter::operator = (rhs);
            }
        return *this;
        }
    FMTTransitionMask::operator std::string() const
        {
		std::string line;
        line += "*TARGET " + std::string(mask) + " " + std::to_string(proportion) + " " + FMTSpec::operator std::string();
        return line;
        }
    FMTDevelopmentPath FMTTransitionMask::disturb(const Core::FMTDevelopment& dev,const FMTYields& yields,
                                                const std::vector<FMTTheme>& themes, const bool& reset_age) const
        {
        FMTDevelopmentPath newPath(this->trans(dev.getMask(), themes),
                    dev.getAge(), dev.getLock(), dev.getPeriod(), proportion);
        FMTDevelopment& newDev = newPath.getDevelopmentReference();
		bool age_change = false;
        if (!lock.empty())
            {
			const int lower_lock = lock.getLower();
            newDev.setLock(std::max(lower_lock, newDev.getLock()));
            }
        if(!age.empty())
            {
            newDev.setAge(age.getLower());
			age_change = true;
            }else if(!yieldnames.empty())
                {
				const FMTYieldRequest newrequest = newDev.getYieldRequest();
                newDev.setAge(yields.getAge(newrequest, *this));
				age_change = true;
                }
		if (reset_age && !age_change)
			{
			newDev.setAge(0);
			}
        return newPath;
        }

    void FMTTransitionMask::setProportion(double newproportion)
        {
        proportion  = std::min(newproportion,100.00);
        }

	FMTTransitionMask FMTTransitionMask::presolve(const FMTMaskFilter& filter, const std::vector<FMTTheme>&presolvedthemes) const
		{
		FMTTransitionMask newtransitionmask(*this);
        newtransitionmask.presolveRef(filter, presolvedthemes);
		return newtransitionmask;
		}

    void FMTTransitionMask::presolveRef(const FMTMaskFilter& filter, const std::vector<FMTTheme>& presolvedthemes)
        {
        mask.presolveRef(filter, presolvedthemes, false);
        flippedselection.clear();
        build(mask, presolvedthemes);
        }

    double FMTTransitionMask::getProportion() const
        {
            return proportion;
        }

    FMTMask FMTTransitionMask::getMask() const
        {
            return mask;
        }

    void FMTTransitionMask::setMask(const Core::FMTMask& mtmask)
        {
        mask = mtmask;
        }

	std::map<std::string, std::string>FMTTransitionMask::get(const std::vector<FMTTheme>& themes) const
        {
		std::map<std::string, std::string>mapping;
        int id = 1;
        for(const FMTTheme& theme : themes)
            {
            const std::string value = mask.get(theme);
			std::string name = theme.getName();
            if (name.empty())
                {
                name = "THEME_"+ std::to_string(id);
                }
            mapping[name] = value;
            ++id;
            }
        return mapping;
        }

	bool FMTTransitionMask::operator == (const FMTTransitionMask& rhs) const
		{
		return (FMTSpec::operator== (rhs) &&
			mask == rhs.mask &&
			proportion == rhs.proportion);
		}
}
