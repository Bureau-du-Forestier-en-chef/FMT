#include "FMTUnboundedPeriod.h"

namespace Exception
	{
	FMTUnboundedPeriod::FMTUnboundedPeriod(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTunboundedperiod,
				FMTlev::FMT_range,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une période n'est pas correctement bornée.",
				"A period is not properly bounded.")
		{
		}
	}