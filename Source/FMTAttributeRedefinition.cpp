#include "FMTAttributeRedefinition.h"

namespace Exception
	{
	FMTAttributeRedefinition::FMTAttributeRedefinition(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTWarning(
				FMTexc::FMTattribute_redefinition,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Un attribut déjÃ  défini a été redéfini.",
				"A previously defined attribute was redefined.")
		{
		}
	}