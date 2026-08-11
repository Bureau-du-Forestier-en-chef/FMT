#include "FMTIgnore.h"

namespace Exception
	{
	FMTIgnore::FMTIgnore(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTWarning(
				FMTexc::FMTignore,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une condition a été ignorée conformément Ã  la configuration du modèle.",
				"A condition was ignored according to the model configuration.")
		{
		}
	}