#include "FMTConstantsReplacement.h"

namespace Exception
	{
	FMTConstantsReplacement::FMTConstantsReplacement(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTWarning(
				FMTexc::FMTconstants_replacement,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une constante du modèle a été automatiquement remplacée pendant l'interprétation des données.",
				"A model constant was automatically replaced while interpreting input data.")
		{
		}
	}