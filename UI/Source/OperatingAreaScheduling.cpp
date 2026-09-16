#include "stdafx.h"
#include <msclr\marshal_cppstd.h>

#include "FMTForm.h"
#include "Controller.h"
#include "Conversions.h"

namespace Wrapper
{
	bool FMTForm::OperatingAreaScheduling(
		System::String^ fichierPri,
		int scenario,
		System::String^ fichierShp,
		int solver,
		int nombrePeriodes,
		int nombreThread,
		int numeroTheme,
		int tempsMaximum,
		int nombreIteration,
		System::String^ nomChampAge,
		System::String^ nomChampSuperficie,
		System::String^ nomChampStanlock,
		System::String^ cheminParametres,
		System::String^ nomFichierResultat,
		int periodeMiseAjour,
		System::String^ returnTimeOutput)
	{
		// fichierPri has never been used: it stays in the public signature,
		// which the .NET UI depends on.
		try
		{
			FMTWrapperCore::OperatingAreaParameters params;
			params.vectorFilePath = Conversions::toStdString(fichierShp);
			params.solver = solver;
			params.numberOfPeriods = nombrePeriodes;
			params.numberOfThreads = nombreThread;
			params.themeNumber = numeroTheme;
			params.maximumTime = tempsMaximum;
			params.numberOfIterations = nombreIteration;
			params.ageField = Conversions::toStdString(nomChampAge);
			params.areaField = Conversions::toStdString(nomChampSuperficie);
			params.lockField = Conversions::toStdString(nomChampStanlock);
			params.parametersFilePath = Conversions::toStdString(cheminParametres);
			params.resultFolder = Conversions::toStdString(nomFichierResultat);
			params.updatePeriod = periodeMiseAjour;

			// The return time output is optional: an empty name means "none".
			params.returnTimeOutputName = Conversions::toStdString(returnTimeOutput);

			return FMTWrapperCore::Controller::scheduleOperatingAreas(params, scenario).success;
		}
		catch (...)
		{
			_raiseFromCatch("", "FMTForm::OperatingAreaScheduling", __LINE__, __FILE__);
			return false;
		}
	}
}
