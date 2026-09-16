#include "Environment.h"

#include "FMTExceptionHandler.h"
#include "FMTFreeExceptionHandler.h"
#include "FMTLpSolver.h"
#include "FMTModelParser.h"
#include "FMTVersion.h"

namespace FMTWrapperCore
{
    std::vector<int> Environment::getAvailableSolvers()
    {
        std::vector<int> solvers;

        try
        {
            for (const Models::FMTSolverInterface SOLVER :
                Models::FMTLpSolver::getAvailableSolverInterface())
            {
                solvers.push_back(static_cast<int>(SOLVER));
            }
        }
        catch (const std::exception& e)
        {
            Exception::FMTFreeExceptionHandler().raiseFromCatch(
                e.what(), "Environment::getAvailableSolvers", __LINE__, __FILE__);
        }
        catch (...)
        {
            Exception::FMTFreeExceptionHandler().raiseFromCatch(
                "Unknown error", "Environment::getAvailableSolvers", __LINE__, __FILE__);
        }

        return solvers;
    }

    std::string Environment::getSolverName(int p_solver)
    {
        std::string name;

        try
        {
            const Models::FMTSolverInterface SOLVER =
                static_cast<Models::FMTSolverInterface>(p_solver);

            name = std::string(Models::FMTLpSolver::toString(SOLVER));
        }
        catch (const std::exception& e)
        {
            Exception::FMTFreeExceptionHandler().raiseFromCatch(
                e.what(), "Environment::getSolverName", __LINE__, __FILE__);
        }
        catch (...)
        {
            Exception::FMTFreeExceptionHandler().raiseFromCatch(
                "Unknown error", "Environment::getSolverName", __LINE__, __FILE__);
        }

        return name;
    }

    std::vector<std::string> Environment::getVectorDriverExtensions()
    {
        std::vector<std::string> filters;

        try
        {
            Parser::FMTModelParser modelParser;

            const std::vector<std::vector<std::string>> EXTENSIONS =
                modelParser.getGDALVectorDriverExtensions();

            const std::vector<std::string> DRIVERS =
                modelParser.getGDALVectorDriverNames();

            for (size_t driver = 0; driver < EXTENSIONS.size(); ++driver)
            {
                for (const std::string& EXTENSION : EXTENSIONS.at(driver))
                {
                    filters.push_back(DRIVERS.at(driver) + "|*." + EXTENSION);
                }
            }
        }
        catch (const std::exception& e)
        {
            Exception::FMTFreeExceptionHandler().raiseFromCatch(
                e.what(), "Environment::getVectorDriverExtensions", __LINE__, __FILE__);
        }
        catch (...)
        {
            Exception::FMTFreeExceptionHandler().raiseFromCatch(
                "Unknown error", "Environment::getVectorDriverExtensions", __LINE__, __FILE__);
        }

        return filters;
    }

    std::string Environment::getChangeLog()
    {
        return Version::FMTVersion::getChangelog(true);
    }

    std::string Environment::getExceptionDescription(int p_exceptionId)
    {
        return Exception::FMTExceptionHandler::getErrorDescription(
            true,
            static_cast<Exception::FMTexc>(p_exceptionId));
    }

    std::vector<int> Environment::getErrorsToIgnore()
    {
        const std::vector<Exception::FMTexc> ERRORS =
            Exception::FMTExceptionHandler::getErrorsToIgnore();

        std::vector<int> toIgnore;
        toIgnore.reserve(ERRORS.size());

        // Do not name this variable ERROR: windows.h makes it a macro.
        for (const Exception::FMTexc EXCEPTION_ID : ERRORS)
        {
            toIgnore.push_back(static_cast<int>(EXCEPTION_ID));
        }

        return toIgnore;
    }
}
