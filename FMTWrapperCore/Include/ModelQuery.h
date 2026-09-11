#ifndef FMTWRAPPERCORE_MODELQUERY_HEADER
#define FMTWRAPPERCORE_MODELQUERY_HEADER

#include <set>
#include <string>
#include <vector>

namespace Core
{
    class FMTActualDevelopment;
    class FMTMask;
    class FMTSchedule;
    class FMTTheme;
}

namespace Models
{
    class FMTModel;
}

namespace FMTWrapperCore
{
    /**
     * @brief Interrogation d'un modèle FMT.
     *
     * Chaque opération existe en deux versions :
     *
     * - une **version pure**, qui prend le modèle en paramètre. C'est celle
     *   qu'utilisent les tests C++ : elle n'a besoin d'aucun cache.
     * - une **version indexée**, qui résout le modèle dans FMTFormCache à partir
     *   de son index de scénario. C'est celle qu'appelle le wrapper, ce qui lui
     *   évite de manipuler le moindre objet FMT.
     *
     * Tout ce qui ne dépend pas d'un modèle appartient à Environment.
     */
    class __declspec(dllexport) ModelQuery
    {
    public:
        /**
         * @brief Retourne l'âge maximum du modèle selon les yields basés sur l'âge.
         * @param p_model Le modèle à interroger.
         * @return L'âge maximum.
         */
        static int getMaxAge(const Models::FMTModel& p_model);
        static int getMaxAge(int p_modelIndex);

        /**
         * @brief Retourne la valeur d'un yield.
         * @param p_model Le modèle à interroger.
         * @param p_mask Le masque du peuplement.
         * @param p_yield Le nom du yield.
         * @param p_age L'âge auquel évaluer le yield.
         * @return La valeur du yield.
         */
        static double getYield(
            const Models::FMTModel& p_model,
            const std::string& p_mask,
            const std::string& p_yield,
            int p_age);
        static double getYield(
            int p_modelIndex,
            const std::string& p_mask,
            const std::string& p_yield,
            int p_age);

        /**
         * @brief Retourne tous les masques utiles du modèle selon les thèmes choisis.
         * @param p_model Le modèle à interroger.
         * @param p_periods Nombre de périodes à simuler si le modèle doit être résolu.
         * @param p_themesNumbers Numéros des thèmes retenus (1-based).
         * @param p_rasterPath Chemin des rasters, vide pour utiliser l'aire du modèle.
         * @return Les masques trouvés.
         */
        static std::set<std::string> getAllMasks(
            const Models::FMTModel& p_model,
            const int p_periods,
            const std::vector<int>& p_themesNumbers,
            const std::string& p_rasterPath);
        static std::set<std::string> getAllMasks(
            int p_modelIndex,
            const int p_periods,
            const std::vector<int>& p_themesNumbers,
            const std::string& p_rasterPath);

        /**
         * @brief Vérifie qu'un masque est valide pour les thèmes du modèle.
         * @param p_model Le modèle à interroger.
         * @param p_mask Le masque à valider.
         * @return true si le masque est valide.
         */
        static bool validateMask(const Models::FMTModel& p_model, const std::string& p_mask);
        static bool validateMask(int p_modelIndex, const std::string& p_mask);

        /**
         * @brief Retourne la représentation textuelle des contraintes du modèle.
         * @param p_model Le modèle à interroger.
         * @return Une entrée par contrainte.
         */
        static std::vector<std::string> getConstraintsAsText(const Models::FMTModel& p_model);
        static std::vector<std::string> getConstraintsAsText(int p_modelIndex);

        /**
         * @brief Retourne les noms des outputs du modèle.
         * @param p_model Le modèle à interroger.
         * @return Les noms des outputs.
         */
        static std::vector<std::string> getOutputsNames(const Models::FMTModel& p_model);
        static std::vector<std::string> getOutputsNames(int p_modelIndex);

        /**
         * @brief Retourne les noms des actions du modèle.
         * @param p_model Le modèle à interroger.
         * @return Les noms des actions.
         */
        static std::vector<std::string> getActionsNames(const Models::FMTModel& p_model);
        static std::vector<std::string> getActionsNames(int p_modelIndex);

        /**
         * @brief Retourne les agrégats distincts déclarés par les actions du modèle.
         * @param p_model Le modèle à interroger.
         * @return Les agrégats, dédoublonnés et triés.
         */
        static std::vector<std::string> getAggregates(const Models::FMTModel& p_model);
        static std::vector<std::string> getAggregates(int p_modelIndex);

        /**
         * @brief Retourne les noms des yields du modèle.
         * @param p_model Le modèle à interroger.
         * @return Les noms des yields.
         */
        static std::vector<std::string> getYieldsNames(const Models::FMTModel& p_model);
        static std::vector<std::string> getYieldsNames(int p_modelIndex);

        /**
         * @brief Retourne le nombre de thèmes du modèle.
         * @param p_model Le modèle à interroger.
         * @return Le nombre de thèmes.
         */
        static int getThemesCount(const Models::FMTModel& p_model);
        static int getThemesCount(int p_modelIndex);

        /**
         * @brief Retourne les attributs de base d'un thème.
         * @param p_model Le modèle à interroger.
         * @param p_themeIndex Index du thème (0-based).
         * @return Les attributs du thème.
         */
        static std::vector<std::string> getThemeAttributes(
            const Models::FMTModel& p_model,
            const int p_themeIndex);
        static std::vector<std::string> getThemeAttributes(
            int p_modelIndex,
            const int p_themeIndex);

        /**
         * @brief Lit les cédules (SEQ) associées au modèle dans un fichier primaire.
         * @param p_primaryFilePath Chemin du fichier .pri.
         * @param p_model Le modèle dont on lit les cédules.
         * @return Les cédules lues, vide si le fichier n'en contient pas.
         */
        static std::vector<Core::FMTSchedule> readSchedules(
            const std::string& p_primaryFilePath,
            const Models::FMTModel& p_model);
        static std::vector<Core::FMTSchedule> readSchedules(
            const std::string& p_primaryFilePath,
            int p_modelIndex);

        /**
         * @brief Retourne la dernière période couverte par les cédules du modèle.
         * @param p_primaryFilePath Chemin du fichier .pri.
         * @param p_modelIndex Index du scénario dans le cache.
         * @return La dernière période, ou 0 si aucune cédule n'est disponible.
         */
        static int getPeriodsCount(const std::string& p_primaryFilePath, int p_modelIndex);

        /**
         * @brief Écrit un projet (base + scénarios) dans un dossier.
         *
         * Le nom du fichier .pri est dérivé du nom du premier modèle, qui sert de base.
         *
         * @param p_models Les modèles à écrire ; le premier sert de base (ROOT).
         * @param p_destinationDirectory Le dossier de destination.
         */
        static void writeToProject(
            const std::vector<Models::FMTModel>& p_models,
            const std::string& p_destinationDirectory);

        /**
         * @brief Écrit tout le contenu du cache dans un dossier.
         * @param p_destinationDirectory Le dossier de destination.
         */
        static void writeToProjectFromCache(const std::string& p_destinationDirectory);

    private:
        /**
         * @brief Résout un modèle du cache en validant que celui-ci n'est pas vide.
         * @param p_modelIndex Index du scénario.
         * @return Le modèle demandé.
         */
        static const Models::FMTModel& _getCachedModel(int p_modelIndex);

        /**
         * @brief Retourne l'aire du modèle telle que lue dans les rasters.
         * @param p_model Le modèle à utiliser.
         * @param p_rasterPath Le dossier contenant les rasters.
         * @return Les développements actuels lus.
         */
        static std::vector<Core::FMTActualDevelopment> _getRasterArea(
            const Models::FMTModel& p_model,
            const std::string& p_rasterPath);

        /**
         * @brief Construit le masque « tout accepté » pour une liste de thèmes.
         * @param p_themes Les thèmes du modèle.
         * @return Le masque correspondant.
         */
        static Core::FMTMask _getFullMask(const std::vector<Core::FMTTheme>& p_themes);

        /**
         * @brief Décompose un masque sur chacun des thèmes fournis.
         * @param p_mask Le masque de départ.
         * @param p_themes Les thèmes sur lesquels décomposer.
         * @return Les masques obtenus.
         */
        static std::set<std::string> _getThemesDecomposition(
            const Core::FMTMask& p_mask,
            const std::vector<Core::FMTTheme>& p_themes);

        /**
         * @brief Au-delà de ce nombre de combinaisons d'attributs, la décomposition
         *        exhaustive des masques est trop coûteuse et le modèle est résolu.
         */
        static const size_t m_GET_ALL_MASKS_THRESHOLD = 1000000;
    };
}

#endif // FMTWRAPPERCORE_MODELQUERY_HEADER
