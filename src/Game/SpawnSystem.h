
#pragma once

#include "IGameSystem.h"
#include "Math/Vector.h"
#include "MonoFwd.h"

#include <vector>
#include <string>

namespace game
{
    class TriggerSystem;

    class SpawnSystem : public mono::IGameSystem
    {
    public:

        struct SpawnPoint
        {
            int spawn_score;
            float radius;
            int interval;
            uint32_t properties;

            uint32_t enable_trigger;
            uint32_t disable_trigger;

            // internal data
            bool active;
            uint32_t counter;
            uint32_t enable_callback_id;
            uint32_t disable_callback_id;
        };

        struct SpawnDefinition
        {
            int value;
            std::string entity_file;
        };

        SpawnSystem(uint32_t n, TriggerSystem* trigger_system, mono::IEntityManager* entity_manager, mono::TransformSystem* transform_system);

        SpawnPoint* AllocateSpawnPoint(uint32_t entity_id);
        void ReleaseSpawnPoint(uint32_t entity_id);
        bool IsAllocated(uint32_t entity_id);
        void SetSpawnPointData(uint32_t entity_id, const SpawnPoint& component_data);

        uint32_t Id() const override;
        const char* Name() const override;
        void Update(const mono::UpdateContext& update_context) override;

        game::TriggerSystem* m_trigger_system;
        mono::IEntityManager* m_entity_manager;
        mono::TransformSystem* m_transform_system;
        std::vector<SpawnDefinition> m_spawn_definitions;
        std::vector<SpawnPoint> m_spawn_points;
        std::vector<bool> m_alive;
    };
}
