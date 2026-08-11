#include "FMTInvalidAction.h"

namespace Exception
	{
	FMTInvalidAction::FMTInvalidAction(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTinvalid_action,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"L'action référencée est invalide ou mal définie.",
				"The referenced action is invalid or improperly defined.")
		{
		}
	}