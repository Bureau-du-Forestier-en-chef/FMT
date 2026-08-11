#include "FMTMissingDevelopment.h"

namespace Exception
	{
	FMTMissingDevelopment::FMTMissingDevelopment(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTmissingdevelopment,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Un développement forestier requis est absent du modèle.",
				"A required forest development is missing from the model.")
		{
		}
	}