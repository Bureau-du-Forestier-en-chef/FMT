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


#include "FMTlandscapeparser.h"

namespace Parser
{

FMTlandscapeparser::FMTlandscapeparser() :
    FMTparser(),
	rxcleanlans("^(\\*THEME)([\\s\\t]*)(([0-9]*$)|([0-9]*)([\\s\\t]*)(.+))|(\\*AGGREGATE)([\\s\\t]*)([^\\s^\\t]*)|([^\\s^\\t]*)([\\s\\t]*)(.+)", std::regex_constants::ECMAScript | std::regex_constants::icase),
	rxindex("^(_INDEX)(\\()([^\\)]*)(\\))", std::regex_constants::ECMAScript| std::regex_constants::icase),
    rxparameter("^([^=]*)(=)(#.+|[\\d.]*)", std::regex_constants::ECMAScript| std::regex_constants::icase)
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


    std::map<std::string,double>FMTlandscapeparser::getindexes(std::string index_line,const Core::FMTconstants& constants)
        {
		std::map<std::string,double>indexes;
        boost::trim(index_line);
		std::smatch kmatch;
        if (std::regex_search(index_line,kmatch,FMTlandscapeparser::rxindex))
            {
			std::vector<std::string>parameters;
			const std::string values = kmatch[3];
            boost::split(parameters,values, boost::is_any_of(","), boost::token_compress_on);
            for(const std::string& parameter : parameters)
                {
                if (std::regex_search(parameter,kmatch,FMTlandscapeparser::rxparameter))
                    {
                    indexes[std::string(kmatch[1])] = getnum<double>(std::string(kmatch[3]),constants);
                    }
                }
            }
        return indexes;
        }
#ifdef FMTWITHGDAL
	std::vector<Core::FMTtheme>FMTlandscapeparser::readrasters(const std::vector<std::string>& locations)
        {
		std::vector<Core::FMTtheme>themes;
		try {
        GDALAllRegister();
        int start = 0;
        int id = 0;
        for(const std::string& location : locations)
            {
            GDALDataset* dataset = getdataset(location);
			const std::vector<std::string>categories = getcat(dataset);
            themes.push_back(Core::FMTtheme(categories,id,start,""));
            start+=static_cast<int>(categories.size());
            ++id;
            }
		}catch (const std::exception& exception)
			{
			_exhandler->throw_nested(exception);
			}
        return themes;
        }
    std::vector<Core::FMTtheme>FMTlandscapeparser::readvectors(const std::string& location)
        {
		std::vector<Core::FMTtheme>themes;
		try {
			GDALAllRegister();
			GDALDataset* dataset = getvectordataset(location);
			OGRLayer * layer = getlayer(dataset, 0);
			std::map<int, int>themes_fields;
			int age, area, lock;
			getWSfields(layer, themes_fields, age, area, lock);
			OGRFeature *feature;
			layer->ResetReading();
			std::vector<std::vector<std::string>>themesattributes(themes_fields.size(), std::vector<std::string>());
			while ((feature = layer->GetNextFeature()) != NULL)
			{
				std::vector<std::string>masks(themes_fields.size());
				for (std::map<int, int>::const_iterator it = themes_fields.begin(); it != themes_fields.end(); ++it)
				{
					const std::string attribute = feature->GetFieldAsString(it->second);
					if (find(themesattributes[it->first].begin(), themesattributes[it->first].end(), attribute) == themesattributes[it->first].end())
					{
						themesattributes[it->first].push_back(attribute);
					}
				}
				OGRFeature::DestroyFeature(feature);
			++_line;
			}
			GDALClose(dataset);
			int start = 0;
			int id = 0;
			for (const std::vector<std::string>& themeattribute : themesattributes)
			{
				themes.push_back(Core::FMTtheme(themeattribute, id, start, ""));
				++id;
				start += static_cast<int>(themeattribute.size());
			}
		}catch (const std::exception& exception)
			{
			_exhandler->throw_nested(exception);
			}
        return themes;
        }
#endif

	std::vector<Core::FMTtheme>FMTlandscapeparser::read(const Core::FMTconstants& constants,const std::string& location)
        {
		std::vector<Core::FMTtheme>themes;
		try {
			std::ifstream landstream(location);
			std::map<std::string, std::vector<std::string>>aggregates;
			std::map<std::string, std::string>valuenames;
			std::map<std::string, std::map<std::string, double>>indexes_values;
			std::string themename, aggregatename;
			int id = 0;
			int stop = 0;
			bool aggregate_redefiniton = false;
			int start = 0;
			int unknownID = 1;
			if (FMTparser::tryopening(landstream, location))
			{
				while (landstream.is_open())
				{
					std::string line = FMTparser::getcleanlinewfor(landstream, themes, constants);
					if (!line.empty())
					{
						std::smatch kmatch;
						std::regex_search(line, kmatch, FMTlandscapeparser::rxcleanlans);
						const std::string theme = std::string(kmatch[4]) + std::string(kmatch[5]);
						const std::string potentialtheme = std::string(kmatch[4]) + std::string(kmatch[5]) + std::string(kmatch[7]);
						const std::string aggregate = std::string(kmatch[8]);
						if (!potentialtheme.empty())
						{
							int tempid = 1;
							if (!std::string(kmatch[7]).empty() && theme.empty())
							{
								tempid = unknownID;
							}
							else {
								tempid = getnum<int>(theme, constants);
							}
							++unknownID;
							if (tempid > 1)
							{
								stop = static_cast<int>(valuenames.size());
								if (valuenames.size() == 0)
								{
									_exhandler->raise(Exception::FMTexc::FMTempty_theme, _section, "Theme " + std::to_string(id + 1), __LINE__, __FILE__);
								}

								themes.push_back(Core::FMTtheme(aggregates, valuenames, indexes_values, id, start, themename));
								start += stop;
								aggregatename.clear();
								aggregates.clear();
								themename.clear();
								valuenames.clear();
								indexes_values.clear();
							}
							id = tempid - 1;
							themename = std::string(kmatch[7]);
						}
						else if (!aggregate.empty())
						{
							aggregatename = kmatch[10];
							aggregate_redefiniton = false;
							if (aggregates.find(aggregatename) != aggregates.end())
							{
								aggregate_redefiniton = true;
								_exhandler->raise(Exception::FMTexc::FMTaggregate_redefinition, _section, aggregatename + " at line " + std::to_string(_line), __LINE__, __FILE__);
							}
							aggregates[aggregatename] = std::vector<std::string>();
						}
						else if (!aggregatename.empty() && !aggregate_redefiniton)
						{
							const std::string value = line;
							const std::vector<std::string>splited = FMTparser::spliter(value, FMTparser::rxseparator);
							for (const std::string& val : splited)
							{
								if (valuenames.find(val) == valuenames.end() && (aggregates.find(val) == aggregates.end() || aggregatename == val))
								{
									_exhandler->raise(Exception::FMTexc::FMTignore, _section,
										val + " at line " + std::to_string(_line), __LINE__, __FILE__);

								}
								else {
									aggregates[aggregatename].push_back(val);
								}
							}
							if (aggregates[aggregatename].size() == 0)
							{
								_exhandler->raise(Exception::FMTexc::FMTignore, _section,
									aggregatename + " empty at line " + std::to_string(_line), __LINE__, __FILE__);
							}
						}
						else {
							std::vector<std::string>splited = FMTparser::spliter(line, FMTparser::rxseparator);
							std::string name = "";
							const std::string ltheme = splited[0];
							if (splited.size() > 1)
							{
								splited.erase(splited.begin());
								name = boost::algorithm::join(splited, " ");
								const std::map<std::string, double>indexes = getindexes(name, constants);
								if (!indexes.empty())
								{
									indexes_values[ltheme] = indexes;
									name.clear();
								}
							}
							if (valuenames.find(ltheme) != valuenames.end())
							{
								_exhandler->raise(Exception::FMTexc::FMTattribute_redefinition, _section, ltheme + " at line " + std::to_string(_line), __LINE__, __FILE__);
							}
							else {
								valuenames[ltheme] = std::string(name);
							}

						}

					}
				}
				if (valuenames.size() == 0)
				{
					_exhandler->raise(Exception::FMTexc::FMTempty_theme, _section, "Theme " + std::to_string(id + 1), __LINE__, __FILE__);
				}
				themes.push_back(Core::FMTtheme(aggregates, valuenames, indexes_values, id, start, themename));
			}
			}catch(...)
				{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, _section, "while reading", __LINE__, __FILE__);
				}
        return themes;
        }
    void FMTlandscapeparser::write(const std::vector<Core::FMTtheme>& themes,const std::string& location) const
        {
        std::ofstream landscapestream;
        landscapestream.open(location);
        if (landscapestream.is_open())
            {
            for(const Core::FMTtheme& theme : themes)
                {
                landscapestream<<std::string(theme);
                }
            landscapestream.close();
            }
        }

}
