
#include <string>
#include "FMTExcelLogger.h"

namespace Logging
{

	void FMTExcelLogger::cout(const char* message) const
		{
		printout += std::string(message);
		}
	FMTExcelLogger::FMTExcelLogger():
		Logging::FMTlogger()
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
	FMTlogger* FMTExcelLogger::clone() const
	{
		return new FMTExcelLogger(*this);
	}
#endif
	std::unique_ptr <FMTlogger> FMTExcelLogger::Clone() const
	{
		return std::unique_ptr <FMTlogger>(new FMTExcelLogger(*this));
	}


}