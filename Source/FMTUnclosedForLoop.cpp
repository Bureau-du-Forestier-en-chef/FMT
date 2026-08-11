#include "FMTUnclosedForLoop.h"

namespace Exception
	{
	FMTUnclosedForLoop::FMTUnclosedForLoop(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTunclosedforloop,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une boucle FOR n'a pas été correctement fermée.",
				"A FOR loop was not properly closed.")
		{
		}
	}