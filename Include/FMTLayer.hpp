/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTLAYER_Hm_included
#define FMTLAYER_Hm_included
#include <memory>
#include <map>
#include <vector>
#include "FMTCoordinate.h"
#include "FMTObject.h"
#include <boost/serialization/nvp.hpp>
#include "FMTExceptionHandler.h"

/// The spatial namespace provides classes for spatially explicit simulation/optimization based on raster files.
namespace Spatial
    {
	// DocString: FMTLayer
	/**
	FMTLayer is a simple template class made to be used like a single raster file.
	It hold the coordinates of each pixel using a map, it also have information about the projection of the raster.
	It is mostly generated using the FMTAreaParser.
	*/
    template <typename T>
    class FMTLayer : public Core::FMTObject
        {
		friend class boost::serialization::access;
		// DocString: FMTLifespans::serialize
		/**
		Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTObject>(*this));
			ar & BOOST_SERIALIZATION_NVP(geotransform);
			ar & BOOST_SERIALIZATION_NVP(maxx);
			ar & BOOST_SERIALIZATION_NVP(maxy);
			ar & BOOST_SERIALIZATION_NVP(SRS_WKT);
			ar & BOOST_SERIALIZATION_NVP(cellsize);
			ar & BOOST_SERIALIZATION_NVP(mapping);
		}
        protected:
		// DocString: FMTLayer::geotransform
		///Geotransform of the map (see GDAL for more information about Geotransform)
		std::vector<double>geotransform;
		// DocString: FMTLayer::maxx
		///Maximal x value in the map
        unsigned int maxx;
		// DocString: FMTLayer::maxy
		///Maximal y value in the map
		unsigned int maxy;
		// DocString: FMTLayer::SRS_WKT
		///Projection string of the raster (see GDAL for more information about Geotransform)
		std::string SRS_WKT;
		// DocString: FMTLayer::cellsize
		///Size of the pixel in the unit used by the map (SRS_WKT)
        double cellsize;
		// DocString: FMTLayer::mapping
		///std::map keeping the information of each pixel.
		std::map<FMTCoordinate, T>mapping;
        public:
			// DocString: FMTLayer::value_type
			///Value typedef of the FMTLayer
			typedef typename std::map<FMTCoordinate,T>::value_type value_type;
			// DocString: FMTLayer::iterator
			///Iterator typedef of the FMTLayer
			typedef typename std::map<FMTCoordinate,T>::iterator iterator;
			// DocString: FMTLayer::const_iterator
			///Const_Iterator typedef of the FMTList
			typedef typename std::map<FMTCoordinate,T>::const_iterator const_iterator;
			// DocString: FMTLayer::operator[]
			/**
			Operator [] for FMTLayer accessing std::map (mapping).
			*/
			T& operator [](const FMTCoordinate& coordinate)
				{
				return mapping[coordinate];
				}
			// DocString: FMTLayer::at
			/**
			At for FMTLayer accessing std::map (mapping).
			*/
			const T& at(const FMTCoordinate& coordinate) const
			{
				return mapping.at(coordinate);
			}
			// DocString: FMTLayer::empty
			/**
			Check if the FMTLayer is empty.
			*/
			inline bool empty() const
			{
				return mapping.empty();
			}
			// DocString: FMTLayer::begin
			/**
			Returns an iterator at the beginning of the FMTLayer.
			*/
			iterator begin()
			{
				return mapping.begin();
			}
			// DocString: FMTLayer::begin
			/**
			Returns an const iterator at the beginning of the FMTLayer.
			*/
			const_iterator begin() const
			{
				return mapping.begin();
			}
			// DocString: FMTLayer::end
			/**
			Returns an iterator at the end of the FMTLayer.
			*/
			iterator  end()
			{
				return mapping.end();
			}
			// DocString: FMTLayer::end
			/**
			Returns an const iterator at the end of the FMTLayer.
			*/
			const_iterator end() const
			{
				return mapping.end();
			}
			// DocString: FMTLayer::find
			/**
			Find the layer element at a given coordinate.
			*/
			const_iterator find(const FMTCoordinate& coordinate) const
			{
				return mapping.find(coordinate);
			}
			// DocString: FMTLayer::find
			/**
			Find the layer element at a given coordinate.
			*/
			iterator find(const FMTCoordinate& coordinate)
			{
				return mapping.find(coordinate);
			}
			// DocString: FMTLayer()
			/**
			Default constructor for FMTLayer.
			*/
            FMTLayer():Core::FMTObject(),geotransform(),maxx(),maxy(),SRS_WKT(),cellsize(),mapping(){}
			// DocString: ~FMTLayer()
			/**
			Default destructor for FMTLayer.
			*/
            virtual~FMTLayer()=default;
			// DocString: FMTLayer(FMTLayer&&)
			/**
			Default move constructor for FMTLayer.
			*/
			FMTLayer(FMTLayer&& rhs) noexcept :
				Core::FMTObject(std::move(rhs)),
				geotransform(std::move(rhs.geotransform)),
				maxx(std::move(rhs.maxx)),
				maxy(std::move(rhs.maxy)),
				SRS_WKT(std::move(rhs.SRS_WKT)),
				cellsize(std::move(rhs.cellsize)),
				mapping(std::move(rhs.mapping))
					{
			
					}
			// DocString: FMTLayer(const std::vector<double>&,const unsigned int&,const unsigned int&,const std::string&,const double&)
			/**
			Constructor for the FMTLayer taking a geotransform, max x, max y, projection string and cell size.
			*/
            FMTLayer(const std::vector<double>& lgeotransform,
                     const unsigned int& lmaxx,
                     const unsigned int& lmaxy,
                     const std::string& lSRS_WKT,
                     const double& lcellsize):Core::FMTObject(),
                     geotransform(lgeotransform),
                     maxx(lmaxx),
                     maxy(lmaxy),
                     SRS_WKT(lSRS_WKT),
                     cellsize(lcellsize),
                     mapping(){}
			// DocString: FMTLayer(const std::map<FMTCoordinate,T>&,const std::vector<double>&,const unsigned int&,const unsigned int&,const std::string&,const double&)
			/**
			Constructor for the FMTLayer used to copy information from an other layer.
			*/
            FMTLayer(const std::map<FMTCoordinate,T>& lmapping,
                     const std::vector<double>& lgeotransform,
                     const unsigned int& lmaxx,
                     const unsigned int& lmaxy,
                     const std::string& lSRS_WKT,
                     const double& lcellsize):
                         Core::FMTObject(),
                geotransform(lgeotransform),
                             maxx(lmaxx),
                             maxy(lmaxy),
                             SRS_WKT(lSRS_WKT),
                             cellsize(lcellsize),
                             mapping(lmapping){}
			// DocString: FMTLayer(const FMTLayer&)
			/**
			Default copy constructor for FMTLayer.
			*/
            FMTLayer(const FMTLayer& rhs):Core::FMTObject(rhs),
                geotransform(rhs.geotransform),
                maxx(rhs.maxx),
                maxy(rhs.maxy),
                SRS_WKT(rhs.SRS_WKT),
                cellsize(rhs.cellsize),
                mapping(rhs.mapping){}
			// DocString: FMTLayer::operator=
			/**
			Default copy assignment for FMTLayer.
			*/
            FMTLayer<T>& operator = (const FMTLayer<T>& rhs)
                {
                if(this!=&rhs)
                    {
                    Core::FMTObject::operator = (rhs);
                    mapping = (rhs.mapping);
                    geotransform = rhs.geotransform;
                    maxx = rhs.maxx;
                    maxy = rhs.maxy;
                    SRS_WKT = rhs.SRS_WKT;
                    cellsize = rhs.cellsize;
                    }
                return *this;
                }
			// DocString: FMTLayer::swap
			/**
			Swap operator for FMTLayer.
			*/
			void swap(FMTLayer<T>& rhs)
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
					_exhandler->raiseFromCatch("", "FMTLayer::swap", __LINE__, __FILE__);
					}
				}

			// DocString: FMTLayer::setExtentFrom
			/**
			Using an other layer (rhs) it's going to set the informations (other than the main map) to this.
			*/
			void setExtentFrom(const FMTLayer<T>& rhs)
				{
				this->geotransform = rhs.geotransform;
				this->maxx = rhs.maxx;
				this->maxy = rhs.maxy;
				this->SRS_WKT = rhs.SRS_WKT;
				this->cellsize = rhs.cellsize;
				}
			// DocString: FMTLayer::copyExtent
			/**
			Create a new FMTLayer by copying informations (other than the main map) to a new FMTLayer from this.
			*/
            template<typename newtype>
            FMTLayer<newtype>copyExtent() const

                {
                return FMTLayer<newtype>(std::map<FMTCoordinate,newtype>(),
                                         this->geotransform,
                                         this->maxx,
                                         this->maxy,
                                         this->SRS_WKT,
                                         this->cellsize);
                }
			// DocString: FMTLayer::general
			/**
			Template specification for adding strings from a layer to a string layer using std::string operator+=.
			*/
			FMTLayer<T>& operator+= (const FMTLayer<T>& rhs)
			{
				return *this;
			}
			// DocString: FMTLayer::getXSize
			/**
			Returns the maximal x value of the FMTLayer.
			*/
            unsigned int getXSize() const
                {
                return maxx;
                }
			// DocString: FMTLayer::getYSize
			/**
			Returns the maximal y value of the FMTLayer.
			*/
            unsigned int getYSize() const
                {
                return maxy;
                }
			// DocString: FMTLayer::getGeoTransform
			/**
			Returns the geotransform of the layer.
			*/
			std::vector<double> getGeoTransform() const
                {
                return geotransform;
                }
			// DocString: FMTLayer::getProjection
			/**
			Returns the projection of the FMTLayer.
			*/
			std::string getProjection() const
                {
                return SRS_WKT;
                }
			// DocString: FMTLayer::getMapping
			/**
			Returns the underlying map of the FMTLayer.
			*/
			std::map<FMTCoordinate,T>getMapping() const
                {
                return mapping;
                }
			// DocString: FMTLayer::area
			/**
			Returns the whole area of the FMTLayer using the size of a pixel.
			*/
            double area() const
                {
                return (cellsize * static_cast<double>(mapping.size()));
                }
			// DocString: FMTLayer::getCellSize
			/**
			Get the cell size for a single coordinate of the map.
			*/
			double getCellSize() const
				{
				return cellsize;
				}
			// DocString: FMTLayer::getAttributes
			/**
			Returns a vector of unique attributes present in the FMTLayer.
			*/
            std::vector<T>getAttributes() const
                {
				std::vector<T>unique_attributes;
                for(typename std::map<FMTCoordinate,T>::const_iterator it = mapping.begin();it != mapping.end(); it++)
                    {
                    if (std::find(unique_attributes.begin(),unique_attributes.end(),it->second)==unique_attributes.end())
                        {
                        unique_attributes.push_back(it->second);
                        }
                    }
                return unique_attributes;
                }
			// DocString: FMTLayer::size
			/**
			Returns the size of the FMTLayer.
			*/
            size_t size() const
                {
                return mapping.size();
                }
			// DocString: FMTLayer::size
			/**
			Replaces values of a layer using iterators coming from an other FMTLayer.
			*/
            void replace(typename std::map<FMTCoordinate,T>::const_iterator first,typename std::map<FMTCoordinate,T>::const_iterator last)
                {
                while (first!=last)
                    {
                    typename std::map<FMTCoordinate,T>::iterator it = mapping.find(first->first);
                    if (it != last)
                        {
                        it->second = first->second;
                        }
                    ++first;
                    }
                }
        };

		template<> inline FMTLayer<std::string>& FMTLayer<std::string>::operator += (const FMTLayer<std::string>& rhs)
		{
			std::map<FMTCoordinate, std::string>new_mapping;
			for (std::map<FMTCoordinate, std::string>::const_iterator mit = mapping.begin(); mit != mapping.end(); mit++)
			{
				std::map<FMTCoordinate, std::string>::const_iterator rhsit = rhs.mapping.find(mit->first);
				if (rhsit != rhs.mapping.end())
				{
					new_mapping[mit->first] = (mit->second + "-" + rhsit->second);
				}
			}
			mapping = new_mapping;
			return *this;
		}

    }

#endif // FMTLAYER_Hm_included
