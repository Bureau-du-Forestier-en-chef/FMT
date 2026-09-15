#ifndef FMTWRAPPERCORE_ENVIRONMENT_HEADER
#define FMTWRAPPERCORE_ENVIRONMENT_HEADER

#include <string>
#include <vector>

#include "FMTWrapperCoreExport.h"

namespace FMTWrapperCore
{
    /**
     * @brief Interrogation de l'environnement FMT, indépendamment de tout modèle.
     *
     * Regroupe ce qui relève des capacités de la bibliothèque elle-même :
     * solveurs disponibles, pilotes GDAL, changelog et description des exceptions.
     * Tout ce qui interroge un modèle appartient à ModelQuery.
     */
    class FMTWRAPPERCOREEXPORT Environment
    {
    public:
        /**
         * @brief Retourne les interfaces de solveurs compilées dans FMT.
         * @return Les valeurs de Models::FMTSolverInterface, converties en int.
         */
        static std::vector<int> getAvailableSolvers();

        /**
         * @brief Retourne le nom lisible d'une interface de solveur.
         * @param p_solver Valeur de Models::FMTSolverInterface.
         * @return Le nom du solveur, ou une chaîne vide en cas d'échec.
         */
        static std::string getSolverName(int p_solver);

        /**
         * @brief Retourne les extensions de fichiers vectoriels supportées par GDAL.
         * @return Une entrée par couple pilote/extension, formatée "PILOTE|*.ext"
         *         pour alimenter directement un filtre de boîte de dialogue.
         */
        static std::vector<std::string> getVectorDriverExtensions();

        /**
         * @brief Retourne le journal des modifications de FMT.
         * @return Le changelog.
         */
        static std::string getChangeLog();

        /**
         * @brief Retourne la description d'une exception FMT.
         * @param p_exceptionId Valeur de Exception::FMTexc.
         * @return La description de l'exception.
         */
        static std::string getExceptionDescription(int p_exceptionId);

        /**
         * @brief Retourne les erreurs qui peuvent être ignorées.
         * @return Les valeurs de Exception::FMTexc, converties en int.
         */
        static std::vector<int> getErrorsToIgnore();
    };
}

#endif // FMTWRAPPERCORE_ENVIRONMENT_HEADER
