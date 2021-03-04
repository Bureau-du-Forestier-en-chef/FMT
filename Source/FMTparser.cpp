/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTparser.h"
#include <boost/filesystem.hpp>
#include <boost/locale.hpp>



namespace Parser
{


Core::FMTsection FMTparser::from_extension(const std::string& ext) const
    {
	try {
		const std::array<std::string, 21>baseextensions = { ".run",".lan",".are",".act",".trn",".yld",".out",".opt",".con",".seq",".lif",
								"._lan","._are","._act","._trn","._yld","._out","._opt","._con","._seq","._lif" };
		const std::string lowercase = boost::to_lower_copy(ext);
		const std::array<std::string, 21>::const_iterator it = std::find(baseextensions.begin(), baseextensions.end(), lowercase);
		const size_t id = (it - baseextensions.begin());
		switch (id)
		{
		case 0:return Core::FMTsection::Control;
		case 1:return Core::FMTsection::Landscape;
		case 2:return Core::FMTsection::Area;
		case 3:return Core::FMTsection::Action;
		case 4:return Core::FMTsection::Transition;
		case 5:return Core::FMTsection::Yield;
		case 6:return Core::FMTsection::Outputs;
		case 7:return Core::FMTsection::Optimize;
		case 8:return Core::FMTsection::Constants;
		case 9:return Core::FMTsection::Schedule;
		case 10:return Core::FMTsection::Lifespan;
		case 11:return Core::FMTsection::Landscape;
		case 12:return Core::FMTsection::Area;
		case 13:return Core::FMTsection::Action;
		case 14:return Core::FMTsection::Transition;
		case 15:return Core::FMTsection::Yield;
		case 16:return Core::FMTsection::Outputs;
		case 17:return Core::FMTsection::Optimize;
		case 18:return Core::FMTsection::Constants;
		case 19:return Core::FMTsection::Schedule;
		case 20:return Core::FMTsection::Lifespan;
		default:      return Core::FMTsection::Empty;
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTparser::from_extension", __LINE__, __FILE__);
	}
	return Core::FMTsection::Empty;
    }

FMTparser::FMTparser() : Core::FMTobject(),
		rxvectortheme("^(THEME)([\\d]*)$"),
		rxnumber("-?[\\d.]+(?:E-?\\d+)?"),
        rxremovecomment("^(.*?)([;]+.*)"),
        rxvalid("^(?!\\s*$).+"),
		rxinclude("^(\\*INCLUDE)([\\s\\t]*)(.+)"),
		rxfor("^(FOREACH)([\\s\\t]*)([^\\s\\t]*)([\\s\\t]*)(IN)([\\s\\t]*)((\\([\\s\\t]*)(_TH)(\\d*)([\\s\\t]*\\([\\s\\t]*)([^\\s\\t]*)([\\s\\t]*\\)[\\s\\t]*\\))|(\\([\\s\\t]*)(_TH)(\\d*)([\\s\\t]*\\))|(\\()(\\d*)(\\.\\.)(\\d*)(\\))|(\\()([^\\)]*)(\\)))|(\\bFOR\\b)([\\s\\t]*)([^\\:\\=]*)([\\:\\=\\s\\t]*)(\\d*)([\\s\\t]*)(TO)([\\s\\t]*)(\\d*)", std::regex_constants::ECMAScript | std::regex_constants::icase),
		rxend("^(ENDFOR)", std::regex_constants::ECMAScript| std::regex_constants::icase),
        _incomment(false),
         _forvalues(),
		_included(),
		rxayld("^(.+)(\\@YLD\\()([\\s\\t]*)(.+)(\\,)([\\s\\t]*)((#[^\\.]*)|([-]*\\d*.[-]*\\d*))(\\.\\.)((#[^\\.]*)|([-]*\\d*.[-]*\\d*)|(_MAXAGE))(\\))(.+)|(.+)(\\@YLD\\()([\\s\\t]*)(.+)(\\,)([\\s\\t]*)((#[^\\.]*)|([-]*\\d*)|([-]*\\d*.[-]*\\d*))(\\))(.+)", std::regex_constants::ECMAScript | std::regex_constants::icase),
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
					"Cannot find GDAL_DATA at "+runtimelocation,"FMTparser::FMTparser()",__LINE__,__FILE__, _section);
				}
			CPLSetConfigOption("GDAL_DATA", runtimelocation.c_str());
		#endif
        }

FMTparser::FMTparser(const FMTparser& rhs):
		Core::FMTobject(rhs),
		rxvectortheme(rhs.rxvectortheme),
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
			rxvectortheme = rhs.rxvectortheme;
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
	try{
	if (isvalidfile(location))
		{
		data = (GDALDataset*)GDALOpen(location.c_str(), GA_ReadOnly);
		if (data == nullptr)
			{
			_exhandler->raise(Exception::FMTexc::FMTinvaliddataset,
				data->GetDescription(),"FMTparser::getdataset", __LINE__, __FILE__,_section);
			}
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTparser::getdataset", __LINE__, __FILE__);
	}
    return data;
    }

GDALRasterBand* FMTparser::getband(GDALDataset* dataset,int bandid) const
    {
	GDALRasterBand* band = nullptr;
	try{
	band = dataset->GetRasterBand(bandid);
    if (band == nullptr)
        {
        _exhandler->raise(Exception::FMTexc::FMTinvalidband,
			dataset->GetDescription(),"FMTparser::getband", __LINE__, __FILE__, _section);
        }
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTparser::getband", __LINE__, __FILE__);
	}
    return band;
    }

GDALRasterBand* FMTparser::createband(GDALDataset* dataset,const std::vector<std::string>& categories,int bandid) const
    {
    GDALRasterBand* band = getband(dataset,bandid);
	try {
		band->SetNoDataValue(-9999);
		band->Fill(-9999);
		if (!categories.empty())
		{
			CPLStringList strlist;
			for (const std::string& value : categories)
			{
				strlist.AddString(value.c_str());
			}
			band->SetCategoryNames(strlist);
		}
		band->FlushCache();
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTparser::createband", __LINE__, __FILE__, _section);
		}
    return band;
    }

std::vector<std::string>FMTparser::getcat(GDALDataset* dataset,int bandid) const
    {
	std::vector<std::string>values;
	try {
		char** meta = dataset->GetMetadata();
		GDALRasterBand* band = getband(dataset,bandid);
		char** names  = band->GetCategoryNames();
		int id = 0;
		while(names[id])
			{
			boost::to_upper(names[id]);
			values.push_back(names[id]);
			++id;
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTparser::getcat", __LINE__, __FILE__, _section);
		}
    return values;
    }

GDALRasterBand* FMTparser::getoverview(GDALRasterBand* band,int view) const
    {
	GDALRasterBand* overview;
	try{
	overview = band->GetOverview(view);
    if (overview == nullptr)
        {
        _exhandler->raise(Exception::FMTexc::FMTinvalidoverview,band->GetDataset()->GetDescription(),
			"FMTparser::getoverview",__LINE__, __FILE__, _section);
        }
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTparser::getoverview", __LINE__, __FILE__);
		}
    return overview;
    }

GDALDataset* FMTparser::getvectordataset(const std::string& location) const
    {
	GDALDataset* dataset = nullptr;
	try {
		if (isvalidfile(location))
		{
			dataset = (GDALDataset*)GDALOpenEx(location.c_str(), GDAL_OF_VECTOR, nullptr, nullptr, nullptr);
			if (dataset == nullptr)
			{
				_exhandler->raise(Exception::FMTexc::FMTinvaliddataset,
					dataset->GetDescription(),"FMTparser::getvectordataset", __LINE__, __FILE__, _section);
			}
		}
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTparser::getvectordataset", __LINE__, __FILE__, _section);
		}
    return dataset;
    }

OGRLayer* FMTparser::getlayer(GDALDataset* dataset,int id) const
    {
	OGRLayer * layer = nullptr;
	try{
	layer = dataset->GetLayer(id);
    if( layer == nullptr )
        {
        _exhandler->raise(Exception::FMTexc::FMTinvalidlayer,dataset->GetDescription(),
			"FMTparser::getlayer",__LINE__, __FILE__, _section);
        }
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTparser::getlayer", __LINE__, __FILE__);
	}
    return layer;
    }

void FMTparser::getWSfields(OGRLayer* layer, std::map<int, int>& themes, int& age, int& area, int& lock, std::string agefield, std::string areafield, std::string lockfield) const
{
	try {
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
				std::smatch kmatch;
				if (std::regex_search(fname, kmatch,rxvectortheme)/*fname.find("THEME") != std::string::npos*/)
				{
					//fname.erase(0, 5);
					//themes[getnum<int>(fname) - 1] = iField;
					themes[getnum<int>(std::string(kmatch[2])) - 1] = iField;
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
				_exhandler->raise(Exception::FMTexc::FMTinvalid_maskrange,
					layer->GetDescription(),"FMTparser::getWSfields", __LINE__, __FILE__, _section);
			}
			if (age == -1 && !agefield.empty())
			{
				_exhandler->raise(Exception::FMTexc::FMTmissingfield, agefield + " " + layer->GetDescription(),
					"FMTparser::getWSfields",__LINE__, __FILE__, _section);
			}
			if (area == -1 && !areafield.empty())
			{
				_exhandler->raise(Exception::FMTexc::FMTmissingfield, areafield + " " + layer->GetDescription(),
					"FMTparser::getWSfields", __LINE__, __FILE__, _section);
			}
			if (lock == -1 && !lockfield.empty())
			{
				_exhandler->raise(Exception::FMTexc::FMTmissingfield, lockfield + " " + layer->GetDescription(),
					"FMTparser::getWSfields", __LINE__, __FILE__, _section);
			}
	}catch (...)
			{
			_exhandler->raisefromcatch("","FMTparser::getWSfields", __LINE__, __FILE__, _section);
			}
}

#endif
std::string FMTparser::setspec(Core::FMTsection section, Core::FMTkwor key,const Core::FMTyields& ylds,const Core::FMTconstants& constants,Core::FMTspec& spec, const std::string& line)
    {
	std::string rest = "";
	try {
			std::smatch kmatch;
			
			if (std::regex_search(line, kmatch, FMTparser::rxayld))
			{
				const std::string yld = std::string(kmatch[4]) + std::string(kmatch[20]);
				bool pushaagebound = false;
				if (yld=="_AGE")
					{
					pushaagebound = true;
					}else {
					isyld(ylds, yld, section);
					}
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
					lowerbound = getnum<double>(strlower, constants);
				}
				else {
					lowerbound = getnum<double>(singlebound, constants);
				}
				if (pushaagebound)
					{
					spec.addbounds(Core::FMTagebounds(section, key, upperbound, lowerbound));
				}else {
					spec.addbounds(Core::FMTyldbounds(section, key, yld, upperbound, lowerbound));
					}
				
				rest = " " + std::string(kmatch[1]) + std::string(kmatch[16]) + std::string(kmatch[28]);
			}
			else {
				rest = line;
			}

			if (std::regex_search(rest, kmatch, FMTparser::rxaage))
			{
				std::string singlebound = std::string(kmatch[20]) + std::string(kmatch[21]) + std::string(kmatch[22]);
				int upperbound = std::numeric_limits<int>::max();
				int lowerbound = std::numeric_limits<int>::min();
				if (singlebound.empty())
				{
					if (std::string(kmatch[13]).empty())
					{
						std::string strupper = std::string(kmatch[10]) + std::string(kmatch[11]) + std::string(kmatch[12]);
						upperbound = getnum<int>(strupper, constants);
					}
					std::string strlower = std::string(kmatch[5]) + std::string(kmatch[6]) + std::string(kmatch[7]);
					lowerbound = getnum<int>(strlower, constants);
				}
				else {
					lowerbound = getnum<int>(singlebound, constants);
				}
				spec.addbounds(Core::FMTagebounds(section, key, upperbound, lowerbound));
				rest = " " + std::string(kmatch[1]) + std::string(kmatch[15]) + std::string(kmatch[16]) + std::string(kmatch[24]);
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("",
				"FMTparser::setspec", __LINE__, __FILE__, _section);
			}
    return rest;
    }

bool FMTparser::isact(Core::FMTsection section,const std::vector<Core::FMTaction>& actions, std::string action) const
    {
	try{
    if (std::find_if(actions.begin(),actions.end(), Core::FMTactioncomparator(action,true))==actions.end())
        {
			_exhandler->raise(Exception::FMTexc::FMTignore, "No action named " + action + " at line " + std::to_string(_line), "FMTparser::isact", __LINE__, __FILE__, section);
        return false;
        }else
		{
        return true;
        }
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTparser::isact", __LINE__, __FILE__);
	}
	return false;
    }



bool FMTparser::isyld(const Core::FMTyields& ylds,const std::string& value, Core::FMTsection section) const
    {
	try{
    if (ylds.isyld(value))
        {
        return true;
        }
     _exhandler->raise(Exception::FMTexc::FMTinvalid_yield,value+" at line " + std::to_string(_line),
		 "FMTparser::isyld",__LINE__, __FILE__,section);
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTparser::isyld", __LINE__, __FILE__);
		}
     return false;
    }







bool FMTparser::tryopening(const std::ifstream& stream, const std::string& location) const
        {
		try{
        _location = location;
		std::string extension = boost::filesystem::extension(location);
        transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        this->setsection(from_extension(extension));
        if (!stream.is_open())
            {
            _exhandler->raise(Exception::FMTexc::FMTinvalid_path,location,
				"FMTparser::tryopening", __LINE__, __FILE__, _section);
            return false;
            }
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTparser::tryopening", __LINE__, __FILE__);
		}
        return true;
        }

bool FMTparser::tryopening(const std::ofstream& stream, const std::string& location) const
        {
		try{
        _location = location;
        if (!stream.is_open())
            {
            _exhandler->raise(Exception::FMTexc::FMTinvalid_path,location,
				"FMTparser::tryopening", __LINE__, __FILE__, _section);
            return false;
            }
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTparser::tryopening", __LINE__, __FILE__);
			}
        return true;
        }

bool FMTparser::isvalidfile(const std::string& location) const
	{
	try {
		boost::filesystem::path pathObj(location);
		std::string extension = boost::filesystem::extension(location);
		transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
		if (!boost::filesystem::exists(pathObj) || !boost::filesystem::is_regular_file(pathObj))
		{
			_exhandler->raise(Exception::FMTexc::FMTinvalid_path, location,
				"FMTparser::isvalidfile", __LINE__, __FILE__, _section);
			return false;
		}
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTparser::isvalidfile", __LINE__, __FILE__);
		}
	return true;
	}

bool FMTparser::isnum(std::string value) const
    {
	boost::erase_all(value, ",");
	return std::regex_match(value,rxnumber);
    }



std::vector<std::string>FMTparser::regexloop(std::regex& cutregex, std::string& str) const
        {
		std::vector<std::string>result;
		try{
        auto els_begin = std::sregex_iterator(str.begin(), str.end(), cutregex);
        auto els_end = std::sregex_iterator();
        for (std::sregex_iterator i = els_begin; i != els_end; ++i)
            {
			std::smatch match = *i;
			std::string match_str = match.str();
            result.push_back(match_str);
            }
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTparser::regexloop", __LINE__, __FILE__);
		}
        return result;
        }
    bool FMTparser::isvalid(std::string& line) const
        {
        return std::regex_match(line,rxvalid);
        }
    void FMTparser::clearcomments(std::string& line)
        {
		try {
			std::smatch kmatch;
			if (std::regex_search(line, kmatch, rxremovecomment))
			{
				_comment = std::string(kmatch[2]);
				boost::to_upper(_comment);
				line = std::string(kmatch[1]);
			}
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTparser::clearcomments", __LINE__, __FILE__);
		}
        }
	std::string FMTparser::upper(const std::string& lowercases) const
        {
		std::locale loc;
		std::string uppercases;
		try{
        for (std::string::size_type i=0;i<lowercases.size(); ++i)
            {
            uppercases+=toupper(lowercases[i],loc);
            }
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTparser::upper", __LINE__, __FILE__);
		}
        return uppercases;
        }
	std::vector<std::string>FMTparser::spliter(std::string strmask, std::regex& xspliter) const
        {
		std::smatch kmatch;
		std::string value;
		std::vector<std::string>vecmask;
		try{
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
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTparser::spliter", __LINE__, __FILE__);
		}
        return vecmask;
        }
	std::string FMTparser::getcleanline(std::ifstream& stream)
        {
        ++_line;
		std::string newline;
		try{
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
			if (_incomment && !_comment.empty() && _comment.find('}') != std::string::npos)
				{
				_incomment = false;
				}
			if (!_comment.empty() && _comment.find('{') != std::string::npos)
				{
				_incomment = true;
				}
            }else{
            stream.close();
            }
        boost::trim(newline);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTparser::getcleanline", __LINE__, __FILE__);
			}
        return newline;
        }

bool FMTparser::getforloops(std::string& line,const std::vector<Core::FMTtheme>& themes, const Core::FMTconstants& cons, std::vector<std::string>& allvalues, std::string& target)
	{
	try{
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
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTparser::getforloops", __LINE__, __FILE__);
		}
	return false;
	}

std::queue<std::string> FMTparser::tryinclude(const std::string& line, const std::vector<Core::FMTtheme>& themes, const Core::FMTconstants& cons)
	{
	std::smatch kmatch;
	std::queue<std::string>included_lines;
	try{
	if (std::regex_search(line, kmatch, FMTparser::rxinclude))
		{
		std::string location = kmatch[3];
		boost::filesystem::path includedpath(location);
		boost::filesystem::path basepath;
		if (includedpath.is_absolute())
			{
			const boost::filesystem::path l_ppath(_location);
			const boost::filesystem::path parent_path = l_ppath.parent_path();
			location = boost::filesystem::canonical(includedpath, parent_path).string();
			}else if (boost::starts_with(location, "."))
				{
				const boost::filesystem::path l_ppath(_location);
				const boost::filesystem::path parent_path = l_ppath.parent_path();
				location.erase(0, 1);
				const boost::filesystem::path full_path = parent_path / location;
				location = full_path.string();
				}
			basepath = boost::filesystem::path(location);
		if (!boost::filesystem::exists(location))//If does not exist then try with the _ type path.
			{
			const std::string extension = boost::filesystem::extension(_location);
			std::string actualextension = boost::filesystem::extension(location);
			if (extension.find("_")!=std::string::npos&&
				actualextension.find("_")==std::string::npos)
				{
				actualextension.erase(0, 1);
				const std::string newextension = "._" + actualextension;
				boost::filesystem::path full_path(location);
				full_path.replace_extension(newextension);
				location = full_path.string();
				}
			}
		if (!boost::filesystem::exists(location))//Look back to the original folder...
			{
			const boost::filesystem::path llpath(location);
			const boost::filesystem::path parentdir = llpath.parent_path();
			const boost::filesystem::path finalpath = parentdir / boost::filesystem::path("../..") / basepath.filename();
			location = finalpath.string();
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
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTparser::tryinclude", __LINE__, __FILE__);
	}
	return included_lines;
	}

std::string FMTparser::returninclude(const std::string& line, const std::vector<Core::FMTtheme>& themes, const Core::FMTconstants& cons)
	{
	std::string thereturn = "";
	try{
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
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTparser::returninclude", __LINE__, __FILE__);
	}
	return thereturn;
	}

std::string FMTparser::getcleanlinewfor(std::ifstream& stream,const std::vector<Core::FMTtheme>& themes,const Core::FMTconstants& cons)
    {
	std::smatch kmatch;
	std::string line;
	try {
		while (!_included.empty())
		{
			line = _included.front();
			_included.pop();
			return line;
		}
		if (_forvalues.empty())
		{
			std::map<std::string, std::vector<std::string>>loops_replaces;
			std::map<std::string, std::pair<int, int>>forlocation;
			int lineid = 0;
			std::stack<std::string>keys;
			std::vector<std::pair<int, std::string>>Sequence;
			std::vector<std::string>ordered_keys;
			do {

				line = getcleanline(stream);
				std::vector<std::string>allvalues;
				std::string target;
				bool gotfor = getforloops(line, themes, cons, allvalues, target);
				if (!gotfor && line.find("FOREACH") != std::string::npos)
				{

					do {
						line += getcleanline(stream);
					} while (line.find(")") == std::string::npos);
					gotfor = getforloops(line, themes, cons, allvalues, target);
				}
				if (gotfor)
				{
					loops_replaces[target] = allvalues;
					forlocation[target] = std::pair<int, int>(lineid, 0);
					keys.push(target);
				}
				else if (keys.empty())
				{
					return returninclude(line, themes, cons);
				}
				else if (std::regex_search(line, kmatch, FMTparser::rxend))
				{
					std::string endfor = keys.top();
					keys.pop();
					forlocation[endfor].second = lineid;
					ordered_keys.push_back(endfor);
				}
				else if (isvalid(line)) {
					Sequence.push_back(std::pair<int, std::string>(lineid, line));
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
					}
					else if (tline.first >= forlocation[key].first && tline.first < forlocation[key].second)
					{
						process.push_back(tline);
					}
					else {
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

		while (!_forvalues.empty())
		{
			line = _forvalues.front();
			_forvalues.pop();
			return returninclude(line, themes, cons);
		}
	return returninclude(line, themes, cons);
	}catch (...)
	 {
		 _exhandler->raisefromcatch("", "FMTparser::getcleanlinewfor", __LINE__, __FILE__);
	 }
	return std::string();
    }

std::map<Core::FMTsection, std::string> FMTparser::getprimary(const std::string& primarylocation)
	{
		
		std::map<Core::FMTsection, std::string>targets;
		try {
			std::ifstream primarystream(primarylocation);
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
							const Core::FMTsection section = from_extension(extension);
							targets[section] = (primary_path.parent_path() / file_path).string();
						}
					}
				}
			}
			if (targets.find(Core::FMTsection::Constants) == targets.end())
			{
				targets[Core::FMTsection::Constants] = "";
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("at "+ primarylocation,"FMTparser::getprimary", __LINE__, __FILE__, _section);
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
		try{
		std::string separator = "_SAMEAS";
        if (allset.find(separator)!= std::string::npos)
            {
			std::string realname = allset.substr(0, allset.find(separator));
            boost::trim(realname);
			if (realname.empty())
				{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Empty target "+allset, "FMTparser::sameas", __LINE__, __FILE__, _section);
				}
            all_elements.push_back(realname);
			std::string sameasstr = allset.substr(allset.find(separator)+separator.size());
            boost::trim(sameasstr);
            all_elements.push_back(sameasstr);
            }else{
			std::string realname = allset;
            boost::trim(realname);
            all_elements.push_back(realname);
            }
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTparser::sameas", __LINE__, __FILE__);
		}
        return all_elements;
        }

std::vector<std::vector<std::string>>FMTparser::readcsv(const std::string& location,const char& separator)
	{
	std::vector<std::vector<std::string>>lines;
	try {
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
	}catch (...)
		{
		_exhandler->raisefromcatch("at " + location ,"FMTparser::readcsv", __LINE__, __FILE__, _section);
		}
	return lines;
	}




std::istream& FMTparser::safeGetline(std::istream& is, std::string& t) const
	{
	t.clear();
	std::istream::sentry se(is, true);
	std::streambuf* sb = is.rdbuf();
	try{
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
	}catch (...)
	{
		_exhandler->raisefromcatch("", "FMTparser::safeGetline", __LINE__, __FILE__);
	}
	return is;
	}

}
