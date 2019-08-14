/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "FMTobject.h"


namespace Exception
{

	string  FMTobject::getruntimelocation()
	{
		string strDLLpath;
		WCHAR   DllPath[MAX_PATH] = { 0 };
		GetModuleFileNameW((HINSTANCE)&__ImageBase, DllPath, boost::size(DllPath));
		wstring wstrpath(DllPath);
		string strpath(wstrpath.begin(), wstrpath.end());
#if defined (_MSC_VER)
		boost::filesystem::path boost_path(strpath);
#elif defined (__CYGWIN__)
        string clean_path;
        if (strpath.find(":")!=string::npos)
            {
             clean_path = strpath.substr(strpath.find(":")-1);
             if (strpath.find("\\")!=string::npos)
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
		return strDLLpath;
	}

	void FMTobject::checksignals()
	{
		if (PyErr_CheckSignals() == -1)
		{
			exit(1);
		}
	}

	FMTobject::FMTobject() : _exhandler(make_shared<FMTdefaultexceptionhandler>()), 
		_logger(make_shared<Logging::FMTlogger>(Logging::FMTlogger(Logging::FMTlogtype::FMT_Warn)))
	{
		this->checksignals();
		//_exhandler = make_shared<FMTdefaultexceptionhandler>();
	}

	FMTobject::~FMTobject()
	{
		this->checksignals();

	}

	FMTobject::FMTobject(const shared_ptr<FMTexceptionhandler> exhandler) : _exhandler(move(exhandler)), _logger()
	{
		this->checksignals();

	}
	FMTobject::FMTobject(const FMTobject& rhs) : _exhandler(move(rhs._exhandler)), _logger(move(rhs._logger))
	{
		this->checksignals();
	}
	FMTobject& FMTobject::operator = (const FMTobject& rhs)
	{
		this->checksignals();
		if (this != &rhs)
		{
			_exhandler = rhs._exhandler;
			_logger = rhs._logger;
		}
		return *this;
	}
	void FMTobject::passinlogger(const std::shared_ptr<Logging::FMTlogger>& logger)
		{
		this->checksignals();
		_logger = logger;
		}

	void FMTobject::passinexceptionhandler(const shared_ptr<FMTexceptionhandler>& exhandler)
		{
		this->checksignals();
		_exhandler = exhandler;
		}
	void FMTobject::setdefaultexceptionhandler()
	{
		this->checksignals();
		_exhandler = make_shared<FMTdefaultexceptionhandler>();
	}
	void FMTobject::setquietexceptionhandler()
	{
		this->checksignals();
		_exhandler = make_shared<FMTquietexceptionhandler>();
	}
	void FMTobject::setdebugexceptionhandler()
	{
		this->checksignals();
		_exhandler = make_shared<FMTdebugexceptionhandler>();
	}

	void  FMTobject::setfreeexceptionhandler()
	{
		this->checksignals();
		_exhandler = make_shared<FMTfreeexceptionhandler>();
	}

}
