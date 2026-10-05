#ifndef FMTWRAPPERCORE_ENVIRONMENT_HEADER
#define FMTWRAPPERCORE_ENVIRONMENT_HEADER

#include <string>
#include <vector>

#include "FMTWrapperCoreExport.h"

namespace FMTWrapper::Backend
{
    /**
     * @brief Queries on the FMT environment, independent of any model.
     *
     * Groups what belongs to the capabilities of the library itself: available solvers,
     * GDAL drivers, changelog and exception descriptions. Anything that queries a model
     * belongs to ModelQuery.
     */
    class FMT_WRAPPER_CORE_EXPORT Environment
    {
    public:
        /**
         * @brief Returns the solver interfaces compiled into FMT.
         * @return The Models::FMTSolverInterface values, converted to int.
         */
        static std::vector<int> getAvailableSolvers();

        /**
         * @brief Returns the readable name of a solver interface.
         * @param p_solver A Models::FMTSolverInterface value.
         * @return The solver name, or an empty string on failure.
         */
        static std::string getSolverName(int p_solver);

        /**
         * @brief Returns the vector file extensions supported by GDAL.
         * @return One entry per driver/extension pair, formatted "DRIVER|*.ext" to feed a
         *         dialog box filter directly.
         */
        static std::vector<std::string> getVectorDriverExtensions();

        /**
         * @brief Returns the FMT changelog.
         * @return The changelog.
         */
        static std::string getChangeLog();

        /**
         * @brief Returns the description of an FMT exception.
         * @param p_exceptionId An Exception::FMTexc value.
         * @return The exception description.
         */
        static std::string getExceptionDescription(int p_exceptionId);

        /**
         * @brief Returns the errors that can be ignored.
         * @return The Exception::FMTexc values, converted to int.
         */
        static std::vector<int> getErrorsToIgnore();
    };
}

#endif // FMTWRAPPERCORE_ENVIRONMENT_HEADER
