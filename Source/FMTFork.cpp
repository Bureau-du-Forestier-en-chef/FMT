/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTFork.h"
#include "FMTDevelopment.h"
#include "FMTDevelopmentPath.h"
#include "FMTYields.h"
#include "FMTTransitionMask.h"
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/unordered_map.hpp>


namespace Core{

    FMTFork::~FMTFork() = default;




FMTFork::FMTFork():FMTSpec(),FMTObject(), m_transitions()
    {


    }

    void FMTFork::add(const FMTTransitionMask& transition)
        {
        m_transitions.push_back(transition);
        }
    void FMTFork::clear()
    {
        m_transitions.clear();
    }


     double FMTFork::sumProp() const
        {
        double total = 0;
        for(const FMTTransitionMask& transition : m_transitions)
            {
            total+=transition.getProportion();
            }
        return total;
        }
    size_t FMTFork::size() const
        {
        return m_transitions.size();
        }
    FMTFork::operator std::string() const
        {
		std::string line;
        line+=" "+FMTSpec::operator std::string();
        line+="\n";
        for(const FMTTransitionMask& transition : m_transitions)
            {
            line+= std::string(transition);
            line+="\n";
            }
        return line;
        }

	FMTFork FMTFork::presolve(const FMTMaskFilter& filter, const std::vector<FMTTheme>&presolvedthemes) const
		{
		FMTFork newfork(*this);
        newfork.presolveRef(filter, presolvedthemes);
        return newfork;
		}

    void FMTFork::presolveRef(const FMTMaskFilter& filter, const std::vector<FMTTheme>& presolvedthemes)
        {
        for (FMTTransitionMask& trmask : m_transitions)
            {
            trmask.presolveRef(filter, presolvedthemes);
            }
        }

    FMTDevelopmentPath FMTFork::_getPath(const FMTTransitionMask& p_target, 
        const Core::FMTDevelopment& p_base, const Core::FMTYields& p_yields,
        const std::vector<FMTTheme>& p_themes, bool p_AgeReset) const
    {
        try {
            FMTDevelopmentPath newPath = p_target.disturb(p_base, p_yields, p_themes, p_AgeReset);
            FMTDevelopment& newDev = newPath.getDevelopmentReference();
            if (!p_AgeReset && p_base == newDev)
                {
                _exhandler->raise(Exception::FMTexc::FMTsourcetotarget_transition,
                    "from " + std::string(p_base) + " to " + std::string(newDev) + "\n",
                    "FMTFork::_getPath", __LINE__, __FILE__, Core::FMTsection::Transition);
                newDev.setLock(newDev.getLock() + 1);
                }
            return newPath;
        }catch (...)
            {
            _exhandler->raiseFromCatch("", "FMTFork::_getPath", __LINE__, __FILE__,
                Core::FMTsection::Transition);
            }
        return FMTDevelopmentPath();
    }

	std::vector<FMTDevelopmentPath> FMTFork::getPaths(const Core::FMTDevelopment& base,const Core::FMTYields& ylds,
		const std::vector<FMTTheme>& themes, const bool& reset_age) const
		{
        std::vector<FMTDevelopmentPath>paths;
        try{
            paths.reserve(size());
        if (size() == 1)
            {
            paths.push_back(_getPath(*m_transitions.begin(), base, ylds,
                themes, reset_age));
        }else {
            boost::unordered_map<Core::FMTDevelopment, size_t>pathmap;
            size_t pathid = 0;
            for (const FMTTransitionMask& tran : m_transitions)
                {
                const FMTDevelopmentPath NEW_PATH = _getPath(tran, base, ylds,
                    themes, reset_age);
                const FMTDevelopment& newDev = NEW_PATH.getDevelopment();
                boost::unordered_map<Core::FMTDevelopment, size_t>::const_iterator mapit = pathmap.find(newDev);
                if (mapit != pathmap.end())
                    {
                        _exhandler->raise(Exception::FMTexc::FMTsame_transitiontargets,
                            "from " + std::string(base) + " to " + std::string(newDev) + "\n",
                            "FMTFork::getPaths", __LINE__, __FILE__, Core::FMTsection::Transition);
                        paths[mapit->second].setProportion(paths[mapit->second].getProportion() + tran.getProportion());
                        continue;
                    }else {
                        pathmap[newDev] = pathid;
                    }
                paths.push_back(NEW_PATH);
                ++pathid;
                }
            }
        }catch (...)
            {
            _exhandler->raiseFromCatch("", "FMTFork::getPaths",
                __LINE__, __FILE__, Core::FMTsection::Transition);
             }
        return paths;
		}

	std::vector<FMTTransitionMask> FMTFork::getMaskTrans() const
        {
        return m_transitions;
        }

     FMTFork FMTFork::single() const
        {
        FMTFork newfork(*this);
        newfork.m_transitions.clear();
        double lastproportion = 0;
        FMTTransitionMask singletrans;
        for(const FMTTransitionMask& tran : m_transitions)
            {
            const double proportion = tran.getProportion();
            if (proportion > lastproportion)
                {
                singletrans = tran;
				lastproportion = proportion;
                }
            }
        singletrans.setProportion(100);
        newfork.m_transitions.push_back(singletrans);
        return newfork;
        }

    FMTDevelopment FMTFork::getMax(const FMTDevelopment& base,const FMTYields& ylds,const std::vector<FMTTheme>& themes,const bool& reset_age) const
        {
        double lastproportion = 0;
        FMTDevelopment dev;
        for(const FMTTransitionMask& tran : m_transitions)
            {
            const double proportion = tran.getProportion();
            if (proportion > lastproportion)
                {
                dev = tran.disturb(base,ylds,themes,reset_age).getDevelopment();
                }
            }
        return dev;
        }

	bool FMTFork::operator == (const FMTFork& rhs) const
		{
        return (FMTSpec::operator == (rhs) &&
            m_transitions == rhs.m_transitions);
            
		}
}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTFork)
