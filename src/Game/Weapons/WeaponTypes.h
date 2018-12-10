
#pragma once

namespace game
{
    enum class WeaponType : int
    {
        STANDARD,
        FLAK_CANON,
        ROCKET_LAUNCHER,
        CACOPLASMA,
        GENERIC
    };

    constexpr int N_WEAPON_TYPES = static_cast<int>(WeaponType::GENERIC);

    enum class WeaponFaction
    {
        PLAYER,
        ENEMY
    };
}
