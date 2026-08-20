#pragma once

#include <cstdint>
#include <typeindex>

namespace Lunaforge::Events
{
    struct SubscriptionHandle
    {
        std::type_index EventType{typeid(void)};
        std::uint64_t Id = 0;

        [[nodiscard]]
        bool IsValid() const
        {
            return Id != 0;
        }
    };
}