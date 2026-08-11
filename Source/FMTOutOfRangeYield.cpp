#include "FMTOutOfRangeYield.h"

namespace Exception
	{
	FMTOutOfRangeYield::FMTOutOfRangeYield(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMToutofrangeyield,
				FMTlev::FMT_range,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une valeur de rendement se situe Ã  l'extérieur des limites acceptables du modèle.",
				"A yield value falls outside the acceptable limits of the model.")
		{
		}
	}