/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#include "FMTmodelparameters.hpp"
#include <boost/thread.hpp>
#include "FMTexceptionhandler.hpp"

namespace Models
{
    FMTmodelparameters::FMTmodelparameters():Core::FMTobject()
    {
        const unsigned int processor_count = boost::thread::hardware_concurrency();
        //INTparameters
        intparameters[LENGTH]=30;
        intparameters[SEED]=25;
        intparameters[NUMBER_OF_ITERATIONS]=10000;
        intparameters[PRESOLVE_ITERATIONS]=10;
        intparameters[NUMBER_OF_THREADS]=processor_count;
		intparameters[MATRIX_TYPE] = 2;//Default value is type II
        //DBLparameters
        dblparameters[TOLERANCE]= FMT_DBL_TOLERANCE;
        dblparameters[GOALING_SCHEDULE_WEIGHT]=10000;
        //BOOLparameters
        boolparameters[FORCE_PARTIAL_BUILD]=false;
        boolparameters[STRICTLY_POSITIVE]=false;
        boolparameters[POSTSOLVE]=true;
        boolparameters[SHOW_LOCK_IN_SCHEDULES]=false;
		boolparameters[PRESOLVE_CAN_REMOVE_STATIC_THEMES] = false;
        //Vector for compresstime at each period
        compresstime = std::vector<int>(30,1);
    }
    
    FMTmodelparameters::FMTmodelparameters(const FMTmodelparameters& rhs):Core::FMTobject(rhs)
    {
        intparameters=rhs.intparameters;
        dblparameters=rhs.dblparameters;
        boolparameters=rhs.boolparameters;
        compresstime=rhs.compresstime;
    }

    FMTmodelparameters& FMTmodelparameters::operator = (const FMTmodelparameters& rhs)
    {
        if (this!=&rhs)
            {
                Core::FMTobject::operator = (rhs);
                intparameters=rhs.intparameters;
                dblparameters=rhs.dblparameters;
                boolparameters=rhs.boolparameters;
                compresstime=rhs.compresstime;
            }
        return *this;
    }

    //###Setter
    bool FMTmodelparameters::setintparameter(const FMTintmodelparameters& key,const int& value)
    {
        try{
            if (key == LastIntModelParam) return (false) ;
            if(key==LENGTH)
            {
                int oldvalue = intparameters[key];
                if(oldvalue>value)
                {
                    compresstime=std::vector<int>(compresstime.begin(),compresstime.begin()+value);
                }else{
                    for(int i = 0 ; i<(value-oldvalue);++i)
                    {
                        compresstime.push_back(compresstime.back());
                    }
                }
            }
            intparameters[key] = value;
        }catch(...){
            _exhandler->raisefromcatch("", "FMTmodelparameters::setintparameter", __LINE__, __FILE__);
        }
        return true;
    }

    bool FMTmodelparameters::setdblparameter(const FMTdblmodelparameters& key,const double& value)
    {
        try{
            if (key == LastDblModelParam) return (false) ;
                dblparameters[key] = value;
        }catch(...){
            _exhandler->raisefromcatch("", "FMTmodelparameters::setdblparameter", __LINE__, __FILE__);
        }
        return true;

    }

    bool FMTmodelparameters::setboolparameter(const FMTboolmodelparameters& key,const bool& value)
    {
        try{
            if (key == LastBoolModelParam) return (false) ;
                boolparameters[key] = value;
        }catch(...){
            _exhandler->raisefromcatch("", "FMTmodelparameters::setboolparameters", __LINE__, __FILE__);
        }
        return true;
    }

    bool FMTmodelparameters::setperiodcompresstime(const int& period, const int& value)
    {
        try{
            compresstime[period]=value;
        }catch(...){
            _exhandler->raisefromcatch("", "FMTmodelparameters::setcompresstime", __LINE__, __FILE__);
        }
        return true;

    }
    //###Getter
    int FMTmodelparameters::getintparameter(const FMTintmodelparameters& key) const
    {
        int value;
        try{
            if (key == LastIntModelParam)
            {
                _exhandler->raise(Exception::FMTexc::FMTrangeerror,
							"LastIntModelParam is not a parameter",
							"FMTmodelparameters::getintparameter", __LINE__, __FILE__);
            }
            value = intparameters[key];
        }catch(...)
        {
            _exhandler->raisefromcatch("", "FMTmodelparameters::getintparameters", __LINE__, __FILE__);
        }
        return value;
    }

    double FMTmodelparameters::getdblparameter(const FMTdblmodelparameters& key) const
    {
        double value;
        try{
            if (key == LastDblModelParam)
            {
                _exhandler->raise(Exception::FMTexc::FMTrangeerror,
							"LastDblModelParam is not a parameter",
							"FMTmodelparameters::getintparameter", __LINE__, __FILE__);
            }
            value = dblparameters[key];
        }catch(...)
        {
            _exhandler->raisefromcatch("", "FMTmodelparameters::getdblparameters", __LINE__, __FILE__);
        }
        return value;
    }

    bool FMTmodelparameters::getboolparameter(const FMTboolmodelparameters& key) const
    {
        bool value;
        try{
            if (key == LastBoolModelParam)
            {
                _exhandler->raise(Exception::FMTexc::FMTrangeerror,
							"LastBoolModelParam is not a parameter",
							"FMTmodelparameters::getintparameter", __LINE__, __FILE__);
            }
            value = boolparameters[key];
        }catch(...)
        {
            _exhandler->raisefromcatch("", "FMTmodelparameters::getboolparameters", __LINE__, __FILE__);
        }
        return value;
    }

    int FMTmodelparameters::getperiodcompresstime(const int& period) const
    {
        int value;
        try{
            value = compresstime.at(period);
        }catch(...)
        {
            _exhandler->raisefromcatch("", "FMTmodelparameters::getcompresstime", __LINE__, __FILE__);
        }
        return value;

    }


}

BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTmodelparameters)