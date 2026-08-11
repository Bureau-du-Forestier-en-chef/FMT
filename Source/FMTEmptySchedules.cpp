#include "FMTEmptySchedules.h"

namespace Exception
	{
	FMTEmptySchedules::FMTEmptySchedules(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTempty_schedules,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Aucun calendrier n'a été généré.",
				"No schedules were generated.")
		{
		}
	}