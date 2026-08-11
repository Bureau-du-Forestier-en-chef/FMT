#include "FMTInvalidYieldNumber.h"

namespace Exception
	{
	FMTInvalidYieldNumber::FMTInvalidYieldNumber(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTinvalidyield_number,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Le numéro de rendement référencé est invalide.",
				"The referenced yield number is invalid.")
		{
		}
	}