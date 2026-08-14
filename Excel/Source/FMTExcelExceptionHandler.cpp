#pragma once
#include "stdafx.h"

#include "FMTExcelExceptionHandler.h"
#include "FMTWarning.h"
#include "FMTError.h"
#include <boost/thread.hpp>

namespace Exception
{

	FMTExcelExceptionHandler::FMTExcelExceptionHandler():FMTExceptionHandler(), m_build_exceptions()
		{
		std::vector<Exception::FMTexc>errors;
		errors.push_back(Exception::FMTexc::FMTmissingyield);
		errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
		errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
		errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
		errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
		errors.push_back(Exception::FMTexc::FMToveridedyield);
		errors.push_back(Exception::FMTexc::FMTinvalid_geometry);
		errors.push_back(Exception::FMTexc::FMTsourcetotarget_transition);
		errors.push_back(Exception::FMTexc::FMTsame_transitiontargets);
		errors.push_back(Exception::FMTexc::FMTdeathwithlock);
		setErrorsToWarnings(errors);
		setMaxWarningsBeforeSilenced(1000);
		}

	std::unordered_map<int, std::vector<std::string>> FMTExcelExceptionHandler::getBuildExceptions() const
		{
		return m_build_exceptions;
		}
	void FMTExcelExceptionHandler::resetBuildExceptions()
		{
		m_build_exceptions.clear();
		}
	FMTException FMTExcelExceptionHandler::raise(FMTexc lexception, std::string text,
		const std::string& method, const int& line, const std::string& file,
		Core::FMTsection lsection, bool throwit)
		{
		std::unique_ptr<FMTException> newException = _createException(lexception, lsection,
			text, method, file, line);
		_updateStatus(newException);
		boost::lock_guard<boost::recursive_mutex> guard(m_mtx);
		if (m_build_exceptions.find(static_cast<int>(lexception))== m_build_exceptions.end())
			{
			m_build_exceptions[static_cast<int>(lexception)] = std::vector<std::string>();
			}
		m_build_exceptions[static_cast<int>(lexception)].push_back(text);
		if (newException->getLevel() != FMTlev::FMT_Warning)
		{
			if (throwit && (newException->isFatal()) && !_needToRethrow())
			{
				std::throw_with_nested(*newException);
			}
		}
		else if (throwit)
		{
			_updateWarningCount(*newException);
		}

		return *newException;
		}

	std::unique_ptr <FMTExceptionHandler> FMTExcelExceptionHandler::clone() const
	{
		return std::unique_ptr<FMTExceptionHandler>(new FMTExcelExceptionHandler(*this));
	}

}