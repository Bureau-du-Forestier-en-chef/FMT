#include "FMTOutputTooMuchOperator.h"

namespace Exception
	{
	FMTOutputTooMuchOperator::FMTOutputTooMuchOperator(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMToutput_too_much_operator,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une expression de sortie contient trop d'opérateurs ou est ambiguÃ«.",
				"An output expression contains too many operators or is ambiguous.")
		{
		}
	}