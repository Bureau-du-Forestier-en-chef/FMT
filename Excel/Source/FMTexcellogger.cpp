#include "stdafx.h"
#include <string>
#include "FMTexcellogger.h"

namespace Logging
{

	void FMTexcellogger::cout(const char* message) const
		{
		printout += std::string(message);
		}
	FMTexcellogger::FMTexcellogger():
		Logging::FMTlogger()
	{
	

	}
	std::string FMTexcellogger::getprintout() const
		{
		return printout;
		}

	void FMTexcellogger::clearout()
		{
		printout.clear();
		}


}