#include "FMTPreexistingYield.h"

namespace Exception
	{
	FMTPreexistingYield::FMTPreexistingYield(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTWarning(
				FMTexc::FMTpreexisting_yield,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Un rendement existe déjÃ  pour la combinaison d'attributs spécifiée.",
				"A yield already exists for the specified attribute combination.")
		{
		}
	}