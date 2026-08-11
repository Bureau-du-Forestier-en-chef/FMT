#include "FMTInvalidRasterBlock.h"

namespace Exception
	{
	FMTInvalidRasterBlock::FMTInvalidRasterBlock(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTinvalidrasterblock,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Un bloc raster est invalide ou ne peut ètre lu.",
				"A raster block is invalid or cannot be read.")
		{
		}
	}