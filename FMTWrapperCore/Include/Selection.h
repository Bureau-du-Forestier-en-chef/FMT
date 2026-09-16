#ifndef FMTWRAPPERCORE_SELECTION_HEADER
#define FMTWRAPPERCORE_SELECTION_HEADER

#include <string>
#include <vector>

#include "FMTConstraint.h"
#include "FMTOutput.h"
#include "FMTWrapperCoreExport.h"

namespace FMTWrapperCore
{
    /**
     * @brief Selection of a subset of constraints or outputs by name.
     *
     * These filters used to be duplicated between the wrapper (FMTForm) and SES, with two
     * diverging semantics. The implementation kept is the wrapper's, which is more
     * tolerant:
     *
     * - names are normalized before comparison (CR/LF removed, then trimmed), because a
     *   name that went through the .NET interface can carry stray line endings;
     * - the result follows the model order, not the selection order. It is the order in
     *   which the model declares them, and it is the one that matters to build the LP
     *   matrix and to order the output columns;
     * - each element of the model is kept at most once, even if several entries of the
     *   selection match it.
     */
    class FMTWRAPPERCOREEXPORT Selection
    {
    public:
        /**
         * @brief Filters the model constraints by the selected names.
         * @param p_allConstraints Every constraint of the model.
         * @param p_selectedNames Text representations of the selected constraints.
         * @return The selected constraints, in the model order.
         */
        static std::vector<Core::FMTConstraint> selectConstraints(
            const std::vector<Core::FMTConstraint>& p_allConstraints,
            const std::vector<std::string>& p_selectedNames);

        /**
         * @brief Filters the model outputs by the selected names.
         * @param p_allOutputs Every output of the model.
         * @param p_selectedNames Names of the selected outputs.
         * @return The selected outputs, in the model order.
         */
        static std::vector<Core::FMTOutput> selectOutputs(
            const std::vector<Core::FMTOutput>& p_allOutputs,
            const std::vector<std::string>& p_selectedNames);

        /**
         * @brief Returns the output with the requested name.
         * @param p_allOutputs Every output of the model.
         * @param p_name Name looked for.
         * @return The matching output, or a default-constructed output if none matches.
         */
        static Core::FMTOutput findOutput(
            const std::vector<Core::FMTOutput>& p_allOutputs,
            const std::string& p_name);

    private:
        /**
         * @brief Removes CR/LF, then leading and trailing spaces.
         * @param p_text Text to normalize.
         * @return The normalized text.
         */
        static std::string _normalize(const std::string& p_text);
    };
}

#endif // FMTWRAPPERCORE_SELECTION_HEADER
