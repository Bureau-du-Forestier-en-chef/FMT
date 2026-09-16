#ifndef NOMINMAX
	#define NOMINMAX
#endif // !NOMINMAX
#include "WarningExceptionHandler.h"
#include "FMTError.h"
#include "FMTWarning.h"
#include "CallbackLogger.h"
#include "FMTExceptionHandler.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>
#include <regex>

#include "FMTConstraint.h"
#include "windows.h"
#include "FMTObject.h"

FMTWrapperCore::WarningExceptionHandler::WarningExceptionHandler(
	const size_t& maxnumberofwarnings)
	: FMTExceptionHandler()
{
	FMTExceptionHandler::setMaxWarningsBeforeSilenced(maxnumberofwarnings);
	ResetThread();
}

void FMTWrapperCore::WarningExceptionHandler::ResetThread()
{
	m_crashedthreadid = m_mainthreadid;
}

void FMTWrapperCore::WarningExceptionHandler::tryfileopener(
	const std::string& fullerrorstr) const
{
	const std::regex linecaper(
		"(In)([\\s\\t]*)(.+)([\\s\\t]*)(at)([\\s\\t]*)(line)([\\s\\t]*)([\\d]*)([\\s\\t]*)(FMTsection)");

	std::smatch kmatch;

	if (std::regex_search(fullerrorstr, kmatch, linecaper))
	{
		std::string file(kmatch[3]);
		boost::trim(file);

		std::string line(kmatch[9]);
		boost::trim(line);

		const std::string udlloc =
			Core::FMTObject::getRuntimeLocation() +
			"\\userDefineLang.xml";

		const std::string udlnewloc =
			std::string(getenv("APPDATA")) +
			"\\Notepad++\\userDefineLang.xml";

		DWORD dwAttrib = GetFileAttributes(udlnewloc.c_str());

		if (dwAttrib == INVALID_FILE_ATTRIBUTES)
		{
			CopyFile(
				udlloc.c_str(),
				udlnewloc.c_str(),
				FALSE);
		}

		const std::string call =
			"C:\\PROGRA~1\\Notepad++\\notepad++.exe " +
			file +
			" -udl=FMT -n" +
			line;

		WinExec(call.c_str(), SW_HIDE);
	}
}

std::string FMTWrapperCore::WarningExceptionHandler::geterrorstack(
	std::string text,
	const std::string& method,
	const int& line,
	const std::string& fil)
{
	std::string finalstack;

	FMTWrapperCore::CallbackLogger* ModifLogger =
		dynamic_cast<FMTWrapperCore::CallbackLogger*>(_logger);

	if (!ModifLogger)
	{
		return text +
			" In Method(" + method +
			") At Line(" + std::to_string(line) + ")";
	}

	ModifLogger->dokeepprint();

	try
	{
		Exception::FMTExceptionHandler::printExceptions(
			text,
			method,
			line,
			fil,
			Core::FMTsection::Empty);
	}
	catch (...)
	{
	}

	finalstack = ModifLogger->getlastprint();

	ModifLogger->resetkeepprint();

	return finalstack;
}

void FMTWrapperCore::WarningExceptionHandler::printExceptions(
	std::string text,
	const std::string& method,
	const int& line,
	const std::string& fil,
	Core::FMTsection lsection)
{
	raiseFromCatch(text, method, line, fil, lsection);
}

std::unique_ptr<Exception::FMTExceptionHandler>
FMTWrapperCore::WarningExceptionHandler::clone() const
{
	return std::unique_ptr<Exception::FMTExceptionHandler>(
		new WarningExceptionHandler(*this));
}

Exception::FMTException
FMTWrapperCore::WarningExceptionHandler::raise(
	Exception::FMTexc lexception,
	std::string text,
	const std::string& method,
	const int& line,
	const std::string& file,
	Core::FMTsection lsection,
	bool throwit)
{
	std::unique_ptr<Exception::FMTException> newException =
		_createException(
			lexception,
			lsection,
			text,
			method,
			file,
			line);

	_updateStatus(newException);

	if (newException->getLevel() != Exception::FMTlev::FMT_Warning)
	{
		if (throwit &&
			(newException->isFatal()) &&
			!_needToRethrow())
		{
			boost::lock_guard<boost::recursive_mutex> guard(m_mtx);
			std::throw_with_nested(*newException);
		}
	}
	else if (throwit)
	{
		_updateWarningCount(*newException);
	}

	return *newException;
}