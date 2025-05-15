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
        static Models::FMTmodel aggregateAllActions(const Models::FMTmodel& p_model, const std::vector<std::string>& p_aggregates, const std::vector<std::string>& p_order,const std::string& p_primary_path, const std::string& p_scenario_name);

        static Models::FMTmodel splitActions(const Models::FMTmodel& p_model, const std::string& p_schedulePri, const std::vector<std::string>& p_splitted, const std::vector<std::string>& p_splitted_mask, const std::string& p_scenario_name);

        static Models::FMTmodel buildAction(const Models::FMTmodel& p_model, const std::string& p_actionName, const std::string& p_targetYield, const std::string& p_schedulePri, const std::string& p_scenario_name);

    };
}

