#include "stdafx.h"
#include <msclr\marshal_cppstd.h>

#include "FMTForm.h"
#include "Controller.h"
#include "Conversions.h"

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
	// fichierPri has never been used: it stays in the public signature,
	// which the .NET UI depends on.
	try
	{
		FMTWrapper::Backend::RasterizationParameters params;
		params.vectorFilePath = Conversions::toStdString(fichierShp);
		params.outputFolder = Conversions::toStdString(repertoireSortie);
		params.resolution = resolution;
		params.ageField = Conversions::toStdString(nomChampAge);
		params.areaField = Conversions::toStdString(nomChampSuperficie);
		params.lockField = Conversions::toStdString(nomChampStanlock);

		FMTWrapper::Backend::Controller::rasterize(params, scenario);
	}
	catch (...)
	{
		_raiseFromCatch("", "Wrapper::FMTForm::Raterisation", __LINE__, __FILE__);
		return false;
	}

	return true;
}
