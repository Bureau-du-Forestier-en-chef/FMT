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



namespace Wrapper
{

	FMTexcelcache::FMTexcelcache():
		cachelog(),
		models(new std::unordered_map<std::string, FMTmodelcache>()),
		exceptionraised(false)
	{
		cachelog = new std::shared_ptr<Logging::FMTlogger>(new Logging::FMTexcellogger());
		FMTmodelcache emptycache;
		emptycache.putlogger(*cachelog);

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
		if (cachelog != nullptr)
			{
			delete cachelog;
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

	std::string FMTexcelcache::getprimaryname(const std::string& primarylocation)
	{
		std::string primaryname;
		try {
			std::string base_filename = primarylocation.substr(primarylocation.find_last_of("/\\") + 1);
			std::string::size_type const fit(base_filename.find_last_of('.'));
			primaryname = base_filename.substr(0, fit);
		}catch (const std::exception& exception)
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
		catch (const std::exception& exception)
		{
			captureexception("FMTexcelcache::mapname");
		}
		return mappath;
		}

	void FMTexcelcache::add(System::String^ primarylocation, System::String^ scenario)
	{
		try {
			msclr::interop::marshal_context context;
			const std::string pfile = context.marshal_as<std::string>(primarylocation);
			const std::string sfile = context.marshal_as<std::string>(scenario);
			const std::string justprimary = getprimaryname(pfile);
			std::string naming = justprimary + "~" + sfile;
			boost::to_upper(naming);
			Parser::FMTmodelparser mparser;
			std::vector<Exception::FMTexc>errors;
			errors.push_back(Exception::FMTexc::FMTmissingyield);
			errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
			errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
			errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
			errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
			errors.push_back(Exception::FMTexc::FMToveridedyield);
			errors.push_back(Exception::FMTexc::FMTinvalid_geometry);
			errors.push_back(Exception::FMTexc::FMTsourcetotarget_transition);
			errors.push_back(Exception::FMTexc::FMTsame_transitiontargets);
			mparser.seterrorstowarnings(errors);
			std::vector<std::string>scenarios(1, sfile);
			const std::vector<Models::FMTmodel> allmodels = mparser.readproject(pfile, scenarios);
			const std::vector<std::vector<Core::FMTschedule>>allschedule = mparser.readschedules(pfile, allmodels);
			const std::string mappath = getmappath(pfile);
			(*models)[naming] = FMTmodelcache(allmodels.at(0), allschedule.at(0), mappath);
		}catch (...)
		{
			captureexception("FMTexcelcache::add");
		}
	}

	void FMTexcelcache::remove(System::String^ primarylocation, System::String^ scenario)
		{
		try {
			msclr::interop::marshal_context context;
			const std::string pfile = context.marshal_as<std::string>(primarylocation);
			const std::string sfile = context.marshal_as<std::string>(scenario);
			const std::string justprimary = getprimaryname(pfile);
			const std::string naming = justprimary + "~" + sfile;
			if (models->find(naming)!= models->end())
				{
				models->erase(naming);
				}
		}catch (const std::exception& exception)
			{
			captureexception("FMTexcelcache::remove");
			}
		}

	System::Collections::Generic::List<int>^ FMTexcelcache::getperiods(System::String^ primaryname, System::String^ scenario)
	{
		System::Collections::Generic::List<int>^ list = gcnew System::Collections::Generic::List<int>();
		try {
			msclr::interop::marshal_context context;
			const std::string pfile = context.marshal_as<std::string>(primaryname);
			const std::string sfile = context.marshal_as<std::string>(scenario);
			const std::string naming = pfile + "~" + sfile;
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
			msclr::interop::marshal_context context;
			const std::string pfile = context.marshal_as<std::string>(primaryname);
			const std::string sfile = context.marshal_as<std::string>(scenario);
			const std::string naming = pfile + "~" + sfile;
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
			msclr::interop::marshal_context context;
			const std::string pfile = context.marshal_as<std::string>(primaryname);
			const std::string sfile = context.marshal_as<std::string>(scenario);
			const std::string naming = pfile + "~" + sfile;
			std::unordered_map<std::string,FMTmodelcache>::const_iterator mit = models->find(naming);
			if (mit!=models->end())//crash wrong definition
				{
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
			msclr::interop::marshal_context context;
			const std::string pfile = context.marshal_as<std::string>(primaryname);
			const std::string sfile = context.marshal_as<std::string>(scenario);
			const std::string naming = pfile + "~" + sfile;
			std::unordered_map<std::string, FMTmodelcache>::const_iterator mit = models->find(naming);
			if (mit != models->end())//crash wrong definition
			{
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

	System::Collections::Generic::List<System::String^>^ FMTexcelcache::getattributes(System::String^ primaryname, System::String^ scenario, int themeid ,System::String^ value)
	{
		System::Collections::Generic::List<System::String^>^ list = gcnew System::Collections::Generic::List<System::String^>();
		try {
			msclr::interop::marshal_context context;
			const std::string pfile = context.marshal_as<std::string>(primaryname);
			const std::string sfile = context.marshal_as<std::string>(scenario);
			const std::string naming = pfile + "~" + sfile;
			std::unordered_map<std::string, FMTmodelcache>::const_iterator mit = models->find(naming);
			if (mit != models->end())
				{
				const std::string valueof = context.marshal_as<std::string>(value);
				for (const std::string& value : mit->second.getattributes(themeid, valueof))
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

	System::Collections::Generic::List<System::String^>^ FMTexcelcache::getaggregates(System::String^ primaryname, System::String^ scenario, int themeid)
	{
		System::Collections::Generic::List<System::String^>^ list = gcnew System::Collections::Generic::List<System::String^>();
		try {
			msclr::interop::marshal_context context;
			const std::string pfile = context.marshal_as<std::string>(primaryname);
			const std::string sfile = context.marshal_as<std::string>(scenario);
			const std::string naming = pfile + "~" + sfile;
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

	System::Collections::Generic::List<System::String^>^ FMTexcelcache::getactions(System::String^ primaryname, System::String^ scenario, System::String^ filter)
	{
		System::Collections::Generic::List<System::String^>^ list = gcnew System::Collections::Generic::List<System::String^>();
		try {
			msclr::interop::marshal_context context;
			const std::string pfile = context.marshal_as<std::string>(primaryname);
			const std::string sfile = context.marshal_as<std::string>(scenario);
			const std::string sfilter = context.marshal_as<std::string>(filter);
			const std::string naming = pfile + "~" + sfile;
			std::unordered_map<std::string, FMTmodelcache>::const_iterator mit = models->find(naming);
			if (mit != models->end())
			{
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

	System::Collections::Generic::List<System::String^>^ FMTexcelcache::getoutputs(System::String^ primaryname, System::String^ scenario)
	{
		System::Collections::Generic::List<System::String^>^ list = gcnew System::Collections::Generic::List<System::String^>();
		try {
			msclr::interop::marshal_context context;
			const std::string pfile = context.marshal_as<std::string>(primaryname);
			const std::string sfile = context.marshal_as<std::string>(scenario);
			const std::string naming = pfile + "~" + sfile;
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
			msclr::interop::marshal_context context;
			const std::string pfile = context.marshal_as<std::string>(primaryname);
			const std::string sfile = context.marshal_as<std::string>(scenario);
			const std::string naming = pfile + "~" + sfile;
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
			msclr::interop::marshal_context context;
			const std::string pfile = context.marshal_as<std::string>(primaryname);
			const std::string sfile = context.marshal_as<std::string>(scenario);
			const std::string naming = pfile + "~" + sfile;
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

	void FMTexcelcache::unraiseexception()
	{
		exceptionraised = false;
	}


}