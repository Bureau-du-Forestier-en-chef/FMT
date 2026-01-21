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
	const boost::regex FMTlandscapeparser::rxPreAttributes = boost::regex(
		"^(\\*ATTRIBUTES)([\\s\\t]*)\\(([\\s\\t]*)_TH(\\d+)([\\s\\t]*)\\)",
		boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex FMTlandscapeparser::rxPreAggregate = boost::regex(
		"^(\\*AGGREGATE)([\\s\\t]*)\\(([\\s\\t]*)_(\\w+)([\\s\\t]*)\\)",
		boost::regex_constants::ECMAScript | boost::regex_constants::icase);
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

	void FMTlandscapeparser::ProcessThemeLine(
		const boost::smatch& kmatch, 
		ThemeParsingContext& ctx, 
		PreDeclarationContext& preContext, 
		std::vector<Core::FMTtheme>& themes, 
		const Core::FMTconstants& constants, 
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
			tempid = getNum<size_t>(theme, constants);
		}
		++unknownID;

		if (tempid > 1)
		{
			if (preContext.declarations.find("THEME_" + std::to_string(tempid - 1)) != preContext.declarations.end())
			{
				ctx.attributes.insert(
					ctx.attributes.begin(),
					preContext.declarations["THEME_" + std::to_string(tempid - 1)].first.begin(),
					preContext.declarations["THEME_" + std::to_string(tempid - 1)].first.end());
				ctx.attributenames.insert(
					ctx.attributenames.begin(),
					preContext.declarations["THEME_" + std::to_string(tempid - 1)].second.begin(),
					preContext.declarations["THEME_" + std::to_string(tempid - 1)].second.end());
				preContext.clearTheme(std::to_string(tempid - 1));
			}

			ctx.stop = ctx.attributes.size();

			if (ctx.attributes.size() == 0)
			{
				_exhandler->raise(Exception::FMTexc::FMTempty_theme,
					"Theme " + std::to_string(ctx.id + 1),
					"FMTlandscapeparser::read", __LINE__, __FILE__,
					m_section);
			}

			themes.push_back(Core::FMTtheme(
				ctx.attributes, ctx.attributenames, ctx.aggregates,
				ctx.aggregatenames, ctx.indexes_values, ctx.id, ctx.start, ctx.themename));

			ctx.start += ctx.stop;
			ctx.clear();
		}

		ctx.id = tempid - 1;
		ctx.themename = themename_match;
	}

	void FMTlandscapeparser::ProcessAggregateLine(
		const boost::smatch& kmatch, 
		ThemeParsingContext& ctx, 
		std::vector<Core::FMTtheme>& themes, 
		const Core::FMTconstants& constants)
	{
		ctx.pasttheme = -1;

		std::string targettheme = std::string(kmatch[13]);
		ctx.aggregatename = std::string(kmatch[20]);

		if (!targettheme.empty())
		{
			ctx.aggregatename = std::string(kmatch[17]);
			ctx.pasttheme = getNum<int>(targettheme, constants) - 1;

			if (static_cast<size_t>(ctx.pasttheme) >= themes.size())
			{
				targettheme.clear();
				ctx.pasttheme = -1;
			}
		}

		ctx.aggregate_redefinition = false;

		if ((std::find(ctx.aggregates.begin(), ctx.aggregates.end(), ctx.aggregatename) != ctx.aggregates.end()) ||
			(ctx.pasttheme != -1 && themes.at(ctx.pasttheme).isaggregate(ctx.aggregatename)))
		{
			ctx.aggregate_redefinition = true;
			_exhandler->raise(Exception::FMTexc::FMTaggregate_redefinition,
				ctx.aggregatename + " at line " + std::to_string(m_line) + " in " + m_location +
				". The latest definition will be ignored.",
				"FMTlandscapeparser::read", __LINE__, __FILE__, m_section);
		}

		if (!ctx.aggregate_redefinition)
		{
			if (targettheme.empty())
			{
				ctx.aggregates.push_back(ctx.aggregatename);
				ctx.aggregatenames.push_back(std::vector<std::string>());
			}
			else {
				themes[ctx.pasttheme].push_aggregate(ctx.aggregatename);
			}
		}
	}

	void FMTlandscapeparser::ProcessAggregateValueLine(const std::string& line, ThemeParsingContext& ctx, std::vector<Core::FMTtheme>& themes)
	{
		const std::vector<std::string> splited = FMTparser::spliter(line, FMTparser::m_SEPARATOR);

		for (const std::string& val : splited)
		{
			bool isInvalid = (std::find(ctx.attributes.begin(), ctx.attributes.end(), val) == ctx.attributes.end()) &&
				((std::find(ctx.aggregates.begin(), ctx.aggregates.end(), val) == ctx.aggregates.end()) ||
					(ctx.aggregatename == val)) &&
				(ctx.pasttheme < 0 ||
					(ctx.pasttheme >= 0 && !(themes.at(ctx.pasttheme).isattribute(val) ||
						themes.at(ctx.pasttheme).isaggregate(val))));

			if (isInvalid)
			{
				_exhandler->raise(Exception::FMTexc::FMTignore,
					val + " at line " + std::to_string(m_line) + " in " + m_location,
					"FMTlandscapeparser::read", __LINE__, __FILE__, m_section);
			}
			else if (ctx.pasttheme >= 0)
			{
				themes[ctx.pasttheme].push_aggregate_value(ctx.aggregatename, val);
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
					"FMTlandscapeparser::read", __LINE__, __FILE__, m_section);
			}
		}
	}

	void FMTlandscapeparser::ProcessAttributeLine(
		const std::string& line, 
		ThemeParsingContext& ctx, 
		const Core::FMTconstants& constants)
	{
		ctx.pasttheme = -1;

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
				"FMTlandscapeparser::read", __LINE__, __FILE__, m_section);
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

	FMTlandscapeparser::ThemeParsingContext::ThemeParsingContext() : id(0), start(0), stop(0), pasttheme(-1), aggregate_redefinition(false)
	{}
	void FMTlandscapeparser::ThemeParsingContext::clear()
	{
		attributes.clear();
		attributenames.clear();
		aggregatenames.clear();
		aggregatename.clear();
		aggregates.clear();
		themename.clear();
		indexes_values.clear();
	}

	FMTlandscapeparser::PreDeclarationContext::PreDeclarationContext() : state(ParseState::NORMAL), currentKey("")
	{}
	void FMTlandscapeparser::PreDeclarationContext::clearTheme(std::string nameID)
	{
		declarations.erase(nameID);
	}

	bool FMTlandscapeparser::ProcessPreDeclarationLine(
		const std::string& line,
		PreDeclarationContext& context, 
		const Core::FMTconstants& constants)
	{
		boost::smatch preDeclaredMatch;
		if (boost::regex_search(line, preDeclaredMatch, FMTlandscapeparser::rxPreAttributes)) {
			context.state = ParseState::IN_PRE_DECLARATION;
			int themeID = getNum<int>(std::string(preDeclaredMatch[4]), constants);
			context.currentKey = "THEME_" + std::to_string(themeID);
			return true;
		}
		
		if (boost::regex_search(line, preDeclaredMatch, FMTlandscapeparser::rxPreAggregate)) {
			context.state = ParseState::IN_PRE_DECLARATION;
			context.currentKey = "AGGREGATE_" + std::string(preDeclaredMatch[4]);
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

		std::vector<std::string> splited = FMTparser::spliter(line, FMTparser::m_SEPARATOR);
		if (context.state != ParseState::NORMAL)
		{
			context.declarations[context.currentKey].first.push_back(splited[0]);
			context.declarations[context.currentKey].second.push_back(
				splited.size() > 1 ? splited[1] : std::string());
		}

		return true;
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

	std::vector<Core::FMTtheme>FMTlandscapeparser::read(
		const Core::FMTconstants& constants,
		const std::string& location)
        {
		std::vector<Core::FMTtheme> themes;
		try {
			std::ifstream landstream(location);
			ThemeParsingContext ctx;
			PreDeclarationContext preContext;
			size_t unknownID = 1;
	
			if (FMTparser::tryOpening(landstream, location))
			{
				std::queue<FMTparser::FMTLineInfo> Lines = FMTparser::GetCleanLinewfor(landstream, themes, constants);
				while (!Lines.empty())
				{
					const std::string line = GetLine(Lines);
					if (!line.empty())
					{
						if (ProcessPreDeclarationLine(line, preContext, constants)) {
							continue; 
						}

						boost::smatch kmatch;
						boost::regex_search(line, kmatch, FMTlandscapeparser::rxcleanlans);
						const std::string potentialtheme = std::string(kmatch[4]) + std::string(kmatch[5]) + std::string(kmatch[7]);
						const std::string aggregate = std::string(kmatch[8]) + std::string(kmatch[18]);
			
						if (!potentialtheme.empty())
						{
							ProcessThemeLine(kmatch, ctx, preContext, themes, constants, unknownID);
						}
						else if (!aggregate.empty())
						{
							ProcessAggregateLine(kmatch, ctx, themes, constants);
						}
						else if (!ctx.aggregatename.empty() /* && !aggregate_redefiniton*/)
						{
							ProcessAggregateValueLine(line, ctx, themes);
						}
						else 
						{
							ProcessAttributeLine(line, ctx, constants);
						}
					}
				}
				if (ctx.attributes.size() == 0)
				{
					_exhandler->raise(
						Exception::FMTexc::FMTempty_theme, "Theme " + std::to_string(ctx.id + 1),
						"FMTlandscapeparser::read",__LINE__, __FILE__,m_section);
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

				themes.push_back(Core::FMTtheme(
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
			for (Core::FMTtheme& theme : themes)
				{
				theme.buildAttributeLocations();
				}
			}catch(...)
				{
				_exhandler->raisefromcatch(
					"In " + m_location + " at line " + std::to_string(m_line),
					"FMTlandscapeparser::read ", __LINE__, __FILE__,m_section);
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
