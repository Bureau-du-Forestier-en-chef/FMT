
#include "FMTlandscapeparser.h"

namespace WSParser
{

FMTlandscapeparser::FMTlandscapeparser() :
    FMTparser(),
    rxcleanlans(regex("^(\\*THEME)([\\s\\t]*)([0-9]*)([\\s\\t]*)(.+)|(\\*AGGREGATE)([\\s\\t]*)([^\\s^\\t]*)|([^\\s^\\t]*)([\\s\\t]*)(.+)",regex_constants::ECMAScript|regex_constants::icase)),
    rxindex("^(_INDEX)(\\()([^\\)]*)(\\))",regex_constants::ECMAScript|regex_constants::icase),
    rxparameter("^([^=]*)(=)(#.+|[\\d.]*)",regex_constants::ECMAScript|regex_constants::icase)
        {

        }


        FMTlandscapeparser::FMTlandscapeparser(const FMTlandscapeparser& rhs):FMTparser(rhs),rxcleanlans(rhs.rxcleanlans),rxindex(rhs.rxindex),rxparameter(rhs.rxparameter){}
        FMTlandscapeparser& FMTlandscapeparser::operator = (const FMTlandscapeparser& rhs)
            {
                if(this!=&rhs)
                {
                FMTparser::operator = (rhs);
                rxcleanlans = rhs.rxcleanlans;
                rxindex = rhs.rxindex;
                rxparameter = rhs.rxparameter;
                }
            return *this;
            }


    map<string,double>FMTlandscapeparser::getindexes(string index_line,const FMTconstants& constants)
        {
        map<string,double>indexes;
        boost::trim(index_line);
        smatch kmatch;
        if (regex_search(index_line,kmatch,FMTlandscapeparser::rxindex))
            {
            vector<string>parameters;
            string values = kmatch[3];
            boost::split(parameters,values, boost::is_any_of(","), boost::token_compress_on);
            for(const string& parameter : parameters)
                {
                if (regex_search(parameter,kmatch,FMTlandscapeparser::rxparameter))
                    {
                    indexes[string(kmatch[1])] = getnum<double>(string(kmatch[3]),constants);
                    }
                }
            }
        return indexes;
        }

    vector<FMTtheme>FMTlandscapeparser::readrasters(const vector<string>& locations)
        {
        GDALAllRegister();
        vector<FMTtheme>themes;
        int start = 0;
        int id = 0;
        for(const string& location : locations)
            {
            GDALDataset* dataset = getdataset(location);
            vector<string>categories = getcat(dataset);
            themes.push_back(FMTtheme(categories,id,start,""));
            start+=int(categories.size());
            ++id;
            }
        return themes;
        }
    vector<FMTtheme>FMTlandscapeparser::readvectors(const string& location)
        {
        GDALAllRegister();
        GDALDataset* dataset = getvectordataset(location);
        OGRLayer * layer = getlayer(dataset,0);
        map<int,int>themes_fields;
        int age,area,lock;
        getWSfields(layer,themes_fields,age,area,lock);
        OGRFeature *feature;
        layer->ResetReading();
        vector<vector<string>>themesattributes(themes_fields.size(),vector<string>());
        while((feature = layer->GetNextFeature()) != NULL)
            {
            vector<string>masks(themes_fields.size());
            for(map<int,int>::const_iterator it=themes_fields.begin(); it!=themes_fields.end(); ++it)
                {
                const string attribute = feature->GetFieldAsString(it->second);
                if (find(themesattributes[it->first].begin(),themesattributes[it->first].end(),attribute)==themesattributes[it->first].end())
                    {
                    themesattributes[it->first].push_back(attribute);
                    }
                }
            OGRFeature::DestroyFeature(feature);
            }
        GDALClose(dataset);
        vector<FMTtheme>themes;
        int start = 0;
        int id = 0;
        for(const vector<string>& themeattribute : themesattributes)
            {
            themes.push_back(FMTtheme(themeattribute,id,start,""));
            ++id;
            start+=int(themeattribute.size());
            }
        return themes;
        }

    vector<FMTtheme>FMTlandscapeparser::read(const FMTconstants& constants,string location)
        {
        ifstream landstream(location);
        string line;
        //vector<string>values;
        map<string,vector<string>>aggregates;
        map<string,string>valuenames;
        map<string,map<string,double>>indexes_values;
        vector<FMTtheme>themes;
        string themename,aggregatename;
		int id = 0;
		int stop = 0;
		bool aggregate_redefiniton = false;
        int start = 0;
		int unknownID = 1;
        if (FMTparser::tryopening(landstream,location))
            {
            while(landstream.is_open())
                {
                //line = FMTparser::getcleanline(landstream);
				line = FMTparser::getcleanlinewfor(landstream, themes, constants);
                if (!line.empty())
                    {
                    smatch kmatch;
                    regex_search(line,kmatch,FMTlandscapeparser::rxcleanlans);
                    string theme = string(kmatch[3]) + string(kmatch[5]);
                    string aggregate = string(kmatch[6]);
                    if (!theme.empty())
                        {
						int tempid = 1;
						if (!string(kmatch[5]).empty() && string(kmatch[3]).empty())
							{
							tempid = unknownID;
							++unknownID;

						}else{
							tempid = getnum<int>(theme, constants);
							}

                        if (tempid>1)
                            {
                            stop = int(valuenames.size());
                            if(valuenames.size()==0)
                                {
                                _exhandler->raise(FMTexc::WSempty_theme,_section,"Theme "+to_string(id+1), __LINE__, __FILE__);
                                }

                            themes.push_back(FMTtheme(aggregates,valuenames,indexes_values,id,start,themename));
                            start += stop;
                            aggregatename.clear();
                            aggregates.clear();
                            themename.clear();
                            valuenames.clear();
                            indexes_values.clear();
                            }
                        id=tempid-1;
                        themename = string(kmatch[5]);
                        }else if(!aggregate.empty())
                            {
                            aggregatename = kmatch[8];
							aggregate_redefiniton = false;
                            if(aggregates.find(aggregatename)!=aggregates.end())
                                {
								aggregate_redefiniton = true;
                                _exhandler->raise(FMTexc::WSaggregate_redefinition,_section,aggregatename+" at line "+to_string(_line), __LINE__, __FILE__);
                                }
                            aggregates[aggregatename] = vector<string>();
                            } else if (!aggregatename.empty() && !aggregate_redefiniton)
                                {
                                string value = line;
                                vector<string>splited = FMTparser::spliter(value,FMTparser::rxseparator);
                                for(string& val : splited)
                                    {
                                    if(valuenames.find(val)==valuenames.end() && (aggregates.find(val)==aggregates.end() || aggregatename == val))
                                        {
                                        //_exhandler->raise(FMTexc::WSundefined_aggregate_value,_section,val+" at line "+to_string(_line), __LINE__, __FILE__);
										_exhandler->raise(FMTexc::WSignore, _section,
											val + " at line " + to_string(_line), __LINE__, __FILE__);

									}else{
                                        aggregates[aggregatename].push_back(val);
                                        }
                                    }
                                if (aggregates[aggregatename].size()==0)
                                    {
                                    //_exhandler->raise(FMTexc::WSempty_theme,_section,aggregatename+" empty at line "+to_string(_line), __LINE__, __FILE__);
									_exhandler->raise(FMTexc::WSignore, _section,
										aggregatename + " empty at line " + to_string(_line), __LINE__, __FILE__);
									//aggregates.erase(aggregatename);

                                    }
                                }else{
                                    vector<string>splited = FMTparser::spliter(line,FMTparser::rxseparator);
                                    string name = "";
                                    string ltheme = splited[0];
                                    if (splited.size()>1)
                                        {
                                        splited.erase(splited.begin());
                                        name = boost::algorithm::join(splited," ");
                                        map<string,double>indexes = getindexes(name,constants);//got indexes here!
                                        if (!indexes.empty())
                                            {
                                            indexes_values[ltheme] = indexes;
                                            name.clear();
                                            }
                                        }
                                    if(valuenames.find(ltheme)!=valuenames.end())
                                        {
                                        _exhandler->raise(FMTexc::WSattribute_redefinition,_section,ltheme+" at line "+to_string(_line), __LINE__, __FILE__);
									}
									else {
										valuenames[ltheme] = string(name);
									}

                                    }

                    }
                }
            if(valuenames.size()==0)
                {
                _exhandler->raise(FMTexc::WSempty_theme,_section,"Theme "+to_string(id+1), __LINE__, __FILE__);
                }
            themes.push_back(FMTtheme(aggregates,valuenames,indexes_values,id,start,themename));
            }
        return themes;
        }
    bool FMTlandscapeparser::write(const vector<FMTtheme>& themes,string location)
        {
        ofstream landscapestream;
        landscapestream.open(location);
        if (landscapestream.is_open())
            {
            for(const FMTtheme& theme : themes)
                {
                landscapestream<<string(theme);
                }
            landscapestream.close();
            return true;
            }
        return false;
        }

}
