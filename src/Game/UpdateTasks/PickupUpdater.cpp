
#include "PickupUpdater.h"
#include "Pickups/Ammo.h"

using namespace game;

PickupUpdater::PickupUpdater(std::vector<Pickup>& pickups, mono::EventHandler& event_handler)
    : m_pickups(pickups)
    , m_event_handler(event_handler)
{ }

void PickupUpdater::doUpdate(unsigned int delta)
{
    CheckPlayerPickups(m_pickups, m_event_handler);
}
