/*
Copyright (c) 2019 Gouvernement du Qubec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#include "FMTobject.h"
#include "FMTexceptionhandler.h"
#include "FMTlogger.h"
#include "FMTdefaultlogger.h"
#include "FMTdebuglogger.h"
#include "FMTquietlogger.h"
#include "FMTtasklogger.h"
#include "FMTquietexceptionhandler.h"
#include "FMTdefaultexceptionhandler.h"
#include "FMTdebugexceptionhandler.h"
#include "FMTfreeexceptionhandler.h"
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

	std::unique_ptr<Logging::FMTlogger> FMTobject::_logger = std::move(std::unique_ptr<Logging::FMTlogger>(new Logging::FMTdefaultlogger()));
	std::unique_ptr<Exception::FMTexceptionhandler> FMTobject::_exhandler = std::move(std::unique_ptr<Exception::FMTexceptionhandler>(new Exception::FMTdefaultexceptionhandler(_logger)));

	Logging::FMTlogger* FMTobject::getLogger()
	{
		return _logger.get();
	}

	Exception::FMTexceptionhandler* FMTobject::GetExceptionHandler()
	{
		return _exhandler.get();
	}


	unsigned long long FMTobject::getAvailableMemory()
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
				_exhandler->raisefromcatch("", "FMTobject::getavailablememory", __LINE__, __FILE__);
			}
		return available;
	}


	std::string  FMTobject::getruntimelocation()
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
			_exhandler->raisefromcatch("", "FMTobject::getruntimelocation", __LINE__, __FILE__);
			}
		return strDLLpath;
	}

	void FMTobject::checkSignals() const
	{
		if (_exhandler)
		{
			_exhandler->checkSignals();
			_exhandler->reRaiseIfThreadCrash();
		}
	}

		void FMTobject::setCPLhandler()
			{
			#if defined  FMTWITHGDAL
				if (_exhandler)
					{
					Exception::FMTexceptionhandler* handler = reinterpret_cast<Exception::FMTexceptionhandler*>(CPLGetErrorHandlerUserData());
					if (handler)
						{
						CPLPopErrorHandler();
						}
					CPLPushErrorHandlerEx(Exception::FMTCPLErrorHandler, _exhandler->getCPLdata());
					}
			#endif
			}


	FMTobject::FMTobject()
	{
		this->checkSignals();
	}

	FMTobject::~FMTobject()
	{
		//this->checkSignals();

	}

	void FMTobject::setTerminateStack()
		{
		std::set_terminate(&FMTobject::_terminate);
		}

	void FMTobject::_logStack()
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

	void FMTobject::_abort(int p_signal)
		{
		*_logger << "Aborted due to error " << "\n";
		_logStack();
		//std::_Exit(EXIT_FAILURE);
		_exhandler->raise(Exception::FMTexc::FMTunhandlederror,
			"Abort called",
			"FMTobject::_abort", __LINE__, __FILE__);
		}

	void FMTobject::setAbortStack()
		{
		if (std::signal(SIGABRT, &FMTobject::_abort) == SIG_ERR)
			{
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
				"Cannot set Abort signal",
				"FMTobject::setAbortStack", __LINE__, __FILE__);
			}
		}

	void FMTobject::_terminate()
	{
		*_logger << "Terminated due to error" << "\n";
		_logStack();
		//std::_Exit(EXIT_FAILURE);
		_exhandler->raise(Exception::FMTexc::FMTunhandlederror,
			"Terminate called",
			"FMTobject::_terminate", __LINE__, __FILE__);
		
	}


	FMTobject::FMTobject(const std::unique_ptr<Exception::FMTexceptionhandler> exhandler)
	{
		_exhandler = std::move(exhandler->Clone());
		_exhandler->passinlogger(_logger);
		this->checkSignals();

	}
	FMTobject::FMTobject(const FMTobject& rhs)
	{
		this->checkSignals();
	}
	FMTobject& FMTobject::operator = (const FMTobject& rhs)
	{
		this->checkSignals();
		return *this;
	}
	void FMTobject::passinlogger(const std::unique_ptr<Logging::FMTlogger>& logger)
		{
		try{
			this->checkSignals();
			_logger = logger->Clone();
			_exhandler->passinlogger(_logger);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTobject::passinlogger", __LINE__, __FILE__);
			}
		}

	void FMTobject::passinexceptionhandler(const std::unique_ptr<Exception::FMTexceptionhandler>& exhandler)
		{
		try{
			this->checkSignals();
			_exhandler = exhandler->Clone();
			_exhandler->passinlogger(_logger);
			setCPLhandler();
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTobject::passinexceptionhandler", __LINE__, __FILE__);
			}
		}


	void FMTobject::redirectLogToFile(const std::string& location)
		{
		_logger->redirectofile(location);
		this->checkSignals();
		}

	void FMTobject::setDefaultLogger()
		{
		try {
			this->checkSignals();
			this->passinlogger(std::unique_ptr<Logging::FMTlogger>(new Logging::FMTdefaultlogger()));
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTobject::setdefaultlogger", __LINE__, __FILE__);
			}
		}

	void FMTobject::setQuietLogger()
		{
		try{
			this->checkSignals();
			this->passinlogger(std::unique_ptr<Logging::FMTlogger>(new Logging::FMTquietlogger()));
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTobject::setquietlogger", __LINE__, __FILE__);
		}
		}

	void FMTobject::setTaskLogger()
	{
		try {
			this->checkSignals();
			this->passinlogger(std::unique_ptr<Logging::FMTlogger>(new Logging::FMTtasklogger()));

		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTobject::settasklogger", __LINE__, __FILE__);
		}
	}

	void FMTobject::setDebugLogger()
		{
		try {
			this->checkSignals();
			this->passinlogger(std::unique_ptr<Logging::FMTlogger>(new Logging::FMTdebuglogger()));
		}	
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTobject::setdebuglogger", __LINE__, __FILE__);
		}
		}

	void FMTobject::setDefaultExceptionHandler()
		{
		try{
			this->checkSignals();
			this->passinexceptionhandler(std::unique_ptr<Exception::FMTexceptionhandler>(new Exception::FMTdefaultexceptionhandler()));
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTobject::setdefaultexceptionhandler", __LINE__, __FILE__);
		}
		}
	void FMTobject::setQuietExceptionHandler()
	{
		try{
			this->checkSignals();
			this->passinexceptionhandler(std::unique_ptr<Exception::FMTexceptionhandler>(new Exception::FMTquietexceptionhandler()));
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTobject::setquietexceptionhandler", __LINE__, __FILE__);
			}
	}
	void FMTobject::setDebugExceptionHandler()
	{
		try{
		this->checkSignals();
		this->passinexceptionhandler(std::unique_ptr<Exception::FMTexceptionhandler>(new Exception::FMTdebugexceptionhandler()));
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTobject::setdebugexceptionhandler", __LINE__, __FILE__);
		}
	}

	void  FMTobject::setFreeExceptionHandler()
	{
		try{
			this->checkSignals();
			this->passinexceptionhandler(std::unique_ptr<Exception::FMTexceptionhandler>(new Exception::FMTfreeexceptionhandler()));
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTobject::setfreeexceptionhandler", __LINE__, __FILE__);
		}
	}

	void FMTobject::disableNestedExceptions()
		{
		try {
			this->checkSignals();
			_exhandler->disableNestedExceptions();
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTobject::disablenestedexceptions", __LINE__, __FILE__);
			}
		}

	void FMTobject::enableNestedExceptions()
		{
		try{
			this->checkSignals();
			_exhandler->enableNestedExceptions();
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTobject::enablenestedexceptions", __LINE__, __FILE__);
			}
		}

	void FMTobject::seterrorstowarnings(const std::vector<Exception::FMTexc>& errors)
	{
		try {
			_exhandler->seterrorstowarnings(errors);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTobject::seterrorstowarnings", __LINE__, __FILE__);
		}
	}

	void FMTobject::setmaxwarningsbeforesilenced(const size_t& maxwarningcount)
	{
		try {
			_exhandler->setmaxwarningsbeforesilenced(maxwarningcount);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTobject::setmaxwarningsbeforesilenced", __LINE__, __FILE__);
		}
	}

	std::chrono::time_point<std::chrono::high_resolution_clock> FMTobject::getClock()
		{
		std::chrono::time_point<std::chrono::high_resolution_clock> newclock;
		try {
			newclock = std::chrono::high_resolution_clock::now();
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTobject::getclock", __LINE__, __FILE__);
			}
		return newclock;
		}


	template<class chrono>
	double FMTobject::getDuration(const std::chrono::time_point<std::chrono::high_resolution_clock>& startclock)
	{
		double result = 0;
		try {
			const std::chrono::time_point<std::chrono::high_resolution_clock> stopclock = getClock();
			const std::chrono::duration<double, chrono>spent(stopclock - startclock);
			result = spent.count();
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTobject::getduration", __LINE__, __FILE__);
		}
		return result;
	}

	template double FMTobject::getDuration<std::chrono::milliseconds::period>(const std::chrono::time_point<std::chrono::high_resolution_clock>& startclock);
	template double FMTobject::getDuration<std::chrono::seconds::period>(const std::chrono::time_point<std::chrono::high_resolution_clock>& startclock);
	template double FMTobject::getDuration<std::chrono::minutes::period>(const std::chrono::time_point<std::chrono::high_resolution_clock>& startclock);
	template double FMTobject::getDuration<std::chrono::hours::period>(const std::chrono::time_point<std::chrono::high_resolution_clock>& startclock);

	std::string FMTobject::getDurationInSeconds(const std::chrono::time_point<std::chrono::high_resolution_clock>& startclock)
	{
		std::string value;
		try {
			const double dblvalue = getDuration<std::chrono::seconds::period>(startclock);
			value = "in "+std::to_string(dblvalue)+" seconds";
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTobject::getdurationinseconds", __LINE__, __FILE__);
			}
		return value;
	}


}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTobject)
