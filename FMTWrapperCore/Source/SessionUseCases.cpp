/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "SessionUseCases.h"

#include "CallbackLogger.h"
#include "FMTException.h"
#include "ModelCache.h"
#include "SessionTypes.h"
#include "WarningExceptionHandler.h"

#include <fstream>
#include <string>
#include <vector>

namespace FMTWrapper::Backend
{
    void SessionUseCases::initializeLogger(
        const std::string& p_logFilePath,
        void* p_callback)
    {
        ModelCache::GetInstance()->InitializeLogger(p_logFilePath, p_callback);
    }

    void SessionUseCases::recoverLoggerAndHandler(void* p_callback)
    {
        ModelCache::GetInstance()->RecoverLoggerAndHandler(p_callback);
    }

    void SessionUseCases::closeLogger()
    {
        ModelCache::GetInstance()->CloseLogger();
    }

    void SessionUseCases::setErrorsToWarnings(
        const std::vector<int>& p_exceptionIds,
        int p_maxWarnings)
    {
        std::vector<Exception::FMTexc> warnings;
        warnings.reserve(p_exceptionIds.size());

        for (const int EXCEPTION_ID : p_exceptionIds)
        {
            warnings.push_back(static_cast<Exception::FMTexc>(EXCEPTION_ID));
        }

        ModelCache::GetInstance()->InitializeExceptionHandler(
            p_maxWarnings > 0 ? p_maxWarnings : DEFAULT_MAX_WARNINGS,
            warnings);
    }

    std::string SessionUseCases::logCurrentException(
        const std::string& p_text,
        const std::string& p_method,
        int p_line,
        const std::string& p_file)
    {
        ModelCache* cache = ModelCache::GetInstance();

        const std::string FORMATTED_STACK =
            cache->GetWarningHandler()->geterrorstack(p_text, p_method, p_line, p_file);

        CallbackLogger* logger = cache->GetCallbackLogger();

        if (logger)
        {
            logger->logWithLevel(
                "*************************************************************\n",
                0);

            logger->logWithLevel(
                "FMT - ERROR " + FORMATTED_STACK + "\n",
                0);
        }
        else
        {
            // Without an interface logger, the stack is appended directly to the log file.
            try
            {
                const std::string& LOGGER_FILENAME = cache->GetLoggerFilename();

                if (!LOGGER_FILENAME.empty())
                {
                    std::ofstream out(LOGGER_FILENAME, std::ios_base::app);

                    if (out.is_open())
                    {
                        out << "*************************************************************\n";
                        out << "FMT - ERROR " << FORMATTED_STACK << "\n";
                    }
                }
            }
            catch (...)
            {
            }
        }

        return FORMATTED_STACK;
    }

    void SessionUseCases::openErrorLocation(const std::string& p_errorStack)
    {
        ModelCache::GetInstance()->GetWarningHandler()->tryfileopener(p_errorStack);
    }
}
