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

bool Wrapper::FMTForm::OperatingAreaScheduling(
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
		FMTFormLogger* logger = Cache->getformlogger();
		*logger << Logging::FMTdefaultlogger().getlogstamp() << "\n";
		*logger << "Préparation du modèle" << "\n";
		Models::FMTlpmodel optimizationmodel(Cache->getmodel(scenario), static_cast<Models::FMTsolverinterface>(solver));
		*logger << "FMT -> Traitement pour le scénario : " + optimizationmodel.getname() << "\n";
		optimizationmodel.setparameter(Models::FMTintmodelparameters::LENGTH, nombrePeriodes);
		optimizationmodel.setparameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, true);
		optimizationmodel.setparameter(Models::FMTintmodelparameters::UPDATE, periodeMiseAjour);
		const int startingperiod = optimizationmodel.getparameter(Models::FMTintmodelparameters::UPDATE);
		const std::string Agg_name = "~BFECOPTOUTPUTYOUVERT~";
		std::vector<Core::FMTaction> newactions;
		int youvert = 0;
		for (Core::FMTaction& action : optimizationmodel.getactions())
		{
			if (action.useyield("YOUVERT"))
			{
				youvert += 1;
				std::vector<std::string> agg = action.getaggregates();
				if (std::count(agg.begin(), agg.end(), Agg_name))
				{
					*logger << "L'utilisateur à utiliser le nom ~BFECOPTOUTPUTYOUVERT~ dans ses outputs." << "\n";
					return false;
				}

				action.push_aggregate(Agg_name);
			}

			newactions.push_back(action);
		}

		if (youvert < 1)
		{
			*logger << "Aucune action dans le modèle n'a de yield youvert" << "\n";
			return false;
		}

		optimizationmodel.setactions(newactions);
		const std::vector<Core::FMTtheme> themes = optimizationmodel.getthemes();
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

		Core::FMTmask fmtMask = Core::FMTmask(stringMask, themes);
		Core::FMToutputnode nodeofoutput = Core::FMToutputnode(fmtMask, Agg_name);
		//Fin createBFECoptaggregate
		/*Besoin de change la signature de fonction, les arguments suivants ne sont plus nécessaire :
		selectedmask
		presolvedtheme
		renommer postsolvedtheme par model themes car peut porter a confusion, mais les themes du modèles sont nécessaire.
		*/

		Parser::FMTareaparser areaparser;
		std::vector<Heuristics::FMToperatingareascheme> opeareas = areaparser.getOperatingArea(
			msclr::interop::marshal_as<std::string>(fichierShp),
			optimizationmodel.getthemes(), numeroTheme, 
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
			ObtenirOutputSelectionnee(optimizationmodel.getoutputs(),
			returnTimeOutput));
		Parallel::FMTtaskhandler handler(maintask, nombreThread);
		*logger << "Génération du calendrier de COS" << "\n";
		handler.conccurentrun();
	}
	catch (...)
	{
		raisefromcatch("", "Wrapper::FMTForm::OperatingAreaScheduling", __LINE__, __FILE__);
		return false;
	}

	return true;
}
