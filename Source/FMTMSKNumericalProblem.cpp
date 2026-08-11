#include "FMTMSKNumericalProblem.h"

namespace Exception
	{
	FMTMSKNumericalProblem::FMTMSKNumericalProblem(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTWarning(
				FMTexc::FMTMSKnumerical_problem,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Le modèle est numériquement insoluble avec les paramètres actuellement définis.",
				"The model is numerically unsolvable with the current parameter configuration.")
		{
		}
	}