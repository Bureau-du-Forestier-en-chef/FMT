#ifndef FMTWRAPPERCORE_MODELQUERY_HEADER
#define FMTWRAPPERCORE_MODELQUERY_HEADER

#include <set>
#include <string>
#include <vector>

#include "FMTWrapperCoreExport.h"

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
     * @brief Queries on an FMT model.
     *
     * Each operation takes the model as a parameter and needs no cache: the C++ tests call
     * it directly. The wrapper reaches it through Controller, which resolves the scenario
     * in ModelCache from its index.
     *
     * Anything that does not depend on a model belongs to Environment.
     */
    class FMT_WRAPPER_CORE_EXPORT ModelQuery
    {
    public:
        /**
         * @brief Returns the maximum age of the model according to its age-based yields.
         * @param p_model The model to query.
         * @return The maximum age.
         */
        static int getMaxAge(const Models::FMTModel& p_model);

        /**
         * @brief Returns the value of a yield.
         * @param p_model The model to query.
         * @param p_mask The stand mask.
         * @param p_yield The yield name.
         * @param p_age The age at which the yield is evaluated.
         * @return The yield value.
         */
        static double getYield(
            const Models::FMTModel& p_model,
            const std::string& p_mask,
            const std::string& p_yield,
            int p_age);

        /**
         * @brief Returns every useful mask of the model for the chosen themes.
         * @param p_model The model to query.
         * @param p_periods Number of periods to simulate if the model has to be solved.
         * @param p_themesNumbers Numbers of the selected themes (1-based).
         * @param p_rasterPath Path of the rasters, empty to use the model area.
         * @return The masks found.
         */
        static std::set<std::string> getAllMasks(
            const Models::FMTModel& p_model,
            const int p_periods,
            const std::vector<int>& p_themesNumbers,
            const std::string& p_rasterPath);

        /**
         * @brief Builds the "accept everything" mask for a list of themes.
         * @param p_themes The model themes.
         * @return The mask whose every attribute is "?".
         */
        static Core::FMTMask getFullMask(const std::vector<Core::FMTTheme>& p_themes);

        /**
         * @brief Checks that a mask is valid for the model themes.
         * @param p_model The model to query.
         * @param p_mask The mask to validate.
         * @return true if the mask is valid.
         */
        static bool validateMask(const Models::FMTModel& p_model, const std::string& p_mask);

        /**
         * @brief Returns the text representation of the model constraints.
         * @param p_model The model to query.
         * @return One entry per constraint.
         */
        static std::vector<std::string> getConstraintsAsText(const Models::FMTModel& p_model);

        /**
         * @brief Returns the names of the model outputs.
         * @param p_model The model to query.
         * @return The output names.
         */
        static std::vector<std::string> getOutputsNames(const Models::FMTModel& p_model);

        /**
         * @brief Returns the names of the model actions.
         * @param p_model The model to query.
         * @return The action names.
         */
        static std::vector<std::string> getActionsNames(const Models::FMTModel& p_model);

        /**
         * @brief Returns the distinct aggregates declared by the model actions.
         * @param p_model The model to query.
         * @return The aggregates, deduplicated and sorted.
         */
        static std::vector<std::string> getAggregates(const Models::FMTModel& p_model);

        /**
         * @brief Returns the names of the model yields.
         * @param p_model The model to query.
         * @return The yield names.
         */
        static std::vector<std::string> getYieldsNames(const Models::FMTModel& p_model);

        /**
         * @brief Returns the number of themes of the model.
         * @param p_model The model to query.
         * @return The number of themes.
         */
        static int getThemesCount(const Models::FMTModel& p_model);

        /**
         * @brief Returns the base attributes of a theme.
         * @param p_model The model to query.
         * @param p_themeIndex Theme index (0-based).
         * @return The theme attributes.
         */
        static std::vector<std::string> getThemeAttributes(
            const Models::FMTModel& p_model,
            const int p_themeIndex);

        /**
         * @brief Reads the schedules (SEQ) of the model from a primary file.
         * @param p_primaryFilePath Path of the .pri file.
         * @param p_model The model whose schedules are read.
         * @return The schedules read, empty if the file holds none.
         */
        static std::vector<Core::FMTSchedule> readSchedules(
            const std::string& p_primaryFilePath,
            const Models::FMTModel& p_model);

        /**
         * @brief Returns the last period covered by the model schedules.
         * @param p_primaryFilePath Path of the .pri file.
         * @param p_model The model whose schedules are read.
         * @return The last period, or 0 if no schedule is available.
         */
        static int getPeriodsCount(
            const std::string& p_primaryFilePath,
            const Models::FMTModel& p_model);

        /**
         * @brief Writes a project (base + scenarios) to a directory.
         *
         * The .pri file name is derived from the name of the first model, which is the base.
         *
         * @param p_models The models to write; the first one is the base (ROOT).
         * @param p_destinationDirectory The destination directory.
         */
        static void writeToProject(
            const std::vector<Models::FMTModel>& p_models,
            const std::string& p_destinationDirectory);

    private:
        /**
         * @brief Returns the model area as read from the rasters.
         * @param p_model The model to use.
         * @param p_rasterPath The directory holding the rasters.
         * @return The actual developments read.
         */
        static std::vector<Core::FMTActualDevelopment> _getRasterArea(
            const Models::FMTModel& p_model,
            const std::string& p_rasterPath);

        /**
         * @brief Decomposes a mask on each of the given themes.
         * @param p_mask The starting mask.
         * @param p_themes The themes to decompose on.
         * @return The resulting masks.
         */
        static std::set<std::string> _getThemesDecomposition(
            const Core::FMTMask& p_mask,
            const std::vector<Core::FMTTheme>& p_themes);
    };
}

#endif // FMTWRAPPERCORE_MODELQUERY_HEADER
