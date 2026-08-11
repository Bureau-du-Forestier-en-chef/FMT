#include "FMTInvalidTheme.h"

namespace Exception
	{
	FMTInvalidTheme::FMTInvalidTheme(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTinvalid_theme,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Le thème référencé est invalide, absent ou incompatible avec la structure attendue du modèle.",
				"The referenced theme is invalid, missing or incompatible with the expected model structure.")
		{
		}
	}