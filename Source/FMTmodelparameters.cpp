/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#include "FMTmodelparameters.h"
#include <boost/thread.hpp>
#include "FMTexceptionhandler.h"

namespace Models
{


    FMTmodelparameters::FMTmodelparameters():Core::FMTobject()
    {
        const unsigned int processor_count = boost::thread::hardware_concurrency();
        //INTparameters
        m_intparameters[LENGTH]=30;
        m_intparameters[SEED]=25;
        m_intparameters[NUMBER_OF_ITERATIONS]=10000;
        m_intparameters[PRESOLVE_ITERATIONS]=10;
        m_intparameters[NUMBER_OF_THREADS]=processor_count;
        m_intparameters[MATRIX_TYPE] = 2;//Default value is type II
        m_intparameters[UPDATE] = 2;//Default is that model period update stop at 2
        m_intparameters[MAX_MOVES] = 80000;
        m_intparameters[MAX_ACCEPTED_CYCLE_MOVES] = 500;
        m_intparameters[MAX_CYCLE_MOVES] = 1000;
        //DBLparameters
        m_dblparameters[TOLERANCE]= FMT_DBL_TOLERANCE;
        m_dblparameters[GOALING_SCHEDULE_WEIGHT]=10000;
        m_dblparameters[INITIAL_ACCEPTANCE_PROBABILITY] = 0.80;
        //BOOLparameters
        m_boolparameters[FORCE_PARTIAL_BUILD]=false;
        m_boolparameters[STRICTLY_POSITIVE]=false;
        m_boolparameters[POSTSOLVE]=true;
        m_boolparameters[SHOW_LOCK_IN_SCHEDULES]=false;
        m_boolparameters[PRESOLVE_CAN_REMOVE_STATIC_THEMES] = false;
        m_boolparameters[DEBUG_MATRIX] = false;
        m_boolparameters[SETSOLUTION_THROW] = true;
        m_boolparameters[QUIET_LOGGING] = false;
        m_strparameters[WORKING_DIRECTORY] = "";
        //Vector for compresstime at each period
        m_compresstime = std::vector<int>(30,1);
    }
    
    FMTmodelparameters::FMTmodelparameters(const FMTmodelparameters& rhs):
        Core::FMTobject(rhs),
        m_intparameters(rhs.m_intparameters),
        m_dblparameters(rhs.m_dblparameters),
        m_boolparameters(rhs.m_boolparameters),
        m_strparameters(rhs.m_strparameters),
        m_compresstime(rhs.m_compresstime)
    {
        
    }

    FMTmodelparameters& FMTmodelparameters::operator = (const FMTmodelparameters& rhs)
    {
        if (this!=&rhs)
            {
                Core::FMTobject::operator = (rhs);
                m_intparameters=rhs.m_intparameters;
                m_dblparameters=rhs.m_dblparameters;
                m_boolparameters=rhs.m_boolparameters;
                m_strparameters = rhs.m_strparameters;
                m_compresstime=rhs.m_compresstime;
            }
        return *this;
    }

    //###Setter
    bool FMTmodelparameters::setIntParameter(FMTintmodelparameters key,const int& value)
    {
        try{
            if (key == LastIntModelParam) return (false) ;
            if(key==LENGTH)
            {
                int oldvalue = m_intparameters[key];
                if(oldvalue>value)
                {
                    m_compresstime=std::vector<int>(m_compresstime.begin(), m_compresstime.begin()+value);
                }else{
                    for(int i = 0 ; i<(value-oldvalue);++i)
                    {
                        m_compresstime.push_back(m_compresstime.back());
                    }
                }
            }
            m_intparameters[key] = value;
        }catch(...){
            _exhandler->raisefromcatch("", "FMTmodelparameters::setIntParameter", __LINE__, __FILE__);
        }
        return true;
    }

    bool FMTmodelparameters::setDblParameter(FMTdblmodelparameters key,const double& value)
    {
        try{
            if (key == LastDblModelParam) return (false) ;
                m_dblparameters[key] = value;
        }catch(...){
            _exhandler->raisefromcatch("", "FMTmodelparameters::setDblParameter", __LINE__, __FILE__);
        }
        return true;

    }

    bool FMTmodelparameters::setBoolParameter(FMTboolmodelparameters key,const bool& value)
    {
        try{
            if (key == LastBoolModelParam) return (false) ;
                m_boolparameters[key] = value;
        }catch(...){
            _exhandler->raisefromcatch("", "FMTmodelparameters::setboolparameters", __LINE__, __FILE__);
        }
        return true;
    }

    bool FMTmodelparameters::setStrParameter(FMTstrmodelparameters p_key, const std::string& p_value)
    {
        try {
            if (p_key == LastStrModelParam) return (false);
            m_strparameters[p_key] = p_value;
        }
        catch (...) {
            _exhandler->raisefromcatch("", "FMTmodelparameters::setStrParameter", __LINE__, __FILE__);
        }
        return true;
    }

    bool FMTmodelparameters::setPeriodCompressTime(const int& period, const int& value)
    {
        try{
            m_compresstime[period]=value;
        }catch(...){
            _exhandler->raisefromcatch("", "FMTmodelparameters::setCompressTime", __LINE__, __FILE__);
        }
        return true;

    }
    //###Getter
    int FMTmodelparameters::getIntParameter(FMTintmodelparameters key) const
    {
        int value;
        try{
            if (key == LastIntModelParam)
            {
                _exhandler->raise(Exception::FMTexc::FMTrangeerror,
							"LastIntModelParam is not a parameter",
							"FMTmodelparameters::getIntParameter", __LINE__, __FILE__);
            }
            value = m_intparameters[key];
        }catch(...)
        {
            _exhandler->raisefromcatch("", "FMTmodelparameters::getintparameters", __LINE__, __FILE__);
        }
        return value;
    }

    double FMTmodelparameters::getDblParameter(FMTdblmodelparameters key) const
    {
        double value;
        try{
            if (key == LastDblModelParam)
            {
                _exhandler->raise(Exception::FMTexc::FMTrangeerror,
							"LastDblModelParam is not a parameter",
							"FMTmodelparameters::getDblParameter", __LINE__, __FILE__);
            }
            value = m_dblparameters[key];
        }catch(...)
        {
            _exhandler->raisefromcatch("", "FMTmodelparameters::getdblparameters", __LINE__, __FILE__);
        }
        return value;
    }

    bool FMTmodelparameters::getBoolParameter(FMTboolmodelparameters key) const
    {
        bool value;
        try{
            if (key == LastBoolModelParam)
            {
                _exhandler->raise(Exception::FMTexc::FMTrangeerror,
							"LastBoolModelParam is not a parameter",
							"FMTmodelparameters::getBoolParameter", __LINE__, __FILE__);
            }
            value = m_boolparameters[key];
        }catch(...)
        {
            _exhandler->raisefromcatch("", "FMTmodelparameters::getboolparameters", __LINE__, __FILE__);
        }
        return value;
    }

    const std::string& FMTmodelparameters::getStrParameter(FMTstrmodelparameters p_key) const
    {
        try {
            if (p_key == LastStrModelParam)
            {
                _exhandler->raise(Exception::FMTexc::FMTrangeerror,
                    "LastStrModelParam is not a parameter",
                    "FMTmodelparameters::getStrParameter", __LINE__, __FILE__);
            }
            return m_strparameters[p_key];
        }
        catch (...)
        {
            _exhandler->raisefromcatch("", "FMTmodelparameters::getstrparameters", __LINE__, __FILE__);
        }
        return nullptr;
    }

    int FMTmodelparameters::getPeriodCompressTime(const int& period) const
    {
        int value;
        try{
            value = m_compresstime.at(period);
        }catch(...)
        {
            _exhandler->raisefromcatch("", "FMTmodelparameters::getCompressTime", __LINE__, __FILE__);
        }
        return value;

    }

    std::vector<int> FMTmodelparameters::getCompressTime() const
    {
        return m_compresstime;
    }

    void FMTmodelparameters::swap(FMTmodelparameters& rhs)
    {
        m_intparameters.swap(rhs.m_intparameters);
        m_dblparameters.swap(rhs.m_dblparameters);
        m_boolparameters.swap(rhs.m_boolparameters);
        m_strparameters.swap(rhs.m_strparameters);
        m_compresstime.swap(rhs.m_compresstime);
    }


}

BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTmodelparameters)