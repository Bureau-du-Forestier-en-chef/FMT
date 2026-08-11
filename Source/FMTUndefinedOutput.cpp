#include "FMTUndefinedOutput.h"

namespace Exception
	{
	FMTUndefinedOutput::FMTUndefinedOutput(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTundefined_output,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une sortie demandée n'a pas été définie.",
				"A requested output has not been defined.")
		{
		}
	}