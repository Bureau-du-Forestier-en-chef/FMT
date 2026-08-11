#include "FMTUnhandledError.h"

namespace Exception
	{
	FMTUnhandledError::FMTUnhandledError(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTunhandlederror,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une erreur non gérée a été détectée.",
				"An unhandled error was detected.")
		{
		}

	FMTUnhandledError::FMTUnhandledError(const std::exception& p_stdException):
		FMTUnhandledError(Core::FMTsection::Empty,
			std::string("std::exception: ") + p_stdException.what(),
			"FMTUnhandledError::FMTUnhandledError", __FILE__, __LINE__)
		{

		}

	}