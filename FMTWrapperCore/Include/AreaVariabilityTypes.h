#ifndef FMTWRAPPERCORE_AREAVARIABILITYTYPES_HEADER
#define FMTWRAPPERCORE_AREAVARIABILITYTYPES_HEADER

/**
 * @file AreaVariabilityTypes.h
 * @brief DTO de AreaVariability : types std uniquement. C'est le contrat de traduction que le
 *        wrapper voit, par Controller.h.
 */

#include <string>
#include <vector>

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
}

#endif // FMTWRAPPERCORE_AREAVARIABILITYTYPES_HEADER
