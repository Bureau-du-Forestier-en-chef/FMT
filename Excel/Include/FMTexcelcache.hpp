#pragma once
#include "stdafx.h"
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include "FMTmodelcache.hpp"

namespace Logging
{
	class FMTlogger;
}

namespace Parser
{
	class FMTmodelparser;
}

namespace Exception
{
	class FMTexcelexceptionhandler;
}


namespace Wrapper
{

	public ref class FMTexcelcache 
	{
		Parser::FMTmodelparser* parser;
		std::shared_ptr<Logging::FMTlogger>* cachelog;
		std::shared_ptr<Exception::FMTexcelexceptionhandler>* cacheexceptionhandler;
		std::unordered_map<std::string, FMTmodelcache>* models;
		bool exceptionraised;
		std::string getprimaryname(const std::string& primarylocation);
		std::string getmappath(const std::string& primarylocation);
		void captureexception(const std::string& method);
		std::string formatforcache(System::String^ primarylocation, System::String^ scenario);
	public:
		FMTexcelcache();
		~FMTexcelcache();
		System::Collections::Generic::List<System::String^>^ readnsolvetemplates(System::String^ primarylocation, System::String^ templatefolder, int length, bool solve);
		bool add(System::String^ primarylocation, System::String^ scenario);
		void remove(System::String^ primarylocation, System::String^ scenario);
		double getvalue(System::String^ primaryname, System::String^ scenario,
			System::String^ outputname, System::String^ themeselection,int period);
		double getyield(System::String^ primaryname, System::String^ scenario,
			System::String^ yieldname, System::String^ themeselection, int period, int age);
		System::Collections::Generic::List<System::String^>^ getattributes(System::String^ primaryname, System::String^ scenario,int themeid, System::String^ value, bool aggregates);
		System::Collections::Generic::List<System::String^>^ getattributesdescription(System::String^ primaryname, System::String^ scenario, int themeid, System::String^ value);
		System::Collections::Generic::List<System::String^>^ getaggregates(System::String^ primaryname, System::String^ scenario, int themeid);
		System::Collections::Generic::List<System::String^>^ getprimaries();
		System::Collections::Generic::List<System::String^>^ getscenarios(System::String^ fichierprimaire);
		System::Collections::Generic::List<System::String^>^ getactions(System::String^ primaryname, System::String^ scenario, System::String^ filter);
		System::Collections::Generic::List<System::String^>^ getactionaggregates(System::String^ primaryname, System::String^ scenario, System::String^ filter);
		System::Collections::Generic::List<System::String^>^ getoutputs(System::String^ primaryname, System::String^ scenario);
		System::Collections::Generic::List<System::String^>^ getyields(System::String^ primaryname, System::String^ scenario);
		System::Collections::Generic::List<System::String^>^ getthemes(System::String^ primaryname, System::String^ scenario);
		System::Collections::Generic::List<System::String^>^ getbuildexceptions(System::String^ primaryname, System::String^ scenario, int exception);
		System::Collections::Generic::List<System::String^>^ getnochoice(System::String^ primaryname, System::String^ scenario, System::String^ filter);
		System::Collections::Generic::Dictionary<System::String^,System::Collections::Generic::List<int>^>^ getrotations(System::String^ primaryname, System::String^ scenario, System::String^ themeselection, System::String^ aggregate);
		System::Collections::Generic::List<int>^ getperiods(System::String^ primaryname, System::String^ scenario);
		System::Collections::Generic::List<System::String^>^ getconstraints(System::String^ primaryname, System::String^ scenario, System::String^ output);
		System::Collections::Generic::List<int>^ getgraphstats(System::String^ primaryname, System::String^ scenario);
		bool writejpeg(System::String^ jpeglocation, System::String^ primaryname, System::String^ scenario, int themeid, System::Collections::Generic::List<System::String^>^ attributes);
		int size();
		bool gotexception();
		void unraiseexception();
		System::String^ getlogoutput();
		System::Collections::Generic::List<double>^ Juxtaposition(System::String^ primaryname, System::String^ scenario,System::Collections::Generic::List<System::String^>^ themeselection, System::String^ yieldname, System::String^ output,double ratio,double perimeters);

	};
}