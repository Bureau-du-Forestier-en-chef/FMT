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
#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/stacktrace.hpp>
#include <exception>
#include <chrono>
#include <csignal>

#if defined __MINGW64__||__CYGWIN__
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
#endif

#if defined FMTWITHR
	#include "Rcpp.h"
#endif

#include "FMTcplhandler.h"

#if defined _WIN32
#ifndef NOMINMAX
	#define NOMINMAX
#endif
#endif


#if defined (_MSC_VER)
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
#endif

#if defined FMTWITHPYTHON
	#include <boost/python.h>
#endif

// Common
#include <cstdint>

// Windows
#if defined(_WIN32)
	#include <windows.h>
#endif

// Linux
#if defined(__linux__)
	#include <sys/sysinfo.h>
#endif

// macOS
#if defined(__APPLE__)
	#include <mach/mach.h>
	#include <unistd.h>
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

		try
		{
		#if defined(_WIN32)

					MEMORYSTATUSEX status{};
					status.dwLength = sizeof(status);

					if (GlobalMemoryStatusEx(&status))
					{
						available =
							static_cast<unsigned long long>(
								status.ullAvailPhys);
					}

		#elif defined(__linux__)

					struct sysinfo info;

					if (sysinfo(&info) == 0)
					{
						available =
							static_cast<unsigned long long>(info.freeram) *
							static_cast<unsigned long long>(info.mem_unit);
					}

		#elif defined(__APPLE__)

					vm_statistics64_data_t vmstat;
					mach_msg_type_number_t count =
						HOST_VM_INFO64_COUNT;

					if (host_statistics64(
						mach_host_self(),
						HOST_VM_INFO64,
						reinterpret_cast<host_info64_t>(&vmstat),
						&count) == KERN_SUCCESS)
					{
						available =
							static_cast<unsigned long long>(
								vmstat.free_count) *
							static_cast<unsigned long long>(
								getpagesize());
					}

		#endif
		}
		catch (...)
		{
			_exhandler->raiseFromCatch(
				"",
				"FMTObject::getAvailableMemory",
				__LINE__,
				__FILE__);
		}

		return available;
	}


	std::string  FMTObject::getRuntimeLocation()
	{
		std::string strDLLpath;
		try {
			boost::filesystem::path THIS_LOCATION = boost::dll::this_line_location();
			strDLLpath = THIS_LOCATION.parent_path().string();
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

		void FMTObject::setCplHandler()
			{
			#if defined  FMTWITHGDAL
				if (_exhandler)
					{
					Exception::FMTExceptionHandler* handler = reinterpret_cast<Exception::FMTExceptionHandler*>(CPLGetErrorHandlerUserData());
					if (handler)
						{
						CPLPopErrorHandler();
						}
					CPLPushErrorHandlerEx(Exception::FMTCPLErrorHandler, _exhandler->getCplData());
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
		_exhandler = std::move(exhandler->clone());
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
			_exhandler = exhandler->clone();
			_exhandler->passInLogger(_logger);
			setCplHandler();
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
