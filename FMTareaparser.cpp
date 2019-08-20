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

namespace WSParser{

FMTareaparser::FMTareaparser() :
    FMTparser(),
    rxcleanarea(regex("^(([*A]*)([^|]*)(_lock)([^0-9]*)([0-9]*))|(([*A]*)([^|]*)([|])([^|]*)([|])([^0-9]*)(.+))|(([*A]*)(([^|]*)([|])([^|]*)([|])))|([*A]*)(.+)",regex_constants::ECMAScript|regex_constants::icase))
        {
        _section=FMTwssect::Area;
        }

    FMTareaparser::FMTareaparser(const FMTareaparser& rhs) : FMTparser(rhs),rxcleanarea(rhs.rxcleanarea)
        {

        }
    FMTareaparser& FMTareaparser::operator = (const FMTareaparser& rhs)
        {
            if(this!=&rhs)
                {
                FMTparser::operator = (rhs);
                rxcleanarea = rhs.rxcleanarea;
                }
        return *this;
        }

    void FMTareaparser::validate_raster(const vector<string>&data_rasters) const
        {
        int xsize = -1;
        int ysize = -1;
        int rastercount = -1;
        int overview = -1;
        string projection = "";
        for(const string& location : data_rasters)
            {
            GDALDataset* data = getdataset(location);
            GDALRasterBand* band = getband(data);
            if (xsize>0)
                {
                if ((data->GetRasterXSize()!=xsize) || (data->GetRasterYSize()!=ysize) || (data->GetRasterCount()!=rastercount) || (data->GetProjectionRef()!=projection) || (band->GetOverviewCount()!=overview))
                    {
                    _exhandler->raise(FMTexc::FMTinvalidband,_section,data->GetDescription(), __LINE__, __FILE__);
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
	vector<FMTGCBMtransition> FMTareaparser::getGCBMtransitions(const FMTlayer<string>& stacked_actions,
																const FMTlayer<int>& ages,
																const FMTforest& newfor,
																const vector<FMTtheme>& themes) const
		{
		map<string, vector<double>>stats;
		map<string, vector<map<string,int>>>attributes;
		vector<FMTlayer<string>>newforests = newfor.getthemes(themes);
		for (map<FMTcoordinate, string>::const_iterator itcoord = stacked_actions.mapping.begin();
			itcoord != stacked_actions.mapping.end(); itcoord++)
		{
			if (ages.mapping.find(itcoord->first) != ages.mapping.end())
			{
				if (stats.find(itcoord->second) == stats.end())
				{
					stats[itcoord->second] = vector<double>(2, 0);
				}
				stats[itcoord->second][0] += ages.mapping.at(itcoord->first);
				++stats[itcoord->second][1];

				if (attributes.find(itcoord->second) == attributes.end())
					{
					attributes[itcoord->second] = vector<map<string, int>>(newforests.size());
					}
				int tid = 0;
				for (const FMTlayer<string>& nfor : newforests)
					{
					string value = nfor.mapping.at(itcoord->first);
					if (attributes.at(itcoord->second).at(tid).find(value) == attributes.at(itcoord->second).at(tid).end())
						{
						attributes[itcoord->second][tid][value] = 0;
						}
					attributes[itcoord->second][tid][value] += 1;
					++tid;
					}
			}
		}

		vector<FMTGCBMtransition>GCBM;
		for (map<string, vector<double>>::const_iterator it = stats.begin(); it != stats.end(); it++)
			{
			int ageafter = int(round(it->second[0] / it->second[1]));
			string action_name = it->first;
			/*vector<string>attributes;
			boost::split(attributes, action_name, boost::is_any_of("-"));*/
			map<string, string>theme_collection;
			//attributes.erase(attributes.begin());
			int id = 0;
			for (const FMTtheme& theme : themes)
			{
				string theme_name = "THEME" + to_string(theme.getid()+1);
				int maxhit = 0;
				string returntheme = "";
				for (map<string,int>::const_iterator cit = attributes.at(it->first).at(id).begin(); cit!= attributes.at(it->first).at(id).end();++cit)
					{
					if (cit->second > maxhit)
						{
						maxhit = cit->second;
						//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "inprocess : " << cit->first << "\n";
						returntheme = cit->first;
						}

					}
				theme_collection[theme_name] = returntheme;
				++id;
			}
			GCBM.push_back(FMTGCBMtransition(ageafter, theme_collection, action_name));
			}
		return GCBM;
		}



    bool FMTareaparser::writeforest(const Spatial::FMTforest& for_layer,
                         const vector<FMTtheme>& themes,
                         const vector<string>&data_rasters,
                         const string& age,
                         const string& lock,
                         vector<map<string,string>>mapping) const
        {
        const vector<Spatial::FMTlayer<string>> themes_layer = for_layer.getthemes(themes);
		if (!themes_layer.empty())
			{
			if (mapping.size() != themes.size())
			{
				mapping.clear();
				for (const FMTtheme& theme : themes)
				{
					const map<string, string> themes_mapping = theme.getvaluenames();
					map<string, string> layer_map;
					for (map<string, string>::const_iterator dit = themes_mapping.begin(); dit != themes_mapping.end(); dit++)
					{
						layer_map[dit->first] = dit->first;
					}
					mapping.push_back(layer_map);
				}
			}
			int layer_id = 0;
			for (const map<string, string>& layermap : mapping)
			{
				writelayer<string>(themes_layer[layer_id], data_rasters[layer_id], layermap);
				++layer_id;
			}
			const map<int, string>emptymapping;
			Spatial::FMTlayer<int>agelayer = for_layer.getage();
			writelayer<int>(agelayer, age, emptymapping);
			Spatial::FMTlayer<string>locklayer = for_layer.getlock();
			map<string, string>lockmap;
			const vector<string> attributes = locklayer.getattributes();
			for (const string& att : attributes)
			{
				lockmap[att] = att;
			}
			writelayer<string>(locklayer, lock, lockmap);
			return true;
			}
		return false;
        }

	string FMTareaparser::getdisturbancepath(const string& location, const int& period) const
		{
		boost::filesystem::path dir(location);
		const string layername = "DIST_" + to_string(period) + ".tif";
		boost::filesystem::path file(layername);
		boost::filesystem::path full_path = dir / file;
		return full_path.string();
		}

	/*bool FMTareaparser::writedisturbances(const string& location,
                                     const Spatial::FMTdisturbancestack& disturbances,
                                    const Spatial::FMTforest& for_layer,
                                    map<string,string> mapping) const
        {
        vector<FMTlayer<string>> distvec = disturbances.getlayers<Core::FMTdevelopment>(for_layer);
		vector<FMTGCBMtransition>transitions;
		if (!distvec.empty())
			{
				if (mapping.empty())
					{
					mapping = disturbances.directmapping();
					}

					int period = 1;
					for (const Spatial::FMTlayer<string>& layer : distvec)
						{
						writelayer<string>(layer, getdisturbancepath(location, period), mapping);
						++period;
						}
			return true;
			}
		return false;
        }*/

	vector<FMTGCBMtransition> FMTareaparser::writedisturbances(const string& location,
		const Spatial::FMTdisturbancestack& disturbances,
		const Spatial::FMTforest& for_layer,
		const Spatial::FMTforest& out_layer,
		const vector<FMTtheme>& themes,
		map<string, string> mapping) const
	{
		vector<FMTlayer<string>> distvec = disturbances.getlayers<Core::FMTdevelopment>(for_layer);
		vector<FMTGCBMtransition>transitions;
		if (!distvec.empty())
		{
			if (mapping.empty())
			{
				mapping = disturbances.directmapping();
			}

			if (!themes.empty())
			{
				vector<FMTlayer<string>>forest_attributes = for_layer.getthemes(themes);
				FMTlayer<int>ages = out_layer.getage();
				int period = int(distvec.size());
				int themeid = 0;
				for (const FMTlayer<string>& attlayer : forest_attributes)
				{
					map<string, string>newmap;
					map<string, string>theme_names = themes[themeid].getvaluenames();
					for (map<string, string>::const_iterator it = mapping.begin(); it != mapping.end(); it++)
					{
						for (map<string, string>::const_iterator the_it = theme_names.begin(); the_it != theme_names.end(); the_it++)
						{
							newmap[it->first + "-" + the_it->first] = it->first + "-" + the_it->first;
						}
					}
					mapping = newmap;
					distvec.back() += attlayer;
					++themeid;
				}
				transitions = getGCBMtransitions(distvec.back(), ages,out_layer,themes);
				writelayer<string>(distvec.back(), getdisturbancepath(location, period), mapping);
			}
			else {
				int period = 1;
				for (const Spatial::FMTlayer<string>& layer : distvec)
				{
					writelayer<string>(layer, getdisturbancepath(location, period), mapping);
					++period;
				}
			}
		}
		return transitions;
	}

    Spatial::FMTforest FMTareaparser::readrasters(const vector<FMTtheme>& themes,
                                             const vector<string>&data_rasters,
                                             const string& age,double agefactor,
                                             double areafactor,string lock) const
        {
        GDALAllRegister();
        vector<string>allrasters = data_rasters;
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
        vector<GInt32>agedata(nXBlockSize * nYBlockSize);
        vector<GInt32>attributedata(nXBlockSize * nYBlockSize);
        GDALDataset* lockdataset = NULL;
        GDALRasterBand* lockband = NULL;
        vector<GInt32>lockdata;
        vector<double>pad(6);
        agedataset->GetGeoTransform(&pad[0]);
        double cellsize = (abs(pad[1]*pad[5]) * areafactor);
        vector<int>lockatts;
        if(!lock.empty())
            {
            lockdataset = getdataset(lock);
            vector<string>lockstr = getcat(lockdataset);
            lockatts.reserve(lockstr.size());
            for (const string& strlock : lockstr)
                {
                vector<string>spstr;
                boost::split(spstr,strlock,boost::is_any_of("\t "), boost::token_compress_on);
                lockatts.push_back(stoi(spstr[1]));
                }
            lockband = getband(lockdataset);
            lockdata = vector<GInt32>(nXBlockSize * nYBlockSize,0);
            }
        vector<GDALDataset*>datasets;
        vector<GDALRasterBand*>bands;
        vector<vector<string>>attributes;
        for(const string& location : data_rasters)
            {
            GDALDataset* dataset = getdataset(location);
            GDALRasterBand* band = getband(dataset);
            datasets.push_back(dataset);
            bands.push_back(band);
            attributes.push_back(getcat(dataset));
            }
        //vector<FMTactualdevelopment>devs;
        map<Spatial::FMTcoordinate,FMTdevelopment>mapping;
        //int nblock = 0;
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
                     _exhandler->raise(FMTexc::FMTinvalidrasterblock,_section,agedataset->GetDescription(), __LINE__, __FILE__);
                    }
                if (lockdataset!= NULL)
                    {
                    if (CE_None!=lockband->ReadBlock( iXBlock, iYBlock, &lockdata[0]))
                        {
                        _exhandler->raise(FMTexc::FMTinvalidrasterblock,_section,lockdataset->GetDescription(), __LINE__, __FILE__);
                        }
                    }
                ageband->GetActualBlockSize(iXBlock, iYBlock, &nXValid, &nYValid);
                map<int,string>mapattributes;
                boost::unordered_map<int,Spatial::FMTcoordinate>coordinates;
                vector<int>counts(nXBlockSize * nYBlockSize,0);
                for(size_t themeid = 0 ; themeid < data_rasters.size();++themeid)
                    {
                    if (CE_None!=bands[themeid]->ReadBlock( iXBlock, iYBlock, &attributedata[0]))
                        {
                        _exhandler->raise(FMTexc::FMTinvalidrasterblock,_section,datasets[themeid]->GetDescription(), __LINE__, __FILE__);
                        }
                    unsigned int y = ystack;
                    for( int iY = 0; iY < nYValid; iY++ )
                        {
                        unsigned int x = xstack;
                        for( int iX = 0; iX < nXValid; iX++ )
                            {
                            unsigned int baselocation = (iX + iY * nXBlockSize);
                            int intattribute = attributedata[baselocation];
                            if (intattribute != nodata)
                                {
                                string attribute = attributes[themeid][intattribute];
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
                    //map<string,int>mapper; key = st+"AGE"+"LOCK" -> int location
				    size_t attcounts = themes.size();
                    if (!mapattributes.empty())
                        {
                        for (map<int,string>::iterator att=mapattributes.begin(); att!=mapattributes.end(); ++att)
                            {
                            string st = att->second;
                            int location = att->first;
                            if (counts[location]==attcounts && agedata[location] != nodata)
                                {
                                string maskvalue = st.substr(0, st.size()-1);
								//FMTmask mask = validate(themes, maskvalue);
                                if (!validate(themes,maskvalue)) continue;
                                FMTmask mask(maskvalue,themes);
                                int lock = 0;
                                if (!lockdata.empty())
                                    {
                                    if(lockdata[location] != nodata)
                                        {
                                        lock = lockatts[lockdata[location]];
                                        }
                                    }
                                FMTdevelopment dev(mask,int(agedata[location]*agefactor),lock);
                                mapping[coordinates[location]] = dev;
                                }else{
                                ++missing;
                                }
                            }
                        }
                    xstack+=nXValid;
                    //++nblock;
                    }
                ystack+=nYValid;
                 }
             if(missing>0)
                {
                string message = " for "+to_string(missing)+" raster cells";
                _exhandler->raise(FMTexc::FMTmissingrasterattribute,_section,message, __LINE__, __FILE__);
                }
             string projection = agedataset->GetProjectionRef();
             const unsigned int xsize = ageband->GetXSize();
             const unsigned int ysize = ageband->GetYSize();
             return Spatial::FMTlayer<FMTdevelopment>(mapping,pad,xsize,ysize,projection,cellsize);
             }
    vector<FMTactualdevelopment>FMTareaparser::readvectors(const vector<FMTtheme>& themes,const string& data_vectors,
		const string& agefield,const string& areafield,double agefactor,double areafactor,string lockfield,double minimalarea) const
        {
        GDALAllRegister();
        GDALDataset* dataset = getvectordataset(data_vectors);
        OGRLayer * layer = getlayer(dataset,0);
        map<int,int>themes_fields;
        int age_field=-1;
        int lock_field=-1;
        int area_field=-1;
        getWSfields(layer,themes_fields,age_field,area_field,lock_field,agefield,areafield,lockfield);
        if (themes_fields.size()!=themes.size())
            {
            _exhandler->raise(FMTexc::WSinvalid_maskrange,_section,dataset->GetDescription(), __LINE__, __FILE__);
            }
        OGRFeature *feature;
        vector<FMTactualdevelopment>devs;
        layer->ResetReading();
        while((feature = layer->GetNextFeature()) != NULL)
            {
            int age  = int(feature->GetFieldAsInteger(age_field)*agefactor);
            double area = (feature->GetFieldAsDouble(area_field)*areafactor);
			if (area > minimalarea)
				{
				int lock = 0;
				if (lock_field!=-1)
					{
					string slock =feature->GetFieldAsString(lock_field);
					boost::to_upper(slock);
					slock.erase(0,5);
					if (isvalid(slock))
						{
						lock = stoi(slock);
						}
					}
				vector<string>masks(themes_fields.size());
				for(map<int,int>::const_iterator it=themes_fields.begin(); it!=themes_fields.end(); ++it)
					{
					string attribute = feature->GetFieldAsString(it->second);
					boost::to_upper(attribute);
					masks[it->first] = attribute;
					}
				string tmask = boost::algorithm::join(masks," ");
				//FMTmask mask = validate(themes, tmask);
				if (!validate(themes, tmask)) continue;
				FMTmask mask(tmask,themes);
				FMTactualdevelopment dev(mask,age,lock,area);
				vector<FMTactualdevelopment>::iterator it = find(devs.begin(),devs.end(),dev);
				if (it!=devs.end())
					{
					it->area+=area;
					}else{
					devs.push_back(dev);
					}
			}
            OGRFeature::DestroyFeature(feature);
            }
        return devs;
        }

    vector<FMTactualdevelopment>FMTareaparser::read(const vector<FMTtheme>& themes,const FMTconstants& constants,string location)
        {
		vector<FMTactualdevelopment>areas;
		if(!location.empty())
			{ 
			ifstream areastream(location);
			string line;
			if (FMTparser::tryopening(areastream,location))
				{
				bool inactualdevs = false;
				while(areastream.is_open())
					{
					//line = FMTparser::getcleanline(areastream);
					line = FMTparser::getcleanlinewfor(areastream, themes, constants);
					if (!line.empty())
						{
						smatch kmatch;
						regex_search(line,kmatch,FMTareaparser::rxcleanarea);
						string strlock = string(kmatch[6]) + string(kmatch[14]);
						string masknage = string(kmatch[3])+string(kmatch[9]) + string(kmatch[18])+ string(kmatch[23]);
						string mask;
						double area;
						int age,lock;
						size_t linesize;
						vector<string>splitted = FMTparser::spliter(masknage,FMTparser::rxseparator);
						linesize = splitted.size();
						inactualdevs = true;
						for(int themeid = 0 ; themeid < (linesize-2); ++themeid)
							{
							mask+=splitted[themeid]+" ";
							}
						mask.pop_back();
						if (!validate(themes, mask)) continue;
						area = getnum<double>(splitted[linesize-1],constants);
						if (area>0)
							{
							age = getnum<int>(splitted[linesize-2],constants);
							lock = 0;
							if (FMTparser::isvalid(strlock))
								{
								lock = getnum<int>(strlock,constants);
								}
							areas.push_back(FMTactualdevelopment(FMTmask(mask,themes),age,lock,area));
							}
							//_exhandler->raise(FMTexc::WSfutur_types,_section,mask+" at line" + to_string(_line), __LINE__, __FILE__);
					}else if(inactualdevs && !_comment.empty() && (_comment.find("=") != string::npos) &&
						(_comment.find("NEW DEVELOPMENT TYPES") != string::npos || _comment.find("NOUVEAUX TYPES DE") != string::npos))
						{
						break;
						}
					}
				}
			}
        return areas;
        }
    bool FMTareaparser::write(const vector<FMTactualdevelopment>& areas, string location)
        {
        ofstream areastream;
        areastream.open(location);
        double sumarea = 0;
        bool gotlock = false;
        for(const FMTactualdevelopment& dev : areas)
            {
            if (dev.lock>0)
                {
                gotlock = true;
                }
            sumarea+=dev.area;
            }
        if (tryopening(areastream,location))
            {
            areastream<<";Total area: "<<to_string(sumarea)<<"\n";
            const string maskstr = areas.at(0).mask.getstr();
            vector<string>splitted_mask;
            boost::split(splitted_mask,maskstr, boost::is_any_of(" /t"), boost::token_compress_on);
            string header_line = ";";
            size_t theme_id = 1;
            for (const string& theme : splitted_mask)
                {
                header_line+="TH"+to_string(theme_id) +" ";
                ++theme_id;
                }
            header_line+="AGE";
            header_line+=" AREA";
			if (gotlock)
			{
				header_line += " LOCK";
			}
            areastream<<header_line<<"\n";
            for(const FMTactualdevelopment& area : areas)
                {
                areastream<<string(area)<<"\n";
                }
            areastream.close();
            return true;
            }
        return false;
        }

        template<typename T>
        bool FMTareaparser::writelayer(const Spatial::FMTlayer<T>& layer,string location,const map<T,string>& mapping) const
            {
            GDALAllRegister();
            GDALDataType datatype = GDT_Int32;
            if(std::is_same<double,T>::value)
                {
                datatype = GDT_Float64;
                }
            GDALDataset* wdataset = createdataset(location,layer,datatype);
            vector<string>table;
            if (!mapping.empty())
                {
                table.reserve(mapping.size());
                for(typename map<T,string>::const_iterator it = mapping.begin(); it!= mapping.end();it++)
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
            vector<int>intblock;
            vector<double>dblblock;
            if (std::is_same<double,T>::value)
                {
                dblblock.resize(nXBlockSize * nYBlockSize);
                }else{
                intblock.resize(nXBlockSize * nYBlockSize);
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
                    //vector<GInt32>block(nXBlockSize * nYBlockSize,0);
                    unsigned int y = ystack;
                    for( int iY = 0; iY < nYValid; iY++ )
                        {
                        unsigned int x = xstack;
                        for( int iX = 0; iX < nXValid; iX++ )
                            {
                            FMTcoordinate coordinate(x,y);
                            typename map<Spatial::FMTcoordinate,T>::const_iterator it = layer.mapping.find(coordinate);
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
                             _exhandler->raise(FMTexc::FMTinvalidrasterblock,_section,wdataset->GetDescription(), __LINE__, __FILE__);
                            }
                    }else{
                        if (wband->WriteBlock(iXBlock,iYBlock,&intblock[0])!=CPLErr::CE_None)
                            {
                             _exhandler->raise(FMTexc::FMTinvalidrasterblock,_section,wdataset->GetDescription(), __LINE__, __FILE__);
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

}
