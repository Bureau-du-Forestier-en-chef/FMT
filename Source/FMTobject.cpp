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


		void FMTobject::setCPLhandler(bool onlynull)
			{
			#if defined  FMTWITHGDAL

			if (_exhandler!=nullptr&&!_exhandler->isCPLpushed())
				{
					Exception::FMTexceptionhandler* handler = reinterpret_cast<Exception::FMTexceptionhandler*>(CPLGetErrorHandlerUserData());

					if (handler == nullptr || (!onlynull && handler != _exhandler->getCPLdata()))
					{
						if (handler != nullptr)
						{
							CPLPopErrorHandler();
						}
						CPLPushErrorHandlerEx(Exception::FMTCPLErrorHandler, _exhandler->getCPLdata());
					}
					
					_exhandler->setCPLpushed();
				}
			#endif
			}


	FMTobject::FMTobject() : _exhandler(std::make_shared<Exception::FMTdefaultexceptionhandler>()),
		_logger(std::make_shared<Logging::FMTdefaultlogger>()), _section(FMTsection::Empty)
	{
		_exhandler->passinlogger(_logger);
		this->checksignals();
			setCPLhandler(true);
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
		try{
			this->checksignals();
			_logger = logger;
			_exhandler->passinlogger(_logger);
			setCPLhandler();
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

	void FMTobject::passinobject(const FMTobject& rhs)
		{
		try {
			_exhandler = rhs._exhandler;
			_logger = rhs._logger;
			_exhandler->passinlogger(_logger);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTobject::passinobject", __LINE__, __FILE__);
			}
		}

	void FMTobject::setsection(const FMTsection& section) const
		{
		_section = section;
		this->checksignals();
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
			setCPLhandler();
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
			setCPLhandler();
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
			setCPLhandler();
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
			setCPLhandler();
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
			setCPLhandler();
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
		setCPLhandler();
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
			setCPLhandler();
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTobject::setfreeexceptionhandler", __LINE__, __FILE__);
		}
	}

	bool FMTobject::checkmask(const std::vector<Core::FMTtheme>& themes,
		const std::vector<std::string>& values, std::string& mask, const std::string& otherinformation) const
	{
		//otherinformation = " at line " + std::to_string(_line);
		bool returnvalue = true;
		if (themes.size() > values.size())
		{
			//_exhandler->raise(Exception::FMTexc::FMTinvalid_maskrange, mask + otherinformation,"FMTobject::checkmask", __LINE__, __FILE__, _section);
			const std::string original(mask);
			mask.clear();
			for (const std::string& value : values)
			{
				mask += value + " ";
			}
			for (size_t id = values.size(); id < themes.size(); ++id)
			{
				mask += "? ";
			}
			mask.pop_back();
			_exhandler->raise(Exception::FMTexc::FMTignore,
				"Extended mask " + original + " to " + mask, "FMTobject::checkmask", __LINE__, __FILE__);
			returnvalue = true;
		}
		else {
			size_t id = 0;
			const std::string original(mask);
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
			if (values.size()!= themes.size())
				{
				_exhandler->raise(Exception::FMTexc::FMTignore,
					"Subset mask " + original + " to " + mask, "FMTobject::checkmask", __LINE__, __FILE__);
				}
			
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
		try {
			this->checksignals();
			this->_exhandler->disablenestedexceptions();
			this->passinexceptionhandler(this->_exhandler);
			setCPLhandler();
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTobject::disablenestedexceptions", __LINE__, __FILE__);
			}
		}

	void FMTobject::enablenestedexceptions()
		{
		try{
			this->checksignals();
			this->_exhandler->enablenestedexceptions();
			this->passinexceptionhandler(this->_exhandler);
			setCPLhandler();
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTobject::enablenestedexceptions", __LINE__, __FILE__);
			}
		}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTobject)
