#ifndef FMTWRAPPERCORE_TRANSFORMATIONCORE_HEADER
#define FMTWRAPPERCORE_TRANSFORMATIONCORE_HEADER

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
    /**
     * @brief Transformations produisant un nouveau scénario à partir d'un modèle.
     *
     * Les méthodes prenant un Models::FMTModel retournent le modèle transformé
     * sans effet de bord : ce sont celles qu'utilisent les tests C++.
     *
     * Les méthodes suffixées FromCache résolvent le scénario dans FMTFormCache et
     * y ajoutent le modèle transformé. Ce sont celles qu'appelle le wrapper, ce
     * qui lui évite de manipuler le moindre objet FMT. Le suffixe les distingue
     * explicitement parce que, contrairement aux premières, elles modifient le cache.
     */
    class __declspec(dllexport) Transformation
    {
    public:
        static Models::FMTModel aggregateAllActions(const Models::FMTModel& p_model, const std::vector<std::string>& p_aggregates, const std::vector<std::string>& p_order,const std::string& p_primary_path, const std::string& p_scenario_name);

        static Models::FMTModel splitActions(const Models::FMTModel& p_model, const std::string& p_schedulePri, const std::vector<std::string>& p_splitted, const std::vector<std::string>& p_splitted_mask, const std::string& p_scenario_name);

        static Models::FMTModel buildAction(const Models::FMTModel& p_model, const std::string& p_actionName, const std::string& p_targetYield, const std::string& p_schedulePri, const std::string& p_scenario_name);

        /**
         * @brief Agrège toutes les actions d'un scénario du cache et ajoute le résultat au cache.
         * @param p_modelIndex Index du scénario dans FMTFormCache.
         * @param p_aggregates Agrégats à appliquer.
         * @param p_order Ordre des actions.
         * @param p_primary_path Chemin du fichier .pri.
         * @param p_scenario_name Nom du scénario produit.
         */
        static void aggregateAllActionsFromCache(
            int p_modelIndex,
            const std::vector<std::string>& p_aggregates,
            const std::vector<std::string>& p_order,
            const std::string& p_primary_path,
            const std::string& p_scenario_name);

        /**
         * @brief Sépare des actions d'un scénario du cache et ajoute le résultat au cache.
         * @param p_modelIndex Index du scénario dans FMTFormCache.
         * @param p_schedulePri Chemin du fichier .pri.
         * @param p_splitted Noms des actions à séparer.
         * @param p_splitted_mask Masques appliqués aux actions séparées.
         * @param p_scenario_name Nom du scénario produit.
         */
        static void splitActionsFromCache(
            int p_modelIndex,
            const std::string& p_schedulePri,
            const std::vector<std::string>& p_splitted,
            const std::vector<std::string>& p_splitted_mask,
            const std::string& p_scenario_name);

        /**
         * @brief Construit une action dans un scénario du cache et ajoute le résultat au cache.
         * @param p_modelIndex Index du scénario dans FMTFormCache.
         * @param p_actionName Nom de l'action à construire.
         * @param p_targetYield Yield cible.
         * @param p_schedulePri Chemin du fichier .pri.
         * @param p_scenario_name Nom du scénario produit.
         */
        static void buildActionFromCache(
            int p_modelIndex,
            const std::string& p_actionName,
            const std::string& p_targetYield,
            const std::string& p_schedulePri,
            const std::string& p_scenario_name);
    };
}

#endif // FMTWRAPPERCORE_TRANSFORMATIONCORE_HEADER
