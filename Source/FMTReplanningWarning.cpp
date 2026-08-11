#include "FMTReplanningWarning.h"

namespace Exception
	{
	FMTReplanningWarning::FMTReplanningWarning(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTWarning(
				FMTexc::FMTreplanningwarning,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"La replanification a été complétée mais certaines hypothèses ou contraintes ont généré des avertissements.",
				"Replanning completed but some assumptions or constraints generated warnings.")
		{
		}
	}