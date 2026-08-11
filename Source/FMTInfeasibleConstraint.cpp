#include "FMTInfeasibleConstraint.h"

namespace Exception
	{
	FMTInfeasibleConstraint::FMTInfeasibleConstraint(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTinfeasibleconstraint,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une contrainte rend le problème d'optimisation infaisable.",
				"A constraint makes the optimization problem infeasible.")
		{
		}
	}