/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTAreaParser.h"
#include "FMTExceptionHandler.h"
#include <boost/algorithm/string/join.hpp>
#include <boost/lexical_cast.hpp>
#include "FMTOperatingAreaClusterBinary.h"
#include "FMTLayer.hpp"
#include "FMTForest.h"
#include "FMTGCBMTransition.h"
#include "FMTConstants.h"
#include "FMTTheme.h"
#include "FMTOperatingAreaScheme.h"
#include "FMTOperatingAreaCluster.h"
#include "FMTDevelopment.h"
#include "FMTSpatialSchedule.h"
//#include "FMTSaSolution.h"
#include "FMTActualDevelopment.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include "FMTExceptionHandler.h"
#include <random>
#include <unordered_map>
#include "FMTDefaultLogger.h"

#ifdef FMTWITHGDAL
	#include "gdal_alg.h"
	#include "gdal_utils.h"
	#include "gdalwarper.h"
	#include "ogrsf_frmts.h"
#endif

namespace Parser{

const boost::regex FMTAreaParser::rxcleanarea = boost::regex("^((\\*A[A]*)([^|]*)(_lock)([^0-9]*)([0-9]*))|((\\*A[A]*)([^|]*)([|])([^|]*)([|])([^0-9]*)(.+))|((\\*A[A]*)(([^|]*)([|])([^|]*)([|])))|(\\*A[A]*)(.+)", boost::regex_constants::ECMAScript | boost::regex_constants::icase);

const boost::regex FMTAreaParser::m_RxExclude = boost::regex("^(\\*EXCLUDE)((.+)|())", boost::regex_constants::ECMAScript | boost::regex_constants::icase);

const boost::regex FMTAreaParser::m_RxExcludeSpec = boost::regex("^(.+)([\\s\\t]*)(\\d.+)", boost::regex_constants::ECMAScript | boost::regex_constants::icase);



#ifdef FMTWITHGDAL


double FMTAreaParser::_GetGeometryArea(const OGRGeometry* p_geometry)
	{
	double area = 0.0;
	if (p_geometry != nullptr)
		{
			const OGRwkbGeometryType TYPE = p_geometry->getGeometryType();
			switch (TYPE)
				{
				case wkbPolygon:
					{
					area = dynamic_cast<const OGRPolygon*>(p_geometry)->get_Area();
					break;
					}
				case wkbCurvePolygon:
					{
					area = dynamic_cast<const OGRCurvePolygon*>(p_geometry)->get_Area();
					break;
					}
				case wkbMultiPolygon:
				case wkbGeometryCollection:
				case wkbMultiSurface:
				{
					const OGRGeometryCollection* poGC =
						dynamic_cast<const OGRGeometryCollection*>(p_geometry);
					if (poGC != nullptr)
					{
						for (int i = 0; i < poGC->getNumGeometries(); i++)
						{
							area += FMTAreaParser::_GetGeometryArea(poGC->getGeometryRef(i));
						}
					}
				}
				default:
					area = 0.0;
				}
		}
	return area;
	}



bool FMTAreaParser::_isMapWithSameThemes(const std::vector<Core::FMTTheme>& p_themes,
	const std::string& p_VectorsMap) const
{
	try {
		GDALDataset* VDataset = getVectorDataset(p_VectorsMap);
		OGRLayer* layer = getLayer(VDataset, 0);
		std::map<int, int>themeFields;
		int age = 0;
		int area = 0;
		int lock = 0;
		getWSFields(layer, themeFields,age,area,lock);
		return (themeFields.size() == p_themes.size());
		GDALClose(VDataset);
	}catch (...)
		{
		_exhandler->raiseFromCatch(
			"", "FMTAreaParser::_isMapWithSameThemes", __LINE__, __FILE__, m_section);
		}
	return false;
}

	std::vector<OGRGeometry*> FMTAreaParser::_GetUnion(const std::vector<OGRMultiPolygon>& p_collections) const
		{
		std::vector<OGRGeometry*> mergedCollections(p_collections.size(),nullptr);
		try {
			size_t i = 0;
			for (const OGRMultiPolygon& COLLECTION : p_collections)
			{
				if (!COLLECTION.IsEmpty())
					{
					OGRGeometry* NEW_GEOMETRY = COLLECTION.UnaryUnion();
					mergedCollections.at(i) = NEW_GEOMETRY;
					}
				++i;
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch(
				"", "FMTAreaParser::_GetUnion", __LINE__, __FILE__, m_section);
			}
		return mergedCollections;
		}

	void FMTAreaParser::_DestroyGeometries(std::vector<OGRGeometry*>& p_geometires) const
	{
		try {
			for (OGRGeometry* geometry : p_geometires)
				{
				OGRGeometryFactory::destroyGeometry(geometry);
				}
			p_geometires.clear();
		}catch (...)
			{
			_exhandler->raiseFromCatch(
				"", "FMTAreaParser::_DestroyGeometries", __LINE__, __FILE__, m_section);
			}
	}

	

    void FMTAreaParser::validateRaster(const std::vector<std::string>&data_rasters) const
        {
		try {
			int xsize = -1;
			int ysize = -1;
			int rastercount = -1;
			//int overview = -1;
			std::string projection = "";
			for (const std::string& location : data_rasters)
			{
				GDALDataset* data = getDataset(location);
				GDALRasterBand* band = getBand(data);
				if (xsize > 0)
				{
					if ((data->GetRasterXSize() != xsize) || (data->GetRasterYSize() != ysize) || (data->GetRasterCount() != rastercount) || (data->GetProjectionRef() != projection) /*|| (band->getOverviewCount() != overview)*/)
					{
						_exhandler->raise(Exception::FMTexc::FMTinvalidband,
							"Rasters are not the same " + std::string(data->GetDescription()),
							"FMTAreaParser::validateRaster", __LINE__, __FILE__, m_section);
					}
				}
				else {
					xsize = data->GetRasterXSize();
					ysize = data->GetRasterYSize();
					rastercount = data->GetRasterCount();
					projection = data->GetProjectionRef();
					//overview = band->getOverviewCount();
				}
				GDALClose(data);
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("","in FMTareaparser::validate_raster", __LINE__, __FILE__,m_section);
			}
        }
	std::vector<Core::FMTGCBMTransition> FMTAreaParser::getGCBMtransitions(const Spatial::FMTLayer<std::string>& stacked_actions,
																const Spatial::FMTLayer<int>& ages,
																const Spatial::FMTForest& newfor,
																const std::vector<Core::FMTTheme>& themes) const
		{
		std::vector<Core::FMTGCBMTransition>GCBM;
		try{
			std::map<std::string, std::vector<double>>stats;
			std::map<std::string, std::vector<std::map<std::string,int>>>attributes;
			const std::vector<Spatial::FMTLayer<std::string>>newforests = newfor.getThemes(themes);
			for (Spatial::FMTLayer<std::string>::const_iterator itcoord = stacked_actions.begin();
				itcoord != stacked_actions.end(); itcoord++)
			{
				if (ages.find(itcoord->first) != ages.end())
				{
					if (stats.find(itcoord->second) == stats.end())
					{
						stats[itcoord->second] = std::vector<double>(2, 0);
					}
					stats[itcoord->second][0] += ages.at(itcoord->first);
					++stats[itcoord->second][1];

					if (attributes.find(itcoord->second) == attributes.end())
						{
						attributes[itcoord->second] = std::vector<std::map<std::string, int>>(newforests.size());
						}
					int tid = 0;
					for (const Spatial::FMTLayer<std::string>& nfor : newforests)
						{
						const std::string value = nfor.at(itcoord->first);
						if (attributes.at(itcoord->second).at(tid).find(value) == attributes.at(itcoord->second).at(tid).end())
							{
							attributes[itcoord->second][tid][value] = 0;
							}
						attributes[itcoord->second][tid][value] += 1;
						++tid;
						}
				}
			}


			for (std::map<std::string,std::vector<double>>::const_iterator it = stats.begin(); it != stats.end(); it++)
				{
				const int ageafter = int(round(it->second[0] / it->second[1]));
				const std::string action_name = it->first;
				std::map<std::string, std::string>theme_collection;
				int id = 0;
				for (const Core::FMTTheme& theme : themes)
				{
					const std::string theme_name = "THEME" + std::to_string(theme.getId()+1);
					int maxhit = 0;
					std::string returntheme = "";
					for (std::map<std::string,int>::const_iterator cit = attributes.at(it->first).at(id).begin(); cit!= attributes.at(it->first).at(id).end();++cit)
						{
						if (cit->second > maxhit)
							{
							maxhit = cit->second;
							returntheme = cit->first;
							}

						}
					theme_collection[theme_name] = returntheme;
					++id;
				}
				GCBM.push_back(Core::FMTGCBMTransition(ageafter, theme_collection, action_name));
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("","FMTAreaParser::getGCBMtransitions", __LINE__, __FILE__, m_section);
			}
		return GCBM;
		}

	bool FMTAreaParser::writeForest(const Spatial::FMTForest& p_for_layer,
		const std::vector<Core::FMTTheme>& p_themes,
		const std::string& p_folder,
		std::vector<std::map<std::string, std::string>> p_mapping ) const
	{
		try {
			if (!p_folder.empty())
			{
				*_logger << "Writing FMTforest to : " << p_folder << "\n";
				boost::filesystem::path basepath(p_folder);
				boost::filesystem::path agepath = basepath / "AGE.tif";
				boost::filesystem::path lockpath = basepath / "STANLOCK.tif";
				std::vector<std::string> themespaths;
				themespaths.reserve(p_themes.size());
				for (size_t i = 1; i <= p_themes.size(); i++)
				{
					boost::filesystem::path fpath("THEME" + std::to_string(i) + ".tif");
					boost::filesystem::path filepath = basepath / fpath;
					themespaths.push_back(filepath.string());
				}
				writeForest(p_for_layer, p_themes, themespaths, agepath.string(), lockpath.string(), p_mapping);
			}
			return false;
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTAreaParser::writeForest",
				__LINE__, __FILE__, m_section);
		}
		return false;
	}



    bool FMTAreaParser::writeForest(
		const Spatial::FMTForest& for_layer,
		const std::vector<Core::FMTTheme>& themes,
		const std::vector<std::string>&data_rasters,
		const std::string& age,
		const std::string& lock,
		std::vector<std::map<std::string, std::string>>mapping) const
        {
        const std::vector<Spatial::FMTLayer<std::string>> themes_layer = for_layer.getThemes(themes);
		try {
			if (!themes_layer.empty())
			{
				if (mapping.size() != themes.size())
				{
					mapping.clear();
					for (const Core::FMTTheme& theme : themes)
					{
						std::map<std::string, std::string> layer_map;
						for (const std::string& attribute : theme.getBaseAttributes())
						{
							layer_map[attribute] = attribute;
						}
						mapping.push_back(layer_map);
					}
				}
				int layer_id = 0;
				for (const std::map<std::string, std::string>& layermap : mapping)
				{
					writeLayer<std::string>(themes_layer[layer_id], data_rasters[layer_id], layermap);
					++layer_id;
				}
				const std::map<int, std::string>emptymapping;
				const Spatial::FMTLayer<int>agelayer = for_layer.getAge();
				writeLayer<int>(agelayer, age, emptymapping);
				const Spatial::FMTLayer<std::string>locklayer = for_layer.getLock();
				std::map<std::string, std::string>lockmap;
				const std::vector<std::string> attributes = locklayer.getAttributes();
				for (const std::string& att : attributes)
				{
					lockmap[att] = att;
				}
				if (!locklayer.empty())
				{
					writeLayer<std::string>(locklayer, lock, lockmap);
				}
				return true;
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("","FMTAreaParser::writeForest", __LINE__, __FILE__, m_section);
			}
		return false;
        }


	std::string FMTAreaParser::getPeriodPathName(const std::string& location, const int& period, const std::string& name) const
	{
		boost::filesystem::path full_path;
		try {
			const boost::filesystem::path dir(location);
			const std::string layername = name+"_" + std::to_string(period) + ".tif";
			const boost::filesystem::path file(layername);
			full_path = dir / file;
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("at " + location, "FMTAreaParser::getPeriodPathName", __LINE__, __FILE__, m_section);
		}
		return full_path.string();
	}

	

	std::vector<Core::FMTGCBMTransition> FMTAreaParser::writeDisturbances(const std::string& location,
		const Spatial::FMTSpatialSchedule& disturbances,
		const std::vector<Core::FMTAction>& actions,
		const std::vector<Core::FMTTheme>& themes,
		const int& period) const
	{
		//std::vector<Core::FMTGCBMTransition>GCBM = disturbances.getGCBMtransitions();
		std::vector<Core::FMTGCBMTransition>transitions;
		try
		{
			if (themes.empty())
			{
				Spatial::FMTLayer<std::string> lastDistLayer = disturbances.lastDistLayer(actions, period);
				std::map<std::string, std::string>lmapping;
				for (const auto& act : actions)
				{
					lmapping[act.getName()] = act.getName();
				}
				writeLayer<std::string>(lastDistLayer, getPeriodPathName(location, period,"DIST"), lmapping);
			}
			else
			{
				Spatial::FMTLayer<std::string> lastDistLayer(disturbances.copyExtent<std::string>());
				transitions = disturbances.getGCBMtransitions(lastDistLayer, actions, themes, period);
				if (!lastDistLayer.getMapping().empty())
				{
					std::map<std::string, std::string>lmapping;
					for (const auto& item : lastDistLayer.getMapping())
					{
						lmapping[item.second] = item.second;
					}
					writeLayer<std::string>(lastDistLayer, getPeriodPathName(location, period,"DIST"), lmapping);
				}
			}
		}
		catch (...)
		{
			_exhandler->printExceptions("at " + location, "FMTAreaParser::writeDisturbances", __LINE__, __FILE__);
		}
		return transitions;
	}

	std::vector<std::vector<Graph::FMTPredictor>> FMTAreaParser::writePredictors(
		const std::string& location,
		const Spatial::FMTSpatialSchedule& spatialsolution,
		const std::vector<std::string>& yieldnames,
		const Models::FMTModel& model,
		const int& period,
		bool periodonevalues,
		bool withGCBMid) const
	{
		std::vector<std::vector<Graph::FMTPredictor>>predictors;
		try {
			Spatial::FMTLayer<int> predictorids(spatialsolution.copyExtent<int>());
			//transitions = disturbances.getGCBMtransitions(lastDistLayer, actions, themes, period);
			if (!spatialsolution.empty())
			{
				predictors = spatialsolution.getPredictors(predictorids,model, yieldnames, period,periodonevalues, withGCBMid);
				std::map<int, std::string>mapping;
				writeLayer<int>(predictorids, getPeriodPathName(location, period,"PREDID"), mapping);
			}
		}catch (...)
			{
			_exhandler->printExceptions("at " + location, "FMTAreaParser::writePredictors", __LINE__, __FILE__);
			}
		return predictors;
	}


    Spatial::FMTForest FMTAreaParser::readRasters(const std::vector<Core::FMTTheme>& themes,
                                             const std::vector<std::string>&data_rasters,
                                             const std::string& age,double agefactor,
                                             double areafactor,std::string lock) const
        {
		try {
			//GDALAllRegister();
			std::vector<std::string>allrasters = data_rasters;
			allrasters.push_back(age);
			if (!lock.empty())
			{
				allrasters.push_back(lock);
			}
			validateRaster(allrasters);
			GDALDataset* agedataset = getDataset(age);
			GDALRasterBand* ageband = getBand(agedataset);
			int nXBlockSize, nYBlockSize;
			ageband->GetBlockSize(&nXBlockSize, &nYBlockSize);
			int nXBlocks = (ageband->GetXSize() + nXBlockSize - 1) / nXBlockSize;
			int nYBlocks = (ageband->GetYSize() + nYBlockSize - 1) / nYBlockSize;
			int nodata = int(ageband->GetNoDataValue());
			std::vector<GInt32>agedata(static_cast<size_t>(nXBlockSize) * static_cast<size_t>(nYBlockSize));
			std::vector<GInt32>attributedata(static_cast<size_t>(nXBlockSize) * static_cast<size_t>(nYBlockSize));
			GDALDataset* lockdataset = NULL;
			GDALRasterBand* lockband = NULL;
			std::vector<GInt32>lockdata;
			std::vector<double>pad(6);
			agedataset->GetGeoTransform(&pad[0]);
			double cellsize = (abs(pad[1] * pad[5]) * areafactor);
			std::vector<int>lockatts;
			if (!lock.empty())
			{
				lockdataset = getDataset(lock);
				const std::vector<std::string>lockstr = getCat(lockdataset);
				lockatts.reserve(lockstr.size());
				for (const std::string& strlock : lockstr)
				{
					std::vector<std::string>spstr;
					boost::split(spstr, strlock, boost::is_any_of(FMT_STR_SEPARATOR), boost::token_compress_on);
					lockatts.push_back(getNum<int>(spstr[1]));
				}
				lockband = getBand(lockdataset);
				lockdata = std::vector<GInt32>(static_cast<size_t>(nXBlockSize) * static_cast<size_t>(nYBlockSize), 0);
			}
			std::vector<GDALDataset*>datasets;
			std::vector<GDALRasterBand*>bands;
			std::vector<std::vector<std::string>>attributes;
			for (const std::string& location : data_rasters)
			{
				GDALDataset* dataset = getDataset(location);
				GDALRasterBand* band = getBand(dataset);
				datasets.push_back(dataset);
				bands.push_back(band);
				attributes.push_back(getCat(dataset));
			}
			//std::map<Spatial::FMTCoordinate, Core::FMTDevelopment>mapping;
			const std::string projection = agedataset->GetProjectionRef();
			const unsigned int xsize = ageband->GetXSize();
			const unsigned int ysize = ageband->GetYSize();
			Spatial::FMTLayer<Core::FMTDevelopment>mapping(pad, xsize, ysize, projection, cellsize);
			int missing = 0;
			unsigned int ystack = 0;
			for (int iYBlock = 0; iYBlock < nYBlocks; iYBlock++)
			{
				unsigned int xstack = 0;
				int nYValid = 0;
				for (int iXBlock = 0; iXBlock < nXBlocks; iXBlock++)
				{
					int  nXValid;
					if (CE_None != ageband->ReadBlock(iXBlock, iYBlock, &agedata[0]))
					{
						_exhandler->raise(Exception::FMTexc::FMTinvalidrasterblock,
							agedataset->GetDescription(),
							"FMTAreaParser::readRasters", __LINE__, __FILE__, m_section);
					}
					if (lockdataset != NULL)
					{
						if (CE_None != lockband->ReadBlock(iXBlock, iYBlock, &lockdata[0]))
						{
							_exhandler->raise(Exception::FMTexc::FMTinvalidrasterblock,
								lockdataset->GetDescription(),
								"FMTAreaParser::readRasters", __LINE__, __FILE__, m_section);
						}
					}
					ageband->GetActualBlockSize(iXBlock, iYBlock, &nXValid, &nYValid);
					std::unordered_map<int, std::string>mapattributes;
					boost::unordered_map<int, Spatial::FMTCoordinate>coordinates;
					std::vector<int>indexes;
					std::vector<int>counts(static_cast<size_t>(nXBlockSize) * static_cast<size_t>(nYBlockSize), 0);
					for (size_t themeid = 0; themeid < data_rasters.size(); ++themeid)
					{
						if (CE_None != bands[themeid]->ReadBlock(iXBlock, iYBlock, &attributedata[0]))
						{
							_exhandler->raise(Exception::FMTexc::FMTinvalidrasterblock,
								datasets[themeid]->GetDescription(),"FMTAreaParser::readRasters", __LINE__, __FILE__, m_section);
						}
						unsigned int y = ystack;
						for (int iY = 0; iY < nYValid; iY++)
						{
							unsigned int x = xstack;
							for (int iX = 0; iX < nXValid; iX++)
							{
								const unsigned int baselocation = (iX + iY * nXBlockSize);
								int intattribute = attributedata[baselocation];
								if (intattribute != nodata)
								{
									const std::string& attribute = attributes[themeid][intattribute];
									if (mapattributes.find(baselocation) == mapattributes.end())
									{
										mapattributes[baselocation] = "";
									}
									mapattributes[baselocation] += (attribute + " ");
									if (themeid == 0)
									{
										if (x>static_cast<size_t>(std::numeric_limits<uint16_t>::max())||
											y>static_cast<size_t>(std::numeric_limits<uint16_t>::max()))
										{
											_exhandler->raise(Exception::FMTexc::FMTrangeerror,
												"Coordinate out of bounds "+std::to_string(x)+" "+std::to_string(y),
												"FMTAreaParser::readRasters", __LINE__, __FILE__, m_section);
										}
										const uint16_t xValue = static_cast<uint16_t>(x);
										const uint16_t yValue = static_cast<uint16_t>(y);
										coordinates[baselocation] = Spatial::FMTCoordinate(xValue, yValue);
										indexes.push_back(baselocation);
									}
									counts[baselocation] += 1;
								}
								++x;
							}
							++y;
						}
					}
					const size_t attcounts = themes.size();
					if (!mapattributes.empty())
					{
						std::unordered_map<std::string, Core::FMTMask>cacheMask;
						for (const int& location : indexes)
						{
							//std::string st = att->second;
							//const int location = att->first;
							const std::string& st = mapattributes.at(location);
							if (counts[location] == attcounts && agedata[location] != nodata)
							{
								std::string maskvalue = st.substr(0, st.size() - 1);
								Core::FMTMask mask;
								if (cacheMask.find(maskvalue) == cacheMask.end())
									{
									const std::string originalMask = maskvalue;
									if (!Core::FMTTheme::validate(themes, maskvalue, " at line " + std::to_string(m_line))) continue;
									mask = Core::FMTMask(maskvalue, themes);
									cacheMask[originalMask] = mask;
								}else {
									mask = cacheMask.at(maskvalue);
								}
								int lock = 0;
								if (!lockdata.empty())
								{
									if (lockdata[location] != nodata)
									{
										lock = lockatts[lockdata[location]];
									}
								}
								Core::FMTDevelopment dev(mask, int(agedata[location] * agefactor), lock);
								//dev.passinobject(*this);
								mapping[coordinates[location]] = dev;
							}
							else {
								++missing;
							}
						}
					}
					xstack += nXValid;
				}
				ystack += nYValid;
			}
			if (missing > 0)
			{
				const std::string message = " for " + std::to_string(missing) + " raster cells";
				_exhandler->raise(Exception::FMTexc::FMTmissingrasterattribute, message,
					"FMTAreaParser::readRasters", __LINE__, __FILE__, m_section);
			}
			
			GDALClose(agedataset);
			for (GDALDataset* dataset : datasets)
				{
				GDALClose(dataset);
				}
			if (lockdataset != NULL)
				{
				GDALClose(lockdataset);
				}
			return mapping;
		}catch (...)
			{
				_exhandler->printExceptions("", "FMTAreaParser::readRasters", __LINE__, __FILE__, m_section);
			}



	return Spatial::FMTForest();
	}

	Core::FMTActualDevelopment FMTAreaParser::getFeatureToDevelopment(
		const OGRFeature* feature,
		const std::vector<Core::FMTTheme>& themes,
		const std::map<int, int>& themes_fields,
		const int& age_field,
		const int& lock_field,
		const int& area_field,
		const double& agefactor,
		const double& areafactor,
		const double& minimalarea) const
		{
		try {
			const int age = static_cast<int>(feature->GetFieldAsInteger(age_field)*agefactor);
			const double area = (feature->GetFieldAsDouble(area_field)*areafactor);
			if (area > minimalarea)
			{
				int lock = 0;
				if (lock_field != -1)
				{
					std::string slock = feature->GetFieldAsString(lock_field);
					if (!slock.empty())
					{
						boost::to_upper(slock);
						slock.erase(0, 5);
						boost::trim(slock);
						if (isValid(slock))
						{
							lock = getNum<int>(slock);
						}
					}
				}
				std::vector<std::string>masks(themes_fields.size());
				for (std::map<int, int>::const_iterator it = themes_fields.begin(); it != themes_fields.end(); ++it)
				{
					std::string attribute = feature->GetFieldAsString(it->second);
					boost::to_upper(attribute);
					masks[it->first] = attribute;
				}
				std::string tmask = boost::algorithm::join(masks, " ");
				if (Core::FMTTheme::validate(themes, tmask, " at line " + std::to_string(m_line)))
				{
					Core::FMTMask mask(tmask, themes);
					size_t emptyTheme = themes_fields.size();
					while (emptyTheme < themes.size())
						{
						mask.set(themes.at(emptyTheme),*themes.at(emptyTheme).getBaseAttributes().begin());
						++emptyTheme;
						}
					Core::FMTActualDevelopment newDev(mask, age, lock, area);
					//newDev.passinobject(*this);
					return newDev;
				}
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch(std::to_string(feature->GetFID()),
				"FMTAreaParser::getFeatureToDevelopment", __LINE__, __FILE__, m_section);
			}
		return Core::FMTActualDevelopment();
		}

	GDALDataset* FMTAreaParser::openVectorFile(std::map<int, int>&themes_fields,int& age_field,int& lock_field,int& area_field,
		const std::string& data_vectors,const std::string& agefield,const std::string& areafield,const std::string& lockfield,
		const std::vector<Core::FMTTheme>& themes) const
		{
		GDALDataset* dataset=nullptr;
		try {
			//GDALAllRegister();
			dataset = getVectorDataset(data_vectors);
			OGRLayer*  layer = getLayer(dataset, 0);
			getWSFields(layer, themes_fields, age_field, area_field, lock_field, agefield, areafield, lockfield);
			if (themes_fields.size() > themes.size())
				{
				_exhandler->raise(Exception::FMTexc::FMTinvalid_maskrange,
					dataset->GetDescription(),"FMTAreaParser::openVectorFile", __LINE__, __FILE__, m_section);
				}
			layer->ResetReading();
		}catch (...)
			{
			_exhandler->raiseFromCatch(data_vectors,"FMTAreaParser::openVectorFile", __LINE__, __FILE__, m_section);
			}
		return dataset;
		}

	OGRLayer* FMTAreaParser::subsetLayer(OGRLayer*layer ,const std::vector<Core::FMTTheme>& themes,
									const std::string& agefield, const std::string& areafield) const
		{
		std::string sqlcall;
		try {
			size_t thid = 1;
			std::vector<std::string>elements;
			for (const Core::FMTTheme& theme : themes)
			{
				elements.push_back("THEME" + std::to_string(thid));
				++thid;
			}
			elements.push_back(agefield);
			elements.push_back(areafield);
			sqlcall = boost::algorithm::join(elements, " IS NOT NULL AND ");
			sqlcall += " IS NOT NULL";
			layer->SetAttributeFilter(sqlcall.c_str());
		}catch (...)
			{
			_exhandler->raiseFromCatch("for SQL " + sqlcall,"FMTAreaParser::subsetLayer", __LINE__, __FILE__, m_section);
			}
		return layer;
		}

	std::vector<Core::FMTActualDevelopment>FMTAreaParser::readVectors(const std::vector<Core::FMTTheme>& themes,const std::string& data_vectors,
		const std::string& agefield,const std::string& areafield,double agefactor,double areafactor, std::string lockfield,double minimalarea) const
        {
		std::vector<Core::FMTActualDevelopment>devs;
		try {
			std::map<int, int>themes_fields;
			int age_field = -1;
			int lock_field = -1;
			int area_field = -1;
			GDALDataset* dataset = openVectorFile(themes_fields, age_field, lock_field, area_field, data_vectors, agefield, areafield, lockfield, themes);
			OGRLayer*  layer = getLayer(dataset, 0);
			layer = this->subsetLayer(layer, themes, agefield, areafield);
			OGRFeature *feature;
			while ((feature = layer->GetNextFeature()) != NULL)
			{
				const Core::FMTActualDevelopment actualdev = this->getFeatureToDevelopment(feature, themes, themes_fields, age_field,
					lock_field, area_field, agefactor, areafactor, minimalarea);

				if (!actualdev.getMask().empty())
				{
					std::vector<Core::FMTActualDevelopment>::iterator it = find(devs.begin(), devs.end(), actualdev);
					if (it != devs.end())
					{
						it->setArea(it->getArea() + actualdev.getArea());
					}
					else {
						devs.push_back(actualdev);
					}
				}
				OGRFeature::DestroyFeature(feature);
				++m_line;
			}
			GDALClose(dataset);
		}catch (...)
		{
			_exhandler->printExceptions("at " + data_vectors, "FMTAreaParser::readVectors", __LINE__, __FILE__, m_section);
		}


        return devs;
        }

	void FMTAreaParser::vectorFieldToRaster(const std::string& data_vectors,
											const std::string& tifpathandname,
											const int& resolution,
											const std::string& field,
											bool fittoforel) const
	{
		try {
				GDALDataset*  dataset = getVectorDataset(data_vectors);
				OGRLayer* layer = getLayer(dataset, 0);
				OGRFeatureDefn* basedefinition = layer->GetLayerDefn();
				const int fieldid = basedefinition->GetFieldIndex(field.c_str());
				if (fieldid==-1)
					{
					_exhandler->raise(Exception::FMTexc::FMTmissingfield, field + " " + layer->GetDescription(),
						"FMTAreaParser::vectorFieldToRaster", __LINE__, __FILE__, m_section);
					}
				if (basedefinition->GetFieldDefn(fieldid)->GetType() == OGRFieldType::OFTReal)
					{
					_exhandler->raise(Exception::FMTexc::FMTinvalidlayer, std::string(layer->GetDescription())+" with Real format for field "+ field,
						"FMTAreaParser::vectorFieldToRaster", __LINE__, __FILE__, m_section);
					}
				bool usecategories = true;
				if (basedefinition->GetFieldDefn(fieldid)->GetType() == OGRFieldType::OFTInteger||
					basedefinition->GetFieldDefn(fieldid)->GetType() == OGRFieldType::OFTInteger64)
					{
					usecategories = false;
					}
				OGRCoordinateTransformation* coordtransf = getProjTransform(layer, fittoforel);
				GDALDataset* memds = getTransFormMemLayerCopy(layer, coordtransf->GetTargetCS(), field);
				OGRLayer* memlayer = getLayer(memds, 0);
				OGRFeatureDefn* memlayerdef = memlayer->GetLayerDefn();
				OGRFeature* feature;
				std::set<std::string>values;
				if (usecategories)
				{
					while ((feature = layer->GetNextFeature()) != NULL)
					{
						const std::string fieldvalue = feature->GetFieldAsString(field.c_str());
						values.insert(fieldvalue);
						OGRFeature::DestroyFeature(feature);
					}
					layer->ResetReading();
				}
				while ((feature = layer->GetNextFeature()) != NULL)
					{
					
					int newfieldvalue = 0;
					if (usecategories)
					{
						const std::string fieldvalue = feature->GetFieldAsString(field.c_str());
						newfieldvalue = static_cast<int>(std::distance(values.begin(), values.find(fieldvalue)));
					}else {
						newfieldvalue = feature->GetFieldAsInteger(field.c_str());
						}
					OGRGeometry* geom= feature->GetGeometryRef()->clone();
					geom->transform(coordtransf);
					if (!geom->IsValid())
					{
						_exhandler->raise(Exception::FMTexc::FMTinvalid_geometry,
							"for feature " + std::to_string(m_line), "FMTAreaParser::vectorFieldToRaster", __LINE__, __FILE__, m_section);
					}else {
						OGRFeature* memfeature;
						memfeature = OGRFeature::CreateFeature(memlayerdef);
						memfeature->SetGeometry(geom);
						memfeature->SetField(field.c_str(),newfieldvalue);
						if (memlayer->CreateFeature(memfeature) != OGRERR_NONE)
						{
							_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
								"feature " + std::to_string(m_line) + " in memory ", "FMTAreaParser::vectorFieldToRaster", __LINE__, __FILE__, m_section);
						}
						OGRFeature::DestroyFeature(memfeature);
					}
					OGRFeature::DestroyFeature(feature);
					}
				if (memlayer->GetFeatureCount() <= 0)
				{
					_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
						"No feature were created in the memory layer, check the areafield",
						"FMTAreaParser::vectorFieldToRaster", __LINE__, __FILE__, m_section);
				}
				OGRCoordinateTransformation::DestroyCT(coordtransf);
				GDALDataset* fieldraster = ogrLayerToRaster(memlayer, field,tifpathandname, resolution, fittoforel);
				GDALClose(memds);
				GDALRasterBand* fieldband = getBand(fieldraster);
				if (usecategories)
					{
					std::vector<std::string>categories(values.begin(), values.end());
					setCategories(fieldband, categories);
					}
				fieldband->ComputeStatistics(FALSE, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
				fieldband->FlushCache();
				GDALClose(fieldraster);
		}catch (...)
			{
			_exhandler->printExceptions("at " + data_vectors,
				"FMTAreaParser::vectorFieldToRaster", __LINE__, __FILE__, m_section);
			}

	}

	Spatial::FMTForest FMTAreaParser::vectormaptoFMTforest( const std::string& data_vectors,
															const int& resolution,const std::vector<Core::FMTTheme>& themes,
															const std::string& agefield,const std::string& areafield,double agefactor,
															double areafactor, std::string lockfield,
															double minimalarea, const std::string& writeforestfolder, 
															const bool& fittoforel) const
	{
		Spatial::FMTForest basemap;
		std::vector<Core::FMTActualDevelopment>devs;
		try {
			/*
			The first part open the dataset and get a subset of the layer with only the fields needed. It's also a selection of only the features that have data. 
			*/
			std::map<int, int>themes_fields;
			int age_field = -1;
			int lock_field = -1;
			int area_field = -1;
			GDALDataset* dataset = openVectorFile(themes_fields, age_field, lock_field, area_field, data_vectors, agefield, areafield, lockfield, themes);
			OGRLayer*  layer = getLayer(dataset, 0);
			const std::vector<Core::FMTTheme>THEMES_SUBSET(themes.begin(), themes.begin() + themes_fields.size());
			layer = this->subsetLayer(layer, THEMES_SUBSET, agefield, areafield);
			OGRCoordinateTransformation* coordtransf = getProjTransform(layer, fittoforel);
			const std::string Fieldname("devid");
			GDALDataset* memds = getTransFormMemLayerCopy(layer, coordtransf->GetTargetCS(), Fieldname);
			OGRLayer* memlayer = getLayer(memds,0);
			OGRFeatureDefn* memlayerdef = memlayer->GetLayerDefn();
			int devid = 0;
			OGRFeature* feature;
			while ((feature = layer->GetNextFeature()) != NULL)
			{
				const Core::FMTActualDevelopment actualdev = this->getFeatureToDevelopment(feature, themes, themes_fields, age_field,
					lock_field, area_field, agefactor, areafactor, minimalarea);

				if (!actualdev.getMask().empty())
				{
					//Only keep valid developments
					devs.push_back(actualdev);
					//memlayer part
					OGRGeometry* geom;
					geom = feature->GetGeometryRef()->clone();
					//if (reproject)
					//{
						geom->transform(coordtransf);
					//}
					if(!geom->IsValid())
					{
						_exhandler->raise(Exception::FMTexc::FMTinvalid_geometry,
										"for feature "+std::to_string(m_line),"FMTAreaParser::vectormaptoFMTforest", __LINE__, __FILE__, m_section);
					}else {
						OGRFeature* memfeature;
						memfeature = OGRFeature::CreateFeature(memlayerdef);
						memfeature->SetGeometry(geom);
						memfeature->SetField(Fieldname.c_str(), devid);
						if (memlayer->CreateFeature(memfeature) != OGRERR_NONE)
						{
							_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
								"feature " + std::to_string(m_line) + " in memory ", "FMTAreaParser::vectormaptoFMTforest", __LINE__, __FILE__, m_section);
						}
						OGRFeature::DestroyFeature(memfeature);
					}
					OGRGeometryFactory::destroyGeometry(geom);

					++devid;
				}
				OGRFeature::DestroyFeature(feature);
				++m_line;
			}
			if(memlayer->GetFeatureCount()<=0)
			{
				_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
								"No feature where create in the memory layer, check the areafield : "+areafield+" and the agefield : "+agefield+" because they are used for a subset selection on the layer.",
								"FMTAreaParser::vectormaptoFMTforest", __LINE__, __FILE__, m_section);
			}
			GDALClose(dataset);
			//if (reproject)
			//{
				OGRCoordinateTransformation::DestroyCT(coordtransf);
			//}
			basemap = getFMTforestfromlayer(memlayer,devs,Fieldname,resolution,areafactor,fittoforel);
			GDALClose(memds);
			writeForest(basemap, themes, writeforestfolder);
		}catch (...)
		{
			_exhandler->printExceptions("at " + data_vectors, "FMTAreaParser::vectormaptoFMTforest", __LINE__, __FILE__, m_section);
		}
		return basemap;
	}

	void FMTAreaParser::writeForestExtended(Spatial::FMTForest& p_forest, 
		const std::string& p_VectorFile,const std::vector<Core::FMTTheme> p_themes,
		const std::vector<Core::FMTActualDevelopment>& p_area, const std::string& p_folder) const
	{
		try {
			if (!_isMapWithSameThemes(p_themes, p_VectorFile));
			{
				p_forest.setLastThemeWithArea(p_area, p_themes);
			}
			writeForest(p_forest, p_themes, p_folder);
		}catch (...)
			{
			_exhandler->printExceptions("",
				"FMTAreaParser::writeForestExtended", 
				__LINE__, __FILE__, m_section);
			}
	}



	Spatial::FMTForest FMTAreaParser::getFMTforestfromlayer(OGRLayer* layer,const std::vector<Core::FMTActualDevelopment>& actualdevs, const std::string& devidfield, const int& resolution, const double& areafactor,const bool& fittoforel) const
	{
		//GDALAllRegister();
		Spatial::FMTForest actualforest;
		try{
			const std::string vsi_path = "/vsimem/"+devidfield+".tif";
			GDALDataset* devidds = ogrLayerToRaster(layer,devidfield,vsi_path,resolution,fittoforel);
			GDALRasterBand* devidband = getBand(devidds);
			int nXBlockSize, nYBlockSize;
			devidband->GetBlockSize(&nXBlockSize, &nYBlockSize);
			int nXBlocks = (devidband->GetXSize() + nXBlockSize - 1) / nXBlockSize;
			int nYBlocks = (devidband->GetYSize() + nYBlockSize - 1) / nYBlockSize;
			int nodata = int(devidband->GetNoDataValue());
			std::vector<GInt32>iddata(static_cast<size_t>(nXBlockSize) * static_cast<size_t>(nYBlockSize));
			std::vector<double>pad(6);
			devidds->GetGeoTransform(&pad[0]);
			double cellsize = (abs(pad[1] * pad[5]) * areafactor);
			std::map<Spatial::FMTCoordinate, Core::FMTDevelopment>mapping;
			unsigned int ystack = 0;
			for (int iYBlock = 0; iYBlock < nYBlocks; iYBlock++)
			{
				unsigned int xstack = 0;
				int nYValid = 0;
				for (int iXBlock = 0; iXBlock < nXBlocks; iXBlock++)
				{
					int  nXValid;
					if (CE_None != devidband->ReadBlock(iXBlock, iYBlock, &iddata[0]))
					{
						_exhandler->raise(Exception::FMTexc::FMTinvalidrasterblock,
							devidds->GetDescription(),
							"FMTAreaParser::readRasters", __LINE__, __FILE__, m_section);
					}
					devidband->GetActualBlockSize(iXBlock, iYBlock, &nXValid, &nYValid);
					unsigned int y = ystack;
					for (int iY = 0; iY < nYValid; iY++)
					{
						unsigned int x = xstack;
						for (int iX = 0; iX < nXValid; iX++)
						{
							const unsigned int baselocation = (iX + iY * nXBlockSize);
							int ldevid = iddata[baselocation];
							if (ldevid != nodata)
							{
								if (x > static_cast<size_t>(std::numeric_limits<uint16_t>::max()) ||
									y > static_cast<size_t>(std::numeric_limits<uint16_t>::max()))
								{
									_exhandler->raise(Exception::FMTexc::FMTrangeerror,
										"Coordinate out of bounds " + std::to_string(x) + " " + std::to_string(y),
										"FMTAreaParser::readRasters", __LINE__, __FILE__, m_section);
								}
								const uint16_t xValue = static_cast<uint16_t>(x);
								const uint16_t yValue = static_cast<uint16_t>(y);
								mapping.emplace(Spatial::FMTCoordinate(xValue, yValue),actualdevs.at(ldevid));
							}
							++x;
						}
						++y;
					}
					xstack += nXValid;
				}
				ystack += nYValid;
			}
			const std::string projection = devidds->GetProjectionRef();
			const unsigned int xsize = devidband->GetXSize();
			const unsigned int ysize = devidband->GetYSize();
			actualforest = Spatial::FMTForest(Spatial::FMTLayer<Core::FMTDevelopment>(mapping, pad, xsize, ysize, projection, cellsize));
			GDALClose(devidds);
			VSIUnlink(vsi_path.c_str());
		}catch (...)
		{
			_exhandler->printExceptions("", "FMTAreaParser::getFMTforestfromlayer", __LINE__, __FILE__, m_section);
		}
		return actualforest;
	}

	GDALDataset* FMTAreaParser::ogrLayerToRaster(OGRLayer* layer, const std::string& fieldname, const std::string& outfilename, const int& resolution,const bool& fittoforel) const
	{
		//GDALAllRegister();
		const char *pszFormat = "GTiff";
		GDALDriver *poDriver = nullptr;
		GDALDataset *nDS = nullptr;
		try{
			int NXSize,NYSize;
			OGREnvelope layerextent;
			if (layer->GetExtent(&layerextent) != OGRERR_NONE)
			{
				_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
											"Getting the layer extent of "+std::string(layer->GetDescription()),"FMTParser::ogrLayerToRaster", __LINE__, __FILE__, m_section);
			}
			if (!layerextent.IsInit())
			{
				_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
											"Layer extent of "+std::string(layer->GetDescription())+" is not Init","FMTParser::ogrLayerToRaster", __LINE__, __FILE__, m_section);
			}
			double min_y = layerextent.MinY;
			double min_x = layerextent.MinX;
			const double x_delta = layerextent.MaxX - layerextent.MinX;
			const double y_delta = layerextent.MaxY - layerextent.MinY;
			if (fittoforel)
			{
				std::unique_ptr<OGRSpatialReference> forelref = getFORELSpatialRef();
				if(layer->GetSpatialRef()->IsSame(&*forelref))
				{
					const double minxforel = -831600;
					const double minyforel = 117980;
					min_x = minxforel+round((layerextent.MinX-minxforel)/resolution)*resolution;
					min_y = minyforel+round((layerextent.MinY-minyforel)/resolution)*resolution;
				}else{
						_exhandler->raise(Exception::FMTexc::FMTinvalidlayer,
											"Layer spatial reference is not ESPG::32198 and fittoforel == True. Layer must be reproject in ESPG::32198 to use the option fitttoforel",
											"FMTParser::ogrLayerToRaster", __LINE__, __FILE__, m_section);
				}
			}
	        NXSize = static_cast<int>((x_delta / resolution) * (resolution / 20));
			NYSize = static_cast<int>((y_delta / resolution) * (resolution / 20));
			poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
			if( poDriver == nullptr )
			{
				_exhandler->raise(Exception::FMTexc::FMTinvaliddriver,
					std::string(pszFormat),"FMTParser::ogrLayerToRaster", __LINE__, __FILE__, m_section);
			}
			char **papszOptions = NULL;
			papszOptions = CSLSetNameValue( papszOptions, "TILED", "YES" );
			papszOptions = CSLSetNameValue( papszOptions, "BLOCKXSIZE", "128" );
			papszOptions = CSLSetNameValue( papszOptions, "BLOCKYSIZE", "128" );
			papszOptions = CSLSetNameValue( papszOptions, "COMPRESS", "LZW" );
			papszOptions = CSLSetNameValue( papszOptions, "ZLEVEL", "9" );
			papszOptions = CSLSetNameValue( papszOptions, "BIGTIFF", "YES" );
			GDALDataset *poDstDS = nullptr;
			const char * basename = "/vsimem/base.tif";
			if (resolution == 20)
			{
				poDstDS  = poDriver->Create(outfilename.c_str(), NXSize, NYSize, 1, GDT_Int32, papszOptions);
			}else{
				poDstDS  = poDriver->Create(basename, NXSize, NYSize, 1, GDT_Int32, papszOptions);
			}
			if (poDstDS == nullptr)
			{
				_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
									"Dataset to : "+outfilename,"FMTParser::ogrLayerToRaster", __LINE__, __FILE__, m_section);
			}
			std::vector<double>geotrans(6,0);
			geotrans[0]=min_x;
			geotrans[1]=20;
			geotrans[3]=(20*NYSize)+min_y;
			geotrans[5]=-20;
			char* spref;
			if (layer->GetSpatialRef()->exportToWkt(&spref)!=OGRERR_NONE)
			{
				_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
											"Spatial reference "+std::string(poDstDS->GetDescription()),"FMTParser::ogrLayerToRaster", __LINE__, __FILE__, m_section);
			}
			poDstDS->SetProjection(spref);
			poDstDS->SetGeoTransform(&geotrans[0]);
        	poDstDS->GetRasterBand(1)->Fill(-9999);
			poDstDS->FlushCache();
			char **rasterizeOptions = NULL;
			rasterizeOptions = CSLSetNameValue( rasterizeOptions, "ATTRIBUTE", fieldname.c_str() );
			int bandlist[1]={1};
			OGRLayerH layers[1] = {layer};
			GDALRasterizeLayers(poDstDS,1,bandlist,1, layers,NULL,NULL,NULL,rasterizeOptions,NULL,NULL);
			CSLDestroy( rasterizeOptions );
			if (resolution == 20)
			{
				CSLDestroy( papszOptions );
				poDstDS->GetRasterBand(1)->SetNoDataValue(-9999);
				poDstDS->FlushCache();
				return poDstDS;
			}
			const int resYsize = static_cast<int>(round(y_delta/resolution));
			const int resXsize = static_cast<int>(round(x_delta/resolution));
			nDS  = poDriver->Create(outfilename.c_str(), resXsize, resYsize, 1, GDT_Int32, papszOptions);
			if (nDS == nullptr)
			{
				_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
									"Dataset to : "+outfilename,"FMTParser::ogrLayerToRaster", __LINE__, __FILE__, m_section);
			}
			geotrans[0]=min_x;
			geotrans[1]=resolution;
			geotrans[3]=(resolution*resYsize)+min_y;
			geotrans[5]=-resolution;
			nDS->SetProjection(spref);
			nDS->SetGeoTransform(&geotrans[0]);
        	nDS->GetRasterBand(1)->Fill(-9999);
			nDS->FlushCache();
			CPLFree(spref);
			CSLDestroy( papszOptions );
			GDALReprojectImage(poDstDS, NULL, nDS, NULL, GRA_Mode , 0.0, 0.0, NULL, NULL,NULL);
			nDS->GetRasterBand(1)->SetNoDataValue(-9999);//We only set the nodata here to be sure that is not ignore in the resampling
			GDALClose(poDstDS);
			VSIUnlink(basename);
			nDS->FlushCache();
		}catch (...)
		{
			_exhandler->raiseFromCatch(layer->GetDescription(), "FMTAreaParser::ogrLayerToRaster", __LINE__, __FILE__, m_section);
		}
		return nDS;
	}
	#ifdef FMTWITHOSI



	std::vector<OGRMultiPolygon>FMTAreaParser::_GetMultiPolygons(
		const std::vector<Heuristics::FMTOperatingArea>& operatingareas,
		const std::vector<Core::FMTTheme>& themes, 
		const std::string& data_vectors,
		const std::string& agefield, 
		const std::string& areafield, 
		double agefactor,
		double areafactor, 
		std::string lockfield,
		double minimal_area) const
		{
		std::vector<OGRMultiPolygon>multipolygons(operatingareas.size(), OGRMultiPolygon());
		try {
			std::map<int, int>themes_fields;
			int age_field = -1;
			int lock_field = -1;
			int area_field = -1;
			GDALDataset* dataset = this->openVectorFile(themes_fields, age_field, lock_field, area_field,
				data_vectors, agefield, areafield, lockfield, themes);
			OGRLayer * layer = getLayer(dataset, 0);
			layer = this->subsetLayer(layer, themes, agefield, areafield);
			OGRFeature *feature;
			while ((feature = layer->GetNextFeature()) != NULL)
			{
				const Core::FMTActualDevelopment actualdev = this->getFeatureToDevelopment(feature, themes, themes_fields, age_field,
				lock_field, area_field, agefactor, areafactor, minimal_area);
				if (!actualdev.getMask().empty())
				{
					size_t opid = 0;
					bool foundoaunit = false;
					for (const Heuristics::FMTOperatingArea& oparea : operatingareas)
					{
						if (actualdev.getMask().isSubsetOf(oparea.getMask()))
						{
							foundoaunit = true;
							break;
						}
						++opid;
					}
					if (foundoaunit)
					{
						const OGRGeometry* polygon = feature->GetGeometryRef();
						if (polygon!=nullptr&&polygon->IsValid())
						{
							multipolygons.at(opid).addGeometry(polygon->clone());
						}	
					}
				}
				OGRFeature::DestroyFeature(feature);
			}
			GDALClose(dataset);
		} catch (...)
			{
			_exhandler->raiseFromCatch("","FMTAreaParser::_GetMultiPolygons", __LINE__, __FILE__, m_section);
			}
		return multipolygons;
		}
	#endif


	template<typename T, typename outT>
	bool FMTAreaParser::writeBand(const Spatial::FMTLayer<T>& layer, GDALRasterBand* wband, const std::map<T, std::string>& mapping) const
	{
		double lastwriten;
		bool gotSomething = false;
		try {
			int nXBlockSize, nYBlockSize;
			wband->GetBlockSize(&nXBlockSize, &nYBlockSize);
			int nXBlocks = (wband->GetXSize() + nXBlockSize - 1) / nXBlockSize;
			int nYBlocks = (wband->GetYSize() + nYBlockSize - 1) / nYBlockSize;
			lastwriten = wband->GetNoDataValue();
			const outT nodata = static_cast<outT>(wband->GetNoDataValue());
			unsigned int ystack = 0;
			for (int iYBlock = 0; iYBlock < nYBlocks; iYBlock++)
			{
				int nYValid = 0;
				unsigned int xstack = 0;
				for (int iXBlock = 0; iXBlock < nXBlocks; iXBlock++)
				{
					std::vector<outT>block(static_cast<size_t>(nXBlockSize * nYBlockSize), nodata);
					int  nXValid;
					wband->GetActualBlockSize(iXBlock, iYBlock, &nXValid, &nYValid);
					bool somethinginblock = false;
					unsigned int y = ystack;
					for (int iY = 0; iY < nYValid; iY++)
					{
						unsigned int x = xstack;
						for (int iX = 0; iX < nXValid; iX++)
						{
							Spatial::FMTCoordinate coordinate(x, y);
							typename Spatial::FMTLayer<T>::const_iterator it = layer.find(coordinate);
							if (it != layer.end())
								{
								somethinginblock = true;
								if (!mapping.empty())
									{
									block[iX + iY * nXBlockSize] = static_cast<outT>(std::distance(mapping.begin(), mapping.find(it->second)));
								}else {
									block[iX + iY * nXBlockSize] = boost::lexical_cast<outT>(it->second);
									}
								}
							++x;
							}
						++y;
						}
						
					if (somethinginblock)
						{
						gotSomething = true;
						if (wband->WriteBlock(iXBlock, iYBlock, &block[0]) != CPLErr::CE_None)
							{
								_exhandler->raise(Exception::FMTexc::FMTinvalidrasterblock,
								"on band id "+	std::to_string(wband->GetBand()), "FMTAreaParser::writeBand", __LINE__, __FILE__, m_section);
							}
						}
					xstack += nXValid;
					}
				ystack += nYValid;
				}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("last "+ std::to_string(lastwriten) + "at band id " + std::to_string(wband->GetBand()), "FMTAreaParser::writeLayer", __LINE__, __FILE__, m_section);
		}
		return gotSomething;
	}

        template<typename T>
        bool FMTAreaParser::writeLayer(
			const Spatial::FMTLayer<T>& layer, 
			std::string location,
			const std::map<T, 
			std::string>& mapping, 
			std::string format) const
            {
			try {
				GDALDataType datatype = GDT_Int32;
				if (std::is_same<double, T>::value)
				{
					datatype = GDT_Float64;
				}
				if (format == "BMP")
				{
					datatype = GDALDataType::GDT_Byte;
				}
				GDALDataset* wdataset = createDataset(location, layer, datatype, format);
				std::vector<std::string> table;
				if (!mapping.empty())
				{
					table.reserve(mapping.size());
					for (typename std::map<T, std::string>::const_iterator it = mapping.begin(); it != mapping.end(); it++)
					{
						table.push_back(it->second);
					}
				}
				GDALRasterBand* wband = createBand(wdataset, table);

				bool bandResult = [&]() {
					switch (datatype) {
					case GDALDataType::GDT_Byte:
						return writeBand<T, uint8_t>(layer, wband, mapping);
					case GDALDataType::GDT_Int32:
						return writeBand<T, int>(layer, wband, mapping);
					default:
						return writeBand<T, double>(layer, wband, mapping);
					}
					}();

				if (datatype == GDALDataType::GDT_Byte || datatype == GDALDataType::GDT_UInt16)
				{
					//Byte or Uint6 only!
					GDALColorTable newcolors(GPI_RGB);
					GDALColorEntry whitekentry;
					whitekentry.c1 = 255;
					whitekentry.c2 = 255;
					whitekentry.c3 = 255;
					//set the white for nodata...
					newcolors.SetColorEntry(wband->GetNoDataValue(), &whitekentry);
					int id = 0;
					//CLEAN UP TABLE
					std::sort(table.begin(), table.end());
					auto last = std::unique(table.begin(), table.end());
					table.erase(last, table.end());
					const double numberofentries = static_cast<double> (table.size() - 1);
					std::default_random_engine generator;
					std::uniform_int_distribution<short>dist(0, 256);
					std::vector<short> c1;
					std::vector<short> c2;
					std::vector<short> c3;
					for (size_t tsize = 0; tsize < table.size(); ++tsize)
					{
						c1.push_back(dist(generator));
						c2.push_back(dist(generator));
						c3.push_back(dist(generator));
					}
					for (typename std::map<T, std::string>::const_iterator it = mapping.begin(); it != mapping.end(); it++)
					{
						//const int n = (static_cast<int>((static_cast<double>(std::distance(table.begin(), std::find(table.begin(), table.end(), it->second))) / numberofentries) * 100));
						const size_t locid = std::distance(table.begin(), std::find(table.begin(), table.end(), it->second));
						GDALColorEntry newentry;
						//const short randomn = dist(generator);
						/*newentry.c1 = (255 * ramdomvals.at(locid)) / 100;
						newentry.c2 = (255 * (100 - ramdomvals.at(locid))) / 100;
						newentry.c3 = 0;*/
						newentry.c1 = c1.at(locid);//dist(generator);
						newentry.c3 = c2.at(locid);//dist(generator);
						newentry.c2 = c3.at(locid);//dist(generator);
						/*newentry.c1 = (255 * n) / 100;
						newentry.c2 = (255 * (100 - n)) / 100;
						newentry.c3 = 0;*/
						newcolors.SetColorEntry(id, &newentry);
						++id;
					}
					wband->SetColorTable(&newcolors);
					wband->SetColorInterpretation(GDALColorInterp::GCI_PaletteIndex);
				}
				if (bandResult) {
					wband->ComputeStatistics(FALSE, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
				}
				wband->FlushCache();
				wdataset->FlushCache();
				GDALClose(wdataset);
			}catch (...)
				{
				_exhandler->raiseFromCatch("at "+location,"FMTAreaParser::writeLayer", __LINE__, __FILE__, m_section);
				}
            return true;
            }


		bool FMTAreaParser::writeLayer(const Spatial::FMTLayer<double>& layer, std::string location, std::string format) const
		{
			try {
				std::map<double, std::string>naming;
				return writeLayer<double>(layer, location, naming, format);
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("at " + location, "FMTAreaParser::writeLayer", __LINE__, __FILE__, m_section);
			}
			return false;
		}

			bool FMTAreaParser::writeForestTheme(
				const Spatial::FMTForest& for_layer,
				const Core::FMTTheme& theme,
				const std::string& location,
				const std::map<std::string, std::string>& mapping,
				std::string format) const
			{
				try {
					const std::vector<Core::FMTTheme>thetheme(1, theme);
					const std::vector<Spatial::FMTLayer<std::string>> themes_layer = for_layer.getThemes(thetheme);
					return writeLayer<std::string>(themes_layer.at(0), location, mapping,format);
				}catch (...)
					{
				_exhandler->raiseFromCatch("at " + location, "FMTAreaParser::writeForestTheme", __LINE__, __FILE__, m_section);
				}
			return false;
			}



	#ifdef FMTWITHOSI
			std::vector<Heuristics::FMTOperatingArea> FMTAreaParser::_GetNeighborsFromPolygons(const std::vector<OGRGeometry*>& polygons,
																						std::vector<Heuristics::FMTOperatingArea> operatingareas,
																						const double& buffersize) const
				{
				try {
					std::map<Core::FMTMask, std::vector<Core::FMTMask>>neighborhood;
					for (size_t opareaindex = 0; opareaindex < operatingareas.size(); ++opareaindex)
					{
						double fullbuffered = 0;
						std::vector<size_t>neighborsid;
						std::vector<double>areas;
						if (polygons.at(opareaindex) && !polygons.at(opareaindex)->IsEmpty() && polygons.at(opareaindex)->IsValid())
						{
							OGRGeometry* buffered = (polygons.at(opareaindex)->Buffer(buffersize));
							for (size_t opareaneighborindex = 0; opareaneighborindex < operatingareas.size(); ++opareaneighborindex)
							{
								if (opareaindex != opareaneighborindex && polygons.at(opareaneighborindex) &&
									buffered->Intersects(polygons.at(opareaneighborindex)))
								{
									OGRGeometry* intersect = buffered->Intersection(polygons.at(opareaneighborindex));
									if (intersect && !intersect->IsEmpty())
									{

										const double AREA = FMTAreaParser::_GetGeometryArea(intersect);
										if (AREA > FMT_DBL_TOLERANCE)
											{
											fullbuffered += AREA;
											neighborsid.push_back(opareaneighborindex);
											areas.push_back(AREA);
											}
									}
									OGRGeometryFactory::destroyGeometry(intersect);
								}
							}
							OGRGeometryFactory::destroyGeometry(buffered);
						}
						std::vector<Core::FMTMask>validneighbors;
						for (size_t neighborid = 0; neighborid < neighborsid.size(); ++neighborid)
						{
							if ((areas.at(neighborid) / fullbuffered) >= operatingareas.at(neighborsid.at(neighborid)).getNeighborsPerimeter())
							{
								validneighbors.push_back(operatingareas.at(neighborsid.at(neighborid)).getMask());
							}
						}
						neighborhood[operatingareas.at(opareaindex).getMask()] = validneighbors;
					}
					//reciprocity
					/////////////
					for (Heuristics::FMTOperatingArea& oparea : operatingareas)
					{
						if (neighborhood.find(oparea.getMask()) != neighborhood.end())
						{
							std::vector<Core::FMTMask>realneighbors;
							for (const Core::FMTMask& nmask : neighborhood.at(oparea.getMask()))
							{
								if (neighborhood.find(nmask) != neighborhood.end() &&
									std::find(neighborhood.at(nmask).begin(), neighborhood.at(nmask).end(), oparea.getMask()) != neighborhood.at(nmask).end())
								{
									realneighbors.push_back(nmask);
								}
							}
							oparea.setNeighbors(realneighbors);
						}
					}
				}catch (...)
					{
					_exhandler->raiseFromCatch("","FMTAreaParser::_GetNeighborsFromPolygons", __LINE__, __FILE__, m_section);
					}
				return operatingareas;
				}


			std::vector<Heuristics::FMTOperatingAreaCluster> FMTAreaParser::getClustersFromPolygons(const std::vector<OGRGeometry*>& polygons,
																								const std::vector<Heuristics::FMTOperatingArea>& operatingareas,
																								const double& maximaldistance) const
			{
				std::vector<Heuristics::FMTOperatingAreaCluster>clusters;
				try {
					if (!(polygons.size()==operatingareas.size()))
						{
						_exhandler->raise(Exception::FMTexc::FMTrangeerror,
							"Invalid number of polygons / operating area",
							"FMTAreaParser::getClustersFromPolygons", __LINE__, __FILE__, m_section);
						}
					std::map<Core::FMTMask, std::map<Core::FMTMask, double>>distances;
					std::map<Core::FMTMask, std::map<Core::FMTMask, std::set<Core::FMTMask>>>excludedfromlink;
				
					size_t opareaid = 0;
					for (const OGRGeometry* polygon : polygons)
						{
						distances[operatingareas.at(opareaid).getMask()] = std::map<Core::FMTMask, double>();
						excludedfromlink[operatingareas.at(opareaid).getMask()] = std::map<Core::FMTMask, std::set<Core::FMTMask>>();
						++opareaid;
						}

					size_t mainopareaid = 0;
					for (const Heuristics::FMTOperatingArea& mainoparea : operatingareas)
						{
						OGRPoint maincentroid;
						const Core::FMTMask mainmask(mainoparea.getMask());
						std::vector<Core::FMTMask>mainneighbors = mainoparea.getNeighbors();
						mainneighbors.push_back(mainmask);
						std::sort(mainneighbors.begin(), mainneighbors.end());
						polygons.at(mainopareaid)->Centroid(&maincentroid);
						size_t sideopareaid = 0;
						std::vector<Heuristics::FMTOperatingAreaClusterBinary>binaries;
						std::vector<Core::FMTMask>binariesmasks;
						std::vector<size_t>polygonids;
						for (const Heuristics::FMTOperatingArea& sideoparea : operatingareas)
							{
							double distance;
							const Core::FMTMask sidemask = sideoparea.getMask();

							if (distances.at(mainmask).find(sidemask) !=
								distances.at(mainmask).end())
								{
								distance = distances.at(mainmask).at(sidemask);
								}else {
								OGRPoint sidecentroid;
								polygons.at(sideopareaid)->Centroid(&sidecentroid);
								distance = maincentroid.Distance(&sidecentroid);
								distances[mainmask][sidemask] = distance;
								distances[sidemask][mainmask] = distance;
								}
							if (distance <= maximaldistance && mainmask != sidemask)
								{
								binaries.push_back(Heuristics::FMTOperatingAreaClusterBinary(sideoparea));
								binariesmasks.push_back(sidemask);
								polygonids.push_back(sideopareaid);
								}
							++sideopareaid;
							}
						std::sort(binariesmasks.begin(), binariesmasks.end());
						size_t binaryid = 0;
						
						std::queue<Core::FMTMask>bannedmask;
						for (Heuristics::FMTOperatingAreaClusterBinary& binary : binaries)
							{
							const Core::FMTMask binarymask(binary.getMask());
							std::vector<Core::FMTMask>linkerneighbors;
							const OGRGeometry* binary_polygon = polygons.at(polygonids.at(binaryid));
							OGRPoint binarycentroid;
							binary_polygon->Centroid(&binarycentroid);
							OGRLineString linkingm_line;
							linkingm_line.setPoint(0, &maincentroid);
							linkingm_line.setPoint(1, &binarycentroid);
							if (excludedfromlink.at(mainmask).find(binarymask)== excludedfromlink.at(mainmask).end())
								{
								excludedfromlink[mainmask][binarymask] = std::set<Core::FMTMask>();
								excludedfromlink[binarymask][mainmask] = std::set<Core::FMTMask>();
								}
							std::set<Core::FMTMask> * exclusion = &excludedfromlink[mainmask][binarymask];
							size_t subbinaryid = 0;
							for (const Heuristics::FMTOperatingAreaClusterBinary& subbinary : binaries)
								{
								const Core::FMTMask subbinarymask(subbinary.getMask());
								if (subbinarymask != binarymask &&
									subbinarymask != mainmask&&
									exclusion->find(subbinarymask)==exclusion->end())
								{
									const OGRGeometry* subbinary_polygon = polygons.at(polygonids.at(subbinaryid));
									if (linkingm_line.Intersects(subbinary_polygon))
									{
										linkerneighbors.push_back(subbinarymask);
									}
									else {
										exclusion->insert(subbinarymask);
									}

								}
								++subbinaryid;
								}
							excludedfromlink[binarymask][mainmask] = *exclusion;
							if (linkerneighbors.empty())//line break-up detector!
							{
								std::vector<Core::FMTMask>binaryn = operatingareas.at(polygonids.at(binaryid)).getNeighbors();
								binaryn.push_back(binarymask);
								std::vector<Core::FMTMask> intersect;
								std::sort(binaryn.begin(), binaryn.end());
								std::set_intersection(mainneighbors.begin(), mainneighbors.end(),
									binaryn.begin(), binaryn.end(),
									std::back_inserter(intersect));
								bool insertiondone = false;
								if (!intersect.empty())
									{
									for (const Core::FMTMask& imask : intersect)
										{
										if (imask==binarymask||imask==mainmask)
											{
											insertiondone = true;
											break;
											}
										}
									if (!insertiondone)
										{
										std::vector<Core::FMTMask>cleanedintersect;
										std::set_intersection(intersect.begin(), intersect.end(),
											binariesmasks.begin(), binariesmasks.end(),
											std::back_inserter(cleanedintersect));
										for (const Core::FMTMask& imask : cleanedintersect)
											{
											linkerneighbors.push_back(imask);
											insertiondone = true;
											}
										}
								}
								if (!insertiondone)
								{
									const double distance = linkingm_line.get_Length();
									OGRGeometry* bufferedbinary = binary_polygon->Buffer(distance);
									OGRGeometry* bufferedmain = polygons.at(mainopareaid)->Buffer(distance);
									OGRGeometry* intersection = bufferedbinary->Intersection(bufferedmain);
									size_t subbinaryid = 0;
									for (const Heuristics::FMTOperatingAreaClusterBinary& subbinary : binaries)
									{
										OGRGeometry* subbinary_polygon = polygons.at(polygonids.at(subbinaryid));
										const Core::FMTMask subbinarymask(subbinary.getMask());

										if (subbinarymask != binarymask && subbinarymask != mainmask && intersection->Intersects(subbinary_polygon))
										{
											OGRGeometry* subintersection = dynamic_cast<OGRPolygon*>(intersection->Intersection(subbinary_polygon));
											if (_GetGeometryArea(subintersection) >= 
												_GetGeometryArea(subbinary_polygon) * 0.5)
											{
												insertiondone = true;
												linkerneighbors.push_back(subbinarymask);
											}
											OGRGeometryFactory::destroyGeometry(subintersection);
										}
										++subbinaryid;
									}
									OGRGeometryFactory::destroyGeometry(intersection);
									OGRGeometryFactory::destroyGeometry(bufferedbinary);
									OGRGeometryFactory::destroyGeometry(bufferedmain); 
								}
								if (!insertiondone)
								{
									bannedmask.push(binarymask);
								}


							}
							binary.setNeighbors(linkerneighbors);
							++binaryid;
							}
						
						while (!bannedmask.empty())
							{
							std::vector<Heuristics::FMTOperatingAreaClusterBinary>cleanedbinaries;
							const Core::FMTMask& mask = bannedmask.front();
							for (const Heuristics::FMTOperatingAreaClusterBinary& binary : binaries)
								{
								const Core::FMTMask binmask = binary.getMask();
								if (mask != binmask)
									{
									bool keepit = true;
									for (const Core::FMTMask& nmask : binary.getNeighbors())
										{
										if (nmask== mask)
											{
											keepit = false;
											break;
											}

										}
									if (keepit)
									{
										cleanedbinaries.push_back(binary);
									}
									else {
										bannedmask.push(binmask);
									}

									}

								}
							bannedmask.pop();
							binaries = cleanedbinaries;
							}

						Heuristics::FMTOperatingAreaClusterBinary basecentroid(mainoparea);
						basecentroid.setNeighbors(std::vector<Core::FMTMask>());
						clusters.push_back(Heuristics::FMTOperatingAreaCluster(basecentroid,binaries));
						++mainopareaid;
						}


				}catch (...)
					{
					_exhandler->raiseFromCatch("", "FMTAreaParser::getClustersFromPolygons", __LINE__, __FILE__, m_section);
					}
				return clusters;
			}


			std::vector<Heuristics::FMTOperatingAreaScheme> FMTAreaParser::getSchemeNeighbors(std::vector<Heuristics::FMTOperatingAreaScheme> operatingareaparameters,
																			const std::vector<Core::FMTTheme>& themes, const std::string& data_vectors,
																			const std::string& agefield, const std::string& areafield, double agefactor,
																			double areafactor, std::string lockfield,
																			double minimal_area , double buffersize) const
				{
				try {

					const std::vector<Heuristics::FMTOperatingArea>baseoparea(operatingareaparameters.begin(), operatingareaparameters.end());
					const std::vector<OGRMultiPolygon>COLLECTIONS = this->_GetMultiPolygons(baseoparea, themes, data_vectors,
						agefield, areafield, agefactor,
						areafactor, lockfield, minimal_area);
					std::vector<OGRGeometry*>GEOMETRIES = this->_GetUnion(COLLECTIONS);
					const std::vector<Heuristics::FMTOperatingArea>schemes = _GetNeighborsFromPolygons(GEOMETRIES, baseoparea, buffersize);
					this->_DestroyGeometries(GEOMETRIES);
					size_t opareaid = 0;
					for (const Heuristics::FMTOperatingArea& oparea : schemes)
						{
						operatingareaparameters[opareaid].setNeighbors(oparea.getNeighbors());
						++opareaid;
						}
				}catch (...)
				{
					_exhandler->printExceptions("", "FMTAreaParser::getSchemeNeighbors", __LINE__, __FILE__, m_section);
				}

				return operatingareaparameters;
				}

			std::vector<Heuristics::FMTOperatingAreaCluster> FMTAreaParser::getClusters(const std::vector<Heuristics::FMTOperatingArea>& operatingareas,
				const std::vector<Core::FMTTheme>& themes, const std::string& data_vectors,
				const std::string& agefield, const std::string& areafield, const double& maximaldistance,
                double agefactor,
				double areafactor, std::string lockfield,
				double minimal_area, double buffersize) const
			{
				std::vector<Heuristics::FMTOperatingAreaCluster>finalclusters;
				try {
					std::vector<OGRMultiPolygon>multipolygons = this->_GetMultiPolygons(operatingareas, themes, data_vectors,
						agefield, areafield, agefactor,
						areafactor, lockfield, minimal_area);
					std::vector<OGRGeometry*>mergedpolygons = this->_GetUnion(multipolygons);
					std::vector<Heuristics::FMTOperatingArea>newopareas(operatingareas.begin(), operatingareas.end());
					const std::vector<Heuristics::FMTOperatingArea>opareawithneighbors = _GetNeighborsFromPolygons(mergedpolygons, newopareas, buffersize);
					finalclusters = this->getClustersFromPolygons(mergedpolygons, opareawithneighbors, maximaldistance);
					this->_DestroyGeometries(mergedpolygons);
				}catch (...)
				{
					_exhandler->printExceptions("", "FMTAreaParser::getClusters", __LINE__, __FILE__, m_section);
				}
				return finalclusters;
			}
			
		#ifdef FMTWITHGDAL
			void FMTAreaParser::writeOAschedulerparameters(const std::string& location, const std::vector<Heuristics::FMTOperatingAreaScheme>& OAschemes,
															std::vector<std::string> creationoptions) const
			{
				try {
					const boost::filesystem::path path(location);
					GDALDataset* new_dataset = createOGRDataset(path.parent_path().string());
					OGRLayer* newlayer = createLayer(new_dataset, path.stem().string(), creationoptions);
					//"OA","OPT","RET","MAXRET","REP","OPR"
					OGRFieldDefn OAField("OA",OFTString);
					OAField.SetWidth(254);
					OGRFieldDefn OPTField("OPT", OFTInteger);
					OPTField.SetWidth(5);
					OGRFieldDefn RETField("RET", OFTInteger);
					RETField.SetWidth(5);
					OGRFieldDefn MAXRETField("MAXRET", OFTInteger);
					MAXRETField.SetWidth(5);
					OGRFieldDefn REPField("REP", OFTInteger);
					REPField.SetWidth(5);
					OGRFieldDefn OPRField("OPR", OFTReal);
					OPRField.SetPrecision(5);
					OPRField.SetWidth(32);
					OGRFieldDefn NEPField("NEP", OFTReal);
					NEPField.SetPrecision(5);
					NEPField.SetWidth(32);
					OGRFieldDefn GUPField("GUP", OFTInteger);
					GUPField.SetWidth(5);


					if (newlayer->CreateField(&OAField) != OGRERR_NONE ||
						newlayer->CreateField(&OPTField) != OGRERR_NONE ||
						newlayer->CreateField(&RETField) != OGRERR_NONE ||
						newlayer->CreateField(&MAXRETField) != OGRERR_NONE ||
						newlayer->CreateField(&REPField) != OGRERR_NONE ||
						newlayer->CreateField(&OPRField) != OGRERR_NONE ||
						newlayer->CreateField(&NEPField) != OGRERR_NONE ||
						newlayer->CreateField(&GUPField) != OGRERR_NONE)
					{
						_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
							"Cannote create new fields outputsdrift", "FMTAreaParser::writeOAschedulerparameters", __LINE__, __FILE__, m_section);
					}
					for (const Heuristics::FMTOperatingAreaScheme& scheme : OAschemes)
						{
						OGRFeature* newfeature = OGRFeature::CreateFeature(newlayer->GetLayerDefn());
						if (newfeature == NULL)
						{
							_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
								"Cannote generate new feature ", "FMTAreaParser::writeOAschedulerparameters", __LINE__, __FILE__, m_section);
							//Failed to generate feature
						}
						newfeature->SetField("OA",std::string(scheme.getMask()).c_str());
						newfeature->SetField("OPT", static_cast<int>(scheme.getOpeningTime()));
						newfeature->SetField("RET", static_cast<int>(scheme.getMinimalReturnTime()));
						newfeature->SetField("MAXRET", static_cast<int>(scheme.getMaximalReturnTime()));
						newfeature->SetField("REP", static_cast<int>(scheme.getRepetition()));
						newfeature->SetField("OPR", static_cast<int>(scheme.getThreshold()));
						newfeature->SetField("NEP", static_cast<double>(scheme.getNeighborsPerimeter()));
						newfeature->SetField("GUP", static_cast<int>(scheme.getGreenUp()));
						if (newlayer->CreateFeature(newfeature) != OGRERR_NONE)
						{
							_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
								"Cannote create new feature id " + std::to_string(newlayer->GetFeatureCount()), "FMTAreaParser::writeOAschedulerparameters", __LINE__, __FILE__, m_section);
							//Failed to generate feature
						}
						OGRFeature::DestroyFeature(newfeature);

						}
				GDALClose(new_dataset);
				}
				catch (...)
				{
					_exhandler->printExceptions("", "FMTAreaParser::writeOAschedulerparameters", __LINE__, __FILE__);
				}
			}
		#endif

			std::vector<Heuristics::FMTOperatingAreaScheme> FMTAreaParser::getOperatingArea(const std::string& fichierShp, const std::vector<Core::FMTTheme>& themes, const int& numeroTheme, const int& startingperiod, const std::string& nomChampAge, const std::string& nomChampSuperficie, const std::string& nomChampStanlock, const std::string& fichierParam) const
			{
				Parser::FMTAreaParser areaParser;
				std::vector<Heuristics::FMTOperatingAreaScheme> opeareas = areaParser.readOAschedulerparameters(fichierParam, themes, numeroTheme, startingperiod);
				for (const auto& op : opeareas)
				{
					if (op.getNeighborsPerimeter() > 0 || op.getGreenUp() > 0)
					{
						Logging::FMTDefaultLogger() << "Lecture des blocs voisins." << "\n";
						opeareas = areaParser.getSchemeNeighbors(opeareas, themes, fichierShp, nomChampAge, nomChampSuperficie, 1.0, 1, nomChampStanlock);
						return opeareas;
					}
				}
				return opeareas;
			}

			std::vector<Heuristics::FMTOperatingAreaScheme> FMTAreaParser::readOAschedulerparameters(const std::string& location, const std::vector<Core::FMTTheme>& modelthemes, const int& themetarget,const int& startingperiod) const
			{
				std::vector<Heuristics::FMTOperatingAreaScheme> schemes;
				try{
					std::ifstream oaparameterfile(location);
					std::vector<std::string> titles = { "OA","OPT","RET","MAXRET","REP"};
					if (tryOpening(oaparameterfile, location))
					{
						int lineid = 0;
						std::string line;
						std::map<std::string, int>columns;
						bool useRETasMAXRET = true;
						while (std::getline(oaparameterfile, line))
						{
							//std::vector<std::string>splittedline = splitoaparamlines(line);
							std::vector<std::string>splittedline;
							line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
							boost::split(splittedline,line,boost::is_any_of(";"));
							if (splittedline.size() > 0)
							{
								if (lineid == 0)
								{
									int colid = 0;
									std::vector<std::string>mvalues;
									for (std::string value : splittedline)
									{
										transform(value.begin(), value.end(), value.begin(), ::toupper);
										columns[value] = colid;
										++colid;
									}
									for (std::string title : titles)
									{
										if (columns.find(title) == columns.end())
										{
											if (title == "MAXRET")
											{
												//add new exception for oascheduler parsing...
												useRETasMAXRET = true;
												*_logger << "MAXRET columns is missing, the value of RET will be used as MAXRET." << "\n";
											}
											else {
												_exhandler->raise(Exception::FMTexc::FMTmissingfield,
													"Missing required column " + title + " in parameters file",
													"FMTAreaParser::readOAschedulerparameters", __LINE__, __FILE__);
											
											}
										}
									}
								}
								else {
									std::string OA = splittedline[columns["OA"]];
									std::transform(OA.begin(), OA.end(), OA.begin(), ::toupper);
									int themeid=0;
									std::string smask = "";
									for (const auto& theme : modelthemes)
									{
										if (themeid == themetarget)
										{
											smask += OA;
										}
										else {
											smask += "?";
										}
										if (themeid != modelthemes.size()-1)
										{
											smask += " ";
										}
										++themeid;
									}
									const Core::FMTMask OAmask(smask,modelthemes);
									for (std::string title : titles)
									{
										if (columns.find(title) != columns.end() && static_cast<int>(splittedline.size()) <= columns[title])
										{
											_exhandler->raise(Exception::FMTexc::FMTmissingfield,
												"Missing required column " + title + " for " + OA + " in parameters file",
												"FMTAreaParser::readOAschedulerparameters", __LINE__, __FILE__);
										}
									}
									const int opt = std::stoi(splittedline[columns["OPT"]]);
									const int ret = std::stoi(splittedline[columns["RET"]]);
									int maxret;
									if (useRETasMAXRET)
									{
										maxret = ret;
									}
									else {
										maxret = std::stoi(splittedline[columns["MAXRET"]]);
									}
									if (maxret < ret)
									{
										maxret = ret;
										*_logger << "MAXRET value for "+OA+" is less than RET value. The MAXRET value will be the same as RET." << "\n";
									}
									const int rep = std::stoi(splittedline[columns["REP"]]);
									double opr = 0.0;
									if (columns.find("OPR") != columns.end())
									{
										//if (int(splittedline.size()) >= 6)
										//{
											if (!splittedline[columns["OPR"]].empty())
											{
												opr = std::stod(splittedline[columns["OPR"]]);
												if (!(opr <= 1.0 && opr >= 0.0))
												{
													_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
														" for " + OA + " opr not <= 1 and >= 0 in parameters file!",
														"FMTAreaParser::readOAschedulerparameters", __LINE__, __FILE__);
												}
											}
										//}
									}
									if (!(opt >= 1))
									{
										_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
											" for " + OA + " opt not >= 1 in parameters file!",
											"FMTAreaParser::readOAschedulerparameters", __LINE__, __FILE__);
									}
									if (!(rep >= 1))
									{
										_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
											" for " + OA + " rep not >= 1 in parameters file!",
											"FMTAreaParser::readOAschedulerparameters", __LINE__, __FILE__);
									}
									double gup = 0;
									double npe = 0;
									if (columns.find("NPE") != columns.end())
									{
										//if (int(splittedline.size()) >= 6)
										//{
											if (!splittedline[columns["NPE"]].empty())
											{
												npe = std::stod(splittedline[columns["NPE"]]);
												if (!(npe <= 1.0 && npe >= 0.0))
												{
													_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
														" for " + OA + " npe not <= 1 and >= 0 in parameters file!",
														"FMTAreaParser::readOAschedulerparameters", __LINE__, __FILE__);
												}
											}
										//}
									}
									if (columns.find("GUP") != columns.end())
									{
										//if (int(splittedline.size()) >= 7)
										//{
											if (!splittedline[columns["GUP"]].empty())
											{
												gup = std::stoi(splittedline[columns["GUP"]]);
												if (!(gup >= 0))
												{
													_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
														" for " + OA + " gup >= 0 in parameters file!",
														"FMTAreaParser::readOAschedulerparameters", __LINE__, __FILE__);
												}
											}
										//}
									}
									schemes.push_back(Heuristics::FMTOperatingAreaScheme(Heuristics::FMTOperatingArea(OAmask, npe),opt,ret,maxret,rep,gup,startingperiod,opr));
								}
							}
							++lineid;
						}
						oaparameterfile.close();
					}
				}
				catch (...)
				{
					_exhandler->printExceptions("", "FMTAreaParser::readOAschedulerparameters", __LINE__, __FILE__);
				}
				return schemes;
			}


		#endif
#endif
			FMTAreaParser::FMTAreaParser() :
				FMTParser()
			{
				m_section = Core::FMTsection::Area;
			}


			bool FMTAreaParser::_isExclude(const std::string& pm_line) const
				{
				bool gotExclude = false;
				boost::smatch theMatch;
				if (boost::regex_search(pm_line, theMatch, m_RxExclude))
					{
					gotExclude = !std::string(theMatch[1]).empty();
					}
				return gotExclude;
				}

			std::string FMTAreaParser::_getExcludeValue(const std::string& pm_line) const
				{
				boost::smatch theMatch;
				std::string excludeLine;
				if (boost::regex_search(pm_line, theMatch, m_RxExclude))
					{
					excludeLine = std::string(theMatch[3]);
					}
				boost::trim(excludeLine);
				return excludeLine;
				}

			std::pair<Core::FMTMask, Core::FMTSpec> FMTAreaParser::_getExcludedSpec(const std::vector<Core::FMTTheme>& p_themes, 
																					const Core::FMTConstants& p_constants,
																					const std::string& p_value) const
				{
				std::pair<Core::FMTMask, Core::FMTSpec>returned;
				boost::smatch theMatch;
				if (boost::regex_search(p_value, theMatch, m_RxExcludeSpec))
					{
					std::string mask = std::string(theMatch[1]);
					boost::trim(mask);
					Core::FMTMask NEW_MASKS(mask, p_themes);
					Core::FMTSpec newSpec;
					const std::string PERIODS = std::string(theMatch[3]);
					if (!setPeriods(newSpec, PERIODS, p_constants))
						{
						_exhandler->raise(Exception::FMTexc::FMTunboundedperiod
							, " at line " + std::to_string(m_line), "FMTAreaParser::_getExcludedSpec", __LINE__, __FILE__, m_section);
						}
					returned = std::pair<Core::FMTMask, Core::FMTSpec>(NEW_MASKS, newSpec);
					}
				return returned;
				}

			bool FMTAreaParser::_gotNewExclude(const std::vector<Core::FMTTheme>& p_themes,
				const Core::FMTConstants& p_constants,
				const std::string& p_value,
				Core::FMTList<Core::FMTSpec>& p_list) const
			{
				bool gotSomething = false;
				if (_isExclude(p_value))
					{
					gotSomething = true;
					}
				const std::string CLEANEDm_line = _getExcludeValue(p_value);
				//if (!CLEANEDm_line.empty())
					//{
					const std::string TARGET = CLEANEDm_line.empty() ? p_value : CLEANEDm_line;
					const std::pair<Core::FMTMask, Core::FMTSpec> NEW_SPEC = _getExcludedSpec(p_themes, p_constants, TARGET);
					if (!NEW_SPEC.second.empty())
						{
						p_list.push_back(NEW_SPEC.first, NEW_SPEC.second);
						gotSomething = true;
						}
					
					//}
				return gotSomething;
			}


			std::vector<Core::FMTActualDevelopment>FMTAreaParser::read(const std::vector<Core::FMTTheme>& themes, const Core::FMTConstants& constants,const std::string& location)
			{
				std::vector<Core::FMTActualDevelopment>areas;
				try {
					if (!location.empty())
					{
						std::ifstream areastream(location);
						bool potential_futurs = false;
						bool got0area = false;
						size_t futurtype = 0;
						if (FMTParser::tryOpening(areastream, location))
						{
							bool inactualdevs = false;
							boost::unordered_map<Core::FMTDevelopment,size_t>devsindex;
							Core::FMTList<Core::FMTSpec>Excluded;
							std::queue<FMTParser::FMTLineInfo>Lines = FMTParser::getCleanLinewfor(areastream, themes, constants);
							while (!Lines.empty())
							{
								const std::string line = getLine(Lines);
								if (!line.empty())
								{
									if (potential_futurs && inactualdevs && !m_comment.empty() && got0area)
									{
										++futurtype;
										if (futurtype >= (areas.size()*0.5))
										{
											break;
										}
									}
									boost::smatch kmatch;
									if (boost::regex_search(line, kmatch, FMTAreaParser::rxcleanarea))
									{
										std::string masknage = std::string(kmatch[3]) + std::string(kmatch[9]) + std::string(kmatch[18]) + std::string(kmatch[23]);
										std::string mask;
										std::vector<std::string>splitted;
										boost::trim(masknage);
										boost::split(splitted, masknage, boost::is_any_of(FMT_STR_SEPARATOR),boost::algorithm::token_compress_on);
										//splitted = FMTParser::spliter(masknage, FMTParser::m_SEPARATOR);
										const size_t linesize = splitted.size();
										inactualdevs = true;
										for (size_t themeid = 0; themeid < (linesize - 2); ++themeid)
										{
											mask += splitted.at(themeid) + " ";
										}
										mask.pop_back();
										const double area = getNum<double>(splitted.at(linesize - 1), constants);
										if (area > 0)
										{
											got0area = false;
											if (!Core::FMTTheme::validate(themes, mask, " at line " + std::to_string(m_line))) continue;
											potential_futurs = false;
											const int age = getNum<int>(splitted.at(linesize - 2), constants);
											int lock = 0;
											const std::string strlock = std::string(kmatch[6]) + std::string(kmatch[14]);
											if (FMTParser::isValid(strlock))
											{
												lock = getNum<int>(strlock, constants);
											}
											const Core::FMTActualDevelopment actualdevelopment(Core::FMTMask(mask, themes), age, lock, area);
											bool excludeDev = false;
											if (!Excluded.empty())
												{
												std::vector<Core::FMTList<Core::FMTSpec>::const_iterator> SPECIFICATIONS = Excluded.findSets(actualdevelopment.getMask());
												size_t i = 0;
												while (!excludeDev && i < SPECIFICATIONS.size())
													{
													if (SPECIFICATIONS[i]->second.getPeriodLowerBound()<=1)
														{
														excludeDev = true;
														}
													++i;
													}
												}
											if (!excludeDev)
											{
												//Weird non unique area section...
												boost::unordered_map<Core::FMTDevelopment, size_t>::const_iterator hashit = devsindex.find(actualdevelopment);
												if (devsindex.find(actualdevelopment) == devsindex.end())
												{
													devsindex[actualdevelopment] = areas.size();
													//actualdevelopment.passinobject(*this);
													areas.push_back(actualdevelopment);
												}
												else {
													areas[hashit->second].setArea(areas[hashit->second].getArea() + area);
												}
											}
											
										}
										else {
											got0area = true;
										}

									}else if(!_gotNewExclude(themes,constants, line, Excluded))
										{ 
										_exhandler->raise(Exception::FMTexc::FMTinvalid_maskrange
											, line+ " at line " + std::to_string(m_line), "FMTAreaParser::read", __LINE__, __FILE__, m_section);
										}
								}
								else if (!areas.empty() && m_comment.empty())
								{
									potential_futurs = true;
								}
							}
						}
					}
				//areas.shrink_to_fit();
				}catch (...)
					{
					_exhandler->raiseFromCatch("In " + m_location + " at line " + std::to_string(m_line),"FMTAreaParser::read", __LINE__, __FILE__, m_section);
					}
				return areas;
			}
			void FMTAreaParser::write(const std::vector<Core::FMTActualDevelopment>& areas,const std::string& location) const
			{
				try {
					std::ofstream areastream;
					areastream.open(location);
					if (tryOpening(areastream, location))
					{
						const std::string maskstr(areas.at(0).getMask());
						std::vector<std::string>splitted_mask;
						boost::split(splitted_mask, maskstr, boost::is_any_of(" /t"), boost::token_compress_on);
						std::string headerm_line = ";*A ";
						for (size_t theme_id = 1; theme_id <= splitted_mask.size(); ++theme_id)
						{
							headerm_line += "Th" + std::to_string(theme_id) + " ";
						}
						headerm_line += "Age";
						headerm_line += " Area";
						areastream << headerm_line << "\n";
						for (const Core::FMTActualDevelopment& area : areas)
						{
							areastream << std::string(area) << "\n";
						}
						areastream.close();
					}
				}catch (...)
					{
					_exhandler->raiseFromCatch("at "+location,"FMTAreaParser::write", __LINE__, __FILE__, m_section);
					}
			}
}

