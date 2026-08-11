#include "FMTCommaReplacement.h"

namespace Exception
	{
	FMTCommaReplacement::FMTCommaReplacement(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTWarning(
				FMTexc::FMTcomma_replacement,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une virgule a été automatiquement remplacée afin de corriger le format des données.",
				"A comma was automatically replaced to correct the input data format.")
		{
		}
	}