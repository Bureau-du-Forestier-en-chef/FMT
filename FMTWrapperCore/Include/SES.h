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
            const int& nombredeperiodes,
            const std::vector<Core::FMTtheme>& growthThemes,
            const bool& incarbon,
            std::function<void(const std::string&)> report
        );

        void writeEvents(
            const Models::FMTsemodel& semodel,
            const std::string& cheminsorties,
            const bool incarbon,
            std::function<void(const std::string&)> report
        );

        std::vector<Core::FMToutput> writeOutputs(
            const Models::FMTsemodel& semodel,
            const std::vector<std::string>& outputs,
            const int& nombredeperiodes,
            const bool& incarbon,
            std::function<void(const std::string&)> report
        );

        void writePredictors(
            const Models::FMTsemodel& semodel,
            const std::string& rastpath,
            const int& periodes,
            const std::vector<std::string>& predictoryields,
            std::function<void(const std::string&)> report
        );

        void writeSpatialOutputs(
            const Models::FMTsemodel& semodel,
            const std::vector<Core::FMToutput>& outputs,
            const int& sortiemin,
            const int& sortiemax,
            std::string& localisation,
            std::function<void(const std::string&)> report
        );
    };
}