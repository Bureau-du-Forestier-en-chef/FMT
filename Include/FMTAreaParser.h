/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTareaparser_Hm_included
#define FMTareaparser_Hm_included

#include "FMTParser.h"
#include <iterator>
#include <map>
#include <string>
#include <vector>
#include "FMTutility.h"
#include <boost/regex.hpp>


class OGRGeometryCollection;
class OGRFeature;
class OGRGeometry;
class OGRMultiPolygon;

namespace Core
{
	class FMTDevelopment;
	class FMTActualDevelopment;
	class FMTGCBMTransition;
	class FMTMask;
	template<typename T>
	class FMTList;
}
namespace Graph
{
	class FMTPredictor;
}

namespace Spatial
{
	class FMTForest;
	class FMTSpatialSchedule;
	class FMTSaSolution;
}

namespace Heuristics
{
	class FMTOperatingAreaScheme;
	class FMTOperatingAreaCluster;
	class FMTOperatingArea;
}

namespace Models
{
	class FMTModel;
}


namespace Parser
{
// DocString: FMTAreaParser
/**
@brief Parser dealing with the spatial aspects used in multiple FMTModel children, reading and writing the area section.
@details Many functionalities require FMT to be compiled with FMTWITHGDAL. Also used by the model parser.
*/
class FMTEXPORT FMTAreaParser : public FMTParser
    {
	public:
#ifdef FMTWITHGDAL
		// DocString: FMTAreaParser::readRasters
		/**
		@brief Generate a FMTForest layer from raster files.
		@param[in] themes the themes.
		@param[in] data_rasters the raster file paths, one per theme.
		@param[in] age the age raster file.
		@param[in] agefactor the age factor.
		@param[in] areafactor the area factor.
		@param[in] lock the optional lock raster file.
		@return the forest layer.
		*/
		Spatial::FMTForest readRasters(const std::vector<Core::FMTTheme>& themes, const std::vector<std::string>& data_rasters,
			const std::string& age, double agefactor = 1.0, double areafactor = 0.0001, std::string lock = "") const;
		// DocString: FMTAreaParser::writeLayer
		/**
		@brief Write a layer into a raster file, with a mapping for categorical variables.
		@tparam T the layer value type.
		@param[in] layer the layer.
		@param[in] location the raster file location.
		@param[in] mapping the categorical mapping.
		@param[in] format the format.
		@return true if the layer is written else false.
		*/
		template<typename T>
		bool writeLayer(const Spatial::FMTLayer<T>& layer, std::string location, const std::map<T, std::string>& mapping, std::string format = "GTiff") const;
		// DocString: FMTAreaParser::writeLayer
		/**
		@brief Write a double layer into a raster file.
		@param[in] layer the layer.
		@param[in] location the raster file location.
		@param[in] format the format.
		@return true if the layer is written else false.
		*/
		bool writeLayer(const Spatial::FMTLayer<double>& layer, std::string location, std::string format = "GTiff") const;
		// DocString: FMTAreaParser::writeForest
		/**
		@brief Write a forest into multiple raster files.
		@param[in] for_layer the forest layer.
		@param[in] themes the themes.
		@param[in] data_rasters the raster file paths, one per theme.
		@param[in] age the age file path.
		@param[in] lock the lock file path.
		@param[in] mapping the categorical mapping per theme.
		@return true if the forest is written else false.
		*/
		bool writeForest(const Spatial::FMTForest& for_layer,
			const std::vector<Core::FMTTheme>& themes,
			const std::vector<std::string>& data_rasters,
			const std::string& age,
			const std::string& lock,
			std::vector<std::map<std::string, std::string>> mapping = std::vector<std::map<std::string, std::string>>()) const;
		// DocString: FMTAreaParser::writeForest
		/**
		@brief Write a forest into a folder.
		@param[in] p_for_layer the forest layer.
		@param[in] p_themes the themes.
		@param[in] p_folder the folder.
		@param[in] p_mapping the categorical mapping per theme.
		@return true if the forest is written else false.
		*/
		bool writeForest(const Spatial::FMTForest& p_for_layer,
			const std::vector<Core::FMTTheme>& p_themes,
			const std::string& p_folder,
			std::vector<std::map<std::string, std::string>> p_mapping = std::vector<std::map<std::string, std::string>>()) const;
		// DocString: FMTAreaParser::writeForestTheme
		/**
		@brief Write a forest theme into a raster file using a mapping.
		@param[in] for_layer the forest layer.
		@param[in] theme the theme.
		@param[in] location the raster file location.
		@param[in] mapping the categorical mapping.
		@param[in] format the format.
		@return true if the theme is written else false.
		*/
		bool writeForestTheme(
			const Spatial::FMTForest& for_layer,
			const Core::FMTTheme& theme,
			const std::string& location,
			const std::map<std::string, std::string>& mapping,
			std::string format = "GTiff") const;
		// DocString: FMTAreaParser::writeDisturbances
		/**
		@brief Write the disturbances into a raster file and return the corresponding GCBM transitions for the period.
		@param[in] location the raster file location.
		@param[in] disturbances the spatial schedule of disturbances.
		@param[in] actions the actions.
		@param[in] themes the themes.
		@param[in] period the period.
		@return the GCBM transitions.
		*/
		std::vector<Core::FMTGCBMTransition> writeDisturbances(const std::string& location,
			const Spatial::FMTSpatialSchedule& disturbances,
			const std::vector<Core::FMTAction>& actions,
			const std::vector<Core::FMTTheme>& themes,
			const int& period) const;
		// DocString: FMTAreaParser::writePredictors()
		/**
		@brief Write the predictor ids into a raster file and return the corresponding predictors for the period.
		@param[in] location the raster file location.
		@param[in] spatialsolution the spatial schedule.
		@param[in] yieldnames the yield names.
		@param[in] model the model.
		@param[in] period the period.
		@param[in] periodonevalues if true uses period one values.
		@param[in] withGCBMid if true writes the GCBM id.
		@return the predictors per period.
		*/
		std::vector<std::vector<Graph::FMTPredictor>> writePredictors(const std::string& location,
			const Spatial::FMTSpatialSchedule& spatialsolution,
			const std::vector<std::string>& yieldnames,
			const Models::FMTModel& model,
			const int& period,
			bool periodonevalues = false,
			bool withGCBMid = true) const;
		// DocString: FMTAreaParser::writeSaSolution
	   /**

	   */
	   /*bool writeSaSolution(const std::string location,
						   const Spatial::FMTSaSolution& solution,
						   const std::vector<Core::FMTTheme>& themes,
						   const std::vector<Core::FMTAction>& actions,
						   const bool& writeevents = true,
						   int periodStart=-1,
						   int periodStop=-1) const;*/
	#ifdef FMTWITHOSI
						   // DocString: FMTAreaParser::getSchemeNeighbors
						   /**
						   @brief Return the operating area schemes with their neighbors filled from a vector file.
						   @param[in] operatingareaparameters the operating area parameters.
						   @param[in] themes the themes.
						   @param[in] data_vectors the vector file.
						   @param[in] agefield the age field name.
						   @param[in] areafield the area field name.
						   @param[in] agefactor the age factor.
						   @param[in] areafactor the area factor.
						   @param[in] lockfield the optional lock field name.
						   @param[in] minimal_area the minimal area below which a feature is not selected.
						   @param[in] buffersize the buffer size.
						   @return the operating area schemes with neighbors.
						   */
		std::vector<Heuristics::FMTOperatingAreaScheme> getSchemeNeighbors(std::vector<Heuristics::FMTOperatingAreaScheme> operatingareaparameters,
			const std::vector<Core::FMTTheme>& themes, const std::string& data_vectors,
			const std::string& agefield, const std::string& areafield, double agefactor = 1.0,
			double areafactor = 1, std::string lockfield = "",
			double minimal_area = 0.0, double buffersize = 100) const;
		// DocString: FMTAreaParser::getClusters
		/**
		@brief Return the operating area clusters with their linker mask from a vector file.
		@param[in] operatingareas the operating areas.
		@param[in] themes the themes.
		@param[in] data_vectors the vector file.
		@param[in] agefield the age field name.
		@param[in] areafield the area field name.
		@param[in] maximaldistance the maximal clustering distance.
		@param[in] agefactor the age factor.
		@param[in] areafactor the area factor.
		@param[in] lockfield the optional lock field name.
		@param[in] minimal_area the minimal area below which a feature is not selected.
		@param[in] buffersize the buffer size.
		@return the operating area clusters.
		*/
		std::vector<Heuristics::FMTOperatingAreaCluster> getClusters(const std::vector<Heuristics::FMTOperatingArea>& operatingareas,
			const std::vector<Core::FMTTheme>& themes, const std::string& data_vectors,
			const std::string& agefield, const std::string& areafield, const double& maximaldistance,
			double agefactor = 1.0, double areafactor = 1, std::string lockfield = "",
			double minimal_area = 0.0, double buffersize = 100) const;
		// DocString: FMTAreaParser::getOperatingArea
		/**
		@brief Return the operating area schemes from a shapefile.
		@param[in] fichierShp the shapefile.
		@param[in] themes the themes.
		@param[in] numeroTheme the theme id.
		@param[in] startingperiod the starting period.
		@param[in] nomChampAge the age field name.
		@param[in] nomChampSuperficie the area field name.
		@param[in] nomChampStanlock the lock field name.
		@param[in] fichierParam the parameters file.
		@return the operating area schemes.
		*/
		std::vector<Heuristics::FMTOperatingAreaScheme> getOperatingArea(const std::string& fichierShp, const std::vector<Core::FMTTheme>& themes, const int& numeroTheme, const int& startingperiod, const std::string& nomChampAge, const std::string& nomChampSuperficie, const std::string& nomChampStanlock, const std::string& fichierParam) const;
		// DocString: FMTAreaParser::readOaSchedulerParameters
		/**
		@brief Return the operating area schemes from a parameters file, for use in the operating area scheduler.
		@param[in] location the parameters file location.
		@param[in] modelthemes the model themes.
		@param[in] themetarget the theme id of the operating area.
		@param[in] startingperiod the starting period.
		@return the operating area schemes.
		*/
		std::vector<Heuristics::FMTOperatingAreaScheme> readOaSchedulerParameters(const std::string& location, const std::vector<Core::FMTTheme>& modelthemes, const int& themetarget, const int& startingperiod) const;
		// DocString: FMTAreaParser::writeOaSchedulerParameters
		/**
		@brief Write a parameters file with the full mask in the OA column into a csv file.
		@param[in] location the file location.
		@param[in] OAschemes the operating area schemes.
		@param[in] creationoptions the creation options.
		*/
		void writeOaSchedulerParameters(const std::string& location, const std::vector<Heuristics::FMTOperatingAreaScheme>& OAschemes,
			std::vector<std::string> creationoptions = std::vector<std::string>()) const;
#endif
		// DocString: FMTAreaParser::readVectors
		/**
		@brief Return the actual developments present in a vector file.
		@param[in] themes the themes.
		@param[in] data_vectors the vector file.
		@param[in] agefield the age field name.
		@param[in] areafield the area field name.
		@param[in] agefactor the age factor.
		@param[in] areafactor the area factor.
		@param[in] lockfield the optional lock field name.
		@param[in] minimalarea the minimal area below which a feature is not selected.
		@return the actual developments.
		*/
		std::vector<Core::FMTActualDevelopment>readVectors(const std::vector<Core::FMTTheme>& themes, const std::string& data_vectors,
			const std::string& agefield, const std::string& areafield, double agefactor = 1.0,
			double areafactor = 1, std::string lockfield = "",
			double minimalarea = 0.0) const;
		// DocString: FMTAreaParser::vectormaptoFMTforest
		/**
		@brief Rasterize a vector file and return the corresponding forest.
		@param[in] data_vectors the vector file.
		@param[in] resolution the resolution.
		@param[in] themes the themes.
		@param[in] agefield the age field name.
		@param[in] areafield the area field name.
		@param[in] agefactor the age factor.
		@param[in] areafactor the area factor.
		@param[in] lockfield the optional lock field name.
		@param[in] minimalarea the minimal area below which a feature is not selected.
		@param[in] writeforestfolder the optional folder to write the forest.
		@param[in] fittoforel if true fits to FOREL.
		@return the forest.
		*/
		Spatial::FMTForest vectormaptoFMTforest(const std::string& data_vectors,
			const int& resolution, const std::vector<Core::FMTTheme>& themes,
			const std::string& agefield, const std::string& areafield, double agefactor = 1.0,
			double areafactor = 1, std::string lockfield = "",
			double minimalarea = 0.0, const std::string& writeforestfolder = "",
			const bool& fittoforel = true) const;
		// DocString: FMTAreaParser::vectormaptoFMTforest
		/**
		@brief Create a raster file of a given resolution from a vector file field.
		@param[in] data_vectors the vector file.
		@param[in] tifpathandname the raster file path.
		@param[in] resolution the resolution.
		@param[in] field the field.
		@param[in] fittoforel if true fits to FOREL.
		*/
		void vectorFieldToRaster(const std::string& data_vectors,
			const std::string& tifpathandname,
			const int& resolution,
			const std::string& field,
			bool fittoforel = true) const;
		// DocString: FMTAreaParser::ogrLayerToRaster
		/**
		@brief Rasterize a layer to 20x20 in memory then reproject it to the given resolution using MODE resampling.
		@param[in] layer the layer.
		@param[in] fieldname the int field to rasterize.
		@param[in] outfilename the output file name.
		@param[in] resolution the resolution.
		@param[in] fittoforel if true aligns with the Quebec FOREL rasters.
		@return the raster dataset.
		*/
		GDALDataset* ogrLayerToRaster(OGRLayer* layer, const std::string& fieldname, const std::string& outfilename, const int& resolution, const bool& fittoforel) const;

		// DocString: FMTAreaParser::writeForestExtended
		/**
		@brief Write an extended forest to a folder using a vector file.
		@param[in,out] p_forest the forest.
		@param[in] p_VectorFile the vector file.
		@param[in] p_themes the themes.
		@param[in] p_area the actual developments.
		@param[in] p_folder the folder.
		*/
		void writeForestExtended(Spatial::FMTForest& p_forest,const std::string& p_VectorFile,
						const std::vector<Core::FMTTheme> p_themes,
						const std::vector<Core::FMTActualDevelopment>& p_area,
							const std::string& p_folder) const;

#endif
		// DocString: FMTAreaParser()
		/**
		@brief Default constructor for FMTAreaParser.
		*/
		FMTAreaParser();
		// DocString: ~FMTAreaParser()
		/**
		@brief Default destructor for FMTAreaParser.
		*/
		~FMTAreaParser() = default;
		// DocString: FMTAreaParser(const FMTAreaParser&)
		/**
		@brief Copy constructor for FMTAreaParser.
		@param[in] rhs the FMTAreaParser to copy.
		*/
		FMTAreaParser(const FMTAreaParser& rhs) = default;
		// DocString: FMTAreaParser::operator=
		/**
		@brief Copy assignment operator for FMTAreaParser.
		@param[in] rhs the FMTAreaParser to copy.
		@return a reference to this FMTAreaParser.
		*/
		FMTAreaParser& operator = (const FMTAreaParser& rhs) = default;
		// DocString: FMTAreaParser::read
		/**
		@brief Read a regular area section and return the actual developments.
		@param[in] themes the themes.
		@param[in] constants the constants.
		@param[in] location the file location.
		@return the actual developments.
		*/
		std::vector<Core::FMTActualDevelopment>read(const std::vector<Core::FMTTheme>& themes, const Core::FMTConstants& constants, const std::string& location);
		// DocString: FMTAreaParser::write
		/**
		@brief Write an area section from a vector of actual developments.
		@param[in] areas the actual developments.
		@param[in] location the file location.
		*/
		void write(const std::vector<Core::FMTActualDevelopment>& areas, const std::string& location) const;
    private:
		// DocString: FMTAreaParser::m_rxcleanarea
		///This regex is used to capture the information kept in the .are section.
        const static boost::regex m_rxcleanarea;
		// DocString: FMTAreaParser::m_RxExclude
		///To capture the exclude keyword
		const static boost::regex m_RxExclude;
		// DocString: FMTAreaParser::m_RxExcludeSpec
		///To capture the line of the exclude
		const static boost::regex m_RxExcludeSpec;
		// DocString: FMTAreaParser::_isExclude
		/**
		@brief Test if a line is an exclude line.
		@param[in] pm_line the line.
		@return true if the line is an exclude else false.
		*/
		bool _isExclude(const std::string& pm_line) const;
		// DocString: FMTAreaParser::_getExcludeValue
		/**
		@brief Return the value of an exclude line.
		@param[in] pm_line the line.
		@return the excluded value, empty if nothing to exclude.
		*/
		std::string _getExcludeValue(const std::string& pm_line) const;
		// DocString: FMTAreaParser::_getExcludedSpec
		/**
		@brief Return the mask and spec of an exclude line.
		@param[in] p_themes the themes.
		@param[in] p_constants the constants.
		@param[in] p_value the excluded line.
		@return the mask and spec.
		*/
		std::pair<Core::FMTMask, Core::FMTSpec> _getExcludedSpec(const std::vector<Core::FMTTheme>& p_themes,
																const Core::FMTConstants& p_constants,
																const std::string& p_value) const;
		// DocString: FMTAreaParser::_gotNewExclude
		/**
		@brief Return the mask and spec of an exclude line and add it to the list.
		@param[in] p_themes the themes.
		@param[in] p_constants the constants.
		@param[in] p_value the excluded line.
		@param[in,out] p_list the list of specs.
		@return true if a new one is added or an exclude section is found else false.
		*/
		bool _gotNewExclude(const std::vector<Core::FMTTheme>& p_themes,
			const Core::FMTConstants& p_constants,
			const std::string& p_value,
			Core::FMTList<Core::FMTSpec>& p_list) const;
		// DocString: FMTAreaParser::_getPeriodPathName
		/**
		@brief Return the path to a disturbance layer raster file for a period.
		@param[in] location the folder.
		@param[in] period the period.
		@param[in] name the name.
		@return the raster file path.
		*/
		std::string _getPeriodPathName(const std::string& location, const int& period,const std::string& name) const;
		// DocString: FMTAreaParser::_getGCBMtransitions
		/**
		@brief Return the GCBM transitions from the disturbances of a FMTSesModel, for use with GCBM.
		@param[in] stacked_actions the stacked actions.
		@param[in] ages the last ages.
		@param[in] newfor the new forest.
		@param[in] themes the themes.
		@return the GCBM transitions.
		*/
		std::vector<Core::FMTGCBMTransition> _getGCBMtransitions(const Spatial::FMTLayer<std::string>& stacked_actions,
													const Spatial::FMTLayer<int>& ages,
													const Spatial::FMTForest& newfor,
													const std::vector<Core::FMTTheme>& themes) const;
		#ifdef FMTWITHGDAL
			// DocString: FMTAreaParser::_writeBand
			/**
			@brief Write a layer into a raster band.
			@tparam T the layer value type.
			@tparam outT the output type.
			@param[in] layer the layer.
			@param[in,out] wband the raster band.
			@param[in] mapping the categorical mapping.
			@return true if the band is written else false.
			*/
			template<typename T,typename outT>
			bool _writeBand(const Spatial::FMTLayer<T>& layer, GDALRasterBand* wband, const std::map<T, std::string>& mapping) const;
			// DocString: FMTAreaParser::_getUnion
			/**
			@brief Merges overlapping or adjacent polygons within each multipolygon using a cascaded union algorithm.
			@details Iterates through the provided vector of multipolygons. For each non-empty multipolygon,
			         it combines its internal polygonal parts into a single optimized geometry.
			@param[in] p_collections Vector of OGRMultiPolygon objects to be unified.
			@return A vector of pointers to the merged `OGRGeometry` objects. The output vector retains the
			        exact same size and indexing as `p_collections`. Elements remain `nullptr` if the
			        corresponding source multipolygon was empty.
			@note The caller takes ownership of the returned `OGRGeometry*` pointers and is responsible for
			      deleting them using `OGRGeometryFactory::destroyGeometry` to prevent memory leaks.
			@exception Re-throws intercepted errors via `_exhandler->raisefromcatch` if the union operation fails.
			 */
			std::vector<OGRGeometry*> _getUnion(const std::vector<OGRMultiPolygon>& p_collections) const;
			// DocString: FMTareaparser::_destroyGeometries
			/**
			@brief Safely releases heap memory for all OGRGeometry pointers in the vector and clears the container.
			@details Iterates through the vector to destroy each geometry instance using the GDAL/OGR
			         `OGRGeometryFactory::destroyGeometry` factory method. Passing `nullptr` elements is
			         safely handled by the GDAL factory. The vector is cleared of all elements upon completion.
			@param[in,out] p_geometires Vector of OGRGeometry pointers to be destroyed. Modified in-place and emptied.
			@note This method should be called on the output of methods like `GetUnion` to prevent memory leaks.
			@exception Re-throws intercepted errors via `_exhandler->raisefromcatch` if the memory cleanup fails.
			 */
			void _destroyGeometries(std::vector<OGRGeometry*>& p_geometires) const;
			// DocString: FMTAreaParser::getFeatureToDevelopment
			/**
			@brief Convert a feature into an actual development.
			@param[in] feature the feature.
			@param[in] themes the themes.
			@param[in] themes_fields the theme field indices.
			@param[in] age_field the age field index.
			@param[in] lock_field the lock field index.
			@param[in] area_field the area field index.
			@param[in] agefactor the age factor.
			@param[in] areafactor the area factor.
			@param[in] minimalarea the minimal area.
			@return the actual development.
			*/
			Core::FMTActualDevelopment _getFeatureToDevelopment(const OGRFeature* feature,
															const std::vector<Core::FMTTheme>& themes,
															const std::map<int, int>& themes_fields,
															const int& age_field,
															const int& lock_field,
															const int& area_field,
															const double& agefactor,
															const double& areafactor,
															const double& minimalarea) const;
			// DocString: FMTAreaParser::_validateRaster
			/**
			@brief Validate that a vector of rasters are perfectly part of each other and can be treated as a stack, throwing on error.
			@param[in] data_rasters the raster file paths.
			*/
			void _validateRaster(const std::vector<std::string>&data_rasters) const;
			// DocString: FMTAreaParser::_openVectorFile
			/**
			@brief Open a vector file and return a dataset if all mandatory fields are present, throwing otherwise, filling the field indices.
			@param[in,out] themes_fields the theme field indices.
			@param[in,out] age_field the age field index.
			@param[in,out] lock_field the lock field index.
			@param[in,out] area_field the area field index.
			@param[in] data_vectors the vector file.
			@param[in] agefield the age field name.
			@param[in] areafield the area field name.
			@param[in] lockfield the lock field name.
			@param[in] themes the themes.
			@return the dataset.
			*/
			GDALDataset* _openVectorFile(std::map<int, int>&themes_fields, int& age_field, int& lock_field, int& area_field,
				const std::string& data_vectors, const std::string& agefield, const std::string& areafield, const std::string& lockfield,
				const std::vector<Core::FMTTheme>& themes) const;
			#ifdef FMTWITHOSI
			// DocString: FMTareaparser::_getMultiPolygons
			/**
			@brief Extracts and groups valid multipolygons from a vector layer, indexed by operating area.
			@details Opens the specified vector data source, applies thematic and attribute filters, and processes
			         each feature based on age, area, and lock constraints. Features meeting the criteria are converted
			         and appended to the corresponding `OGRMultiPolygon` container matching the feature's operating area mask.
			@param[in] operatingareas Vector of target operating areas used to classify and index the multipolygons.
			@param[in] themes Vector of FMT themes used for configuration and data filtering.
			@param[in] data_vectors File path or connection string to the vector data source.
			@param[in] agefield Name of the attribute field representing age.
			@param[in] areafield Name of the attribute field representing area.
			@param[in] agefactor Multiplier factor applied to the parsed age attribute values (Default = 1.0).
			@param[in] areafactor Multiplier factor applied to the parsed area attribute values (Default = 1.0).
			@param[in] lockfield Name of the attribute field representing the lock status (Default = "").
			@param[in] minimal_area Minimum required surface area threshold to retain a feature (Default = 0.0).
			@return A vector of `OGRMultiPolygon` objects matching the exact size of `operatingareas`.
			        Each index holds the compiled valid multipolygons belonging to that specific operating area.
			@note Only valid spatial features that pass the `minimal_area` threshold are appended to the resulting collection.
			 */
			std::vector<OGRMultiPolygon> _getMultiPolygons(const std::vector<Heuristics::FMTOperatingArea>& operatingareas,
				const std::vector<Core::FMTTheme>& themes, const std::string& data_vectors,
				const std::string& agefield, const std::string& areafield, double agefactor = 1.0,
				double areafactor = 1, std::string lockfield = "",
				double minimal_area = 0.0) const;
			// DocString: FMTareaparser::_getNeighborsFromPolygons
			/**
			@brief Identifies and assigns mutual neighboring operating areas by evaluating spatial intersections using buffers.
			@details For each operating area, its corresponding geometry is buffered by a given distance to find intersections
			         with other areas. A neighbor is initially considered if the intersection area ratio meets a threshold defined
			         by `getneihgborsperimeter()`. A final reciprocity pass ensures that two areas are only saved as neighbors
			         if the neighborhood relationship is mutual (A recognizes B, and B recognizes A).
			@param[in] polygons Vector of OGRGeometry pointers representing the spatial boundary of each operating area.
			@param[in] operatingareas A copy of the operating areas vector to be updated with neighborhood information.
			@param[in] buffersize The buffer distance (double) used to expand geometries to test for spatial adjacency.
			@return A new vector of `Heuristics::FMToperatingarea` with updated neighborhood masks, maintaining the same size
			        and order as the input parameters.
			@note This method safely handles memory management for all internal intermediary geometries (`buffered` and `intersect`)
			      using `OGRGeometryFactory::destroyGeometry`.
			@exception Re-throws intercepted errors via `_exhandler->raisefromcatch` if geometry buffering or intersection math fails.
			 */
			std::vector<Heuristics::FMTOperatingArea> _getNeighborsFromPolygons(const std::vector<OGRGeometry*>& polygons,
				std::vector<Heuristics::FMTOperatingArea> operatingareas,
				const double& buffersize) const;

			// DocString: FMTAreaParser::_getClustersFromPolygons
			/**
			@brief Return the potential clusters of operating areas from polygons and a maximal clustering distance.
			@param[in] polygons the polygons.
			@param[in] operatingareas the operating areas.
			@param[in] maximaldistance the maximal clustering distance.
			@return the operating area clusters.
			*/
			std::vector<Heuristics::FMTOperatingAreaCluster> _getClustersFromPolygons(const std::vector<OGRGeometry*>&polygons,
																		const std::vector<Heuristics::FMTOperatingArea>& operatingareas,const double& maximaldistance) const;
			// DocString: FMTAreaParser::_isMapWithSameThemes
			/**
			@brief Return true if a vector map has the same themes as the model.
			@param[in] p_themes the themes.
			@param[in] p_VectorsMap the vector map.
			@return true if the map has the same themes else false.
			*/
			bool _isMapWithSameThemes(const std::vector<Core::FMTTheme>& p_themes,
				const std::string& p_VectorsMap) const;
			// DocString: FMTareaparser::_GetGeometryArea
			/**
			 * @brief Calculates the surface area of a given geometry if it is a polygon or multipolygon.
			 * @param[in] p_geometry Pointer to the OGRGeometry object to evaluate. Can be nullptr.
			 * @return The calculated area as a double; returns 0.0 if the geometry is invalid,
			 *         nullptr, or not a polygonal type.
			 */
			static double _GetGeometryArea(const OGRGeometry* p_geometry);
		#endif
			// DocString: FMTAreaParser::_getFMTforestfromlayer
			/**
			@brief Rasterize a layer and return the corresponding forest.
			@param[in] layer the layer.
			@param[in] actualdevs the actual developments.
			@param[in] devidfield the development id field.
			@param[in] resolution the resolution.
			@param[in] areafactor the area factor.
			@param[in] fittoforel if true aligns with the Quebec FOREL rasters.
			@return the forest.
			*/
			Spatial::FMTForest _getFMTforestfromlayer(OGRLayer* layer,const std::vector<Core::FMTActualDevelopment>& actualdevs, const std::string& devidfield, const int& resolution, const double& areafactor,const bool& fittoforel) const;
			// DocString: FMTAreaParser::_subsetLayer
			/**
			@brief Return a layer with only the non null features of a layer.
			@param[in,out] layer the layer.
			@param[in] themes the themes.
			@param[in] agefield the age field name.
			@param[in] areafield the area field name.
			@return the subset layer.
			*/
			OGRLayer* _subsetLayer(OGRLayer*layer, const std::vector<Core::FMTTheme>& themes,
								const std::string& agefield, const std::string& areafield) const;
	#endif
			
    };
}
#endif // FMTareaparser_Hm_included

