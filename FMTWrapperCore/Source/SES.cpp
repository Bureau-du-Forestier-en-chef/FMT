#include "FMTsesmodel.h"
#include "SES.h"
#include "FMTmodelparser.h"
#include "FMTscheduleparser.h"
#include "FMTtransitionparser.h"
#include "FMTareaparser.h"
#include "FMTexceptionhandler.h"
#include "FMTmodel.h"
#include "Tools.h"
#include "FMTforest.h"
#include <boost/filesystem.hpp>

void FMTWrapperCore::SES::spatialCarbonReport(
	const Models::FMTsemodel& semodel,
	const int& nombredeperiodes,
	const std::vector<Core::FMTschedule>& schedules,
	std::function<void(const std::string&)> report)
{
	Models::FMTmodel localmodel(semodel);
	const auto& spatialSchedule = semodel.getspschedule();
	const auto newschedule = spatialSchedule.getschedules(semodel.getactions(), false);

	size_t scid = 0;
	for (int period = 1; period <= nombredeperiodes; ++period)
	{
		int jsonloc = period - 1;
		auto periodicconstraints = semodel.getconstraints();
		for (auto& c : periodicconstraints)
		{
			int l = c.getperiodlowerbound();
			int u = std::min(period, c.getperiodupperbound());
			c.setlength(l, u);
		}
		localmodel.setconstraints(periodicconstraints);

		double primalinf = 0, objectivevalue = 0;
		spatialSchedule.getsolutionstatus(objectivevalue, primalinf, localmodel, nullptr, true, false);

		report("objectives;" + std::to_string(jsonloc) + ";Objective;" + std::to_string(objectivevalue));
		report("objectives;" + std::to_string(jsonloc) + ";Primalinfeasibility;" + std::to_string(primalinf));

		double oldtotal = 0, newtotal = 0;
		size_t oriloc = 0, newloc = 0;
		for (const auto& s : schedules)
		{
			if (s.getperiod() == period) break;
			++oriloc;
		}
		for (const auto& s : newschedule)
		{
			if (s.getperiod() == period) break;
			++newloc;
		}

		if (scid < newschedule.size() && scid < schedules.size())
		{
			for (const auto& data : schedules.at(oriloc))
			{
				double basearea = schedules.at(oriloc).actionarea(data.first);
				double newarea = 0;
				auto it = newschedule.at(newloc).find(data.first);
				if (it != newschedule.at(newloc).end())
					newarea = newschedule.at(newloc).actionarea(data.first);

				oldtotal += basearea;
				newtotal += newarea;

				report(
					"objectives;" +
					std::to_string(jsonloc) + ";" +
					data.first.getname() + ";" +
					std::to_string(newarea / basearea)
				);
			}
			report("objectives;" + std::to_string(jsonloc) + ";Total;" + std::to_string(newtotal / oldtotal));
		}
		++scid;
	}
}

void FMTWrapperCore::SES::writeDisturbance(
	const Models::FMTsemodel& semodel, 
	const std::string& rastersPath,
	const int& nombredeperiodes, 
	const std::vector<Core::FMTtheme>& growthThemes, 
	const bool& incarbon,
	std::function<void(const std::string&)> report)
{
	const Spatial::FMTspatialschedule& schedule = semodel.getspschedule();
	report("FMT -> Écriture des perturbations");
	Parser::FMTtransitionparser transitionparser;
	Parser::FMTareaparser areaparser;

	for (int period = 1; period <= nombredeperiodes; ++period)
	{
		//const Spatial::FMTspatialschedule spatialsolution = simulationmodel.getspschedule();
		const std::vector<Core::FMTaction> actions = semodel.getactions();
		const std::vector<Core::FMTGCBMtransition> transitions = areaparser.writedisturbances(
			rastersPath,
			schedule,
			actions,
			growthThemes, 
			period);

		std::string fichier = rastersPath + "transition" + std::to_string(period) + ".txt";
		transitionparser.writeGCBM(transitions, fichier);
		if (incarbon){
			report("GCBMtransitionlocations;" + fichier);
		}
	}
}

void FMTWrapperCore::SES::writeEvents(
	const Models::FMTsemodel& semodel,
	const std::string& cheminsorties, 
	const bool incarbon,
	std::function<void(const std::string&)> report)
{
	const Spatial::FMTspatialschedule& schedule = semodel.getspschedule();
	const std::vector<Core::FMTaction> actions = semodel.getactions();
	const std::string stats = schedule.getpatchstats(actions);
	
	boost::filesystem::path path = cheminsorties;
	path /= "events.txt";
	const std::string eventpath = path.string();
	report("Écriture des évènements ici: " + eventpath);

	std::ofstream ofs;
	ofs.open(path.string(), std::ios::trunc);
	ofs << stats << '\n';

	if (incarbon)
	{
		report("eventslocation;" + eventpath);
	}
}

std::vector<Core::FMToutput> FMTWrapperCore::SES::writeOutputs(
	const Models::FMTsemodel& semodel,
	const std::vector<std::string>& outputs,
	const int& nombredeperiodes,
	const bool& incarbon,
	std::function<void(const std::string&)> report)
{
	std::vector<Core::FMToutput> listeOutputs;
	std::vector<Core::FMToutput> listeOutputs;
	for (const Core::FMToutput& fmtOutput : semodel.getoutputs())
	{
		const std::string name = fmtOutput.getname();
		if (std::find(outputs.begin(), outputs.end(), name) != outputs.end())
		{
			listeOutputs.push_back(fmtOutput);
		}
	}

	if (incarbon)
	{
		const Spatial::FMTspatialschedule& schedule = semodel.getspschedule();
		for (const Core::FMToutput& output : listeOutputs)
		{
			std::map<std::string, std::vector<double>> resultatOutputsCarbon = schedule.getoutput(semodel, output, 1, nombredeperiodes);

			for (double valeurCarbon : resultatOutputsCarbon["Total"])
			{
				report("outputs;" + output.getname() + ";" + std::to_string(valeurCarbon));
			}
		}
	}
	return listeOutputs;
}

void FMTWrapperCore::SES::writePredictors(
	const Models::FMTsemodel& semodel,
	const std::string& rastpath,
	const int& periodes,
	const std::vector<std::string>& predictoryields,
	std::function<void(const std::string&)> report)
{
	Parser::FMTareaparser areaparser;
	const Spatial::FMTspatialschedule& schedule = semodel.getspschedule();
	std::vector<std::vector<std::vector<double>>> allpredictors;
	std::vector<std::string> allpredictornames;

	for (size_t period = 1; period <= periodes; ++period)
	{
		std::vector<std::vector<Graph::FMTpredictor>> predictors = areaparser.writepredictors(
			rastpath, 
			schedule, 
			predictoryields,
			semodel, 
			period);
		std::vector<std::vector<double>> periodpredictors;
		allpredictornames = predictors.back().back().getpredictornames(predictoryields);
		size_t indexPredictors = 0;

		for (const auto& predictorslist : predictors)
		{
			std::vector<double> graphpred;
			int indexPeriode = period - 1;
			std::string outof = std::to_string(indexPeriode) + ";" + std::to_string(indexPredictors) + ";";
			for (const double& predval : predictorslist.back().getpredictors())
			{
				graphpred.push_back(predval);
				outof += std::to_string(predval);
				outof += ";";
			}

			++indexPredictors;
			outof.pop_back();
			report("allpredictionsnodes;" + outof);
			periodpredictors.push_back(graphpred);
		}
		allpredictors.push_back(periodpredictors);
	}
	for (const std::string& value : allpredictornames)
	{
		report("allpredictornames;" + value);
	}
}

void FMTWrapperCore::SES::writeSpatialOutputs(
	const Models::FMTsemodel& semodel,
	const std::vector<Core::FMToutput>& outputs,
	const int& sortiemin, 
	const int& sortiemax,
	std::string& localisation,
	std::function<void(const std::string&)> report)
	{
	Parser::FMTareaparser areaparser;
	report("FMT -> Écriture des outputs spatiaux");
	for (int period = sortiemin; period <= sortiemax; ++period)
	{
		for (const Core::FMToutput& output : outputs)
		{
			const std::string outputname = output.getname() + "_" + std::to_string(period) + ".tif";
			boost::filesystem::path outputpath = boost::filesystem::path(localisation) / outputname;
			const std::string stdoutputpath = outputpath.string();
			const Spatial::FMTlayer<double> outputlayer = semodel.getspatialoutput(output, period);
			areaparser.writelayer(outputlayer, stdoutputpath);
		}
	}

	report("FMT -> Écriture des outputs spatiaux terminée");
}

bool FMTWrapperCore::SES::spatiallyExplicitSimulation(
	Models::FMTsesmodel p_sesModel, 
	const std::string& p_priFilePath,
	const std::string& p_rastersPath, 
	int p_scenario, 
	const std::vector<std::string>& p_constraints, 
	int p_length, 
	int p_greedySearch, 
	const std::vector<std::string>& p_outputs, 
	bool p_stanlock, 
	int output_level, 
	int etanduSortiesMin, 
	int etanduSortiesMax, 
	const std::string& cheminSorties,
	bool indGenererEvents,
	bool indSortiesSpatiales, 
	const std::string& providerGdal, 
	bool indCarbon, 
	const std::vector<std::string>& predictoryields, 
	const std::vector<int>& growththemes, 
	std::function<void(const std::string&)> report)
{
	try
	{
		// Modèle
		Models::FMTsemodel simulationmodel = p_sesModel;
		report("FMT -> Traitement pour le scénario : " + simulationmodel.getname());
		
		// Contraintes
		if (!p_constraints.empty())
		{
			report("FMT -> Intégration des contraintes sélectionnées");
			simulationmodel.setconstraints(
				FMTWrapperCore::Tools::getSelectedConstraints(simulationmodel.getconstraints(), p_constraints));
		}
		
		// Transition
		std::vector<Core::FMTtransition> strans;
		report("FMT -> Modification et intégration des transitions");
		for (const auto& tran : simulationmodel.gettransitions())
		{
			strans.push_back(tran.single());
		}
		simulationmodel.settransitions(strans);

		// Rasters
		report("FMT -> Lecture des rasters");
		Parser::FMTareaparser areaparser;
		const std::string agerast = p_rastersPath + "AGE.tif";
		std::vector<std::string> themesrast;
		for (size_t i = 1; i <= simulationmodel.getthemes().size(); i++)
		{
			themesrast.push_back(p_rastersPath + "THEME" + std::to_string(i) + ".tif");
		}	
		Spatial::FMTforest initialforestmap;
		if (!p_stanlock)
		{
			initialforestmap = areaparser.readrasters(simulationmodel.getthemes(), themesrast, agerast, 1, 0.0001);
		}
		else
		{
			initialforestmap = areaparser.readrasters(simulationmodel.getthemes(), themesrast, agerast, 1, 0.0001, p_rastersPath + "STANLOCK.tif");
		}
		simulationmodel.setinitialmapping(initialforestmap);
	
		// Squedules
		const std::vector<Core::FMTschedule> schedules = Tools::getSchedule(p_priFilePath, simulationmodel); // différent de tommy, idk if it works
		if (schedules.back().getperiod() < p_length)
		{
			const std::string logout = "Dépassement de la période : size " + std::to_string(schedules.size()) + " periode " + std::to_string((schedules.back().getperiod() + 1));
			report(logout);
			return false;
		}

		// Parameters
		simulationmodel.setparameter(Models::FMTintmodelparameters::LENGTH, p_length);
		simulationmodel.setparameter(Models::FMTintmodelparameters::NUMBER_OF_ITERATIONS, p_greedySearch);
		simulationmodel.setparameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, true);
		simulationmodel.setparameter(Models::FMTboolmodelparameters::POSTSOLVE, true);
		
		// Simulation
		simulationmodel.doplanning(false, schedules);
		std::string directoryFullName;
		if (indCarbon)
		{
			directoryFullName = p_rastersPath;
		} 
		else 
		{
			directoryFullName = std::filesystem::path(cheminSorties).parent_path().string();
		}

		const Spatial::FMTspatialschedule& schedule = simulationmodel.getspschedule();
		// FMTForm::RapportdeBris(simulationmodel); // Ne sert à rien, est dans le header mais nul part ailleur

		if (indCarbon) 
		{
			spatialCarbonReport(simulationmodel, p_length, schedules);
		}

		writeDisturbance(simulationmodel, directoryFullName, p_length, growththemes, indCarbon);
		
		if (indGenererEvents || indCarbon)
		{
			writeEvents(simulationmodel, directoryFullName, p_length, indCarbon);
		}
		
		if (!p_outputs.empty())
		{
			const std::vector<Core::FMToutput> listeOutputs = Tools::writeOutputs(simulationmodel, p_outputs, p_length, indCarbon);
			Parser::FMTscheduleparser scheduparser;
			std::string schedulepath = boost::filesystem::path(directoryFullName) / simulationmodel.getname() + "_.seq";
			const std::string stdschedulepath = schedulepath.to_string();
			scheduparser.write(schedule.getschedules(simulationmodel.getactions()), stdschedulepath);
			if (!indCarbon)
			{
				Parser::FMTmodelparser Modelparser;
				report("FMT -> Exportations des sorties");
				Modelparser.writeresults(
					simulationmodel,
					p_outputs,
					output_min,
					output_max,
					cheminSorties,
					static_cast<Core::FMToutputlevel>(output_level),
					providerGdal);
			}
			else
			{				
				report("schedules;" + stdschedulepath);
				areaparser.writeforest(
					schedule.getforestperiod(0), 
					simulationmodel.getthemes(), 
					themesrast, 
					p_rastersPath + "AGE.tif", 
					p_rastersPath + "STANLOCK.tif");
				if (!predictoryields.empty())
					{
					Tools::writePredictors(simulationmodel, p_rastersPath, p_length, predictoryields);
					}
			}
			if (indSortiesSpatiales)
			{
				writeSpatialOutputs(simulationmodel, p_outputs, output_min, output_max, directoryFullName);
			}
	}
	catch (...)
	{
		return false;
	}

	return true;
}