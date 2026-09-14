#ifndef FMTWRAPPERCORE_AREAVARIABILITY_HEADER
#define FMTWRAPPERCORE_AREAVARIABILITY_HEADER

#include <string>
#include <vector>

namespace Models
{
    class FMTModel;
}

namespace FMTWrapperCore
{
    /**
     * @brief Paramètres de la variabilité de l'aire initiale.
     */
    struct AreaVariabilityParameters
    {
        /** Solveur, valeur de Models::FMTSolverInterface. */
        int solver = 0;
        /** Contraintes retenues, par leur représentation textuelle. */
        std::vector<std::string> constraintNames;
        /** Nombre de périodes à construire. */
        int numberOfPeriods = 0;
        /** Noms des outputs à écrire ; si la liste est vide, l'opération est refusée. */
        std::vector<std::string> outputNames;
        /** Niveau des sorties, valeur de Core::FMToutputlevel. */
        int outputLevel = 0;
        /** Première période écrite. */
        int outputMinPeriod = 0;
        /** Dernière période écrite. */
        int outputMaxPeriod = 0;
        /** Emplacement des sorties. */
        std::string outputPath;
        /** Pilote GDAL des sorties. */
        std::string gdalProvider;
        /**
         * Table des proportions, telle que lue du fichier de paramètres : la première
         * ligne est un en-tête, la dernière colonne de chaque ligne est la proportion et
         * les colonnes précédentes forment le masque.
         */
        std::vector<std::vector<std::string>> proportionsTable;
        /** false si l'interface n'a transmis aucune table. */
        bool proportionsTableProvided = false;
    };

    /**
     * @brief Résultat de la variabilité de l'aire initiale.
     */
    struct AreaVariabilityResults
    {
        /** false quand l'opération a été refusée. */
        bool success = false;
        /** Motif du refus quand success vaut false, vide sinon. */
        std::string errorMessage;
    };

    /**
     * @brief Variabilité de l'aire initiale : écrit les sorties d'un modèle dont l'aire
     *        initiale est modifiée selon des proportions par masque.
     *
     * Trois situations font refuser l'opération, sans exception : table absente, table
     * réduite à son en-tête, aucun output demandé. Des masques qui se recoupent sur un
     * même développement lèvent une exception.
     */
    class __declspec(dllexport) AreaVariability
    {
    public:
        /**
         * @brief Applique les proportions au modèle et écrit ses sorties.
         * @param p_params Paramètres de l'opération.
         * @param p_model Modèle de départ.
         * @return Le résultat ; success vaut false si l'opération a été refusée.
         */
        static AreaVariabilityResults run(
            const AreaVariabilityParameters& p_params,
            const Models::FMTModel& p_model);

        /**
         * @brief Applique les proportions à un scénario du cache et écrit ses sorties.
         * @param p_params Paramètres de l'opération.
         * @param p_modelIndex Index du scénario dans FMTFormCache.
         * @return Le résultat ; success vaut false si l'opération a été refusée.
         */
        static AreaVariabilityResults run(
            const AreaVariabilityParameters& p_params,
            int p_modelIndex);
    };
}

#endif // FMTWRAPPERCORE_AREAVARIABILITY_HEADER
