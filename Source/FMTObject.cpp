/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#include "FMTObject.h"
#include "FMTExceptionHandler.h"
#include "FMTLogger.h"
#include "FMTDefaultLogger.h"
#include "FMTDebugLogger.h"
#include "FMTQuietLogger.h"
#include "FMTTaskLogger.h"
#include "FMTQuietExceptionHandler.h"
#include "FMTDefaultExceptionHandler.h"
#include "FMTDebugExceptionHandler.h"
#include "FMTFreeExceptionHandler.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/stacktrace.hpp>
#include <exception>
#include <chrono>
#include <csignal>

#if defined __MINGW64__||__CYGWIN__
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
	#include "windows.h"
	EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#else
    #include <boost/dll/runtime_symbol_info.hpp>
#endif

#if defined FMTWITHR
	#include "Rcpp.h"
#endif

#include "FMTcplhandler.h"

#if defined _WIN32
#ifndef NOMINMAX
	#define NOMINMAX
#endif
#include <windows.h>
#endif

#if defined __unix
#include <sys/sysinfo.h>
#endif

#if defined (_MSC_VER)
#ifndef NOMINMAX
	#define NOMINMAX
#endif
#include <comdef.h>
#include <windows.h>
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#include <boost/filesystem.hpp>
#endif

#if defined FMTWITHPYTHON
	#include <boost/python.h>
#endif


namespace Core
{

	std::unique_ptr<Logging::FMTLogger> FMTObject::_logger = std::move(std::unique_ptr<Logging::FMTLogger>(new Logging::FMTDefaultLogger()));
	std::unique_ptr<Exception::FMTExceptionHandler> FMTObject::_exhandler = std::move(std::unique_ptr<Exception::FMTExceptionHandler>(new Exception::FMTDefaultExceptionHandler(_logger)));

	Logging::FMTLogger* FMTObject::getLogger()
	{
		return _logger.get();
	}

	Exception::FMTExceptionHandler* FMTObject::getExceptionHandler()
	{
		return _exhandler.get();
	}


	unsigned long long FMTObject::getAvailableMemory()
	{
		unsigned long long available = 0;
		try {
			#if defined _WIN32
			MEMORYSTATUSEX status;
			status.dwLength = sizeof(status);
			GlobalMemoryStatusEx(&status);
			available = static_cast<unsigned long long>(status.ullAvailPhys);
			#endif
			#if defined __unix
			struct sysinfo i;
			short status = sysinfo(&i);
			available = static_cast<unsigned long long>(sysinfo.freeram*sysinfo.mem_unit);
			#endif
		}catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTObject::getAvailableMemory", __LINE__, __FILE__);
			}
		return available;
	}


	std::string  FMTObject::getRuntimeLocation()
	{
		std::string strDLLpath;
		try {
			WCHAR   DllPath[MAX_PATH] = { 0 };
			GetModuleFileNameW((HINSTANCE)&__ImageBase, DllPath, boost::size(DllPath));
			std::wstring wstrpath(DllPath);
			const std::string strpath(wstrpath.begin(), wstrpath.end());
	#if defined (_MSC_VER)
			const boost::filesystem::path boost_path(strpath);

	#elif defined __MINGW64__ || __CYGWIN__
			std::string clean_path;
			if (strpath.find(":")!= std::string::npos)
				{
				 clean_path = strpath.substr(strpath.find(":")-1);
				 if (strpath.find("\\")!= std::string::npos)
					{
					std::replace( clean_path.begin(), clean_path.end(), '\\', '/');
					}
				}
			boost::filesystem::path boost_path(clean_path);

	#else
			int var;
			boost::filesystem::path boost_path = boost::dll::symbolm_location(var);
	#endif
			strDLLpath = boost_path.parent_path().string();
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTObject::getRuntimeLocation", __LINE__, __FILE__);
			}
		return strDLLpath;
	}

	void FMTObject::checkSignals() const
	{
		if (_exhandler)
		{
			_exhandler->checkSignals();
			_exhandler->reRaiseIfThreadCrash();
		}
	}

		void FMTObject::setCPLhandler()
			{
			#if defined  FMTWITHGDAL
				if (_exhandler)
					{
					Exception::FMTExceptionHandler* handler = reinterpret_cast<Exception::FMTExceptionHandler*>(CPLGetErrorHandlerUserData());
					if (handler)
						{
						CPLPopErrorHandler();
						}
					CPLPushErrorHandlerEx(Exception::FMTCPLErrorHandler, _exhandler->getCPLdata());
					}
			#endif
			}


	FMTObject::FMTObject()
	{
		this->checkSignals();
	}

	FMTObject::~FMTObject()
	{
		//this->checkSignals();

	}

	void FMTObject::setTerminateStack()
		{
		std::set_terminate(&FMTObject::_terminate);
		}

	void FMTObject::_logStack()
		{
		std::string baseLine;
		std::vector<std::string>Splitted;
		const std::string TRACE = boost::stacktrace::to_string(boost::stacktrace::stacktrace());
		boost::split(Splitted, TRACE, boost::is_any_of("\n"));
		for (const auto& TRACEm_line : Splitted)
			{
				if (!TRACEm_line.empty())
					{
					*_logger << baseLine << TRACEm_line << "\n";
					baseLine += "\t";
					}


			}
		}

	void FMTObject::_abort(int p_signal)
		{
		*_logger << "Aborted due to error " << "\n";
		_logStack();
		//std::_Exit(EXIT_FAILURE);
		_exhandler->raise(Exception::FMTexc::FMTunhandlederror,
			"Abort called",
			"FMTObject::_abort", __LINE__, __FILE__);
		}

	void FMTObject::setAbortStack()
		{
		if (std::signal(SIGABRT, &FMTObject::_abort) == SIG_ERR)
			{
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
				"Cannot set Abort signal",
				"FMTObject::setAbortStack", __LINE__, __FILE__);
			}
		}

	void FMTObject::_terminate()
	{
		*_logger << "Terminated due to error" << "\n";
		_logStack();
		//std::_Exit(EXIT_FAILURE);
		_exhandler->raise(Exception::FMTexc::FMTunhandlederror,
			"Terminate called",
			"FMTObject::_terminate", __LINE__, __FILE__);
		
	}


	FMTObject::FMTObject(const std::unique_ptr<Exception::FMTExceptionHandler> exhandler)
	{
		_exhandler = std::move(exhandler->Clone());
		_exhandler->passInLogger(_logger);
		this->checkSignals();

	}
	FMTObject::FMTObject(const FMTObject& rhs)
	{
		this->checkSignals();
	}
	FMTObject& FMTObject::operator = (const FMTObject& rhs)
	{
		this->checkSignals();
		return *this;
	}
	void FMTObject::passInLogger(const std::unique_ptr<Logging::FMTLogger>& logger)
		{
		try{
			this->checkSignals();
			_logger = logger->Clone();
			_exhandler->passInLogger(_logger);
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTObject::passInLogger", __LINE__, __FILE__);
			}
		}

	void FMTObject::passInExceptionHandler(const std::unique_ptr<Exception::FMTExceptionHandler>& exhandler)
		{
		try{
			this->checkSignals();
			_exhandler = exhandler->Clone();
			_exhandler->passInLogger(_logger);
			setCPLhandler();
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTObject::passInExceptionHandler", __LINE__, __FILE__);
			}
		}


	void FMTObject::redirectLogToFile(const std::string& location)
		{
		_logger->redirectToFile(location);
		this->checkSignals();
		}

	void FMTObject::setDefaultLogger()
		{
		try {
			this->checkSignals();
			this->passInLogger(std::unique_ptr<Logging::FMTLogger>(new Logging::FMTDefaultLogger()));
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTObject::setDefaultLogger", __LINE__, __FILE__);
			}
		}

	void FMTObject::setQuietLogger()
		{
		try{
			this->checkSignals();
			this->passInLogger(std::unique_ptr<Logging::FMTLogger>(new Logging::FMTQuietLogger()));
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTObject::setQuietLogger", __LINE__, __FILE__);
		}
		}

	void FMTObject::setTaskLogger()
	{
		try {
			this->checkSignals();
			this->passInLogger(std::unique_ptr<Logging::FMTLogger>(new Logging::FMTTaskLogger()));

		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTObject::setTaskLogger", __LINE__, __FILE__);
		}
	}

	void FMTObject::setDebugLogger()
		{
		try {
			this->checkSignals();
			this->passInLogger(std::unique_ptr<Logging::FMTLogger>(new Logging::FMTDebugLogger()));
		}	
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTObject::setDebugLogger", __LINE__, __FILE__);
		}
		}

	void FMTObject::setDefaultExceptionHandler()
		{
		try{
			this->checkSignals();
			this->passInExceptionHandler(std::unique_ptr<Exception::FMTExceptionHandler>(new Exception::FMTDefaultExceptionHandler()));
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTObject::setDefaultExceptionHandler", __LINE__, __FILE__);
		}
		}
	void FMTObject::setQuietExceptionHandler()
	{
		try{
			this->checkSignals();
			this->passInExceptionHandler(std::unique_ptr<Exception::FMTExceptionHandler>(new Exception::FMTQuietExceptionHandler()));
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTObject::setQuietExceptionHandler", __LINE__, __FILE__);
			}
	}
	void FMTObject::setDebugExceptionHandler()
	{
		try{
		this->checkSignals();
		this->passInExceptionHandler(std::unique_ptr<Exception::FMTExceptionHandler>(new Exception::FMTDebugExceptionHandler()));
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTObject::setDebugExceptionHandler", __LINE__, __FILE__);
		}
	}

	void  FMTObject::setFreeExceptionHandler()
	{
		try{
			this->checkSignals();
			this->passInExceptionHandler(std::unique_ptr<Exception::FMTExceptionHandler>(new Exception::FMTFreeExceptionHandler()));
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTObject::setFreeExceptionHandler", __LINE__, __FILE__);
		}
	}

	void FMTObject::disableNestedExceptions()
		{
		try {
			this->checkSignals();
			_exhandler->disableNestedExceptions();
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTObject::disableNestedExceptions", __LINE__, __FILE__);
			}
		}

	void FMTObject::enableNestedExceptions()
		{
		try{
			this->checkSignals();
			_exhandler->enableNestedExceptions();
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTObject::enableNestedExceptions", __LINE__, __FILE__);
			}
		}

	void FMTObject::setErrorsToWarnings(const std::vector<Exception::FMTexc>& errors)
	{
		try {
			_exhandler->setErrorsToWarnings(errors);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTObject::setErrorsToWarnings", __LINE__, __FILE__);
		}
	}

	void FMTObject::setMaxWarningsBeforeSilenced(const size_t& maxwarningcount)
	{
		try {
			_exhandler->setMaxWarningsBeforeSilenced(maxwarningcount);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTObject::setMaxWarningsBeforeSilenced", __LINE__, __FILE__);
		}
	}

	std::chrono::time_point<std::chrono::high_resolution_clock> FMTObject::getClock()
		{
		std::chrono::time_point<std::chrono::high_resolution_clock> newclock;
		try {
			newclock = std::chrono::high_resolution_clock::now();
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTObject::getClock", __LINE__, __FILE__);
			}
		return newclock;
		}


	template<class chrono>
	double FMTObject::getDuration(const std::chrono::time_point<std::chrono::high_resolution_clock>& startclock)
	{
		double result = 0;
		try {
			const std::chrono::time_point<std::chrono::high_resolution_clock> stopclock = getClock();
			const std::chrono::duration<double, chrono>spent(stopclock - startclock);
			result = spent.count();
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTObject::getDuration", __LINE__, __FILE__);
		}
		return result;
	}

	template double FMTObject::getDuration<std::chrono::milliseconds::period>(const std::chrono::time_point<std::chrono::high_resolution_clock>& startclock);
	template double FMTObject::getDuration<std::chrono::seconds::period>(const std::chrono::time_point<std::chrono::high_resolution_clock>& startclock);
	template double FMTObject::getDuration<std::chrono::minutes::period>(const std::chrono::time_point<std::chrono::high_resolution_clock>& startclock);
	template double FMTObject::getDuration<std::chrono::hours::period>(const std::chrono::time_point<std::chrono::high_resolution_clock>& startclock);

	std::string FMTObject::getDurationInSeconds(const std::chrono::time_point<std::chrono::high_resolution_clock>& startclock)
	{
		std::string value;
		try {
			const double dblvalue = getDuration<std::chrono::seconds::period>(startclock);
			value = "in "+std::to_string(dblvalue)+" seconds";
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTObject::getDurationInSeconds", __LINE__, __FILE__);
			}
		return value;
	}


}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTObject)
