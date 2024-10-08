/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTareaparser_H_INCLUDED
#define FMTareaparser_H_INCLUDED

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
	class FMTdevelopment;
	class FMTactualdevelopment;
	class FMTGCBMtransition;
	class FMTmask;
	template<typename T>
	class FMTlist;
}
namespace Graph
{
	class FMTpredictor;
}

namespace Spatial
{
	class FMTforest;
	class FMTspatialschedule;
	class FMTsasolution;
}

namespace Heuristics
{
	class FMToperatingareascheme;
	class FMToperatingareacluster;
	class FMToperatingarea;
}

namespace Models
{
	class FMTmodel;
}


namespace Parser
{
// DocString: FMTareaparser
/**
FMTareaparser is a important parser in FMT because this parser deals with spatial stuff used in multiple FMTmodel children.
If FMT is compiled without the compile proprocessor FMTWITHGDAL than alot of funcionalities of the FMTareaparser wont be
available to the user. This class is also used by the FMTmodelparser.
*/
class FMTEXPORT FMTareaparser : public FMTparser
    {
	public:
#ifdef FMTWITHGDAL
		// DocString: FMTareaparser::readrasters
		/**
		Using a complete vector of (themes), a vector of raster files path (data_rasters) each raster represent a theme, an (age) raster file and some optional parameters
		(agefactor=1.0),(areafactor=0.0001) to multiply with the actualdevelopement age and area and a optional (lock) raster file.
		The function generates a FMTforest layer from those rasters files.
		*/
		Spatial::FMTforest readrasters(const std::vector<Core::FMTtheme>& themes, const std::vector<std::string>& data_rasters,
			const std::string& age, double agefactor = 1.0, double areafactor = 0.0001, std::string lock = "") const;
		// DocString: FMTareaparser::writelayer
		/**
		Using a layer of a given type T the function will write this (layer) into a raster file (location). the mapping add
		a table to the raster file when dealing with categorical variables
		*/
		template<typename T>
		bool writelayer(const Spatial::FMTlayer<T>& layer, std::string location, const std::map<T, std::string>& mapping, std::string format = "GTiff") const;
		// DocString: FMTareaparser::writelayer
		/**
		Using a layer of a given type double the function will write this (layer) into a raster file (location). the mapping add
		a table to the raster file when dealing with categorical variables
		*/
		bool writelayer(const Spatial::FMTlayer<double>& layer, std::string location, std::string format = "GTiff") const;
		// DocString: FMTareaparser::writeforest
		/**
		The function will write a complete FMTforest (for_layer) using a complete vector of (themes), in multiple (data_rasters) file paths
		number of paths should be equal to number of themes an (age) file path and (lock) file path.
		The generated .tiff files can have categorical values but it needs to be specified in the
		mapping vector each element of the vector represent a corresponging key to write in the categorical dataset of the raster.
		*/
		bool writeforest(const Spatial::FMTforest& for_layer,
			const std::vector<Core::FMTtheme>& themes,
			const std::vector<std::string>& data_rasters,
			const std::string& age,
			const std::string& lock,
			std::vector<std::map<std::string, std::string>> mapping = std::vector<std::map<std::string, std::string>>()) const;
		// DocString: FMTareaparser::writeforesttheme
		/**
		Write a forest theme based on a mapping for a FMTforest with a given file format in raster file.
		*/
		bool writeforesttheme(
			const Spatial::FMTforest& for_layer,
			const Core::FMTtheme& theme,
			const std::string& location,
			const std::map<std::string, std::string>& mapping,
			std::string format = "GTiff") const;
		// DocString: FMTareaparser::writedisturbances
		/**
		Giving a .tif file (location) and a disturbancesstack (disturbances) the actual forest (for_layer) and the last forest layer (out_layer).
		a complete vector of model (themes) and a optional (mapping) for the disturbance stack layer created.
		The function will write all the disturbances in the locaiton .tif file and it will also returns the corresponding GCBMtransition for
		this planning period.
		*/
		std::vector<Core::FMTGCBMtransition> writedisturbances(const std::string& location,
			const Spatial::FMTspatialschedule& disturbances,
			const std::vector<Core::FMTaction>& actions,
			const std::vector<Core::FMTtheme>& themes,
			const int& period) const;
		// DocString: FMTareaparser::writepredictors()
		/**
		Giving a .tif file (location) and a spatialschedule (spatialsolution).
		a complete vector of model (yieldnames) and  a yield section (yields)
		The function will write all the predictor id in the location .tif file and it will also returns the corresponding predictors for
		this planning period.
		*/
		std::vector<std::vector<Graph::FMTpredictor>> writepredictors(const std::string& location,
			const Spatial::FMTspatialschedule& spatialsolution,
			const std::vector<std::string>& yieldnames,
			const Models::FMTmodel& model,
			const int& period,
			bool periodonevalues = false,
			bool withGCBMid = true) const;
		// DocString: FMTareaparser::writesasolution
	   /**

	   */
	   /*bool writesasolution(const std::string location,
						   const Spatial::FMTsasolution& solution,
						   const std::vector<Core::FMTtheme>& themes,
						   const std::vector<Core::FMTaction>& actions,
						   const bool& writeevents = true,
						   int periodstart=-1,
						   int periodstop=-1) const;*/
	#ifdef FMTWITHOSI
						   // DocString: FMTareaparser::getschemeneighbors
						   /**
						   Using a vector of operating area (operatingareaparameters), a complete vector of FMTtheme (themes), a vector file (data_vectors),
						   the name of the age field name (agefield) an area field name (areafield), an (gefactor), an (areafactor), an optional (lockfield) name,
						   a (minimal_area) : the minimal area parameters indicate that if a feature has an area lower than the minimal area it wont be selected.
						   For (buffersize) see getneighborsfrompolygons function. The returned operating area will have theirs neighboors vector filled.
						   */
		std::vector<Heuristics::FMToperatingareascheme> getschemeneighbors(std::vector<Heuristics::FMToperatingareascheme> operatingareaparameters,
			const std::vector<Core::FMTtheme>& themes, const std::string& data_vectors,
			const std::string& agefield, const std::string& areafield, double agefactor = 1.0,
			double areafactor = 1, std::string lockfield = "",
			double minimal_area = 0.0, double buffersize = 100) const;
		// DocString: FMTareaparser::getclusters
		/**
		Using a vector of operating area (operatingareaparameters), a complete vector of FMTtheme (themes), a vector file (data_vectors),
		the name of the age field name (agefield) an area field name (areafield), an (gefactor), an (areafactor), an optional (lockfield) name,
		a (minimal_area) : the minimal area parameters indicate that if a feature has an area lower than the minimal area it wont be selected.
		For (buffersize) see getneighborsfrompolygons function. The returned operating area clusters with their linker mask.
		*/
		std::vector<Heuristics::FMToperatingareacluster> getclusters(const std::vector<Heuristics::FMToperatingarea>& operatingareas,
			const std::vector<Core::FMTtheme>& themes, const std::string& data_vectors,
			const std::string& agefield, const std::string& areafield, const double& maximaldistance,
			double agefactor = 1.0, double areafactor = 1, std::string lockfield = "",
			double minimal_area = 0.0, double buffersize = 100) const;
		std::vector<Heuristics::FMToperatingareascheme> getOperatingArea(const std::string& fichierShp, const std::vector<Core::FMTtheme>& themes, const int& numeroTheme, const int& startingperiod, const std::string& nomChampAge, const std::string& nomChampSuperficie, const std::string& nomChampStanlock, const std::string& fichierParam) const;
		// DocString: FMTareaparser::readOAschedulerparameters
		/**
		Using the location for parameters file (must contain headers "OA","OPT","RET","MAXRET","REP","OPR"), the modelthemes,
		the themetarget corresponding to the themeid where the OA in parameters is in, and the startingperiod it return a vector
		of operatingareascheme, to use in operatingareascheduler.
		*/
		std::vector<Heuristics::FMToperatingareascheme> readOAschedulerparameters(const std::string& location, const std::vector<Core::FMTtheme>& modelthemes, const int& themetarget, const int& startingperiod) const;
		// DocString: FMTareaparser::writeOAschedulerparameters
		/**
		Write down a parameters files on the form ("OA","OPT","RET","MAXRET","REP","OPR") but the column OA is gona contains the full mask not just the
		selected theme into a regular csv files.
		*/
		void writeOAschedulerparameters(const std::string& location, const std::vector<Heuristics::FMToperatingareascheme>& OAschemes,
			std::vector<std::string> creationoptions = std::vector<std::string>()) const;
#endif
		// DocString: FMTareaparser::readvectors
		/**
		This function returns a vector of actualdevelopement present in a vector file (data_vectors) using a complete (themes) vector,
		an age field name (agefield), and area field name (areafield), an age factor (agefactor), and areafactor (areafactor), an optional
		lock field name (lockfield) and an (minimal_area) which is this minimal size a feature needs to have to be selected.
		*/
		std::vector<Core::FMTactualdevelopment>readvectors(const std::vector<Core::FMTtheme>& themes, const std::string& data_vectors,
			const std::string& agefield, const std::string& areafield, double agefactor = 1.0,
			double areafactor = 1, std::string lockfield = "",
			double minimalarea = 0.0) const;
		// DocString: FMTareaparser::vectormaptoFMTforest
		/**

		*/
		Spatial::FMTforest vectormaptoFMTforest(const std::string& data_vectors,
			const int& resolution, const std::vector<Core::FMTtheme>& themes,
			const std::string& agefield, const std::string& areafield, double agefactor = 1.0,
			double areafactor = 1, std::string lockfield = "",
			double minimalarea = 0.0, const std::string& writeforestfolder = "",
			const bool& fittoforel = true) const;
		// DocString: FMTareaparser::vectormaptoFMTforest
		/**
		Create a raster file of a given resolution based on a the data_vectors field.
		*/
		void vectorfieldtoraster(const std::string& data_vectors,
			const std::string& tifpathandname,
			const int& resolution,
			const std::string& field,
			bool fittoforel = true) const;
		// DocString: FMTareaparser::OGRlayertoRaster
		/**
		This function first rasterize the layer to a resolution of 20x20 in memory using gdal virtual file system.
		Then, the raster is reproject with the given (resolution) using (for now) only MODE as resampling.
		The field to rasterize (fieldname) must be of type int.
		If (fittoforel), the layer must be projected in ESPG32198 and will align with the Quebec FOREL rasters.
		--In the future, arguments will be added to change the resampling type and allow field with float type.
		*/
		GDALDataset* OGRlayertoRaster(OGRLayer* layer, const std::string& fieldname, const std::string& outfilename, const int& resolution, const bool& fittoforel) const;
#endif
		// DocString: FMTareaparser()
		/**
		Default constructor for FMTareaparser
		*/
		FMTareaparser();
		// DocString: ~FMTareaparser()
		/**
		Default destructor for FMTareaparser
		*/
		~FMTareaparser() = default;
		// DocString: FMTareaparser(const FMTareaparser&)
		/**
		Default copy constructor for FMTareaparser
		*/
		FMTareaparser(const FMTareaparser& rhs) = default;
		// DocString: FMTareaparser::operator=
		/**
		Default copy assignment for FMTareaparser
		*/
		FMTareaparser& operator = (const FMTareaparser& rhs) = default;
		// DocString: FMTareaparser::read
		/**
		The read function will read a regular area section (location) with a complete vector of (themes) and some (constants).
		It will return a vector of actualdevelopment present in the area file.
		*/
		std::vector<Core::FMTactualdevelopment>read(const std::vector<Core::FMTtheme>& themes, const Core::FMTconstants& constants, const std::string& location);
		// DocString: FMTareaparser::write
		/**
		Giving a vector of actual development (areas) and a file (location) for the area section this function
		is going to write a new area section usging the areas developments.
		*/
		void write(const std::vector<Core::FMTactualdevelopment>& areas, const std::string& location) const;
    private:
		// DocString: FMTareaparser::rxcleanarea
		///This regex is used to capture the information kept in the .are section.
        const static boost::regex rxcleanarea;
		// DocString: FMTareaparser::m_RxExclude
		///To capture the exclude keyword
		const static boost::regex m_RxExclude;
		// DocString: FMTareaparser::m_RxExcludeSpec
		///To capture the line of the exclude
		const static boost::regex m_RxExcludeSpec;
		// DocString: FMTareaparser::_isExclude
		/**
		@brief test if we got an exclude
		@param[in] line read
		@return true if got exclude
		*/
		bool _isExclude(const std::string& p_line) const;
		// DocString: FMTareaparser::_getExcludeValue
		/**
		@brief get the excluded line Value
		@param[in] line read
		@return a non empty line if something to exclude
		*/
		std::string _getExcludeValue(const std::string& p_line) const;
		// DocString: FMTareaparser::_getExcludedSpec
		/**
		@brief get a mask n spec of the excluded line
		@param[in] the themes
		@param[in] the constants
		@param[in] the excluded line
		@return a mask and spec.
		*/
		std::pair<Core::FMTmask, Core::FMTspec> _getExcludedSpec(const std::vector<Core::FMTtheme>& p_themes,
																const Core::FMTconstants& p_constants,
																const std::string& p_value) const;
		// DocString: FMTareaparser::_gotNewExclude
		/**
		@brief get a mask n spec of the excluded line and add it to the FMTlist
		@param[in] the themes
		@param[in] the constants
		@param[in] the excluded line
		@param[out] an FMTlist of spec...
		@return true if added a new one or got an exclude seciton
		*/
		bool _gotNewExclude(const std::vector<Core::FMTtheme>& p_themes,
			const Core::FMTconstants& p_constants,
			const std::string& p_value,
			Core::FMTlist<Core::FMTspec>& p_list) const;
		// DocString: FMTareaparser::getperiodpathname
		/**
		Giving a folder (location) and a (period) and a (name)
		the function returns the path to a disturbance layer (.tiff) raster file.
		*/
		std::string getperiodpathname(const std::string& location, const int& period,const std::string& name) const;
		// DocString: FMTareaparser::getGCBMtransitions
		/**
		This function is only usefull whe using GCBM. Using the FMTsesmodel elements disturbances (stacked_actions),
		last ages of the area generated by the FMTsesmodel, the new generated FMTforest by FMTsesmodel and the themes of the
		FMTmodel.
		*/
		std::vector<Core::FMTGCBMtransition> getGCBMtransitions(const Spatial::FMTlayer<std::string>& stacked_actions,
													const Spatial::FMTlayer<int>& ages,
													const Spatial::FMTforest& newfor,
													const std::vector<Core::FMTtheme>& themes) const;
		#ifdef FMTWITHGDAL
			// DocString: FMTareaparser::writeband
			/**
			Write the FMTlayer in a band.
			*/
			template<typename T,typename outT>
			void writeband(const Spatial::FMTlayer<T>& layer, GDALRasterBand* wband, const std::map<T, std::string>& mapping) const;
			// DocString: FMTareaparser::getunion
				/**
				Simply call a union cascaded on all multipartpolygons to create single polygon for each multipart.
				You need to call the destroypolygons function after to make sure no memory leaks appear.
				*/
			std::vector<OGRPolygon*> getunion(const std::vector<OGRMultiPolygon>& multipartpolygons) const;
			// DocString: FMTareaparser::destroypolygons
			/**
			Will destroy all heap allocaed OGRpolygon in the vector.
			*/
			void destroypolygons(std::vector<OGRPolygon*>& polygonstodestroy) const;
			// DocString: FMTareaparser::getfeaturetodevelopment
			/**
			When the FMTareaparser read features from a shapefile it needs to convert this feature into
			an actual development to be used into the area section. The feature require a (feature) a complete
			vector of (themes), the index of each age,lock and area field and finaly the factor to use with those
			fields.
			*/
			Core::FMTactualdevelopment getfeaturetodevelopment(const OGRFeature* feature,
															const std::vector<Core::FMTtheme>& themes,
															const std::map<int, int>& themes_fields,
															const int& age_field,
															const int& lock_field,
															const int& area_field,
															const double& agefactor,
															const double& areafactor,
															const double& minimalarea) const;
			// DocString: FMTareaparser::validate_raster
			/**
			Simple function to validate that a vector of rasters are perfectly part of each other.
			So that each raster cell intersect the center point of a other raster raster cell.
			If the raster cannot be treated has a stack the function will throw errors.
			*/
			void validate_raster(const std::vector<std::string>&data_rasters) const;
			// DocString: FMTareaparser::openvectorfile
			/**
			Function that open a vector file from a path (data_vectors) and returns a GDALdataset if all mandatory fields are
			present in the vector files else it's going to throw an error. the user should provide a age,area,lock field name and a complete
			vector of themes, the age,lock,area field index are going to be fill by the function.
			*/
			GDALDataset* openvectorfile(std::map<int, int>&themes_fields, int& age_field, int& lock_field, int& area_field,
				const std::string& data_vectors, const std::string& agefield, const std::string& areafield, const std::string& lockfield,
				const std::vector<Core::FMTtheme>& themes) const;
			#ifdef FMTWITHOSI
			// DocString: FMTareaparser::getmultipolygons
			/**
			This function uses a  vector of operating area and a vector file (data_vectors), age and area field name, an age,area,lock factor to
			aggregates all polygons of a vector file into a vector of OGRMultipolygon. Each Multipolygon is a spatial representation of a FMToperatingarea
			unit (the returned vector is ordered as the operatingareas vector).
			*/
			std::vector<OGRMultiPolygon>getmultipolygons(const std::vector<Heuristics::FMToperatingarea>& operatingareas,
											  const std::vector<Core::FMTtheme>& themes, const std::string& data_vectors,
											  const std::string& agefield, const std::string& areafield, double agefactor = 1.0,
											  double areafactor = 1, std::string lockfield = "",
											  double minimal_area = 0.0) const;
			// DocString: FMTareaparser::getneighborsfrompolygons
			/**
			This function uses a vector of Multipolygons representing each FMToperatingarea (multipolygon),
			a vector of operating area (operatingareas) and a (buffersize) to calculate the neighbors of each operating area.
			It returns a vector of FMToperatingarea but with theirs neighbors data member filled.
			The buffersize is the width of the buffer used to determine the amount of perimeter shared between each operating area.
			*/
			std::vector<Heuristics::FMToperatingarea> getneighborsfrompolygons(const std::vector<OGRPolygon*>&polygons,
																			std::vector<Heuristics::FMToperatingarea> operatingareas,
																	const double& buffersize) const;

			// DocString: FMTareaparser::getclustersfrompolygons
			/**
			Base on a list of polygons and a list of corresponding operatingareas it returns a list of potential cluster of those
			FMToperating area based on a maximal clustering distance.
			*/
			std::vector<Heuristics::FMToperatingareacluster> getclustersfrompolygons(const std::vector<OGRPolygon*>&polygons,
																		const std::vector<Heuristics::FMToperatingarea>& operatingareas,
																		const double& maximaldistance) const;
		#endif
			// DocString: FMTareaparser::getFMTforestfromlayer
			/**
			This function first rasterize the layer to a resolution of 20x20 in memory using gdal virtual file system
			and reproject it to the desired resolution. Finally, it return and FMTforest based on the raster and 
			the developments correspondant to the value in the field containing the development id(devidfield).
			If (fittoforel), the layer must be projected in ESPG32198 and will align with the Quebec FOREL rasters.
			*/
			Spatial::FMTforest getFMTforestfromlayer(OGRLayer* layer,const std::vector<Core::FMTactualdevelopment>& actualdevs, const std::string& devidfield, const int& resolution, const double& areafactor,const bool& fittoforel) const;
			// DocString: FMTareaparser::subsetlayer
			/**
			Sometime vector files can be realy large with empty value in the age/area fields or themes fields.
			This function subset the element that are not null from a (layer) using a complete (themes) vector,
			an (agefield) and an (areafield). It returns a OGRlayer with only the non null features.
			*/
			OGRLayer* subsetlayer(OGRLayer*layer, const std::vector<Core::FMTtheme>& themes,
								const std::string& agefield, const std::string& areafield) const;
	#endif
			
    };
}
#endif // FMTareaparser_H_INCLUDED

