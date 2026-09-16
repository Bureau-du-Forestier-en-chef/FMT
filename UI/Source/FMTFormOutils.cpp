#include "stdafx.h"
#include <string>

#include <msclr/marshal_cppstd.h>

#include "FMTForm.h"
#include "Controller.h"
#include "Conversions.h"

namespace Wrapper
{
	System::String^ FMTForm::getChangeLog()
	{
		return Conversions::fromUtf8(FMTWrapperCore::Controller::getChangeLog());
	}

	System::String^ FMTForm::getExceptionDescription(int p_exceptionId)
	{
		return Conversions::fromUtf8(
			FMTWrapperCore::Controller::getExceptionDescription(p_exceptionId));
	}

	void FMTForm::_raiseFromCatch(
		std::string text,
		const std::string& method,
		const int& line,
		const std::string& fil)
	{
		const std::string errorstack =
			FMTWrapperCore::Controller::logCurrentException(
				text,
				method,
				line,
				fil);

		FeedBack(
			"*************************************************************",
			gcnew System::EventArgs());

		const std::string message =
			"FMT - ERROR " + errorstack;

		FeedBack(
			gcnew System::String(message.c_str()),
			gcnew System::EventArgs());

		FMTWrapperCore::Controller::openErrorLocation(errorstack);
	}

	void FMTForm::_toFeedback(
		const char* p_message)
	{
		System::String^ newstr =
			gcnew System::String(p_message);

		System::String^ cleaned =
			newstr->Replace("\n", "");

		cleaned->Trim();

		if (cleaned->Length > 0)
		{
			FeedBack(
				cleaned,
				gcnew System::EventArgs());
		}
	}

	System::Collections::Generic::List<System::String^>^
		FMTForm::ObtenirListeContraintes(
			int indexScenario)
	{
		System::Collections::Generic::List<System::String^>^ retour =
			gcnew System::Collections::Generic::List<System::String^>();

		try
		{
			retour = Conversions::toManagedList(
				FMTWrapperCore::Controller::getConstraintsAsText(indexScenario));
		}
		catch (...)
		{
			retour->Clear();

			_raiseFromCatch(
				"",
				"FMTForm::ObtenirListeContraintes",
				__LINE__,
				__FILE__);
		}

		return retour;
	}

	System::Collections::Generic::List<System::String^>^
		FMTForm::ObtenirListeExtentionsSorties()
	{
		System::Collections::Generic::List<System::String^>^ retour =
			gcnew System::Collections::Generic::List<System::String^>();

		try
		{
			retour = Conversions::toManagedList(
				FMTWrapperCore::Controller::getVectorDriverExtensions());
		}
		catch (...)
		{
			retour->Clear();

			_raiseFromCatch(
				"",
				"FMTForm::ObtenirListeExtentionsSorties",
				__LINE__,
				__FILE__);
		}

		return retour;
	}

	System::Collections::Generic::List<int>^
		FMTForm::ObtenirListeSolvers()
	{
		System::Collections::Generic::List<int>^ retour =
			gcnew System::Collections::Generic::List<int>();

		try
		{
			for (const int SOLVER : FMTWrapperCore::Controller::getAvailableSolvers())
			{
				retour->Add(SOLVER);
			}
		}
		catch (...)
		{
			retour->Clear();

			_raiseFromCatch(
				"",
				"FMTForm::ObtenirListeSolvers",
				__LINE__,
				__FILE__);
		}

		return retour;
	}

	System::String^ FMTForm::ObtenirNomSolveur(int p_solveur)
	{
		System::String^ name;

		try
		{
			name = gcnew System::String(
				FMTWrapperCore::Controller::getSolverName(p_solveur).c_str());
		}
		catch (...)
		{
			_raiseFromCatch(
				"",
				"FMTForm::ObtenirNomSolveur",
				__LINE__,
				__FILE__);
		}

		return name;
	}

	System::Collections::Generic::List<System::String^>^
		FMTForm::ObtenirListeOutputs(
			int indexScenario)
	{
		System::Collections::Generic::List<System::String^>^ retour =
			gcnew System::Collections::Generic::List<System::String^>();

		try
		{
			retour = Conversions::toManagedList(
				FMTWrapperCore::Controller::getOutputsNames(indexScenario));
		}
		catch (...)
		{
			retour->Clear();

			_raiseFromCatch(
				"",
				"FMTForm::ObtenirListeOutputs",
				__LINE__,
				__FILE__);
		}

		return retour;
	}

	int FMTForm::ObtenirNombreThemes(
		int indexScenario)
	{
		int retour = 0;

		try
		{
			retour = FMTWrapperCore::Controller::getThemesCount(indexScenario);
		}
		catch (...)
		{
			_raiseFromCatch(
				"",
				"FMTForm::ObtenirNombreThemes",
				__LINE__,
				__FILE__);
		}

		return retour;
	}

	int FMTForm::ObtenirNombrePeriodes(
		System::String^ nomFichierPri,
		int indexScenario)
	{
		int retour = 0;

		try
		{
			retour = FMTWrapperCore::Controller::getPeriodsCount(
				Conversions::toStdString(nomFichierPri),
				indexScenario);
		}
		catch (...)
		{
			_raiseFromCatch(
				"",
				"FMTForm::ObtenirNombrePeriodes",
				__LINE__,
				__FILE__);
		}

		return retour;
	}

	System::Collections::Generic::List<System::String^>^
		FMTForm::getActionsNames(int p_index)
	{
		System::Collections::Generic::List<System::String^>^ actionsNames =
			gcnew System::Collections::Generic::List<System::String^>();

		try
		{
			for (const std::string& NAME :
				FMTWrapperCore::Controller::getActionsNames(p_index))
			{
				actionsNames->Add(
					msclr::interop::marshal_as<System::String^>(NAME));
			}
		}
		catch (...)
		{
			_raiseFromCatch(
				"",
				"FMTForm::getActionsNames",
				__LINE__,
				__FILE__);
		}

		return actionsNames;
	}

	System::Collections::Generic::List<System::String^>^
		FMTForm::getAggregates(int p_modelIndex)
	{
		System::Collections::Generic::List<System::String^>^ aggregatesList =
			gcnew System::Collections::Generic::List<System::String^>();

		try
		{
			aggregatesList = Conversions::toManagedList(
				FMTWrapperCore::Controller::getAggregates(p_modelIndex));
		}
		catch (...)
		{
			_raiseFromCatch(
				"",
				"FMTForm::getAggregates",
				__LINE__,
				__FILE__);
		}

		return aggregatesList;
	}

	System::Collections::Generic::List<System::String^>^
		FMTForm::getYields(int p_index)
	{
		System::Collections::Generic::List<System::String^>^
			yieldsNamesConverted =
			gcnew System::Collections::Generic::List<System::String^>();

		try
		{
			yieldsNamesConverted = Conversions::toManagedList(
				FMTWrapperCore::Controller::getYieldsNames(p_index));
		}
		catch (...)
		{
			_raiseFromCatch(
				"",
				"FMTForm::getYields",
				__LINE__,
				__FILE__);
		}

		return yieldsNamesConverted;
	}

	double FMTForm::getYield(
		int p_modelIndex,
		System::String^ p_mask,
		System::String^ p_yield,
		int p_age)
	{
		double result = 0.0;

		try
		{
			result = FMTWrapperCore::Controller::getYield(
				p_modelIndex,
				Conversions::toStdString(p_mask),
				Conversions::toStdString(p_yield),
				p_age);
		}
		catch (...)
		{
			_raiseFromCatch(
				"",
				"FMTForm::getYield",
				__LINE__,
				__FILE__);
		}

		return result;
	}

	double FMTForm::getMaxAge(int p_modelIndex)
	{
		double result = 0.0;

		try
		{
			result = static_cast<double>(
				FMTWrapperCore::Controller::getMaxAge(p_modelIndex));
		}
		catch (...)
		{
			_raiseFromCatch(
				"",
				"FMTForm::getMaxAge",
				__LINE__,
				__FILE__);
		}

		return result;
	}

	bool FMTForm::validateMask(
		const int p_modelIndex,
		System::String^ p_mask)
	{
		bool result = false;

		try
		{
			result = FMTWrapperCore::Controller::validateMask(
				p_modelIndex,
				Conversions::toStdString(p_mask));
		}
		catch (...)
		{
			_raiseFromCatch(
				"",
				"FMTForm::validateMask",
				__LINE__,
				__FILE__);
		}

		return result;
	}

	System::Collections::Generic::List<System::String^>^
		FMTForm::getAllMasks(
			int p_modelIndex,
			const int p_periods,
			System::Collections::Generic::List<int>^ p_themesNumbers,
			System::String^ p_cheminRasters)
	{
		System::Collections::Generic::List<System::String^>^ result =
			gcnew System::Collections::Generic::List<System::String^>();

		try
		{
			std::vector<int> themes;

			for each (int theme in p_themesNumbers)
			{
				themes.push_back(theme);
			}

			for (const std::string& MASK :
				FMTWrapperCore::Controller::getAllMasks(
					p_modelIndex,
					p_periods,
					themes,
					Conversions::toStdString(p_cheminRasters)))
			{
				result->Add(gcnew System::String(MASK.c_str()));
			}
		}
		catch (...)
		{
			_raiseFromCatch(
				"",
				"FMTForm::getAllMasks",
				__LINE__,
				__FILE__);
		}

		return result;
	}

	bool FMTForm::writetoprojectfromcache(
		System::String^ p_destinationDirectory)
	{
		bool passed = true;

		try
		{
			FMTWrapperCore::Controller::writeScenariosToProject(
				Conversions::toStdString(p_destinationDirectory));
		}
		catch (...)
		{
			_raiseFromCatch(
				"",
				"FMTForm::writetoprojectfromcache",
				__LINE__,
				__FILE__);

			passed = false;
		}

		return passed;
	}

	void FMTForm::CloseLogger()
	{
		try
		{
			FMTWrapperCore::Controller::closeLogger();
		}
		catch (...)
		{
			_raiseFromCatch(
				"",
				"FMTForm::CloseLogger",
				__LINE__,
				__FILE__);
		}
	}
}
