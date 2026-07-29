/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#include "FMTLandscapeParser.h"
#include "FMTTheme.h"
#include "FMTConstants.h"
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string.hpp>
#ifdef FMTWITHGDAL
	#include "ogrsf_frmts.h"
#endif
#include "FMTExceptionHandler.h"

namespace Parser
{
	const boost::regex FMTLandscapeParser::m_rxPreAttributes = boost::regex(
		"^(\\*ATTRIBUTES)([\\s\\t]*)\\(([\\s\\t]*)_TH(\\d+)([\\s\\t]*)\\)",
		boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex FMTLandscapeParser::m_rxPreAggregate = boost::regex(
		"^(\\*AGGREGATE)([\\s\\t]*)\\(([\\s\\t]*)_(\\w+)([\\s\\t]*)\\)",
		boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex FMTLandscapeParser::m_rxcleanlans = boost::regex(
		"^(\\*THEME)([\\s\\t]*)(([0-9]*$)|([0-9]*)([\\s\\t]*)(.+))|(\\*AGGREGATE)([\\s\\t]*)(\\()([\\s\\t]*)(_TH)(\\d*)([\\s\\t]*)(\\))([\\s\\t]*)(.+)|(\\*AGGREGATE)([\\s\\t]*)([^\\s^\\t]*)|([^\\s^\\t]*)([\\s\\t]*)(.+)", 
		boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex FMTLandscapeParser::m_rxindex = boost::regex(
		"^(_INDEX)(\\()([^\\)]*)(\\))", 
		boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex FMTLandscapeParser::m_rxparameter = boost::regex(
		"^([^=]*)(=)(#.+|[-\\s\\t\\d.]*)", 
		boost::regex_constants::ECMAScript | boost::regex_constants::icase);

	FMTLandscapeParser::FMTLandscapeParser() :
    FMTParser()
        {
		_setSection(Core::FMTsection::Landscape);
        }

    std::map<std::string,double>FMTLandscapeParser::_getIndexes(std::string indexm_line,const Core::FMTConstants& constants)
        {
		std::map<std::string,double>indexes;
		try {
			boost::trim(indexm_line);
			boost::smatch kmatch;
			if (boost::regex_search(indexm_line, kmatch, FMTLandscapeParser::m_rxindex))
			{
				std::vector<std::string>parameters;
				const std::string values = kmatch[3];
				boost::split(parameters, values, boost::is_any_of(","), boost::token_compress_on);
				for (const std::string& parameter : parameters)
				{
					if (boost::regex_search(parameter, kmatch, FMTLandscapeParser::m_rxparameter))
					{
						std::string index_value = std::string(kmatch[3]);
						boost::trim(index_value);
						indexes[std::string(kmatch[1])] = _getNum<double>(index_value, constants);
					}
				}
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch(
				 "for line " + indexm_line,"FMTLandscapeParser::_getIndexes", __LINE__, __FILE__,m_section);
			}
        return indexes;
        }

	void FMTLandscapeParser::_processThemeLine(
		const boost::smatch& kmatch, 
		themeParsingContext& ctx, 
		preDeclarationContext& preContext, 
		std::vector<Core::FMTTheme>& themes, 
		const Core::FMTConstants& constants, 
		size_t& unknownID)
	{
		ctx.pasttheme = -1;

		const std::string theme = std::string(kmatch[4]) + std::string(kmatch[5]);
		const std::string themename_match = std::string(kmatch[7]);

		size_t tempid = 1;
		if (!themename_match.empty() && theme.empty())
		{
			tempid = unknownID;
		}
		else {
			tempid = _getNum<size_t>(theme, constants);
		}
		++unknownID;

		if (tempid > 1)
		{
			if (preContext.declarations.find( std::to_string(tempid - 1)) != preContext.declarations.end())
			{
				ctx.attributes.insert(
					ctx.attributes.begin(),
					preContext.declarations[std::to_string(tempid - 1)].first.begin(),
					preContext.declarations[std::to_string(tempid - 1)].first.end());
				ctx.attributenames.insert(
					ctx.attributenames.begin(),
					preContext.declarations[std::to_string(tempid - 1)].second.begin(),
					preContext.declarations[std::to_string(tempid - 1)].second.end());
				preContext.clearTheme(std::to_string(tempid - 1));
			}

			ctx.stop = ctx.attributes.size();

			if (ctx.attributes.size() == 0)
			{
				_exhandler->raise(Exception::FMTexc::FMTempty_theme,
					"Theme " + std::to_string(ctx.id + 1),
					"FMTLandscapeParser::read", __LINE__, __FILE__,
					m_section);
			}

			themes.push_back(Core::FMTTheme(
				ctx.attributes, ctx.attributenames, ctx.aggregates,
				ctx.aggregatenames, ctx.indexes_values, ctx.id, ctx.start, ctx.themename));

			ctx.start += ctx.stop;
			ctx.clear();
		}

		ctx.id = tempid - 1;
		ctx.themename = themename_match;
	}

	void FMTLandscapeParser::_processAggregateLine(
		const boost::smatch& kmatch, 
		themeParsingContext& ctx, 
		std::vector<Core::FMTTheme>& themes, 
		const Core::FMTConstants& constants)
	{
		ctx.pasttheme = -1;

		std::string targetTheme = std::string(kmatch[13]);
		ctx.aggregatename = std::string(kmatch[20]);

		if (!targetTheme.empty())
		{
			ctx.aggregatename = std::string(kmatch[17]);
			ctx.pasttheme = _getNum<int>(targetTheme, constants) - 1;

			if (static_cast<size_t>(ctx.pasttheme) >= themes.size())
			{
				targetTheme.clear();
				ctx.pasttheme = -1;
			}
		}

		ctx.aggregate_redefinition = false;

		if ((std::find(ctx.aggregates.begin(), ctx.aggregates.end(), ctx.aggregatename) != ctx.aggregates.end()) ||
			(ctx.pasttheme != -1 && themes.at(ctx.pasttheme).isAggregate(ctx.aggregatename)))
		{
			ctx.aggregate_redefinition = true;
			_exhandler->raise(Exception::FMTexc::FMTaggregate_redefinition,
				ctx.aggregatename + " at line " + std::to_string(m_line) + " in " + m_location +
				". The latest definition will be ignored.",
				"FMTLandscapeParser::read", __LINE__, __FILE__, m_section);
		}

		if (!ctx.aggregate_redefinition)
		{
			if (targetTheme.empty())
			{
				ctx.aggregates.push_back(ctx.aggregatename);
				ctx.aggregatenames.push_back(std::vector<std::string>());
			}
			else {
				themes[ctx.pasttheme]._pushAggregate(ctx.aggregatename);
			}
		}
	}

	void FMTLandscapeParser::_processAggregateValueLine(const std::string& line, themeParsingContext& ctx, std::vector<Core::FMTTheme>& themes)
	{
		const std::vector<std::string> splited = FMTParser::spliter(line, FMTParser::m_SEPARATOR);

		for (const std::string& val : splited)
		{
			bool isInvalid = (std::find(ctx.attributes.begin(), ctx.attributes.end(), val) == ctx.attributes.end()) &&
				((std::find(ctx.aggregates.begin(), ctx.aggregates.end(), val) == ctx.aggregates.end()) ||
					(ctx.aggregatename == val)) &&
				(ctx.pasttheme < 0 ||
					(ctx.pasttheme >= 0 && !(themes.at(ctx.pasttheme).isAttribute(val) ||
						themes.at(ctx.pasttheme).isAggregate(val))));

			if (isInvalid)
			{
				_exhandler->raise(Exception::FMTexc::FMTignore,
					val + " at line " + std::to_string(m_line) + " in " + m_location,
					"FMTLandscapeParser::read", __LINE__, __FILE__, m_section);
			}
			else if (ctx.pasttheme >= 0)
			{
				themes[ctx.pasttheme]._pushAggregateValue(ctx.aggregatename, val);
			}
			else {
				ctx.aggregatenames[std::distance(ctx.aggregates.begin(),
					std::find(ctx.aggregates.begin(), ctx.aggregates.end(), ctx.aggregatename))].push_back(val);
			}
		}

		if (ctx.pasttheme == -1)
		{
			size_t aggIndex = std::distance(ctx.aggregates.begin(),
				std::find(ctx.aggregates.begin(), ctx.aggregates.end(), ctx.aggregatename));
			if (ctx.aggregatenames.at(aggIndex).size() == 0)
			{
				_exhandler->raise(Exception::FMTexc::FMTignore,
					ctx.aggregatename + " empty at line " + std::to_string(m_line) + " in " + m_location,
					"FMTLandscapeParser::read", __LINE__, __FILE__, m_section);
			}
		}
	}

	void FMTLandscapeParser::_processAttributeLine(
		const std::string& line, 
		themeParsingContext& ctx, 
		const Core::FMTConstants& constants)
	{
		ctx.pasttheme = -1;

		std::vector<std::string> splited = FMTParser::spliter(line, FMTParser::m_SEPARATOR);
		std::string name = "";
		const std::string ltheme = splited[0];

		if (splited.size() > 1)
		{
			splited.erase(splited.begin());
			name = boost::algorithm::join(splited, " ");

			const std::map<std::string, double> indexes = _getIndexes(name, constants);
			if (!indexes.empty())
			{
				const size_t indexlocation = std::distance(ctx.attributes.begin(),
					std::find(ctx.attributes.begin(), ctx.attributes.end(), ltheme));
				ctx.indexes_values.resize(indexlocation + 1);
				ctx.indexes_values[indexlocation] = indexes;
				name.clear();
			}
		}

		if (std::find(ctx.attributes.begin(), ctx.attributes.end(), ltheme) != ctx.attributes.end())
		{
			_exhandler->raise(Exception::FMTexc::FMTattribute_redefinition,
				ltheme + " at line " + std::to_string(m_line) + " in " + m_location,
				"FMTLandscapeParser::read", __LINE__, __FILE__, m_section);
		}
		else {
			ctx.attributes.push_back(ltheme);
			if (!ctx.indexes_values.empty() && ctx.attributes.size() != ctx.indexes_values.size())
			{
				ctx.indexes_values.resize(ctx.attributes.size());
			}
			ctx.attributenames.push_back(name);
		}
	}

	FMTLandscapeParser::themeParsingContext::themeParsingContext() : id(0), start(0), stop(0), pasttheme(-1), aggregate_redefinition(false)
	{}
	void FMTLandscapeParser::themeParsingContext::clear()
	{
		attributes.clear();
		attributenames.clear();
		aggregatenames.clear();
		aggregatename.clear();
		aggregates.clear();
		themename.clear();
		indexes_values.clear();
	}

	FMTLandscapeParser::preDeclarationContext::preDeclarationContext() : state(ParseState::NORMAL), currentKey("")
	{}
	void FMTLandscapeParser::preDeclarationContext::clearTheme(std::string nameID)
	{
		declarations.erase(nameID);
	}

	bool FMTLandscapeParser::_processPreDeclarationLine(
		const std::string& line,
		preDeclarationContext& context, 
		const Core::FMTConstants& constants)
	{
		boost::smatch preDeclaredMatch;
		if (boost::regex_search(line, preDeclaredMatch, FMTLandscapeParser::m_rxPreAttributes)) {
			context.state = ParseState::IN_PRE_DECLARATION;
			int themeID = _getNum<int>(std::string(preDeclaredMatch[4]), constants);
			context.currentKey = std::to_string(themeID);
			return true;
		}

		if (context.state == ParseState::NORMAL) {
			return false; 
		}

		if (line[0] == '*') {
			context.state = ParseState::NORMAL;
			context.currentKey = "";
			return false;
		}

		std::vector<std::string> splited = FMTParser::spliter(line, FMTParser::m_SEPARATOR);
		if (context.state != ParseState::NORMAL)
		{
			context.declarations[context.currentKey].first.push_back(splited[0]);
			context.declarations[context.currentKey].second.push_back(
				splited.size() > 1 ? splited[1] : std::string());
		}

		return true;
	}

#ifdef FMTWITHGDAL
	std::vector<Core::FMTTheme>FMTLandscapeParser::readRasters(const std::vector<std::string>& locations)
        {
		std::vector<Core::FMTTheme>themes;
		try {
        //GDALAllRegister();
        size_t start = 0;
		size_t id = 0;
        for(const std::string& location : locations)
            {
            GDALDataset* dataset = _getDataset(location);
			const std::vector<std::string>categories = _getCat(dataset);
            themes.push_back(Core::FMTTheme(categories,id,start,""));
			//themes.back().passinobject(*this);
            start+=static_cast<int>(categories.size());
            ++id;
            }
		}
		catch (...)
		{
			_exhandler->printExceptions("", "FMTLandscapeParser::readRasters", __LINE__, __FILE__,m_section);
		}
			
        return themes;
        }

    std::vector<Core::FMTTheme>FMTLandscapeParser::readVectors(const std::string& location)
        {
		std::vector<Core::FMTTheme>themes;
		try {
			//GDALAllRegister();
			GDALDataset* dataset = _getVectorDataset(location);
			OGRLayer * layer = _getLayer(dataset, 0);
			std::map<int, int>themes_fields;
			int age, area, lock;
			_getWSFields(layer, themes_fields, age, area, lock);
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
				themes.push_back(Core::FMTTheme(themeattribute, id, start, ""));
				//themes.back().passinobject(*this);
				++id;
				start += themeattribute.size();
			}
		}catch (...)
			{
				_exhandler->printExceptions("at " + location, "FMTLandscapeParser::readVectors",
					__LINE__, __FILE__, m_section);
			}

        return themes;
        }
#endif

	std::vector<Core::FMTTheme>FMTLandscapeParser::read(
		const Core::FMTConstants& constants,
		const std::string& location)
        {
		std::vector<Core::FMTTheme> themes;
		try {
			std::ifstream landstream(location);
			themeParsingContext ctx;
			preDeclarationContext preContext;
			size_t unknownID = 1;
	
			if (FMTParser::tryOpening(landstream, location))
			{
				std::queue<FMTParser::FMTLineInfo> Lines = FMTParser::_getCleanLinewfor(landstream, themes, constants);
				while (!Lines.empty())
				{
					const std::string line = _getLine(Lines);
					if (!line.empty())
					{
						if (_processPreDeclarationLine(line, preContext, constants)) {
							continue; 
						}

						boost::smatch kmatch;
						boost::regex_search(line, kmatch, FMTLandscapeParser::m_rxcleanlans);
						const std::string potentialtheme = std::string(kmatch[4]) + std::string(kmatch[5]) + std::string(kmatch[7]);
						const std::string aggregate = std::string(kmatch[8]) + std::string(kmatch[18]);
			
						if (!potentialtheme.empty())
						{
							_processThemeLine(kmatch, ctx, preContext, themes, constants, unknownID);
						}
						else if (!aggregate.empty())
						{
							_processAggregateLine(kmatch, ctx, themes, constants);
						}
						else if (!ctx.aggregatename.empty() /* && !aggregate_redefiniton*/)
						{
							_processAggregateValueLine(line, ctx, themes);
						}
						else 
						{
							_processAttributeLine(line, ctx, constants);
						}
					}
				}
				if (ctx.attributes.size() == 0)
				{
					_exhandler->raise(
						Exception::FMTexc::FMTempty_theme, "Theme " + std::to_string(ctx.id + 1),
						"FMTLandscapeParser::read",__LINE__, __FILE__,m_section);
				}
				
				if (!preContext.declarations.empty())
				{
					ctx.attributes.insert(
						ctx.attributes.begin(),
						preContext.declarations[std::to_string(ctx.id + 1)].first.begin(),
						preContext.declarations[std::to_string(ctx.id + 1)].first.end());
					ctx.attributenames.insert(
						ctx.attributenames.begin(),
						preContext.declarations[std::to_string(ctx.id + 1)].second.begin(),
						preContext.declarations[std::to_string(ctx.id + 1)].second.end());
				}

				themes.push_back(Core::FMTTheme(
					ctx.attributes,
					ctx.attributenames,
					ctx.aggregates,
					ctx.aggregatenames,
					ctx.indexes_values, 
					ctx.id, 
					ctx.start, 
					ctx.themename));
			}
			themes.shrink_to_fit();
			for (Core::FMTTheme& theme : themes)
				{
				theme._buildAttributeLocations();
				}
			}catch(...)
				{
				_exhandler->raiseFromCatch(
					"In " + m_location + " at line " + std::to_string(m_line),
					"FMTLandscapeParser::read ", __LINE__, __FILE__,m_section);
				}
        return themes;
        }

    void FMTLandscapeParser::write(const std::vector<Core::FMTTheme>& themes,const std::string& location) const
        {
		try {
			std::ofstream landscapestream;
			landscapestream.open(location);
			if (landscapestream.is_open())
			{
				for (const Core::FMTTheme& theme : themes)
				{
					landscapestream << std::string(theme);
				}
				landscapestream.close();
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("at " + location,"FMTLandscapeParser::write", __LINE__, __FILE__, m_section);
			}
        }
	
}
