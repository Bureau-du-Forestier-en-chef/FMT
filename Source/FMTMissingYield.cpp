#include "FMTMissingYield.h"

namespace Exception
	{
	FMTMissingYield::FMTMissingYield(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTmissingyield,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Aucun rendement compatible n'a été trouvé pour les attributs demandés.",
				"No compatible yield was found for the requested attributes.")
		{
		}
	}