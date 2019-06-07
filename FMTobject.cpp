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

	FMTobject::FMTobject() : _exhandler(make_shared<FMTdefaultexceptionhandler>())
	{
		this->checksignals();
		//_exhandler = make_shared<FMTdefaultexceptionhandler>();
	}

	FMTobject::~FMTobject()
	{
		this->checksignals();

	}

	FMTobject::FMTobject(const shared_ptr<FMTexceptionhandler> exhandler) : _exhandler(move(exhandler))
	{
		this->checksignals();

	}
	FMTobject::FMTobject(const FMTobject& rhs) : _exhandler(move(rhs._exhandler))
	{
		this->checksignals();
	}
	FMTobject& FMTobject::operator = (const FMTobject& rhs)
	{
		this->checksignals();
		if (this != &rhs)
		{
			_exhandler = rhs._exhandler;
		}
		return *this;
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
