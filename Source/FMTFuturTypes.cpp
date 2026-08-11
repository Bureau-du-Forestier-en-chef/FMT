#include "FMTFuturTypes.h"

namespace Exception
	{
	FMTFuturTypes::FMTFuturTypes(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTWarning(
				FMTexc::FMTfutur_types,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Le modèle contient des éléments provenant d'une version plus récente de FMT.",
				"The model contains elements originating from a newer version of FMT.")
		{
		}
	}