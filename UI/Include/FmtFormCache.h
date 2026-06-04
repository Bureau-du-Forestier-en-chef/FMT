#pragma once
#include "stdafx.h"
#include <vector>
#include <string>
#include <memory>
#include "FMTexception.h"
#include "FMTmodelparser.h"

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
	class FMTFormCache
	{
		public:
			~FMTFormCache()=default;
			const Models::FMTmodel& getmodel(const int& index) const;
			FMTexceptionhandlerwarning* GetFormHandler();
			const FMTexceptionhandlerwarning* GetFormHandler() const;
			Exception::FMTexceptionhandler* GetExceptionHandler() const;
			FMTFormLogger* GetFormLogger();
			static FMTFormCache* GetInstance();
			bool empty() const;
			void push_back(const Models::FMTmodel& model);
			void erase(const int& index);
			void clear();
			void InitializeExceptionHandler(const int& maxwarnings, const std::vector<Exception::FMTexc>& warning);
			void InitializeLogger(const std::string& filename,System::IntPtr intptrptr);
			Parser::FMTmodelparser GetConfiguredParser() const;
			FMTFormCache(const FMTFormCache& rhs) = delete;
			FMTFormCache& operator =(const FMTFormCache& rhs) = delete;
		private:
			FMTFormCache()=default;
			std::vector<std::unique_ptr<Models::FMTmodel>> m_Models;
			static std::unique_ptr<FMTFormCache> m_Instance;
			
			

	};
}