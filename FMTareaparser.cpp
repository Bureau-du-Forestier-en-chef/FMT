/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "FMTareaparser.h"
#include <boost/algorithm/string/join.hpp>
#include <boost/lexical_cast.hpp>

namespace Parser{

#ifdef FMTWITHGDAL

    void FMTareaparser::validate_raster(const std::vector<std::string>&data_rasters) const
        {
        int xsize = -1;
        int ysize = -1;
        int rastercount = -1;
        int overview = -1;
		std::string projection = "";
        for(const std::string& location : data_rasters)
            {
            GDALDataset* data = getdataset(location);
            GDALRasterBand* band = getband(data);
            if (xsize>0)
                {
                if ((data->GetRasterXSize()!=xsize) || (data->GetRasterYSize()!=ysize) || (data->GetRasterCount()!=rastercount) || (data->GetProjectionRef()!=projection) || (band->GetOverviewCount()!=overview))
                    {
                    _exhandler->raise(Exception::FMTexc::FMTinvalidband,_section,"Rasters are not the same "+ std::string(data->GetDescription()), __LINE__, __FILE__);
                    }
                }else{
                xsize=data->GetRasterXSize();
                ysize=data->GetRasterYSize();
                rastercount=data->GetRasterCount();
                projection=data->GetProjectionRef();
                overview = band->GetOverviewCount();
                }
            GDALClose(data);
            }
        }
	std::vector<Core::FMTGCBMtransition> FMTareaparser::getGCBMtransitions(const Spatial::FMTlayer<std::string>& stacked_actions,
																const Spatial::FMTlayer<int>& ages,
																const Spatial::FMTforest& newfor,
																const std::vector<Core::FMTtheme>& themes) const
		{
		std::map<std::string, std::vector<double>>stats;
		std::map<std::string, std::vector<std::map<std::string,int>>>attributes;
		const std::vector<Spatial::FMTlayer<std::string>>newforests = newfor.getthemes(themes);
		for (std::map<Spatial::FMTcoordinate, std::string>::const_iterator itcoord = stacked_actions.mapping.begin();
			itcoord != stacked_actions.mapping.end(); itcoord++)
		{
			if (ages.mapping.find(itcoord->first) != ages.mapping.end())
			{
				if (stats.find(itcoord->second) == stats.end())
				{
					stats[itcoord->second] = std::vector<double>(2, 0);
				}
				stats[itcoord->second][0] += ages.mapping.at(itcoord->first);
				++stats[itcoord->second][1];

				if (attributes.find(itcoord->second) == attributes.end())
					{
					attributes[itcoord->second] = std::vector<std::map<std::string, int>>(newforests.size());
					}
				int tid = 0;
				for (const Spatial::FMTlayer<std::string>& nfor : newforests)
					{
					const std::string value = nfor.mapping.at(itcoord->first);
					if (attributes.at(itcoord->second).at(tid).find(value) == attributes.at(itcoord->second).at(tid).end())
						{
						attributes[itcoord->second][tid][value] = 0;
						}
					attributes[itcoord->second][tid][value] += 1;
					++tid;
					}
			}
		}

		std::vector<Core::FMTGCBMtransition>GCBM;
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
		if (!themes_layer.empty())
			{
			if (mapping.size() != themes.size())
			{
				mapping.clear();
				for (const Core::FMTtheme& theme : themes)
				{
					const std::map<std::string, std::string> themes_mapping = theme.getvaluenames();
					std::map<std::string, std::string> layer_map;
					for (std::map<std::string, std::string>::const_iterator dit = themes_mapping.begin(); dit != themes_mapping.end(); dit++)
					{
						layer_map[dit->first] = dit->first;
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
			writelayer<std::string>(locklayer, lock, lockmap);
			return true;
			}
		return false;
        }

	std::string FMTareaparser::getdisturbancepath(const std::string& location, const int& period) const
		{
		const boost::filesystem::path dir(location);
		const std::string layername = "DIST_" + std::to_string(period) + ".tif";
		const boost::filesystem::path file(layername);
		const boost::filesystem::path full_path = dir / file;
		return full_path.string();
		}

	std::vector<Core::FMTGCBMtransition> FMTareaparser::writedisturbances(const std::string& location,
		const Spatial::FMTdisturbancestack& disturbances,
		const Spatial::FMTforest& for_layer,
		const Spatial::FMTforest& out_layer,
		const std::vector<Core::FMTtheme>& themes,
		std::map<std::string, std::string> mapping) const
	{
		std::vector<Spatial::FMTlayer<std::string>> distvec = disturbances.getlayers<Core::FMTdevelopment>(for_layer);
		std::vector<Core::FMTGCBMtransition>transitions;
		if (!distvec.empty())
		{
			if (mapping.empty())
			{
				mapping = disturbances.directmapping();
			}

			if (!themes.empty())
			{
				const std::vector<Spatial::FMTlayer<std::string>>forest_attributes = for_layer.getthemes(themes);
				const Spatial::FMTlayer<int>ages = out_layer.getage();
				const int period = static_cast<int>(distvec.size());
				int themeid = 0;
				for (const Spatial::FMTlayer<std::string>& attlayer : forest_attributes)
				{
					std::map<std::string,std::string>newmap;
					const std::map<std::string,std::string>theme_names = themes[themeid].getvaluenames();
					for (std::map<std::string, std::string>::const_iterator it = mapping.begin(); it != mapping.end(); it++)
					{
						for (std::map<std::string, std::string>::const_iterator the_it = theme_names.begin(); the_it != theme_names.end(); the_it++)
						{
							newmap[it->first + "-" + the_it->first] = it->first + "-" + the_it->first;
						}
					}
					mapping = newmap;
					distvec.back() += attlayer;
					++themeid;
				}
				transitions = getGCBMtransitions(distvec.back(), ages,out_layer,themes);
				writelayer<std::string>(distvec.back(), getdisturbancepath(location, period), mapping);
			}
			else {
				int period = 1;
				for (const Spatial::FMTlayer<std::string>& layer : distvec)
				{
					writelayer<std::string>(layer, getdisturbancepath(location, period), mapping);
					++period;
				}
			}
		}
		return transitions;
	}

    Spatial::FMTforest FMTareaparser::readrasters(const std::vector<Core::FMTtheme>& themes,
                                             const std::vector<std::string>&data_rasters,
                                             const std::string& age,double agefactor,
                                             double areafactor,std::string lock) const
        {
        GDALAllRegister();
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
        ageband->GetBlockSize(&nXBlockSize,&nYBlockSize);
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
        double cellsize = (abs(pad[1]*pad[5]) * areafactor);
		std::vector<int>lockatts;
        if(!lock.empty())
            {
            lockdataset = getdataset(lock);
			const std::vector<std::string>lockstr = getcat(lockdataset);
            lockatts.reserve(lockstr.size());
            for (const std::string& strlock : lockstr)
                {
				std::vector<std::string>spstr;
                boost::split(spstr,strlock,boost::is_any_of(FMT_STR_SEPARATOR), boost::token_compress_on);
                lockatts.push_back(getnum<int>(spstr[1]));
                }
            lockband = getband(lockdataset);
            lockdata = std::vector<GInt32>(static_cast<size_t>(nXBlockSize) * static_cast<size_t>(nYBlockSize),0);
            }
        std::vector<GDALDataset*>datasets;
		std::vector<GDALRasterBand*>bands;
		std::vector<std::vector<std::string>>attributes;
        for(const std::string& location : data_rasters)
            {
            GDALDataset* dataset = getdataset(location);
            GDALRasterBand* band = getband(dataset);
            datasets.push_back(dataset);
            bands.push_back(band);
            attributes.push_back(getcat(dataset));
            }
		std::map<Spatial::FMTcoordinate,Core::FMTdevelopment>mapping;
        int missing = 0;
        unsigned int ystack = 0;
        for( int iYBlock = 0; iYBlock < nYBlocks; iYBlock++ )
            {
            unsigned int xstack = 0;
            int nYValid = 0;
            for( int iXBlock = 0; iXBlock < nXBlocks; iXBlock++ )
                {
                int  nXValid;
                if (CE_None!=ageband->ReadBlock( iXBlock, iYBlock,&agedata[0]))
                    {
                     _exhandler->raise(Exception::FMTexc::FMTinvalidrasterblock,_section,agedataset->GetDescription(), __LINE__, __FILE__);
                    }
                if (lockdataset!= NULL)
                    {
                    if (CE_None!=lockband->ReadBlock( iXBlock, iYBlock, &lockdata[0]))
                        {
                        _exhandler->raise(Exception::FMTexc::FMTinvalidrasterblock,_section,lockdataset->GetDescription(), __LINE__, __FILE__);
                        }
                    }
                ageband->GetActualBlockSize(iXBlock, iYBlock, &nXValid, &nYValid);
				std::map<int, std::string>mapattributes;
                boost::unordered_map<int,Spatial::FMTcoordinate>coordinates;
				std::vector<int>counts(static_cast<size_t>(nXBlockSize) * static_cast<size_t>(nYBlockSize),0);
                for(size_t themeid = 0 ; themeid < data_rasters.size();++themeid)
                    {
                    if (CE_None!=bands[themeid]->ReadBlock( iXBlock, iYBlock, &attributedata[0]))
                        {
                        _exhandler->raise(Exception::FMTexc::FMTinvalidrasterblock,_section,datasets[themeid]->GetDescription(), __LINE__, __FILE__);
                        }
                    unsigned int y = ystack;
                    for( int iY = 0; iY < nYValid; iY++ )
                        {
                        unsigned int x = xstack;
                        for( int iX = 0; iX < nXValid; iX++ )
                            {
                            const unsigned int baselocation = (iX + iY * nXBlockSize);
                            int intattribute = attributedata[baselocation];
                            if (intattribute != nodata)
                                {
								const std::string attribute = attributes[themeid][intattribute];
                                if (mapattributes.find(baselocation)==mapattributes.end())
                                    {
                                    mapattributes[baselocation]="";
                                    }
                                mapattributes[baselocation]+=(attribute+" ");
                                if(themeid==0)
                                    {
                                    coordinates[baselocation]=Spatial::FMTcoordinate(x,y);
                                    }
                                counts[baselocation]+=1;
                                }
                            ++x;
                            }
                        ++y;
                        }
                    }
				    const size_t attcounts = themes.size();
                    if (!mapattributes.empty())
                        {
                        for (std::map<int, std::string>::iterator att=mapattributes.begin(); att!=mapattributes.end(); ++att)
                            {
							std::string st = att->second;
                            const int location = att->first;
                            if (counts[location]==attcounts && agedata[location] != nodata)
                                {
								std::string maskvalue = st.substr(0, st.size()-1);
                                if (!validate(themes,maskvalue," at line " + std::to_string(_line))) continue;
                                const Core::FMTmask mask(maskvalue,themes);
                                int lock = 0;
                                if (!lockdata.empty())
                                    {
                                    if(lockdata[location] != nodata)
                                        {
                                        lock = lockatts[lockdata[location]];
                                        }
                                    }
                                const Core::FMTdevelopment dev(mask,int(agedata[location]*agefactor),lock);
                                mapping[coordinates[location]] = dev;
                                }else{
                                ++missing;
                                }
                            }
                        }
                    xstack+=nXValid;
                    }
                ystack+=nYValid;
                 }
             if(missing>0)
                {
                const std::string message = " for "+std::to_string(missing)+" raster cells";
                _exhandler->raise(Exception::FMTexc::FMTmissingrasterattribute,_section,message, __LINE__, __FILE__);
                }
             const std::string projection = agedataset->GetProjectionRef();
             const unsigned int xsize = ageband->GetXSize();
             const unsigned int ysize = ageband->GetYSize();
             return Spatial::FMTlayer<Core::FMTdevelopment>(mapping,pad,xsize,ysize,projection,cellsize);
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
				if (validate(themes, tmask, " at line " + std::to_string(_line)))
					{
					const Core::FMTmask mask(tmask, themes);
					return Core::FMTactualdevelopment(mask, age, lock, area);
					}
			}
		return Core::FMTactualdevelopment();
		}

	GDALDataset* FMTareaparser::openvectorfile(std::map<int, int>&themes_fields,int& age_field,int& lock_field,int& area_field,
		const std::string& data_vectors,const std::string& agefield,const std::string& areafield,const std::string& lockfield,
		const std::vector<Core::FMTtheme>& themes) const
		{
		GDALAllRegister();
		GDALDataset* dataset = getvectordataset(data_vectors);
		OGRLayer*  layer = getlayer(dataset, 0);
		getWSfields(layer, themes_fields, age_field, area_field, lock_field, agefield, areafield, lockfield);
		if (themes_fields.size() != themes.size())
			{
			_exhandler->raise(Exception::FMTexc::WSinvalid_maskrange, _section, dataset->GetDescription(), __LINE__, __FILE__);
			}
		layer->ResetReading();
		return dataset;
		}

	OGRLayer* FMTareaparser::subsetlayer(OGRLayer*layer ,const std::vector<Core::FMTtheme>& themes,
									const std::string& agefield, const std::string& areafield) const
		{
		size_t thid = 1;
		std::vector<std::string>elements;
		for (const Core::FMTtheme& theme: themes)
			{
			elements.push_back("THEME"+std::to_string(thid));
			++thid;
			}
		elements.push_back(agefield);
		elements.push_back(areafield);
		std::string sqlcall = boost::algorithm::join(elements, " IS NOT NULL AND ");
		sqlcall += " IS NOT NULL";
		layer->SetAttributeFilter(sqlcall.c_str());
		return layer;
		}

	std::vector<Core::FMTactualdevelopment>FMTareaparser::readvectors(const std::vector<Core::FMTtheme>& themes,const std::string& data_vectors,
		const std::string& agefield,const std::string& areafield,double agefactor,double areafactor, std::string lockfield,double minimalarea) const
        {
		std::map<int, int>themes_fields;
		int age_field = -1;
		int lock_field = -1;
		int area_field = -1;
		GDALDataset* dataset =  openvectorfile(themes_fields,age_field,lock_field,area_field,data_vectors,agefield,areafield,lockfield,themes);
		OGRLayer*  layer = getlayer(dataset, 0);
		layer = this->subsetlayer(layer, themes, agefield, areafield);
		OGRFeature *feature;
		std::vector<Core::FMTactualdevelopment>devs;
        while((feature = layer->GetNextFeature()) != NULL)
            {
				const Core::FMTactualdevelopment actualdev = this->getfeaturetodevelopment(feature, themes, themes_fields, age_field,
					lock_field, area_field, agefactor, areafactor, minimalarea);

				if (!actualdev.mask.empty())
					{
					std::vector<Core::FMTactualdevelopment>::iterator it = find(devs.begin(), devs.end(), actualdev);
					if (it != devs.end())
					{
						it->setarea(it->getarea()+ actualdev.getarea());
					}
					else {
						devs.push_back(actualdev);
					}
					}
            OGRFeature::DestroyFeature(feature);
            }
		GDALClose(dataset);
        return devs;
        }
	#ifdef FMTWITHOSI
	std::vector<OGRMultiPolygon>FMTareaparser::getmultipolygons(const std::vector<Heuristics::FMToperatingarea>& operatingareas,
												const std::vector<Core::FMTtheme>& themes, const std::string& data_vectors,
												const std::string& agefield, const std::string& areafield, double agefactor,
												double areafactor, std::string lockfield,
												double minimal_area) const
		{
		std::map<int, int>themes_fields;
		int age_field = -1;
		int lock_field = -1;
		int area_field = -1;
		GDALDataset* dataset = this->openvectorfile(themes_fields, age_field, lock_field, area_field,
			data_vectors, agefield, areafield, lockfield, themes);
		OGRLayer * layer = getlayer(dataset, 0);
		layer = this->subsetlayer(layer, themes, agefield, areafield);

		OGRFeature *feature;
		std::vector<OGRMultiPolygon>multipolygons(operatingareas.size(),OGRMultiPolygon());
		while ((feature = layer->GetNextFeature()) != NULL)
			{
				const Core::FMTactualdevelopment actualdev = this->getfeaturetodevelopment(feature, themes, themes_fields, age_field,
					lock_field, area_field, agefactor, areafactor, minimal_area);
				if (!actualdev.mask.empty())
					{
					const OGRGeometry* polygon = feature->GetGeometryRef();
					if (polygon->IsValid())
						{
						size_t opid = 0;
						for (const Heuristics::FMToperatingarea& oparea : operatingareas)
							{
								if (actualdev.mask.issubsetof(oparea.getmask()))
								{

									multipolygons[opid].addGeometry(polygon);
									break;
								}
								++opid;
							}
						}

					}
				OGRFeature::DestroyFeature(feature);
			}
		GDALClose(dataset);
		return multipolygons;
		}
	#endif


        template<typename T>
        bool FMTareaparser::writelayer(const Spatial::FMTlayer<T>& layer, std::string location,const std::map<T, std::string>& mapping) const
            {
            GDALAllRegister();
            GDALDataType datatype = GDT_Int32;
            if(std::is_same<double,T>::value)
                {
                datatype = GDT_Float64;
                }
            GDALDataset* wdataset = createdataset(location,layer,datatype);
			std::vector<std::string>table;
            if (!mapping.empty())
                {
                table.reserve(mapping.size());
                for(typename std::map<T, std::string>::const_iterator it = mapping.begin(); it!= mapping.end();it++)
                    {
                    table.push_back(it->second);
                    }
                }
            GDALRasterBand* wband = createband(wdataset,table);
            //iterate on blocks fill them and write them!
            int nXBlockSize, nYBlockSize;
            wband->GetBlockSize(&nXBlockSize,&nYBlockSize);
            int nXBlocks = (wband->GetXSize() + nXBlockSize - 1) / nXBlockSize;
            int nYBlocks = (wband->GetYSize() + nYBlockSize - 1) / nYBlockSize;
            double nodata = wband->GetNoDataValue();
			std::vector<int>intblock;
			std::vector<double>dblblock;
            if (std::is_same<double,T>::value)
                {
                dblblock.resize(static_cast<size_t>(nXBlockSize) * static_cast<size_t>(nYBlockSize));
                }else{
                intblock.resize(static_cast<size_t>(nXBlockSize) * static_cast<size_t>(nYBlockSize));
                }

            unsigned int ystack = 0;
            for( int iYBlock = 0; iYBlock < nYBlocks; iYBlock++ )
                {
                int nYValid=0;
                unsigned int xstack = 0;
                for( int iXBlock = 0; iXBlock < nXBlocks; iXBlock++ )
                    {
                    int  nXValid;
                    wband->GetActualBlockSize(iXBlock, iYBlock, &nXValid, &nYValid);
                    unsigned int y = ystack;
                    for( int iY = 0; iY < nYValid; iY++ )
                        {
                        unsigned int x = xstack;
                        for( int iX = 0; iX < nXValid; iX++ )
                            {
                            Spatial::FMTcoordinate coordinate(x,y);
                            typename std::map<Spatial::FMTcoordinate,T>::const_iterator it = layer.mapping.find(coordinate);
                            if (it!=layer.mapping.end())
                                {
                                if (!mapping.empty())
                                    {
                                    intblock[iX + iY * nXBlockSize] = static_cast<int>(std::distance(mapping.begin(),mapping.find(it->second)));
                                    }else if(std::is_same<int,T>::value)
                                        {
                                        intblock[iX + iY * nXBlockSize] = boost::lexical_cast<int>(it->second);
                                        }else if(std::is_same<double,T>::value)
                                            {
                                            dblblock[iX + iY * nXBlockSize] = boost::lexical_cast<int>(it->second);
                                            }
                                }else{
                                if(intblock.empty())
                                    {
                                    dblblock[iX + iY * nXBlockSize] = nodata;
                                    }else{
                                    intblock[iX + iY * nXBlockSize]  = static_cast<int>(nodata);
                                    }

                                }

                            ++x;
                            }
                        ++y;
                        }
                    if(intblock.empty())
                    {
                        if (wband->WriteBlock(iXBlock,iYBlock,&dblblock[0])!=CPLErr::CE_None)
                            {
                             _exhandler->raise(Exception::FMTexc::FMTinvalidrasterblock,_section,wdataset->GetDescription(), __LINE__, __FILE__);
                            }
                    }else{
                        if (wband->WriteBlock(iXBlock,iYBlock,&intblock[0])!=CPLErr::CE_None)
                            {
                             _exhandler->raise(Exception::FMTexc::FMTinvalidrasterblock,_section,wdataset->GetDescription(), __LINE__, __FILE__);
                            }
                        }
                    xstack+=nXValid;
                    }
                ystack+=nYValid;
                }
            wband->ComputeStatistics(FALSE,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr);
            wband->FlushCache();
            wdataset->FlushCache();
            GDALClose(wdataset);
            return true;
            }
	#ifdef FMTWITHOSI
			std::vector<Heuristics::FMToperatingarea> FMTareaparser::getneighborsfrompolygons(const std::vector<OGRMultiPolygon>& multipolygons,
																						std::vector<Heuristics::FMToperatingarea> operatingareas,
																						const double& buffersize) const
				{
				std::vector<OGRPolygon*>mergedpolygons;
				for (const OGRMultiPolygon& polygons : multipolygons)
					{
					OGRGeometry* geometry = polygons.UnionCascaded();
					OGRPolygon* polygon = reinterpret_cast<OGRPolygon*>(geometry);
					mergedpolygons.push_back(polygon);
					}
				std::map<Core::FMTmask,std::vector<Core::FMTmask>>neighborhood;
				for (size_t opareaindex = 0; opareaindex < operatingareas.size();++opareaindex)
					{
					double fullbuffered = 0;
					std::vector<size_t>neighborsid;
					std::vector<double>areas;
					if (mergedpolygons.at(opareaindex) && !mergedpolygons.at(opareaindex)->IsEmpty() && mergedpolygons.at(opareaindex)->IsValid())
						{
						OGRGeometry* buffered = (mergedpolygons.at(opareaindex)->Buffer(buffersize));
						for (size_t opareaneighborindex = 0; opareaneighborindex < operatingareas.size(); ++opareaneighborindex)
							{
							if (opareaindex != opareaneighborindex&&mergedpolygons.at(opareaneighborindex)&&
								buffered->Intersects(mergedpolygons.at(opareaneighborindex)))
								{
								OGRGeometry* intersect = buffered->Intersection(mergedpolygons.at(opareaneighborindex));
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
					for (size_t neighborid = 0 ; neighborid < neighborsid.size(); ++neighborid)
						{
						if ((areas.at(neighborid)/fullbuffered) >= operatingareas.at(neighborsid.at(neighborid)).getneihgborsperimeter())
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
					if (neighborhood.find(oparea.getmask())!= neighborhood.end())
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
				////////////
				for (OGRPolygon* polygon : mergedpolygons)
					{
					OGRGeometryFactory::destroyGeometry(polygon);
					}
				return operatingareas;
				}


			std::vector<Heuristics::FMToperatingarea> FMTareaparser::getneighbors(std::vector<Heuristics::FMToperatingarea> operatingareaparameters,
																			const std::vector<Core::FMTtheme>& themes, const std::string& data_vectors,
																			const std::string& agefield, const std::string& areafield, double agefactor,
																			double areafactor, std::string lockfield,
																			double minimal_area , double buffersize) const
				{
				std::vector<OGRMultiPolygon>multipolygons = this->getmultipolygons(operatingareaparameters, themes, data_vectors,
																agefield, areafield, agefactor,
																areafactor, lockfield, minimal_area);
				return getneighborsfrompolygons(multipolygons, operatingareaparameters, buffersize);
				}
		#endif
#endif
			FMTareaparser::FMTareaparser() :
				FMTparser(),
				rxcleanarea("^(([*A]*)([^|]*)(_lock)([^0-9]*)([0-9]*))|(([*A]*)([^|]*)([|])([^|]*)([|])([^0-9]*)(.+))|(([*A]*)(([^|]*)([|])([^|]*)([|])))|([*A]*)(.+)", std::regex_constants::ECMAScript | std::regex_constants::icase)
			{
				_section = Core::FMTwssect::Area;
			}

			FMTareaparser::FMTareaparser(const FMTareaparser& rhs) : FMTparser(rhs), rxcleanarea(rhs.rxcleanarea)
			{

			}
			FMTareaparser& FMTareaparser::operator = (const FMTareaparser& rhs)
			{
				if (this != &rhs)
				{
					FMTparser::operator = (rhs);
					rxcleanarea = rhs.rxcleanarea;
				}
				return *this;
			}

			std::vector<Core::FMTactualdevelopment>FMTareaparser::read(const std::vector<Core::FMTtheme>& themes, const Core::FMTconstants& constants, std::string location)
			{
				std::vector<Core::FMTactualdevelopment>areas;
				if (!location.empty())
				{
					std::ifstream areastream(location);
					bool potential_futurs = false;
					bool got0area = false;
					size_t futurtype = 0;
					std::smatch kmatch;
					std::vector<std::string>splitted;
					if (FMTparser::tryopening(areastream, location))
					{
						bool inactualdevs = false;
						std::unordered_map<size_t, size_t>devsindex;
						while (areastream.is_open())
						{
							std::string line = FMTparser::getcleanlinewfor(areastream, themes, constants);
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
								if (std::regex_search(line, kmatch, FMTareaparser::rxcleanarea))
								{
									std::string strlock = std::string(kmatch[6]) + std::string(kmatch[14]);
									const std::string masknage = std::string(kmatch[3]) + std::string(kmatch[9]) + std::string(kmatch[18]) + std::string(kmatch[23]);
									std::string mask;
									double area;
									int age, lock;
									size_t linesize;
									splitted = FMTparser::spliter(masknage, FMTparser::rxseparator);
									linesize = splitted.size();
									inactualdevs = true;
									for (size_t themeid = 0; themeid < (linesize - 2); ++themeid)
									{
										mask += splitted.at(themeid) + " ";
									}
									mask.pop_back();
									area = getnum<double>(splitted.at(linesize - 1), constants);
									if (area > 0)
									{
										got0area = false;
										if (!validate(themes, mask, " at line " + std::to_string(_line))) continue;
										potential_futurs = false;
										age = getnum<int>(splitted.at(linesize - 2), constants);
										lock = 0;
										if (FMTparser::isvalid(strlock))
										{
											lock = getnum<int>(strlock, constants);
										}
										const Core::FMTactualdevelopment actualdevelopment(Core::FMTmask(mask, themes), age, lock, area);
										//Weird non unique area section...
										const size_t hashform = boost::hash<Core::FMTdevelopment>()(actualdevelopment);
										std::unordered_map<size_t, size_t>::const_iterator hashit = devsindex.find(hashform);
										if (devsindex.find(hashform) == devsindex.end())
										{
											devsindex[hashform] = areas.size();
											areas.push_back(actualdevelopment);
										}
										else {
											areas[hashit->second].setarea(areas[hashit->second].getarea() + area);
										}
									}
									else {
										got0area = true;
									}

								}
							}
							else if (!areas.empty() && _comment.empty())
							{
								potential_futurs = true;
							}
						}
					}
				}
				return areas;
			}
			bool FMTareaparser::write(const std::vector<Core::FMTactualdevelopment>& areas, std::string location)
			{
				std::ofstream areastream;
				areastream.open(location);
				double sumarea = 0;
				for (const Core::FMTactualdevelopment& dev : areas)
				{
					sumarea += dev.getarea();
				}
				if (tryopening(areastream, location))
				{
					areastream << ";Total area: " << std::to_string(sumarea) << "\n";
					const std::string maskstr(areas.at(0).mask);
					std::vector<std::string>splitted_mask;
					boost::split(splitted_mask, maskstr, boost::is_any_of(" /t"), boost::token_compress_on);
					std::string header_line = ";*A ";
					for (size_t theme_id = 1; theme_id <= splitted_mask.size();++theme_id)
					{
						header_line += "TH" + std::to_string(theme_id) + " ";
						++theme_id;
					}
					header_line += "AGE";
					header_line += " AREA";
					areastream << header_line << "\n";
					for (const Core::FMTactualdevelopment& area : areas)
					{
						areastream << std::string(area) << "\n";
					}
					areastream.close();
					return true;
				}
				return false;
			}
}
