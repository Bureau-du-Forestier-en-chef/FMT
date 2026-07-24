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
	@brief Template class used like a single raster file, holding the coordinates of each pixel in a map along with the raster projection.
	@details Mostly generated using the FMTAreaParser.
	@tparam T the type of the value held for each pixel.
	*/
    template <typename T>
    class FMTLayer : public Core::FMTObject
        {
		friend class boost::serialization::access;
		// DocString: FMTLifespans::serialize
		/**
		@brief Serialize the FMTLayer through its base FMTObject for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTObject>(*this));
			ar & boost::serialization::make_nvp("geotransform", m_geotransform);
			ar & boost::serialization::make_nvp("maxx", m_maxx);
			ar & boost::serialization::make_nvp("maxy", m_maxy);
			ar & boost::serialization::make_nvp("SRS_WKT", m_SRS_WKT);
			ar & boost::serialization::make_nvp("cellsize", m_cellsize);
			ar & boost::serialization::make_nvp("mapping", m_mapping);
		}
        protected:
		// DocString: FMTLayer::m_geotransform
		///Geotransform of the map (see GDAL for more information about Geotransform)
		std::vector<double>m_geotransform;
		// DocString: FMTLayer::m_maxx
		///Maximal x value in the map
        unsigned int m_maxx;
		// DocString: FMTLayer::m_maxy
		///Maximal y value in the map
		unsigned int m_maxy;
		// DocString: FMTLayer::m_SRS_WKT
		///Projection string of the raster (see GDAL for more information about Geotransform)
		std::string m_SRS_WKT;
		// DocString: FMTLayer::m_cellsize
		///Size of the pixel in the unit used by the map (SRS_WKT)
        double m_cellsize;
		// DocString: FMTLayer::m_mapping
		///std::map keeping the information of each pixel.
		std::map<FMTCoordinate, T>m_mapping;
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
			@brief Access operator to the mapping of the layer.
			@param[in] coordinate the coordinate.
			@return a reference to the value at the coordinate.
			*/
			T& operator [](const FMTCoordinate& coordinate)
				{
				return m_mapping[coordinate];
				}
			// DocString: FMTLayer::at
			/**
			@brief Access the value at a given coordinate.
			@param[in] coordinate the coordinate.
			@return a const reference to the value at the coordinate.
			*/
			const T& at(const FMTCoordinate& coordinate) const
			{
				return m_mapping.at(coordinate);
			}
			// DocString: FMTLayer::empty
			/**
			@brief Return true if the layer is empty.
			@return true if the layer is empty else false.
			*/
			inline bool empty() const
			{
				return m_mapping.empty();
			}
			// DocString: FMTLayer::begin
			/**
			@brief Return an iterator to the beginning of the layer.
			@return an iterator to the beginning.
			*/
			iterator begin()
			{
				return m_mapping.begin();
			}
			// DocString: FMTLayer::begin
			/**
			@brief Return a const iterator to the beginning of the layer.
			@return a const iterator to the beginning.
			*/
			const_iterator begin() const
			{
				return m_mapping.begin();
			}
			// DocString: FMTLayer::end
			/**
			@brief Return an iterator to the end of the layer.
			@return an iterator to the end.
			*/
			iterator  end()
			{
				return m_mapping.end();
			}
			// DocString: FMTLayer::end
			/**
			@brief Return a const iterator to the end of the layer.
			@return a const iterator to the end.
			*/
			const_iterator end() const
			{
				return m_mapping.end();
			}
			// DocString: FMTLayer::find
			/**
			@brief Find the layer element at a given coordinate.
			@param[in] coordinate the coordinate.
			@return a const iterator to the element.
			*/
			const_iterator find(const FMTCoordinate& coordinate) const
			{
				return m_mapping.find(coordinate);
			}
			// DocString: FMTLayer::find
			/**
			@brief Find the layer element at a given coordinate.
			@param[in] coordinate the coordinate.
			@return an iterator to the element.
			*/
			iterator find(const FMTCoordinate& coordinate)
			{
				return m_mapping.find(coordinate);
			}
			// DocString: FMTLayer()
			/**
			@brief Default constructor for FMTLayer.
			*/
            FMTLayer():Core::FMTObject(),m_geotransform(),m_maxx(),m_maxy(),m_SRS_WKT(),m_cellsize(),m_mapping(){}
			// DocString: ~FMTLayer()
			/**
			@brief Default virtual destructor for FMTLayer.
			*/
            virtual~FMTLayer()=default;
			// DocString: FMTLayer(FMTLayer&&)
			/**
			@brief Default move constructor for FMTLayer.
			@param[in,out] rhs the layer to move from.
			*/
			FMTLayer(FMTLayer&& rhs) noexcept :
				Core::FMTObject(std::move(rhs)),
				m_geotransform(std::move(rhs.m_geotransform)),
				m_maxx(std::move(rhs.m_maxx)),
				m_maxy(std::move(rhs.m_maxy)),
				m_SRS_WKT(std::move(rhs.m_SRS_WKT)),
				m_cellsize(std::move(rhs.m_cellsize)),
				m_mapping(std::move(rhs.m_mapping))
					{
			
					}
			// DocString: FMTLayer(const std::vector<double>&,const unsigned int&,const unsigned int&,const std::string&,const double&)
			/**
			@brief Construct a layer from a geotransform, max x, max y, projection string and cell size.
			@param[in] lgeotransform the geotransform.
			@param[in] lmaxx the maximal x value.
			@param[in] lmaxy the maximal y value.
			@param[in] lSRS_WKT the projection string.
			@param[in] lcellsize the cell size.
			*/
            FMTLayer(const std::vector<double>& lgeotransform,
                     const unsigned int& lmaxx,
                     const unsigned int& lmaxy,
                     const std::string& lSRS_WKT,
                     const double& lcellsize):Core::FMTObject(),
                     m_geotransform(lgeotransform),
                     m_maxx(lmaxx),
                     m_maxy(lmaxy),
                     m_SRS_WKT(lSRS_WKT),
                     m_cellsize(lcellsize),
                     m_mapping(){}
			// DocString: FMTLayer(const std::map<FMTCoordinate,T>&,const std::vector<double>&,const unsigned int&,const unsigned int&,const std::string&,const double&)
			/**
			@brief Construct a layer by copying the information from a mapping and the extent.
			@param[in] lmapping the mapping.
			@param[in] lgeotransform the geotransform.
			@param[in] lmaxx the maximal x value.
			@param[in] lmaxy the maximal y value.
			@param[in] lSRS_WKT the projection string.
			@param[in] lcellsize the cell size.
			*/
            FMTLayer(const std::map<FMTCoordinate,T>& lmapping,
                     const std::vector<double>& lgeotransform,
                     const unsigned int& lmaxx,
                     const unsigned int& lmaxy,
                     const std::string& lSRS_WKT,
                     const double& lcellsize):
                         Core::FMTObject(),
                m_geotransform(lgeotransform),
                             m_maxx(lmaxx),
                             m_maxy(lmaxy),
                             m_SRS_WKT(lSRS_WKT),
                             m_cellsize(lcellsize),
                             m_mapping(lmapping){}
			// DocString: FMTLayer(const FMTLayer&)
			/**
			@brief Copy constructor for FMTLayer.
			@param[in] rhs the FMTLayer to copy.
			*/
            FMTLayer(const FMTLayer& rhs):Core::FMTObject(rhs),
                m_geotransform(rhs.m_geotransform),
                m_maxx(rhs.m_maxx),
                m_maxy(rhs.m_maxy),
                m_SRS_WKT(rhs.m_SRS_WKT),
                m_cellsize(rhs.m_cellsize),
                m_mapping(rhs.m_mapping){}
			// DocString: FMTLayer::operator=
			/**
			@brief Copy assignment operator for FMTLayer.
			@param[in] rhs the FMTLayer to copy.
			@return a reference to this FMTLayer.
			*/
            FMTLayer<T>& operator = (const FMTLayer<T>& rhs)
                {
                if(this!=&rhs)
                    {
                    Core::FMTObject::operator = (rhs);
                    m_mapping = (rhs.m_mapping);
                    m_geotransform = rhs.m_geotransform;
                    m_maxx = rhs.m_maxx;
                    m_maxy = rhs.m_maxy;
                    m_SRS_WKT = rhs.m_SRS_WKT;
                    m_cellsize = rhs.m_cellsize;
                    }
                return *this;
                }
			// DocString: FMTLayer::swap
			/**
			@brief Swap this layer with another.
			@param[in,out] rhs the layer to swap with.
			*/
			void swap(FMTLayer<T>& rhs)
				{
				try {
					m_mapping.swap(rhs.m_mapping);
					m_geotransform.swap(rhs.m_geotransform);
					std::swap(m_maxx, rhs.m_maxx);
					std::swap(m_maxy, rhs.m_maxy);
					std::swap(m_SRS_WKT, rhs.m_SRS_WKT);
					std::swap(m_cellsize, rhs.m_cellsize);
				}catch (...)
					{
					_exhandler->raiseFromCatch("", "FMTLayer::swap", __LINE__, __FILE__);
					}
				}

			// DocString: FMTLayer::setExtentFrom
			/**
			@brief Set the extent information (other than the mapping) from another layer.
			@param[in] rhs the layer to copy the extent from.
			*/
			void setExtentFrom(const FMTLayer<T>& rhs)
				{
				this->m_geotransform = rhs.m_geotransform;
				this->m_maxx = rhs.m_maxx;
				this->m_maxy = rhs.m_maxy;
				this->m_SRS_WKT = rhs.m_SRS_WKT;
				this->m_cellsize = rhs.m_cellsize;
				}
			// DocString: FMTLayer::copyExtent
			/**
			@brief Create a new layer by copying the extent information (other than the mapping) from this layer.
			@tparam newtype the value type of the new layer.
			@return the new layer with the copied extent.
			*/
            template<typename newtype>
            FMTLayer<newtype>copyExtent() const

                {
                return FMTLayer<newtype>(std::map<FMTCoordinate,newtype>(),
                                         this->m_geotransform,
                                         this->m_maxx,
                                         this->m_maxy,
                                         this->m_SRS_WKT,
                                         this->m_cellsize);
                }
			// DocString: FMTLayer::general
			/**
			@brief Addition assignment operator for FMTLayer.
			@param[in] rhs the layer to add.
			@return a reference to this FMTLayer.
			*/
			FMTLayer<T>& operator+= (const FMTLayer<T>& rhs)
			{
				return *this;
			}
			// DocString: FMTLayer::getXSize
			/**
			@brief Return the maximal x value of the layer.
			@return the maximal x value.
			*/
            unsigned int getXSize() const
                {
                return m_maxx;
                }
			// DocString: FMTLayer::getYSize
			/**
			@brief Return the maximal y value of the layer.
			@return the maximal y value.
			*/
            unsigned int getYSize() const
                {
                return m_maxy;
                }
			// DocString: FMTLayer::getGeoTransform
			/**
			@brief Return the geotransform of the layer.
			@return the geotransform.
			*/
			std::vector<double> getGeoTransform() const
                {
                return m_geotransform;
                }
			// DocString: FMTLayer::getProjection
			/**
			@brief Return the projection of the layer.
			@return the projection.
			*/
			std::string getProjection() const
                {
                return m_SRS_WKT;
                }
			// DocString: FMTLayer::getMapping
			/**
			@brief Return the underlying map of the layer.
			@return the mapping.
			*/
			std::map<FMTCoordinate,T>getMapping() const
                {
                return m_mapping;
                }
			// DocString: FMTLayer::area
			/**
			@brief Return the whole area of the layer using the size of a pixel.
			@return the area of the layer.
			*/
            double area() const
                {
                return (m_cellsize * static_cast<double>(m_mapping.size()));
                }
			// DocString: FMTLayer::getCellSize
			/**
			@brief Return the cell size of the layer.
			@return the cell size.
			*/
			double getCellSize() const
				{
				return m_cellsize;
				}
			// DocString: FMTLayer::getAttributes
			/**
			@brief Return the unique attributes present in the layer.
			@return the unique attributes.
			*/
            std::vector<T>getAttributes() const
                {
				std::vector<T>unique_attributes;
                for(typename std::map<FMTCoordinate,T>::const_iterator it = m_mapping.begin();it != m_mapping.end(); it++)
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
			@brief Return the size of the layer.
			@return the size of the layer.
			*/
            size_t size() const
                {
                return m_mapping.size();
                }
			// DocString: FMTLayer::size
			/**
			@brief Replace the values of the layer using iterators from another layer.
			@param[in] first the first iterator.
			@param[in] last the last iterator.
			*/
            void replace(typename std::map<FMTCoordinate,T>::const_iterator first,typename std::map<FMTCoordinate,T>::const_iterator last)
                {
                while (first!=last)
                    {
                    typename std::map<FMTCoordinate,T>::iterator it = m_mapping.find(first->first);
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
			for (std::map<FMTCoordinate, std::string>::const_iterator mit = m_mapping.begin(); mit != m_mapping.end(); mit++)
			{
				std::map<FMTCoordinate, std::string>::const_iterator rhsit = rhs.m_mapping.find(mit->first);
				if (rhsit != rhs.m_mapping.end())
				{
					new_mapping[mit->first] = (mit->second + "-" + rhsit->second);
				}
			}
			m_mapping = new_mapping;
			return *this;
		}

    }

#endif // FMTLAYER_Hm_included
