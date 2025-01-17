
#pragma once

#include "MonoFwd.h"
#include "Math/Vector.h"
#include "StateMachine.h"
#include "System/Audio.h"

#include "Entity/IEntityLogic.h"
#include "PlayerGamepadController.h"
#include "Weapons/WeaponTypes.h"
#include "Weapons/IWeaponFactory.h"

#include "PickupTypes.h"

#include <memory>

namespace game
{
    struct PlayerInfo;

    enum ItemSlotIndex
    {
        LEFT,
        RIGHT,
        N_SLOTS
    };

    class PlayerLogic : public IEntityLogic
    {
    public:

        PlayerLogic(
            uint32_t entity_id,
            PlayerInfo* player_info,
            mono::EventHandler* event_handler,
            const System::ControllerState& controller,
            mono::SystemContext* system_context);

        ~PlayerLogic();

        void Update(const mono::UpdateContext& update_context) override;
        void UpdateAnimation(float aim_direction, const math::Vector& player_velocity);
        void UpdateWeaponAnimation(const mono::UpdateContext& update_context);

        void Fire();
        void StopFire();
        void Reload(uint32_t timestamp);
        void UseItemSlot(ItemSlotIndex slot_index);
        void HandlePickup(shared::PickupType type, int amount);

        void Throw(float throw_force);
        void ThrowAction();
        void PickupDrop();
        bool HoldingPickup() const; 

        void TriggerInteraction();

        void SelectWeapon(WeaponSetup weapon);
        void SelectSecondaryWeapon(WeaponSetup weapon);
        void HandleWeaponPickup(shared::PickupType type);
        
        void MoveInDirection(const math::Vector& direction);
        void ApplyImpulse(const math::Vector& force);
        void ApplyForce(const math::Vector& force);
        void SetVelocity(const math::Vector& velocity);
        void ResetForces();

        void SetAimDirection(float aim_direction);
        void Blink(const math::Vector& direction);

        void ToDefault();
        void DefaultState(const mono::UpdateContext& update_context);

        void ToDead();
        void DeadState(const mono::UpdateContext& update_context);

        void ToBlink();
        void BlinkState(const mono::UpdateContext& update_context);

        const uint32_t m_entity_id;
        const uint32_t m_controller_id;
        PlayerInfo* m_player_info;
        PlayerGamepadController m_gamepad_controller;

        enum class PlayerStates
        {
            DEFAULT,
            DEAD,
            BLINK
        };

        using PlayerStateMachine = StateMachine<PlayerStates, const mono::UpdateContext&>;
        PlayerStateMachine m_state;

        uint32_t m_weapon_entity;

        bool m_fire;
        int m_total_ammo_left;
        WeaponSetup m_weapon_type;
        IWeaponPtr m_weapon;
        float m_aim_direction;
        float m_aim_target;

        int m_idle_anim_id;
        int m_run_anim_id;

        uint32_t m_blink_counter;
        math::Vector m_blink_direction;

        std::unique_ptr<class TrailEffect> m_trail_effect;
        std::unique_ptr<class BlinkEffect> m_blink_effect;

        audio::ISoundPtr m_blink_sound;

        mono::TransformSystem* m_transform_system;
        mono::PhysicsSystem* m_physics_system;
        mono::SpriteSystem* m_sprite_system;
        mono::IEntityManager* m_entity_system;
        class DamageSystem* m_damage_system;
        class PickupSystem* m_pickup_system;
        class InteractionSystem* m_interaction_system;

        struct ItemSlot
        {};
        ItemSlot m_item_slots[ItemSlotIndex::N_SLOTS];

        uint32_t m_picked_up_id;
        mono::IConstraint* m_pickup_constraint;
    };
}
