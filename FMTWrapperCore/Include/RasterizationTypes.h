#ifndef FMTWRAPPERCORE_RASTERIZATIONTYPES_HEADER
#define FMTWRAPPERCORE_RASTERIZATIONTYPES_HEADER

/**
 * @file RasterizationTypes.h
 * @brief Rasterization DTOs: std types only. This is the translation contract the
 *        wrapper sees, through Controller.h.
 */

#include <string>

namespace FMTWrapperCore
{
    /**
     * @brief Parameters of the rasterization of a vector file.
     */
    struct RasterizationParameters
    {
        /** Vector file to rasterize. */
        std::string vectorFilePath;
        /** Folder where the forest rasters are written. */
        std::string outputFolder;
        /** Resolution of the produced rasters (size of a cell). */
        int resolution = 0;
        /** Field of the vector file holding the age. */
        std::string ageField;
        /** Field of the vector file holding the area. */
        std::string areaField;
        /** Field of the vector file holding the lock; empty if there is none. */
        std::string lockField;
    };
}

#endif // FMTWRAPPERCORE_RASTERIZATIONTYPES_HEADER
