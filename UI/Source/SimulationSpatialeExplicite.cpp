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
#include "SES.h"

void Wrapper::FMTForm::RapportdeCarboneSpatial(
    const Models::FMTsemodel& semodel, 
    const int& nombredeperiodes, 
    const std::vector<Core::FMTschedule>& schedules)
{
    try
    {
    	SES::spatialCarbonReport(
            semodel,
            nombredeperiodes,
            schedules,
            [this](const std::string& msg)
            {
                auto managedMsg = gcnew System::String(msg.c_str());
                this->RetourJson(managedMsg, gcnew System::EventArgs());
            }
        );
    }
    catch (...)
    {
		// TODO;
    }
}

void Wrapper::FMTForm::EcrituredesPerturbations(
	const Models::FMTsemodel& semodel, 
	System::String^ cheminsorties,
	const int& nombredeperiodes, 
	System::Collections::Generic::List<int>^ growththemes, 
	const bool& incarbon)
{
	try {
		const std::string rastpath = msclr::interop::marshal_as<std::string>(cheminsorties);
		std::vector<Core::FMTtheme> growththeme;
		if (growththemes->Count != 0)
		{
			for each (int themeID in growththemes)
			{
				growththeme.push_back(semodel.getthemes().at(themeID - 1));
			}
		}

		FMTWrapperCore::SES::writeDisturbance(
			semodel,
			rastpath,
			nombredeperiodes,
			growththeme,
			incarbon,
			[this](const std::string& msg) {
				System::String^ managed = gcnew System::String(msg.c_str());
				this->RetourJson(managed, gcnew System::EventArgs());
			});
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
			*logger << "�criture des �v�nements ici: " + msclr::interop::marshal_as<std::string>(eventpath) << "\n";
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
		*logger << "FMT -> �criture des outputs spatiaux" << "\n";
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

		*logger << "FMT -> �criture des outputs spatiaux termin�e" << "\n";
		
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
		auto toStdStr = [](System::String^ s) {
			return msclr::interop::marshal_as<std::string>(s);
		};
		std::vector<std::string> nativeContraintes;
		for each (auto^ c in contraintes) nativeContraintes.push_back(toStdStr(c));
		std::vector<std::string> nativeOutputs;
		for each (auto^ o in outputs) nativeOutputs.push_back(toStdStr(o));
		std::vector<std::string> nativePredictors;
		for each (auto^ p in predictoryields) nativePredictors.push_back(toStdStr(p));
		std::vector<int> nativeGrowth;
		for each (auto i in growththemes) nativeGrowth.push_back(i);

		Models::FMTsesmodel simulationmodel(Cache->getmodel(scenario));

        bool ok = FMTWrapperCore::SES::spatiallyExplicitSimulation(
			simulationmodel,
			toStdStr(fichierPri),
			toStdStr(cheminRasters),
			scenario,
			nativeContraintes,
			periodes,
			greedySearch,
			nativeOutputs,
			indicateurStanlock,
			outputLevel,
			etanduSortiesMin,
			etanduSortiesMax,
			toStdStr(cheminSorties),
			indGenererEvents,
			indSortiesSpatiales,
			toStdStr(providerGdal),
			indCarbon,
			nativePredictors,
			nativeGrowth,

			[this](const std::string& msg)
			{
				auto managed = gcnew System::String(msg.c_str());
				this->RetourJson(managed, gcnew System::EventArgs());
			}
        );
        return ok;
    }
    catch (...)
    {
        raisefromcatch("", "Wrapper::FMTForm::SimulationSpatialeExplicite", __LINE__, __FILE__);
        return false;
    }
}