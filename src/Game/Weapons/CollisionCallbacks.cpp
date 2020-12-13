
#include "CollisionCallbacks.h"

#include "DamageSystem.h"
#include "Effects/DamageEffect.h"
#include "Effects/ImpactEffect.h"

#include "EntitySystem/IEntityManager.h"
#include "Math/MathFwd.h"
#include "Math/MathFunctions.h"
#include "Particle/ParticleSystem.h"
#include "Physics/IBody.h"
#include "Physics/PhysicsSystem.h"
#include "Rendering/Sprite/SpriteSystem.h"
#include "Rendering/Sprite/Sprite.h"
#include "SystemContext.h"
#include "TransformSystem/TransformSystem.h"

namespace
{
    game::DamageEffect* g_damage_effect = nullptr;
    game::ImpactEffect* g_impact_effect = nullptr;
}

void game::InitWeaponCallbacks(mono::SystemContext* system_context)
{
    mono::ParticleSystem* particle_system = system_context->GetSystem<mono::ParticleSystem>();
    g_damage_effect = new game::DamageEffect(particle_system);
    g_impact_effect = new game::ImpactEffect(particle_system);
}

void game::CleanupWeaponCallbacks()
{
    delete g_damage_effect;
    g_damage_effect = nullptr;

    delete g_impact_effect;
    g_impact_effect = nullptr;
}

void game::SpawnEntityWithAnimation(
    const char* entity_file,
    int animation_id,
    uint32_t position_at_transform_id,
    mono::IEntityManager* entity_manager,
    mono::TransformSystem* transform_system,
    mono::SpriteSystem* sprite_system)
{
    const mono::Entity spawned_entity = entity_manager->CreateEntity(entity_file);
    math::Matrix& entity_transform = transform_system->GetTransform(spawned_entity.id);
    entity_transform = transform_system->GetWorld(position_at_transform_id);

    mono::Sprite* spawned_entity_sprite = sprite_system->GetSprite(spawned_entity.id);

    const auto remove_entity_callback = [spawned_entity, entity_manager]() {
        entity_manager->ReleaseEntity(spawned_entity.id);
    };
    spawned_entity_sprite->SetAnimation(animation_id, remove_entity_callback);
}

void game::StandardCollision(
    uint32_t entity_id,
    uint32_t owner_entity_id,
    game::BulletCollisionFlag flags,
    const mono::IBody* other,
    const math::Vector& collision_point,
    mono::IEntityManager* entity_manager,
    game::DamageSystem* damage_system,
    mono::PhysicsSystem* physics_system,
    mono::TransformSystem* transform_system)
{
    bool did_damage = false;

    const uint32_t other_entity_id = physics_system->GetIdFromBody(other);
    if(other_entity_id != std::numeric_limits<uint32_t>::max() && flags & game::BulletCollisionFlag::APPLY_DAMAGE)
    {
        const DamageResult result = damage_system->ApplyDamage(other_entity_id, 20, owner_entity_id);
        did_damage = result.did_damage;
    }

    if(other)
    {
        const math::Vector world_position = math::GetPosition(transform_system->GetWorld(entity_id));
        const float direction = math::AngleFromVector(world_position - collision_point);

        if(did_damage)
            g_damage_effect->EmitGibsAt(collision_point, direction);
        else
            g_impact_effect->EmittAt(collision_point, direction);
    }

    if(flags & game::BulletCollisionFlag::DESTROY_THIS)
        entity_manager->ReleaseEntity(entity_id);
}

void game::RocketCollision(
    uint32_t entity_id,
    uint32_t owner_entity_id,
    game::BulletCollisionFlag flags, 
    const mono::IBody* other,
    const math::Vector& collision_point,
    mono::IEntityManager* entity_manager,
    game::DamageSystem* damage_system,
    mono::PhysicsSystem* physics_system,
    mono::SpriteSystem* sprite_system,
    mono::TransformSystem* transform_system)
{
    StandardCollision(entity_id, owner_entity_id, flags, other, collision_point, entity_manager, damage_system, physics_system, transform_system);
    //SpawnEntityWithAnimation("res/entities/explosion.entity", 0, entity_id, entity_manager, transform_system, sprite_system);
    //event_handler.DispatchEvent(game::ShockwaveEvent(explosion_config.position, 150));
}

void game::CacoPlasmaCollision(
    uint32_t entity_id,
    uint32_t owner_entity_id,
    game::BulletCollisionFlag flags,
    const mono::IBody* other,
    const math::Vector& collision_point,
    mono::IEntityManager* entity_manager,
    game::DamageSystem* damage_system,
    mono::PhysicsSystem* physics_system,
    mono::SpriteSystem* sprite_system,
    mono::TransformSystem* transform_system)
{
    StandardCollision(entity_id, owner_entity_id, flags, other, collision_point, entity_manager, damage_system, physics_system, transform_system);
    SpawnEntityWithAnimation("res/entities/caco_explosion.entity", 0, entity_id, entity_manager, transform_system, sprite_system);
}
