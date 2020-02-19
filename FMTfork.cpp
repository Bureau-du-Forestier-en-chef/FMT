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

#include "FMTfork.h"


namespace Core{

FMTfork::FMTfork():FMTspec(),transitions(){}
    FMTfork::FMTfork(const FMTfork& rhs):FMTspec(rhs), transitions(rhs.transitions)
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

	FMTfork FMTfork::presolve(const FMTmask& selectedmask, const std::vector<FMTtheme>&presolvedthemes) const
		{
		FMTfork newfork(*this);
		for (FMTtransitionmask& trmask : newfork.transitions)
			{
			trmask = trmask.presolve(selectedmask, presolvedthemes);
			}
		return newfork;
		}

	std::vector<FMTdevelopmentpath> FMTfork::getpaths(const FMTdevelopment& base,
		 const FMTyields& ylds, const std::vector<FMTtheme>& themes, const bool& reset_age) const
		{
		std::vector<FMTdevelopmentpath>paths(transitions.size());
		int id = 0;
		for (const FMTtransitionmask& tran : transitions)
			{
			 const FMTdevelopmentpath newpath(tran.disturb(base, ylds, themes,reset_age), tran.getproportion());
			 paths[id] = newpath;
			 ++id;
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
