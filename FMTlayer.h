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

#ifndef FMTLAYER_H_INCLUDED
#define FMTLAYER_H_INCLUDED
#include "FMTdevelopment.h"
#include "FMTactualdevelopment.h"
#include "FMTexception.h"
#include "FMTaction.h"
#include "FMTtransition.h"
#include "FMTyields.h"
#include <memory>
#include <map>
#include <vector>
#include "FMTlifespans.h"
#include "FMTcoordinate.h"
#include "FMTobject.h"
#include <boost/serialization/nvp.hpp>

using namespace boost;
using namespace Core;

namespace Spatial
    {
    template <typename T>
    class FMTlayer : public FMTobject
        {
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & BOOST_SERIALIZATION_NVP(geotransform);
			ar & BOOST_SERIALIZATION_NVP(maxx);
			ar & BOOST_SERIALIZATION_NVP(maxy);
			ar & BOOST_SERIALIZATION_NVP(SRS_WKT);
			ar & BOOST_SERIALIZATION_NVP(cellsize);
			ar & BOOST_SERIALIZATION_NVP(mapping);
		}
        protected:
        vector<double>geotransform;
        unsigned int maxx,maxy;
        string SRS_WKT;
        double cellsize;
        public:
            map<FMTcoordinate,T>mapping;
            FMTlayer():FMTobject(),geotransform(),maxx(),maxy(),SRS_WKT(),cellsize(),mapping(){}
            virtual~FMTlayer()=default;
            FMTlayer(const vector<double>& lgeotransform,
                     const unsigned int& lmaxx,
                     const unsigned int& lmaxy,
                     const string& lSRS_WKT,
                     const double& lcellsize):FMTobject(),
                     geotransform(lgeotransform),
                     maxx(lmaxx),
                     maxy(lmaxy),
                     SRS_WKT(lSRS_WKT),
                     cellsize(lcellsize),
                     mapping(){}
            FMTlayer(const map<FMTcoordinate,T>& lmapping,
                     const vector<double>& lgeotransform,
                     const unsigned int& lmaxx,
                     const unsigned int& lmaxy,
                     const string& lSRS_WKT,
                     const double& lcellsize):
                         FMTobject(),
                geotransform(lgeotransform),
                             maxx(lmaxx),
                             maxy(lmaxy),
                             SRS_WKT(lSRS_WKT),
                             cellsize(lcellsize),
                             mapping(lmapping){}
            FMTlayer(const FMTlayer& rhs):FMTobject(rhs),
                geotransform(rhs.geotransform),
                maxx(rhs.maxx),
                maxy(rhs.maxy),
                SRS_WKT(rhs.SRS_WKT),
                cellsize(rhs.cellsize),
                mapping(rhs.mapping){}
            FMTlayer<T>& operator = (const FMTlayer<T>& rhs)
                {
                if(this!=&rhs)
                    {
                    FMTobject::operator = (rhs);
                    mapping = (rhs.mapping);
                    geotransform = rhs.geotransform;
                    maxx = rhs.maxx;
                    maxy = rhs.maxy;
                    SRS_WKT = rhs.SRS_WKT;
                    cellsize = rhs.cellsize;
                    }
                return *this;
                }
            template<typename newtype>
            FMTlayer<newtype>copyextent() const

                {
                return FMTlayer<newtype>(map<FMTcoordinate,newtype>(),
                                         this->geotransform,
                                         this->maxx,
                                         this->maxy,
                                         this->SRS_WKT,
                                         this->cellsize);
                }
			FMTlayer<string>& operator += (const FMTlayer<string>& rhs)
				{
				map<FMTcoordinate, string>new_mapping;
				for (map<FMTcoordinate, string>::const_iterator mit = mapping.begin();mit!= mapping.end();mit++)
					{
					map<FMTcoordinate, string>::const_iterator rhsit = rhs.mapping.find(mit->first);
					if (rhsit != rhs.mapping.end())
						{
						new_mapping[mit->first]=(mit->second+ "-" + rhsit->second);
						}
					}
				mapping = new_mapping;
				return *this;
				}

            unsigned int GetXSize() const
                {
                return maxx;
                }
            unsigned int GetYSize() const
                {
                return maxy;
                }
            vector<double> getgeotransform() const
                {
                return geotransform;
                }
            string getprojection() const
                {
                return SRS_WKT;
                }
            map<FMTcoordinate,T>getmapping() const
                {
                return mapping;
                }
            double area() const
                {
                return (cellsize * mapping.size());
                }
			double getcellsize() const
				{
				return cellsize;
				}
            vector<T>getattributes() const
                {
                vector<T>unique_attributes;
                for(typename map<FMTcoordinate,T>::const_iterator it = mapping.begin();it != mapping.end(); it++)
                    {
                    if (std::find(unique_attributes.begin(),unique_attributes.end(),it->second)==unique_attributes.end())
                        {
                        unique_attributes.push_back(it->second);
                        }
                    }
                return unique_attributes;
                }
            size_t size()
                {
                return mapping.size();
                }
            void replace(typename map<FMTcoordinate,T>::const_iterator first,typename map<FMTcoordinate,T>::const_iterator last)
                {
                while (first!=last)
                    {
                    typename map<FMTcoordinate,T>::iterator it = mapping.find(first->first);
                    if (it != last)
                        {
                        it->second = first->second;
                        }
                    ++first;
                    }
                }
        };

    }

#endif // FMTLAYER_H_INCLUDED
