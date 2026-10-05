#ifndef FMTWRAPPERCORE_TRANSFORMATIONCORE_HEADER
#define FMTWRAPPERCORE_TRANSFORMATIONCORE_HEADER

#include <string>
#include <vector>

#include "FMTWrapperCoreExport.h"

namespace Core {
    class FMTSchedule;
    class FMTConstraint;
    class FMTOutput;
}

namespace Models
{
	class FMTModel;
}

namespace FMTWrapper::Backend
{
    /**
     * @brief Transformations producing a new scenario from a model.
     *
     * Each method writes the transformed scenario and its schedules to the project, then
     * returns the transformed model; the C++ tests call them directly. The wrapper reaches
     * them through Controller, which adds that model to the interface cache.
     */
    class FMT_WRAPPER_CORE_EXPORT Transformation
    {
    public:
        static Models::FMTModel aggregateAllActions(const Models::FMTModel& p_model, const std::vector<std::string>& p_aggregates, const std::vector<std::string>& p_order,const std::string& p_primary_path, const std::string& p_scenario_name);

        static Models::FMTModel splitActions(const Models::FMTModel& p_model, const std::string& p_schedulePri, const std::vector<std::string>& p_splitted, const std::vector<std::string>& p_splitted_mask, const std::string& p_scenario_name);

        static Models::FMTModel buildAction(const Models::FMTModel& p_model, const std::string& p_actionName, const std::string& p_targetYield, const std::string& p_schedulePri, const std::string& p_scenario_name);
    };
}

#endif // FMTWRAPPERCORE_TRANSFORMATIONCORE_HEADER
