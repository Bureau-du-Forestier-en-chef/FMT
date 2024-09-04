#include <string>
#include <vector>

namespace Core { 
    class FMTschedule;
    class FMTconstraint;
    class FMToutput;
}

namespace Models
{
	class FMTmodel;
}

namespace FMTWrapperCore
{
    class __declspec(dllexport) Transformation
    {
    public:
        static Models::FMTmodel aggregateAllActions(const Models::FMTmodel& p_model, const std::vector<std::string>& p_aggregates, const std::vector<std::string>& p_order,const std::string& p_schedulePri, const std::string& p_outputDirPath, const std::string& p_scenario_name);

        //static bool splitActions(Models::FMTmodel p_model, const std::string& p_schedulePri, const std::vector<std::string>& p_splitted, const std::vector<std::string>& p_splitted_mask, const std::string& p_outputDirPath, const std::string& p_scenario_name);

        //std::vector<std::string> getActionsNames(int p_modelIndex);

        //std::vector<std::string> getAggregates(int p_modelIndex);

        //std::vector<std::string> getYields(int p_modelIndex);

        //double getYield(int p_modelIndex, const std::string& p_mask, const std::string& p_yield, int p_age);

        //double getMaxAge(int p_modelIndex);

        static Models::FMTmodel buildAction(const Models::FMTmodel& p_model, const std::string& p_actionName, const std::string& p_targetYield, const std::string& p_schedulePri, const std::string& p_outputDirPath, const std::string& p_scenario_name);

        //std::vector<std::string> getAttributes(int p_modelIndex, int p_themeIndex);

       //bool validateMask(int p_modelIndex, const std::string& p_mask);
    };
}

