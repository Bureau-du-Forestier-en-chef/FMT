#include "FMTUndefinedAggregateValue.h"

namespace Exception
	{
	FMTUndefinedAggregateValue::FMTUndefinedAggregateValue(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTWarning(
				FMTexc::FMTundefined_aggregate_value,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une valeur d'agrégat référencée n'a jamais été définie.",
				"A referenced aggregate value has not been defined.")
		{
		}
	}