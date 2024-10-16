/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTfork.h"
#include "FMTdevelopment.h"
#include "FMTdevelopmentpath.h"
#include "FMTyields.h"
#include "FMTtransitionmask.h"
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/unordered_map.hpp>


namespace Core{

    FMTfork::~FMTfork() = default;




FMTfork::FMTfork():FMTspec(),FMTobject(), transitions()
    {


    }
    FMTfork::FMTfork(const FMTfork& rhs):
        FMTspec(rhs),
        FMTobject(rhs),
        transitions(rhs.transitions)
        {


        }
    FMTfork& FMTfork::operator = (const FMTfork& rhs)
        {
        if (this!=&rhs)
            {
            FMTspec::operator=(rhs);
            FMTobject::operator=(rhs);
            transitions = rhs.transitions;
            }
        return *this;
        }
    void FMTfork::add(const FMTtransitionmask& transition)
        {
        transitions.push_back(transition);
        }
    void FMTfork::clear()
    {
        transitions.clear();
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
        newfork.presolveRef(filter, presolvedthemes);
        return newfork;
		}

    void FMTfork::presolveRef(const FMTmaskfilter& filter, const std::vector<FMTtheme>& presolvedthemes)
        {
        for (FMTtransitionmask& trmask : transitions)
            {
            trmask.presolveRef(filter, presolvedthemes);
            }
        }

	std::vector<FMTdevelopmentpath> FMTfork::getpaths(const Core::FMTdevelopment& base,const Core::FMTyields& ylds,
		const std::vector<FMTtheme>& themes, const bool& reset_age) const
		{
        std::vector<FMTdevelopmentpath>paths;
        try{
        paths.reserve(transitions.size());
        boost::unordered_map<Core::FMTdevelopment,size_t>pathmap;
        bool keeptags = false;
        if (size()>1)
            {
            keeptags = true;
            pathmap.reserve(transitions.size());
            }   
        size_t pathid = 0;
        for (const FMTtransitionmask& tran : transitions)
        {
           Core::FMTdevelopment newdev = tran.disturb(base, ylds, themes, reset_age);
           if (!reset_age && base==newdev)
                {
                _exhandler->raise(Exception::FMTexc::FMTsourcetotarget_transition,
                    "from "+ std::string(base) +" to "+ std::string(newdev) + "\n",
                    "FMTfork::getpaths", __LINE__, __FILE__, Core::FMTsection::Transition);
                newdev.setlock(newdev.getlock()+1);
                }
           if (keeptags)
                {
                   boost::unordered_map<Core::FMTdevelopment, size_t>::const_iterator mapit = pathmap.find(newdev);
                   if (mapit != pathmap.end())
                        {
                       _exhandler->raise(Exception::FMTexc::FMTsame_transitiontargets,
                           "from " + std::string(base) + " to " + std::string(newdev) + "\n",
                           "FMTfork::getpaths", __LINE__, __FILE__, Core::FMTsection::Transition);
                       paths[mapit->second].setProportion(paths[mapit->second].getProportion() + tran.getproportion());
                       continue;
                   }else {
                       pathmap[newdev] = pathid;
                       }
                }
            paths.emplace_back(newdev,tran.getproportion());
            ++pathid;
        }
        }catch (...)
            {
            _exhandler->raisefromcatch("", "FMTfork::getpaths", __LINE__, __FILE__, Core::FMTsection::Transition);
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
