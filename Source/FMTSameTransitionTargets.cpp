#include "FMTSameTransitionTargets.h"

namespace Exception
	{
	FMTSameTransitionTargets::FMTSameTransitionTargets(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTsame_transitiontargets,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Plusieurs transitions produisent des cibles équivalentes.",
				"Multiple transitions produce equivalent targets.")
		{
		}
	}