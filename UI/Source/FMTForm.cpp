#include "stdafx.h"
#include "FMTForm.h"
#include "FMTModelParser.h"
#include <msclr\marshal_cppstd.h>
#include "FMTScheduleParser.h"
#include "FMTFormCache.h"
#include "FMTModel.h"
#include "FMTTheme.h"

namespace Wrapper
{ 


void FMTForm::SetErrorsToWarnings(
	System::Collections::Generic::List<int>^ listeWarnings,
	int maxWarnings)
{
	try
	{
		int defaultwarnings = 10;
		if (maxWarnings > 0)
		{
			defaultwarnings = maxWarnings;
		}

		std::vector<Exception::FMTexc> listeExceptions;
		for each (int valeur in listeWarnings)
		{
			listeExceptions.push_back(static_cast<Exception::FMTexc>(valeur));
		}
		FMTFormCache::GetInstance()->InitializeExceptionHandler(defaultwarnings, listeExceptions);
	}
	catch (...)
	{
		raiseFromCatch("", "FMTForm::SetErrorsToWarnings", __LINE__, __FILE__);
	}
}

void FMTForm::Cache_InitialiserModelParser(
	System::Collections::Generic::List<int>^ listeWarnings,
	int maxWarnings)
{
	// Alias retrocompatible : le UI .NET externe appelle encore cette methode.
	SetErrorsToWarnings(listeWarnings, maxWarnings);
}

void FMTForm::RecoverFromCrash()
{
	try
	{
		// Recree un delegue manage frais : apres un crash, l'ancien pointeur de
		// fonction peut etre invalide. Le logger est ensuite reconstruit sur le
		// fichier deja fige (mode append), avec un exception handler neuf.
		managed = gcnew managedFeed(this, &FMTForm::ToFeedBack);
		unmanaged = System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(managed);
		FMTFormCache::GetInstance()->RecoverLoggerAndHandler(unmanaged);
	}
	catch (...)
	{
		raiseFromCatch("", "FMTForm::RecoverFromCrash", __LINE__, __FILE__);
	}
}

void FMTForm::Cache_AssignerNomLogger(System::String^ nomFichierLogger)
{
	try
	{
		managed = gcnew managedFeed(this, &FMTForm::ToFeedBack);
		unmanaged = System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(managed);
		const std::string filename = msclr::interop::marshal_as<std::string>(nomFichierLogger);
		FMTFormCache::GetInstance()->InitializeLogger(filename, unmanaged);
	}
	catch (...)
	{
		raiseFromCatch("", "FMTForm::Cache_AssignerNomLogger", __LINE__, __FILE__);
	}
}

bool FMTForm::Cache_AjouterScenarios(
	System::String^ fichierPriSystem,
	System::String^ scenarioSystem)
{
	try
	{
		std::string fichierPri = msclr::interop::marshal_as<std::string>(fichierPriSystem);
		std::vector<std::string>scenarios;
		std::string scenario = msclr::interop::marshal_as<std::string>(scenarioSystem);
		scenarios.push_back(scenario);
		Parser::FMTModelParser Modelparser = FMTFormCache::GetInstance()->GetConfiguredParser();
		const std::vector<Models::FMTModel> models = Modelparser.readproject(fichierPri, scenarios);
		for (const Models::FMTModel& model : models)
		{
			FMTFormCache::GetInstance()->push_back(model);
		}
		return true;
	}
	catch (...)
	{
		raiseFromCatch("", "FMTForm::Cache_AjouterScenarios", __LINE__, __FILE__);
	}

	return false;
}

bool FMTForm::Cache_EnleverModel(int indexScenario)
{
	try
	{
		FMTFormCache::GetInstance()->erase(indexScenario);
		return true;
	}
	catch (...)
	{
		raiseFromCatch("", "FMTForm::Cache_EnleverModel", __LINE__, __FILE__);
	}

	return false;
}

void FMTForm::Cache_Vider()
{
	try
	{
		FMTFormCache::GetInstance()->clear();
	}
	catch (...)
	{
		raiseFromCatch("", "FMTForm::Cache_Vider", __LINE__, __FILE__);
	}
}
}