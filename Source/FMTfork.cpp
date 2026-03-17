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




FMTfork::FMTfork():FMTspec(),FMTobject(), m_transitions()
    {


    }

    void FMTfork::add(const FMTtransitionmask& transition)
        {
        m_transitions.push_back(transition);
        }
    void FMTfork::clear()
    {
        m_transitions.clear();
    }


     double FMTfork::sumprop() const
        {
        double total = 0;
        for(const FMTtransitionmask& transition : m_transitions)
            {
            total+=transition.getproportion();
            }
        return total;
        }
    size_t FMTfork::size() const
        {
        return m_transitions.size();
        }
    FMTfork::operator std::string() const
        {
		std::string line;
        line+=" "+FMTspec::operator std::string();
        line+="\n";
        for(const FMTtransitionmask& transition : m_transitions)
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
        for (FMTtransitionmask& trmask : m_transitions)
            {
            trmask.presolveRef(filter, presolvedthemes);
            }
        }

    FMTdevelopmentpath FMTfork::_GetPath(const FMTtransitionmask& p_target, 
        const Core::FMTdevelopment& p_base, const Core::FMTyields& p_yields,
        const std::vector<FMTtheme>& p_themes, bool p_AgeReset) const
    {
        try {
            FMTdevelopmentpath newPath = p_target.disturb(p_base, p_yields, p_themes, p_AgeReset);
            FMTdevelopment& newdev = newPath.getDevelopmentReference();
            if (!p_AgeReset && p_base == newdev)
                {
                _exhandler->raise(Exception::FMTexc::FMTsourcetotarget_transition,
                    "from " + std::string(p_base) + " to " + std::string(newdev) + "\n",
                    "FMTfork::_GetPath", __LINE__, __FILE__, Core::FMTsection::Transition);
                newdev.setlock(newdev.getlock() + 1);
                }
            return newPath;
        }catch (...)
            {
            _exhandler->raisefromcatch("", "FMTfork::_GetPath", __LINE__, __FILE__,
                Core::FMTsection::Transition);
            }
        return FMTdevelopmentpath();
    }

	std::vector<FMTdevelopmentpath> FMTfork::getpaths(const Core::FMTdevelopment& base,const Core::FMTyields& ylds,
		const std::vector<FMTtheme>& themes, const bool& reset_age) const
		{
        std::vector<FMTdevelopmentpath>paths;
        try{
            paths.reserve(size());
        if (size() == 1)
            {
            paths.push_back(_GetPath(*m_transitions.begin(), base, ylds,
                themes, reset_age));
        }else {
            boost::unordered_map<Core::FMTdevelopment, size_t>pathmap;
            size_t pathid = 0;
            for (const FMTtransitionmask& tran : m_transitions)
                {
                const FMTdevelopmentpath NEW_PATH = _GetPath(tran, base, ylds,
                    themes, reset_age);
                const FMTdevelopment& newdev = NEW_PATH.getDevelopment();
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
                paths.push_back(NEW_PATH);
                ++pathid;
                }
            }
        }catch (...)
            {
            _exhandler->raisefromcatch("", "FMTfork::getpaths",
                __LINE__, __FILE__, Core::FMTsection::Transition);
             }
        return paths;
		}

	std::vector<FMTtransitionmask> FMTfork::getmasktrans() const
        {
        return m_transitions;
        }

     FMTfork FMTfork::single() const
        {
        FMTfork newfork(*this);
        newfork.m_transitions.clear();
        double lastproportion = 0;
        FMTtransitionmask singletrans;
        for(const FMTtransitionmask& tran : m_transitions)
            {
            const double proportion = tran.getproportion();
            if (proportion > lastproportion)
                {
                singletrans = tran;
				lastproportion = proportion;
                }
            }
        singletrans.setproportion(100);
        newfork.m_transitions.push_back(singletrans);
        return newfork;
        }

    FMTdevelopment FMTfork::getmax(const FMTdevelopment& base,const FMTyields& ylds,const std::vector<FMTtheme>& themes,const bool& reset_age) const
        {
        double lastproportion = 0;
        FMTdevelopment dev;
        for(const FMTtransitionmask& tran : m_transitions)
            {
            const double proportion = tran.getproportion();
            if (proportion > lastproportion)
                {
                dev = tran.disturb(base,ylds,themes,reset_age).getDevelopment();
                }
            }
        return dev;
        }

	bool FMTfork::operator == (const FMTfork& rhs) const
		{
        return (FMTspec::operator == (rhs) &&
            m_transitions == rhs.m_transitions);
            
		}
}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTfork)
