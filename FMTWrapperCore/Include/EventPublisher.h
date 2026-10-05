/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTWRAPPERCORE_EVENTPUBLISHER_HEADER
#define FMTWRAPPERCORE_EVENTPUBLISHER_HEADER

#include "Events.h"
#include "FMTWrapperCoreExport.h"

#include <memory>
#include <mutex>
#include <utility>
#include <vector>

namespace FMTWrapper::Backend
{
    // DocString: EventPublisher
    /**
    @brief The single point through which the Core reports its events.

    What publishes an event -- the logger, the exception handling, a use case -- knows
    nothing of who listens. How an event reaches its subscribers is decided here and
    nowhere else: one subscriber or several today, a filter or a queue for the thread of an
    interface tomorrow, without touching what publishes.

    Thread safety: the three operations may be called from any thread. A subscriber runs on
    the thread that publishes, outside the internal lock, and may therefore subscribe,
    unsubscribe or publish in turn.
    */
    class FMT_WRAPPER_CORE_EXPORT EventPublisher
    {
    public:
        /**
        @brief Adds a subscriber.

        @param[in] p_handler Called for every event until it is removed.

        @return The identifier needed to remove it, never zero.
        */
        SubscriptionId subscribe(EventHandler p_handler);

        /**
        @brief Removes a subscriber. An unknown identifier is ignored.
        */
        void unsubscribe(SubscriptionId p_subscription);

        /**
        @brief Reports an event to every subscriber, in subscription order.
        */
        void publish(const Event& p_event) const;

    private:
        using Subscribers = std::vector<std::pair<SubscriptionId, EventHandler>>;

        // The list is replaced rather than modified, so publishing only copies a shared
        // pointer: no allocation on a path that carries every log line.
        mutable std::mutex m_mutex;
        std::shared_ptr<const Subscribers> m_subscribers;
        SubscriptionId m_lastSubscription = 0;
    };
}

#endif // FMTWRAPPERCORE_EVENTPUBLISHER_HEADER
