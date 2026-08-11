#include "FMTUndefinedConstant.h"

namespace Exception
	{
	FMTUndefinedConstant::FMTUndefinedConstant(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTundefined_constant,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une constante référencée n'a jamais été définie.",
				"A referenced constant has not been defined.")
		{
		}
	}