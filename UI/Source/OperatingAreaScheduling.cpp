#include "stdafx.h"
#include <msclr\marshal_cppstd.h>

#include "FMTForm.h"
#include "Controller.h"

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
		// fichierPri n'a jamais été utilisé : il reste dans la signature publique,
		// dont dépend le UI .NET.
		try
		{
			FMTWrapperCore::OperatingAreaParameters params;
			params.vectorFilePath = msclr::interop::marshal_as<std::string>(fichierShp);
			params.solver = solver;
			params.numberOfPeriods = nombrePeriodes;
			params.numberOfThreads = nombreThread;
			params.themeNumber = numeroTheme;
			params.maximumTime = tempsMaximum;
			params.numberOfIterations = nombreIteration;
			params.ageField = msclr::interop::marshal_as<std::string>(nomChampAge);
			params.areaField = msclr::interop::marshal_as<std::string>(nomChampSuperficie);
			params.lockField = msclr::interop::marshal_as<std::string>(nomChampStanlock);
			params.parametersFilePath = msclr::interop::marshal_as<std::string>(cheminParametres);
			params.resultFolder = msclr::interop::marshal_as<std::string>(nomFichierResultat);
			params.updatePeriod = periodeMiseAjour;

			// L'output de temps de retour est facultatif : marshal_as lève sur nullptr,
			// alors qu'un nom vide signifie simplement « aucun ».
			params.returnTimeOutputName = returnTimeOutput == nullptr
				? std::string()
				: msclr::interop::marshal_as<std::string>(returnTimeOutput);

			return FMTWrapperCore::Controller::scheduleOperatingAreas(params, scenario).success;
		}
		catch (...)
		{
			_raiseFromCatch("", "FMTForm::OperatingAreaScheduling", __LINE__, __FILE__);
			return false;
		}
	}
}
