#pragma once
#include "stdafx.h"
#include <vector>
#include <string>
#include <memory>
#include "FMTexception.h"

namespace Models
{
	class FMTmodel;
}

namespace Exception
{
	class FMTexceptionhandler;
	class FMTLogger;
}

namespace Wrapper
{
	class FMTFormLogger;
	class FMTexceptionhandlerwarning;
	public class FMTformCache
	{
		/*Exception::FMTexceptionhandler* ExceptHandler;*/
		Parser::FMTmodelparser* ModelParser;
		std::vector<std::unique_ptr<Models::FMTmodel>> Models;
		Logging::FMTlogger* Logger; 
		Exception::FMTexceptionhandler* getExceptionHandler() const;
		public:
			FMTformCache();
			~FMTformCache();
			const Models::FMTmodel& getmodel(const int& index) const;
			FMTexceptionhandlerwarning* getformhandler();
			FMTFormLogger* getformlogger();
			void push_back(const Models::FMTmodel& model);
			void erase(const int& index);
			void clear();
			void initializeexceptionhandler(const int& maxwarnings, const std::vector<Exception::FMTexc>& warning);
			void initializelogger(const std::string& filename,System::IntPtr intptrptr);
			bool empty() const;
			

	};
}