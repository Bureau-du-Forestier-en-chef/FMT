/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#include "FMTlandscapeparser.h"
#include "FMTtheme.h"
#include "FMTconstants.h"
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string.hpp>
#ifdef FMTWITHGDAL
	#include "ogrsf_frmts.h"
#endif
#include "FMTexceptionhandler.h"

namespace Parser
{
	const boost::regex FMTlandscapeparser::rxattributes = boost::regex(
		"^(\\*ATTRIBUTES)([\\s\\t]*)\\(([\\s\\t]*)_TH(\\d+)([\\s\\t]*)\\)",
		boost::regex_constants::ECMAScript | boost::regex_constants::icase
	);
	const boost::regex FMTlandscapeparser::rxcleanlans = boost::regex(
		"^(\\*THEME)([\\s\\t]*)(([0-9]*$)|([0-9]*)([\\s\\t]*)(.+))|(\\*AGGREGATE)([\\s\\t]*)(\\()([\\s\\t]*)(_TH)(\\d*)([\\s\\t]*)(\\))([\\s\\t]*)(.+)|(\\*AGGREGATE)([\\s\\t]*)([^\\s^\\t]*)|([^\\s^\\t]*)([\\s\\t]*)(.+)", 
		boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex FMTlandscapeparser::rxindex = boost::regex(
		"^(_INDEX)(\\()([^\\)]*)(\\))", 
		boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex FMTlandscapeparser::rxparameter = boost::regex(
		"^([^=]*)(=)(#.+|[-\\s\\t\\d.]*)", 
		boost::regex_constants::ECMAScript | boost::regex_constants::icase);

FMTlandscapeparser::FMTlandscapeparser() :
    FMTparser()
        {
		setSection(Core::FMTsection::Landscape);
        }


    std::map<std::string,double>FMTlandscapeparser::getindexes(std::string indexm_line,const Core::FMTconstants& constants)
        {
		std::map<std::string,double>indexes;
		try {
			boost::trim(indexm_line);
			boost::smatch kmatch;
			if (boost::regex_search(indexm_line, kmatch, FMTlandscapeparser::rxindex))
			{
				std::vector<std::string>parameters;
				const std::string values = kmatch[3];
				boost::split(parameters, values, boost::is_any_of(","), boost::token_compress_on);
				for (const std::string& parameter : parameters)
				{
					if (boost::regex_search(parameter, kmatch, FMTlandscapeparser::rxparameter))
					{
						std::string index_value = std::string(kmatch[3]);
						boost::trim(index_value);
						indexes[std::string(kmatch[1])] = getNum<double>(index_value, constants);
					}
				}
			}
		}catch (...)
			{
			_exhandler->raisefromcatch(
				 "for line " + indexm_line,"FMTlandscapeparser::getindexes", __LINE__, __FILE__,m_section);
			}
        return indexes;
        }

	std::tuple<bool, int> FMTlandscapeparser::isPreDeclaredTheme(
		int preDeclaredThemeId, 
		std::string line, 
		const Core::FMTconstants& constants)
	{
		bool isDeclared = false;
		if (preDeclaredThemeId == -1) {
			boost::smatch kmatch;
			if (boost::regex_search(line, kmatch, FMTlandscapeparser::rxattributes))
			{
				preDeclaredThemeId = getNum<int>(kmatch[4], constants);
			}
		}

		return {isDeclared, preDeclaredThemeId};
	}

#ifdef FMTWITHGDAL
	std::vector<Core::FMTtheme>FMTlandscapeparser::readrasters(const std::vector<std::string>& locations)
        {
		std::vector<Core::FMTtheme>themes;
		try {
        //GDALAllRegister();
        size_t start = 0;
		size_t id = 0;
        for(const std::string& location : locations)
            {
            GDALDataset* dataset = getDataset(location);
			const std::vector<std::string>categories = getCat(dataset);
            themes.push_back(Core::FMTtheme(categories,id,start,""));
			//themes.back().passinobject(*this);
            start+=static_cast<int>(categories.size());
            ++id;
            }
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTlandscapeparser::readrasters", __LINE__, __FILE__,m_section);
		}
			

        return themes;
        }
    std::vector<Core::FMTtheme>FMTlandscapeparser::readvectors(const std::string& location)
        {
		std::vector<Core::FMTtheme>themes;
		try {
			//GDALAllRegister();
			GDALDataset* dataset = getVectorDataset(location);
			OGRLayer * layer = getLayer(dataset, 0);
			std::map<int, int>themes_fields;
			int age, area, lock;
			getWSFields(layer, themes_fields, age, area, lock);
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
			++m_line;
			}
			GDALClose(dataset);
			size_t start = 0;
			size_t id = 0;
			for (const std::vector<std::string>& themeattribute : themesattributes)
			{
				themes.push_back(Core::FMTtheme(themeattribute, id, start, ""));
				//themes.back().passinobject(*this);
				++id;
				start += themeattribute.size();
			}
		}catch (...)
			{
				_exhandler->printexceptions("at " + location, "FMTlandscapeparser::readvectors",
					__LINE__, __FILE__, m_section);
			}

        return themes;
        }
#endif

	std::vector<Core::FMTtheme>FMTlandscapeparser::read(const Core::FMTconstants& constants,const std::string& location)
        {
		std::vector<Core::FMTtheme>themes;
		try {
			std::ifstream landstream(location);
			std::vector<std::string> attributes;
			std::vector<std::string> attributenames;
			std::vector<std::string> aggregates;
			std::vector<std::vector<std::string>> aggregatenames;
			std::vector<std::map<std::string, double>> indexes_values;

			std::string themename, aggregatename;
			size_t id = 0;
			size_t stop = 0;
			bool aggregate_redefiniton = false;
			size_t start = 0;
			size_t unknownID = 1;
			int pasttheme = -1;
			std::map<size_t, std::pair<std::vector<std::string>, std::vector<std::string>>> preDeclarations;
			int preDeclaredThemeId = -1;
			if (FMTparser::tryOpening(landstream, location))
			{
				std::queue<FMTparser::FMTLineInfo> Lines = FMTparser::GetCleanLinewfor(landstream, themes, constants);
				while (!Lines.empty())
				{
					const std::string line = GetLine(Lines);
					if (!line.empty())
					{
						boost::smatch kmatch2;
						if (boost::regex_search(line, kmatch2, FMTlandscapeparser::rxattributes))
						{
							preDeclaredThemeId = getNum<int>(kmatch2[4], constants);
							continue;
						}

						std::vector<std::string> splited = FMTparser::spliter(line, FMTparser::m_SEPARATOR);

						if (preDeclaredThemeId != -1)
						{
							std::string emptyName = "";
							if (splited.size() > 1)
							{
								emptyName = splited[1];
							}
							preDeclarations[preDeclaredThemeId].first.push_back(splited[0]);
							preDeclarations[preDeclaredThemeId].second.push_back(emptyName);

							continue;
						}


						boost::smatch kmatch;
						boost::regex_search(line, kmatch, FMTlandscapeparser::rxcleanlans);
						const std::string theme = std::string(kmatch[4]) + std::string(kmatch[5]);
						const std::string potentialtheme = std::string(kmatch[4]) + std::string(kmatch[5]) + std::string(kmatch[7]);
						const std::string aggregate = std::string(kmatch[8]) + std::string(kmatch[18]);
			
						if (!potentialtheme.empty())
						{
							preDeclaredThemeId = -1; 
							pasttheme = -1;
							size_t tempid = 1;
							if (!std::string(kmatch[7]).empty() && theme.empty())
							{
								tempid = unknownID;
							}
							else {
								tempid = getNum<size_t>(theme, constants);
							}
							++unknownID;
							if (tempid > 1)
							{
								stop = attributes.size();
								if (attributes.size() == 0)
								{
									_exhandler->raise(Exception::FMTexc::FMTempty_theme,
										"Theme " + std::to_string(id + 1),"FMTlandscapeparser::read", __LINE__, __FILE__, m_section);
								}
								
								if (preDeclarations.find(themes.size() + 1) != preDeclarations.end())
								{
									attributes.insert(attributes.begin(), preDeclarations[themes.size() + 1].first.begin(), preDeclarations[themes.size() + 1].first.end());
									attributenames.insert(attributenames.begin(), preDeclarations[themes.size() + 1].second.begin(), preDeclarations[themes.size() + 1].second.end());
								}
								
								themes.push_back(Core::FMTtheme(
									attributes, attributenames, aggregates, aggregatenames, indexes_values, id, start, themename));
								
								//themes.back().passinobject(*this);
								start += stop;
								attributes.clear();
								attributenames.clear();
								aggregatenames.clear();
								aggregatename.clear();
								aggregates.clear();
								themename.clear();
							
								indexes_values.clear();
							}
							id = tempid - 1;
							themename = std::string(kmatch[7]);
						}
						else if (!aggregate.empty())
						{
							pasttheme = -1;
							std::string targettheme = kmatch[13];
							int themelocation = 0;
							aggregatename = kmatch[20];
							if (!targettheme.empty())
								{
								aggregatename = kmatch[17];
								pasttheme= (getNum<int>(targettheme, constants) - 1);
								if (static_cast<size_t>(pasttheme)>= themes.size())
									{
									targettheme.clear();
									pasttheme = -1;
									}
								}
							aggregate_redefiniton = false;

							if ((std::find(aggregates.begin(), aggregates.end(), aggregatename) != aggregates.end()) ||
								(pasttheme!=-1 &&
									themes.at(pasttheme).isaggregate(aggregatename)))
							{
								aggregate_redefiniton = true;
								_exhandler->raise(Exception::FMTexc::FMTaggregate_redefinition,
									aggregatename + " at line " + std::to_string(m_line)+" in "+ m_location +". The lattest definition will be ignored.",
									"FMTlandscapeparser::read", __LINE__, __FILE__, m_section);
							}
							if (!aggregate_redefiniton)
							{
								if (targettheme.empty())
								{
									aggregates.push_back(aggregatename);
									aggregatenames.push_back(std::vector<std::string>());
								}
								else {
									themes[pasttheme].push_aggregate(aggregatename);
								}
							}

						}
						else if (!aggregatename.empty() /* && !aggregate_redefiniton*/)
						{
							const std::string value = line;
							const std::vector<std::string>splited = FMTparser::spliter(value, FMTparser::m_SEPARATOR);
							for (const std::string& val : splited)
							{
								if (((std::find(attributes.begin(), attributes.end(), val) == attributes.end()) &&
									((std::find(aggregates.begin(), aggregates.end(), val) == aggregates.end()) || (aggregatename == val))&&
									(pasttheme<0||(pasttheme >= 0 && !(themes.at(pasttheme).isattribute(val)||themes.at(pasttheme).isaggregate(val)))))
									)
								{
									_exhandler->raise(Exception::FMTexc::FMTignore,
										val + " at line " + std::to_string(m_line) + " in "+m_location,
										"FMTlandscapeparser::read", __LINE__, __FILE__, m_section);

								}else if(pasttheme>=0)
								{
									themes[pasttheme].push_aggregate_value(aggregatename,val);
								}
								else {
									aggregatenames[std::distance(aggregates.begin(), std::find(aggregates.begin(), aggregates.end(), aggregatename))].push_back(val);
								}
							}
							if (pasttheme==-1&&aggregatenames.at(std::distance(aggregates.begin(), std::find(aggregates.begin(), aggregates.end(), aggregatename))).size() == 0)
							{
								_exhandler->raise(Exception::FMTexc::FMTignore,
									aggregatename + " empty at line " + std::to_string(m_line) + " in " + m_location,
									"FMTlandscapeparser::read",__LINE__, __FILE__, m_section);
							}
						}
						else 
						{
							pasttheme = -1;
							std::vector<std::string> splited = FMTparser::spliter(line, FMTparser::m_SEPARATOR);
							std::string name = "";
							const std::string ltheme = splited[0];
							if (splited.size() > 1)
							{
								splited.erase(splited.begin());
								name = boost::algorithm::join(splited, " ");
								const std::map<std::string, double> indexes = getindexes(name, constants);
								if (!indexes.empty())
								{
									const size_t indexlocation = std::distance(attributes.begin(), std::find(attributes.begin(), attributes.end(), ltheme));
									indexes_values.resize(indexlocation+1);
									indexes_values[indexlocation] = indexes;
									name.clear();
								}
							}
							if (std::find(attributes.begin(), attributes.end(), ltheme) != attributes.end())
							{
								_exhandler->raise(Exception::FMTexc::FMTattribute_redefinition,
									ltheme + " at line " + std::to_string(m_line) + " in " + m_location,
									"FMTlandscapeparser::read", __LINE__, __FILE__, m_section); 
							}
							else 
							{
								attributes.push_back(ltheme);
								if (!indexes_values.empty() && attributes.size() != indexes_values.size())
									{
									indexes_values.resize(attributes.size());
									}
								attributenames.push_back(name);
							}
						}
					}
				}
				if (attributes.size() == 0)
				{
					_exhandler->raise(Exception::FMTexc::FMTempty_theme, "Theme " + std::to_string(id + 1),
						"FMTlandscapeparser::read",__LINE__, __FILE__,m_section);
				}
				
				themes.push_back(Core::FMTtheme(attributes,attributenames,aggregates,aggregatenames,indexes_values, id, start, themename));
				//themes.back().passinobject(*this);
			}
			themes.shrink_to_fit();
			for (Core::FMTtheme& theme : themes)
				{
				theme.buildAttributeLocations();
				}
			}catch(...)
				{
				_exhandler->raisefromcatch(
					"In " + m_location + " at line " + std::to_string(m_line),"FMTlandscapeparser::read ", __LINE__, __FILE__,m_section);
				}
        return themes;
        }

    void FMTlandscapeparser::write(const std::vector<Core::FMTtheme>& themes,const std::string& location) const
        {
		try {
			std::ofstream landscapestream;
			landscapestream.open(location);
			if (landscapestream.is_open())
			{
				for (const Core::FMTtheme& theme : themes)
				{
					landscapestream << std::string(theme);
				}
				landscapestream.close();
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("at " + location,"FMTlandscapeparser::write", __LINE__, __FILE__, m_section);
			}
        }
}
