/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTWRAPPERCORE_EVENTS_HEADER
#define FMTWRAPPERCORE_EVENTS_HEADER

#include <functional>
#include <string>
#include <variant>

namespace FMTWrapper::Backend
{
    // DocString: LogEvent
    /**
    @brief A message produced while an operation runs.
    */
    struct LogEvent
    {
        std::string message;
    };

    // DocString: ErrorEvent
    /**
    @brief An error caught at the boundary of an interface, with its formatted stack.
    */
    struct ErrorEvent
    {
        std::string errorStack;
    };

    // DocString: Event
    /**
    @brief What the Core reports to an interface, in portable types.

    Reporting something new means adding a structure to this list. A subscriber written with
    std::get_if or std::visit ignores the events it does not know, so the Core can report
    more without breaking an interface that was written before.
    */
    using Event = std::variant<LogEvent, ErrorEvent>;

    // DocString: EventHandler
    /**
    @brief A subscriber.

    It runs on the thread that publishes the event, which may be a worker thread of FMT, and
    must therefore return quickly and handle its own synchronization.
    */
    using EventHandler = std::function<void(const Event&)>;

    // DocString: SubscriptionId
    /**
    @brief Identifies a subscription. Zero is never issued and means "not subscribed".
    */
    using SubscriptionId = int;
}

#endif // FMTWRAPPERCORE_EVENTS_HEADER
