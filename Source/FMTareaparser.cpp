/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTareaparser.hpp"
#include "FMTexceptionhandler.hpp"
#include <boost/algorithm/string/join.hpp>
#include <boost/lexical_cast.hpp>
#include "FMToperatingareaclusterbinary.hpp"
#include "FMTlayer.hpp"
#include "FMTforest.hpp"
#include "FMTGCBMtransition.hpp"
#include "FMTconstants.hpp"
#include "FMTtheme.hpp"
#include "FMToperatingareascheme.hpp"
#include "FMToperatingareacluster.hpp"
#include "FMTdevelopment.hpp"
#include "FMTspatialschedule.hpp"
//#include "FMTsasolution.hpp"
#include "FMTactualdevelopment.hpp"
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include "FMTexceptionhandler.hpp"
#include <random>

#ifdef FMTWITHGDAL
	#include "gdal_alg.h"
	#include "gdal_utils.h"
	#include "gdalwarper.h"
	#include "ogrsf_frmts.h"
#endif

namespace Parser{

const boost::regex FMTareaparser::rxcleanarea = boost::regex("^((\\*A[A]*)([^|]*)(_lock)([^0-9]*)([0-9]*))|((\\*A[A]*)([^|]*)([|])([^|]*)([|])([^0-9]*)(.+))|((\\*A[A]*)(([^|]*)([|])([^|]*)([|])))|(\\*A[A]*)(.+)", boost::regex_constants::ECMAScript | boost::regex_constants::icase);

std::vector<std::string> FMTareaparser::splitoaparamlines(std::string line) const
{
	boost::regex xspliter = boost::regex("([\\s\\t,;]*)([^\\s\\t,;]*)");
	boost::smatch kmatch;
	std::string value;
	std::vector<std::string>vecmask;
	int endsize;
	while (!line.empty())
	{
		boost::regex_search(line, kmatch, xspliter);
		value = std::string(kmatch[2]);
		if (boost::regex_match(value, boost::regex("^(?!\\s*$).+")))
		{
			vecmask.push_back(value);
		}
		endsize = (std::string(kmatch[0]).size());
		line.erase(line.begin(), line.begin() + endsize);
	}
	return vecmask;
}

#ifdef FMTWITHGDAL

	std::vector<OGRPolygon*> FMTareaparser::getunion(const std::vector<OGRMultiPolygon>& multipartpolygons) const
		{
		std::vector<OGRPolygon*>mergedpolygons;
		try {
			for (const OGRMultiPolygon& polygons : multipartpolygons)
			{
				OGRGeometry* geometry = polygons.UnionCascaded();
				OGRPolygon* polygon = reinterpret_cast<OGRPolygon*>(geometry);
				mergedpolygons.push_back(polygon);
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch(
				"", "FMTareaparser::getunion", __LINE__, __FILE__, _section);
		}
		return mergedpolygons;
		}

	void FMTareaparser::destroypolygons(std::vector<OGRPolygon*>& polygonstodestroy) const
		{
		try{
		for (OGRPolygon* polygon : polygonstodestroy)
			{
			OGRGeometryFactory::destroyGeometry(polygon);
			}
		polygonstodestroy.clear();
		}
		catch (...)
		{
			_exhandler->raisefromcatch(
				"", "FMTareaparser::destroypolygons", __LINE__, __FILE__, _section);
		}
		}
	/*
    bool FMTareaparser::writesasolution(const std::string location, const Spatial::FMTsasolution& solution,
                                        const std::vector<Core::FMTtheme>& themes, const std::vector<Core::FMTaction>& actions,
                                        const bool& writeevents, int periodstart, int periodstop) const
    {
		try{
        const Spatial::FMTeventcontainer& events = solution.getevents();
        if (periodstart==-1)
        {
            periodstart = events.firstperiod();
        }
        if (periodstop==-1)
        {
            periodstop = events.lastperiod();
        }
        boost::filesystem::path basepath(location);
        boost::filesystem::path agefile("AGE.tif");
        boost::filesystem::path lockfile("LOCK.tif");
        for (int period = periodstart;period<=periodstop;++period)
        {
            boost::filesystem::path new_dir("/Period"+std::to_string(period));
            boost::filesystem::path dirpath = basepath / new_dir;
            if (!boost::filesystem::exists(dirpath))
            {
                boost::filesystem::create_directory(dirpath);
            }
            Spatial::FMTforest forest = solution.getforestperiod(period);
            std::vector<std::string> themespaths;
            themespaths.reserve(themes.size());
            for (size_t i = 1; i <= themes.size();i++)
            {
                boost::filesystem::path fpath("THEME"+std::to_string(i)+".tif");
                boost::filesystem::path filepath = dirpath / fpath;
                themespaths.push_back(fpath.string());
            }
            boost::filesystem::path agepath = dirpath / agefile;
            boost::filesystem::path lockpath = dirpath / lockfile;
            writeforest(forest,themes,themespaths,agepath.string(),lockpath.string());
            if (writeevents)
            {
                for (int aid=0; aid<static_cast<int>(actions.size()); ++aid)
                {
                    Spatial::FMTlayer<int> action_layer(solution.copyextent<int>());//Setting layer information
                    int event_id = 1; //To write in the map
                    std::map<int, std::string> event_map;
                    std::vector<Spatial::FMTeventcontainer::const_iterator> eventsit = events.getevents(period,aid);
                    if (!eventsit.empty())
                    {
                        for(const auto eventit :eventsit)
                        {
                            Spatial::FMTevent event = *eventit;
                            for (std::set<Spatial::FMTcoordinate>::const_iterator coordit = event.elements.begin(); coordit != event.elements.end(); ++coordit)
                            {

                                action_layer[*coordit]=event_id;
                            }
                            event_map[event_id] = "Event_"+ std::to_string(event_id);
                            event_id++;
                        }
                        if (!action_layer.empty())
                        {
                            const std::string action_name = actions.at(aid).getname();
                            boost::filesystem::path filepath(action_name+"_events_period_"+std::to_string(period)+".tif");
                            boost::filesystem::path out_path = dirpath / filepath ;
                            writelayer(action_layer,out_path.string(),event_map);
                        }
                    }
                }
            }
        }
		}
		catch (...)
		{
			_exhandler->raisefromcatch(
				"", "FMTareaparser::writesasolution", __LINE__, __FILE__, _section);
		}
        return true;
    }*/

    void FMTareaparser::validate_raster(const std::vector<std::string>&data_rasters) const
        {
		try {
			int xsize = -1;
			int ysize = -1;
			int rastercount = -1;
			//int overview = -1;
			std::string projection = "";
			for (const std::string& location : data_rasters)
			{
				GDALDataset* data = getdataset(location);
				GDALRasterBand* band = getband(data);
				if (xsize > 0)
				{
					if ((data->GetRasterXSize() != xsize) || (data->GetRasterYSize() != ysize) || (data->GetRasterCount() != rastercount) || (data->GetProjectionRef() != projection) /*|| (band->GetOverviewCount() != overview)*/)
					{
						_exhandler->raise(Exception::FMTexc::FMTinvalidband,
							"Rasters are not the same " + std::string(data->GetDescription()),
							"FMTareaparser::validate_raster", __LINE__, __FILE__, _section);
					}
				}
				else {
					xsize = data->GetRasterXSize();
					ysize = data->GetRasterYSize();
					rastercount = data->GetRasterCount();
					projection = data->GetProjectionRef();
					//overview = band->GetOverviewCount();
				}
				GDALClose(data);
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("","in FMTareaparser::validate_raster", __LINE__, __FILE__,_section);
			}
        }
	std::vector<Core::FMTGCBMtransition> FMTareaparser::getGCBMtransitions(const Spatial::FMTlayer<std::string>& stacked_actions,
																const Spatial::FMTlayer<int>& ages,
																const Spatial::FMTforest& newfor,
																const std::vector<Core::FMTtheme>& themes) const
		{
		std::vector<Core::FMTGCBMtransition>GCBM;
		try{
			std::map<std::string, std::vector<double>>stats;
			std::map<std::string, std::vector<std::map<std::string,int>>>attributes;
			const std::vector<Spatial::FMTlayer<std::string>>newforests = newfor.getthemes(themes);
			for (Spatial::FMTlayer<std::string>::const_iterator itcoord = stacked_actions.begin();
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
					for (const Spatial::FMTlayer<std::string>& nfor : newforests)
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
				for (const Core::FMTtheme& theme : themes)
				{
					const std::string theme_name = "THEME" + std::to_string(theme.getid()+1);
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
				GCBM.push_back(Core::FMTGCBMtransition(ageafter, theme_collection, action_name));
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("","FMTareaparser::getGCBMtransitions", __LINE__, __FILE__, _section);
			}
		return GCBM;
		}



    bool FMTareaparser::writeforest(const Spatial::FMTforest& for_layer,
                         const std::vector<Core::FMTtheme>& themes,
                         const std::vector<std::string>&data_rasters,
                         const std::string& age,
                         const std::string& lock,
						std::vector<std::map<std::string, std::string>>mapping) const
        {
        const std::vector<Spatial::FMTlayer<std::string>> themes_layer = for_layer.getthemes(themes);
		try {
			if (!themes_layer.empty())
			{
				if (mapping.size() != themes.size())
				{
					mapping.clear();
					for (const Core::FMTtheme& theme : themes)
					{
						std::map<std::string, std::string> layer_map;
						for (const std::string& attribute : theme.getbaseattributes())
						{
							layer_map[attribute] = attribute;
						}
						mapping.push_back(layer_map);
					}
				}
				int layer_id = 0;
				for (const std::map<std::string, std::string>& layermap : mapping)
				{
					writelayer<std::string>(themes_layer[layer_id], data_rasters[layer_id], layermap);
					++layer_id;
				}
				const std::map<int, std::string>emptymapping;
				const Spatial::FMTlayer<int>agelayer = for_layer.getage();
				writelayer<int>(agelayer, age, emptymapping);
				const Spatial::FMTlayer<std::string>locklayer = for_layer.getlock();
				std::map<std::string, std::string>lockmap;
				const std::vector<std::string> attributes = locklayer.getattributes();
				for (const std::string& att : attributes)
				{
					lockmap[att] = att;
				}
				if (!locklayer.empty())
				{
					writelayer<std::string>(locklayer, lock, lockmap);
				}
				return true;
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("","FMTareaparser::writeforest", __LINE__, __FILE__, _section);
			}
		return false;
        }


	std::string FMTareaparser::getperiodpathname(const std::string& location, const int& period, const std::string& name) const
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
			_exhandler->raisefromcatch("at " + location, "FMTareaparser::getperiodpathname", __LINE__, __FILE__, _section);
		}
		return full_path.string();
	}

	

	std::vector<Core::FMTGCBMtransition> FMTareaparser::writedisturbances(const std::string& location,
		const Spatial::FMTspatialschedule& disturbances,
		const std::vector<Core::FMTaction>& actions,
		const std::vector<Core::FMTtheme>& themes,
		const int& period) const
	{
		//std::vector<Core::FMTGCBMtransition>GCBM = disturbances.getGCBMtransitions();
		std::vector<Core::FMTGCBMtransition>transitions;
		try
		{
			if (themes.empty())
			{
				Spatial::FMTlayer<std::string> lastdistlayer = disturbances.lastdistlayer(actions, period);
				std::map<std::string, std::string>lmapping;
				for (const auto& act : actions)
				{
					lmapping[act.getname()] = act.getname();
				}
				writelayer<std::string>(lastdistlayer, getperiodpathname(location, period,"DIST"), lmapping);
			}
			else
			{
				Spatial::FMTlayer<std::string> lastdistlayer(disturbances.copyextent<std::string>());
				transitions = disturbances.getGCBMtransitions(lastdistlayer, actions, themes, period);
				if (!lastdistlayer.getmapping().empty())
				{
					std::map<std::string, std::string>lmapping;
					for (const auto& item : lastdistlayer.getmapping())
					{
						lmapping[item.second] = item.second;
					}
					writelayer<std::string>(lastdistlayer, getperiodpathname(location, period,"DIST"), lmapping);
				}
			}
		}
		catch (...)
		{
			_exhandler->printexceptions("at " + location, "FMTareaparser::writedisturbances", __LINE__, __FILE__);
		}
		return transitions;
	}

	std::vector<std::vector<Graph::FMTpredictor>> FMTareaparser::writepredictors(
		const std::string& location,
		const Spatial::FMTspatialschedule& spatialsolution,
		const std::vector<std::string>& yieldnames,
		const Models::FMTmodel& model,
		const int& period,
		bool periodonevalues,
		bool withGCBMid) const
	{
		std::vector<std::vector<Graph::FMTpredictor>>predictors;
		try {
			Spatial::FMTlayer<int> predictorids(spatialsolution.copyextent<int>());
			//transitions = disturbances.getGCBMtransitions(lastdistlayer, actions, themes, period);
			if (!spatialsolution.empty())
			{
				predictors = spatialsolution.getpredictors(predictorids,model, yieldnames, period,periodonevalues, withGCBMid);
				std::map<int, std::string>mapping;
				writelayer<int>(predictorids, getperiodpathname(location, period,"PREDID"), mapping);
			}
		}catch (...)
			{
			_exhandler->printexceptions("at " + location, "FMTareaparser::writepredictors", __LINE__, __FILE__);
			}
		return predictors;
	}


    Spatial::FMTforest FMTareaparser::readrasters(const std::vector<Core::FMTtheme>& themes,
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
			validate_raster(allrasters);
			GDALDataset* agedataset = getdataset(age);
			GDALRasterBand* ageband = getband(agedataset);
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
				lockdataset = getdataset(lock);
				const std::vector<std::string>lockstr = getcat(lockdataset);
				lockatts.reserve(lockstr.size());
				for (const std::string& strlock : lockstr)
				{
					std::vector<std::string>spstr;
					boost::split(spstr, strlock, boost::is_any_of(FMT_STR_SEPARATOR), boost::token_compress_on);
					lockatts.push_back(getnum<int>(spstr[1]));
				}
				lockband = getband(lockdataset);
				lockdata = std::vector<GInt32>(static_cast<size_t>(nXBlockSize) * static_cast<size_t>(nYBlockSize), 0);
			}
			std::vector<GDALDataset*>datasets;
			std::vector<GDALRasterBand*>bands;
			std::vector<std::vector<std::string>>attributes;
			for (const std::string& location : data_rasters)
			{
				GDALDataset* dataset = getdataset(location);
				GDALRasterBand* band = getband(dataset);
				datasets.push_back(dataset);
				bands.push_back(band);
				attributes.push_back(getcat(dataset));
			}
			std::map<Spatial::FMTcoordinate, Core::FMTdevelopment>mapping;
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
							"FMTareaparser::readrasters", __LINE__, __FILE__, _section);
					}
					if (lockdataset != NULL)
					{
						if (CE_None != lockband->ReadBlock(iXBlock, iYBlock, &lockdata[0]))
						{
							_exhandler->raise(Exception::FMTexc::FMTinvalidrasterblock,
								lockdataset->GetDescription(),
								"FMTareaparser::readrasters", __LINE__, __FILE__, _section);
						}
					}
					ageband->GetActualBlockSize(iXBlock, iYBlock, &nXValid, &nYValid);
					std::map<int, std::string>mapattributes;
					boost::unordered_map<int, Spatial::FMTcoordinate>coordinates;
					std::vector<int>counts(static_cast<size_t>(nXBlockSize) * static_cast<size_t>(nYBlockSize), 0);
					for (size_t themeid = 0; themeid < data_rasters.size(); ++themeid)
					{
						if (CE_None != bands[themeid]->ReadBlock(iXBlock, iYBlock, &attributedata[0]))
						{
							_exhandler->raise(Exception::FMTexc::FMTinvalidrasterblock,
								datasets[themeid]->GetDescription(),"FMTareaparser::readrasters", __LINE__, __FILE__, _section);
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
									const std::string attribute = attributes[themeid][intattribute];
									if (mapattributes.find(baselocation) == mapattributes.end())
									{
										mapattributes[baselocation] = "";
									}
									mapattributes[baselocation] += (attribute + " ");
									if (themeid == 0)
									{
										coordinates[baselocation] = Spatial::FMTcoordinate(x, y);
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
						for (std::map<int, std::string>::iterator att = mapattributes.begin(); att != mapattributes.end(); ++att)
						{
							std::string st = att->second;
							const int location = att->first;
							if (counts[location] == attcounts && agedata[location] != nodata)
							{
								std::string maskvalue = st.substr(0, st.size() - 1);
								if (!Core::FMTtheme::validate(themes, maskvalue, " at line " + std::to_string(_line))) continue;
								const Core::FMTmask mask(maskvalue, themes);
								int lock = 0;
								if (!lockdata.empty())
								{
									if (lockdata[location] != nodata)
									{
										lock = lockatts[lockdata[location]];
									}
								}
								Core::FMTdevelopment dev(mask, int(agedata[location] * agefactor), lock);
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
					"FMTareaparser::readrasters", __LINE__, __FILE__, _section);
			}
			const std::string projection = agedataset->GetProjectionRef();
			const unsigned int xsize = ageband->GetXSize();
			const unsigned int ysize = ageband->GetYSize();
			GDALClose(agedataset);
			for (GDALDataset* dataset : datasets)
				{
				GDALClose(dataset);
				}
			if (lockdataset != NULL)
				{
				GDALClose(lockdataset);
				}
			return Spatial::FMTlayer<Core::FMTdevelopment>(mapping, pad, xsize, ysize, projection, cellsize);
		}catch (...)
			{
				_exhandler->printexceptions("", "FMTareaparser::readrasters", __LINE__, __FILE__, _section);
			}



	return Spatial::FMTforest();
	}

	Core::FMTactualdevelopment FMTareaparser::getfeaturetodevelopment(const OGRFeature* feature,
				const std::vector<Core::FMTtheme>& themes,
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
					boost::to_upper(slock);
					slock.erase(0, 5);
					boost::trim(slock);
					if (isvalid(slock))
					{
						lock = getnum<int>(slock);
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
				if (Core::FMTtheme::validate(themes, tmask, " at line " + std::to_string(_line)))
				{
					const Core::FMTmask mask(tmask, themes);
					Core::FMTactualdevelopment newdev(mask, age, lock, area);
					//newdev.passinobject(*this);
					return newdev;
				}
			}
		}catch (...)
			{
			_exhandler->raisefromcatch(std::to_string(feature->GetFID()),
				"FMTareaparser::getfeaturetodevelopment", __LINE__, __FILE__, _section);
			}
		return Core::FMTactualdevelopment();
		}

	GDALDataset* FMTareaparser::openvectorfile(std::map<int, int>&themes_fields,int& age_field,int& lock_field,int& area_field,
		const std::string& data_vectors,const std::string& agefield,const std::string& areafield,const std::string& lockfield,
		const std::vector<Core::FMTtheme>& themes) const
		{
		GDALDataset* dataset=nullptr;
		try {
		//GDALAllRegister();
		dataset = getvectordataset(data_vectors);
		OGRLayer*  layer = getlayer(dataset, 0);
		getWSfields(layer, themes_fields, age_field, area_field, lock_field, agefield, areafield, lockfield);
		if (themes_fields.size() != themes.size())
			{
			_exhandler->raise(Exception::FMTexc::FMTinvalid_maskrange,
				dataset->GetDescription(),"FMTareaparser::openvectorfile", __LINE__, __FILE__, _section);
			}
		layer->ResetReading();
		}catch (...)
			{
			_exhandler->raisefromcatch(data_vectors,"FMTareaparser::openvectorfile", __LINE__, __FILE__, _section);
			}
		return dataset;
		}

	OGRLayer* FMTareaparser::subsetlayer(OGRLayer*layer ,const std::vector<Core::FMTtheme>& themes,
									const std::string& agefield, const std::string& areafield) const
		{
		std::string sqlcall;
		try {
			size_t thid = 1;
			std::vector<std::string>elements;
			for (const Core::FMTtheme& theme : themes)
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
			_exhandler->raisefromcatch("for SQL " + sqlcall,"FMTareaparser::subsetlayer", __LINE__, __FILE__, _section);
			}
		return layer;
		}

	std::vector<Core::FMTactualdevelopment>FMTareaparser::readvectors(const std::vector<Core::FMTtheme>& themes,const std::string& data_vectors,
		const std::string& agefield,const std::string& areafield,double agefactor,double areafactor, std::string lockfield,double minimalarea) const
        {
		std::vector<Core::FMTactualdevelopment>devs;
		try {
			std::map<int, int>themes_fields;
			int age_field = -1;
			int lock_field = -1;
			int area_field = -1;
			GDALDataset* dataset = openvectorfile(themes_fields, age_field, lock_field, area_field, data_vectors, agefield, areafield, lockfield, themes);
			OGRLayer*  layer = getlayer(dataset, 0);
			layer = this->subsetlayer(layer, themes, agefield, areafield);
			OGRFeature *feature;
			while ((feature = layer->GetNextFeature()) != NULL)
			{
				const Core::FMTactualdevelopment actualdev = this->getfeaturetodevelopment(feature, themes, themes_fields, age_field,
					lock_field, area_field, agefactor, areafactor, minimalarea);

				if (!actualdev.getmask().empty())
				{
					std::vector<Core::FMTactualdevelopment>::iterator it = find(devs.begin(), devs.end(), actualdev);
					if (it != devs.end())
					{
						it->setarea(it->getarea() + actualdev.getarea());
					}
					else {
						devs.push_back(actualdev);
					}
				}
				OGRFeature::DestroyFeature(feature);
				++_line;
			}
			GDALClose(dataset);
		}catch (...)
		{
			_exhandler->printexceptions("at " + data_vectors, "FMTareaparser::readvectors", __LINE__, __FILE__, _section);
		}


        return devs;
        }

	void FMTareaparser::vectorfieldtoraster(const std::string& data_vectors,
											const std::string& tifpathandname,
											const int& resolution,
											const std::string& field,
											bool fittoforel) const
	{
		try {
				GDALDataset*  dataset = getvectordataset(data_vectors);
				OGRLayer* layer = getlayer(dataset, 0);
				OGRFeatureDefn* basedefinition = layer->GetLayerDefn();
				const int fieldid = basedefinition->GetFieldIndex(field.c_str());
				if (fieldid==-1)
					{
					_exhandler->raise(Exception::FMTexc::FMTmissingfield, field + " " + layer->GetDescription(),
						"FMTareaparser::vectorfieldtoraster", __LINE__, __FILE__, _section);
					}
				if (basedefinition->GetFieldDefn(fieldid)->GetType() == OGRFieldType::OFTReal)
					{
					_exhandler->raise(Exception::FMTexc::FMTinvalidlayer, std::string(layer->GetDescription())+" with Real format for field "+ field,
						"FMTareaparser::vectorfieldtoraster", __LINE__, __FILE__, _section);
					}
				bool usecategories = true;
				if (basedefinition->GetFieldDefn(fieldid)->GetType() == OGRFieldType::OFTInteger||
					basedefinition->GetFieldDefn(fieldid)->GetType() == OGRFieldType::OFTInteger64)
					{
					usecategories = false;
					}
				OGRCoordinateTransformation* coordtransf = getprojtransform(layer, fittoforel);
				GDALDataset* memds = gettransformmemlayercopy(layer, coordtransf->GetTargetCS(), field);
				OGRLayer* memlayer = getlayer(memds, 0);
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
							"for feature " + std::to_string(_line), "FMTareaparser::vectorfieldtoraster", __LINE__, __FILE__, _section);
					}else {
						OGRFeature* memfeature;
						memfeature = OGRFeature::CreateFeature(memlayerdef);
						memfeature->SetGeometry(geom);
						memfeature->SetField(field.c_str(),newfieldvalue);
						if (memlayer->CreateFeature(memfeature) != OGRERR_NONE)
						{
							_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
								"feature " + std::to_string(_line) + " in memory ", "FMTareaparser::vectorfieldtoraster", __LINE__, __FILE__, _section);
						}
						OGRFeature::DestroyFeature(memfeature);
					}
					OGRFeature::DestroyFeature(feature);
					}
				if (memlayer->GetFeatureCount() <= 0)
				{
					_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
						"No feature were created in the memory layer, check the areafield",
						"FMTareaparser::vectorfieldtoraster", __LINE__, __FILE__, _section);
				}
				OGRCoordinateTransformation::DestroyCT(coordtransf);
				GDALDataset* fieldraster = OGRlayertoRaster(memlayer, field,tifpathandname, resolution, fittoforel);
				GDALClose(memds);
				GDALRasterBand* fieldband = getband(fieldraster);
				if (usecategories)
					{
					std::vector<std::string>categories(values.begin(), values.end());
					setcategories(fieldband, categories);
					}
				fieldband->ComputeStatistics(FALSE, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
				fieldband->FlushCache();
				GDALClose(fieldraster);
		}catch (...)
			{
			_exhandler->printexceptions("at " + data_vectors,
				"FMTareaparser::vectorfieldtoraster", __LINE__, __FILE__, _section);
			}

	}

	Spatial::FMTforest FMTareaparser::vectormaptoFMTforest( const std::string& data_vectors,
															const int& resolution,const std::vector<Core::FMTtheme>& themes,
															const std::string& agefield,const std::string& areafield,double agefactor,
															double areafactor, std::string lockfield,
															double minimalarea, const std::string& writeforestfolder, 
															const bool& fittoforel) const
	{
		Spatial::FMTforest basemap;
		std::vector<Core::FMTactualdevelopment>devs;
		try {
			/*
			The first part open the dataset and get a subset of the layer with only the fields needed. It's also a selection of only the features that have data. 
			*/
			std::map<int, int>themes_fields;
			int age_field = -1;
			int lock_field = -1;
			int area_field = -1;
			GDALDataset* dataset = openvectorfile(themes_fields, age_field, lock_field, area_field, data_vectors, agefield, areafield, lockfield, themes);
			OGRLayer*  layer = getlayer(dataset, 0);
			layer = this->subsetlayer(layer, themes, agefield, areafield);
			OGRCoordinateTransformation* coordtransf = getprojtransform(layer, fittoforel);
			/*OGRwkbGeometryType lgeomtype = layer->GetGeomType();
			if (lgeomtype != wkbMultiPolygon && lgeomtype != wkbPolygon )
			{
				_exhandler->raise(Exception::FMTexc::FMTinvalidlayer,
						"Geometry type from layer is not valid, must be wkbMultiPolygon or wkbPolygon : "+std::to_string(lgeomtype),"FMTareaparser::vectormaptoFMTforest", __LINE__, __FILE__, _section);
			}
			OGRSpatialReference* lspref = layer->GetSpatialRef();
			GDALDataset* memds = createvectormemoryds();
			OGRCoordinateTransformation* coordtransf = nullptr;
			std::unique_ptr<OGRSpatialReference> forelspref = getFORELspatialref();
			bool reproject = false;
			if (fittoforel && !(lspref->IsSame(&*forelspref)))
			{
				coordtransf = OGRCreateCoordinateTransformation(lspref, &*forelspref);
				if (coordtransf == NULL)
				{
					_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
										"Coordinate Transformation","FMTparser::vectormaptoFMTforest", __LINE__, __FILE__, _section);
				}
				reproject=true;
			}*/
			/*
			Here we create a layer in memory and populate the field 'devid' with the id of the development in (devs) 
			*/
			/*OGRLayer* memlayer = nullptr;
			if (reproject)
			{
				memlayer = memds->CreateLayer( "Memlayer", &*forelspref, lgeomtype, NULL );
			}else{
				memlayer = memds->CreateLayer( "Memlayer", lspref, lgeomtype, NULL );
			}
			if (memlayer == NULL)
			{
				 _exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
										"Layer in memory","FMTparser::vectormaptoFMTforest", __LINE__, __FILE__, _section);
			}
			std::string Fieldname = "devid";
			OGRFieldDefn oField( Fieldname.c_str(), OFTInteger );
			if (memlayer->CreateField(&oField) != OGRERR_NONE)
			{
				_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
										"Field definition","FMTparser::vectormaptoFMTforest", __LINE__, __FILE__, _section);
			}*/
			const std::string Fieldname("devid");
			GDALDataset* memds = gettransformmemlayercopy(layer, coordtransf->GetTargetCS(), Fieldname);
			OGRLayer* memlayer = getlayer(memds,0);
			OGRFeatureDefn* memlayerdef = memlayer->GetLayerDefn();
			int devid = 0;
			OGRFeature* feature;
			while ((feature = layer->GetNextFeature()) != NULL)
			{
				const Core::FMTactualdevelopment actualdev = this->getfeaturetodevelopment(feature, themes, themes_fields, age_field,
					lock_field, area_field, agefactor, areafactor, minimalarea);

				if (!actualdev.getmask().empty())
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
										"for feature "+std::to_string(_line),"FMTareaparser::vectormaptoFMTforest", __LINE__, __FILE__, _section);
					}else {
						OGRFeature* memfeature;
						memfeature = OGRFeature::CreateFeature(memlayerdef);
						memfeature->SetGeometry(geom);
						memfeature->SetField(Fieldname.c_str(), devid);
						if (memlayer->CreateFeature(memfeature) != OGRERR_NONE)
						{
							_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
								"feature " + std::to_string(_line) + " in memory ", "FMTareaparser::vectormaptoFMTforest", __LINE__, __FILE__, _section);
						}
						OGRFeature::DestroyFeature(memfeature);
					}
					OGRGeometryFactory::destroyGeometry(geom);

					++devid;
				}
				OGRFeature::DestroyFeature(feature);
				++_line;
			}
			if(memlayer->GetFeatureCount()<=0)
			{
				_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
								"No feature where create in the memory layer, check the areafield : "+areafield+" and the agefield : "+agefield+" because they are used for a subset selection on the layer.",
								"FMTareaparser::vectormaptoFMTforest", __LINE__, __FILE__, _section);
			}
			GDALClose(dataset);
			//if (reproject)
			//{
				OGRCoordinateTransformation::DestroyCT(coordtransf);
			//}
			basemap = getFMTforestfromlayer(memlayer,devs,Fieldname,resolution,areafactor,fittoforel);
			GDALClose(memds);
			if(writeforestfolder!="")
			{
				*_logger<<"Writing FMTforest to : "<<writeforestfolder<<"\n";
				boost::filesystem::path basepath(writeforestfolder);
				boost::filesystem::path agepath = basepath / "AGE.tif";
        		boost::filesystem::path lockpath = basepath / "STANLOCK.tif";
				std::vector<std::string> themespaths;
				themespaths.reserve(themes.size());
            	for (size_t i = 1; i <= themes.size();i++)
				{
					boost::filesystem::path fpath("THEME"+std::to_string(i)+".tif");
					boost::filesystem::path filepath = basepath / fpath;
					themespaths.push_back(filepath.string());
				}
				writeforest(basemap,themes,themespaths,agepath.string(),lockpath.string());
			}
		}catch (...)
		{
			_exhandler->printexceptions("at " + data_vectors, "FMTareaparser::vectormaptoFMTforest", __LINE__, __FILE__, _section);
		}
		return basemap;
	}
	Spatial::FMTforest FMTareaparser::getFMTforestfromlayer(OGRLayer* layer,const std::vector<Core::FMTactualdevelopment>& actualdevs, const std::string& devidfield, const int& resolution, const double& areafactor,const bool& fittoforel) const
	{
		//GDALAllRegister();
		Spatial::FMTforest actualforest;
		try{
			const std::string vsi_path = "/vsimem/"+devidfield+".tif";
			GDALDataset* devidds = OGRlayertoRaster(layer,devidfield,vsi_path,resolution,fittoforel);
			GDALRasterBand* devidband = getband(devidds);
			int nXBlockSize, nYBlockSize;
			devidband->GetBlockSize(&nXBlockSize, &nYBlockSize);
			int nXBlocks = (devidband->GetXSize() + nXBlockSize - 1) / nXBlockSize;
			int nYBlocks = (devidband->GetYSize() + nYBlockSize - 1) / nYBlockSize;
			int nodata = int(devidband->GetNoDataValue());
			std::vector<GInt32>iddata(static_cast<size_t>(nXBlockSize) * static_cast<size_t>(nYBlockSize));
			std::vector<double>pad(6);
			devidds->GetGeoTransform(&pad[0]);
			double cellsize = (abs(pad[1] * pad[5]) * areafactor);
			std::map<Spatial::FMTcoordinate, Core::FMTdevelopment>mapping;
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
							"FMTareaparser::readrasters", __LINE__, __FILE__, _section);
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
								mapping.emplace(Spatial::FMTcoordinate(x,y),actualdevs.at(ldevid));
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
			actualforest = Spatial::FMTforest(Spatial::FMTlayer<Core::FMTdevelopment>(mapping, pad, xsize, ysize, projection, cellsize));
			GDALClose(devidds);
			VSIUnlink(vsi_path.c_str());
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTareaparser::getFMTforestfromlayer", __LINE__, __FILE__, _section);
		}
		return actualforest;
	}

	GDALDataset* FMTareaparser::OGRlayertoRaster(OGRLayer* layer, const std::string& fieldname, const std::string& outfilename, const int& resolution,const bool& fittoforel) const
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
											"Getting the layer extent of "+std::string(layer->GetDescription()),"FMTparser::OGRlayertoRaster", __LINE__, __FILE__, _section);
			}
			if (!layerextent.IsInit())
			{
				_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
											"Layer extent of "+std::string(layer->GetDescription())+" is not Init","FMTparser::OGRlayertoRaster", __LINE__, __FILE__, _section);
			}
			double min_y = layerextent.MinY;
			double min_x = layerextent.MinX;
			const double x_delta = layerextent.MaxX - layerextent.MinX;
			const double y_delta = layerextent.MaxY - layerextent.MinY;
			if (fittoforel)
			{
				std::unique_ptr<OGRSpatialReference> forelref = getFORELspatialref();
				if(layer->GetSpatialRef()->IsSame(&*forelref))
				{
					const double minxforel = -831600;
					const double minyforel = 117980;
					min_x = minxforel+round((layerextent.MinX-minxforel)/resolution)*resolution;
					min_y = minyforel+round((layerextent.MinY-minyforel)/resolution)*resolution;
				}else{
						_exhandler->raise(Exception::FMTexc::FMTinvalidlayer,
											"Layer spatial reference is not ESPG::32198 and fittoforel == True. Layer must be reproject in ESPG::32198 to use the option fitttoforel",
											"FMTparser::OGRlayertoRaster", __LINE__, __FILE__, _section);
				}
			}
	        NXSize = static_cast<int>((x_delta / resolution) * (resolution / 20));
			NYSize = static_cast<int>((y_delta / resolution) * (resolution / 20));
			poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
			if( poDriver == nullptr )
			{
				_exhandler->raise(Exception::FMTexc::FMTinvaliddriver,
					std::string(pszFormat),"FMTparser::OGRlayertoRaster", __LINE__, __FILE__, _section);
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
									"Dataset to : "+outfilename,"FMTparser::OGRlayertoRaster", __LINE__, __FILE__, _section);
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
											"Spatial reference "+std::string(poDstDS->GetDescription()),"FMTparser::OGRlayertoRaster", __LINE__, __FILE__, _section);
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
									"Dataset to : "+outfilename,"FMTparser::OGRlayertoRaster", __LINE__, __FILE__, _section);
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
			_exhandler->raisefromcatch(layer->GetDescription(), "FMTareaparser::OGRlayertoRaster", __LINE__, __FILE__, _section);
		}
		return nDS;
	}
	#ifdef FMTWITHOSI



	std::vector<OGRMultiPolygon>FMTareaparser::getmultipolygons(const std::vector<Heuristics::FMToperatingarea>& operatingareas,
												const std::vector<Core::FMTtheme>& themes, const std::string& data_vectors,
												const std::string& agefield, const std::string& areafield, double agefactor,
												double areafactor, std::string lockfield,
												double minimal_area) const
		{
		std::vector<OGRMultiPolygon>multipolygons(operatingareas.size(), OGRMultiPolygon());
		try {
		std::map<int, int>themes_fields;
		int age_field = -1;
		int lock_field = -1;
		int area_field = -1;
		GDALDataset* dataset = this->openvectorfile(themes_fields, age_field, lock_field, area_field,
			data_vectors, agefield, areafield, lockfield, themes);
		OGRLayer * layer = getlayer(dataset, 0);
		layer = this->subsetlayer(layer, themes, agefield, areafield);
		OGRFeature *feature;
		while ((feature = layer->GetNextFeature()) != NULL)
			{
				const Core::FMTactualdevelopment actualdev = this->getfeaturetodevelopment(feature, themes, themes_fields, age_field,
					lock_field, area_field, agefactor, areafactor, minimal_area);
				if (!actualdev.getmask().empty())
					{
						size_t opid = 0;
						bool foundoaunit = false;
						for (const Heuristics::FMToperatingarea& oparea : operatingareas)
							{
								if (actualdev.getmask().issubsetof(oparea.getmask()))
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
								multipolygons[opid].addGeometry(polygon);
								}
							}
					}
				OGRFeature::DestroyFeature(feature);
			}
		GDALClose(dataset);
		}catch (...)
			{
			_exhandler->raisefromcatch("","FMTareaparser::getmultipolygons", __LINE__, __FILE__, _section);
			}
		return multipolygons;
		}
	#endif


	template<typename T, typename outT>
	void FMTareaparser::writeband(const Spatial::FMTlayer<T>& layer, GDALRasterBand* wband, const std::map<T, std::string>& mapping) const
	{
		double lastwriten;
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
							Spatial::FMTcoordinate coordinate(x, y);
							typename Spatial::FMTlayer<T>::const_iterator it = layer.find(coordinate);
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
						if (wband->WriteBlock(iXBlock, iYBlock, &block[0]) != CPLErr::CE_None)
							{
								_exhandler->raise(Exception::FMTexc::FMTinvalidrasterblock,
								"on band id "+	std::to_string(wband->GetBand()), "FMTareaparser::writeband", __LINE__, __FILE__, _section);
							}
						}
					xstack += nXValid;
					}
				ystack += nYValid;
				}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("last "+ std::to_string(lastwriten) + "at band id " + std::to_string(wband->GetBand()), "FMTareaparser::writelayer", __LINE__, __FILE__, _section);
		}
	}

        template<typename T>
        bool FMTareaparser::writelayer(const Spatial::FMTlayer<T>& layer, std::string location,const std::map<T, std::string>& mapping, std::string format) const
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
				GDALDataset* wdataset = createdataset(location, layer, datatype,format);
				std::vector<std::string>table;
				if (!mapping.empty())
				{
					table.reserve(mapping.size());
					for (typename std::map<T, std::string>::const_iterator it = mapping.begin(); it != mapping.end(); it++)
					{
						table.push_back(it->second);
					}
				}
				GDALRasterBand* wband = createband(wdataset, table);
				if (datatype == GDALDataType::GDT_Byte)
					{
					writeband<T,uint8_t>(layer, wband, mapping);
				}else if (datatype == GDALDataType::GDT_Int32)
					{
					writeband<T,int>(layer, wband, mapping);
				}else {
					writeband<T,double>(layer, wband, mapping);
					}
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
					const double numberofentries = static_cast<double>(table.size() - 1);
					std::default_random_engine generator;
					std::uniform_int_distribution<short>dist(0,256);
					std::vector<short>c1;
					std::vector<short>c2;
					std::vector<short>c3;
					for (size_t tsize = 0 ; tsize < table.size();++tsize)
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
				wband->ComputeStatistics(FALSE, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
				wband->FlushCache();
				wdataset->FlushCache();
				GDALClose(wdataset);
			}catch (...)
				{
				_exhandler->raisefromcatch("at "+location,"FMTareaparser::writelayer", __LINE__, __FILE__, _section);
				}
            return true;
            }


		bool FMTareaparser::writelayer(const Spatial::FMTlayer<double>& layer, std::string location, std::string format) const
		{
			try {
				std::map<double, std::string>naming;
				return writelayer<double>(layer, location, naming, format);
			}
			catch (...)
			{
				_exhandler->raisefromcatch("at " + location, "FMTareaparser::writelayer", __LINE__, __FILE__, _section);
			}
			return false;
		}

			bool FMTareaparser::writeforesttheme(
				const Spatial::FMTforest& for_layer,
				const Core::FMTtheme& theme,
				const std::string& location,
				const std::map<std::string, std::string>& mapping,
				std::string format) const
			{
				try {
					const std::vector<Core::FMTtheme>thetheme(1, theme);
					const std::vector<Spatial::FMTlayer<std::string>> themes_layer = for_layer.getthemes(thetheme);
					return writelayer<std::string>(themes_layer.at(0), location, mapping,format);
				}catch (...)
					{
				_exhandler->raisefromcatch("at " + location, "FMTareaparser::writeforesttheme", __LINE__, __FILE__, _section);
				}
			return false;
			}



	#ifdef FMTWITHOSI
			std::vector<Heuristics::FMToperatingarea> FMTareaparser::getneighborsfrompolygons(const std::vector<OGRPolygon*>& polygons,
																						std::vector<Heuristics::FMToperatingarea> operatingareas,
																						const double& buffersize) const
				{
				try {
					std::map<Core::FMTmask, std::vector<Core::FMTmask>>neighborhood;
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
									const OGRSurface* area = reinterpret_cast<OGRSurface*>(intersect);
									const double intersectarea = area->get_Area();
									fullbuffered += intersectarea;
									neighborsid.push_back(opareaneighborindex);
									areas.push_back(intersectarea);
									OGRGeometryFactory::destroyGeometry(intersect);
								}
							}
							OGRGeometryFactory::destroyGeometry(buffered);
						}

						std::vector<Core::FMTmask>validneighbors;
						for (size_t neighborid = 0; neighborid < neighborsid.size(); ++neighborid)
						{
							if ((areas.at(neighborid) / fullbuffered) >= operatingareas.at(neighborsid.at(neighborid)).getneihgborsperimeter())
							{
								validneighbors.push_back(operatingareas.at(neighborsid.at(neighborid)).getmask());
							}
						}
						neighborhood[operatingareas.at(opareaindex).getmask()] = validneighbors;
					}
					//reciprocity
					/////////////
					for (Heuristics::FMToperatingarea& oparea : operatingareas)
					{
						if (neighborhood.find(oparea.getmask()) != neighborhood.end())
						{
							std::vector<Core::FMTmask>realneighbors;
							for (const Core::FMTmask& nmask : neighborhood.at(oparea.getmask()))
							{
								if (neighborhood.find(nmask) != neighborhood.end() &&
									std::find(neighborhood.at(nmask).begin(), neighborhood.at(nmask).end(), oparea.getmask()) != neighborhood.at(nmask).end())
								{
									realneighbors.push_back(nmask);
								}
							}
							oparea.setneighbors(realneighbors);
						}
					}
				}catch (...)
					{
					_exhandler->raisefromcatch("","FMTareaparser::getneighborsfrompolygons", __LINE__, __FILE__, _section);
					}
				return operatingareas;
				}

			std::vector<Heuristics::FMToperatingareacluster> FMTareaparser::getclustersfrompolygons(const std::vector<OGRPolygon*>& polygons,
																								const std::vector<Heuristics::FMToperatingarea>& operatingareas,
																								const double& maximaldistance) const
			{
				std::vector<Heuristics::FMToperatingareacluster>clusters;
				try {
					if (!(polygons.size()==operatingareas.size()))
						{
						_exhandler->raise(Exception::FMTexc::FMTrangeerror,
							"Invalid number of polygons / operating area",
							"FMTareaparser::getclustersfrompolygons", __LINE__, __FILE__, _section);
						}
					std::map<Core::FMTmask, std::map<Core::FMTmask, double>>distances;
					std::map<Core::FMTmask, std::map<Core::FMTmask, std::set<Core::FMTmask>>>excludedfromlink;
				
					size_t opareaid = 0;
					for (const OGRPolygon* polygon : polygons)
						{
						distances[operatingareas.at(opareaid).getmask()] = std::map<Core::FMTmask, double>();
						excludedfromlink[operatingareas.at(opareaid).getmask()] = std::map<Core::FMTmask, std::set<Core::FMTmask>>();
						++opareaid;
						}

					size_t mainopareaid = 0;
					for (const Heuristics::FMToperatingarea& mainoparea : operatingareas)
						{
						OGRPoint maincentroid;
						const Core::FMTmask mainmask(mainoparea.getmask());
						std::vector<Core::FMTmask>mainneighbors = mainoparea.getneighbors();
						mainneighbors.push_back(mainmask);
						std::sort(mainneighbors.begin(), mainneighbors.end());
						polygons.at(mainopareaid)->Centroid(&maincentroid);
						size_t sideopareaid = 0;
						std::vector<Heuristics::FMToperatingareaclusterbinary>binaries;
						std::vector<Core::FMTmask>binariesmasks;
						std::vector<size_t>polygonids;
						for (const Heuristics::FMToperatingarea& sideoparea : operatingareas)
							{
							double distance;
							const Core::FMTmask sidemask = sideoparea.getmask();

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
								binaries.push_back(Heuristics::FMToperatingareaclusterbinary(sideoparea));
								binariesmasks.push_back(sidemask);
								polygonids.push_back(sideopareaid);
								}
							++sideopareaid;
							}
						std::sort(binariesmasks.begin(), binariesmasks.end());
						size_t binaryid = 0;
						
						std::queue<Core::FMTmask>bannedmask;
						for (Heuristics::FMToperatingareaclusterbinary& binary : binaries)
							{
							const Core::FMTmask binarymask(binary.getmask());
							std::vector<Core::FMTmask>linkerneighbors;
							const OGRPolygon* binary_polygon = polygons.at(polygonids.at(binaryid));
							OGRPoint binarycentroid;
							binary_polygon->Centroid(&binarycentroid);
							OGRLineString linking_line;
							linking_line.setPoint(0, &maincentroid);
							linking_line.setPoint(1, &binarycentroid);
							if (excludedfromlink.at(mainmask).find(binarymask)== excludedfromlink.at(mainmask).end())
								{
								excludedfromlink[mainmask][binarymask] = std::set<Core::FMTmask>();
								excludedfromlink[binarymask][mainmask] = std::set<Core::FMTmask>();
								}
							std::set<Core::FMTmask> * exclusion = &excludedfromlink[mainmask][binarymask];
							size_t subbinaryid = 0;
							for (const Heuristics::FMToperatingareaclusterbinary& subbinary : binaries)
								{
								const Core::FMTmask subbinarymask(subbinary.getmask());
								if (subbinarymask != binarymask &&
									subbinarymask != mainmask&&
									exclusion->find(subbinarymask)==exclusion->end())
								{
									const OGRPolygon* subbinary_polygon = polygons.at(polygonids.at(subbinaryid));
									if (linking_line.Intersects(subbinary_polygon))
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
								std::vector<Core::FMTmask>binaryn = operatingareas.at(polygonids.at(binaryid)).getneighbors();
								binaryn.push_back(binarymask);
								std::vector<Core::FMTmask> intersect;
								std::sort(binaryn.begin(), binaryn.end());
								std::set_intersection(mainneighbors.begin(), mainneighbors.end(),
									binaryn.begin(), binaryn.end(),
									std::back_inserter(intersect));
								bool insertiondone = false;
								if (!intersect.empty())
									{
									for (const Core::FMTmask& imask : intersect)
										{
										if (imask==binarymask||imask==mainmask)
											{
											insertiondone = true;
											break;
											}
										}
									if (!insertiondone)
										{
										std::vector<Core::FMTmask>cleanedintersect;
										std::set_intersection(intersect.begin(), intersect.end(),
											binariesmasks.begin(), binariesmasks.end(),
											std::back_inserter(cleanedintersect));
										for (const Core::FMTmask& imask : cleanedintersect)
											{
											linkerneighbors.push_back(imask);
											insertiondone = true;
											}
										}
								}
								if (!insertiondone)
								{
									const double distance = linking_line.get_Length();
									OGRGeometry* bufferedbinary = binary_polygon->Buffer(distance);
									OGRGeometry* bufferedmain = polygons.at(mainopareaid)->Buffer(distance);
									OGRGeometry* intersection = bufferedbinary->Intersection(bufferedmain);
									size_t subbinaryid = 0;
									for (const Heuristics::FMToperatingareaclusterbinary& subbinary : binaries)
									{
										OGRPolygon* subbinary_polygon = polygons.at(polygonids.at(subbinaryid));
										const Core::FMTmask subbinarymask(subbinary.getmask());
										if (subbinarymask != binarymask && subbinarymask != mainmask && intersection->Intersects(subbinary_polygon))
										{
											OGRPolygon* subintersection = reinterpret_cast<OGRPolygon*>(intersection->Intersection(subbinary_polygon));
											if (subintersection->get_Area() >= subbinary_polygon->get_Area()*0.5)
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
							binary.setneighbors(linkerneighbors);
							++binaryid;
							}
						
						while (!bannedmask.empty())
							{
							std::vector<Heuristics::FMToperatingareaclusterbinary>cleanedbinaries;
							const Core::FMTmask& mask = bannedmask.front();
							for (const Heuristics::FMToperatingareaclusterbinary& binary : binaries)
								{
								const Core::FMTmask binmask = binary.getmask();
								if (mask != binmask)
									{
									bool keepit = true;
									for (const Core::FMTmask& nmask : binary.getneighbors())
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

						Heuristics::FMToperatingareaclusterbinary basecentroid(mainoparea);
						basecentroid.setneighbors(std::vector<Core::FMTmask>());
						clusters.push_back(Heuristics::FMToperatingareacluster(basecentroid,binaries));
						++mainopareaid;
						}


				}catch (...)
					{
					_exhandler->raisefromcatch("", "FMTareaparser::getclustersfrompolygons", __LINE__, __FILE__, _section);
					}
				return clusters;
			}


			std::vector<Heuristics::FMToperatingareascheme> FMTareaparser::getschemeneighbors(std::vector<Heuristics::FMToperatingareascheme> operatingareaparameters,
																			const std::vector<Core::FMTtheme>& themes, const std::string& data_vectors,
																			const std::string& agefield, const std::string& areafield, double agefactor,
																			double areafactor, std::string lockfield,
																			double minimal_area , double buffersize) const
				{
				try {
					const std::vector<Heuristics::FMToperatingarea>baseoparea(operatingareaparameters.begin(), operatingareaparameters.end());
					std::vector<OGRMultiPolygon>multipolygons = this->getmultipolygons(baseoparea, themes, data_vectors,
						agefield, areafield, agefactor,
						areafactor, lockfield, minimal_area);
					std::vector<OGRPolygon*>mergedpolygons = this->getunion(multipolygons);
					const std::vector<Heuristics::FMToperatingarea>schemes = getneighborsfrompolygons(mergedpolygons, baseoparea, buffersize);
					this->destroypolygons(mergedpolygons);
					size_t opareaid = 0;
					for (const Heuristics::FMToperatingarea& oparea : schemes)
						{
						operatingareaparameters[opareaid].setneighbors(oparea.getneighbors());
						++opareaid;
						}
				}catch (...)
				{
					_exhandler->printexceptions("", "FMTareaparser::getschemeneighbors", __LINE__, __FILE__, _section);
				}

				return operatingareaparameters;
				}

			std::vector<Heuristics::FMToperatingareacluster> FMTareaparser::getclusters(const std::vector<Heuristics::FMToperatingarea>& operatingareas,
				const std::vector<Core::FMTtheme>& themes, const std::string& data_vectors,
				const std::string& agefield, const std::string& areafield, const double& maximaldistance,
                double agefactor,
				double areafactor, std::string lockfield,
				double minimal_area, double buffersize) const
			{
				std::vector<Heuristics::FMToperatingareacluster>finalclusters;
				try {
					std::vector<OGRMultiPolygon>multipolygons = this->getmultipolygons(operatingareas, themes, data_vectors,
						agefield, areafield, agefactor,
						areafactor, lockfield, minimal_area);
					std::vector<OGRPolygon*>mergedpolygons = this->getunion(multipolygons);
					std::vector<Heuristics::FMToperatingarea>newopareas(operatingareas.begin(), operatingareas.end());
					const std::vector<Heuristics::FMToperatingarea>opareawithneighbors = getneighborsfrompolygons(mergedpolygons, newopareas, buffersize);
					finalclusters = this->getclustersfrompolygons(mergedpolygons, opareawithneighbors, maximaldistance);
					this->destroypolygons(mergedpolygons);
				}catch (...)
				{
					_exhandler->printexceptions("", "FMTareaparser::getclusters", __LINE__, __FILE__, _section);
				}
				return finalclusters;
			}
			
		#ifdef FMTWITHGDAL
			void FMTareaparser::writeOAschedulerparameters(const std::string& location, const std::vector<Heuristics::FMToperatingareascheme>& OAschemes,
															std::vector<std::string> creationoptions) const
			{
				try {
					const boost::filesystem::path path(location);
					GDALDataset* new_dataset = createOGRdataset(path.parent_path().string());
					OGRLayer* newlayer = createlayer(new_dataset, path.stem().string(), creationoptions);
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
							"Cannote create new fields outputsdrift", "FMTareaparser::writeOAschedulerparameters", __LINE__, __FILE__, _section);
					}
					for (const Heuristics::FMToperatingareascheme& scheme : OAschemes)
						{
						OGRFeature* newfeature = OGRFeature::CreateFeature(newlayer->GetLayerDefn());
						if (newfeature == NULL)
						{
							_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
								"Cannote generate new feature ", "FMTareaparser::writeOAschedulerparameters", __LINE__, __FILE__, _section);
							//Failed to generate feature
						}
						newfeature->SetField("OA",std::string(scheme.getmask()).c_str());
						newfeature->SetField("OPT", static_cast<int>(scheme.getopeningtime()));
						newfeature->SetField("RET", static_cast<int>(scheme.getminimalreturntime()));
						newfeature->SetField("MAXRET", static_cast<int>(scheme.getmaximalreturntime()));
						newfeature->SetField("REP", static_cast<int>(scheme.getrepetition()));
						newfeature->SetField("OPR", static_cast<int>(scheme.getthreshold()));
						newfeature->SetField("NEP", static_cast<double>(scheme.getneihgborsperimeter()));
						newfeature->SetField("GUP", static_cast<int>(scheme.getgreenup()));
						if (newlayer->CreateFeature(newfeature) != OGRERR_NONE)
						{
							_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
								"Cannote create new feature id " + std::to_string(newlayer->GetFeatureCount()), "FMTareaparser::writeOAschedulerparameters", __LINE__, __FILE__, _section);
							//Failed to generate feature
						}
						OGRFeature::DestroyFeature(newfeature);

						}

				}
				catch (...)
				{
					_exhandler->printexceptions("", "FMTareaparser::writeOAschedulerparameters", __LINE__, __FILE__);
				}
			}
		#endif


			std::vector<Heuristics::FMToperatingareascheme> FMTareaparser::readOAschedulerparameters(const std::string& location, const std::vector<Core::FMTtheme>& modelthemes, const int& themetarget,const int& startingperiod) const
			{
				std::vector<Heuristics::FMToperatingareascheme> schemes;
				try{
					std::ifstream oaparameterfile(location);
					std::vector<std::string> titles = { "OA","OPT","RET","MAXRET","REP"};
					if (tryopening(oaparameterfile, location))
					{
						int lineid = 0;
						std::string line;
						std::map<std::string, int>columns;
						bool useRETasMAXRET = true;
						while (getline(oaparameterfile, line))
						{
							std::vector<std::string>splittedline = splitoaparamlines(line);
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
													"FMTareaparser::readOAschedulerparameters", __LINE__, __FILE__);
											
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
									const Core::FMTmask OAmask(smask,modelthemes);
									for (std::string title : titles)
									{
										if (columns.find(title) != columns.end() && int(splittedline.size()) <= columns[title])
										{
											_exhandler->raise(Exception::FMTexc::FMTmissingfield,
												"Missing required column " + title + " for " + OA + " in parameters file",
												"FMTareaparser::readOAschedulerparameters", __LINE__, __FILE__);
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
										if (int(splittedline.size()) >= 6)
										{
											if (!splittedline[columns["OPR"]].empty())
											{
												opr = std::stod(splittedline[columns["OPR"]]);
												if (!(opr <= 1.0 && opr >= 0.0))
												{
													_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
														" for " + OA + " opr not <= 1 and >= 0 in parameters file!",
														"FMTareaparser::readOAschedulerparameters", __LINE__, __FILE__);
												}
											}
										}
									}
									if (!(opt >= 1))
									{
										_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
											" for " + OA + " opt not >= 1 in parameters file!",
											"FMTareaparser::readOAschedulerparameters", __LINE__, __FILE__);
									}
									if (!(rep >= 1))
									{
										_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
											" for " + OA + " rep not >= 1 in parameters file!",
											"FMTareaparser::readOAschedulerparameters", __LINE__, __FILE__);
									}
									double gup = 0;
									double npe = 0;
									if (columns.find("NPE") != columns.end())
									{
										if (int(splittedline.size()) >= 6)
										{
											if (!splittedline[columns["NPE"]].empty())
											{
												npe = std::stod(splittedline[columns["NPE"]]);
												if (!(npe <= 1.0 && npe >= 0.0))
												{
													_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
														" for " + OA + " npe not <= 1 and >= 0 in parameters file!",
														"FMTareaparser::readOAschedulerparameters", __LINE__, __FILE__);
												}
											}
										}
									}
									if (columns.find("GUP") != columns.end())
									{
										if (int(splittedline.size()) >= 7)
										{
											if (!splittedline[columns["GUP"]].empty())
											{
												gup = std::stoi(splittedline[columns["GUP"]]);
												if (!(gup >= 0))
												{
													_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
														" for " + OA + " gup >= 0 in parameters file!",
														"FMTareaparser::readOAschedulerparameters", __LINE__, __FILE__);
												}
											}
										}
									}
									schemes.push_back(Heuristics::FMToperatingareascheme(Heuristics::FMToperatingarea(OAmask, npe),opt,ret,maxret,rep,gup,startingperiod,opr));
								}
							}
							++lineid;
						}
						oaparameterfile.close();
					}
				}
				catch (...)
				{
					_exhandler->printexceptions("", "FMTareaparser::readOAschedulerparameters", __LINE__, __FILE__);
				}
				return schemes;
			}


		#endif
#endif
			FMTareaparser::FMTareaparser() :
				FMTparser()
			{
				_section = Core::FMTsection::Area;
			}


			std::vector<Core::FMTactualdevelopment>FMTareaparser::read(const std::vector<Core::FMTtheme>& themes, const Core::FMTconstants& constants,const std::string& location)
			{
				std::vector<Core::FMTactualdevelopment>areas;
				try {
					if (!location.empty())
					{
						std::ifstream areastream(location);
						bool potential_futurs = false;
						bool got0area = false;
						size_t futurtype = 0;
						if (FMTparser::tryopening(areastream, location))
						{
							bool inactualdevs = false;
							boost::unordered_map<Core::FMTdevelopment,size_t>devsindex;
							while (areastream.is_open())
							{
								const std::string line = FMTparser::getcleanlinewfor(areastream, themes, constants);
								if (!line.empty())
								{
									if (potential_futurs && inactualdevs && !_comment.empty() && got0area)
									{
										++futurtype;
										if (futurtype >= (areas.size()*0.5))
										{
											break;
										}
									}
									boost::smatch kmatch;
									if (boost::regex_search(line, kmatch, FMTareaparser::rxcleanarea))
									{
										std::string masknage = std::string(kmatch[3]) + std::string(kmatch[9]) + std::string(kmatch[18]) + std::string(kmatch[23]);
										std::string mask;
										std::vector<std::string>splitted;
										boost::trim(masknage);
										boost::split(splitted, masknage, boost::is_any_of(FMT_STR_SEPARATOR),boost::algorithm::token_compress_on);
										//splitted = FMTparser::spliter(masknage, FMTparser::rxseparator);
										const size_t linesize = splitted.size();
										inactualdevs = true;
										for (size_t themeid = 0; themeid < (linesize - 2); ++themeid)
										{
											mask += splitted.at(themeid) + " ";
										}
										mask.pop_back();
										const double area = getnum<double>(splitted.at(linesize - 1), constants);
										if (area > 0)
										{
											got0area = false;
											if (!Core::FMTtheme::validate(themes, mask, " at line " + std::to_string(_line))) continue;
											potential_futurs = false;
											const int age = getnum<int>(splitted.at(linesize - 2), constants);
											int lock = 0;
											const std::string strlock = std::string(kmatch[6]) + std::string(kmatch[14]);
											if (FMTparser::isvalid(strlock))
											{
												lock = getnum<int>(strlock, constants);
											}
											const Core::FMTactualdevelopment actualdevelopment(Core::FMTmask(mask, themes), age, lock, area);
											//Weird non unique area section...
											boost::unordered_map<Core::FMTdevelopment, size_t>::const_iterator hashit = devsindex.find(actualdevelopment);
											if (devsindex.find(actualdevelopment) == devsindex.end())
											{
												devsindex[actualdevelopment] = areas.size();
												//actualdevelopment.passinobject(*this);
												areas.push_back(actualdevelopment);
											}
											else {
												areas[hashit->second].setarea(areas[hashit->second].getarea() + area);
											}
										}
										else {
											got0area = true;
										}

									}else {
										_exhandler->raise(Exception::FMTexc::FMTinvalid_maskrange
											, " at line " + std::to_string(_line),"FMTareaparser::read", __LINE__, __FILE__, _section);
										}
								}
								else if (!areas.empty() && _comment.empty())
								{
									potential_futurs = true;
								}
							}
						}
					}
				areas.shrink_to_fit();
				}catch (...)
					{
					_exhandler->raisefromcatch("In " + _location + " at line " + std::to_string(_line),"FMTareaparser::read", __LINE__, __FILE__, _section);
					}
				return areas;
			}
			void FMTareaparser::write(const std::vector<Core::FMTactualdevelopment>& areas,const std::string& location) const
			{
				try {
					std::ofstream areastream;
					areastream.open(location);
					if (tryopening(areastream, location))
					{
						const std::string maskstr(areas.at(0).getmask());
						std::vector<std::string>splitted_mask;
						boost::split(splitted_mask, maskstr, boost::is_any_of(" /t"), boost::token_compress_on);
						std::string header_line = ";*A ";
						for (size_t theme_id = 1; theme_id <= splitted_mask.size(); ++theme_id)
						{
							header_line += "TH" + std::to_string(theme_id) + " ";
						}
						header_line += "AGE";
						header_line += " AREA";
						areastream << header_line << "\n";
						for (const Core::FMTactualdevelopment& area : areas)
						{
							areastream << std::string(area) << "\n";
						}
						areastream.close();
					}
				}catch (...)
					{
					_exhandler->raisefromcatch("at "+location,"FMTareaparser::write", __LINE__, __FILE__, _section);
					}
			}
}

