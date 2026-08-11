#include "FMTYieldModelPrediction.h"

namespace Exception
	{
	FMTYieldModelPrediction::FMTYieldModelPrediction(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTWarning(
				FMTexc::FMTyieldmodelprediction,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Un avertissement est survenu lors de la prédiction d'un modèle de rendement.",
				"A warning occurred while predicting a yield model.")
		{
		}
	}