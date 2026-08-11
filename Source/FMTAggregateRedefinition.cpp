#include "FMTAggregateRedefinition.h"

namespace Exception
	{
	FMTAggregateRedefinition::FMTAggregateRedefinition(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTWarning(
				FMTexc::FMTaggregate_redefinition,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Un agrégat déjÃ  défini a été redéfini dans le modèle.",
				"A previously defined aggregate was redefined in the model.")
		{
		}
	}