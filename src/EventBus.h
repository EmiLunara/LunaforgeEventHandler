#pragma once

#include <functional>
#include <utility>
#include <vector>
#include <typeindex>
#include <unordered_map>

class EventBus
{
public:
    template<typename TEvent>
    using Callback = std::function<void(const TEvent&)>;

    template<typename TEvent>
    void Subscribe(Callback<TEvent> CallbackFunction)
    {
        auto Wrapper = 
            [CallbackFunction = std::move(CallbackFunction)](const void* Event)
            {
                CallbackFunction(*static_cast<const TEvent*>(Event));
            };
            
        m_Subscribers[typeid(TEvent)].push_back(std::move(Wrapper));
    }

    template<typename TEvent>
    void Publish(const TEvent& Event)
    {
        const auto it = m_Subscribers.find(typeid(TEvent));

        if(it == m_Subscribers.end())
        {
            return;
        }

        for(const auto& CallbackFunction : it->second)
        {
            CallbackFunction(&Event);
        }
    }

private:
    using ErasedCallback = std::function<void(const void*)>;

    std::unordered_map<std::type_index, std::vector<ErasedCallback>> m_Subscribers;
};