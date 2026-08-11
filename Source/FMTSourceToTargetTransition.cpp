#include "FMTSourceToTargetTransition.h"

namespace Exception
	{
	FMTSourceToTargetTransition::FMTSourceToTargetTransition(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTsourcetotarget_transition,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une transition peut retourner vers un état source ou cible invalide.",
				"A transition can return to an invalid source or target state.")
		{
		}
	}