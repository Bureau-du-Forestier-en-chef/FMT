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

namespace FMTWrapperCore
{
    /**
     * @brief Transformations produisant un nouveau scénario à partir d'un modèle.
     *
     * Chaque méthode écrit le scénario transformé et ses cédules dans le projet, puis
     * retourne le modèle transformé ; les tests C++ l'appellent directement. Le wrapper
     * y accède par Controller, qui ajoute ce modèle au cache de l'interface.
     */
    class FMTWRAPPERCOREEXPORT Transformation
    {
    public:
        static Models::FMTModel aggregateAllActions(const Models::FMTModel& p_model, const std::vector<std::string>& p_aggregates, const std::vector<std::string>& p_order,const std::string& p_primary_path, const std::string& p_scenario_name);

        static Models::FMTModel splitActions(const Models::FMTModel& p_model, const std::string& p_schedulePri, const std::vector<std::string>& p_splitted, const std::vector<std::string>& p_splitted_mask, const std::string& p_scenario_name);

        static Models::FMTModel buildAction(const Models::FMTModel& p_model, const std::string& p_actionName, const std::string& p_targetYield, const std::string& p_schedulePri, const std::string& p_scenario_name);
    };
}

#endif // FMTWRAPPERCORE_TRANSFORMATIONCORE_HEADER
