#include "FMTUnsupportedOutput.h"

namespace Exception
	{
	FMTUnsupportedOutput::FMTUnsupportedOutput(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTunsupported_output,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Le type de sortie demandé n'est pas supporté.",
				"The requested output type is not supported.")
		{
		}
	}