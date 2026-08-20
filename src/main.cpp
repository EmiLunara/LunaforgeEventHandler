#include <iostream>
#include "EventBus.h"
#include <string>

struct PlayerDamaged
{
    int Damage;
};

struct ItemPickedUp
{
    std::string ItemName;
};

int main()
{
    EventBus Bus;

    Bus.Subscribe<PlayerDamaged>([](const PlayerDamaged& Event)
    {   
        printf("Player took %d damage.\n", Event.Damage);
    });
    Bus.Subscribe<PlayerDamaged>([](const PlayerDamaged& Event)
    {
        printf("Audio: Play Damage Sound.\n");
    });

    Bus.Subscribe<ItemPickedUp>([](const ItemPickedUp& Event)
    {
        printf("Inventory: Picked up %s.\n", Event.ItemName.c_str());    
    });

    Bus.Publish(PlayerDamaged{25});
    Bus.Publish(ItemPickedUp{"Health Potion"});
    
    return 0;
}