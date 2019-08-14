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

#ifndef FMTDISTURBANCE_H_INCLUDED
#define FMTDISTURBANCE_H_INCLUDED

#include "FMTlayer.h"
#include "FMTspatialaction.h"
#include "FMTevent.h"
#include "FMTschedule.h"
#include <vector>
#include <map>
#include <boost/serialization/nvp.hpp>

namespace Spatial
{

class FMTdisturbancestack
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		ar & BOOST_SERIALIZATION_NVP(data);
		}
    public:
        vector<map<string,vector<FMTevent<FMTdevelopment>>>> data;
        FMTdisturbancestack();
        FMTdisturbancestack (const FMTdisturbancestack & rhs);
        FMTdisturbancestack & operator = (const FMTdisturbancestack & rhs);
        bool allow(const FMTspatialaction& action,const FMTcoordinate& location) const;
        map<string,string>directmapping() const;
        void push(const map<string,vector<FMTevent<FMTdevelopment>>>& element);
        void add(const string& action,const vector<FMTevent<FMTdevelopment>>& events);
		/*FMTschedule getlastschedule(double cellsize,
			const vector<FMTaction>& actions,
			const FMTlayer<FMTdevelopment>& forest) const;*/
		string getpatchstats() const;
        template <typename T>
        vector<FMTlayer<string>> getlayers(const FMTlayer<T>& layer) const
            {
            vector<FMTlayer<string>>alllayers;
            for(const map<string,vector<FMTevent<FMTdevelopment>>>& events : data)
                {
                FMTlayer<string> newlayer= layer.template copyextent<string>();
				FMTlayer<int> orderlayer = layer.template copyextent<int>(); // only one event per period per pixel!
                for(map<string,vector<FMTevent<FMTdevelopment>>>::const_iterator acit = events.begin(); acit != events.end();acit++)
                    {
                    const string actionname = acit->first;
                    for(const FMTevent<FMTdevelopment>& event : acit->second)
                        {
                        for(map<FMTcoordinate,const FMTdevelopment*>::const_iterator devit  = event.elements.begin(); devit!= event.elements.end(); devit++)
                            {
							map<FMTcoordinate, int>::iterator orit = orderlayer.mapping.find(devit->first);
							if (orit == orderlayer.mapping.end() || (orit != orderlayer.mapping.end() && orit->second < event.getorder()))//happend after so priority!
								{
								newlayer.mapping[devit->first] = actionname;
								orderlayer.mapping[devit->first] = event.getorder();
								}
                            }
                        }
                    }
                alllayers.push_back(newlayer);
                }
            return alllayers;
            }
    };
}



#endif // FMTDISTURBANCE_H_INCLUDED
