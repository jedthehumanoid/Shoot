
#pragma once

#include "IGameSystem.h"
#include "MonoFwd.h"
#include "Math/Quad.h"

#include "Physics/PhysicsFwd.h"
#include "TriggerTypes.h"
#include "Util/ActiveVector.h"

#include <cstdint>
#include <vector>
#include <array>
#include <unordered_map>
#include <functional>
#include <memory>

namespace game
{
    struct ShapeTriggerComponent
    {
        uint32_t trigger_hash_enter;
        uint32_t trigger_hash_exit;
        uint32_t collision_mask;
        std::unique_ptr<mono::ICollisionHandler> shape_trigger_handler;
    };

    struct DestroyedTriggerComponent
    {
        uint32_t trigger_hash;
        shared::DestroyedTriggerType trigger_type;

        // Internal data
        uint32_t callback_id;
    };

    struct AreaEntityTriggerComponent
    {
        uint32_t trigger_hash;
        math::Quad world_bb;
        uint32_t faction;
        int n_entities;
        shared::AreaTriggerOperation operation;
    };

    struct TimeTriggerComponent
    {
        uint32_t trigger_hash;
        float timeout_ms;
        float timeout_counter_ms;
        bool repeating;
    };

    struct CounterTriggerComponent
    {
        uint32_t listen_trigger_hash;
        uint32_t completed_trigger_hash;
        bool reset_on_completed;
        int count;

        // Internal data
        uint32_t callback_id;
        int counter;
    };

    struct SetConditionTriggerComponent
    {
        uint32_t trigger_hash;
        uint32_t condition_hash;
        bool condition_state;

        // Internal data
        uint32_t callback_id;
    };

    using TriggerCallback = std::function<void (uint32_t trigger_id)>;

    class TriggerSystem : public mono::IGameSystem
    {
    public:

        TriggerSystem(
            size_t n_triggers,
            class DamageSystem* damage_system,
            class ConditionSystem* condition_system,
            mono::PhysicsSystem* physics_system,
            mono::IEntityManager* entity_system);

        ShapeTriggerComponent* AllocateShapeTrigger(uint32_t entity_id);
        void ReleaseShapeTrigger(uint32_t entity_id);
        void AddShapeTrigger(
            uint32_t entity_id, uint32_t trigger_hash_enter, uint32_t trigger_hash_exit, uint32_t collision_mask);

        DestroyedTriggerComponent* AllocateDestroyedTrigger(uint32_t entity_id);
        void ReleaseDestroyedTrigger(uint32_t entity_id);
        void AddDestroyedTrigger(uint32_t entity_id, uint32_t trigger_hash, shared::DestroyedTriggerType type);

        AreaEntityTriggerComponent* AllocateAreaTrigger(uint32_t entity_id);
        void ReleaseAreaTrigger(uint32_t entity_id);
        void AddAreaEntityTrigger(
            uint32_t entity_id, uint32_t trigger_hash, const math::Quad& world_bb, uint32_t faction, shared::AreaTriggerOperation operation, int n_entities);

        TimeTriggerComponent* AllocateTimeTrigger(uint32_t entity_id);
        void ReleaseTimeTrigger(uint32_t entity_id);
        void AddTimeTrigger(uint32_t entity_id, uint32_t trigger_hash, float timeout_ms, bool repeating);

        CounterTriggerComponent* AllocateCounterTrigger(uint32_t entity_id);
        void ReleaseCounterTrigger(uint32_t entity_id);
        void AddCounterTrigger(
            uint32_t entity_id, uint32_t listener_hash, uint32_t completed_hash, int count, bool reset_on_completed);

        SetConditionTriggerComponent* AllocateSetConditionTrigger(uint32_t entity_id);
        void ReleaseSetConditionTrigger(uint32_t entity_id);
        void AddSetConditionTrigger(uint32_t entity_id, uint32_t trigger_hash, uint32_t condition_hash, bool new_condition_state);

        [[nodiscard]]
        uint32_t RegisterTriggerCallback(uint32_t trigger_hash, TriggerCallback callback, uint32_t debug_entity_id);
        void RemoveTriggerCallback(uint32_t trigger_hash, uint32_t callback_id, uint32_t debug_entity_id);

        const std::unordered_map<uint32_t, std::vector<uint32_t>>& GetTriggerTargets() const;

        void EmitTrigger(uint32_t trigger_hash);

        template<typename T>
        void ForEachShapeTrigger(T&& callable)
        {
            m_shape_triggers.ForEach(callable);
        }

        template<typename T>
        void ForEachDestroyedTrigger(T&& callable)
        {
            m_destroyed_triggers.ForEach(callable);
        }

        template<typename T>
        void ForEachAreaTrigger(T&& callable)
        {
            m_area_triggers.ForEach(callable);
        }

        template<typename T>
        void ForEachTimeTrigger(T&& callable)
        {
            m_time_triggers.ForEach(callable);
        }

        template<typename T>
        void ForEachCounterTrigger(T&& callable)
        {
            m_counter_triggers.ForEach(callable);
        }

        template<typename T>
        void ForEachSetCondition(T&& callable)
        {
            m_set_condition_triggers.ForEach(callable);
        }

        uint32_t Id() const override;
        const char* Name() const override;
        void Update(const mono::UpdateContext& update_context) override;

    private:

        void UpdateAreaEntityTriggers(const mono::UpdateContext& update_context);
        void UpdateTimeTriggers(const mono::UpdateContext& update_context);

        class DamageSystem* m_damage_system;
        class ConditionSystem* m_condition_system;
        mono::PhysicsSystem* m_physics_system;
        mono::IEntityManager* m_entity_system;

        using TriggerCallbacks = std::array<TriggerCallback, 8>;
        std::unordered_map<uint32_t, TriggerCallbacks> m_trigger_callbacks;

        mono::ActiveVector<ShapeTriggerComponent> m_shape_triggers;
        mono::ActiveVector<DestroyedTriggerComponent> m_destroyed_triggers;
        mono::ActiveVector<AreaEntityTriggerComponent> m_area_triggers;
        mono::ActiveVector<TimeTriggerComponent> m_time_triggers;
        mono::ActiveVector<CounterTriggerComponent> m_counter_triggers;
        mono::ActiveVector<SetConditionTriggerComponent> m_set_condition_triggers;

        uint32_t m_area_trigger_timer;

        std::vector<uint32_t> m_triggers_to_emit;

        // Debug data
        std::unordered_map<uint32_t, std::vector<uint32_t>> m_entity_id_to_trigger_hashes;
    };
}
