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
		const std::vector<Core::FMTtheme> THEMES = FMTFormCache::GetInstance()->getmodel(scenario).getthemes();
		const std::vector<Core::FMTactualdevelopment> AREA = FMTFormCache::GetInstance()->getmodel(scenario).getarea();
		const std::string VECTORS_PATH = msclr::interop::marshal_as<std::string>(fichierShp);
		const std::string OUTPUT_FOLDER = msclr::interop::marshal_as<std::string>(repertoireSortie);
		Parser::FMTareaparser areaparser;
		Spatial::FMTforest forest = areaparser.vectormaptoFMTforest(
			VECTORS_PATH,
			resolution,
			THEMES,
			msclr::interop::marshal_as<std::string>(nomChampAge),
			msclr::interop::marshal_as<std::string>(nomChampSuperficie),
			1,
			0.0001,
			msclr::interop::marshal_as<std::string>(nomChampStanlock),
			0.0);
		areaparser.WriteForestExtended(forest,
							VECTORS_PATH, THEMES, AREA, OUTPUT_FOLDER);
	}catch (...)
		{
		raisefromcatch("", "Wrapper::FMTForm::Raterisation", __LINE__, __FILE__);
		return false;
		}
	return true;
}