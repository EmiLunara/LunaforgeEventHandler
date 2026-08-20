#pragma once

#include "Subscription.h"
#include "SubscriptionHandle.h"

#include <functional>
#include <utility>
#include <vector>
#include <typeindex>
#include <unordered_map>
#include <cstdint>
#include <memory>

namespace Lunaforge::Events
{
    class EventBus
    {
    public:
        template<typename TEvent>
        using Callback = std::function<void(const TEvent&)>;

        template<typename TEvent>
        Subscription Subscribe(Callback<TEvent> CallbackFunction)
        {
            const std::uint64_t Id = m_NextSubscriptionId++;

            auto Wrapper = 
                [CallbackFunction = std::move(CallbackFunction)](const void* Event)
                {
                    CallbackFunction(*static_cast<const TEvent*>(Event));
                };
                
            m_Subscribers[typeid(TEvent)].push_back(Subscriber{Id, std::move(Wrapper)});

            return Subscription{this, SubscriptionHandle{typeid(TEvent), Id}, m_LifetimeToken};
        }

        template<typename TEvent>
        void Publish(const TEvent& Event)
        {
            const auto it = m_Subscribers.find(typeid(TEvent));

            if(it == m_Subscribers.end())
            {
                return;
            }

            for(const auto& Entry : it->second)
            {
                Entry.Function(&Event);
            }
        }

        void Unsubscribe(const SubscriptionHandle& Handle)
        {
            if(!Handle.IsValid())
            {
                return;
            }

            const auto it = m_Subscribers.find(Handle.EventType);

            if(it == m_Subscribers.end())
            {
                return;
            }

            auto& Subscribers = it->second;
            std::erase_if(Subscribers, [&Handle](const Subscriber& Entry)
            {
                return Entry.Id == Handle.Id;
            });
        }

    private:
        using ErasedCallback = std::function<void(const void*)>;

        struct Subscriber
        {
            std::uint64_t Id;
            ErasedCallback Function;
        };

        struct LifetimeToken
        {
        };
        

        std::unordered_map<std::type_index, std::vector<Subscriber>> m_Subscribers;
        std::uint64_t m_NextSubscriptionId = 1;
        std::shared_ptr<LifetimeToken> m_LifetimeToken = std::make_shared<LifetimeToken>();
    };
}

