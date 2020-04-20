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
#include "FMTsesevent.h"
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
		std::vector<std::map<std::string, std::vector<FMTsesevent<Core::FMTdevelopment>>>> data;
        FMTdisturbancestack();
        FMTdisturbancestack (const FMTdisturbancestack & rhs);
        FMTdisturbancestack & operator = (const FMTdisturbancestack & rhs);
        bool allow(const FMTspatialaction& action,const FMTcoordinate& location) const;
		std::map<std::string, std::string>directmapping() const;
        void push(const std::map<std::string, std::vector<FMTsesevent<Core::FMTdevelopment>>>& element);
        void add(const std::string& action,const std::vector<FMTsesevent<Core::FMTdevelopment>>& events);
		std::string getpatchstats() const;
        template <typename T>
		std::vector<FMTlayer<std::string>> getlayers(const FMTlayer<T>& layer) const
            {
			std::vector<FMTlayer<std::string>>alllayers;
            for(const std::map<std::string,std::vector<FMTsesevent<Core::FMTdevelopment>>>& events : data)
                {
                FMTlayer<std::string> newlayer= layer.template copyextent<std::string>();
				FMTlayer<int> orderlayer = layer.template copyextent<int>(); // only one event per period per pixel!
                for(std::map<std::string, std::vector<FMTsesevent<Core::FMTdevelopment>>>::const_iterator acit = events.begin(); acit != events.end();acit++)
                    {
                    const std::string actionname = acit->first;
                    for(const FMTsesevent<Core::FMTdevelopment>& event : acit->second)
                        {
                        for(std::map<FMTcoordinate,const Core::FMTdevelopment*>::const_iterator devit  = event.mapped_elements.begin(); devit!= event.mapped_elements.end(); devit++)
                            {
							std::map<FMTcoordinate, int>::iterator orit = orderlayer.mapping.find(devit->first);
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
