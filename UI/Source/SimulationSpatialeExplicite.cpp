#include "stdafx.h"
#include <sstream>
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