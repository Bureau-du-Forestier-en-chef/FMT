#include "FMTInvalidPath.h"

namespace Exception
	{
	FMTInvalidPath::FMTInvalidPath(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTinvalid_path,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Le chemin vers une ressource ou un fichier est invalide.",
				"The path to a required resource or file is invalid.")
		{
		}
	}