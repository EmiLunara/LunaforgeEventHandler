#pragma once

#include "SubscriptionHandle.h"

#include <memory>

namespace Lunaforge::Events
{
    class EventBus;

    class Subscription
    {
    public:
        Subscription() = default;
        Subscription(EventBus* Bus, SubscriptionHandle Handle, std::weak_ptr<void> Lifetime);

        ~Subscription();

        Subscription(const Subscription&) = delete;
        Subscription& operator=(const Subscription&) = delete;

        Subscription(Subscription&& Other) noexcept;
        Subscription& operator=(Subscription&& Other) noexcept;

        void Reset();

        bool IsValid() const;
    private:
        EventBus* m_Bus = nullptr;
        SubscriptionHandle m_Handle;
        std::weak_ptr<void> m_Lifetime;
    };
}