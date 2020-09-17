/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTobject.h"
#include "FMTdefaultlogger.h"
#include "FMTdebuglogger.h"
#include "FMTquietlogger.h"
#include "FMTquietexceptionhandler.h"
#include "FMTdefaultexceptionhandler.h"
#include "FMTdebugexceptionhandler.h"
#include "FMTfreeexceptionhandler.h"
#include <boost/filesystem.hpp>

#if defined __MINGW64__||__CYGWIN__
	#include "windows.h"
	EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#else
    #include <boost/dll/runtime_symbol_info.hpp>
#endif

#include "FMTtheme.h"

#if defined FMTWITHR
	#include "Rcpp.h"
#endif


namespace Core
{

	std::string  FMTobject::getruntimelocation() const
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
			_exhandler->raisefromcatch("", "FMTobject::getruntimelocation", __LINE__, __FILE__,_section);
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
				CPLPopErrorHandler();
				CPLPushErrorHandlerEx(Exception::FMTCPLErrorHandler, _exhandler->getCPLdata());
			#endif
			}


	FMTobject::FMTobject() : _exhandler(std::make_shared<Exception::FMTdefaultexceptionhandler>()),
		_logger(std::make_shared<Logging::FMTdefaultlogger>()), _section(FMTsection::Empty)
	{
		this->checksignals();
			setCPLhandler();
	}

	FMTobject::~FMTobject()
	{
		this->checksignals();

	}

	FMTobject::FMTobject(const std::shared_ptr<Exception::FMTexceptionhandler> exhandler) :
		_exhandler(exhandler), _logger(std::make_shared<Logging::FMTdefaultlogger>()), _section(FMTsection::Empty)
	{
		_exhandler->passinlogger(_logger);
		setCPLhandler();
		this->checksignals();

	}
	FMTobject::FMTobject(const FMTobject& rhs) :
		_exhandler(rhs._exhandler), _logger(rhs._logger), _section(rhs._section)
	{
		_exhandler->passinlogger(_logger);
		setCPLhandler();
		this->checksignals();
	}
	FMTobject& FMTobject::operator = (const FMTobject& rhs)
	{
		this->checksignals();
		if (this != &rhs)
		{
			_exhandler = rhs._exhandler;
			_logger = rhs._logger;
			_exhandler->passinlogger(_logger);
			setCPLhandler();
			_section = rhs._section;
		}
		return *this;
	}
	void FMTobject::passinlogger(const std::shared_ptr<Logging::FMTlogger>& logger)
		{
		this->checksignals();
		_logger = logger;
		_exhandler->passinlogger(_logger);
		setCPLhandler();
		}

	void FMTobject::passinexceptionhandler(const std::shared_ptr<Exception::FMTexceptionhandler>& exhandler)
		{
		this->checksignals();
		_exhandler = exhandler;
		_exhandler->passinlogger(_logger);
		setCPLhandler();
		}

	void FMTobject::passinobject(const FMTobject& rhs)
		{
		_exhandler = rhs._exhandler;
		_logger = rhs._logger;
		}

	void FMTobject::setsection(const FMTsection& section) const
		{
		_section = section;
		this->checksignals();
		}

	void FMTobject::setdefaultlogger()
		{
		this->checksignals();
		_logger = std::make_shared<Logging::FMTdefaultlogger>();
		_exhandler->passinlogger(_logger);
		setCPLhandler();
		}

	void FMTobject::setquietlogger()
		{
		this->checksignals();
		_logger = std::make_shared<Logging::FMTquietlogger>();
		_exhandler->passinlogger(_logger);
		setCPLhandler();
		}

	void FMTobject::setdebuglogger()
		{
		this->checksignals();
		_logger = std::make_shared<Logging::FMTdebuglogger>();
		_exhandler->passinlogger(_logger);
		setCPLhandler();
		}

	void FMTobject::setdefaultexceptionhandler()
		{
		this->checksignals();
		_exhandler = std::make_shared<Exception::FMTdefaultexceptionhandler>();
		_exhandler->passinlogger(_logger);
		setCPLhandler();
		}
	void FMTobject::setquietexceptionhandler()
	{
		this->checksignals();
		_exhandler = std::make_shared<Exception::FMTquietexceptionhandler>();
		_exhandler->passinlogger(_logger);
		setCPLhandler();
	}
	void FMTobject::setdebugexceptionhandler()
	{
		this->checksignals();
		_exhandler = std::make_shared<Exception::FMTdebugexceptionhandler>();
		_exhandler->passinlogger(_logger);
		setCPLhandler();
	}

	void  FMTobject::setfreeexceptionhandler()
	{
		this->checksignals();
		_exhandler = std::make_shared<Exception::FMTfreeexceptionhandler>();
		_exhandler->passinlogger(_logger);
		setCPLhandler();
	}

	bool FMTobject::checkmask(const std::vector<Core::FMTtheme>& themes,
		const std::vector<std::string>& values, std::string& mask, const std::string& otherinformation) const
	{
		//otherinformation = " at line " + std::to_string(_line);
		bool returnvalue = true;
		if (themes.size() < values.size())
		{
			_exhandler->raise(Exception::FMTexc::FMTinvalid_maskrange, mask + otherinformation,"FMTobject::checkmask", __LINE__, __FILE__, _section);
			returnvalue = false;
		}
		else {
			size_t id = 0;
			mask.clear();
			for (const Core::FMTtheme& theme : themes)
			{
				if (id < values.size() && !theme.isvalid(values[id]))
				{
					const std::string message = values[id] + " at theme " + std::to_string(theme.getid() + 1) + otherinformation;
					_exhandler->raise(Exception::FMTexc::FMTundefined_attribute,message,
						"FMTobject::checkmask",__LINE__, __FILE__, _section);
					returnvalue = false;
				}
				std::string value = "?";
				if (id < values.size())
					{
					value = values[id];
					}
				mask += value + " ";
				++id;
			}
			mask.pop_back();
		}
		return  returnvalue;
	}

	bool FMTobject::validate(const std::vector<Core::FMTtheme>& themes, std::string& mask,std::string otherinformation) const
		{
		std::vector<std::string>values;
		boost::split(values, mask, boost::is_any_of(" \t"), boost::token_compress_on);
		return checkmask(themes, values, mask, otherinformation);
		}


	void FMTobject::disablenestedexceptions()
		{
		this->checksignals();
		this->_exhandler->disablenestedexceptions();
		setCPLhandler();
		}

	void FMTobject::enablenestedexceptions()
		{
		this->checksignals();
		this->_exhandler->enablenestedexceptions();
		setCPLhandler();
		}


}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTobject)
