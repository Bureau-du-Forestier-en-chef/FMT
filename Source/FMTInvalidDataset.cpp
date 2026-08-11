#include "FMTInvalidDataset.h"

namespace Exception
	{
	FMTInvalidDataset::FMTInvalidDataset(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTinvaliddataset,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Le jeu de données est invalide ou inaccessible.",
				"The dataset is invalid or inaccessible.")
		{
		}
	}