#include "FMTEmptyAction.h"

namespace Exception
	{
	FMTEmptyAction::FMTEmptyAction(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTWarning(
				FMTexc::FMTempty_action,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une action a été déclarée sans opération exécutable.",
				"An action was declared without any executable operation.")
		{
		}
	}