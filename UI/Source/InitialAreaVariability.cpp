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
#include "FMTdefaultlogger.h"

namespace Wrapper{

bool FMTForm::InitialAreaVariability(
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
		FMTFormLogger* logger = FMTFormCache::GetInstance()->GetFormLogger();
		*logger << Logging::FMTDefaultLogger().getLogStamp() << "\n";
		Models::FMTLpModel optimizationmodel(FMTFormCache::GetInstance()->getModel(scenario), static_cast<Models::FMTsolverinterface>(solver));
		*logger << "FMT -> Traitement pour le scénario : " + optimizationmodel.getName() << "\n";
		*logger << "FMT Event Spatialy Explicit Simulation c++ - > Intégration des contraintes sélectionnées" << "\n";
		optimizationmodel.setConstraints(ObtenirArrayContraintesSelectionnees(optimizationmodel.getconstraints(), contraintes));

		//Période
		for (size_t per = 0; per < period; ++per)
		{
			optimizationmodel.buildPeriod();
		}
	
		std::vector<Core::FMTTheme> themes = optimizationmodel.getThemes();
		//std::vector<Core::FMTMask> masktargets = { Core::FMTMask("? PEUPLEMENT2 ?", themes), Core::FMTMask("? PEUPLEMENT3 ?", themes) };
		std::vector<Core::FMTMask> masktargets = {};
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
				masktargets.push_back(Core::FMTMask(msclr::interop::marshal_as<std::string>(mask), themes));
			}

			for (Core::FMTActualDevelopment development : optimizationmodel.getArea()) {
				int count = 0;
				for (Core::FMTMask target : masktargets) {
					if (development.getMask().isSubsetOf(target)) {
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
				std::vector<Core::FMTOutput> listeOutputs;
				for (const Core::FMTOutput& fmtOutput : optimizationmodel.getOutputs())
				{
					if (outputs->Contains(gcnew System::String(fmtOutput.getName().c_str())))
					{
						listeOutputs.push_back(fmtOutput);
					}
				}
				*logger << "FMT - Démarrage de Initial Area Variability" << "\n";
				Parser::FMTModelParser Modelparser = FMTFormCache::GetInstance()->GetConfiguredParser();
				Modelparser.writeResults(
					optimizationmodel.getModelFromProportions(masktargets, proportions),
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
		raiseFromCatch("", "FMTForm::InitialAreaVariability", __LINE__, __FILE__);
		return false;
	}	

	return true;
}

}
