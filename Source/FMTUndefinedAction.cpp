#include "FMTUndefinedAction.h"

namespace Exception
	{
	FMTUndefinedAction::FMTUndefinedAction(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTundefined_action,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une action référencée n'a jamais été définie.",
				"A referenced action has not been defined.")
		{
		}
	}