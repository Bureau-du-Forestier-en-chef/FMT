#include "FMTInvalidConstraint.h"

namespace Exception
	{
	FMTInvalidConstraint::FMTInvalidConstraint(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTinvalid_constraint,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une contrainte du modèle est invalide ou mal formulée.",
				"A model constraint is invalid or incorrectly formulated.")
		{
		}
	}