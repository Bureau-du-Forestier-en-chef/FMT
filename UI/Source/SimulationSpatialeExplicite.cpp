#include "stdafx.h"
#include <sstream>
#include "FMTforest.h"
#include "FMTlpmodel.h"
#include "FMTmodelparser.h"
#include "FMTsesmodel.h"
#include "FMTareaparser.h"
#include "FMTscheduleparser.h"
#include "FMTtransitionparser.h"
#include "FMTGCBMtransition.h"
#include "FMToutputnode.h"
#include <msclr\marshal_cppstd.h>
#include "FMTFormLogger.h"
#include "FMTForm.h"
#include "FMTmodel.h"
#include "FMTFormCache.h"
#include "FMTexceptionhandlerwarning.h"


void Wrapper::FMTForm::RapportdeBris(const Models::FMTsemodel& semodel)
{
	try{
		semodel.LogConstraintsInfeasibilities();
	}catch (...)
	{
		Cache->getformhandler()->raisefromcatch("", "Wrapper::FMTForm::RapportdeBris", __LINE__, __FILE__);
	}
}

void Wrapper::FMTForm::RapportdeCarboneSpatial(const Models::FMTsemodel& semodel,const int& nombredeperiodes, const std::vector<Core::FMTschedule>& schedules)
{
	try {
		Models::FMTmodel localmodel(semodel);
		const Spatial::FMTspatialschedule& schedule = semodel.getspschedule();
		const std::vector<Core::FMTschedule> newschedule = schedule.getschedules(semodel.getactions(), false);
		size_t scid = 0;
		for (int period = 1; period <= nombredeperiodes; ++period)
		{
			int jsonloc = period - 1;
			std::vector<Core::FMTconstraint>periodicconstraints = semodel.getconstraints();
			for (Core::FMTconstraint& periodconstraint : periodicconstraints)
			{
				const int lowerperiod = periodconstraint.getperiodlowerbound();
				const int upperperiod = std::min(period, periodconstraint.getperiodupperbound());
				periodconstraint.setlength(lowerperiod, upperperiod);
			}
			localmodel.setconstraints(periodicconstraints);
			double primalinf = 0;
			double objectivevalue = 0;
			schedule.getsolutionstatus(objectivevalue, primalinf, localmodel, nullptr, true, false);
			RetourJson("objectives;" + jsonloc + ";Objective;" + objectivevalue, gcnew System::EventArgs());
			RetourJson("objectives;" + jsonloc + ";Primalinfeasibility;" + primalinf, gcnew System::EventArgs());
			double oldtotal = 0;
			double newtotal = 0;
			size_t oriloc = 0;
			for (const Core::FMTschedule& schedule : schedules)
			{
				if (schedule.getperiod() == period)
				{
					break;
				}
				++oriloc;
			}
			size_t newloc = 0;
			for (const Core::FMTschedule& schedule : newschedule)
			{
				if (schedule.getperiod() == period)
				{
					break;
				}
				++newloc;
			}
			if (scid < newschedule.size() && scid < schedules/*.at(0)*/.size())
			{
				for (const auto& data : schedules.at(oriloc))
				{
					const double basearea = schedules.at(oriloc).actionarea(data.first);
					double newarea = 0;
					if (newschedule.at(newloc).find(data.first) != newschedule.at(newloc).end())
					{
						newarea = newschedule.at(newloc).actionarea(data.first);
					}
					oldtotal += basearea;
					newtotal += newarea;
					RetourJson("objectives;" + jsonloc + ";" + gcnew System::String(data.first.getname().c_str()) + ";" + (newarea / basearea), gcnew System::EventArgs());
				}
				RetourJson("objectives;" + jsonloc + ";Total;" + (newtotal / oldtotal), gcnew System::EventArgs());
			}
			++scid;
		}
	}catch (...)
	{
		Cache->getformhandler()->raisefromcatch("", "Wrapper::FMTForm::RapportdeCarboneSpatial", __LINE__, __FILE__);
	}

}

void Wrapper::FMTForm::EcrituredesPerturbations(const Models::FMTsemodel& semodel, System::String^ cheminsorties,const int& nombredeperiodes, System::Collections::Generic::List<int>^ growththemes, const bool& incarbon)
{
	try {
		const std::string rastpath = msclr::interop::marshal_as<std::string>(cheminsorties);
		std::vector<Core::FMTtheme>growththeme;
		if (growththemes->Count != 0)
		{
			for each (int themeID in growththemes)
			{
				growththeme.push_back(semodel.getthemes().at(themeID - 1));
			}
		}
		const Spatial::FMTspatialschedule& schedule = semodel.getspschedule();
		FMTFormLogger* logger = Cache->getformlogger();
		*logger << "FMT -> Écriture des perturbations" << "\n";
		Parser::FMTtransitionparser transitionparser;
		Parser::FMTareaparser areaparser;
		for (int period = 1; period <= nombredeperiodes; ++period)
		{
			//const Spatial::FMTspatialschedule spatialsolution = simulationmodel.getspschedule();
			const std::vector<Core::FMTaction>actions = semodel.getactions();
			const std::vector<Core::FMTGCBMtransition>transitions = areaparser.writedisturbances(rastpath,
				schedule,
				actions,
				growththeme, period);
			std::string fichier = rastpath + "transition" + std::to_string(period) + ".txt";
			transitionparser.writeGCBM(transitions, fichier);
			if (incarbon)
			{
				RetourJson(gcnew System::String(("GCBMtransitionlocations;" + fichier).c_str()), gcnew System::EventArgs());
			}
		}
	}catch (...)
		{
		Cache->getformhandler()->raisefromcatch("", "Wrapper::FMTForm::EcrituredesPerturbations", __LINE__, __FILE__);
		}
}

void Wrapper::FMTForm::EcritureDesEvenements(const Models::FMTsemodel& semodel, System::String^ cheminsorties,const int& nombredeperiodes, const bool& incarbon)
	{
	try {
		FMTFormLogger* logger = Cache->getformlogger();
		const Spatial::FMTspatialschedule& schedule = semodel.getspschedule();
			const std::vector<Core::FMTaction>actions = semodel.getactions();
			const std::string stats = schedule.getpatchstats(actions);
			System::String^ eventpath = System::IO::Path::Combine(cheminsorties, gcnew System::String(std::string("events.txt").c_str()));
			*logger << "Écriture des évènements ici: " + msclr::interop::marshal_as<std::string>(eventpath) << "\n";
			//InscrireLigneFichierTexte(eventpath, "Period Action size perimeter height width", false,true);
			InscrireLigneFichierTexte(eventpath, gcnew System::String(stats.c_str()), false,true);
			if (incarbon)
			{
				const std::string stdeventpath = msclr::interop::marshal_as<std::string>(eventpath);
				RetourJson(gcnew System::String(("eventslocation;" + stdeventpath).c_str()), gcnew System::EventArgs());
			}
	}catch (...)
		{
		Cache->getformhandler()->raisefromcatch("", "Wrapper::FMTForm::EcritureDesEvenements", __LINE__, __FILE__);
		}
	}

std::vector<Core::FMToutput>  Wrapper::FMTForm::EcritureDesOutputs(const Models::FMTsemodel& semodel, System::Collections::Generic::List<System::String^>^ outputs, const int& nombredeperiodes, const bool& incarbon)
{
	std::vector<Core::FMToutput> listeOutputs;
	try {
		FMTFormLogger* logger = Cache->getformlogger();
		const Spatial::FMTspatialschedule& schedule = semodel.getspschedule();
		listeOutputs = ObtenirArrayOutputsSelectionnees(semodel.getoutputs(), outputs);
		for (const Core::FMToutput& output : listeOutputs)
			{
			if (incarbon)
			{
				std::map<std::string, std::vector<double>> resultatOutputsCarbon = schedule.getoutput(semodel, output, 1, nombredeperiodes);

				for (double valeurCarbon : resultatOutputsCarbon["Total"])
				{
					*logger << "outputs;" + output.getname() + ";" + std::to_string(valeurCarbon) << "\n";
					RetourJson("outputs;" + gcnew System::String(output.getname().c_str()) + ";" + valeurCarbon, gcnew System::EventArgs());
				}
			}

			}
	}catch (...)
	{
		Cache->getformhandler()->raisefromcatch("", "Wrapper::FMTForm::EcritureDesOutputs", __LINE__, __FILE__);
	}
	return listeOutputs;
}

void Wrapper::FMTForm::EcrituredesOutputsSpatiaux(const Models::FMTsemodel& semodel, const std::vector<Core::FMToutput>& outputs, const int& sortiemin, const int& sortiemax, System::String^ localisation)
{
	try {
		FMTFormLogger* logger = Cache->getformlogger();
		Parser::FMTareaparser areaparser;
		*logger << "FMT -> Écriture des outputs spatiaux" << "\n";
		for (int period = sortiemin; period <= sortiemax; ++period)
		{
			for (const Core::FMToutput& output : outputs)
			{
				const std::string outputname = output.getname() + "_" + std::to_string(period) + ".tif";
				System::String^ outputrasterpath = System::IO::Path::Combine(localisation, gcnew System::String(outputname.c_str()));
				const std::string stdoutputpath = msclr::interop::marshal_as<std::string>(outputrasterpath);
				const Spatial::FMTlayer<double> outputlayer = semodel.getspatialoutput(output, period);
				areaparser.writelayer(outputlayer, stdoutputpath);
			}
		}

		*logger << "FMT -> Écriture des outputs spatiaux terminée" << "\n";
		
	}
	catch (...)
	{
		Cache->getformhandler()->raisefromcatch("", "Wrapper::FMTForm::EcrituredesOutputsSpatiaux", __LINE__, __FILE__);
	}
}

void Wrapper::FMTForm::EcritureDesPredicteurs(const Models::FMTsemodel& semodel, const std::string& rastpath, const int& periodes, System::Collections::Generic::List<System::String^>^ predictoryields)
{
	try {
		Parser::FMTareaparser areaparser;
		std::vector<std::string> yieldsforpredictors;
		for each (System::String ^ valeur in predictoryields)
		{
			yieldsforpredictors.push_back(msclr::interop::marshal_as<std::string>(valeur));
		}
		const Spatial::FMTspatialschedule& schedule = semodel.getspschedule();
		std::vector<std::vector<std::vector<double>>> allpredictors;
		std::vector<std::string>allpredictornames;
		for (size_t period = 1; period <= periodes; ++period)
		{
			std::vector<std::vector<double>> periodpredictors;
			std::vector<std::vector<Graph::FMTpredictor>> predictors = areaparser.writepredictors(rastpath, schedule, yieldsforpredictors, semodel, period);
			allpredictornames = predictors.back().back().getpredictornames(yieldsforpredictors);
			size_t indexPredictors = 0;
			for (const auto& predictorslist : predictors)
			{
				std::vector<double> graphpred;
				int indexPeriode = period - 1;
				std::string outof = msclr::interop::marshal_as<std::string>(indexPeriode + ";" + indexPredictors + ";");
				for (const double& predval : predictorslist.back().getpredictors())
				{
					graphpred.push_back(predval);
					outof += std::to_string(predval);
					outof += ";";
				}

				++indexPredictors;
				outof.pop_back();
				RetourJson("allpredictionsnodes;" + gcnew System::String(outof.c_str()), gcnew System::EventArgs());
				periodpredictors.push_back(graphpred);
			}
			allpredictors.push_back(periodpredictors);
		}
		for (const std::string& value : allpredictornames)
		{
			RetourJson("allpredictornames;" + gcnew System::String(value.c_str()), gcnew System::EventArgs());
		}
	}
	catch (...)
	{
		Cache->getformhandler()->raisefromcatch("", "Wrapper::FMTForm::EcritureDesPredicteurs", __LINE__, __FILE__);
	}

}


bool Wrapper::FMTForm::SimulationSpatialeExplicite(
	System::String^ fichierPri, 
	System::String^ cheminRasters, 
	int scenario, 
	System::Collections::Generic::List<System::String^>^ contraintes, 
	int periodes, 
	int greedySearch, 
	System::Collections::Generic::List<System::String^>^ outputs, 
	bool indicateurStanlock,
	int outputLevel, 
	int etanduSortiesMin, 
	int etanduSortiesMax, 
	System::String^ cheminSorties, 
	bool indGenererEvents, 
	bool indSortiesSpatiales,
	System::String^ providerGdal, 
	bool indCarbon, 
	System::Collections::Generic::List<System::String^>^ predictoryields, 
	System::Collections::Generic::List<int>^ growththemes)
{
	try
	{
		Models::FMTsesmodel simulationmodel(Cache->getmodel(scenario));
		FMTFormLogger* logger = Cache->getformlogger();
		*logger << "FMT -> Traitement pour le scénario : " + simulationmodel.getname() << "\n";
		if (contraintes->Count > 0)
		{
			*logger << "FMT -> Intégration des contraintes sélectionnées" << "\n";
			simulationmodel.setconstraints(ObtenirArrayContraintesSelectionnees(ObtenirArrayContraintes(scenario), contraintes));
		}
		*logger << "FMT -> Modification et intégration des transitions" << "\n";
		std::vector<Core::FMTtransition> strans;
		for (const auto& tran : simulationmodel.gettransitions())
		{
			strans.push_back(tran.single());
		}
		*logger << "FMT -> Lecture des rasters" << "\n";
		simulationmodel.settransitions(strans);
		Parser::FMTareaparser areaparser;
		std::string rastpath = msclr::interop::marshal_as<std::string>(cheminRasters);
		const std::string agerast = rastpath + "AGE.tif";
		std::vector<std::string> themesrast;
		for (int i = 1; i <= simulationmodel.getthemes().size(); i++)
		{
			themesrast.push_back(rastpath + "THEME" + std::to_string(i) + ".tif");
		}
			{
				Spatial::FMTforest initialforestmap;
				if (!indicateurStanlock)
				{
					initialforestmap = areaparser.readrasters(simulationmodel.getthemes(), themesrast, agerast, 1, 0.0001);
				}else
				{
					initialforestmap = areaparser.readrasters(simulationmodel.getthemes(), themesrast, agerast, 1, 0.0001, rastpath + "STANLOCK.tif");
				}
				simulationmodel.setinitialmapping(initialforestmap);
			}

		const std::vector<Core::FMTschedule> schedules = ObtenirSEQ(fichierPri, scenario);

		if (schedules.back().getperiod() < periodes)
		{
			const std::string logout = "Dépassement de la période : size " + std::to_string(schedules.size()) + " periode " + std::to_string((schedules.back().getperiod() + 1));
			*logger << logout << "\n";
			return false;
		}
		simulationmodel.setparameter(Models::FMTintmodelparameters::LENGTH, periodes);
		simulationmodel.setparameter(Models::FMTintmodelparameters::NUMBER_OF_ITERATIONS, greedySearch);
		simulationmodel.setparameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, true);
		simulationmodel.setparameter(Models::FMTboolmodelparameters::POSTSOLVE, true);
		simulationmodel.doplanning(false, schedules);
		System::String^ directoryFullName;
		if (indCarbon)
		{
			directoryFullName = cheminRasters;
		}else {
			System::IO::DirectoryInfo^ parentdir = System::IO::Directory::GetParent(cheminSorties);
			directoryFullName = parentdir->FullName;
		}

		const Spatial::FMTspatialschedule& schedule = simulationmodel.getspschedule();
		RapportdeBris(simulationmodel);
		if (indCarbon) 
			{
			RapportdeCarboneSpatial(simulationmodel, periodes, schedules);
			}
		EcrituredesPerturbations(simulationmodel, directoryFullName, periodes, growththemes, indCarbon);
		if (indGenererEvents || indCarbon)
			{
			EcritureDesEvenements(simulationmodel, directoryFullName, periodes, indCarbon);
			}
		if (outputs->Count > 0)
		{
			const std::vector<Core::FMToutput> listeOutputs = EcritureDesOutputs(simulationmodel, outputs, periodes, indCarbon);
			Parser::FMTscheduleparser scheduparser;
			System::String^ schedulepath = System::IO::Path::Combine(directoryFullName, gcnew System::String(std::string(simulationmodel.getname() + "_.seq").c_str()));
			const std::string stdschedulepath = msclr::interop::marshal_as<std::string>(schedulepath);
			scheduparser.write(schedule.getschedules(simulationmodel.getactions()), stdschedulepath);
			if (!indCarbon)
			{
				Parser::FMTmodelparser Modelparser;
				*logger << "FMT -> Exportations des sorties " << "\n";
				Modelparser.writeresults(
					simulationmodel,
					listeOutputs,
					etanduSortiesMin,
					etanduSortiesMax,
					msclr::interop::marshal_as<std::string>(cheminSorties),
					static_cast<Core::FMToutputlevel>(outputLevel),
					msclr::interop::marshal_as<std::string>(providerGdal)
				);
			}
			else
			{				
				RetourJson("schedules;" + gcnew System::String(stdschedulepath.c_str()), gcnew System::EventArgs());
				areaparser.writeforest(schedule.getforestperiod(0), simulationmodel.getthemes(), themesrast, rastpath + "AGE.tif", rastpath + "STANLOCK.tif");
				if (predictoryields->Count > 0)
					{
					EcritureDesPredicteurs(simulationmodel, rastpath, periodes, predictoryields);
					}
			}
			if (indSortiesSpatiales)
			{
				EcrituredesOutputsSpatiaux(simulationmodel, listeOutputs, etanduSortiesMin, etanduSortiesMax, directoryFullName);
			}


		}
	}
	catch (...)
	{
		raisefromcatch("", "Wrapper::FMTForm::SimulationSpatialeExplicite", __LINE__, __FILE__);
		return false;
	}

	return true;
}