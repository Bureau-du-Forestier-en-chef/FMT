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

#include "FMTparser.h"
#include <boost/filesystem.hpp>



namespace Parser
{


Core::FMTwssect FMTparser::from_extension(const std::string& ext) const
    {
	const std::array<std::string, 21>baseextensions = { ".run",".lan",".are",".act",".trn",".yld",".out",".opt",".con",".seq",".lif",
							"._lan","._are","._act","._trn","._yld","._out","._opt","._con","._seq","._lif" };
	const std::string lowercase = boost::to_lower_copy(ext);
	const std::array<std::string, 21>::const_iterator it = std::find(baseextensions.begin(), baseextensions.end(), lowercase);
    const size_t id = (it - baseextensions.begin());
    switch (id)
        {
            case 0 :return Core::FMTwssect::Control;
            case 1 :return Core::FMTwssect::Landscape;
            case 2 :return Core::FMTwssect::Area;
            case 3 :return Core::FMTwssect::Action;
            case 4 :return Core::FMTwssect::Transition;
            case 5 :return Core::FMTwssect::Yield;
            case 6 :return Core::FMTwssect::Outputs;
            case 7 :return Core::FMTwssect::Optimize;
            case 8 :return Core::FMTwssect::Constants;
            case 9 :return Core::FMTwssect::Schedule;
			case 10:return Core::FMTwssect::Lifespan;
			case 11:return Core::FMTwssect::Landscape;
			case 12:return Core::FMTwssect::Area;
			case 13:return Core::FMTwssect::Action;
			case 14:return Core::FMTwssect::Transition;
			case 15:return Core::FMTwssect::Yield;
			case 16:return Core::FMTwssect::Outputs;
			case 17:return Core::FMTwssect::Optimize;
			case 18:return Core::FMTwssect::Constants;
			case 19:return Core::FMTwssect::Schedule;
			case 20:return Core::FMTwssect::Lifespan;
            default:      return Core::FMTwssect::Empty;
        }
    }

FMTparser::FMTparser() : Core::FMTobject(),
		rxnumber("[-+]?([0-9]*\\.[0-9]+|[0-9]+)"),
        rxremovecomment("^(.*?)([;]+.*)"),
        rxvalid("^(?!\\s*$).+"),
		rxinclude("^(\\*INCLUDE)([\\s\\t]*)(.+)"),
          rxfor("^(FOREACH)([\\s\\t]*)([^\\s\\t]*)([\\s\\t]*)(IN)([\\s\\t]*)((\\()(_TH)(\\d*)(\\()([^\\s\\t]*)(\\)\\))|(\\()(_TH)(\\d*)(\\))|(\\()(\\d*)(\\.\\.)(\\d*)(\\))|(\\()([^\\)]*)(\\)))|(\\bFOR\\b)([\\s\\t]*)([^\\:\\=]*)([\\:\\=\\s\\t]*)(\\d*)([\\s\\t]*)(TO)([\\s\\t]*)(\\d*)", std::regex_constants::ECMAScript | std::regex_constants::icase),
		rxend("^(ENDFOR)", std::regex_constants::ECMAScript| std::regex_constants::icase),
        _incomment(false),
         _forvalues(),
		_included(),
		rxayld("^(.+)(\\@YLD\\()([\\s\\t]*)(.+)(\\,)([\\s\\t]*)((#[^\\.]*)|(\\d*.\\d*))(\\.\\.)((#[^\\.]*)|(\\d*.\\d*)|(_MAXAGE))(\\))(.+)|(.+)(\\@YLD\\()([\\s\\t]*)(.+)(\\,)([\\s\\t]*)((#[^\\.]*)|(\\d*)|(\\d*.\\d*))(\\))(.+)", std::regex_constants::ECMAScript | std::regex_constants::icase),
        rxaage("^(.+)(\\@AGE\\()([\\s\\t]*)((#[^\\.]*)|(\\d*)|(\\d*.\\d*))(\\.\\.)((#[^\\.]*)|(\\d*)|(\\d*.\\d*)|(_MAXAGE))(\\))(.+)|(.+)(\\@AGE\\()([\\s\\t]*)((#[^\\.]*)|(\\d*)|(\\d*.\\d*))(\\))(.+)", std::regex_constants::ECMAScript| std::regex_constants::icase),
		rxoperators("([^\\+\\-\\/\\*]*)([\\+\\-\\/\\*]*)", std::regex_constants::icase),
		rxprimary("^([^\\[]*)(\\[)([^\\]]*)(.+)"),
		_constreplacement(0),
        _line(0),
		_comment(),
       _location(),
        rxseparator("([\\s\\t]*)([^\\s\\t]*)")
        {
		#ifdef FMTWITHGDAL
			std::string runtimelocation = getruntimelocation();
			runtimelocation += "\\GDAL_DATA";
			if (!boost::filesystem::is_directory(boost::filesystem::path(runtimelocation)))
				{
				_exhandler->raise(Exception::FMTexc::FMTinvalid_path,
					_section,"Cannot find GDAL_DATA at "+runtimelocation,__LINE__,__FILE__);
				}
			CPLSetConfigOption("GDAL_DATA", runtimelocation.c_str());
		#endif
        }

FMTparser::FMTparser(const FMTparser& rhs):
		Core::FMTobject(rhs),
		rxnumber(rhs.rxnumber),
        rxremovecomment(rhs.rxremovecomment),
        rxvalid(rhs.rxvalid),
		rxinclude(rhs.rxinclude),
        rxfor(rhs.rxfor),
        rxend(rhs.rxend),
         _incomment(rhs._incomment),
        _forvalues(rhs._forvalues),
		_included(rhs._included),
        rxayld(rhs.rxayld),
        rxaage(rhs.rxaage),
		rxoperators(rhs.rxoperators),
		rxprimary(rhs.rxprimary),
        _constreplacement(rhs._constreplacement),
        _line(rhs._line),
		_comment(rhs._comment),
        _location(rhs._location),
        rxseparator(rhs.rxseparator)
    {

    }

FMTparser& FMTparser::operator = (const FMTparser& rhs)
    {
        if (this!=&rhs)
            {
			Core::FMTobject::operator = (rhs);
			rxnumber = rhs.rxnumber;
            rxremovecomment = rhs.rxremovecomment;
            rxvalid = rhs.rxvalid;
			rxinclude = rhs.rxinclude;
            _incomment = rhs._incomment;
            _forvalues = rhs._forvalues;
			_included = rhs._included;
            rxayld = rhs.rxayld;
            rxaage = rhs.rxaage;
			rxoperators = rhs.rxoperators;
            rxfor = rhs.rxfor;
            rxend = rhs.rxend;
			rxprimary = rhs.rxprimary;
            _constreplacement = rhs._constreplacement;
			_comment = rhs._comment;
            _line = rhs._line;
            _location = rhs._location;
            }
    return *this;
    }
#ifdef FMTWITHGDAL

GDALDataset* FMTparser::getdataset(const std::string& location) const
    {
	GDALDataset* data = nullptr;
	if (isvalidfile(location))
		{
		data = (GDALDataset*)GDALOpen(location.c_str(), GA_ReadOnly);
		if (data == nullptr)
			{
			_exhandler->raise(Exception::FMTexc::FMTinvaliddataset,_section,data->GetDescription(), __LINE__, __FILE__);
			}
		}
    return data;
    }

GDALRasterBand* FMTparser::getband(GDALDataset* dataset,int bandid) const
    {
    GDALRasterBand* band = dataset->GetRasterBand(bandid);
    if (band == nullptr)
        {
        _exhandler->raise(Exception::FMTexc::FMTinvalidband,_section,dataset->GetDescription(), __LINE__, __FILE__);
        }
    return band;
    }

GDALRasterBand* FMTparser::createband(GDALDataset* dataset,const std::vector<std::string>& categories,int bandid) const
    {
    GDALRasterBand* band = getband(dataset,bandid);
    band->SetNoDataValue(-9999);
    band->Fill(-9999);
    if (!categories.empty())
        {
        CPLStringList strlist;
        for(const std::string& value : categories)
            {
            strlist.AddString(value.c_str());
            }
        band->SetCategoryNames(strlist);
        }
    band->FlushCache();
    return band;
    }

std::vector<std::string>FMTparser::getcat(GDALDataset* dataset,int bandid) const
    {
    char** meta = dataset->GetMetadata();
    GDALRasterBand* band = getband(dataset,bandid);
    char** names  = band->GetCategoryNames();
	std::vector<std::string>values;
    int id = 0;
    while(names[id])
        {
		boost::to_upper(names[id]);
        values.push_back(names[id]);
        ++id;
        }
    return values;
    }

GDALRasterBand* FMTparser::getoverview(GDALRasterBand* band,int view) const
    {
    GDALRasterBand* overview = band->GetOverview(view);
    if (overview == nullptr)
        {
        _exhandler->raise(Exception::FMTexc::FMTinvalidoverview,_section,band->GetDataset()->GetDescription(), __LINE__, __FILE__);
        }
    return overview;
    }

GDALDataset* FMTparser::getvectordataset(const std::string& location) const
    {
	GDALDataset* dataset = nullptr;
	if (isvalidfile(location))
		{
		dataset = (GDALDataset*) GDALOpenEx(location.c_str(), GDAL_OF_VECTOR, nullptr, nullptr, nullptr );
		if( dataset == nullptr )
			{
			_exhandler->raise(Exception::FMTexc::FMTinvaliddataset,_section,dataset->GetDescription(), __LINE__, __FILE__);
			}
		}
    return dataset;
    }

OGRLayer* FMTparser::getlayer(GDALDataset* dataset,int id) const
    {
    OGRLayer * layer =dataset->GetLayer(id);
    if( layer == nullptr )
        {
        _exhandler->raise(Exception::FMTexc::FMTinvalidlayer,_section,dataset->GetDescription(), __LINE__, __FILE__);
        }
    return layer;
    }

void FMTparser::getWSfields(OGRLayer* layer, std::map<int, int>& themes, int& age, int& area, int& lock, std::string agefield, std::string areafield, std::string lockfield) const
{
	OGRFeatureDefn *fdef = layer->GetLayerDefn();
	std::string capage = agefield;
	std::string caparea = areafield;
	std::string caplock = lockfield;
	boost::to_upper(capage);
	boost::to_upper(caparea);
	boost::to_upper(caplock);
	std::map<int, int>themes_fields;
	age = -1;
	area = -1;
	lock = -1;
	for (int iField = 0; iField < fdef->GetFieldCount(); ++iField)
	{
		OGRFieldDefn* fielddef = fdef->GetFieldDefn(iField);
		std::string fname = fielddef->GetNameRef();
		boost::to_upper(fname);
		if (fname.find("THEME") != std::string::npos)
		{
			fname.erase(0, 5);
			themes[getnum<int>(fname) - 1] = iField;
		}
		else if (fname == caplock)
		{
			lock = iField;
		}
		else if (fname == capage)
		{
			age = iField;
		}
		else if (fname == caparea)
		{
			area = iField;
		}
	}
	if (themes.size() == 0)
	{
		_exhandler->raise(Exception::FMTexc::WSinvalid_maskrange, _section, layer->GetDescription(), __LINE__, __FILE__);
	}
	if (age == -1 && !agefield.empty())
	{
		_exhandler->raise(Exception::FMTexc::FMTmissingfield, _section, agefield + " " + layer->GetDescription(), __LINE__, __FILE__);
	}
	if (area == -1 && !areafield.empty())
	{
		_exhandler->raise(Exception::FMTexc::FMTmissingfield, _section, areafield + " " + layer->GetDescription(), __LINE__, __FILE__);
	}
	if (lock == -1 && !lockfield.empty())
	{
		_exhandler->raise(Exception::FMTexc::FMTmissingfield, _section, lockfield + " " + layer->GetDescription(), __LINE__, __FILE__);
	}
}

#endif
std::string FMTparser::setspec(Core::FMTwssect section, Core::FMTwskwor key,const Core::FMTyields& ylds,const Core::FMTconstants& constants,Core::FMTspec& spec, const std::string& line)
    {
	std::smatch kmatch;
	std::string rest = "";
    if (std::regex_search(line,kmatch,FMTparser::rxayld))
        {
		const std::string yld = std::string(kmatch[4]) + std::string(kmatch[20]);
        isyld(ylds,yld,section);
		const std::string singlebound = std::string(kmatch[21]) + std::string(kmatch[22]) + std::string(kmatch[23]);
        double upperbound = std::numeric_limits<double>::max();
        double lowerbound = std::numeric_limits<double>::min();
        if (singlebound.empty())
            {
			if (std::string(kmatch[14]).empty())
				{
				const std::string strupper = std::string(kmatch[12]) + std::string(kmatch[13]);
				upperbound = getnum<double>(strupper, constants);
				}
			const std::string strlower = std::string(kmatch[8]) + std::string(kmatch[9]);
            lowerbound = getnum<double>(strlower,constants);
            }else{
            lowerbound = getnum<double>(singlebound,constants);
            }
			spec.addbounds(Core::FMTyldbounds(section, key, yld, upperbound, lowerbound));
		rest = " "+ std::string(kmatch[1]) + std::string(kmatch[16]) + std::string(kmatch[28]);
		}else{
        rest = line;
        }

    if (std::regex_search(rest,kmatch,FMTparser::rxaage))
        {
		std::string singlebound = std::string(kmatch[20]) + std::string(kmatch[21])+ std::string(kmatch[22]);
        int upperbound = std::numeric_limits<int>::max();
        int lowerbound = std::numeric_limits<int>::min();
        if (singlebound.empty())
            {
            if(std::string(kmatch[13]).empty())
                {
				std::string strupper = std::string(kmatch[10])+ std::string(kmatch[11]) + std::string(kmatch[12]);
                upperbound = getnum<int>(strupper,constants);
                }
			std::string strlower = std::string(kmatch[5])+ std::string(kmatch[6]) + std::string(kmatch[7]);
            lowerbound = getnum<int>(strlower,constants);
            }else{
            lowerbound = getnum<int>(singlebound,constants);
            }
            spec.addbounds(Core::FMTagebounds(section,key,upperbound,lowerbound));
        rest = " "+ std::string(kmatch[1]) + std::string(kmatch[15]) + std::string(kmatch[16]) + std::string(kmatch[24]);
        }
    return rest;
    }

bool FMTparser::isact(Core::FMTwssect section,const std::vector<Core::FMTaction>& actions, std::string action) const
    {
    if (std::find_if(actions.begin(),actions.end(), Core::FMTactioncomparator(action,true))==actions.end())
        {
        _exhandler->raise(Exception::FMTexc::WSundefined_action,section,action+" at line " + std::to_string(_line), __LINE__, __FILE__);
        return false;
        }else{
        return true;
        }
    }



bool FMTparser::isyld(const Core::FMTyields& ylds,const std::string& value, Core::FMTwssect section) const
    {
    if (ylds.isyld(value))
        {
        return true;
        }
     _exhandler->raise(Exception::FMTexc::WSinvalid_yield,section,value+" at line " + std::to_string(_line), __LINE__, __FILE__);
     return false;
    }




bool FMTparser::tryopening(const std::ifstream& stream, const std::string& location)
        {
        _location = location;
		std::string extension = boost::filesystem::extension(location);
        transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        this->setsection(from_extension(extension));
        if (!stream.is_open())
            {
            _exhandler->raise(Exception::FMTexc::FMTinvalid_path,_section,location, __LINE__, __FILE__);
            return false;
            }
        return true;
        }

bool FMTparser::tryopening(const std::ofstream& stream, const std::string& location)
        {
        _location = location;
        if (!stream.is_open())
            {
            _exhandler->raise(Exception::FMTexc::FMTinvalid_path,_section,location, __LINE__, __FILE__);
            return false;
            }
        return true;
        }

bool FMTparser::isvalidfile(const std::string& location) const
	{
	boost::filesystem::path pathObj(location);
	std::string extension = boost::filesystem::extension(location);
	transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
	if (!boost::filesystem::exists(pathObj) || !boost::filesystem::is_regular_file(pathObj))
		{
		_exhandler->raise(Exception::FMTexc::FMTinvalid_path, _section, location, __LINE__, __FILE__);
		return false;
		}
	return true;
	}

bool FMTparser::isnum(const std::string& value) const
    {
	return std::regex_match(value,rxnumber);
    }



std::vector<std::string>FMTparser::regexloop(std::regex& cutregex, std::string& str) const
        {
		std::vector<std::string>result;
        auto els_begin = std::sregex_iterator(str.begin(), str.end(), cutregex);
        auto els_end = std::sregex_iterator();
        for (std::sregex_iterator i = els_begin; i != els_end; ++i)
            {
			std::smatch match = *i;
			std::string match_str = match.str();
            result.push_back(match_str);
            }
        return result;
        }
    bool FMTparser::isvalid(std::string& line) const
        {
        return std::regex_match(line,rxvalid);
        }
    void FMTparser::clearcomments(std::string& line)
        {
		std::smatch kmatch;
        if (std::regex_search(line,kmatch,rxremovecomment))
            {
			_comment = std::string(kmatch[2]);
			boost::to_upper(_comment);
			line = std::string(kmatch[1]);
			}
        }
	std::string FMTparser::upper(const std::string& lowercases) const
        {
		std::locale loc;
		std::string uppercases;
        for (std::string::size_type i=0;i<lowercases.size(); ++i)
            {
            uppercases+=toupper(lowercases[i],loc);
            }
        return uppercases;
        }
	std::vector<std::string>FMTparser::spliter(std::string strmask, std::regex& xspliter) const
        {
		std::smatch kmatch;
		std::string value;
		std::vector<std::string>vecmask;
        size_t endsize;
        while(!strmask.empty())
            {
			std::regex_search(strmask,kmatch,xspliter);
            value = std::string(kmatch[2]);
            if (isvalid(value))
                {
                vecmask.push_back(value);
                }
            endsize = (std::string(kmatch[0]).size());
            strmask.erase(strmask.begin(),strmask.begin()+endsize);
            }
        return vecmask;
        }
	std::string FMTparser::getcleanline(std::ifstream& stream)
        {
        ++_line;
		std::string newline;
        if (safeGetline(stream, newline))
            {
			_comment.clear();
            clearcomments(newline);
			std::string fullline = newline;
            newline = "";
            for(int loc = 0; loc < static_cast<int>(fullline.size()); ++loc)
                {
                char value = fullline[loc];
                if(_incomment)
                    {
                    if (value=='}')
                        {
                        _incomment = false;
                        }
                    }else{
                    if (value=='{')
                        {
                        _incomment = true;
                        }else{
                        newline+=toupper(value);
                        }
                    }
                }
            }else{
            stream.close();
            }
        boost::trim(newline);
        return newline;
        }

bool FMTparser::getforloops(std::string& line,const std::vector<Core::FMTtheme>& themes, const Core::FMTconstants& cons, std::vector<std::string>& allvalues, std::string& target)
	{
	std::smatch kmatch;
	if (std::regex_search(line, kmatch, rxfor))
	{
		target = std::string(kmatch[3]);
		if (!std::string(kmatch[24]).empty())
		{
			std::string val = std::string(kmatch[24]);
			boost::trim_if(val, boost::is_any_of(FMT_STR_SEPARATOR));
			const std::string forstrsep = FMT_STR_SEPARATOR + std::string(",");
			boost::split(allvalues, val, boost::is_any_of(forstrsep), boost::token_compress_on);
			if (val.find("..")!= std::string::npos)
				{
				std::vector<std::string>newvalues;
				for (const std::string& oldvalue: allvalues)
					{
					if (oldvalue.find("..") != std::string::npos)
						{
						std::vector<std::string>lowerNupper;
						boost::split(lowerNupper, oldvalue, boost::is_any_of(".."), boost::token_compress_on);
						const int lower = getnum<int>(lowerNupper.at(0), cons);
						const int upper = getnum<int>(lowerNupper.at(1), cons);
						for (int id = lower; id <= upper; ++id)
							{
							newvalues.push_back(std::to_string(id));
							}
						
					}else {
						newvalues.push_back(oldvalue);
						}
					}
				allvalues = newvalues;
				}
		}
		else if (!std::string(kmatch[16]).empty())
		{
			const int theme = getnum<int>(std::string(kmatch[16])) - 1;
			std::string att = "?";
			allvalues = themes[theme].getattributes(att);
		}
		else if (!std::string(kmatch[10]).empty())
		{
			const int theme = getnum<int>(std::string(kmatch[10])) - 1;
			std::string aggregate = kmatch[12];
			allvalues = themes[theme].getattributes(aggregate,true);
		}
		else if (!std::string(kmatch[19]).empty() || !std::string(kmatch[26]).empty())
		{
			if (!std::string(kmatch[28]).empty())
				{
				target = kmatch[28];
				}
			const std::string slower = std::string(kmatch[19]) + std::string(kmatch[30]);
			const std::string supper = std::string(kmatch[21]) + std::string(kmatch[34]);
			const int lower = getnum<int>(slower, cons);
			const int upper = getnum<int>(supper, cons);
			for (int id = lower; id <= upper; ++id)
			{
				allvalues.push_back(std::to_string(id));
			}
		}
		return true;
	}
	return false;
	}

std::queue<std::string> FMTparser::tryinclude(const std::string& line, const std::vector<Core::FMTtheme>& themes, const Core::FMTconstants& cons)
	{
	std::smatch kmatch;
	std::queue<std::string>included_lines;
	if (std::regex_search(line, kmatch, FMTparser::rxinclude))
		{
		std::string location = kmatch[3];
		if (boost::starts_with(location, "."))
			{
			const boost::filesystem::path l_ppath(_location);
			const boost::filesystem::path parent_path = l_ppath.parent_path();
			location.erase(0, 1);
			const boost::filesystem::path full_path = parent_path / location;
			location = full_path.string();
			}
		FMTparser newparser;
		std::ifstream newstream(location);
		if (newparser.tryopening(newstream, location))
			{
			while (newstream.is_open())
				{
				const std::string newline = newparser.getcleanlinewfor(newstream,themes,cons);
				if (!newline.empty())
					{
					included_lines.push(newline);
					}
				}
			}
		}
	return included_lines;
	}

std::string FMTparser::returninclude(const std::string& line, const std::vector<Core::FMTtheme>& themes, const Core::FMTconstants& cons)
	{
	std::string thereturn = "";
	if (!_included.empty())
		{
		thereturn = _included.front();
		_included.pop();
		}else{
			std::queue<std::string>inlines = tryinclude(line, themes, cons);
			if (!inlines.empty())
				{
				thereturn = inlines.front();
				inlines.pop();
				_included = inlines;
				}else {
				thereturn = line;
				}

			}
	return thereturn;
	}

std::string FMTparser::getcleanlinewfor(std::ifstream& stream,const std::vector<Core::FMTtheme>& themes,const Core::FMTconstants& cons)
    {
	std::smatch kmatch;
	std::string line;
	while (!_included.empty())
		{
		line = _included.front();
		_included.pop();
		return line;
		}
	if(_forvalues.empty())
		{
		std::map<std::string, std::vector<std::string>>loops_replaces;
		std::map<std::string, std::pair<int,int>>forlocation;
		int lineid = 0;
		std::stack<std::string>keys;
		std::vector<std::pair<int, std::string>>Sequence;
		std::vector<std::string>ordered_keys;
		do{

			line = getcleanline(stream);
			std::vector<std::string>allvalues;
			std::string target;
			bool gotfor = getforloops(line, themes, cons, allvalues, target);
			if (!gotfor && line.find("FOREACH")!= std::string::npos)
				{

				do {
					line += getcleanline(stream);
				} while (line.find(")") == std::string::npos);
				gotfor = getforloops(line, themes, cons, allvalues, target);
				}
			if (gotfor)
				{
				loops_replaces[target] = allvalues;
				forlocation[target] = std::pair<int,int>(lineid,0);
				keys.push(target);
				}else if(keys.empty())
					{
					return returninclude(line,themes,cons);
					}else if(std::regex_search(line,kmatch,FMTparser::rxend))
							{
							std::string endfor = keys.top();
							keys.pop();
							forlocation[endfor].second = lineid;
							ordered_keys.push_back(endfor);
					}else if (isvalid(line)){
					Sequence.push_back(std::pair<int, std::string>(lineid,line));
					++lineid;
					}
		} while (!keys.empty());
		for (const std::string& key : ordered_keys)
			{
			std::vector<std::pair<int, std::string>>frontsequence;
			std::vector<std::pair<int, std::string>>backsequence;
			std::vector<std::pair<int, std::string>>process;
			for (std::pair<int, std::string> tline : Sequence)
				{
				if (tline.first < forlocation[key].first)
					{
					frontsequence.push_back(tline);
				}else if (tline.first >= forlocation[key].first && tline.first < forlocation[key].second)
					{
					process.push_back(tline);
					}else{
					backsequence.push_back(tline);
					}
				}
			std::vector<std::pair<int, std::string>>midsequence;
			for (const std::string& value : loops_replaces[key])
				{
				for (std::pair<int, std::string> tline : process)
					{
					midsequence.push_back(std::pair<int, std::string>(tline.first, std::regex_replace(tline.second, std::regex(key), value)));
					}
				}
			Sequence.clear();
			Sequence.insert(Sequence.end(), frontsequence.begin(), frontsequence.end());
			Sequence.insert(Sequence.end(), midsequence.begin(), midsequence.end());
			Sequence.insert(Sequence.end(), backsequence.begin(), backsequence.end());
			}
			for (const std::pair<int, std::string>& nline : Sequence)
				{
				_forvalues.push(nline.second);
				}
			}

	 while(!_forvalues.empty())
		{
		line = _forvalues.front();
		_forvalues.pop();
		return returninclude(line, themes, cons);
		}

	return returninclude(line, themes, cons);
    }

std::map<Core::FMTwssect, std::string> FMTparser::getprimary(const std::string& primarylocation)
	{
		std::ifstream primarystream(primarylocation);
		std::map<Core::FMTwssect, std::string>targets;
		const boost::filesystem::path primary_path(primarylocation);
		if (FMTparser::tryopening(primarystream, primarylocation))
		{
			while (primarystream.is_open())
			{
				std::string line = getcleanline(primarystream);
				if (isvalid(line))
				{
					std::smatch kmatch;
					if (std::regex_search(line, kmatch, rxprimary))
					{
						const std::string file_name = std::string(kmatch[3]);
						const boost::filesystem::path file_path(file_name);
						std::string extension = boost::filesystem::extension(file_path.string());
						boost::to_lower(extension);
						const Core::FMTwssect section = from_extension(extension);
						targets[section] = (primary_path.parent_path() / file_path).string();
					}
				}
			}
		}
		if (targets.find(Core::FMTwssect::Constants)==targets.end())
			{
			targets[Core::FMTwssect::Constants] = "";
			}
		return targets;
	}

std::array<std::string, 5>FMTparser::getbaseoperators() const
	{
	return std::array<std::string, 5>({ "=", "<=", ">=", "<", ">" });
	}

std::vector<std::string> FMTparser::sameas(const std::string& allset) const
        {
		std::vector<std::string>all_elements;
		std::string separator = "_SAMEAS";
        if (allset.find(separator)!= std::string::npos)
            {
			std::string realname = allset.substr(0, allset.find(separator));
            boost::trim(realname);
            all_elements.push_back(realname);
			std::string sameasstr = allset.substr(allset.find(separator)+separator.size());
            boost::trim(sameasstr);
            all_elements.push_back(sameasstr);
            }else{
			std::string realname = allset;
            boost::trim(realname);
            all_elements.push_back(realname);
            }
        return all_elements;
        }

std::vector<std::vector<std::string>>FMTparser::readcsv(const std::string& location,const char& separator)
	{
	std::vector<std::vector<std::string>>lines;
	std::string regexstring = "(["+ std::string(1,separator) +"]*)([^"+ std::string(1,separator) +"]*)";
	std::regex csvsplitregex(regexstring);
	if (!location.empty())
		{
			std::ifstream csvstream(location);
			std::string line;
			std::vector<std::string>splitted;
			if (FMTparser::tryopening(csvstream, location))
				{
				//bool inactualdevs = false;
				while (csvstream.is_open())
					{
					if (FMTparser::safeGetline(csvstream, line))
						{
						boost::trim(line);
						if (!line.empty())
							{
							lines.push_back(FMTparser::spliter(line, csvsplitregex));
							}
						}
					}
				}
		}
	return lines;
	}




std::istream& FMTparser::safeGetline(std::istream& is, std::string& t) const
	{
	t.clear();
	std::istream::sentry se(is, true);
	std::streambuf* sb = is.rdbuf();
	for (;;) {
		int c = sb->sbumpc();
		switch (c) {
		case '\n':
			return is;
		case '\r':
			if (sb->sgetc() == '\n')
				sb->sbumpc();
			return is;
		case std::streambuf::traits_type::eof():
			if (t.empty())
				is.setstate(std::ios::eofbit);
			return is;
		default:
			t += (char)c;
		}
	}

	}

}
