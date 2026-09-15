#include "stdafx.h"
#include "FMTForm.h"

#include <msclr/marshal_cppstd.h>

#include "Controller.h"

namespace Wrapper
{

	void FMTForm::SetErrorsToWarnings(
		System::Collections::Generic::List<int>^ listeWarnings,
		int maxWarnings)
	{
		try
		{
			std::vector<int> listeExceptions;

			for each (int valeur in listeWarnings)
			{
				listeExceptions.push_back(valeur);
			}

			FMTWrapperCore::Controller::setErrorsToWarnings(
				listeExceptions,
				maxWarnings);
		}
		catch (...)
		{
			_raiseFromCatch(
				"",
				"FMTForm::SetErrorsToWarnings",
				__LINE__,
				__FILE__);
		}
	}

	System::Collections::Generic::List<int>^ FMTForm::getErrorsToIgnore()
	{
		System::Collections::Generic::List<int>^ errors = gcnew System::Collections::Generic::List<int>();
		try
		{
			for (int error : FMTWrapperCore::Controller::getErrorsToIgnore())
			{
				errors->Add(error);
			}
			
		}
		catch (...)
		{
			_raiseFromCatch(
				"",
				" FMTForm::getErrorsToIgnore",
				__LINE__,
				__FILE__);
		}
		return errors;
	}

	void FMTForm::Cache_InitialiserModelParser(
		System::Collections::Generic::List<int>^ listeWarnings,
		int maxWarnings)
	{
		// Alias retrocompatible : le UI .NET externe appelle encore cette methode.
		SetErrorsToWarnings(
			listeWarnings,
			maxWarnings);
	}

	void FMTForm::RecoverFromCrash()
	{
		try
		{
			// Recree un delegue manage frais : apres un crash, l'ancien pointeur
			// de fonction peut etre invalide.

			m_managedFeed =
				gcnew ManagedFeed(
					this,
					&FMTForm::_toFeedback);

			m_unmanagedFeed =
				System::Runtime::InteropServices::Marshal::
				GetFunctionPointerForDelegate(
					m_managedFeed);

			FMTWrapperCore::Controller::recoverLoggerAndHandler(
				m_unmanagedFeed.ToPointer());
		}
		catch (...)
		{
			_raiseFromCatch(
				"",
				"FMTForm::RecoverFromCrash",
				__LINE__,
				__FILE__);
		}
	}

	void FMTForm::Cache_AssignerNomLogger(
		System::String^ nomFichierLogger)
	{
		try
		{
			m_managedFeed =
				gcnew ManagedFeed(
					this,
					&FMTForm::_toFeedback);

			m_unmanagedFeed =
				System::Runtime::InteropServices::Marshal::
				GetFunctionPointerForDelegate(
					m_managedFeed);

			const std::string filename =
				msclr::interop::marshal_as<std::string>(
					nomFichierLogger);

			FMTWrapperCore::Controller::initializeLogger(
				filename,
				m_unmanagedFeed.ToPointer());
		}
		catch (...)
		{
			_raiseFromCatch(
				"",
				"FMTForm::Cache_AssignerNomLogger",
				__LINE__,
				__FILE__);
		}
	}

	bool FMTForm::Cache_AjouterScenarios(
		System::String^ fichierPriSystem,
		System::String^ scenarioSystem)
	{
		try
		{
			std::string fichierPri =
				msclr::interop::marshal_as<std::string>(
					fichierPriSystem);

			std::vector<std::string> scenarios;

			std::string scenario =
				msclr::interop::marshal_as<std::string>(
					scenarioSystem);

			scenarios.push_back(scenario);

			FMTWrapperCore::Controller::addScenarios(
				fichierPri,
				scenarios);

			return true;
		}
		catch (...)
		{
			_raiseFromCatch(
				"",
				"FMTForm::Cache_AjouterScenarios",
				__LINE__,
				__FILE__);
		}

		return false;
	}

	bool FMTForm::Cache_EnleverModel(
		int indexScenario)
	{
		try
		{
			FMTWrapperCore::Controller::removeScenario(
				indexScenario);

			return true;
		}
		catch (...)
		{
			_raiseFromCatch(
				"",
				"FMTForm::Cache_EnleverModel",
				__LINE__,
				__FILE__);
		}

		return false;
	}

	void FMTForm::Cache_Vider()
	{
		try
		{
			FMTWrapperCore::Controller::clearScenarios();
		}
		catch (...)
		{
			_raiseFromCatch(
				"",
				"FMTForm::Cache_Vider",
				__LINE__,
				__FILE__);
		}
	}

}