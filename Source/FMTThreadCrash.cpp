#include "FMTThreadCrash.h"

namespace Exception
	{
	FMTThreadCrash::FMTThreadCrash(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTthreadcrash,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Un thread de traitement s'est terminé de faÃ§on inattendue.",
				"A processing thread terminated unexpectedly.")
		{
		}
	}