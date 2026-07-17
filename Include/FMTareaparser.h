/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTareaparser_Hm_included
#define FMTareaparser_Hm_included

#include "FMTparser.h"
#include <iterator>
#include <map>
#include <string>
#include <vector>
#include "FMTutility.h"
#include <boost/regex.hpp>


class OGRPolygon;
class OGRMultiPolygon;
class OGRFeature;

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
FMTAreaParser is a important parser in FMT because this parser deals with spatial stuff used in multiple FMTModel children.
If FMT is compiled without the compile proprocessor FMTWITHGDAL than alot of funcionalities of the FMTAreaParser wont be
available to the user. This class is also used by the FMTModelParser.
*/
class FMTEXPORT FMTAreaParser : public FMTParser
    {
	public:
#ifdef FMTWITHGDAL
		// DocString: FMTAreaParser::readRasters
		/**
		Using a complete vector of (themes), a vector of raster files path (data_rasters) each raster represent a theme, an (age) raster file and some optional parameters
		(agefactor=1.0),(areafactor=0.0001) to multiply with the actualdevelopment age and area and a optional (lock) raster file.
		The function generates a FMTForest layer from those rasters files.
		*/
		Spatial::FMTForest readRasters(const std::vector<Core::FMTTheme>& themes, const std::vector<std::string>& data_rasters,
			const std::string& age, double agefactor = 1.0, double areafactor = 0.0001, std::string lock = "") const;
		// DocString: FMTAreaParser::writeLayer
		/**
		Using a layer of a given type T the function will write this (layer) into a raster file (location). the mapping add
		a table to the raster file when dealing with categorical variables
		*/
		template<typename T>
		bool writeLayer(const Spatial::FMTLayer<T>& layer, std::string location, const std::map<T, std::string>& mapping, std::string format = "GTiff") const;
		// DocString: FMTAreaParser::writeLayer
		/**
		Using a layer of a given type double the function will write this (layer) into a raster file (location). the mapping add
		a table to the raster file when dealing with categorical variables
		*/
		bool writeLayer(const Spatial::FMTLayer<double>& layer, std::string location, std::string format = "GTiff") const;
		// DocString: FMTAreaParser::writeForest
		/**
		The function will write a complete FMTForest (for_layer) using a complete vector of (themes), in multiple (data_rasters) file paths
		number of paths should be equal to number of themes an (age) file path and (lock) file path.
		The generated .tiff files can have categorical values but it needs to be specified in the
		mapping vector each element of the vector represent a corresponging key to write in the categorical dataset of the raster.
		*/
		bool writeForest(const Spatial::FMTForest& for_layer,
			const std::vector<Core::FMTTheme>& themes,
			const std::vector<std::string>& data_rasters,
			const std::string& age,
			const std::string& lock,
			std::vector<std::map<std::string, std::string>> mapping = std::vector<std::map<std::string, std::string>>()) const;
		// DocString: FMTAreaParser::writeForest
		/**
		Write FMT forest in a folder.
		*/
		bool writeForest(const Spatial::FMTForest& p_for_layer,
			const std::vector<Core::FMTTheme>& p_themes,
			const std::string& p_folder,
			std::vector<std::map<std::string, std::string>> p_mapping = std::vector<std::map<std::string, std::string>>()) const;
		// DocString: FMTAreaParser::writeForestTheme
		/**
		Write a forest theme based on a mapping for a FMTForest with a given file format in raster file.
		*/
		bool writeForestTheme(
			const Spatial::FMTForest& for_layer,
			const Core::FMTTheme& theme,
			const std::string& location,
			const std::map<std::string, std::string>& mapping,
			std::string format = "GTiff") const;
		// DocString: FMTAreaParser::writeDisturbances
		/**
		Giving a .tif file (location) and a disturbancesstack (disturbances) the actual forest (for_layer) and the last forest layer (out_layer).
		a complete vector of model (themes) and a optional (mapping) for the disturbance stack layer created.
		The function will write all the disturbances in the locaiton .tif file and it will also returns the corresponding GCBMtransition for
		this planning period.
		*/
		std::vector<Core::FMTGCBMTransition> writeDisturbances(const std::string& location,
			const Spatial::FMTSpatialSchedule& disturbances,
			const std::vector<Core::FMTAction>& actions,
			const std::vector<Core::FMTTheme>& themes,
			const int& period) const;
		// DocString: FMTAreaParser::writePredictors()
		/**
		Giving a .tif file (location) and a spatialschedule (spatialsolution).
		a complete vector of model (yieldnames) and  a yield section (yields)
		The function will write all the predictor id in the location .tif file and it will also returns the corresponding predictors for
		this planning period.
		*/
		std::vector<std::vector<Graph::FMTPredictor>> writePredictors(const std::string& location,
			const Spatial::FMTSpatialSchedule& spatialsolution,
			const std::vector<std::string>& yieldnames,
			const Models::FMTModel& model,
			const int& period,
			bool periodonevalues = false,
			bool withGCBMid = true) const;
		// DocString: FMTAreaParser::writesasolution
	   /**

	   */
	   /*bool writesasolution(const std::string location,
						   const Spatial::FMTSaSolution& solution,
						   const std::vector<Core::FMTTheme>& themes,
						   const std::vector<Core::FMTAction>& actions,
						   const bool& writeevents = true,
						   int periodStart=-1,
						   int periodStop=-1) const;*/
	#ifdef FMTWITHOSI
						   // DocString: FMTAreaParser::getSchemeNeighbors
						   /**
						   Using a vector of operating area (operatingareaparameters), a complete vector of FMTTheme (themes), a vector file (data_vectors),
						   the name of the age field name (agefield) an area field name (areafield), an (gefactor), an (areafactor), an optional (lockfield) name,
						   a (minimal_area) : the minimal area parameters indicate that if a feature has an area lower than the minimal area it wont be selected.
						   For (buffersize) see getNeighborsFromPolygons function. The returned operating area will have theirs neighboors vector filled.
						   */
		std::vector<Heuristics::FMTOperatingAreaScheme> getSchemeNeighbors(std::vector<Heuristics::FMTOperatingAreaScheme> operatingareaparameters,
			const std::vector<Core::FMTTheme>& themes, const std::string& data_vectors,
			const std::string& agefield, const std::string& areafield, double agefactor = 1.0,
			double areafactor = 1, std::string lockfield = "",
			double minimal_area = 0.0, double buffersize = 100) const;
		// DocString: FMTAreaParser::getClusters
		/**
		Using a vector of operating area (operatingareaparameters), a complete vector of FMTTheme (themes), a vector file (data_vectors),
		the name of the age field name (agefield) an area field name (areafield), an (gefactor), an (areafactor), an optional (lockfield) name,
		a (minimal_area) : the minimal area parameters indicate that if a feature has an area lower than the minimal area it wont be selected.
		For (buffersize) see getNeighborsFromPolygons function. The returned operating area clusters with their linker mask.
		*/
		std::vector<Heuristics::FMTOperatingAreaCluster> getClusters(const std::vector<Heuristics::FMTOperatingArea>& operatingareas,
			const std::vector<Core::FMTTheme>& themes, const std::string& data_vectors,
			const std::string& agefield, const std::string& areafield, const double& maximaldistance,
			double agefactor = 1.0, double areafactor = 1, std::string lockfield = "",
			double minimal_area = 0.0, double buffersize = 100) const;
		std::vector<Heuristics::FMTOperatingAreaScheme> getOperatingArea(const std::string& fichierShp, const std::vector<Core::FMTTheme>& themes, const int& numeroTheme, const int& startingperiod, const std::string& nomChampAge, const std::string& nomChampSuperficie, const std::string& nomChampStanlock, const std::string& fichierParam) const;
		// DocString: FMTAreaParser::readOAschedulerparameters
		/**
		Using the location for parameters file (must contain headers "OA","OPT","RET","MAXRET","REP","OPR"), the modelthemes,
		the themetarget corresponding to the themeid where the OA in parameters is in, and the startingperiod it return a vector
		of operatingareascheme, to use in operatingareascheduler.
		*/
		std::vector<Heuristics::FMTOperatingAreaScheme> readOAschedulerparameters(const std::string& location, const std::vector<Core::FMTTheme>& modelthemes, const int& themetarget, const int& startingperiod) const;
		// DocString: FMTAreaParser::writeOAschedulerparameters
		/**
		Write down a parameters files on the form ("OA","OPT","RET","MAXRET","REP","OPR") but the column OA is gona contains the full mask not just the
		selected theme into a regular csv files.
		*/
		void writeOAschedulerparameters(const std::string& location, const std::vector<Heuristics::FMTOperatingAreaScheme>& OAschemes,
			std::vector<std::string> creationoptions = std::vector<std::string>()) const;
#endif
		// DocString: FMTAreaParser::readVectors
		/**
		This function returns a vector of actualdevelopment present in a vector file (data_vectors) using a complete (themes) vector,
		an age field name (agefield), and area field name (areafield), an age factor (agefactor), and areafactor (areafactor), an optional
		lock field name (lockfield) and an (minimal_area) which is this minimal size a feature needs to have to be selected.
		*/
		std::vector<Core::FMTActualDevelopment>readVectors(const std::vector<Core::FMTTheme>& themes, const std::string& data_vectors,
			const std::string& agefield, const std::string& areafield, double agefactor = 1.0,
			double areafactor = 1, std::string lockfield = "",
			double minimalarea = 0.0) const;
		// DocString: FMTAreaParser::vectormaptoFMTforest
		/**

		*/
		Spatial::FMTForest vectormaptoFMTforest(const std::string& data_vectors,
			const int& resolution, const std::vector<Core::FMTTheme>& themes,
			const std::string& agefield, const std::string& areafield, double agefactor = 1.0,
			double areafactor = 1, std::string lockfield = "",
			double minimalarea = 0.0, const std::string& writeforestfolder = "",
			const bool& fittoforel = true) const;
		// DocString: FMTAreaParser::vectormaptoFMTforest
		/**
		Create a raster file of a given resolution based on a the data_vectors field.
		*/
		void vectorFieldToRaster(const std::string& data_vectors,
			const std::string& tifpathandname,
			const int& resolution,
			const std::string& field,
			bool fittoforel = true) const;
		// DocString: FMTAreaParser::ogrLayerToRaster
		/**
		This function first rasterize the layer to a resolution of 20x20 in memory using gdal virtual file system.
		Then, the raster is reproject with the given (resolution) using (for now) only MODE as resampling.
		The field to rasterize (fieldname) must be of type int.
		If (fittoforel), the layer must be projected in ESPG32198 and will align with the Quebec FOREL rasters.
		--In the future, arguments will be added to change the resampling type and allow field with float type.
		*/
		GDALDataset* ogrLayerToRaster(OGRLayer* layer, const std::string& fieldname, const std::string& outfilename, const int& resolution, const bool& fittoforel) const;

		void writeForestExtended(Spatial::FMTForest& p_forest,const std::string& p_VectorFile,
						const std::vector<Core::FMTTheme> p_themes,
						const std::vector<Core::FMTActualDevelopment>& p_area,
							const std::string& p_folder) const;

#endif
		// DocString: FMTAreaParser()
		/**
		Default constructor for FMTAreaParser
		*/
		FMTAreaParser();
		// DocString: ~FMTAreaParser()
		/**
		Default destructor for FMTAreaParser
		*/
		~FMTAreaParser() = default;
		// DocString: FMTAreaParser(const FMTAreaParser&)
		/**
		Default copy constructor for FMTAreaParser
		*/
		FMTAreaParser(const FMTAreaParser& rhs) = default;
		// DocString: FMTAreaParser::operator=
		/**
		Default copy assignment for FMTAreaParser
		*/
		FMTAreaParser& operator = (const FMTAreaParser& rhs) = default;
		// DocString: FMTAreaParser::read
		/**
		The read function will read a regular area section (location) with a complete vector of (themes) and some (constants).
		It will return a vector of actualdevelopment present in the area file.
		*/
		std::vector<Core::FMTActualDevelopment>read(const std::vector<Core::FMTTheme>& themes, const Core::FMTConstants& constants, const std::string& location);
		// DocString: FMTAreaParser::write
		/**
		Giving a vector of actual development (areas) and a file (location) for the area section this function
		is going to write a new area section usging the areas developments.
		*/
		void write(const std::vector<Core::FMTActualDevelopment>& areas, const std::string& location) const;
    private:
		// DocString: FMTAreaParser::rxcleanarea
		///This regex is used to capture the information kept in the .are section.
        const static boost::regex rxcleanarea;
		// DocString: FMTAreaParser::m_RxExclude
		///To capture the exclude keyword
		const static boost::regex m_RxExclude;
		// DocString: FMTAreaParser::m_RxExcludeSpec
		///To capture the line of the exclude
		const static boost::regex m_RxExcludeSpec;
		// DocString: FMTAreaParser::_isExclude
		/**
		@brief test if we got an exclude
		@param[in] line read
		@return true if got exclude
		*/
		bool _isExclude(const std::string& pm_line) const;
		// DocString: FMTAreaParser::_getExcludeValue
		/**
		@brief get the excluded line Value
		@param[in] line read
		@return a non empty line if something to exclude
		*/
		std::string _getExcludeValue(const std::string& pm_line) const;
		// DocString: FMTAreaParser::_getExcludedSpec
		/**
		@brief get a mask n spec of the excluded line
		@param[in] the themes
		@param[in] the constants
		@param[in] the excluded line
		@return a mask and spec.
		*/
		std::pair<Core::FMTMask, Core::FMTSpec> _getExcludedSpec(const std::vector<Core::FMTTheme>& p_themes,
																const Core::FMTConstants& p_constants,
																const std::string& p_value) const;
		// DocString: FMTAreaParser::_gotNewExclude
		/**
		@brief get a mask n spec of the excluded line and add it to the FMTList
		@param[in] the themes
		@param[in] the constants
		@param[in] the excluded line
		@param[out] an FMTList of spec...
		@return true if added a new one or got an exclude seciton
		*/
		bool _gotNewExclude(const std::vector<Core::FMTTheme>& p_themes,
			const Core::FMTConstants& p_constants,
			const std::string& p_value,
			Core::FMTList<Core::FMTSpec>& p_list) const;
		// DocString: FMTAreaParser::getPeriodPathName
		/**
		Giving a folder (location) and a (period) and a (name)
		the function returns the path to a disturbance layer (.tiff) raster file.
		*/
		std::string getPeriodPathName(const std::string& location, const int& period,const std::string& name) const;
		// DocString: FMTAreaParser::getGCBMtransitions
		/**
		This function is only usefull whe using GCBM. Using the FMTSesModel elements disturbances (stacked_actions),
		last ages of the area generated by the FMTSesModel, the new generated FMTForest by FMTSesModel and the themes of the
		FMTModel.
		*/
		std::vector<Core::FMTGCBMTransition> getGCBMtransitions(const Spatial::FMTLayer<std::string>& stacked_actions,
													const Spatial::FMTLayer<int>& ages,
													const Spatial::FMTForest& newfor,
													const std::vector<Core::FMTTheme>& themes) const;
		#ifdef FMTWITHGDAL
			// DocString: FMTAreaParser::writeBand
			/**
			Write the FMTLayer in a band.
			*/
			template<typename T,typename outT>
			bool writeBand(const Spatial::FMTLayer<T>& layer, GDALRasterBand* wband, const std::map<T, std::string>& mapping) const;
			// DocString: FMTAreaParser::getUnion
				/**
				Simply call a union cascaded on all multipartpolygons to create single polygon for each multipart.
				You need to call the destroyPolygons function after to make sure no memory leaks appear.
				*/
			std::vector<OGRPolygon*> getUnion(const std::vector<OGRMultiPolygon>& multipartpolygons) const;
			// DocString: FMTAreaParser::destroyPolygons
			/**
			Will destroy all heap allocaed OGRpolygon in the vector.
			*/
			void destroyPolygons(std::vector<OGRPolygon*>& polygonstodestroy) const;
			// DocString: FMTAreaParser::getFeatureToDevelopment
			/**
			When the FMTAreaParser read features from a shapefile it needs to convert this feature into
			an actual development to be used into the area section. The feature require a (feature) a complete
			vector of (themes), the index of each age,lock and area field and finaly the factor to use with those
			fields.
			*/
			Core::FMTActualDevelopment getFeatureToDevelopment(const OGRFeature* feature,
															const std::vector<Core::FMTTheme>& themes,
															const std::map<int, int>& themes_fields,
															const int& age_field,
															const int& lock_field,
															const int& area_field,
															const double& agefactor,
															const double& areafactor,
															const double& minimalarea) const;
			// DocString: FMTAreaParser::validateRaster
			/**
			Simple function to validate that a vector of rasters are perfectly part of each other.
			So that each raster cell intersect the center point of a other raster raster cell.
			If the raster cannot be treated has a stack the function will throw errors.
			*/
			void validateRaster(const std::vector<std::string>&data_rasters) const;
			// DocString: FMTAreaParser::openVectorFile
			/**
			Function that open a vector file from a path (data_vectors) and returns a GDALdataset if all mandatory fields are
			present in the vector files else it's going to throw an error. the user should provide a age,area,lock field name and a complete
			vector of themes, the age,lock,area field index are going to be fill by the function.
			*/
			GDALDataset* openVectorFile(std::map<int, int>&themes_fields, int& age_field, int& lock_field, int& area_field,
				const std::string& data_vectors, const std::string& agefield, const std::string& areafield, const std::string& lockfield,
				const std::vector<Core::FMTTheme>& themes) const;
			#ifdef FMTWITHOSI
			// DocString: FMTAreaParser::getMultipolygons
			/**
			This function uses a  vector of operating area and a vector file (data_vectors), age and area field name, an age,area,lock factor to
			aggregates all polygons of a vector file into a vector of OGRMultipolygon. Each Multipolygon is a spatial representation of a FMTOperatingArea
			unit (the returned vector is ordered as the operatingareas vector).
			*/
			std::vector<OGRMultiPolygon>getMultipolygons(const std::vector<Heuristics::FMTOperatingArea>& operatingareas,
											  const std::vector<Core::FMTTheme>& themes, const std::string& data_vectors,
											  const std::string& agefield, const std::string& areafield, double agefactor = 1.0,
											  double areafactor = 1, std::string lockfield = "",
											  double minimal_area = 0.0) const;
			// DocString: FMTAreaParser::getNeighborsFromPolygons
			/**
			This function uses a vector of Multipolygons representing each FMTOperatingArea (multipolygon),
			a vector of operating area (operatingareas) and a (buffersize) to calculate the neighbors of each operating area.
			It returns a vector of FMTOperatingArea but with theirs neighbors data member filled.
			The buffersize is the width of the buffer used to determine the amount of perimeter shared between each operating area.
			*/
			std::vector<Heuristics::FMTOperatingArea> getNeighborsFromPolygons(const std::vector<OGRPolygon*>&polygons,
																			std::vector<Heuristics::FMTOperatingArea> operatingareas,
																	const double& buffersize) const;

			// DocString: FMTAreaParser::getClustersFromPolygons
			/**
			Base on a list of polygons and a list of corresponding operatingareas it returns a list of potential cluster of those
			FMToperating area based on a maximal clustering distance.
			*/
			std::vector<Heuristics::FMTOperatingAreaCluster> getClustersFromPolygons(const std::vector<OGRPolygon*>&polygons,
																		const std::vector<Heuristics::FMTOperatingArea>& operatingareas,const double& maximaldistance) const;
			bool _isMapWithSameThemes(const std::vector<Core::FMTTheme>& p_themes,
				const std::string& p_VectorsMap) const;
		#endif
			// DocString: FMTAreaParser::getFMTforestfromlayer
			/**
			This function first rasterize the layer to a resolution of 20x20 in memory using gdal virtual file system
			and reproject it to the desired resolution. Finally, it return and FMTForest based on the raster and 
			the developments correspondant to the value in the field containing the development id(devidfield).
			If (fittoforel), the layer must be projected in ESPG32198 and will align with the Quebec FOREL rasters.
			*/
			Spatial::FMTForest getFMTforestfromlayer(OGRLayer* layer,const std::vector<Core::FMTActualDevelopment>& actualdevs, const std::string& devidfield, const int& resolution, const double& areafactor,const bool& fittoforel) const;
			// DocString: FMTAreaParser::subsetLayer
			/**
			Sometime vector files can be realy large with empty value in the age/area fields or themes fields.
			This function subset the element that are not null from a (layer) using a complete (themes) vector,
			an (agefield) and an (areafield). It returns a OGRlayer with only the non null features.
			*/
			OGRLayer* subsetLayer(OGRLayer*layer, const std::vector<Core::FMTTheme>& themes,
								const std::string& agefield, const std::string& areafield) const;
	#endif
			
    };
}
#endif // FMTareaparser_Hm_included

