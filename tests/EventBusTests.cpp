#include <EventHandler/EventBus.h>

#include <catch2/catch_test_macros.hpp>

using namespace Lunaforge::Events;

namespace
{
    struct TestEvent
    {
        int Value;
    };

    struct OtherTestEvent
    {
        int Value;
    };
}

TEST_CASE("Publishing an even calls subscriber")
{
    EventBus Bus;

    int RecievedValue = 0;

    auto Subscription = Bus.Subscribe<TestEvent>(
        [&RecievedValue](const TestEvent& Event)
        {
            RecievedValue = Event.Value;
        }
    );

    Bus.Publish(TestEvent{42});

    REQUIRE(RecievedValue == 42);
}

TEST_CASE("Publishing an event calls all subscribers")
{
    EventBus Bus;

    int FirstRecievedValue = 0;
    int SecondRecievedValue = 0;

    auto FirstSubscription = Bus.Subscribe<TestEvent>(
        [&FirstRecievedValue](const TestEvent& Event)
        {
            FirstRecievedValue = Event.Value;
        }
    );

    auto SecondSubscription = Bus.Subscribe<TestEvent>(
        [&SecondRecievedValue](const TestEvent& Event)
        {
            SecondRecievedValue = Event.Value;
        }
    );

    Bus.Publish(TestEvent{42});

    REQUIRE(FirstRecievedValue == 42);
    REQUIRE(SecondRecievedValue == 42);
}

TEST_CASE("Publishing an event only calls subscribers of that event type")
{
    EventBus Bus;

    int TestEventCalls = 0;
    int OtherEventCalls = 0;

    auto FirstSubscription = Bus.Subscribe<TestEvent>(
        [&TestEventCalls](const TestEvent&)
        {
            ++TestEventCalls;
        }
    );

    auto SecondSubscription = Bus.Subscribe<OtherTestEvent>(
        [&OtherEventCalls](const OtherTestEvent&)
        {
            ++OtherEventCalls;
        }
    );

    Bus.Publish(TestEvent{42});

    REQUIRE(TestEventCalls == 1);
    REQUIRE(OtherEventCalls == 0);
}

TEST_CASE("Resetting a subscription unsubscribes it")
{
    EventBus Bus;

    int CallCount = 0;

    auto Subscription = Bus.Subscribe<TestEvent>(
        [&CallCount](const TestEvent&)
        {
            ++CallCount;
        }
    );

    Bus.Publish(TestEvent{42});

    REQUIRE(CallCount == 1);

    Subscription.Reset();

    Bus.Publish(TestEvent{42});

    REQUIRE(CallCount == 1);
    REQUIRE_FALSE(Subscription.IsValid());
}

TEST_CASE("Destroying a subscription automatically unsubscribe it")
{
    EventBus Bus;

    int CallCount = 0;

    {
        auto Subscription = Bus.Subscribe<TestEvent>(
            [&CallCount](const TestEvent&)
            {
                ++CallCount;
            }
        );

        REQUIRE(Subscription.IsValid());

        Bus.Publish(TestEvent{42});

        REQUIRE(CallCount == 1);
    }

    Bus.Publish(TestEvent{42});

    REQUIRE(CallCount == 1);
}

TEST_CASE("A subscription is safe when its EventBus is destroyed first")
{
    Subscription Subscription;

    {
        EventBus Bus;

        Subscription = Bus.Subscribe<TestEvent>([](const TestEvent&) {});

        REQUIRE(Subscription.IsValid());
    }

    REQUIRE_FALSE(Subscription.IsValid());

    Subscription.Reset();

    REQUIRE_FALSE(Subscription.IsValid());
}

TEST_CASE("Moving a subscription transfers ownership")
{
    EventBus Bus;

    int CallCount = 0;

    auto Original = Bus.Subscribe<TestEvent>(
        [&CallCount](const TestEvent&)
        {
            ++CallCount;
        }
    );

    REQUIRE(Original.IsValid());

    Subscription Moved = std::move(Original);

    REQUIRE_FALSE(Original.IsValid());
    REQUIRE(Moved.IsValid());

    Bus.Publish(TestEvent{42});

    REQUIRE(CallCount == 1);
}

TEST_CASE("Move assigning a subscription releases the previous subscription")
{
    EventBus Bus;

    int FirstCallCount = 0;
    int SecondCallCount = 0;

    auto First = Bus.Subscribe<TestEvent>(
        [&FirstCallCount](const TestEvent&)
        {
            ++FirstCallCount;
        }
    );

    auto Second = Bus.Subscribe<TestEvent>(
        [&SecondCallCount](const TestEvent&)
        {
            ++SecondCallCount;
        }
    );

    REQUIRE(First.IsValid());
    REQUIRE(Second.IsValid());

    First = std::move(Second);

    REQUIRE(First.IsValid());
    REQUIRE_FALSE(Second.IsValid());

    Bus.Publish(TestEvent{42});

    REQUIRE(FirstCallCount == 0);
    REQUIRE(SecondCallCount == 1);
}