#ifndef FMTWRAPPERCORE_SES_HEADER
#define FMTWRAPPERCORE_SES_HEADER

#include <string>
#include <vector>

#include "FMTWrapperCoreExport.h"
#include "SESTypes.h"

namespace Core {
    class FMTSchedule;
    class FMTConstraint;
    class FMTOutput;
    class FMTTheme;
    class FMTAction;
}

namespace Models {
    class FMTModel;
    class FMTSeModel;
}

namespace Spatial {
    class FMTSpatialSchedule;
}

namespace FMTWrapperCore
{
    /**
     * @brief Classe pour les simulations spatiales explicites
     */
    class FMTWRAPPERCOREEXPORT SES
    {
    public:
        /**
         * @brief Exécute une simulation spatiale explicite telle que l'interface la lance
         * @param params Paramètres de simulation ; les cédules sont lues dans
         *        params.primaryFilePath
         * @param baseModel Modèle FMT de base
         * @return Résultats complets de la simulation
         *
         * Lit les cédules du modèle, repasse au modèle un clone du logger courant, puis
         * journalise la progression autour de la simulation proprement dite, faite par
         * RunSES(const SESParameters&, const Models::FMTModel&, const std::vector<Core::FMTSchedule>&).
         */
        static SESResults RunSES(
            const SESParameters& params,
            const Models::FMTModel& baseModel);

        /**
         * @brief Exécute une simulation spatiale explicite complète
         * @param params Paramètres de simulation
         * @param baseModel Modèle FMT de base
         * @param schedules Schedules à utiliser pour la simulation
         * @return Résultats complets de la simulation
         *
         * Cette méthode orchestre toute la simulation et peut être appelée
         * directement depuis du code C++ pur pour les tests et le débogage.
         */
        static SESResults RunSES(
            const SESParameters& params,
            const Models::FMTModel& baseModel,
            const std::vector<Core::FMTSchedule>& schedules);

        /**
         * @brief Exécute une optimisation spatiale (Simulated Annealing)
         * @param params Paramètres d'optimisation
         * @param baseModel Modèle FMT de base (déjà chargé)
         * @return Résultats complets de l'optimisation
         *
         * Cette méthode orchestre toute l'optimisation spatiale, en journalise la
         * progression et les outputs, et peut être appelée directement depuis du code
         * C++ pur pour les tests et le débogage.
         */
        static SAResults RunOptimization(
            const SAParameters& params,
            const Models::FMTModel& baseModel);

    private:
        /**
         * @brief Construit les thèmes de croissance à partir des indices
         * @param allThemes Tous les thèmes du modèle
         * @param themeIndices Indices des thèmes (1-based)
         * @return Vecteur des thèmes sélectionnés
         */
        static std::vector<Core::FMTTheme> buildGrowthThemes(
            const std::vector<Core::FMTTheme>& allThemes,
            const std::vector<int>& themeIndices);

        /**
         * @brief Convertit toutes les transitions du modèle en transitions "single"
         * @param model Le modèle dont les transitions sont modifiées en place
         */
        static void applySingleTransitions(Models::FMTModel& model);

        /**
         * @brief Construit les chemins des rasters, lit la forêt initiale et
         *        l'assigne au modèle.
         * @param model Le modèle SES qui reçoit la forêt initiale
         * @param rastersPath Chemin de base des rasters
         * @param useStanlock true pour utiliser le raster STANLOCK
         * @param[out] ageRasterPath Chemin du raster d'âge construit
         * @param[out] themeRasterPaths Chemins des rasters de thèmes construits
         */
        static void prepareInitialForest(
            Models::FMTSeModel& model,
            const std::string& rastersPath,
            bool useStanlock,
            std::string& ageRasterPath,
            std::vector<std::string>& themeRasterPaths);

        /**
         * @brief Génère le rapport des contraintes infaisables
         * @param semodel Le modèle SES à analyser
         * @return Vecteur de messages d'infaisabilité (un par contrainte brisée,
         *         suivi du pourcentage global de contraintes infaisables)
         */
        static std::vector<std::string> generateInfeasibilityReport(const Models::FMTSeModel& semodel);

        /**
         * @brief Génère le rapport de carbone spatial
         * @param semodel Le modèle SES
         * @param numberOfPeriods Nombre de périodes
         * @param schedules Vecteur des schedules originaux
         * @return Structure contenant les données du rapport
         */
        static CarbonReportData generateCarbonReport(
            const Models::FMTSeModel& semodel,
            const int numberOfPeriods,
            const std::vector<Core::FMTSchedule>& schedules);

        /**
         * @brief Écrit les perturbations (transitions GCBM)
         * @param semodel Le modèle SES
         * @param outputBasePath Chemin de base pour les sorties
         * @param numberOfPeriods Nombre de périodes
         * @param growthThemeIndices Indices des thèmes de croissance (1-based)
         * @return Vecteur des chemins de fichiers de transition créés
         */
        static std::vector<std::string> writeDisturbances(
            const Models::FMTSeModel& semodel,
            const std::string& outputBasePath,
            const int numberOfPeriods,
            const std::vector<int>& growthThemeIndices);

        /**
         * @brief Génère les données d'événements
         * @param semodel Le modèle SES
         * @return Structure contenant les statistiques d'événements
         */
        static EventsData generateEventsData(const Models::FMTSeModel& semodel);

        /**
         * @brief Génère les données d'événements et les écrit dans un fichier
         * @param semodel Le modèle SES
         * @param eventsFilePath Chemin complet du fichier d'événements à écrire
         * @return Structure contenant les statistiques d'événements
         */
        static EventsData writeEventsFile(
            const Models::FMTSeModel& semodel,
            const std::string& eventsFilePath);

        /**
         * @brief Calcule les outputs pour toutes les périodes
         * @param semodel Le modèle SES
         * @param outputNames Noms des outputs à calculer
         * @param numberOfPeriods Nombre de périodes
         * @param[out] selectedOutputs Outputs retenus par outputNames, dans l'ordre du modèle
         * @return Structure contenant les résultats des outputs
         */
        static OutputsData calculateOutputs(
            const Models::FMTSeModel& semodel,
            const std::vector<std::string>& outputNames,
            const int numberOfPeriods,
            std::vector<Core::FMTOutput>& selectedOutputs);

        /**
         * @brief Écrit les outputs spatiaux
         * @param semodel Le modèle SES
         * @param outputs Vecteur des outputs à écrire
         * @param minPeriod Période minimum
         * @param maxPeriod Période maximum
         * @param outputPath Chemin de sortie
         * @return Vecteur des chemins de fichiers raster créés
         */
        static std::vector<std::string> writeSpatialOutputs(
            const Models::FMTSeModel& semodel,
            const std::vector<Core::FMTOutput>& outputs,
            const int minPeriod,
            const int maxPeriod,
            const std::string& outputPath);

        /**
         * @brief Calcule les prédicteurs
         * @param semodel Le modèle SES
         * @param rasterPath Chemin des rasters
         * @param numberOfPeriods Nombre de périodes
         * @param predictorYields Noms des yields pour les prédicteurs
         * @return Structure contenant les données des prédicteurs
         */
        static PredictorsData calculatePredictors(
            const Models::FMTSeModel& semodel,
            const std::string& rasterPath,
            const int numberOfPeriods,
            const std::vector<std::string>& predictorYields);

        /**
         * @brief Écrit le schedule dans un fichier
         * @param semodel Le modèle SES
         * @param outputPath Chemin du fichier de sortie
         * @return Chemin complet du fichier créé
         */
        static std::string writeSchedule(
            const Models::FMTSeModel& semodel,
            const std::string& outputPath);

        /**
         * @brief Écrit la forêt mise à jour (rasters)
         * @param semodel Le modèle SES
         * @param rasterPath Chemin de base des rasters
         * @param themeRasterPaths Chemins des rasters de thèmes
         * @param ageRasterPath Chemin du raster d'âge
         * @param stanlockRasterPath Chemin du raster stanlock
         */
        static void writeUpdatedForest(
            const Models::FMTSeModel& semodel,
            const std::string& rasterPath,
            const std::vector<std::string>& themeRasterPaths,
            const std::string& ageRasterPath,
            const std::string& stanlockRasterPath);

        /**
         * @brief Exporte les résultats via le model parser
         * @param semodel Le modèle SES
         * @param outputs Vecteur des outputs
         * @param minPeriod Période minimum
         * @param maxPeriod Période maximum
         * @param outputPath Chemin de sortie
         * @param outputLevel Niveau de détail des outputs
         * @param gdalProvider Provider GDAL à utiliser
         */
        static void exportResults(
            const Models::FMTSeModel& semodel,
            const std::vector<Core::FMTOutput>& outputs,
            const int minPeriod,
            const int maxPeriod,
            const std::string& outputPath,
            const int outputLevel,
            const std::string& gdalProvider);
    };
}

#endif // FMTWRAPPERCORE_SES_HEADER