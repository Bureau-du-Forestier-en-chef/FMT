#include "stdafx.h"
#include "FMTmodelparser.h"
#include "FMTforest.h"
#include "FMTareaparser.h"
#include <msclr\marshal_cppstd.h>
#include "FMTForm.h"
#include "FMTmodel.h"
#include "FMTFormCache.h"
#include "FMTFormLogger.h"

bool Wrapper::FMTForm::Raterisation(System::String^ fichierPri, int scenario, System::String^ fichierShp, System::String^ repertoireSortie, int resolution, System::String^ nomChampAge, System::String^ nomChampSuperficie, System::String^ nomChampStanlock)
{
	try
	{
		const std::vector<Core::FMTtheme> themes = Cache->getmodel(scenario).getthemes();
		Parser::FMTareaparser areaparser;
		const Spatial::FMTforest forest = areaparser.vectormaptoFMTforest(
			msclr::interop::marshal_as<std::string>(fichierShp),
			resolution,
			themes,
			msclr::interop::marshal_as<std::string>(nomChampAge),
			msclr::interop::marshal_as<std::string>(nomChampSuperficie),
			1,
			0.0001,
			msclr::interop::marshal_as<std::string>(nomChampStanlock),
			0.0,
			msclr::interop::marshal_as<std::string>(repertoireSortie),
			true);
	}
	catch (...)
	{
		raisefromcatch("", "Wrapper::FMTForm::Raterisation", __LINE__, __FILE__);
		return false;
	}

	return true;
}