#include "stdafx.h"
#include <sstream>
#include "FMTlpmodel.h"
#include "FMToutputnode.h"
#include "FMTmask.h"
#include "FMTmodelparameters.h"
#include "FMToperatingareascheduler.h"
#include "FMTtaskhandler.h"
#include "FMTareaparser.h"
#include "FMTopareaschedulertask.h"
#include <msclr\marshal_cppstd.h>
#include "FMTFormLogger.h"
#include "FMTForm.h"
#include "FMTFormCache.h"
#include "FMTdefaultlogger.h"

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
		try
		{
			FMTFormLogger* logger = FMTFormCache::GetInstance()->GetFormLogger();
			*logger << Logging::FMTDefaultLogger().getLogStamp() << "\n";
			*logger << "Préparation du modèle" << "\n";
			Models::FMTLpModel optimizationmodel(FMTFormCache::GetInstance()->getModel(scenario), static_cast<Models::FMTsolverinterface>(solver));
			*logger << "FMT -> Traitement pour le scénario : " + optimizationmodel.getName() << "\n";
			optimizationmodel.setParameter(Models::FMTintmodelparameters::LENGTH, nombrePeriodes);
			optimizationmodel.setParameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, true);
			optimizationmodel.setParameter(Models::FMTintmodelparameters::UPDATE, periodeMiseAjour);
			const int startingperiod = optimizationmodel.getParameter(Models::FMTintmodelparameters::UPDATE);
			const std::string Agg_name = "~BFECOPTOUTPUTYOUVERT~";
			std::vector<Core::FMTAction> newactions;
			int youvert = 0;
			for (Core::FMTAction& action : optimizationmodel.getactions())
			{
				if (action.useYield("YOUVERT"))
				{
					youvert += 1;
					std::vector<std::string> agg = action.getAggregates();
					if (std::count(agg.begin(), agg.end(), Agg_name))
					{
						*logger << "L'utilisateur à utiliser le nom ~BFECOPTOUTPUTYOUVERT~ dans ses outputs." << "\n";
						return false;
					}

					action.pushAggregate(Agg_name);
				}

				newactions.push_back(action);
			}

			if (youvert < 1)
			{
				*logger << "Aucune action dans le modèle n'a de yield youvert" << "\n";
				return false;
			}

			optimizationmodel.setActions(newactions);
			const std::vector<Core::FMTTheme> themes = optimizationmodel.getThemes();
			std::string stringMask = "";
			for (int i = 1; i <= themes.size(); i++)
			{
				if (stringMask == "")
				{
					stringMask += "?";
				}
				else
				{
					stringMask += " ?";
				}
			}

			Core::FMTMask fmtMask = Core::FMTMask(stringMask, themes);
			Core::FMTOutputNode nodeofoutput = Core::FMTOutputNode(fmtMask, Agg_name);
			//Fin createBFECoptaggregate
			/*Besoin de change la signature de fonction, les arguments suivants ne sont plus nécessaire :
			selectedmask
			presolvedtheme
			renommer postsolvedtheme par model themes car peut porter a confusion, mais les themes du modèles sont nécessaire.
			*/

			Parser::FMTAreaParser areaparser;
			std::vector<Heuristics::FMToperatingareascheme> opeareas = areaparser.getOperatingArea(
				msclr::interop::marshal_as<std::string>(fichierShp),
				optimizationmodel.getThemes(), numeroTheme,
				startingperiod, msclr::interop::marshal_as<std::string>(nomChampAge),
				msclr::interop::marshal_as<std::string>(nomChampSuperficie),
				msclr::interop::marshal_as<std::string>(nomChampStanlock),
				msclr::interop::marshal_as<std::string>(cheminParametres));
			*logger << "Résolution du modèle" << "\n";
			Parallel::FMTopareaschedulertask maintask(
				optimizationmodel,
				opeareas,
				nodeofoutput,
				msclr::interop::marshal_as<std::string>(nomFichierResultat) + "\\Retour",
				"YOUVERT",
				nombreIteration,
				tempsMaximum,
				ObtenirOutputSelectionnee(optimizationmodel.getOutputs(),
					returnTimeOutput));
			Parallel::FMTtaskhandler handler(maintask, nombreThread);
			*logger << "Génération du calendrier de COS" << "\n";
			handler.conccurentRun();
		}
		catch (...)
		{
			raiseFromCatch("", "FMTForm::OperatingAreaScheduling", __LINE__, __FILE__);
			return false;
		}

		return true;
	}

}