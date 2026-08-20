#include <EventHandler/EventBus.h>
#include <iostream>

struct PlayerDamaged
{
    int Damage;
};

int main()
{
    Lunaforge::Events::EventBus Bus;

    auto UI = Bus.Subscribe<PlayerDamaged>([](const PlayerDamaged& Event)
        {
            printf("Player took %d damage.\n", Event.Damage);
        });
    
    Bus.Publish(PlayerDamaged{25});
    
    return 0;
}