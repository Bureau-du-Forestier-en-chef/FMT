#include "stdafx.h"
#include <string>
#include <fstream>

#include <msclr/marshal_cppstd.h>

#include "FMTForm.h"
#include "FMTSchedule.h"
#include "FMTFormLogger.h"
#include "FMTExceptionHandlerWarning.h"
#include "FMTFormCache.h"
#include "Environment.h"
#include "ModelQuery.h"
#include "Selection.h"

namespace Wrapper
{
	namespace
	{
		// Conversion entrante : une liste managee devient un vecteur std.
		std::vector<std::string> _toStdVector(
			System::Collections::Generic::List<System::String^>^ p_values)
		{
			std::vector<std::string> converted;

			if (p_values == nullptr)
			{
				return converted;
			}

			for each (System::String ^ value in p_values)
			{
				converted.push_back(msclr::interop::marshal_as<std::string>(value));
			}

			return converted;
		}

		// Conversion sortante : un vecteur std devient une liste managee.
		System::Collections::Generic::List<System::String^>^ _toManagedList(
			const std::vector<std::string>& p_values)
		{
			System::Collections::Generic::List<System::String^>^ converted =
				gcnew System::Collections::Generic::List<System::String^>();

			for (const std::string& VALUE : p_values)
			{
				converted->Add(gcnew System::String(VALUE.c_str()));
			}

			return converted;
		}
	}

	System::String^ FMTForm::getChangeLog()
	{
		return _convertToSystemString(FMTWrapperCore::Environment::getChangeLog());
	}

	System::String^ FMTForm::getExceptionDescription(int p_exceptionId)
	{
		return _convertToSystemString(
			FMTWrapperCore::Environment::getExceptionDescription(p_exceptionId));
	}

	System::String^ FMTForm::_convertToSystemString(std::string value)
	{
		array<System::Byte>^ bytes = gcnew array<System::Byte>(static_cast<int>(value.size()));

		for (size_t i = 0; i < value.size(); ++i)
		{
			bytes[i] = static_cast<System::Byte>(value[i]);
		}

		return System::Text::Encoding::UTF8->GetString(bytes);
	}

	void FMTForm::_raiseFromCatch(
		std::string text,
		const std::string& method,
		const int& line,
		const std::string& fil)
	{
		FMTWrapperCore::FMTExceptionHandlerWarning* exhandler =
			FMTWrapperCore::FMTFormCache::GetInstance()->GetFormHandler();

		const std::string errorstack =
			exhandler->geterrorstack(
				text,
				method,
				line,
				fil);

		FMTWrapperCore::FMTFormLogger* logger =
			FMTWrapperCore::FMTFormCache::GetInstance()->GetFormLogger();

		if (logger)
		{
			logger->logWithLevel(
				"*************************************************************\n",
				0);

			logger->logWithLevel(
				"FMT - ERROR " + errorstack + "\n",
				0);
		}
		else
		{
			try
			{
				const std::string& logfile =
					FMTWrapperCore::FMTFormCache::GetInstance()->GetLoggerFilename();

				if (!logfile.empty())
				{
					std::ofstream out(
						logfile,
						std::ios_base::app);

					if (out.is_open())
					{
						out << "*************************************************************\n";
						out << "FMT - ERROR "
							<< errorstack
							<< "\n";
					}
				}
			}
			catch (...)
			{
			}
		}

		FeedBack(
			"*************************************************************",
			gcnew System::EventArgs());

		const std::string message =
			"FMT - ERROR " + errorstack;

		FeedBack(
			gcnew System::String(message.c_str()),
			gcnew System::EventArgs());

		exhandler->tryfileopener(errorstack);
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
			retour = _toManagedList(
				FMTWrapperCore::ModelQuery::getConstraintsAsText(indexScenario));
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

	std::vector<Core::FMTConstraint>
		FMTForm::_ObtenirArrayContraintesSelectionnees(
			std::vector<Core::FMTConstraint> contraitesBase,
			System::Collections::Generic::List<System::String^>^ contraintesSelection)
	{
		return FMTWrapperCore::Selection::selectConstraints(
			contraitesBase,
			_toStdVector(contraintesSelection));
	}

	std::vector<Core::FMTOutput>
		FMTForm::_ObtenirArrayOutputsSelectionnees(
			std::vector<Core::FMTOutput> outputsBase,
			System::Collections::Generic::List<System::String^>^ outputsSelection)
	{
		return FMTWrapperCore::Selection::selectOutputs(
			outputsBase,
			_toStdVector(outputsSelection));
	}

	Core::FMTOutput
		FMTForm::_ObtenirOutputSelectionnee(
			std::vector<Core::FMTOutput> outputsBase,
			System::String^ outputSelection)
	{
		return FMTWrapperCore::Selection::findOutput(
			outputsBase,
			msclr::interop::marshal_as<std::string>(outputSelection));
	}

	std::vector<Core::FMTSchedule>
		FMTForm::_ObtenirSEQ(
			System::String^ nomFichierPri,
			int indexScenario)
	{
		std::vector<Core::FMTSchedule> retour;

		try
		{
			retour = FMTWrapperCore::ModelQuery::readSchedules(
				msclr::interop::marshal_as<std::string>(nomFichierPri),
				indexScenario);
		}
		catch (...)
		{
			_raiseFromCatch(
				"",
				"FMTForm::_ObtenirSEQ",
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
			retour = _toManagedList(
				FMTWrapperCore::Environment::getVectorDriverExtensions());
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
			for (const int SOLVER : FMTWrapperCore::Environment::getAvailableSolvers())
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
				FMTWrapperCore::Environment::getSolverName(p_solveur).c_str());
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
			retour = _toManagedList(
				FMTWrapperCore::ModelQuery::getOutputsNames(indexScenario));
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
			retour = FMTWrapperCore::ModelQuery::getThemesCount(indexScenario);
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
			retour = FMTWrapperCore::ModelQuery::getPeriodsCount(
				msclr::interop::marshal_as<std::string>(nomFichierPri),
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
				FMTWrapperCore::ModelQuery::getActionsNames(p_index))
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
			aggregatesList = _toManagedList(
				FMTWrapperCore::ModelQuery::getAggregates(p_modelIndex));
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
			yieldsNamesConverted = _toManagedList(
				FMTWrapperCore::ModelQuery::getYieldsNames(p_index));
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
			result = FMTWrapperCore::ModelQuery::getYield(
				p_modelIndex,
				msclr::interop::marshal_as<std::string>(p_mask),
				msclr::interop::marshal_as<std::string>(p_yield),
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
				FMTWrapperCore::ModelQuery::getMaxAge(p_modelIndex));
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
			result = FMTWrapperCore::ModelQuery::validateMask(
				p_modelIndex,
				msclr::interop::marshal_as<std::string>(p_mask));
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
				FMTWrapperCore::ModelQuery::getAllMasks(
					p_modelIndex,
					p_periods,
					themes,
					msclr::interop::marshal_as<std::string>(p_cheminRasters)))
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
			FMTWrapperCore::ModelQuery::writeToProjectFromCache(
				msclr::interop::marshal_as<std::string>(p_destinationDirectory));
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
			FMTWrapperCore::FMTFormCache::GetInstance()->CloseLogger();
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
