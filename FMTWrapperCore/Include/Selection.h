#ifndef FMTWRAPPERCORE_SELECTION_HEADER
#define FMTWRAPPERCORE_SELECTION_HEADER

#include <string>
#include <vector>

#include "FMTConstraint.h"
#include "FMTOutput.h"

namespace FMTWrapperCore
{
    /**
     * @brief Sélection d'un sous-ensemble de contraintes ou d'outputs par nom.
     *
     * Ces filtres étaient auparavant dupliqués entre le wrapper (FMTForm) et SES,
     * avec deux sémantiques divergentes. L'implémentation retenue est celle du
     * wrapper, plus tolérante :
     *
     * - les noms sont normalisés avant comparaison (retrait des CR/LF, puis trim),
     *   parce qu'un nom qui a transité par l'interface .NET peut porter des fins
     *   de ligne parasites ;
     * - le résultat suit l'ordre du modèle, pas celui de la sélection. C'est
     *   l'ordre dans lequel le modèle les déclare, et c'est lui qui compte pour la
     *   construction de la matrice LP et pour l'ordre des colonnes de sortie ;
     * - chaque élément du modèle est retenu au plus une fois, même si plusieurs
     *   entrées de la sélection lui correspondent.
     */
    class __declspec(dllexport) Selection
    {
    public:
        /**
         * @brief Filtre les contraintes du modèle selon les noms sélectionnés.
         * @param p_allConstraints Toutes les contraintes du modèle.
         * @param p_selectedNames Représentations textuelles des contraintes retenues.
         * @return Les contraintes retenues, dans l'ordre du modèle.
         */
        static std::vector<Core::FMTConstraint> selectConstraints(
            const std::vector<Core::FMTConstraint>& p_allConstraints,
            const std::vector<std::string>& p_selectedNames);

        /**
         * @brief Filtre les outputs du modèle selon les noms sélectionnés.
         * @param p_allOutputs Tous les outputs du modèle.
         * @param p_selectedNames Noms des outputs retenus.
         * @return Les outputs retenus, dans l'ordre du modèle.
         */
        static std::vector<Core::FMTOutput> selectOutputs(
            const std::vector<Core::FMTOutput>& p_allOutputs,
            const std::vector<std::string>& p_selectedNames);

        /**
         * @brief Retourne l'output portant le nom demandé.
         * @param p_allOutputs Tous les outputs du modèle.
         * @param p_name Nom recherché.
         * @return L'output correspondant, ou un output construit par défaut si
         *         aucun ne correspond.
         */
        static Core::FMTOutput findOutput(
            const std::vector<Core::FMTOutput>& p_allOutputs,
            const std::string& p_name);

    private:
        /**
         * @brief Retire les CR/LF puis les espaces de tête et de fin.
         * @param p_text Texte à normaliser.
         * @return Le texte normalisé.
         */
        static std::string _normalize(const std::string& p_text);
    };
}

#endif // FMTWRAPPERCORE_SELECTION_HEADER
