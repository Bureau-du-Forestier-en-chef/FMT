/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTfork.hpp"
#include "FMTdevelopment.hpp"
#include "FMTdevelopmentpath.hpp"
#include "FMTyields.hpp"
#include "FMTtransitionmask.hpp"
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>


namespace Core{

    FMTfork::~FMTfork() = default;




FMTfork::FMTfork():FMTspec(),transitions()
    {


    }
    FMTfork::FMTfork(const FMTfork& rhs):
        FMTspec(rhs),
        transitions(rhs.transitions)
        {


        }
    FMTfork& FMTfork::operator = (const FMTfork& rhs)
        {
        if (this!=&rhs)
            {
            FMTspec::operator=(rhs);
            transitions = rhs.transitions;
            }
        return *this;
        }
    void FMTfork::add(FMTtransitionmask& transition)
        {
        transitions.push_back(transition);
        }
     double FMTfork::sumprop() const
        {
        double total = 0;
        for(const FMTtransitionmask& transition : transitions)
            {
            total+=transition.getproportion();
            }
        return total;
        }
    size_t FMTfork::size() const
        {
        return transitions.size();
        }
    FMTfork::operator std::string() const
        {
		std::string line;
        line+=" "+FMTspec::operator std::string();
        line+="\n";
        for(const FMTtransitionmask& transition : transitions)
            {
            line+= std::string(transition);
            line+="\n";
            }
        return line;
        }

	FMTfork FMTfork::presolve(const FMTmaskfilter& filter, const std::vector<FMTtheme>&presolvedthemes) const
		{
		FMTfork newfork(*this);
		for (FMTtransitionmask& trmask : newfork.transitions)
			{
			trmask = trmask.presolve(filter, presolvedthemes);
			}
		return newfork;
		}

	std::vector<FMTdevelopmentpath> FMTfork::getpaths(const Core::FMTdevelopment& base,const Core::FMTyields& ylds,
		const std::vector<FMTtheme>& themes, const bool& reset_age) const
		{
		std::vector<FMTdevelopmentpath>paths;
		paths.reserve(transitions.size());
		for (const FMTtransitionmask& tran : transitions)
			{
			paths.emplace_back(tran.disturb(base, ylds, themes, reset_age), tran.getproportion());
			}
		 return paths;
		}

	std::vector<FMTtransitionmask> FMTfork::getmasktrans() const
        {
        return transitions;
        }

     FMTfork FMTfork::single() const
        {
        FMTfork newfork(*this);
        newfork.transitions.clear();
        double lastproportion = 0;
        FMTtransitionmask singletrans;
        for(const FMTtransitionmask& tran : transitions)
            {
            const double proportion = tran.getproportion();
            if (proportion > lastproportion)
                {
                singletrans = tran;
				lastproportion = proportion;
                }
            }
        singletrans.setproportion(100);
        newfork.transitions.push_back(singletrans);
        return newfork;
        }

    FMTdevelopment FMTfork::getmax(const FMTdevelopment& base,const FMTyields& ylds,const std::vector<FMTtheme>& themes,const bool& reset_age) const
        {
        double lastproportion = 0;
        FMTdevelopment dev;
        for(const FMTtransitionmask& tran : transitions)
            {
            const double proportion = tran.getproportion();
            if (proportion > lastproportion)
                {
                dev = tran.disturb(base,ylds,themes,reset_age);
                }
            }
        return dev;
        }

	bool FMTfork::operator == (const FMTfork& rhs) const
		{
        return (FMTspec::operator == (rhs) &&
            transitions == rhs.transitions);
            
		}
}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTfork)
