#include "FMTMissingField.h"

namespace Exception
	{
	FMTMissingField::FMTMissingField(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTmissingfield,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Un champ obligatoire est absent du jeu de données.",
				"A required field is missing from the dataset.")
		{
		}
	}