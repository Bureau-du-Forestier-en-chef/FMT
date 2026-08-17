#include "stdafx.h"
#include <string>
#include <fstream>
#include <algorithm> // Added

#include "FMTConstraint.h"
#include "FMTModelParser.h"
#include "FMTOutput.h"
#include "FMTParser.h"
#include "FMTLpModel.h"
#include "FMTAreaParser.h"
#include "FMTScheduleParser.h"
#include <msclr/marshal_cppstd.h>
#include "FMTLpSolver.h"

#include "FMTForm.h"
#include "FMTFormLogger.h"
#include "FMTExceptionHandlerWarning.h"
#include "FMTFormCache.h"
#include "Tools.h"

namespace Wrapper
{

	System::String^ FMTForm::getChangeLog()
	{
		return gcnew System::String(
			FMTWrapperCore::Tools::getChangeLog().c_str());
	}

	System::String^ FMTForm::getExceptionDescription(
		int p_exceptionId)
	{
		return gcnew System::String(
			FMTWrapperCore::Tools::getExceptionDescription(
				p_exceptionId).c_str());
	}

	void FMTForm::_raiseFromCatch(
		std::string text,
		const std::string& method,
		const int& line,
		const std::string& fil)
	{
		FMTExceptionHandlerWarning* exhandler =
			FMTFormCache::GetInstance()->GetFormHandler();

		const std::string errorstack =
			exhandler->geterrorstack(
				text,
				method,
				line,
				fil);

		FMTFormLogger* logger =
			FMTFormCache::GetInstance()->GetFormLogger();

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
					FMTFormCache::GetInstance()->GetLoggerFilename();

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

	System::Collections::Generic::List<System::String^>^
		FMTForm::ObtenirListeContraintes(
			int indexScenario)
	{
		System::Collections::Generic::List<System::String^>^ retour =
			gcnew System::Collections::Generic::List<
			System::String^>();

		try
		{
			for (const Core::FMTConstraint& constraint :
				_ObtenirArrayContraintes(indexScenario))
			{
				retour->Add(
					gcnew System::String(
						std::string(constraint).c_str()));
			}
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
		FMTForm::_ObtenirArrayContraintes(
			int indexScenario)
	{
		std::vector<Core::FMTConstraint> retour;

		try
		{
			retour =
				FMTFormCache::GetInstance()
				->getModel(indexScenario)
				.getConstraints();
		}
		catch (...)
		{
			FMTFormCache::GetInstance()
				->GetFormHandler()
				->raiseFromCatch(
					"",
					"FMTForm::ObtenirArrayContraintes",
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
		std::vector<Core::FMTConstraint> retour;

		try
		{
			for (const Core::FMTConstraint& contrainte : contraitesBase)
			{
				for each (System::String ^ selection in contraintesSelection)
				{
					std::string stdSelection =
						msclr::interop::marshal_as<std::string>(selection);

					std::string stdContraite =
						std::string(contrainte);

					stdSelection.erase(
						std::remove(
							stdSelection.begin(),
							stdSelection.end(),
							'\n'),
						stdSelection.cend());

					stdContraite.erase(
						std::remove(
							stdContraite.begin(),
							stdContraite.end(),
							'\n'),
						stdContraite.cend());

					stdSelection.erase(
						std::remove(
							stdSelection.begin(),
							stdSelection.end(),
							'\r'),
						stdSelection.cend());

					stdContraite.erase(
						std::remove(
							stdContraite.begin(),
							stdContraite.end(),
							'\r'),
						stdContraite.cend());

					System::String^ selectiontexte =
						(gcnew System::String(stdSelection.c_str()))
						->TrimEnd();

					System::String^ contraiteTexte =
						(gcnew System::String(stdContraite.c_str()))
						->TrimEnd();

					if (selectiontexte == contraiteTexte)
					{
						retour.push_back(contrainte);
					}
				}
			}
		}
		catch (...)
		{
			FMTFormCache::GetInstance()
				->GetFormHandler()
				->raiseFromCatch(
					"",
					"FMTForm::ObtenirArrayContraintesSelectionnees",
					__LINE__,
					__FILE__);
		}

		return retour;
	}

	std::vector<Core::FMTOutput>
		FMTForm::_ObtenirArrayOutputsSelectionnees(
			std::vector<Core::FMTOutput> outputsBase,
			System::Collections::Generic::List<System::String^>^ outputsSelection)
	{
		std::vector<Core::FMTOutput> retour;

		try
		{
			for (const Core::FMTOutput& fmtOutput : outputsBase)
			{
				if (outputsSelection->Contains(
					gcnew System::String(
						fmtOutput.getName().c_str())))
				{
					retour.push_back(fmtOutput);
				}
			}
		}
		catch (...)
		{
			FMTFormCache::GetInstance()
				->GetFormHandler()
				->raiseFromCatch(
					"",
					"FMTForm::ObtenirArrayOutputsSelectionnees",
					__LINE__,
					__FILE__);
		}

		return retour;
	}

	Core::FMTOutput
		FMTForm::_ObtenirOutputSelectionnee(
			std::vector<Core::FMTOutput> outputsBase,
			System::String^ outputSelection)
	{
		Core::FMTOutput retour;

		try
		{
			for (const Core::FMTOutput& fmtOutput : outputsBase)
			{
				if (outputSelection ==
					gcnew System::String(
						fmtOutput.getName().c_str()))
				{
					retour = fmtOutput;
					break;
				}
			}
		}
		catch (...)
		{
			FMTFormCache::GetInstance()
				->GetFormHandler()
				->raiseFromCatch(
					"",
					"FMTForm::ObtenirOutputSelectionnee",
					__LINE__,
					__FILE__);
		}

		return retour;
	}

	System::Collections::Generic::List<System::String^>^
		FMTForm::ObtenirListeExtentionsSorties()
	{
		System::Collections::Generic::List<System::String^>^ retour =
			gcnew System::Collections::Generic::List<
			System::String^>();

		try
		{
			Parser::FMTModelParser Modelparser =
				FMTFormCache::GetInstance()->GetConfiguredParser();

			const std::vector<std::vector<std::string>>
				listeExtensions =
				Modelparser.getGDALVectorDriverExtensions();

			const std::vector<std::string>
				listeDrivers =
				Modelparser.getGDALVectorDriverNames();

			for (int index = 0;
				index < listeExtensions.size();
				++index)
			{
				for (int indexExtension = 0;
					indexExtension < listeExtensions.at(index).size();
					++indexExtension)
				{
					retour->Add(
						gcnew System::String(
							(listeDrivers.at(index) +
								"|*." +
								listeExtensions.at(index).at(indexExtension))
							.c_str()));
				}
			}
		}
		catch (...)
		{
			retour =
				gcnew System::Collections::Generic::List<
				System::String^>();

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
			for (Models::FMTSolverInterface solver :
			Models::FMTLpSolver::getAvailableSolverInterface())
			{
				retour->Add(
					static_cast<int>(solver));
			}
		}
		catch (...)
		{
			retour =
				gcnew System::Collections::Generic::List<int>();

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
			const Models::FMTSolverInterface SOLVEUR = static_cast<Models::FMTSolverInterface>(p_solveur);
			const std::string NAME = std::string(Models::FMTLpSolver::toString(SOLVEUR));
			name = gcnew System::String(NAME.c_str());
		}catch (...)
		{

			_raiseFromCatch(
				"",
				"FMTForm::ObtenirNomSolveur",
				__LINE__,
				__FILE__);
		}
		return name;
	}

	void FMTForm::_InscrireLigneFichierTexte(
		System::String^ nomFichier,
		System::String^ message,
		bool indicateurFeedback,
		bool nouveaufichier)
	{
		try
		{
			if (indicateurFeedback)
			{
				FeedBack(
					message,
					gcnew System::EventArgs());
			}

			const std::string ficher =
				msclr::interop::marshal_as<std::string>(
					nomFichier);

			std::ofstream fichierResultat;

			if (!nouveaufichier)
			{
				fichierResultat.open(
					ficher,
					std::ios_base::app);
			}
			else
			{
				fichierResultat.open(ficher);
			}

			fichierResultat
				<< msclr::interop::marshal_as<std::string>(
					message + "\n");

			fichierResultat.close();
		}
		catch (...)
		{
			_raiseFromCatch(
				"",
				"FMTForm::_InscrireLigneFichierTexte",
				__LINE__,
				__FILE__);
		}
	}

	System::Collections::Generic::List<System::String^>^
		FMTForm::ObtenirListeOutputs(
			int indexScenario)
	{
		System::Collections::Generic::List<System::String^>^ retour =
			gcnew System::Collections::Generic::List<System::String^>();

		try
		{
			if (!FMTFormCache::GetInstance()->empty())
			{
				for (const Core::FMTOutput& output :
					FMTFormCache::GetInstance()
					->getModel(indexScenario)
					.getOutputs())
				{
					retour->Add(
						gcnew System::String(
							std::string(output.getName()).c_str()));
				}
			}
		}
		catch (...)
		{
			retour =
				gcnew System::Collections::Generic::List<System::String^>();

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
			retour =
				static_cast<int>(
					FMTFormCache::GetInstance()
					->getModel(indexScenario)
					.getThemes()
					.size());
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
			retour =
				_ObtenirSEQ(
					nomFichierPri,
					indexScenario)
				.back()
				.getPeriod();
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

	std::vector<Core::FMTSchedule>
		FMTForm::_ObtenirSEQ(
			System::String^ nomFichierPri,
			int indexScenario)
	{
		std::vector<Core::FMTSchedule> retour;

		try
		{
			Parser::FMTScheduleParser schedulerparser;

			Parser::FMTModelParser Modelparser =
				FMTFormCache::GetInstance()
				->GetConfiguredParser();

			std::vector<Models::FMTModel> models;

			models.push_back(
				FMTFormCache::GetInstance()
				->getModel(indexScenario));

			std::vector<Core::FMTSchedule> liste =
				Modelparser.readSchedules(
					msclr::interop::marshal_as<std::string>(
						nomFichierPri),
					models).at(0);

			if (!liste.empty())
			{
				retour = liste;
			}
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
		FMTForm::getActionsNames(int p_index)
	{
		System::Collections::Generic::List<System::String^>^ actionsNames =
			gcnew System::Collections::Generic::List<System::String^>();

		try
		{
			if (FMTFormCache::GetInstance()->empty())
			{
				throw std::out_of_range("Invalid model index");
			}

			const Models::FMTModel MODEL =
				FMTFormCache::GetInstance()->getModel(p_index);

			const std::vector<Core::FMTAction> ACTIONS =
				MODEL.getActions();

			for (int i = 0; i < static_cast<int>(ACTIONS.size()); ++i)
			{
				System::String^ name =
					msclr::interop::marshal_as<System::String^>(
						ACTIONS[i].getName());

				actionsNames->Add(name);
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
			if (FMTFormCache::GetInstance()->empty())
			{
				throw std::out_of_range("Invalid model index");
			}

			const Models::FMTModel MODEL =
				FMTFormCache::GetInstance()->getModel(p_modelIndex);

			const std::vector<Core::FMTAction> ACTIONS =
				MODEL.getActions();

			std::set<std::string> uniqueAggregates;

			for (int i = 0; i < static_cast<int>(ACTIONS.size()); ++i)
			{
				std::vector<std::string> agg =
					ACTIONS[i].getAggregates();

				for (const auto& aggregate : agg)
				{
					uniqueAggregates.insert(aggregate);
				}
			}

			for (const auto& aggregate : uniqueAggregates)
			{
				aggregatesList->Add(
					gcnew System::String(
						aggregate.c_str()));
			}
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
			gcnew System::Collections::Generic::List<
			System::String^>();

		try
		{
			if (FMTFormCache::GetInstance()->empty())
			{
				throw std::out_of_range("Invalid model index");
			}

			const Models::FMTModel& MODEL =
				FMTFormCache::GetInstance()->getModel(p_index);

			const Core::FMTYields YIELDS =
				MODEL.getYields();

			std::vector<std::string> yieldsNames =
				YIELDS.getAllYieldNames();

			for (int i = 0;
				i < static_cast<int>(yieldsNames.size());
				++i)
			{
				System::String^ convertedString =
					gcnew System::String(
						yieldsNames[i].c_str());

				yieldsNamesConverted->Add(
					convertedString);
			}
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
		double result = 0.0; // Fixed: was NULL

		try
		{
			if (FMTFormCache::GetInstance()->empty())
			{
				throw std::out_of_range("Invalid model index");
			}

			const std::string CONVERTEDSTRING =
				msclr::interop::marshal_as<std::string>(
					p_mask);

			const std::string CONVERTEDYIELD =
				msclr::interop::marshal_as<std::string>(
					p_yield);

			const Models::FMTModel& MODEL =
				FMTFormCache::GetInstance()
				->getModel(p_modelIndex);

			result =
				FMTWrapperCore::Tools::getYield(
					MODEL,
					CONVERTEDSTRING,
					CONVERTEDYIELD,
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
			if (FMTFormCache::GetInstance()->empty())
			{
				throw std::out_of_range("Invalid model index");
			}

			const Models::FMTModel MODEL =
				FMTFormCache::GetInstance()
				->getModel(p_modelIndex);

			result = static_cast<double>(
				FMTWrapperCore::Tools::getMaxAge(
					MODEL));
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

	bool FMTForm::validateMask(
		const int p_modelIndex,
		System::String^ p_mask)
	{
		bool result = false;

		try
		{
			if (FMTFormCache::GetInstance()->empty())
			{
				throw std::out_of_range("Invalid model index");
			}

			std::string MASK =
				msclr::interop::marshal_as<std::string>(
					p_mask);

			const Models::FMTModel& MODEL =
				FMTFormCache::GetInstance()
				->getModel(p_modelIndex);

			const std::vector<Core::FMTTheme> THEMES =
				MODEL.getThemes();

			result =
				Core::FMTTheme::validate(
					THEMES,
					MASK);
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
			gcnew System::Collections::Generic::List<
			System::String^>();

		try
		{
			if (FMTFormCache::GetInstance()->empty())
			{
				throw std::out_of_range("Invalid model index");
			}

			const Models::FMTModel& MODEL =
				FMTFormCache::GetInstance()
				->getModel(p_modelIndex);

			std::vector<int> themes;

			for each (int theme in p_themesNumbers)
			{
				themes.push_back(theme);
			}

			std::string rasterPath =
				msclr::interop::marshal_as<std::string>(
					p_cheminRasters);

			std::set<std::string> masks =
				FMTWrapperCore::Tools::getAllMasks(
					MODEL,
					p_periods,
					themes,
					rasterPath);

			for (const std::string& mask : masks)
			{
				result->Add(
					gcnew System::String(
						mask.c_str()));
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
			if (FMTFormCache::GetInstance()->empty())
			{
				throw std::out_of_range(
					"Empty cache");
			}

			const std::string DESTINATION_DIRECTORY =
				msclr::interop::marshal_as<std::string>(
					p_destinationDirectory);

			std::vector<Models::FMTModel> models;

			models.reserve(
				FMTFormCache::GetInstance()->size());

			for (size_t index = 0;
				index < FMTFormCache::GetInstance()->size();
				++index)
			{
				models.push_back(
					FMTFormCache::GetInstance()
					->getModel(
						static_cast<int>(index)));
			}

			FMTWrapperCore::Tools::writeToProject(
				models,
				DESTINATION_DIRECTORY);
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
			FMTFormCache::GetInstance()
				->CloseLogger();
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