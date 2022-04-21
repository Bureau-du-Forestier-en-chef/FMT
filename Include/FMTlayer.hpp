/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTLAYER_H_INCLUDED
#define FMTLAYER_H_INCLUDED
#include <memory>
#include <map>
#include <vector>
#include "FMTcoordinate.hpp"
#include "FMTobject.hpp"
#include <boost/serialization/nvp.hpp>

/// The spatial namespace provides classes for spatialy explicit simulation/optimization based on raster files.
namespace Spatial
    {
	// DocString: FMTlayer
	/**
	FMTlayer is a simple template class made to be used like a single raster file.
	It hold the coordinates of each pixel using a map, it also have information about the projection of the raster.
	It is mostly generated using the FMTareaparser.
	*/
    template <typename T>
    class FMTlayer : public Core::FMTobject
        {
		friend class boost::serialization::access;
		// DocString: FMTlifespans::serialize
		/**
		Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTobject>(*this));
			ar & BOOST_SERIALIZATION_NVP(geotransform);
			ar & BOOST_SERIALIZATION_NVP(maxx);
			ar & BOOST_SERIALIZATION_NVP(maxy);
			ar & BOOST_SERIALIZATION_NVP(SRS_WKT);
			ar & BOOST_SERIALIZATION_NVP(cellsize);
			ar & BOOST_SERIALIZATION_NVP(mapping);
		}
        protected:
		// DocString: FMTlayer::geotransform
		///Geotransform of the map (see GDAL for more information about Geotransform)
		std::vector<double>geotransform;
		// DocString: FMTlayer::maxx
		///Maximal x value in the map
        unsigned int maxx;
		// DocString: FMTlayer::maxy
		///Maximal y value in the map
		unsigned int maxy;
		// DocString: FMTlayer::SRS_WKT
		///Projection string of the raster (see GDAL for more information about Geotransform)
		std::string SRS_WKT;
		// DocString: FMTlayer::cellsize
		///Size of the pixel in the unit used by the map (SRS_WKT)
        double cellsize;
		// DocString: FMTlayer::mapping
		///std::map keeping the information of each pixel.
		std::map<FMTcoordinate, T>mapping;
        public:
			// DocString: FMTlayer::value_type
			///Value typedef of the FMTlayer
			typedef typename std::map<FMTcoordinate,T>::value_type value_type;
			// DocString: FMTlayer::iterator
			///Iterator typedef of the FMTlayer
			typedef typename std::map<FMTcoordinate,T>::iterator iterator;
			// DocString: FMTlayer::const_iterator
			///Const_Iterator typedef of the FMTlist
			typedef typename std::map<FMTcoordinate,T>::const_iterator const_iterator;
			// DocString: FMTlayer::operator[]
			/**
			Operator [] for FMTlayer accessing std::map (mapping).
			*/
			T& operator [](const FMTcoordinate& coordinate)
				{
				return mapping[coordinate];
				}
			// DocString: FMTlayer::at
			/**
			At for FMTlayer accessing std::map (mapping).
			*/
			const T& at(const FMTcoordinate& coordinate) const
			{
				return mapping.at(coordinate);
			}
			// DocString: FMTlayer::empty
			/**
			Check if the FMTlayer is empty.
			*/
			inline bool empty() const
			{
				return mapping.empty();
			}
			// DocString: FMTlayer::begin
			/**
			Returns an iterator at the beginning of the FMTlayer.
			*/
			iterator begin()
			{
				return mapping.begin();
			}
			// DocString: FMTlayer::begin
			/**
			Returns an const iterator at the beginning of the FMTlayer.
			*/
			const_iterator begin() const
			{
				return mapping.begin();
			}
			// DocString: FMTlayer::end
			/**
			Returns an iterator at the end of the FMTlayer.
			*/
			iterator  end()
			{
				return mapping.end();
			}
			// DocString: FMTlayer::end
			/**
			Returns an const iterator at the end of the FMTlayer.
			*/
			const_iterator end() const
			{
				return mapping.end();
			}
			// DocString: FMTlayer::find
			/**
			Find the layer element at a given coordinate.
			*/
			const_iterator find(const FMTcoordinate& coordinate) const
			{
				return mapping.find(coordinate);
			}
			// DocString: FMTlayer()
			/**
			Default constructor for FMTlayer.
			*/
            FMTlayer():Core::FMTobject(),geotransform(),maxx(),maxy(),SRS_WKT(),cellsize(),mapping(){}
			// DocString: ~FMTlayer()
			/**
			Default destructor for FMTlayer.
			*/
            virtual~FMTlayer()=default;
			// DocString: FMTlayer(FMTlayer&&)
			/**
			Default move constructor for FMTlayer.
			*/
			FMTlayer(FMTlayer&& rhs) noexcept :
				Core::FMTobject(std::move(rhs)),
				geotransform(std::move(rhs.geotransform)),
				maxx(std::move(rhs.maxx)),
				maxy(std::move(rhs.maxy)),
				SRS_WKT(std::move(rhs.SRS_WKT)),
				cellsize(std::move(rhs.cellsize)),
				mapping(std::move(rhs.mapping))
					{
			
					}
			// DocString: FMTlayer(const std::vector<double>&,const unsigned int&,const unsigned int&,const std::string&,const double&)
			/**
			Constructor for the FMTlayer taking a geotransform, max x, max y, projection string and cell size.
			*/
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
			// DocString: FMTlayer(const std::map<FMTcoordinate,T>&,const std::vector<double>&,const unsigned int&,const unsigned int&,const std::string&,const double&)
			/**
			Constructor for the FMTlayer used to copy information from an other layer.
			*/
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
			// DocString: FMTlayer(const FMTlayer&)
			/**
			Default copy constructor for FMTlayer.
			*/
            FMTlayer(const FMTlayer& rhs):Core::FMTobject(rhs),
                geotransform(rhs.geotransform),
                maxx(rhs.maxx),
                maxy(rhs.maxy),
                SRS_WKT(rhs.SRS_WKT),
                cellsize(rhs.cellsize),
                mapping(rhs.mapping){}
			// DocString: FMTlayer::operator=
			/**
			Default copy assignment for FMTlayer.
			*/
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
			// DocString: FMTlayer::swap
			/**
			Swap operator for FMTlayer.
			*/
			void swap(FMTlayer<T>& rhs)
				{
				try {
					mapping.swap(rhs.mapping);
					geotransform.swap(rhs.geotransform);
					std::swap(maxx, rhs.maxx);
					std::swap(maxy, rhs.maxy);
					std::swap(SRS_WKT, rhs.SRS_WKT);
					std::swap(cellsize, rhs.cellsize);
				}catch (...)
					{
					_exhandler->raisefromcatch("", "FMTlayer::swap", __LINE__, __FILE__);
					}
				}

			// DocString: FMTlayer::setextentfrom
			/**
			Using an other layer (rhs) it's going to set the informations (other than the main map) to this.
			*/
			void setextentfrom(const FMTlayer<T>& rhs)
				{
				this->geotransform = rhs.geotransform;
				this->maxx = rhs.maxx;
				this->maxy = rhs.maxy;
				this->SRS_WKT = rhs.SRS_WKT;
				this->cellsize = rhs.cellsize;
				}
			// DocString: FMTlayer::copyextent
			/**
			Create a new FMTlayer by copying informations (other than the main map) to a new FMTlayer from this.
			*/
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
			// DocString: FMTlayer::general
			/**
			Template specification for adding strings from a layer to a string layer using std::string operator+=.
			*/
			FMTlayer<T>& operator+= (const FMTlayer<T>& rhs)
			{
				return *this;
			}
			// DocString: FMTlayer::GetXSize
			/**
			Returns the maximal x value of the FMTlayer.
			*/
            unsigned int GetXSize() const
                {
                return maxx;
                }
			// DocString: FMTlayer::GetYSize
			/**
			Returns the maximal y value of the FMTlayer.
			*/
            unsigned int GetYSize() const
                {
                return maxy;
                }
			// DocString: FMTlayer::getgeotransform
			/**
			Returns the geotransform of the layer.
			*/
			std::vector<double> getgeotransform() const
                {
                return geotransform;
                }
			// DocString: FMTlayer::getprojection
			/**
			Returns the projection of the FMTlayer.
			*/
			std::string getprojection() const
                {
                return SRS_WKT;
                }
			// DocString: FMTlayer::getmapping
			/**
			Returns the underlying map of the FMTlayer.
			*/
			std::map<FMTcoordinate,T>getmapping() const
                {
                return mapping;
                }
			// DocString: FMTlayer::area
			/**
			Returns the whole area of the FMTlayer using the size of a pixel.
			*/
            double area() const
                {
                return (cellsize * static_cast<double>(mapping.size()));
                }
			// DocString: FMTlayer::getcellsize
			/**
			Get the cell size for a single coordinate of the map.
			*/
			double getcellsize() const
				{
				return cellsize;
				}
			// DocString: FMTlayer::getattributes
			/**
			Returns a vector of unique attributes present in the FMTlayer.
			*/
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
			// DocString: FMTlayer::size
			/**
			Returns the size of the FMTlayer.
			*/
            size_t size() const
                {
                return mapping.size();
                }
			// DocString: FMTlayer::size
			/**
			Replaces values of a layer using iterators coming from an other FMTlayer.
			*/
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

		template<> inline FMTlayer<std::string>& FMTlayer<std::string>::operator += (const FMTlayer<std::string>& rhs)
		{
			std::map<FMTcoordinate, std::string>new_mapping;
			for (std::map<FMTcoordinate, std::string>::const_iterator mit = mapping.begin(); mit != mapping.end(); mit++)
			{
				std::map<FMTcoordinate, std::string>::const_iterator rhsit = rhs.mapping.find(mit->first);
				if (rhsit != rhs.mapping.end())
				{
					new_mapping[mit->first] = (mit->second + "-" + rhsit->second);
				}
			}
			mapping = new_mapping;
			return *this;
		}

    }

#endif // FMTLAYER_H_INCLUDED
