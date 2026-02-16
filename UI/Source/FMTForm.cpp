#include "stdafx.h"
#include "FMTForm.h"
#include "FMTmodelparser.h"
#include <msclr\marshal_cppstd.h>
#include "FMTscheduleparser.h"
#include "FMTFormCache.h"
#include "FMTmodel.h"
#include "FMTtheme.h"

namespace Wrapper
{ 


void FMTForm::Cache_InitialiserModelParser(
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
		raisefromcatch("", "FMTForm::Cache_InitialiserModelParser", __LINE__, __FILE__);
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
		raisefromcatch("", "FMTForm::Cache_AssignerNomLogger", __LINE__, __FILE__);
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
		Parser::FMTmodelparser Modelparser;
		const std::vector<Models::FMTmodel> models = Modelparser.readproject(fichierPri, scenarios);
		for (const Models::FMTmodel& model : models)
		{
			FMTFormCache::GetInstance()->push_back(model);
		}
		return true;
	}
	catch (...)
	{
		raisefromcatch("", "FMTForm::Cache_AjouterScenarios", __LINE__, __FILE__);
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
		raisefromcatch("", "FMTForm::Cache_EnleverModel", __LINE__, __FILE__);
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
		raisefromcatch("", "FMTForm::Cache_Vider", __LINE__, __FILE__);
	}
}
}