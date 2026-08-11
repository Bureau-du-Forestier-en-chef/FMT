#include "FMTFunctionFailed.h"

namespace Exception
	{
	FMTFunctionFailed::FMTFunctionFailed(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTfunctionfailed,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une fonction interne a échoué.",
				"An internal function failed.")
		{
		}
	}