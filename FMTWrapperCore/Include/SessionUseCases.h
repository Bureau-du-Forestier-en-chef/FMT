/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTWRAPPERCORE_SESSIONUSECASES_HEADER
#define FMTWRAPPERCORE_SESSIONUSECASES_HEADER

#include "Events.h"

#include <string>
#include <vector>

namespace FMTWrapper::Backend
{
    /**
    @brief Session of the interface: log file, exception handler and error reporting.

    Internal to FMTWrapperCore. The interface reaches these operations through Controller,
    which holds no logic of its own.
    */
    class SessionUseCases
    {
    public:
        /**
        @brief Adds a subscriber to the events of the session.

        @return The identifier needed to remove it, never zero.
        */
        static SubscriptionId subscribe(EventHandler p_handler);

        static void unsubscribe(SubscriptionId p_subscription);

        static void initializeLogger(const std::string& p_logFilePath);

        static void recoverLoggerAndHandler();

        static void closeLogger();

        /**
        @brief Turns the given FMT exceptions into warnings.

        @param[in] p_exceptionIds Identifiers of Exception::FMTexc values.
        @param[in] p_maxWarnings Warnings tolerated before silence. Zero or less selects
        DEFAULT_MAX_WARNINGS.
        */
        static void setErrorsToWarnings(
            const std::vector<int>& p_exceptionIds,
            int p_maxWarnings);

        /**
        @brief Logs the exception being handled, reports it and returns its formatted stack.

        Called from a catch block. Without an interface logger, the stack is appended to the
        log file instead. The stack is published as an ErrorEvent, which is how an interface
        shows it.
        */
        static std::string logCurrentException(
            const std::string& p_text,
            const std::string& p_method,
            int p_line,
            const std::string& p_file);

        /**
        @brief Opens the file location named in an error stack, in the text editor.
        */
        static void openErrorLocation(const std::string& p_errorStack);
    };
}

#endif // FMTWRAPPERCORE_SESSIONUSECASES_HEADER
