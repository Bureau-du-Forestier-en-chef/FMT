#include "FMTNonAddedConstraint.h"

namespace Exception
	{
	FMTNonAddedConstraint::FMTNonAddedConstraint(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTWarning(
				FMTexc::FMTnonaddedconstraint,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une contrainte n'a pas pu ètre ajoutée au problème d'optimisation.",
				"A constraint could not be added to the optimization problem.")
		{
		}
	}