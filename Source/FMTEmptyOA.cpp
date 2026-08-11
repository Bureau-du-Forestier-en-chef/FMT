#include "FMTEmptyOA.h"

namespace Exception
	{
	FMTEmptyOA::FMTEmptyOA(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTEmpty_OA,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Aucune aire d'opération n'est disponible.",
				"No operating area is available.")
		{
		}
	}