#include <EventHandler/EventBus.h>

#include <iostream>
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
    Lunaforge::Events::EventBus Bus;

    auto UISubscription = Bus.Subscribe<PlayerDamaged>([](const PlayerDamaged& Event)
    {
        printf("[UI] Player took %d damage.\n", Event.Damage);
    });
    
    auto AudioSubscription = Bus.Subscribe<PlayerDamaged>([](const PlayerDamaged&)
    {
        printf("[Audio] Play damage sound.\n");
    });

    auto InventorySubscription = Bus.Subscribe<ItemPickedUp>([](const ItemPickedUp& Event)
    {
        printf("[Inventory] Picked up %s.\n", Event.ItemName.c_str());
    });
    
    Bus.Publish(PlayerDamaged{25});
    Bus.Publish(ItemPickedUp{"Health Potion"});

    return 0;
}