#include "FMTUndefinedAttribute.h"

namespace Exception
	{
	FMTUndefinedAttribute::FMTUndefinedAttribute(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTundefined_attribute,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Un attribut référencé est absent ou non défini.",
				"A referenced attribute is missing or undefined.")
		{
		}
	}