#ifndef FMTWRAPPERCORE_CONTROLLER_HEADER
#define FMTWRAPPERCORE_CONTROLLER_HEADER

#include <set>
#include <string>
#include <vector>

#include "AreaVariabilityTypes.h"
#include "FMTWrapperCoreExport.h"
#include "OperatingAreaTypes.h"
#include "PlanningTypes.h"
#include "RasterizationTypes.h"
#include "SESTypes.h"

namespace FMTWrapperCore
{
    /**
     * @brief Contrôleur façade (GRASP) : le seul point d'entrée du wrapper dans le Core.
     *
     * Chaque méthode correspond à une opération système de FMTForm. Elle reçoit des types
     * std, des DTO et des index de scénario, résout le scénario dans FMTFormCache, puis
     * délègue à l'entrée pure d'un service : Environment, ModelQuery, Transformation, SES,
     * Rasterization, OperatingArea, AreaVariability ou Planning. Le contrôleur coordonne
     * sans calculer : la logique reste dans les services, que les tests appellent
     * directement, sans passer par le cache.
     *
     * Aucun type de FMTlib n'apparaît dans cette interface : le wrapper n'inclut que ce
     * header, et ne peut donc manipuler aucun objet FMT.
     *
     * Le contrôleur n'a pas d'état. L'état de la session (scénarios chargés, logger et
     * gestionnaire d'exceptions de l'interface) est dans FMTFormCache.
     *
     * Toute opération qui prend un index de scénario lève une exception FMTrangeerror si
     * le cache est vide, sauf getOutputsNames, qui retourne alors une liste vide.
     */
    class FMTWRAPPERCOREEXPORT Controller
    {
    public:
        // Session : journal, erreurs et scénarios chargés.

        /**
         * @brief Installe le logger de l'interface.
         * @param p_logFilePath Fichier du journal.
         * @param p_callback Pointeur de fonction natif du délégué managé qui reçoit chaque
         *        message.
         */
        static void initializeLogger(const std::string& p_logFilePath, void* p_callback);

        /**
         * @brief Reconstruit le logger et le gestionnaire d'exceptions après un plantage,
         *        avec la configuration conservée par FMTFormCache.
         * @param p_callback Nouveau pointeur de fonction du délégué managé.
         */
        static void recoverLoggerAndHandler(void* p_callback);

        /**
         * @brief Ferme le fichier du journal.
         */
        static void closeLogger();

        /**
         * @brief Installe le gestionnaire d'exceptions de l'interface.
         * @param p_exceptionIds Exceptions à traiter comme des avertissements, valeurs de
         *        Exception::FMTexc.
         * @param p_maxWarnings Nombre d'avertissements avant qu'ils ne soient tus ; 10 si la
         *        valeur n'est pas positive.
         */
        static void setErrorsToWarnings(
            const std::vector<int>& p_exceptionIds,
            int p_maxWarnings);

        /**
         * @brief Journalise l'exception en cours de traitement.
         *
         * À appeler depuis un bloc catch : la pile d'erreurs est reconstruite à partir de
         * l'exception courante. Elle est écrite dans le journal, ou directement dans son
         * fichier si le logger de l'interface n'est pas installé.
         *
         * @param p_text Message à ajouter à la pile.
         * @param p_method Méthode où l'exception a été attrapée.
         * @param p_line Ligne où l'exception a été attrapée.
         * @param p_file Fichier où l'exception a été attrapée.
         * @return La pile d'erreurs formatée, à renvoyer à l'interface.
         */
        static std::string logCurrentException(
            const std::string& p_text,
            const std::string& p_method,
            int p_line,
            const std::string& p_file);

        /**
         * @brief Ouvre dans Notepad++ le fichier et la ligne cités par une pile d'erreurs,
         *        s'il y en a.
         * @param p_errorStack Pile retournée par logCurrentException.
         */
        static void openErrorLocation(const std::string& p_errorStack);

        /**
         * @brief Lit des scénarios d'un projet et les ajoute au cache.
         * @param p_primaryFilePath Fichier .pri du projet.
         * @param p_scenarioNames Noms des scénarios à lire.
         */
        static void addScenarios(
            const std::string& p_primaryFilePath,
            const std::vector<std::string>& p_scenarioNames);

        /**
         * @brief Retire un scénario du cache.
         * @param p_modelIndex Index du scénario.
         */
        static void removeScenario(int p_modelIndex);

        /**
         * @brief Retire tous les scénarios du cache.
         */
        static void clearScenarios();

        /**
         * @brief Écrit tous les scénarios du cache dans un projet ; le premier sert de base.
         * @param p_destinationDirectory Dossier de destination.
         */
        static void writeScenariosToProject(const std::string& p_destinationDirectory);

        // Environnement, sans scénario : voir Environment.

        /** @brief Voir Environment::getAvailableSolvers. */
        static std::vector<int> getAvailableSolvers();

        /** @brief Voir Environment::getSolverName. */
        static std::string getSolverName(int p_solver);

        /** @brief Voir Environment::getVectorDriverExtensions. */
        static std::vector<std::string> getVectorDriverExtensions();

        /** @brief Voir Environment::getChangeLog. */
        static std::string getChangeLog();

        /** @brief Voir Environment::getExceptionDescription. */
        static std::string getExceptionDescription(int p_exceptionId);

        /** @brief Voir Environment::getErrorsToIgnore. */
        static std::vector<int> getErrorsToIgnore();

        // Interrogation d'un scénario du cache, désigné par son index : voir ModelQuery.

        /** @brief Voir ModelQuery::getMaxAge. */
        static int getMaxAge(int p_modelIndex);

        /** @brief Voir ModelQuery::getYield. */
        static double getYield(
            int p_modelIndex,
            const std::string& p_mask,
            const std::string& p_yield,
            int p_age);

        /** @brief Voir ModelQuery::getAllMasks. */
        static std::set<std::string> getAllMasks(
            int p_modelIndex,
            int p_periods,
            const std::vector<int>& p_themesNumbers,
            const std::string& p_rasterPath);

        /** @brief Voir ModelQuery::validateMask. */
        static bool validateMask(int p_modelIndex, const std::string& p_mask);

        /** @brief Voir ModelQuery::getConstraintsAsText. */
        static std::vector<std::string> getConstraintsAsText(int p_modelIndex);

        /**
         * @brief Voir ModelQuery::getOutputsNames.
         *
         * Retourne une liste vide si le cache est vide : l'interface peuple sa liste
         * d'outputs avant qu'un scénario soit chargé.
         */
        static std::vector<std::string> getOutputsNames(int p_modelIndex);

        /** @brief Voir ModelQuery::getActionsNames. */
        static std::vector<std::string> getActionsNames(int p_modelIndex);

        /** @brief Voir ModelQuery::getAggregates. */
        static std::vector<std::string> getAggregates(int p_modelIndex);

        /** @brief Voir ModelQuery::getYieldsNames. */
        static std::vector<std::string> getYieldsNames(int p_modelIndex);

        /** @brief Voir ModelQuery::getThemesCount. */
        static int getThemesCount(int p_modelIndex);

        /** @brief Voir ModelQuery::getThemeAttributes. */
        static std::vector<std::string> getThemeAttributes(int p_modelIndex, int p_themeIndex);

        /** @brief Voir ModelQuery::getPeriodsCount. */
        static int getPeriodsCount(const std::string& p_primaryFilePath, int p_modelIndex);

        // Transformations : voir Transformation. Le scénario produit est ajouté au cache.

        /** @brief Voir Transformation::aggregateAllActions. */
        static void aggregateAllActions(
            int p_modelIndex,
            const std::vector<std::string>& p_aggregates,
            const std::vector<std::string>& p_order,
            const std::string& p_primaryFilePath,
            const std::string& p_scenarioName);

        /** @brief Voir Transformation::splitActions. */
        static void splitActions(
            int p_modelIndex,
            const std::string& p_primaryFilePath,
            const std::vector<std::string>& p_splitted,
            const std::vector<std::string>& p_splittedMasks,
            const std::string& p_scenarioName);

        /** @brief Voir Transformation::buildAction. */
        static void buildAction(
            int p_modelIndex,
            const std::string& p_actionName,
            const std::string& p_targetYield,
            const std::string& p_primaryFilePath,
            const std::string& p_scenarioName);

        // Traitements sur un scénario du cache.

        /** @brief Voir SES::RunSES(const SESParameters&, const Models::FMTModel&). */
        static SESResults runSpatialSimulation(const SESParameters& p_params, int p_modelIndex);

        /** @brief Voir SES::RunOptimization. */
        static SAResults runSpatialOptimization(const SAParameters& p_params, int p_modelIndex);

        /** @brief Voir Rasterization::rasterize. */
        static void rasterize(const RasterizationParameters& p_params, int p_modelIndex);

        /** @brief Voir OperatingArea::schedule. */
        static OperatingAreaResults scheduleOperatingAreas(
            const OperatingAreaParameters& p_params,
            int p_modelIndex);

        /** @brief Voir AreaVariability::run. */
        static AreaVariabilityResults runAreaVariability(
            const AreaVariabilityParameters& p_params,
            int p_modelIndex);

        /**
         * @brief Voir Planning::plan.
         *
         * Une relecture de cédule en échec est journalisée comme toute erreur de
         * l'interface (logCurrentException, openErrorLocation), et la planification continue.
         *
         * @param p_params Paramètres de la planification.
         * @param p_modelIndexes Index des scénarios à planifier.
         * @param p_playback Drapeau de relecture de chaque scénario, par position.
         */
        static void plan(
            const PlanningParameters& p_params,
            const std::vector<int>& p_modelIndexes,
            const std::vector<bool>& p_playback);

        /**
         * @brief Voir Planning::replan.
         * @param p_params Paramètres de la replanification.
         * @param p_strategicModelIndex Index du scénario stratégique (global).
         * @param p_stochasticModelIndex Index du scénario stochastique.
         * @param p_tacticalModelIndex Index du scénario tactique (local).
         */
        static void replan(
            const ReplanningParameters& p_params,
            int p_strategicModelIndex,
            int p_stochasticModelIndex,
            int p_tacticalModelIndex);
    };
}

#endif // FMTWRAPPERCORE_CONTROLLER_HEADER
