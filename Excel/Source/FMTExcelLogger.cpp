#pragma once
#include "stdafx.h"

#include <string>
#include "FMTExcelLogger.h"

namespace Logging
{

	void FMTExcelLogger::cout(const char* message) const
		{
		printout += std::string(message);
		}
	FMTExcelLogger::FMTExcelLogger():
		Logging::FMTLogger()
	{
	

	}
	std::string FMTExcelLogger::getPrintOut() const
		{
		return printout;
		}

	void FMTExcelLogger::clearOut()
		{
		printout.clear();
		}

#ifdef FMTWITHOSI
	FMTLogger* FMTExcelLogger::clone() const
	{
		return new FMTExcelLogger(*this);
	}
#endif
	std::unique_ptr <FMTLogger> FMTExcelLogger::Clone() const
	{
		return std::unique_ptr <FMTLogger>(new FMTExcelLogger(*this));
	}


}