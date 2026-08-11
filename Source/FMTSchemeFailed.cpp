#include "FMTSchemeFailed.h"

namespace Exception
	{
	FMTSchemeFailed::FMTSchemeFailed(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTWarning(
				FMTexc::FMTschemefailed,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une stratégie ou un schéma de résolution n'a pas pu ètre complété correctement.",
				"A solution scheme or strategy could not be completed successfully.")
		{
		}
	}