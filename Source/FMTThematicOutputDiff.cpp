#include "FMTThematicOutputDiff.h"

namespace Exception
	{
	FMTThematicOutputDiff::FMTThematicOutputDiff(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTWarning(
				FMTexc::FMTthematic_output_diff,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une différence a été détectée dans les sorties thématiques.",
				"A difference was detected in thematic outputs.")
		{
		}
	}