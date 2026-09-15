#include "stdafx.h"
#include <msclr\marshal_cppstd.h>

#include "FMTForm.h"
#include "Controller.h"

bool Wrapper::FMTForm::Raterisation(
	System::String^ fichierPri,
	int scenario,
	System::String^ fichierShp,
	System::String^ repertoireSortie,
	int resolution,
	System::String^ nomChampAge,
	System::String^ nomChampSuperficie,
	System::String^ nomChampStanlock)
{
	// fichierPri n'a jamais été utilisé : il reste dans la signature publique,
	// dont dépend le UI .NET.
	try
	{
		FMTWrapperCore::RasterizationParameters params;
		params.vectorFilePath = msclr::interop::marshal_as<std::string>(fichierShp);
		params.outputFolder = msclr::interop::marshal_as<std::string>(repertoireSortie);
		params.resolution = resolution;
		params.ageField = msclr::interop::marshal_as<std::string>(nomChampAge);
		params.areaField = msclr::interop::marshal_as<std::string>(nomChampSuperficie);
		params.lockField = msclr::interop::marshal_as<std::string>(nomChampStanlock);

		FMTWrapperCore::Controller::rasterize(params, scenario);
	}
	catch (...)
	{
		_raiseFromCatch("", "Wrapper::FMTForm::Raterisation", __LINE__, __FILE__);
		return false;
	}

	return true;
}
