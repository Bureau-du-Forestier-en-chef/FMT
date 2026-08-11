#include "FMTLeakingTransition.h"

namespace Exception
	{
	FMTLeakingTransition::FMTLeakingTransition(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTleakingtransition,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une transition provoque une perte inexpliquée de superficie dans le modèle.",
				"A transition causes an unexplained loss of area in the model.")
		{
		}
	}