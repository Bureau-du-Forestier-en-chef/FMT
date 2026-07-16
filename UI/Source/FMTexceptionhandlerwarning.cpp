#include "stdafx.h"
#include "FMTexceptionhandlerwarning.h"
#include "FMTerror.h"
#include "FMTwarning.h"
#include "FMTFormLogger.h"
#include "FMTexceptionhandler.h"
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>
#include <regex>
#include "FMTconstraint.h"
#include "windows.h"
#include "FMTobject.h"


Wrapper::FMTexceptionhandlerwarning::FMTexceptionhandlerwarning(const size_t& maxnumberofwarnings) : FMTExceptionHandler()
{
	FMTExceptionHandler::setMaxWarningsBeforeSilenced(maxnumberofwarnings);
	ResetThread();
};

void Wrapper::FMTexceptionhandlerwarning::ResetThread()
{
	crashedthreadid = mainthreadid;
}

void Wrapper::FMTexceptionhandlerwarning::tryfileopener(const std::string& fullerrorstr) const
	{
	const std::regex linecaper("(In)([\\s\\t]*)(.+)([\\s\\t]*)(at)([\\s\\t]*)(line)([\\s\\t]*)([\\d]*)([\\s\\t]*)(FMTsection)");
	std::smatch kmatch;
	if (std::regex_search(fullerrorstr, kmatch, linecaper))
		{
		std::string file(kmatch[3]);
		boost::trim(file);
		std::string line(kmatch[9]);
		boost::trim(line);
		//First copy the userdefined language file
		const std::string udlloc = Core::FMTobject::getRuntimeLocation() + "\\userDefineLang.xml";
		const std::string udlnewloc(std::string(getenv("APPDATA"))+"\\Notepad++\\userDefineLang.xml");
		DWORD dwAttrib = GetFileAttributes(udlnewloc.c_str());
		if (dwAttrib == INVALID_FILE_ATTRIBUTES)
			{
			CopyFile(udlloc.c_str(), udlnewloc.c_str(), FALSE);
			}
		const std::string call = "C:\\PROGRA~1\\Notepad++\\notepad++.exe " + file +" -udl=FMT -n" + line;
		WinExec(call.c_str(), SW_HIDE);
		}

	}


std::string Wrapper::FMTexceptionhandlerwarning::geterrorstack(
	std::string text,
	const std::string& method, 
	const int& line, 
	const std::string& fil)
{
	std::string finalstack;
	Wrapper::FMTFormLogger* ModifLogger = dynamic_cast<Wrapper::FMTFormLogger*>(_logger);
	if (!ModifLogger)
	{
		// Le logger a été remplacé (setDefaultLogger). On ne peut pas capturer via keepprint ;
		// on renvoie au moins le contexte sans déréférencer un pointeur null.
		return text + " In Method(" + method + ") At Line(" + std::to_string(line) + ")";
	}
	ModifLogger->dokeepprint();
	try {
		Exception::FMTExceptionHandler::printExceptions(text, method, line, fil, Core::FMTsection::Empty);
	}catch (...)
		{
		//Do nothing
		}
	finalstack = ModifLogger->getlastprint();
	ModifLogger->resetkeepprint();
	return finalstack;
}

void Wrapper::FMTexceptionhandlerwarning::printExceptions(std::string text,
	const std::string& method, const int& line, const std::string& fil,
	Core::FMTsection lsection)
{
	raiseFromCatch(text, method, line, fil, lsection);
}
std::unique_ptr <Exception::FMTExceptionHandler> Wrapper::FMTexceptionhandlerwarning::Clone() const
{
	return std::unique_ptr<Exception::FMTExceptionHandler>(new FMTexceptionhandlerwarning(*this));
}

Exception::FMTException Wrapper::FMTexceptionhandlerwarning::raise(Exception::FMTexc lexception, std::string text, const std::string& method, const int& line, const std::string& file, Core::FMTsection lsection, bool throwit)
{
	const Exception::FMTlev LEVEL = getLevel(lexception);
	Exception::FMTException excp = Exception::FMTException(lexception, updateStatus(lexception, text));
	if (lsection != Core::FMTsection::Empty)
	{
		excp = Exception::FMTException(lexception, lsection, updateStatus(lexception, text));
	}
	if (LEVEL != Exception::FMTlev::FMT_Warning)
	{
		if (lsection == Core::FMTsection::Empty)
		{
			excp = Exception::FMTException(lexception, updateStatus(lexception, text), method, file, line);
		}
		else {
			excp = Exception::FMTException(lexception, lsection, updateStatus(lexception, text), method, file, line);
		}
		if (throwit && (LEVEL == Exception::FMTlev::FMT_logic || LEVEL == Exception::FMTlev::FMT_range) && !needToRethrow())
		{
			boost::lock_guard<boost::recursive_mutex> guard(mtx);
			std::throw_with_nested(Exception::FMTError(excp));
		}
	}
	else if (throwit)
	{
		Exception::FMTWarning(excp).warn(*_logger, _specificwarningcount, maxwarningsbeforesilenced);
	}
	return excp;
}