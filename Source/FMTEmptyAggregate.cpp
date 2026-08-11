#include "FMTEmptyAggregate.h"

namespace Exception
	{
	FMTEmptyAggregate::FMTEmptyAggregate(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTWarning(
				FMTexc::FMTempty_aggregate,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Un agrégat a été déclaré sans contenir de valeurs utilisables.",
				"An aggregate was declared without containing usable values.")
		{
		}
	}