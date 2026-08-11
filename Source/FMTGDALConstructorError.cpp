#include "FMTGDALConstructorError.h"

namespace Exception
	{
	FMTGDALConstructorError::FMTGDALConstructorError(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTgdal_constructor_error,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une erreur est survenue lors de la construction d'un objet GDAL.",
				"An error occurred while constructing a GDAL object.")
		{
		}
	}