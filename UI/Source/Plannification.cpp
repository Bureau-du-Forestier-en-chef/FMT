#include "stdafx.h"
#include <sstream>
#include "FMTlpmodel.h"
#include "FMTmodelparser.h"
#include "FMTscheduleparser.h"
#include "FMTutility.h"
#include "FMTnssmodel.h"
#include "FMTtaskhandler.h"
#include "FMTreplanningtask.h"
#include <msclr\marshal_cppstd.h>
#include <string>
#include <iostream>
#include "FMTFormLogger.h"
#include "FMTForm.h"
#include "FMTFormCache.h"
#include "FMTplanningtask.h"
#include "FMTdefaultlogger.h"

namespace Wrapper
{

	bool  FMTForm::Plannification(
		System::String^ fichierPri,
		System::Collections::Generic::List<int>^ scenarios,
		int solver,
		int period,
		int nbreProcessus,
		System::Collections::Generic::List<System::String^>^ outputs,
		int outputLevel,
		int etanduSortiesMin,
		int etanduSortiesMax,
		System::String^ cheminSorties,
		System::String^ providerGdal,
		System::Collections::Generic::List<bool>^ playback)
	{
		try
		{
			FMTFormLogger* logger = FMTFormCache::GetInstance()->GetFormLogger();
			*logger << Logging::FMTdefaultlogger().getlogstamp() << "\n";
			std::vector<std::string> layersoptions;
			if (msclr::interop::marshal_as<std::string>(providerGdal) == "CSV")
			{
				layersoptions.push_back("SEPARATOR=SEMICOLON");
			}

			Parallel::FMTplanningtask newplanningtask(etanduSortiesMin, etanduSortiesMax,
				msclr::interop::marshal_as<std::string>(cheminSorties),
				msclr::interop::marshal_as<std::string>(providerGdal),
				layersoptions, static_cast<Core::FMToutputlevel>(outputLevel),
				msclr::interop::marshal_as<std::string>(fichierPri));
			for each (int scen in scenarios)
			{
				Models::FMTlpmodel optimizationmodel(FMTFormCache::GetInstance()->getmodel(scen), static_cast<Models::FMTsolverinterface>(solver));
				*logger << "FMT -> Préparation pour le scénario : " + optimizationmodel.getname() << "\n";
				std::vector<Core::FMTschedule> cedule;
				bool playbackscen = playback[scenarios->IndexOf(scen)];
				if (playbackscen)
				{
					*logger << "FMT -> Lecture de cédule pour le scénario : " + optimizationmodel.getname() << "\n";
					cedule = ObtenirSEQ(fichierPri, scen);
				}
				optimizationmodel.setstrictlypositivesoutputsmatrix();
				optimizationmodel.setparameter(Models::FMTintmodelparameters::LENGTH, period);
				int valeur_NUMBER_OF_THREADS = 1;
				if (scenarios->Count <= nbreProcessus)
				{
					valeur_NUMBER_OF_THREADS = nbreProcessus / scenarios->Count;
				}

				optimizationmodel.setparameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, valeur_NUMBER_OF_THREADS);
				optimizationmodel.FMTmodel::setparameter(Models::FMTdblmodelparameters::TOLERANCE, 0.01);
				std::vector<Core::FMToutput> selectedoutputs = ObtenirArrayOutputsSelectionnees(optimizationmodel.getoutputs(), outputs);
				optimizationmodel.setparameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, playbackscen);
				newplanningtask.push_back(optimizationmodel, cedule, selectedoutputs);
				*logger << "FMT -> Scénario : " + optimizationmodel.getname() + " prêt a être lancer." << "\n";
			}

			Parallel::FMTtaskhandler handler(newplanningtask, nbreProcessus);
			handler.conccurentrun();
		}
		catch (...)
		{
			raisefromcatch("", "FMTForm::Plannification", __LINE__, __FILE__);
			return false;
		}

		return true;
	}

	bool FMTForm::Replanification(
		int indexScenStrategique,
		int indexScenStochastique,
		int indexScenTactique,
		int solver,
		int period,
		int periodReplannif,
		double variabilite,
		int nbreProcessus,
		int nombreReplicasMin,
		int nombreReplicasMax,
		System::Collections::Generic::List<System::String^>^ outputs,
		int outputLevel,
		System::String^ cheminSorties,
		System::String^ providerGdal,
		int taskLogLevel,
		bool indProduireSolution,
		bool p_writeSchedule)
	{
		try
		{
			FMTFormLogger* logger = FMTFormCache::GetInstance()->GetFormLogger();
			*logger << Logging::FMTdefaultlogger().getlogstamp() << "\n";
			Models::FMTlpmodel global(FMTFormCache::GetInstance()->getmodel(indexScenStrategique), static_cast<Models::FMTsolverinterface>(solver));
			global.setparameter(Models::FMTintmodelparameters::LENGTH, period);
			global.setparameter(Models::FMTboolmodelparameters::DEBUG_MATRIX, true);
			global.setparameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, 1);
			global.setparameter(Models::FMTboolmodelparameters::PRESOLVE_CAN_REMOVE_STATIC_THEMES, true);
			Models::FMTnssmodel stochastic(FMTFormCache::GetInstance()->getmodel(indexScenStochastique), 0);
			stochastic.setparameter(Models::FMTintmodelparameters::LENGTH, 1);
			stochastic.setparameter(Models::FMTboolmodelparameters::DEBUG_MATRIX, true);
			Models::FMTlpmodel local(FMTFormCache::GetInstance()->getmodel(indexScenTactique), static_cast<Models::FMTsolverinterface>(solver));
			local.setparameter(Models::FMTintmodelparameters::LENGTH, 1);
			local.setparameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, 1);
			local.setparameter(Models::FMTboolmodelparameters::DEBUG_MATRIX, true);
			std::vector<Core::FMToutput> listeOutputs = ObtenirArrayOutputsSelectionnees(global.getoutputs(), outputs);

			std::vector<std::string>layersoptions;
			if (msclr::interop::marshal_as<std::string>(providerGdal) == "CSV")
			{
				layersoptions.push_back("SEPARATOR=SEMICOLON");
			}
			*logger << "FMT -> Préparation de la replanification " << "\n";
			logger->logtime();
			Parallel::FMTreplanningtask* task = new Parallel::FMTreplanningtask(
				global,
				stochastic,
				local,
				listeOutputs,
				msclr::interop::marshal_as<std::string>(cheminSorties),
				msclr::interop::marshal_as<std::string>(providerGdal),
				layersoptions,
				nombreReplicasMax,
				periodReplannif,
				variabilite,
				static_cast<Core::FMToutputlevel>(outputLevel),
				indProduireSolution);
			task->setreplicates(nombreReplicasMin, nombreReplicasMax);
			std::unique_ptr<Parallel::FMTtask> maintaskptr(task);
			*logger << "FMT -> Préparation de la replanification terminée" << "\n";
			Parallel::FMTtaskhandler handler(maintaskptr, nbreProcessus);
			logger->settasklogginglevel(taskLogLevel);
			handler.ondemandrun();
			//handler.conccurentrun();
			logger->logtime();
			logger->setdefaultlogginglevel();
		}
		catch (...)
		{
			raisefromcatch("", "FMTForm::Replanification", __LINE__, __FILE__);
			return false;
		}

		return true;
	}
}
