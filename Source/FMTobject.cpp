/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTobject.hpp"
#include "FMTdefaultlogger.hpp"
#include "FMTdebuglogger.hpp"
#include "FMTquietlogger.hpp"
#include "FMTquietexceptionhandler.hpp"
#include "FMTdefaultexceptionhandler.hpp"
#include "FMTdebugexceptionhandler.hpp"
#include "FMTfreeexceptionhandler.hpp"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#if defined __MINGW64__||__CYGWIN__
	#include "windows.h"
	EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#else
    #include <boost/dll/runtime_symbol_info.hpp>
#endif

#if defined FMTWITHR
	#include "Rcpp.h"
#endif

#if defined _WIN32
#include <windows.h>
#endif

#if defined __unix
#include <sys/sysinfo.h>
#endif


namespace Core
{

	std::shared_ptr<Logging::FMTlogger> FMTobject::_logger = std::shared_ptr<Logging::FMTlogger>(new Logging::FMTdefaultlogger);
	std::shared_ptr<Exception::FMTexceptionhandler> FMTobject::_exhandler = std::shared_ptr<Exception::FMTexceptionhandler>(new Exception::FMTdefaultexceptionhandler(_logger));

	unsigned long long FMTobject::getavailablememory() const
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
				_exhandler->raisefromcatch("", "FMTobject::getratioofavailablememory", __LINE__, __FILE__);
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
			boost::filesystem::path boost_path = boost::dll::symbol_location(var);
	#endif
			strDLLpath = boost_path.parent_path().string();
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTobject::getruntimelocation", __LINE__, __FILE__);
			}
		return strDLLpath;
	}

	void FMTobject::checksignals() const
	{
	#if defined FMTWITHPYTHON
		if (PyErr_CheckSignals() == -1)
		{
			exit(1);
		}
	#endif

	#if defined FMTWITHR
		Rcpp::checkUserInterrupt();
	#endif

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
		this->checksignals();
	}

	FMTobject::~FMTobject()
	{
		this->checksignals();

	}

	FMTobject::FMTobject(const std::shared_ptr<Exception::FMTexceptionhandler> exhandler)
	{
		_exhandler = exhandler;
		_exhandler->passinlogger(_logger);
		this->checksignals();

	}
	FMTobject::FMTobject(const FMTobject& rhs)
	{
		this->checksignals();
	}
	FMTobject& FMTobject::operator = (const FMTobject& rhs)
	{
		this->checksignals();
		return *this;
	}
	void FMTobject::passinlogger(const std::shared_ptr<Logging::FMTlogger>& logger)
		{
		try{
			this->checksignals();
			_logger = logger;
			_exhandler->passinlogger(_logger);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTobject::passinlogger", __LINE__, __FILE__);
			}
		}

	void FMTobject::passinexceptionhandler(const std::shared_ptr<Exception::FMTexceptionhandler>& exhandler)
		{
		try{
			this->checksignals();
			_exhandler = exhandler;
			_exhandler->passinlogger(_logger);
			setCPLhandler();
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTobject::passinexceptionhandler", __LINE__, __FILE__);
			}
		}


	void FMTobject::redirectlogtofile(const std::string& location)
		{
		_logger->redirectofile(location);
		this->checksignals();
		}

	void FMTobject::setdefaultlogger()
		{
		try {
			this->checksignals();
			this->passinlogger(std::make_shared<Logging::FMTdefaultlogger>());
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTobject::setdefaultlogger", __LINE__, __FILE__);
			}
		}

	void FMTobject::setquietlogger()
		{
		try{
			this->checksignals();
			this->passinlogger(std::make_shared<Logging::FMTquietlogger>());
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTobject::setquietlogger", __LINE__, __FILE__);
		}
		}

	void FMTobject::setdebuglogger()
		{
		try {
			this->checksignals();
			this->passinlogger(std::make_shared<Logging::FMTdebuglogger>());
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTobject::setdebuglogger", __LINE__, __FILE__);
		}
		}

	void FMTobject::setdefaultexceptionhandler()
		{
		try{
			this->checksignals();
			this->passinexceptionhandler(std::make_shared<Exception::FMTdefaultexceptionhandler>());
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTobject::setdefaultexceptionhandler", __LINE__, __FILE__);
		}
		}
	void FMTobject::setquietexceptionhandler()
	{
		try{
			this->checksignals();
			this->passinexceptionhandler(std::make_shared<Exception::FMTquietexceptionhandler>());
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTobject::setquietexceptionhandler", __LINE__, __FILE__);
			}
	}
	void FMTobject::setdebugexceptionhandler()
	{
		try{
		this->checksignals();
		this->passinexceptionhandler(std::make_shared<Exception::FMTdebugexceptionhandler>());
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTobject::setdebugexceptionhandler", __LINE__, __FILE__);
		}
	}

	void  FMTobject::setfreeexceptionhandler()
	{
		try{
			this->checksignals();
			this->passinexceptionhandler(std::make_shared<Exception::FMTfreeexceptionhandler>());
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTobject::setfreeexceptionhandler", __LINE__, __FILE__);
		}
	}

	void FMTobject::disablenestedexceptions()
		{
		try {
			this->checksignals();
			_exhandler->disablenestedexceptions();
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTobject::disablenestedexceptions", __LINE__, __FILE__);
			}
		}

	void FMTobject::enablenestedexceptions()
		{
		try{
			this->checksignals();
			_exhandler->enablenestedexceptions();
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

}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTobject)
