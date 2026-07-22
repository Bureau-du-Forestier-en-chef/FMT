#pragma once
#include "stdafx.h"

#include "FMTExcelExceptionHandler.h"
#include "FMTWarning.h"
#include "FMTError.h"
#include <boost/thread.hpp>

namespace Exception
{

	FMTExcelExceptionHandler::FMTExcelExceptionHandler():FMTExceptionHandler(), build_exceptions()
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
		return build_exceptions;
		}
	void FMTExcelExceptionHandler::resetBuildExceptions()
		{
		build_exceptions.clear();
		}
	FMTException FMTExcelExceptionHandler::raise(FMTexc lexception, std::string text,
		const std::string& method, const int& line, const std::string& file,
		Core::FMTsection lsection, bool throwit)
		{
		const FMTlev LEVEL = getLevel(lexception);
		boost::lock_guard<boost::recursive_mutex> guard(mtx);
		if (build_exceptions.find(static_cast<int>(lexception))== build_exceptions.end())
			{
			build_exceptions[static_cast<int>(lexception)] = std::vector<std::string>();
			}
		build_exceptions[static_cast<int>(lexception)].push_back(text);
		FMTException excp = FMTException(lexception, updateStatus(lexception, text));
		if (lsection != Core::FMTsection::Empty)
		{
			excp = FMTException(lexception, lsection, updateStatus(lexception, text));
		}
		if (LEVEL != FMTlev::FMT_Warning)
		{
			if (lsection == Core::FMTsection::Empty)
			{
				excp = FMTException(lexception, updateStatus(lexception, text), method, file, line);
			}
			else {
				excp = FMTException(lexception, lsection, updateStatus(lexception, text), method, file, line);
			}
			if (throwit && (LEVEL == FMTlev::FMT_logic || LEVEL == FMTlev::FMT_range) && !needToRethrow())
			{
				std::throw_with_nested(FMTError(excp));
			}
		}
		else if (throwit)
		{
			FMTWarning(excp).warn(*_logger, _specificwarningcount, maxwarningsbeforesilenced);
		}

		return excp;
		}

	std::unique_ptr <FMTExceptionHandler> FMTExcelExceptionHandler::Clone() const
	{
		return std::unique_ptr<FMTExceptionHandler>(new FMTExcelExceptionHandler(*this));
	}

}