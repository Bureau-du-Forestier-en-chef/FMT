#include <string>
#include <vector>

namespace Core { 
    class FMTSchedule;
    class FMTConstraint;
    class FMTOutput;
}

namespace Models
{
	class FMTModel;
}

namespace FMTWrapperCore
{
    class __declspec(dllexport) Transformation
    {
    public:
        static Models::FMTModel aggregateAllActions(const Models::FMTModel& p_model, const std::vector<std::string>& p_aggregates, const std::vector<std::string>& p_order,const std::string& p_primary_path, const std::string& p_scenario_name);

        static Models::FMTModel splitActions(const Models::FMTModel& p_model, const std::string& p_schedulePri, const std::vector<std::string>& p_splitted, const std::vector<std::string>& p_splitted_mask, const std::string& p_scenario_name);

        static Models::FMTModel buildAction(const Models::FMTModel& p_model, const std::string& p_actionName, const std::string& p_targetYield, const std::string& p_schedulePri, const std::string& p_scenario_name);

    };
}

