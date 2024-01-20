#include "stdafx.h"
#include "FMTexcelcache.hpp"
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include "FMTlpmodel.hpp"
#include "FMToutput.hpp"
#include "FMTtheme.hpp"
#include "FMTmask.hpp"
#include "FMTschedule.hpp"
#include "FMTexceptionhandler.hpp"
#include "FMTexception.hpp"
#include "FMTobject.hpp"
#include <msclr\marshal_cppstd.h>
#include <cliext/list>
#include "FMTmodelparser.hpp"
#include "FMTmodelcache.hpp"
#include "FMTexcellogger.hpp"
#include "FMTexception.hpp"
#include "FMTobject.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include "FMTexcelexceptionhandler.hpp"



namespace Wrapper
{

	FMTexcelcache::FMTexcelcache():
		parser(new Parser::FMTmodelparser()),
		cachelog(),
		cacheexceptionhandler(),
		models(new std::unordered_map<std::string, FMTmodelcache>()),
		exceptionraised(false)
	{
		cachelog = new std::shared_ptr<Logging::FMTlogger>(new Logging::FMTexcellogger());
		cacheexceptionhandler = new std::shared_ptr<Exception::FMTexcelexceptionhandler>(new Exception::FMTexcelexceptionhandler());
		parser->passinlogger(*cachelog);
		parser->passinexceptionhandler(*cacheexceptionhandler);
		//FMTmodelcache emptycache;
		//emptycache.putlogger(*cachelog);
		

	}

	bool FMTexcelcache::gotexception()
		{
		return exceptionraised;
		}
	

	System::String^ FMTexcelcache::getlogoutput()
		{
		FMTmodelcache emptycache;
		Logging::FMTexcellogger* log = emptycache.getlogger();
		std::string out;
		if (log != nullptr)
		{
			out = log->getprintout();
			log->clearout();
		}
		System::String^ str2 = gcnew System::String(out.c_str());
		return str2;
		}

	FMTexcelcache::~FMTexcelcache()
	{
		if (parser!=nullptr)
			{
			delete parser;
			}
		if (cachelog != nullptr)
			{
			delete cachelog;
			}
		if (cacheexceptionhandler != nullptr)
		{
			delete cacheexceptionhandler;
		}
		if (models!=nullptr)
			{
			delete models;
			}

	}

	void FMTexcelcache::captureexception(const std::string& method)
	{
		exceptionraised = true;
	}

	std::string FMTexcelcache::formatforcache(System::String^ primarylocation, System::String^ scenario)
	{
		std::string cachename;
		try {
			msclr::interop::marshal_context context;
			const std::string pfile = context.marshal_as<std::string>(primarylocation);
			const std::string sfile = context.marshal_as<std::string>(scenario);
			const std::string justprimary = getprimaryname(pfile);
			cachename = justprimary + "~" + sfile;
			boost::to_upper(cachename);
		}
		catch (...)
		{
			captureexception("FMTexcelcache::formatforcache");
		}
		return cachename;

	}

	std::string FMTexcelcache::getprimaryname(const std::string& primarylocation)
	{
		std::string primaryname;
		try {
			std::string base_filename = primarylocation.substr(primarylocation.find_last_of("/\\") + 1);
			std::string::size_type const fit(base_filename.find_last_of('.'));
			primaryname = base_filename.substr(0, fit);
		}catch (...)
		{
			captureexception("FMTexcelcache::getprimaryname");
		}
		return primaryname;
	}

	std::string FMTexcelcache::getmappath(const std::string& primarylocation)
		{
		std::string mappath;
		try {
			const boost::filesystem::path primpath(primarylocation);
			boost::filesystem::path pridir = primpath.parent_path();
			const boost::filesystem::path mapdir = pridir / boost::filesystem::path("Carte");
			if (boost::filesystem::is_directory(mapdir))
				{
				for (auto& entry : boost::make_iterator_range(boost::filesystem::directory_iterator(mapdir), {}))
					{
					std::string extension = entry.path().extension().string();
					boost::to_upper(extension);
					if (extension==".SHP")
						{
						mappath = std::string(entry.path().string());
						break;
						}
					}
				}
		}
		catch (...)
		{
			captureexception("FMTexcelcache::mapname");
		}
		return mappath;
		}

	System::Collections::Generic::List<System::String^>^ FMTexcelcache::readnsolvetemplates(System::String^ primarylocation, System::String^ templatefolder, int length,bool solve)
	{
		System::Collections::Generic::List<System::String^>^ scenarios = gcnew System::Collections::Generic::List<System::String^>();
		try {
			msclr::interop::marshal_context context;
			const std::string pfile = context.marshal_as<std::string>(primarylocation);
			const std::string sfile = context.marshal_as<std::string>(templatefolder);
			const std::vector<Models::FMTmodel> allmodels = parser->readtemplates(pfile, sfile);
			const std::string mappath = getmappath(pfile);
			for (const Models::FMTmodel& model :  allmodels)
			{
				const std::string name = model.getname();
				if (name!="ROOT")
				{
					System::String^ modelname = gcnew System::String(name.c_str());
					const std::string naming = formatforcache(primarylocation, modelname);
					(*models)[naming] = FMTmodelcache(model, mappath);
					(*models)[naming].setlength(length);
					if ((*models)[naming].buildnsolve(solve))
					{
						scenarios->Add(modelname);
					}
				}
			}
		}catch (...)
		{
			captureexception("FMTexcelcache::readnsolvetemplates");
		}
		return scenarios;
	}

	bool FMTexcelcache::add(System::String^ primarylocation, System::String^ scenario)
	{
		try {
			const std::string naming = formatforcache(primarylocation, scenario);
			if(models->find(naming)==models->end())
			{
				//Default behavior reload pas avec le même nom 
				msclr::interop::marshal_context context;
				const std::string pfile = context.marshal_as<std::string>(primarylocation);
				const std::string sfile = context.marshal_as<std::string>(scenario);
				std::vector<std::string>scenarios(1, sfile);
				const std::vector<Models::FMTmodel> allmodels = parser->readproject(pfile, scenarios);
				const std::vector<std::vector<Core::FMTschedule>>allschedule = parser->readschedules(pfile, allmodels);
				const std::string mappath = getmappath(pfile);
				int period = 0;
				for (const Core::FMTschedule& schedule : allschedule.at(0))
				{
					period = std::max(period, schedule.getperiod());
				}
				(*models)[naming] = FMTmodelcache(allmodels.at(0), mappath);
				(*models)[naming].setlength(period);
				//(*models)[naming] = FMTmodelcache(allmodels.at(0), allschedule.at(0), mappath);
				//Removed from constructor because of the copy after it's created, the solver started ....
				models->at(naming).setsolution(allschedule.at(0));
				return true;
			}else{
				FMTmodelcache emptycache;
				Logging::FMTexcellogger* log = emptycache.getlogger();
				if (log != nullptr)
				{
					log->clearout();
				}
			}
		}catch (...)
		{
			captureexception("FMTexcelcache::add");
		}
		return false;
	}

	void FMTexcelcache::remove(System::String^ primarylocation, System::String^ scenario)
		{
		try {
			const std::string naming = formatforcache(primarylocation, scenario);
			if (models->find(naming)!= models->end())
				{
				models->erase(naming);
				}
		}catch (...)
			{
			captureexception("FMTexcelcache::remove");
			}
		}

	System::Collections::Generic::List<int>^ FMTexcelcache::getperiods(System::String^ primaryname, System::String^ scenario)
	{
		System::Collections::Generic::List<int>^ list = gcnew System::Collections::Generic::List<int>();
		try {
			const std::string naming = formatforcache(primaryname, scenario);
			std::unordered_map<std::string, FMTmodelcache>::const_iterator mit = models->find(naming);
			if (mit != models->end())//crash wrong definition
			{
				for (int period = 1 ; period <= mit->second.getperiods();++period)
					{
					list->Add(period);
					}
			}
		}
		catch (...)
		{
			captureexception("FMTexcelcache::getperiods");
		}
		return list;
	}

	bool FMTexcelcache::writejpeg(System::String^ jpeglocation,System::String^ primaryname, System::String^ scenario, int themeid, System::Collections::Generic::List<System::String^>^ attributes)
	{
		try {
			const std::string naming = formatforcache(primaryname, scenario);
			std::unordered_map<std::string, FMTmodelcache>::const_iterator mit = models->find(naming);
			if (mit != models->end())//crash wrong definition
			{
				std::vector<std::string>values;
				msclr::interop::marshal_context context;
				for each (System::String ^ attribute in attributes)
					{
					values.push_back(context.marshal_as<std::string>(attribute));
					}
				const std::string location = context.marshal_as<std::string>(jpeglocation);
				return mit->second.writejpeg(static_cast<size_t>(themeid), values, location);
			}
		}catch (...)
		{
			captureexception("FMTexcelcache::writejpeg");
		}
		return false;
	}

	double FMTexcelcache::getvalue(System::String^ primaryname, System::String^ scenario,
		System::String^ outputname, System::String^ themeselection, int period)
	{
		double value = 0;
		try {
			const std::string naming = formatforcache(primaryname, scenario);
			std::unordered_map<std::string,FMTmodelcache>::const_iterator mit = models->find(naming);
			if (mit!=models->end())//crash wrong definition
				{
				msclr::interop::marshal_context context;
				const std::string outname = context.marshal_as<std::string>(outputname);
				const std::string selection = context.marshal_as<std::string>(themeselection);
				value = mit->second.getvalue(outname, selection, period);
				}
		}catch (...)
		{
			captureexception("FMTexcelcache::getvalue ");
		}
		return value;
	}

	

	double FMTexcelcache::getyield(System::String^ primaryname, System::String^ scenario,
		System::String^ yieldname, System::String^ themeselection, int period, int age)
	{
		double value = 0;
		try {
			const std::string naming = formatforcache(primaryname, scenario);
			std::unordered_map<std::string, FMTmodelcache>::const_iterator mit = models->find(naming);
			if (mit != models->end())//crash wrong definition
			{
				msclr::interop::marshal_context context;
				const std::string yieldnamec = context.marshal_as<std::string>(yieldname);
				const std::string selection = context.marshal_as<std::string>(themeselection);
				value = mit->second.getyield(yieldnamec, selection, age, period);
			}
			
		}
		catch (...)
		{
			captureexception("FMTexcelcache::getyield");
		}
		return value;
	}

	System::Collections::Generic::List<System::String^>^ FMTexcelcache::getattributes(System::String^ primaryname, System::String^ scenario, int themeid ,System::String^ value, bool aggregates)
	{
		System::Collections::Generic::List<System::String^>^ list = gcnew System::Collections::Generic::List<System::String^>();
		try {
			
			const std::string naming = formatforcache(primaryname, scenario);
			std::unordered_map<std::string, FMTmodelcache>::const_iterator mit = models->find(naming);
			if (mit != models->end())
				{
				msclr::interop::marshal_context context;
				const std::string valueof = context.marshal_as<std::string>(value);
				for (const std::string& value : mit->second.getattributes(themeid, valueof,aggregates))
					{
					System::String^ sysvalue = gcnew System::String(value.c_str());
					list->Add(sysvalue);
					}
				}
		}
		catch (...)
		{
			captureexception("FMTexcelcache::getattributes");
		}
		return list;
	}

	System::Collections::Generic::List<System::String^>^ FMTexcelcache::getbuildexceptions(System::String^ primaryname, System::String^ scenario, int exception)
	{
		System::Collections::Generic::List<System::String^>^ list = gcnew System::Collections::Generic::List<System::String^>();
		try {
			std::unordered_map<std::string, FMTmodelcache>::const_iterator mit = models->find(formatforcache(primaryname, scenario));
			if (mit != models->end())
			{
				for (const std::string& value : mit->second.getbuildexceptions(exception))
				{
					System::String^ sysvalue = gcnew System::String(value.c_str());
					list->Add(sysvalue);
				}
			}
		}
		catch (...)
		{
			captureexception("FMTexcelcache::getbuildexceptions");
		}
		return list;
	}

	System::Collections::Generic::List<System::String^>^ FMTexcelcache::getnochoice(System::String^ primaryname, System::String^ scenario, System::String^ filter)
	{
		System::Collections::Generic::List<System::String^>^ list = gcnew System::Collections::Generic::List<System::String^>();
		try {

			const std::string naming = formatforcache(primaryname, scenario);
			std::unordered_map<std::string, FMTmodelcache>::const_iterator mit = models->find(naming);
			if (mit != models->end())
			{
				msclr::interop::marshal_context context;
				const std::string sfilter = context.marshal_as<std::string>(filter);
				for (const std::string& value : mit->second.getnoaction(sfilter))
				{
					System::String^ sysvalue = gcnew System::String(value.c_str());
					list->Add(sysvalue);
				}
			}
		}
		catch (...)
		{
			captureexception("FMTexcelcache::getattributesdescription");
		}
		return list;
	}


	System::Collections::Generic::List<System::String^>^ FMTexcelcache::getattributesdescription(System::String^ primaryname, System::String^ scenario, int themeid, System::String^ value)
	{
		System::Collections::Generic::List<System::String^>^ list = gcnew System::Collections::Generic::List<System::String^>();
		try {

			const std::string naming = formatforcache(primaryname, scenario);
			std::unordered_map<std::string, FMTmodelcache>::const_iterator mit = models->find(naming);
			if (mit != models->end())
			{
				msclr::interop::marshal_context context;
				const std::string valueof = context.marshal_as<std::string>(value);
				for (const std::string& value : mit->second.getattributesdescription(themeid, valueof))
				{
					System::String^ sysvalue = gcnew System::String(value.c_str());
					list->Add(sysvalue);
				}
			}
		}
		catch (...)
		{
			captureexception("FMTexcelcache::getattributesdescription");
		}
		return list;

	}

	System::Collections::Generic::List<System::String^>^ FMTexcelcache::getaggregates(System::String^ primaryname, System::String^ scenario, int themeid)
	{
		System::Collections::Generic::List<System::String^>^ list = gcnew System::Collections::Generic::List<System::String^>();
		try {
			const std::string naming = formatforcache(primaryname, scenario);
			std::unordered_map<std::string, FMTmodelcache>::const_iterator mit = models->find(naming);
			if (mit != models->end())
			{
				for (const std::string& value : mit->second.getaggregates(themeid))
				{
					System::String^ sysvalue = gcnew System::String(value.c_str());
					list->Add(sysvalue);
				}
			}
		}
		catch (...)
		{
			captureexception("FMTexcelcache::getaggregates");
		}
		return list;
	}


	System::Collections::Generic::List<System::String^>^ FMTexcelcache::getprimaries()
	{
		System::Collections::Generic::List<System::String^>^ list = gcnew System::Collections::Generic::List<System::String^>();
		for (const auto& item : *models)
			{
			const std::string value = item.first.substr(0, item.first.find("~"));
			System::String^ str2 = gcnew System::String(value.c_str());
			if (!list->Contains(str2))
				{
				list->Add(str2);
				}
			}
		return list;
	}

	int FMTexcelcache::size()
		{
		return static_cast<int>(models->size());
		}

	System::Collections::Generic::List<System::String^>^ FMTexcelcache::getscenarios(System::String^ fichierprimaire)
	{
		System::Collections::Generic::List<System::String^>^ list = gcnew System::Collections::Generic::List<System::String^>();
		msclr::interop::marshal_context context;
		const std::string pfile = context.marshal_as<std::string>(fichierprimaire);
		for (const auto& item : *models)
		{
			if (item.first.find(pfile)!=std::string::npos)
			{
				const std::string value = item.first.substr(item.first.find("~") + 1, item.first.size());
				System::String^ str2 = gcnew System::String(value.c_str());
				list->Add(str2);
			}
			
		}
		return list;
	}

	System::Collections::Generic::List<System::String^>^ FMTexcelcache::getconstraints(System::String^ primaryname, System::String^ scenario, System::String^ output)
	{
		System::Collections::Generic::List<System::String^>^ list = gcnew System::Collections::Generic::List<System::String^>();
		try {
			const std::string naming = formatforcache(primaryname, scenario);
			std::unordered_map<std::string, FMTmodelcache>::const_iterator mit = models->find(naming);
			if (mit != models->end())
			{
				msclr::interop::marshal_context context;
				const std::string sfilter = context.marshal_as<std::string>(output);
				for (const std::string& value : mit->second.getconstraints(sfilter))
				{
					System::String^ sysvalue = gcnew System::String(value.c_str());
					list->Add(sysvalue);
				}
			}

		}
		catch (...)
		{
			captureexception("FMTexcelcache::getconstraints");
		}
		return list;
	}

	System::Collections::Generic::List<int>^ FMTexcelcache::getgraphstats(System::String^ primaryname, System::String^ scenario)
	{
		System::Collections::Generic::List<int>^ list = gcnew System::Collections::Generic::List<int>();
		try {
			const std::string naming = formatforcache(primaryname, scenario);
			std::unordered_map<std::string, FMTmodelcache>::const_iterator mit = models->find(naming);
			if (mit != models->end())
			{
				for (const int& value : mit->second.getgraphstats())
				{
					list->Add(value);
				}
			}

		}
		catch (...)
		{
			captureexception("FMTexcelcache::getgraphstats");
		}
		return list;
	}



	System::Collections::Generic::List<System::String^>^ FMTexcelcache::getactions(System::String^ primaryname, System::String^ scenario, System::String^ filter)
	{
		System::Collections::Generic::List<System::String^>^ list = gcnew System::Collections::Generic::List<System::String^>();
		try {
			const std::string naming = formatforcache(primaryname, scenario);
			std::unordered_map<std::string, FMTmodelcache>::const_iterator mit = models->find(naming);
			if (mit != models->end())
			{
				msclr::interop::marshal_context context;
				const std::string sfilter = context.marshal_as<std::string>(filter);
				for (const std::string& value : mit->second.getactions(sfilter))
				{
					System::String^ sysvalue = gcnew System::String(value.c_str());
					list->Add(sysvalue);
				}
			}

		}
		catch (...)
		{
			captureexception("FMTexcelcache::getactions");
		}
		return list;
	}

	System::Collections::Generic::List<System::String^>^ FMTexcelcache::getactionaggregates(System::String^ primaryname, System::String^ scenario, System::String^ filter)
	{
		System::Collections::Generic::List<System::String^>^ list = gcnew System::Collections::Generic::List<System::String^>();
		try {
			const std::string naming = formatforcache(primaryname, scenario);
			std::unordered_map<std::string, FMTmodelcache>::const_iterator mit = models->find(naming);
			if (mit != models->end())
			{
				msclr::interop::marshal_context context;
				const std::string sfilter = context.marshal_as<std::string>(filter);
				for (const std::string& value : mit->second.getactionaggregates(sfilter))
				{
					System::String^ sysvalue = gcnew System::String(value.c_str());
					list->Add(sysvalue);
				}
			}
		}catch (...)
		{
			captureexception("FMTexcelcache::getactionaggregates");
		}
		return list;
	}

	System::Collections::Generic::List<System::String^>^ FMTexcelcache::getoutputs(System::String^ primaryname, System::String^ scenario)
	{
		System::Collections::Generic::List<System::String^>^ list = gcnew System::Collections::Generic::List<System::String^>();
		try {
			const std::string naming = formatforcache(primaryname, scenario);
			std::unordered_map<std::string, FMTmodelcache>::const_iterator mit = models->find(naming);
			if (mit != models->end())
			{
				for (const std::string& value : mit->second.getoutputs())
				{
					System::String^ sysvalue = gcnew System::String(value.c_str());
					list->Add(sysvalue);
				}
			}

		}
		catch (...)
		{
			captureexception("FMTexcelcache::getoutputs");
		}
		return list;
	}

	System::Collections::Generic::List<System::String^>^ FMTexcelcache::getyields(System::String^ primaryname, System::String^ scenario)
	{
		System::Collections::Generic::List<System::String^>^ list = gcnew System::Collections::Generic::List<System::String^>();
		try {
			const std::string naming = formatforcache(primaryname, scenario);
			std::unordered_map<std::string, FMTmodelcache>::const_iterator mit = models->find(naming);
			if (mit != models->end())
			{
				for (const std::string& value : mit->second.getyields())
				{
					System::String^ sysvalue = gcnew System::String(value.c_str());
					list->Add(sysvalue);
				}
			}

		}
		catch (...)
		{
			captureexception("FMTexcelcache::getyields");
		}
		return list;
	}

	System::Collections::Generic::List<System::String^>^ FMTexcelcache::getthemes(System::String^ primaryname, System::String^ scenario)
	{
		System::Collections::Generic::List<System::String^>^ list = gcnew System::Collections::Generic::List<System::String^>();
		try {
			const std::string naming = formatforcache(primaryname, scenario);
			std::unordered_map<std::string, FMTmodelcache>::const_iterator mit = models->find(naming);
			if (mit != models->end())
			{
				for (const std::string& value : mit->second.getthemes())
				{
					System::String^ sysvalue = gcnew System::String(value.c_str());
					list->Add(sysvalue);
				}
			}

		}
		catch (...)
		{
			captureexception("FMTexcelcache::getthemes");
		}
		return list;
	}

	System::Collections::Generic::Dictionary<System::String^, System::Collections::Generic::List<int>^>^ FMTexcelcache::getrotations(System::String^ primaryname, System::String^ scenario, System::String^ themeselection, System::String^ aggregate)
	{
		System::Collections::Generic::Dictionary < System::String^, System::Collections::Generic::List<int>^>^ values = gcnew System::Collections::Generic::Dictionary<System::String^,System::Collections::Generic::List<int>^>();
		try {
			const std::string naming = formatforcache(primaryname, scenario);
			std::unordered_map<std::string, FMTmodelcache>::const_iterator mit = models->find(naming);
			if (mit != models->end())
				{
				msclr::interop::marshal_context context;
				const std::string sfilter = context.marshal_as<std::string>(themeselection);
				const std::string saggregate= context.marshal_as<std::string>(aggregate);
				for (const std::pair<std::string, int>& rotations : mit->second.getrotations(sfilter, saggregate))
					{
					System::String^ key = gcnew System::String(rotations.first.c_str());
					if (!values->ContainsKey(key))
						{
						values[key] = gcnew System::Collections::Generic::List<int>();
						}
					values[key]->Add(rotations.second);
					}

				}
		}
		catch (...)
		{
			captureexception("FMTexcelcache::getrotations");
		}
		return values;
	}


	System::Collections::Generic::List<double>^ FMTexcelcache::Juxtaposition(System::String^ primaryname, System::String^ scenario,System::Collections::Generic::List<System::String^>^ themeselection, System::String^ yieldname, System::String^ output, double ratio, double perimeters)
	{
		System::Collections::Generic::List<double>^ list = gcnew System::Collections::Generic::List<double>();
		try {
			const std::string naming = formatforcache(primaryname, scenario);
			std::unordered_map<std::string, FMTmodelcache>::const_iterator mit = models->find(naming);
			if (mit != models->end())
			{
				std::vector<std::string>themeselections;
				msclr::interop::marshal_context context;
				for each (System::String ^ ths in themeselection)
					{
					themeselections.push_back(context.marshal_as<std::string>(ths));
					}
				const std::string stroutput = context.marshal_as<std::string>(output);
				const std::string yieldof = context.marshal_as<std::string>(yieldname);
				for (const double& value : mit->second.Juxtaposition(themeselections, yieldof, stroutput, ratio, perimeters))
					{
					list->Add(value);
					}
			}

		}
		catch (...)
		{
			captureexception("FMTexcelcache::Juxtaposition");
		}
		return list;
	}

	void FMTexcelcache::unraiseexception()
	{
		exceptionraised = false;
	}


}