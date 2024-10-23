#include "stdafx.h"
#include <string>
#include <sstream>
#include "FMTlpmodel.h"
#include "FMTmodelparser.h"
#include <msclr\marshal_cppstd.h>
#include "FMTmask.h"
#include "FMTFormLogger.h"
#include "FMTForm.h"
#include "FMTFormCache.h"

bool Wrapper::FMTForm::InitialAreaVariability(
	System::String^ fichierPri,
	int scenario, 
	int solver,
	System::Collections::Generic::List<System::String^>^ contraintes, 
	int period, 
	System::Collections::Generic::List<System::String^>^ outputs, 
	int outputLevel, 
	int etanduSortiesMin, 
	int etanduSortiesMax, 
	System::String^ cheminSorties, 
	System::String^ providerGdal, 
	System::Collections::Generic::List<System::Collections::Generic::List<System::String^>^>^ ListeInformations)
{
	try
	{
		FMTFormLogger* logger = Cache->getformlogger();
		Models::FMTlpmodel optimizationmodel(Cache->getmodel(scenario), static_cast<Models::FMTsolverinterface>(solver));
		*logger << "FMT -> Traitement pour le scénario : " + optimizationmodel.getname() << "\n";
		*logger << "FMT Event Spatialy Explicit Simulation c++ - > Intégration des contraintes sélectionnées" << "\n";
		optimizationmodel.setconstraints(ObtenirArrayContraintesSelectionnees(optimizationmodel.getconstraints(), contraintes));

		//Période
		for (size_t per = 0; per < period; ++per)
		{
			optimizationmodel.buildperiod();
		}
	
		std::vector<Core::FMTtheme> themes = optimizationmodel.getthemes();
		//std::vector<Core::FMTmask> masktargets = { Core::FMTmask("? PEUPLEMENT2 ?", themes), Core::FMTmask("? PEUPLEMENT3 ?", themes) };
		std::vector<Core::FMTmask> masktargets = {};
		//std::vector<double> proportions{0.01, -0.1};
		std::vector<double> proportions{};

		if (ListeInformations && ListeInformations->Count > 1) {
			for (size_t ligne = 1; ligne < ListeInformations->Count; ligne++)
			{
				System::String^ mask = "";

				for (int i = 0; i < ListeInformations[ligne]->Count; i++) {
					if (ListeInformations[ligne]->ToArray()->GetValue(ListeInformations[ligne]->Count - 1)->ToString() != ListeInformations[ligne]->ToArray()->GetValue(i)->ToString()) {
						System::String^ valeur = ListeInformations[ligne]->ToArray()->GetValue(i)->ToString();
						mask += valeur + " ";
					}
				}

				mask = mask->Trim();
				proportions.push_back(std::atof(msclr::interop::marshal_as<std::string>(ListeInformations[ligne]->ToArray()->GetValue(ListeInformations[ligne]->Count - 1)->ToString()).c_str()));
				masktargets.push_back(Core::FMTmask(msclr::interop::marshal_as<std::string>(mask), themes));
			}

			for (Core::FMTactualdevelopment development : optimizationmodel.getarea()) {
				int count = 0;
				for (Core::FMTmask target : masktargets) {
					if (development.getmask().issubsetof(target)) {
						count += 1;
					}
				}
				if (count > 1) {
					*logger << "Intersecting globalmask!" << "\n";
					exit(-1);
				}
			}

			if (outputs->Count > 0)
			{
				std::vector<Core::FMToutput> listeOutputs;
				for (const Core::FMToutput& fmtOutput : optimizationmodel.getoutputs())
				{
					if (outputs->Contains(gcnew System::String(fmtOutput.getname().c_str())))
					{
						listeOutputs.push_back(fmtOutput);
					}
				}
				*logger << "FMT - Démarrage de Initial Area Variability" << "\n";
				Parser::FMTmodelparser Modelparser;
				Modelparser.writeresults(
					optimizationmodel.getmodelfromproportions(masktargets, proportions),
					listeOutputs,
					etanduSortiesMin,
					etanduSortiesMax,
					msclr::interop::marshal_as<std::string>(cheminSorties),
					static_cast<Core::FMToutputlevel>(outputLevel),
					msclr::interop::marshal_as<std::string>(providerGdal)
				);
				*logger << "FMT - Initial Area Variability complété." << "\n";
			}
			else
			{
				*logger << "FMT - Modèle non réalisable" << "\n";
				return false;
			}
		}
		else 
		{
			if (!ListeInformations) {
				*logger << "FMT - Arrêt du traitement. Le fichier de paramètres .csv est obligatoire." << "\n";
				return false;
			}
			else {
				*logger << "FMTErreur - Arrêt du traitement. Le fichier de paramètres .csv est vide." << "\n";
				return false;
			}
		}		
	}
	catch (...)
	{
		raisefromcatch("", "Wrapper::FMTForm::InitialAreaVariability", __LINE__, __FILE__);
		return false;
	}	

	return true;
}