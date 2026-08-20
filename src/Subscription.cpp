#include <EventHandler/Subscription.h>

#include <EventHandler/EventBus.h>
#include <utility>

namespace Lunaforge::Events
{
    Subscription::Subscription(EventBus* Bus, SubscriptionHandle Handle, std::weak_ptr<void> Lifetime)
            : m_Bus(Bus), m_Handle(Handle), m_Lifetime(std::move(Lifetime))
        {
        }

    Subscription::~Subscription()
    {
        Reset();
    }

    Subscription::Subscription(Subscription&& Other) noexcept
            : m_Bus(Other.m_Bus), m_Handle(Other.m_Handle), m_Lifetime(std::move(Other.m_Lifetime))
        {
            Other.m_Bus = nullptr;
            Other.m_Handle = {};
        }

    void Subscription::Reset()
    {
        if(m_Bus != nullptr && m_Handle.IsValid() && !m_Lifetime.expired())
        {
            m_Bus->Unsubscribe(m_Handle);
        }

        m_Bus = nullptr;
        m_Handle = {};
        m_Lifetime.reset();
    }

    Subscription& Subscription::operator=(Subscription&& Other) noexcept
    {
        if(this == &Other)
        {
            return *this;
        }

        Reset();
        m_Bus = Other.m_Bus;
        m_Handle = std::move(Other.m_Handle);
        m_Lifetime = std::move(Other.m_Lifetime);

        Other.m_Bus = nullptr;
        Other.m_Handle = {};

        return *this;
    }

    bool Subscription::IsValid() const
    {
        return m_Bus != nullptr && m_Handle.IsValid() && !m_Lifetime.expired();
    }
}