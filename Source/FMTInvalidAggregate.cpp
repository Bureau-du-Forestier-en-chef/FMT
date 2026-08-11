#include "FMTInvalidAggregate.h"

namespace Exception
	{
	FMTInvalidAggregate::FMTInvalidAggregate(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTinvalid_aggregate,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"L'agrégat défini dans le modèle est invalide ou contient des valeurs incohérentes.",
				"The aggregate defined in the model is invalid or contains inconsistent values.")
		{
		}
	}