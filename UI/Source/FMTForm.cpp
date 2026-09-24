#include "stdafx.h"
#include "FMTForm.h"

#include <msclr/marshal_cppstd.h>
#include <vcclr.h>

#include "Controller.h"
#include "Conversions.h"

namespace Wrapper
{
	namespace
	{
		// Built here rather than in a method of FMTForm: cl refuses a local class in a
		// member function of a managed class (C3923). gcroot keeps the form reachable for
		// the Core, which holds the subscriber.
		FMTWrapper::Backend::EventHandler makeEventHandler(FMTForm^ p_form)
		{
			gcroot<FMTForm^> form(p_form);

			return [form](const FMTWrapper::Backend::Event& p_event)
			{
				if (const FMTWrapper::Backend::LogEvent* log =
					std::get_if<FMTWrapper::Backend::LogEvent>(&p_event))
				{
					form->_toFeedback(log->message.c_str());
				}
				else if (const FMTWrapper::Backend::ErrorEvent* error =
					std::get_if<FMTWrapper::Backend::ErrorEvent>(&p_event))
				{
					form->_toErrorFeedback(error->errorStack.c_str());
				}

				// An event the interface does not know yet is ignored.
			};
		}
	}


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

			FMTWrapper::Backend::Controller::setErrorsToWarnings(
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
			for (int error : FMTWrapper::Backend::Controller::getErrorsToIgnore())
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
		// Backward-compatible alias: the external .NET UI still calls this method.
		SetErrorsToWarnings(
			listeWarnings,
			maxWarnings);
	}

	void FMTForm::RecoverFromCrash()
	{
		try
		{
			// The subscription survives a crash: only the logger and the exception
			// handler are rebuilt.
			FMTWrapper::Backend::Controller::recoverLoggerAndHandler();
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
			if (m_eventSubscription == 0)
			{
				m_eventSubscription =
					FMTWrapper::Backend::Controller::subscribe(
						makeEventHandler(this));
			}

			const std::string filename =
				Conversions::toStdString(
					nomFichierLogger);

			FMTWrapper::Backend::Controller::initializeLogger(filename);
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
				Conversions::toStdString(
					fichierPriSystem);

			std::vector<std::string> scenarios;

			std::string scenario =
				Conversions::toStdString(
					scenarioSystem);

			scenarios.push_back(scenario);

			FMTWrapper::Backend::Controller::addScenarios(
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
			FMTWrapper::Backend::Controller::removeScenario(
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
			FMTWrapper::Backend::Controller::clearScenarios();
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