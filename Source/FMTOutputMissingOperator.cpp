#include "FMTOutputMissingOperator.h"

namespace Exception
	{
	FMTOutputMissingOperator::FMTOutputMissingOperator(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMToutput_missing_operator,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une expression de sortie ne contient aucun opérateur valide.",
				"An output expression contains no valid operator.")
		{
		}
	}