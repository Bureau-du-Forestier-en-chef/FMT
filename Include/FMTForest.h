/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTFOREST_Hm_included
#define FMTFOREST_Hm_included


#include "FMTLayer.hpp"
#include "FMTDevelopment.h"
#include <iterator>
#include <numeric>

namespace Core {
	class FMTTheme;
	class FMTActualDevelopment;
	class FMTFuturDevelopment;
}


namespace Spatial
    {
    // DocString: FMTForest
    /**
    @brief Spatially referenced layer of Core::FMTDevelopment representing the forest.
    @details FMTForest is a FMTLayer mapping each spatial cell to a FMTDevelopment and provides
    growth, area aggregation and preSolve / postsolve operations on the forest.
    */
    class FMTEXPORT FMTForest: public FMTLayer<Core::FMTDevelopment>
        {
		friend class boost::serialization::access;
		// DocString: FMTForest::serialize
		/**
		@brief Serialize the FMTForest through its base FMTLayer.
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
			{
			ar & boost::serialization::make_nvp("layer", boost::serialization::base_object<FMTLayer<Core::FMTDevelopment>>(*this));
			}
        public:
            // DocString: FMTForest()
            /**
            @brief Default constructor for FMTForest.
            */
            FMTForest();
            // DocString: FMTForest(const FMTForest&)
            /**
            @brief Copy constructor for FMTForest.
            @param[in] rhs the FMTForest to copy.
            */
            FMTForest(const FMTForest& rhs);
            // DocString: FMTForest(const FMTLayer<Core::FMTDevelopment>&)
            /**
            @brief Construct a FMTForest from a layer of developments.
            @param[in] rhs the layer of developments to construct from.
            */
            FMTForest(const FMTLayer<Core::FMTDevelopment>& rhs);
            // DocString: FMTForest::operator=
            /**
            @brief Copy assignment for FMTForest.
            @param[in] rhs the FMTForest to copy.
            @return a reference to this FMTForest.
            */
            FMTForest& operator = (const FMTForest& rhs);
			// DocString: FMTForest::getArea
			/**
			@brief Return the actual developments of the forest, aggregating the area of identical developments by cell size.
			@return the actual developments of the forest.
			*/
			std::vector<Core::FMTActualDevelopment>getArea() const;
			// DocString: FMTForest::setLastThemeWithArea
			/**
			@brief Set the last theme attribute of each development from the area developments matching the other themes.
			@details Raises FMTinvalid_theme if the mask of a development is missing from the area developments.
			@param[in] p_devs the actual developments holding the last theme attribute per mask subset.
			@param[in] p_themes the themes of the model.
			*/
			void setLastThemeWithArea(const std::vector<Core::FMTActualDevelopment>& p_devs,
										const std::vector<Core::FMTTheme>& p_themes);
        // DocString: FMTForest::getCopy
        /**
        @brief Return a copy of the forest layer.
        @param[in] copyData if true copies the development data, otherwise returns an empty layer with the same spatial reference.
        @return the copied forest.
        */
        FMTForest getCopy(bool copyData = true) const;
        // DocString: FMTForest::grow
        /**
        @brief Grow every development of the forest by one period.
        @return a new grown forest.
        */
        FMTForest grow() const;
		// DocString: FMTForest::getThemes
		/**
		@brief Return one string layer per theme with the attribute value of each development.
		@param[in] themes the themes to extract.
		@return a vector of string layers, one per theme.
		*/
		std::vector<FMTLayer<std::string>> getThemes(const std::vector<Core::FMTTheme>& themes) const;
        // DocString: FMTForest::getAge
        /**
        @brief Return a layer with the age of each development.
        @return an int layer of ages.
        */
        FMTLayer<int>getAge() const;
        // DocString: FMTForest::getLock
        /**
        @brief Return a string layer with the lock of each locked development.
        @return a string layer of locks.
        */
        FMTLayer<std::string>getLock() const;
		// DocString: FMTForest::setPeriod
		/**
		@brief Set the period of every development of the forest.
		@param[in] period the period to set.
		*/
		void setPeriod(int period);
		// DocString: FMTForest::preSolve
		/**
		@brief Return a presolved copy of the forest by presolving each development with the mask filter.
		@param[in] filter the mask filter to apply.
		@param[in] presolvedthemes the presolved themes.
		@return the presolved forest.
		*/
		FMTForest preSolve(const Core::FMTMaskFilter& filter, const std::vector<Core::FMTTheme>&presolvedthemes) const;
		// DocString: FMTForest::postSolve
		/**
		@brief Return a postsolved copy of the forest by postsolving each development mask.
		@param[in] filter the mask filter to apply.
		@param[in] originalbasethemes the original base themes.
		@return the postsolved forest.
		*/
		FMTForest postSolve(const Core::FMTMaskFilter& filter, const std::vector<Core::FMTTheme>&originalbasethemes) const;
        };
    }
#endif // FMTFOREST_Hm_included
