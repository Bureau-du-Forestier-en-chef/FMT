/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "EventPublisher.h"

#include <memory>
#include <mutex>
#include <utility>

namespace FMTWrapper::Backend
{
    SubscriptionId EventPublisher::subscribe(EventHandler p_handler)
    {
        std::lock_guard<std::mutex> guard(m_mutex);

        Subscribers subscribers = m_subscribers ? *m_subscribers : Subscribers();
        ++m_lastSubscription;
        subscribers.emplace_back(m_lastSubscription, std::move(p_handler));
        m_subscribers = std::make_shared<const Subscribers>(std::move(subscribers));

        return m_lastSubscription;
    }

    void EventPublisher::unsubscribe(SubscriptionId p_subscription)
    {
        std::lock_guard<std::mutex> guard(m_mutex);

        if (!m_subscribers)
        {
            return;
        }

        Subscribers remaining;
        remaining.reserve(m_subscribers->size());

        for (const std::pair<SubscriptionId, EventHandler>& SUBSCRIBER : *m_subscribers)
        {
            if (SUBSCRIBER.first != p_subscription)
            {
                remaining.push_back(SUBSCRIBER);
            }
        }

        m_subscribers = std::make_shared<const Subscribers>(std::move(remaining));
    }

    void EventPublisher::publish(const Event& p_event) const
    {
        std::shared_ptr<const Subscribers> subscribers;

        {
            std::lock_guard<std::mutex> guard(m_mutex);
            subscribers = m_subscribers;
        }

        if (!subscribers)
        {
            return;
        }

        // Outside the lock: a subscriber may subscribe, unsubscribe or publish in turn.
        for (const std::pair<SubscriptionId, EventHandler>& SUBSCRIBER : *subscribers)
        {
            SUBSCRIBER.second(p_event);
        }
    }
}
