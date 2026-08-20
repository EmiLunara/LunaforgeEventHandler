# Lunaforge EventHandler

A lightweight, type-safe C++ event system built for games, game engines, tools, and other modular applications.

Lunaforge EventHandler provides synchronous event dispatch with multiple subscribers, move-only RAII subscriptions, and defined behavior when subscriptions are modified during dispatch.

## Features

* Type-safe event publishing and subscription
* Any C++ type can be used as an event
* Multiple subscribers per event type
* Synchronous event dispatch
* Move-only RAII subscriptions
* Automatic unsubscription when a `Subscription` is destroyed
* Manual unsubscription with `Subscription::Reset()`
* Safe subscription lifetime when the `EventBus` is destroyed first
* Safe subscription and unsubscription during event dispatch
* No inheritance or common event base class required
* Small API with minimal dependencies
* CMake integration
* Unit tested with Catch2

## Requirements

* C++20
* CMake 3.25 or newer

The library itself has no external runtime dependencies.

Catch2 is used for the test suite.

## Basic Usage

Events are ordinary C++ types:

```cpp
struct PlayerDamaged
{
    int Damage;
};
```

Create an event bus and subscribe to an event:

```cpp
#include <EventHandler/EventBus.h>

#include <iostream>

int main()
{
    Lunaforge::Events::EventBus Bus;

    auto Subscription =
        Bus.Subscribe<PlayerDamaged>(
            [](const PlayerDamaged& Event)
            {
                printf("Player took %d damage.\n", Event.Damage);
            });

    Bus.Publish(PlayerDamaged{25});

    return 0;
}
```

Output:

```text
Player took 25 damage.
```

## Multiple Subscribers

Multiple systems can independently listen for the same event:

```cpp
auto UISubscription =
    Bus.Subscribe<PlayerDamaged>(
        [](const PlayerDamaged& Event)
        {
            // Update the health UI.
        });

auto AudioSubscription =
    Bus.Subscribe<PlayerDamaged>(
        [](const PlayerDamaged& Event)
        {
            // Play a damage sound.
        });

Bus.Publish(PlayerDamaged{25});
```

Both subscribers receive the event.

This allows systems to communicate without requiring the publisher to directly know about the receiving systems.

## Subscription Lifetime

Subscriptions use RAII(Resource Acquisition Is Initialization).

When a `Subscription` is destroyed, it automatically unsubscribes itself from the associated `EventBus`.

```cpp
{
    auto Subscription =
        Bus.Subscribe<PlayerDamaged>(
            [](const PlayerDamaged&)
            {
                // Handle event.
            });

    Bus.Publish(PlayerDamaged{25});

} // Automatically unsubscribed here.
```

Subscriptions are moveable but cannot be copied.

```cpp
auto First = Bus.Subscribe<PlayerDamaged>(Callback);

auto Second = std::move(First);
```

After the move, `Second` owns the subscription and `First` is no longer valid.

## Manual Unsubscription

A subscription can be removed before its lifetime ends using `Reset()`:

```cpp
auto Subscription =
    Bus.Subscribe<PlayerDamaged>(Callback);

Subscription.Reset();
```

Its state can be queried with:

```cpp
if (Subscription.IsValid())
{
    // Subscription is active.
}
```

Calling `Reset()` on an invalid subscription is safe.

## EventBus Lifetime

A `Subscription` does not keep its `EventBus` alive.

It is safe for the event bus to be destroyed before its subscriptions:

```cpp
Lunaforge::Events::Subscription Subscription;

{
    Lunaforge::Events::EventBus Bus;

    Subscription =
        Bus.Subscribe<PlayerDamaged>(Callback);

} // EventBus destroyed.

Subscription.Reset(); // Safe.
```

Internally, subscriptions observe the lifetime of their associated event bus without owning it.

## Dispatch Semantics

Event dispatch in v0.1 is **synchronous and immediate**.

Calling:

```cpp
Bus.Publish(Event);
```

invokes eligible subscribers before `Publish()` returns.

### Subscribing During Dispatch

Subscribers added while an event is currently being dispatched do **not** receive that event.

They begin receiving events on the next call to `Publish()`.

### Unsubscribing During Dispatch

A subscriber can safely unsubscribe itself during its callback.

If one subscriber removes another subscriber before that subscriber has been invoked, the removed subscriber will **not** receive the current event.

These behaviors are explicitly defined and tested.

## CMake Integration

Link the library against your target:

```cmake
target_link_libraries(MyGame
    PRIVATE
        Lunaforge::EventHandler
)
```

Then include the public API:

```cpp
#include <EventHandler/EventBus.h>
```

### Adding the Repository Directly

When Lunaforge EventHandler exists as part of another source tree, it can be added using:

```cmake
add_subdirectory(path/to/EventHandler)

target_link_libraries(MyGame
    PRIVATE
        Lunaforge::EventHandler
)
```

## Project Structure

```text
ProjectEventHandler/
├── include/
│   └── EventHandler/
│       ├── EventBus.h
│       ├── Subscription.h
│       └── SubscriptionHandle.h
│
├── src/
│   └── Subscription.cpp
│
├── examples/
│   └── BasicExample.cpp
│
├── tests/
│   └── EventBusTests.cpp
│
├── CMakeLists.txt
└── README.md
```

## Design

Lunaforge EventHandler uses `std::type_index` to associate subscribers with event types.

Callbacks are internally type-erased, allowing subscribers for arbitrary event types to be stored by the same event bus while preserving a type-safe public API:

```cpp
Bus.Subscribe<PlayerDamaged>(...);
Bus.Subscribe<ItemPickedUp>(...);

Bus.Publish(PlayerDamaged{25});
Bus.Publish(ItemPickedUp{"Health Potion"});
```

Users of the library never need to interact with the internal type-erased representation.

## Testing

The test suite uses Catch2.

The current tests cover:

* Publishing to a subscriber
* Multiple subscribers
* Isolation between event types
* Manual subscription reset
* RAII unsubscription
* EventBus destruction before subscription destruction
* Subscription move construction
* Subscription move assignment
* Self-unsubscription during dispatch
* Subscription during dispatch
* Removing another subscriber during dispatch

Tests can be run using CTest:

```bash
ctest --output-on-failure
```

## Current Limitations

Version 0.1 intentionally focuses on a small synchronous event system.

The following are **not currently supported**:

* Thread-safe publishing or subscription
* Asynchronous dispatch
* Queued events
* Subscriber priorities
* Event cancellation or consumption
* Custom allocators
* Performance-oriented subscriber storage

These features may be considered in future versions where they provide a clear use case.

## Version

Current development target:

**Lunaforge EventHandler v0.1.0**

The goal of v0.1.0 is to provide a small, predictable, and reusable synchronous event system suitable for use as a foundation in other Lunaforge projects.
