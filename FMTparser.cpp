

#include "FMTparser.h"
/*#include "FMTCore.h"*/
#include <boost/filesystem.hpp>




using namespace std;

namespace WSParser
{


FMTwssect FMTparser::from_extension(const string& ext)
    {
    vector<string>extensions={".run",".lan",".are",".act",".trn",".yld",".out",".opt",".con",".seq",".lif",
							"._lan","._are","._act","._trn","._yld","._out","._opt","._con","._seq","._lif" };
    vector<string>::const_iterator it = find(extensions.begin(), extensions.end(),ext);
    size_t id = (it - extensions.begin());
	//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "build stats: " <<ext<<" "<< id << "\n";
    switch (id)
        {
            case 0 :return FMTwssect::Control;
            case 1 :return FMTwssect::Landscape;
            case 2 :return FMTwssect::Area;
            case 3 :return FMTwssect::Action;
            case 4 :return FMTwssect::Transition;
            case 5 :return FMTwssect::Yield;
            case 6 :return FMTwssect::Outputs;
            case 7 :return FMTwssect::Optimize;
            case 8 :return FMTwssect::Constants;
            case 9 :return FMTwssect::Schedule;
			case 10:return FMTwssect::Lifespan;
			case 11:return FMTwssect::Landscape;
			case 12:return FMTwssect::Area;
			case 13:return FMTwssect::Action;
			case 14:return FMTwssect::Transition;
			case 15:return FMTwssect::Yield;
			case 16:return FMTwssect::Outputs;
			case 17:return FMTwssect::Optimize;
			case 18:return FMTwssect::Constants;
			case 19:return FMTwssect::Schedule;
			case 20:return FMTwssect::Lifespan;
            default:      return FMTwssect::Empty;
        }
    }

FMTparser::FMTparser() : FMTobject(),
        rxremovecomment(regex("^(.*?)([;]+.*)")),
        rxvalid(regex("^(?!\\s*$).+")),
		rxinclude(regex("^(\\*INCLUDE)([\\s\\t]*)(.+)")),
        //rxcommentsplit(regex("(\\{[\\s\\S]*?})([^\\{]*)")),
        //rxfor(regex("^(FOREACH)([\\s\\t]*)([^\\s\\t]*)([\\s\\t]*)(IN)([\\s\\t]*)((\\()(_TH)(\\d*)(\\()([^\\s\\t]*)(\\)\\))|(\\()(_TH)(\\d*)(\\))|(\\()(\\d*)(\\.\\.)(\\d*)(\\))|(\\()([^\\s\\t]*)(\\)))",regex_constants::ECMAScript|regex_constants::icase)),
          rxfor(regex("^(FOREACH)([\\s\\t]*)([^\\s\\t]*)([\\s\\t]*)(IN)([\\s\\t]*)((\\()(_TH)(\\d*)(\\()([^\\s\\t]*)(\\)\\))|(\\()(_TH)(\\d*)(\\))|(\\()(\\d*)(\\.\\.)(\\d*)(\\))|(\\()([^\\)]*)(\\)))|(\\bFOR\\b)([\\s\\t]*)([^\\:\\=]*)([\\:\\=\\s\\t]*)(\\d*)([\\s\\t]*)(TO)([\\s\\t]*)(\\d*)", regex_constants::ECMAScript | regex_constants::icase)),
		rxend(regex("^(ENDFOR)",regex_constants::ECMAScript|regex_constants::icase)),
        _incomment(false),
         _forvalues(),
		_included(),
        //rxayld(regex("^(.+)(\\@YLD\\()([\\s\\t]*)(.+)(\\,)([\\s\\t]*)((#[^\\.]*)|(\\d*.\\d*))(\\.\\.)((#[^\\.]*)|(\\d*.\\d*))(\\))(.+)|(.+)(\\@YLD\\()([\\s\\t]*)(.+)(\\,)([\\s\\t]*)((#[^\\.]*)|(\\d*)|(\\d*.\\d*))(\\))(.+)",regex_constants::ECMAScript|regex_constants::icase)),
		rxayld(regex("^(.+)(\\@YLD\\()([\\s\\t]*)(.+)(\\,)([\\s\\t]*)((#[^\\.]*)|(\\d*.\\d*))(\\.\\.)((#[^\\.]*)|(\\d*.\\d*)|(_MAXAGE))(\\))(.+)|(.+)(\\@YLD\\()([\\s\\t]*)(.+)(\\,)([\\s\\t]*)((#[^\\.]*)|(\\d*)|(\\d*.\\d*))(\\))(.+)", regex_constants::ECMAScript | regex_constants::icase)),
        rxaage(regex("^(.+)(\\@AGE\\()([\\s\\t]*)((#[^\\.]*)|(\\d*)|(\\d*.\\d*))(\\.\\.)((#[^\\.]*)|(\\d*)|(\\d*.\\d*)|(_MAXAGE))(\\))(.+)|(.+)(\\@AGE\\()([\\s\\t]*)((#[^\\.]*)|(\\d*)|(\\d*.\\d*))(\\))(.+)",regex_constants::ECMAScript|regex_constants::icase)),
		rxoperators(regex("([^\\+\\-\\/\\*]*)([\\+\\-\\/\\*]*)", regex_constants::icase)),
		rxprimary("^([^\\[]*)(\\[)([^\\]]*)(.+)"),
		_constreplacement(0),
        _line(0),
		_comment(),
       _location(),
       _section(),
        rxseparator(regex("([\\s\\t]*)([^\\s\\t]*)"))
        {
		string runtimelocation = getruntimelocation();
		runtimelocation += "\\GDAL_DATA";
		if (!filesystem::is_directory(filesystem::path(runtimelocation)))
			{
			_exhandler->raise(FMTexc::FMTinvalid_path,
				_section,"Cannot find GDAL_DATA at "+runtimelocation,__LINE__,__FILE__);
			}
		CPLSetConfigOption("GDAL_DATA", runtimelocation.c_str());
        }

FMTparser::FMTparser(const FMTparser& rhs):
        FMTobject(rhs),
        rxremovecomment(rhs.rxremovecomment),
        rxvalid(rhs.rxvalid),
		rxinclude(rhs.rxinclude),
        //rxstartcomment(rhs.rxstartcomment),
        //rxstopcomment(rhs.rxstopcomment),
        //rxcommentsplit(rhs.rxcommentsplit),
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
        _section(rhs._section),
        rxseparator(rhs.rxseparator)
    {

    }

FMTparser& FMTparser::operator = (const FMTparser& rhs)
    {
        if (this!=&rhs)
            {
            FMTobject::operator = (rhs);
            rxremovecomment = rhs.rxremovecomment;
            rxvalid = rhs.rxvalid;
			rxinclude = rhs.rxinclude;
            //rxstartcomment = rhs.rxstartcomment;
            //rxstopcomment = rhs.rxstopcomment;
            //rxcommentsplit = rhs.rxcommentsplit;
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
            _section = rhs._section;
            }
    return *this;
    }

GDALDataset* FMTparser::getdataset(const string& location) const
    {
	GDALDataset* data = nullptr;
	if (isvalidfile(location))
		{
		data = (GDALDataset*)GDALOpen(location.c_str(), GA_ReadOnly);
		if (data == nullptr)
			{
			_exhandler->raise(FMTexc::FMTinvaliddataset,_section,data->GetDescription(), __LINE__, __FILE__);
			}
		}
    return data;
    }
/*
GDALDataset* FMTparser::createdataset(const string& location,const FMTlayer<int>& layer) const
    {
    const char *pszFormat = "GTiff";
    GDALDriver *poDriver;
    char **papszMetadata;
    poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
    if( poDriver == nullptr )
        {
        _exhandler->raise(FMTexc::FMTinvaliddriver,_section,string(pszFormat));
        }
    char **papszOptions = NULL;
    papszOptions = CSLSetNameValue( papszOptions, "TILED", "YES" );
    papszOptions = CSLSetNameValue( papszOptions, "BLOCKXSIZE", "128" );
    papszOptions = CSLSetNameValue( papszOptions, "BLOCKYSIZE", "128" );
    papszOptions = CSLSetNameValue( papszOptions, "COMPRESS", "LZW" );
    papszOptions = CSLSetNameValue( papszOptions, "ZLEVEL", "9" );
    papszOptions = CSLSetNameValue( papszOptions, "BIGTIFF", "YES" );
    GDALDataset *poDstDS = poDriver->Create( location.c_str(), layer.GetXSize(), layer.GetYSize(), 1, GDT_Int32,papszOptions);
    if (poDstDS == nullptr)
        {
        _exhandler->raise(FMTexc::FMTinvaliddataset,_section,poDstDS->GetDescription());
        }
    //setprojection and geotransform
    vector<double>geotrans = layer.getgeotransform();
    string projection = layer.getprojection();
    poDstDS->SetGeoTransform(&geotrans[0]);
    poDstDS->SetProjection(projection.c_str());
    return poDstDS;
    }
*/
GDALRasterBand* FMTparser::getband(GDALDataset* dataset,int bandid) const
    {
    GDALRasterBand* band = dataset->GetRasterBand(bandid);
    if (band == nullptr)
        {
        _exhandler->raise(FMTexc::FMTinvalidband,_section,dataset->GetDescription(), __LINE__, __FILE__);
        }
    return band;
    }

GDALRasterBand* FMTparser::createband(GDALDataset* dataset,const vector<string>& categories,int bandid) const
    {
    GDALRasterBand* band = getband(dataset,bandid);
    band->SetNoDataValue(-9999);
    band->Fill(-9999);
    if (!categories.empty())
        {
        CPLStringList strlist;
        for(const string& value : categories)
            {
            strlist.AddString(value.c_str());
            }
        band->SetCategoryNames(strlist);
        }
    band->FlushCache();
    return band;
    }

vector<string>FMTparser::getcat(GDALDataset* dataset,int bandid) const
    {
    char** meta = dataset->GetMetadata();
    GDALRasterBand* band = getband(dataset,bandid);
    char** names  = band->GetCategoryNames();
    vector<string>values;
    int id = 0;
    while(names[id])
        {
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
        _exhandler->raise(FMTexc::FMTinvalidoverview,_section,band->GetDataset()->GetDescription(), __LINE__, __FILE__);
        }
    return overview;
    }

GDALDataset* FMTparser::getvectordataset(const string& location) const
    {
	GDALDataset* dataset = nullptr;
	if (isvalidfile(location))
		{
		dataset = (GDALDataset*) GDALOpenEx(location.c_str(), GDAL_OF_VECTOR, nullptr, nullptr, nullptr );
		if( dataset == nullptr )
			{
			_exhandler->raise(FMTexc::FMTinvaliddataset,_section,dataset->GetDescription(), __LINE__, __FILE__);
			}
		}
    return dataset;
    }

OGRLayer* FMTparser::getlayer(GDALDataset* dataset,int id) const
    {
    OGRLayer * layer =dataset->GetLayer(id);
    if( layer == nullptr )
        {
        _exhandler->raise(FMTexc::FMTinvalidlayer,_section,dataset->GetDescription(), __LINE__, __FILE__);
        }
    return layer;
    }

string FMTparser::setspec(FMTwssect section,FMTwskwor key,const FMTyields& ylds,const FMTconstants& constants,FMTspec& spec, const string& line)
    {
    smatch kmatch;
    string rest = "";
    /*if (key==FMTwskwor::Source && section == FMTwssect::Transition && line.find("@")!=string::npos)
        {
        cout<<line<<endl;
        }*/

    if (regex_search(line,kmatch,FMTparser::rxayld))
        {
        //string yld = string(kmatch[4]) + string(kmatch[14]);
        string yld = string(kmatch[4]) + string(kmatch[20]); //16 before? //  string(kmatch[4]) + string(kmatch[19])
        /*for (int id = 0; id < 24 ; ++id)
            {
            cout<<id<<string(kmatch[id])<<endl;
            }*/

        isyld(ylds,yld,section);
        //string singlebound = string(kmatch[17]);
        string singlebound = string(kmatch[21]) + string(kmatch[22]) + string(kmatch[23]);//string(kmatch[20]) + string(kmatch[21]) + string(kmatch[22]);
        double upperbound = numeric_limits<double>::max();
        double lowerbound = numeric_limits<double>::min();
        if (singlebound.empty())
            {
            //string strupper = string(kmatch[9]);
			if (string(kmatch[14]).empty())
				{
				string strupper = string(kmatch[12]) + string(kmatch[13]);
				upperbound = getnum<double>(strupper, constants);
				}
            string strlower = string(kmatch[8]) + string(kmatch[9]);//string(kmatch[7]) + string(kmatch[8]);//string(kmatch[8]) + string(kmatch[9]);

            //string strlower = string(kmatch[7]);
            lowerbound = getnum<double>(strlower,constants);
            }else{
            lowerbound = getnum<double>(singlebound,constants);
            }
        spec.addbounds(FMTyldbounds(section,key,yld,upperbound,lowerbound));

		rest = " "+ string(kmatch[1]) + string(kmatch[16]) + string(kmatch[28]);//+ string(kmatch[15]) + string(kmatch[27]);
        }else{
        rest = line;
        }
    if (regex_search(rest,kmatch,FMTparser::rxaage))
        {
        //string singlebound = string(kmatch[10]);
        string singlebound = string(kmatch[20]) + string(kmatch[21])+ string(kmatch[22]);

        int upperbound = numeric_limits<int>::max();
        int lowerbound = numeric_limits<int>::min();

        if (singlebound.empty())
            {
            //string strupper = string(kmatch[6]);
            //string strlower = string(kmatch[4]);
            if(string(kmatch[13]).empty())
                {
                string strupper = string(kmatch[10])+string(kmatch[11]) + string(kmatch[12]);
                upperbound = getnum<int>(strupper,constants);
                }
            string strlower = string(kmatch[5])+string(kmatch[6]) + string(kmatch[7]);
            lowerbound = getnum<int>(strlower,constants);
            }else{
            lowerbound = getnum<int>(singlebound,constants);
            }
            spec.addbounds(FMTagebounds(section,key,upperbound,lowerbound));
        //rest = " "+ string(kmatch[1]) + string(kmatch[7]) + string(kmatch[8]) + string(kmatch[12]);
        rest = " "+ string(kmatch[1]) + string(kmatch[15]) + string(kmatch[16]) +  string(kmatch[24]);
        }

    return rest;
    }

bool FMTparser::isact(FMTwssect section,const vector<FMTaction>& actions,string action)
    {
    if (find_if(actions.begin(),actions.end(), FMTactioncomparator(action))==actions.end())
        {
        _exhandler->raise(FMTexc::WSundefined_action,section,action+" at line " + to_string(_line), __LINE__, __FILE__);
        return false;
        }else{
        return true;
        }
    }

void FMTparser::getWSfields(OGRLayer* layer,map<int,int>& themes,int& age,int& area,int& lock,string agefield,string areafield,string lockfield) const
    {
    OGRFeatureDefn *fdef = layer->GetLayerDefn();
    string capage = agefield;
    string caparea = areafield;
    string caplock = lockfield;
    boost::to_upper(capage);
    boost::to_upper(caparea);
    boost::to_upper(caplock);
    map<int,int>themes_fields;
    age=-1;
    area=-1;
    lock=-1;
    for( int iField = 0; iField < fdef->GetFieldCount(); ++iField)
        {
        OGRFieldDefn* fielddef = fdef->GetFieldDefn(iField);
        string fname = fielddef->GetNameRef();
        boost::to_upper(fname);
        if (fname.find("THEME")!=string::npos)
            {
            fname.erase(0,5);
            themes[stoi(fname)-1] = iField;
            }else if(fname==caplock)
                {
                lock = iField;
                }else if(fname==capage)
                    {
                    age = iField;
                    }else if(fname==caparea)
                        {
                        area = iField;
                        }
        }
    if (themes.size()==0)
        {
        _exhandler->raise(FMTexc::WSinvalid_maskrange,_section,layer->GetDescription(), __LINE__, __FILE__);
        }
    if (age==-1 && !agefield.empty())
        {
        _exhandler->raise(FMTexc::FMTmissingfield,_section,agefield+" "+layer->GetDescription(), __LINE__, __FILE__);
        }
    if (area==-1 && !areafield.empty())
        {
        _exhandler->raise(FMTexc::FMTmissingfield,_section,areafield+" "+layer->GetDescription(), __LINE__, __FILE__);
        }
    if (lock==-1 && !lockfield.empty())
        {
        _exhandler->raise(FMTexc::FMTmissingfield,_section,lockfield+" "+layer->GetDescription(), __LINE__, __FILE__);
        }
    }

bool FMTparser::isyld(const FMTyields& ylds,const string& value,FMTwssect section)
    {
    if (ylds.isyld(value))
        {
        return true;
        }
     _exhandler->raise(FMTexc::WSinvalid_yield,section,value+" at line " + to_string(_line), __LINE__, __FILE__);
     return false;
    }

/*double FMTparser::getexactdouble(const string& value) const
	{
	size_t dot = value.find(".");
	double result = stod(value);
	if (dot != string::npos)//round for exact representation
		{
		size_t precision = (value.size() - dot);
		result = round(result * precision) / precision;
		}
	return result;
	}*/

bool FMTparser::checkmask(const vector<FMTtheme>& themes, const vector<string>& values, const string& mask) const
	{
	bool returnvalue = true;
	if (themes.size() != values.size())
	{
		_exhandler->raise(FMTexc::WSinvalid_maskrange, _section, mask + " at line " + to_string(_line), __LINE__, __FILE__);
		returnvalue = false;
	}
	else {
		int id = 0;
		for (const FMTtheme& theme : themes)
		{
			if (!theme.isattribute(values[id]) && !theme.isaggregate(values[id]) && values[id] != "?")
			{
				_exhandler->raise(FMTexc::WSundefined_attribute, _section, mask + " at line " + to_string(_line), __LINE__, __FILE__);
				returnvalue = false;
			}
			++id;
		}
	}
	return  returnvalue;
	}

bool FMTparser::validate(const vector<FMTtheme>& themes, const string& mask) const
	{
	vector<string>values;
	boost::split(values, mask, boost::is_any_of(" /t"), boost::token_compress_on);
	return checkmask(themes, values,mask);
	}


bool FMTparser::tryopening(const ifstream& stream, const string& location)
        {
        _location = location;
        string extension = boost::filesystem::extension(location);
        transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        _section = from_extension(extension);
        if (!stream.is_open())
            {
            _exhandler->raise(FMTexc::FMTinvalid_path,_section,location, __LINE__, __FILE__);
            return false;
            }
        return true;
        }

bool FMTparser::tryopening(const ofstream& stream, const string& location)
        {
        _location = location;
        //set the _section stuff here...
        if (!stream.is_open())
            {
            _exhandler->raise(FMTexc::FMTinvalid_path,_section,location, __LINE__, __FILE__);
            return false;
            }
        return true;
        }

bool FMTparser::isvalidfile(const string& location) const
	{
	boost::filesystem::path pathObj(location);
	string extension = boost::filesystem::extension(location);
	transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
	if (!boost::filesystem::exists(pathObj) || !boost::filesystem::is_regular_file(pathObj))
		{
		_exhandler->raise(FMTexc::FMTinvalid_path, _section, location, __LINE__, __FILE__);
		return false;
		}
	return true;
	}

/*bool FMTparser::isvalidfolder(const string& location) const
	{

	}*/

bool FMTparser::isnum(const string& value) const
    {
    try{
        stod(value);
    }catch(...)
        {
        return false;
        }
    return true;
    }



vector<string>FMTparser::regexloop(regex& cutregex,string& str) const
        {
        vector<string>result;
        auto els_begin = sregex_iterator(str.begin(), str.end(), cutregex);
        auto els_end = sregex_iterator();
        for (sregex_iterator i = els_begin; i != els_end; ++i)
            {
            smatch match = *i;
            string match_str = match.str();
            result.push_back(match_str);
            }
        return result;
        }
    bool FMTparser::isvalid(string& line) const
        {
        return regex_match(line,rxvalid);
        }
    void FMTparser::clearcomments(string& line)
        {
        smatch kmatch;
        if (regex_search(line,kmatch,rxremovecomment))
            {
			_comment = string(kmatch[2]);
			line = string(kmatch[1]);
			}
        }
    string FMTparser::upper(const string& lowercases) const
        {
        locale loc;
        string uppercases;
        for (string::size_type i=0;i<lowercases.size(); ++i)
            {
            uppercases+=toupper(lowercases[i],loc);
            }
        return uppercases;
        }
    vector<string>FMTparser::spliter(string strmask,regex& xspliter) const
        {
        smatch kmatch;
        string value;
        vector<string>vecmask;
        size_t endsize;
        while(!strmask.empty())
            {
            regex_search(strmask,kmatch,xspliter);
            value = string(kmatch[2]);
            if (isvalid(value))
                {
                vecmask.push_back(value);
                }
            endsize = (string(kmatch[0]).size());
            strmask.erase(strmask.begin(),strmask.begin()+endsize);
            }
        return vecmask;
        }
    string FMTparser::getcleanline(ifstream& stream)
        {
        ++_line;
        string newline;
        if (safeGetline(stream, newline))//getline(stream,newline)
            {

            clearcomments(newline);

            string fullline = newline;
            newline = "";
            for(int loc = 0; loc < int(fullline.size()); ++loc)
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
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "testing " << newline << "\n";
        boost::trim(newline);
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "GOT "<< newline << "\n";
        return newline;
        }

bool FMTparser::getforloops(string& line,const vector<FMTtheme>& themes, const FMTconstants& cons, vector<string>& allvalues, string& target)
	{
	smatch kmatch;
	//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "testing |" << line<<"|"<< "\n";
	if (regex_search(line, kmatch, rxfor))
	{
		//target = "\\" + string(kmatch[3]);
		target = string(kmatch[3]);

		if (!string(kmatch[24]).empty())
		{
			string val = string(kmatch[24]);
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "splitting  " << val<<" for line "<< line<< "\n";
			boost::split(allvalues, val, boost::is_any_of(" /t,"),boost::token_compress_on);
		}
		else if (!string(kmatch[16]).empty())
		{
			int theme = stoi(string(kmatch[16])) - 1;
			string att = "?";
			allvalues = themes[theme].getattributes(att);
		}
		else if (!string(kmatch[10]).empty())
		{
			int theme = stoi(string(kmatch[10])) - 1;
			string aggregate = kmatch[12];
			//need to keep the location of aggregates!...done!

			allvalues = themes[theme].getattributes(aggregate,true);//see last argument!
		}
		else if (!string(kmatch[19]).empty() || !string(kmatch[26]).empty())
		{
			if (!string(kmatch[28]).empty())
				{
				target = kmatch[28];
				}
			string slower = string(kmatch[19]) + string(kmatch[30]);
			string supper = string(kmatch[21]) + string(kmatch[34]);
			int lower = getnum<int>(slower, cons);
			int upper = getnum<int>(supper, cons);
			for (int id = lower; id <= upper; ++id)
			{
				allvalues.push_back(to_string(id));
			}
		}
		return true;
	}
	return false;
	}

queue<string> FMTparser::tryinclude(const string& line, const vector<FMTtheme>& themes, const FMTconstants& cons)
	{
	smatch kmatch;
	queue<string>included_lines;
	if (regex_search(line, kmatch, FMTparser::rxinclude))
		{
		string location = kmatch[3];
		if (boost::starts_with(location, "."))
			{
			boost::filesystem::path l_ppath(_location);
			boost::filesystem::path parent_path = l_ppath.parent_path();
			location.erase(0, 1);
			boost::filesystem::path full_path = parent_path / location;
			location = full_path.string();
			}
		FMTparser newparser;
		ifstream newstream(location);
		if (newparser.tryopening(newstream, location))
			{
			while (newstream.is_open())
				{
				string newline = newparser.getcleanlinewfor(newstream,themes,cons);
				if (!newline.empty())
					{
					included_lines.push(newline);
					}
				}
			}
		}
	return included_lines;
	}

string FMTparser::returninclude(const string& line, const vector<FMTtheme>& themes, const FMTconstants& cons)
	{
	string thereturn = "";
	if (!_included.empty())
		{
		thereturn = _included.front();
		_included.pop();
		}else{
			queue<string>inlines = tryinclude(line, themes, cons);
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

string FMTparser::getcleanlinewfor(ifstream& stream,const vector<FMTtheme>& themes,const FMTconstants& cons)
    {
    smatch kmatch;
    string line;
	while (!_included.empty())
		{
		line = _included.front();
		_included.pop();
		return line;
		}
	if(_forvalues.empty())
		{
		map<string,vector<string>>loops_replaces;
		map<string,pair<int,int>>forlocation;
		int lineid = 0;
		stack<string>keys;
		vector<pair<int, string>>Sequence;
		vector<string>ordered_keys;
		do{

			line = getcleanline(stream);


			//check for include here



			vector<string>allvalues;
			string target;
			bool gotfor = getforloops(line, themes, cons, allvalues, target);
			if (!gotfor && line.find("FOREACH")!=string::npos) //gay fix for multiple lines...
				{

				do {
					line += getcleanline(stream);
				} while (line.find(")") == string::npos);
				gotfor = getforloops(line, themes, cons, allvalues, target);
				}
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "got for? " << gotfor << "\n";
			if (gotfor)
				{
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " got for on  " << line << "\n";
				loops_replaces[target] = allvalues;
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " Rsize " << loops_replaces[target].size() << "\n";
				forlocation[target] = pair<int,int>(lineid,0);
				keys.push(target);
				}else if(keys.empty())
					{
					//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "no keys "<<line<< "\n";
					return returninclude(line,themes,cons);
					}else if(regex_search(line,kmatch,FMTparser::rxend))
							{
							string endfor = keys.top();
							keys.pop();
							forlocation[endfor].second = lineid;
							//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " START " << forlocation[endfor].first << "\n";
							//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " STOP " << forlocation[endfor].second << "\n";
							ordered_keys.push_back(endfor);
					}else if (isvalid(line)){
					//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "out here?" << "\n";
					//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " in " << line << "\n";
					Sequence.push_back(pair<int,string>(lineid,line));
					++lineid;
					}
		} while (!keys.empty());

		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "Werid!!!!!! " << Sequence.size() << "\n";


		//deal with the fucting alllines thats is too short!!!!
		for (const string& key : ordered_keys)
			{
			vector<pair<int, string>>frontsequence;
			vector<pair<int, string>>backsequence;
			vector<pair<int, string>>process;
			for (pair<int, string> tline : Sequence)
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
			vector<pair<int, string>>midsequence;
			for (const string& value : loops_replaces[key])
				{
				for (pair<int, string> tline : process)
					{
					//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " REPLACING " << key << "\n";
					//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info)<<" IN " << tline.second << "\n";
					midsequence.push_back(pair<int, string>(tline.first, std::regex_replace(tline.second, regex(key), value)));
					//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " OUT " << std::regex_replace(tline.second, regex(key), value) << "\n";
					}
				}
			Sequence.clear();
			Sequence.insert(Sequence.end(), frontsequence.begin(), frontsequence.end());
			Sequence.insert(Sequence.end(), midsequence.begin(), midsequence.end());
			Sequence.insert(Sequence.end(), backsequence.begin(), backsequence.end());
			}
			for (const pair<int, string>& nline : Sequence)
				{
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " to for loops " << nline.second << "\n";
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

	map<FMTwssect, string> FMTparser::getprimary(const string& primarylocation)
	{
		ifstream primarystream(primarylocation);
		map<FMTwssect, string>targets;
		boost::filesystem::path primary_path(primarylocation);
		if (FMTparser::tryopening(primarystream, primarylocation))
		{
			string line;
			while (primarystream.is_open())
			{
				line = getcleanline(primarystream);
				if (isvalid(line))
				{
					smatch kmatch;
					if (regex_search(line, kmatch, rxprimary))
					{
						string file_name = string(kmatch[3]);
						boost::filesystem::path file_path(file_name);
						string extension = boost::filesystem::extension(file_path.string());
						boost::to_lower(extension);
						FMTwssect section = from_extension(extension);
						targets[section] = (primary_path.parent_path() / file_path).string();
					}
				}
			}
		}
		return targets;
	}

    vector<string> FMTparser::sameas(const string& allset) const
        {
        vector<string>all_elements;
        string separator = "_SAMEAS";//neeed sameas for operator and transition...
        if (allset.find(separator)!=string::npos)
            {
            string realname = allset.substr(0, allset.find(separator));
            boost::trim(realname);
            all_elements.push_back(realname);
            string sameasstr = allset.substr(allset.find(separator)+separator.size());
            boost::trim(sameasstr);
            all_elements.push_back(sameasstr);
            }else{
            string realname = allset;
            boost::trim(realname);
            all_elements.push_back(realname);
            }
        return all_elements;
        }


istream& FMTparser::safeGetline(istream& is, string& t) const
	{
	t.clear();

	// The characters in the stream are read one-by-one using a std::streambuf.
	// That is faster than reading them one-by-one using the std::istream.
	// Code that uses streambuf this way must be guarded by a sentry object.
	// The sentry object performs various tasks,
	// such as thread synchronization and updating the stream state.

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
			// Also handle the case when the last line has no line ending
			if (t.empty())
				is.setstate(std::ios::eofbit);
			return is;
		default:
			t += (char)c;
		}
	}

	}

}
