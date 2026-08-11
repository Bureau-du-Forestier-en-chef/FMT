#include "FMTThemeRedefinition.h"

namespace Exception
	{
	FMTThemeRedefinition::FMTThemeRedefinition(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTWarning(
				FMTexc::FMTtheme_redefinition,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Un thème déjÃ  défini a été redéfini dans le modèle.",
				"A previously defined theme was redefined in the model.")
		{
		}
	}