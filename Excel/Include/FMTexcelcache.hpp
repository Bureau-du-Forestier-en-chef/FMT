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


namespace Wrapper
{

	public ref class FMTexcelcache 
	{
		std::shared_ptr<Logging::FMTlogger>* cachelog;
		std::unordered_map<std::string, FMTmodelcache>* models;
		bool exceptionraised;
		std::string getprimaryname(const std::string& primarylocation);
		void captureexception(const std::string& method);
	public:
		FMTexcelcache();
		~FMTexcelcache();
		void add(System::String^ primarylocation, System::String^ scenario);
		void remove(System::String^ primarylocation, System::String^ scenario);
		double getvalue(System::String^ primaryname, System::String^ scenario,
			System::String^ outputname, System::String^ themeselection,int period);
		double getyield(System::String^ primaryname, System::String^ scenario,
			System::String^ yieldname, System::String^ themeselection, int period, int age);
		System::Collections::Generic::List<System::String^>^ getattributes(System::String^ primaryname, System::String^ scenario,int themeid, System::String^ value);
		System::Collections::Generic::List<System::String^>^ getaggregates(System::String^ primaryname, System::String^ scenario, int themeid);
		System::Collections::Generic::List<System::String^>^ getprimaries();
		System::Collections::Generic::List<System::String^>^ getscenarios(System::String^ fichierprimaire);
		System::Collections::Generic::List<System::String^>^ getactions(System::String^ primaryname, System::String^ scenario);
		System::Collections::Generic::List<System::String^>^ getoutputs(System::String^ primaryname, System::String^ scenario);
		System::Collections::Generic::List<System::String^>^ getyields(System::String^ primaryname, System::String^ scenario);
		System::Collections::Generic::List<System::String^>^ getthemes(System::String^ primaryname, System::String^ scenario);
		System::Collections::Generic::List<int>^ getperiods(System::String^ primaryname, System::String^ scenario);
		int size();
		bool gotexception();
		void unraiseexception();
		System::String^ getlogoutput();

	};
}