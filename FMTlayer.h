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

namespace Spatial
    {
    template <typename T>
    class FMTlayer : public Core::FMTobject
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
		std::vector<double>geotransform;
        unsigned int maxx,maxy;
		std::string SRS_WKT;
        double cellsize;
        public:
			std::map<FMTcoordinate,T>mapping;
            FMTlayer():Core::FMTobject(),geotransform(),maxx(),maxy(),SRS_WKT(),cellsize(),mapping(){}
            virtual~FMTlayer()=default;
            FMTlayer(const std::vector<double>& lgeotransform,
                     const unsigned int& lmaxx,
                     const unsigned int& lmaxy,
                     const std::string& lSRS_WKT,
                     const double& lcellsize):Core::FMTobject(),
                     geotransform(lgeotransform),
                     maxx(lmaxx),
                     maxy(lmaxy),
                     SRS_WKT(lSRS_WKT),
                     cellsize(lcellsize),
                     mapping(){}
            FMTlayer(const std::map<FMTcoordinate,T>& lmapping,
                     const std::vector<double>& lgeotransform,
                     const unsigned int& lmaxx,
                     const unsigned int& lmaxy,
                     const std::string& lSRS_WKT,
                     const double& lcellsize):
                         Core::FMTobject(),
                geotransform(lgeotransform),
                             maxx(lmaxx),
                             maxy(lmaxy),
                             SRS_WKT(lSRS_WKT),
                             cellsize(lcellsize),
                             mapping(lmapping){}
            FMTlayer(const FMTlayer& rhs):Core::FMTobject(rhs),
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
                    Core::FMTobject::operator = (rhs);
                    mapping = (rhs.mapping);
                    geotransform = rhs.geotransform;
                    maxx = rhs.maxx;
                    maxy = rhs.maxy;
                    SRS_WKT = rhs.SRS_WKT;
                    cellsize = rhs.cellsize;
                    }
                return *this;
                }
			void setextentfrom(const FMTlayer<T>& rhs)
				{
				this->geotransform = rhs.geotransform;
				this->maxx = rhs.maxx;
				this->maxy = rhs.maxy;
				this->SRS_WKT = rhs.SRS_WKT;
				this->cellsize = rhs.cellsize;
				}
            template<typename newtype>
            FMTlayer<newtype>copyextent() const

                {
                return FMTlayer<newtype>(std::map<FMTcoordinate,newtype>(),
                                         this->geotransform,
                                         this->maxx,
                                         this->maxy,
                                         this->SRS_WKT,
                                         this->cellsize);
                }
			FMTlayer<std::string>& operator += (const FMTlayer<std::string>& rhs)
				{
				std::map<FMTcoordinate, std::string>new_mapping;
				for (std::map<FMTcoordinate, std::string>::const_iterator mit = mapping.begin();mit!= mapping.end();mit++)
					{
					std::map<FMTcoordinate, std::string>::const_iterator rhsit = rhs.mapping.find(mit->first);
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
			std::vector<double> getgeotransform() const
                {
                return geotransform;
                }
			std::string getprojection() const
                {
                return SRS_WKT;
                }
			std::map<FMTcoordinate,T>getmapping() const
                {
                return mapping;
                }
            double area() const
                {
                return (cellsize * static_cast<double>(mapping.size()));
                }
			double getcellsize() const
				{
				return cellsize;
				}
            std::vector<T>getattributes() const
                {
				std::vector<T>unique_attributes;
                for(typename std::map<FMTcoordinate,T>::const_iterator it = mapping.begin();it != mapping.end(); it++)
                    {
                    if (std::find(unique_attributes.begin(),unique_attributes.end(),it->second)==unique_attributes.end())
                        {
                        unique_attributes.push_back(it->second);
                        }
                    }
                return unique_attributes;
                }
            size_t size() const
                {
                return mapping.size();
                }
            void replace(typename std::map<FMTcoordinate,T>::const_iterator first,typename std::map<FMTcoordinate,T>::const_iterator last)
                {
                while (first!=last)
                    {
                    typename std::map<FMTcoordinate,T>::iterator it = mapping.find(first->first);
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
