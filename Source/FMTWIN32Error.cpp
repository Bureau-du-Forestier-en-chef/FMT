#include "FMTWIN32Error.h"
#include "FMTSeException.h"

namespace Exception
	{
	FMTWIN32Error::FMTWIN32Error(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTWIN32_Error,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une erreur Win32 a été signalée par le système d'exploitation.",
				"A Win32 error was reported by the operating system.")
		{

		}

	#if defined _MSC_VER
		FMTWIN32Error::FMTWIN32Error(const FMTSeException& p_seError) :
			FMTWIN32Error(Core::FMTsection::Empty,
				"Win32 Error number " + std::to_string(p_seError.getSeNumber()),
				"FMTWIN32Error::FMTWIN32Error", __FILE__, __LINE__)
		{

		}
	#endif
	}