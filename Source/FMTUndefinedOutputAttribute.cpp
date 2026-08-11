#include "FMTUndefinedOutputAttribute.h"

namespace Exception
	{
	FMTUndefinedOutputAttribute::FMTUndefinedOutputAttribute(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTundefinedoutput_attribute,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Un attribut requis par une sortie n'est pas défini.",
				"An attribute required by an output is not defined.")
		{
		}
	}