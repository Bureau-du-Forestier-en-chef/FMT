#include "FMTInvalidLayer.h"

namespace Exception
	{
	FMTInvalidLayer::FMTInvalidLayer(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTinvalidlayer,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"La couche géospatiale est invalide ou inaccessible.",
				"The geospatial layer is invalid or inaccessible.")
		{
		}
	}