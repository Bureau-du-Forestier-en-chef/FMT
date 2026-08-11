#include "FMTDeathWithLock.h"

namespace Exception
	{
	FMTDeathWithLock::FMTDeathWithLock(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTdeathwithlock,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Le développement forestier deviendra inactif avant l'expiration du verrouillage.",
				"The forest development will become inactive before the lock expires.")
		{
		}
	}