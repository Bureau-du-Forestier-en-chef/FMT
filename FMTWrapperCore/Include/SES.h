#include "FMTmodel.h"
#include <vector>
#include <string>

namespace Core { 
	class FMTschedule;
	class FMTconstraint;
	class FMToutput;
}


namespace Models {
	class FMTsemodel;
}

namespace FMTWrapperCore {

    class __declspec(dllexport) SES
    {
    public:
        bool spatiallyExplicitSimulation(
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
            std::function<void(const std::string&)> report
        );
    private:
        void spatialCarbonReport(
            const Models::FMTsemodel& semodel,
            const int& nombredeperiodes,
            const std::vector<Core::FMTschedule>& schedules,
            std::function<void(const std::string&)> report
        );

        void writeDisturbance(
            const Models::FMTsemodel& semodel,
            const std::string& rastersPath,
            int nombredeperiodes,
            const std::vector<int>& growthThemes,
            bool incarbon,
            std::function<void(const std::string&)> report
        );

        void writeEvents(
            const Models::FMTsemodel& semodel,
            const std::string& cheminsorties,
            int nombredeperiodes,
            bool incarbon,
            std::function<void(const std::string&)> report
        );

        std::vector<Core::FMToutput> EcritureDesOutputs(
            const Models::FMTsemodel& semodel,
            const std::vector<std::string>& outputs,
            int nombredeperiodes,
            bool incarbon
        );

        void EcrituredesOutputsSpatiaux(
            const Models::FMTsemodel& semodel,
            const std::vector<Core::FMToutput>& outputs,
            int sortiemin,
            int sortiemax,
            const std::string& localisation
        );

        void EcritureDesPredicteurs(
            const Models::FMTsemodel& semodel,
            const std::string& rastpath,
            int periodes,
            const std::vector<std::string>& predictoryields
        );
    }
}