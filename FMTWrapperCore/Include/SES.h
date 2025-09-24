#include "FMTmodel.h"
#include "FMTsesmodel.h"
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
        static bool spatiallyExplicitSimulation(
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
            int output_min,
            int output_max,
            const std::string& cheminSorties,
            bool indGenererEvents,
            bool indSortiesSpatiales,
            const std::string& providerGdal,
            bool indCarbon,
            const std::vector<std::string>& predictoryields,
            const std::vector<int>& growththemes
        );
    private:
        void spatialCarbonReport(
            const Models::FMTsemodel& semodel,
            const int& nombredeperiodes,
            const std::vector<Core::FMTschedule>& schedules
        );

        void writeDisturbance(
            const Models::FMTsemodel& semodel,
            const std::string& rastersPath,
            const int& nombredeperiodes,
            const std::vector<int>& growthThemes,
            const bool& incarbon
        );

        void writeEvents(
            const Models::FMTsemodel& semodel,
            const std::string& cheminsorties,
            const bool incarbon
        );

        std::vector<Core::FMToutput> writeOutputs(
            const Models::FMTsemodel& semodel,
            const std::vector<std::string>& outputs,
            const int& nombredeperiodes,
            const bool& incarbon
        );

        void writePredictors(
            const Models::FMTsemodel& semodel,
            const std::string& rastpath,
            const int& periodes,
            const std::vector<std::string>& predictoryields
        );

        void writeSpatialOutputs(
            const Models::FMTsemodel& semodel,
            const std::vector<Core::FMToutput>& outputs,
            const int& sortiemin,
            const int& sortiemax,
            std::string& localisation
        );
    };
}