
#include "ScreenSparkles.h"
#include "Particle/ParticlePool.h"
#include "Particle/ParticleEmitter.h"
#include "Particle/ParticleDrawer.h"
#include "Particle/ParticleSystemDefaults.h"
#include "Rendering/Texture/TextureFactory.h"
#include "Random.h"

#include "Math/MathFunctions.h"
#include "Math/Quad.h"

using namespace game;

namespace
{
    void Generator(const math::Vector& position, mono::ParticlePool& pool, size_t index, const math::Quad& viewport)
    {
        const float half_height = viewport.mB.y / 2.0f;

        const float x = 0.0f;
        const float y = mono::Random(-half_height, half_height);

        const float velocity_x = mono::Random(-10.0f, -5.0f);
        const float velocity_y = mono::Random(-2.0f, 2.0f);

        const int life = mono::RandomInt(0, 500) + 4000;

        pool.m_position[index] = position + math::Vector(x, y);
        pool.m_velocity[index] = math::Vector(velocity_x, velocity_y);
        pool.m_start_color[index] = mono::Color::RGBA(0.8f, 0.8f, 1.0f, 1.0f);
        pool.m_end_color[index] = mono::Color::RGBA(0.5f, 0.5f, 1.0f, 0.5f);
        pool.m_start_size[index] = mono::Random(8.0f, 16.0f);
        pool.m_end_size[index] = mono::Random(2.0f, 6.0f);
        pool.m_start_life[index] = life;
        pool.m_life[index] = life;
    }
}

ScreenSparkles::ScreenSparkles(const math::Quad& viewport)
    : m_viewport(viewport)
{
    const float x = viewport.mB.x;
    const float y = viewport.mB.y / 2.0f;

    m_position = math::Vector(x, y);

    mono::ParticleEmitter::Configuration emit_config;
    emit_config.duration = -1.0f;
    emit_config.emit_rate = 100.0f;
    emit_config.burst = false;
    emit_config.generator = [viewport](const math::Vector& position, mono::ParticlePool& pool, size_t index) {
        Generator(position, pool, index, viewport);
    };

    m_pool = std::make_unique<mono::ParticlePool>(500, mono::DefaultUpdater);
    m_emitter = std::make_unique<mono::ParticleEmitter>(emit_config, m_pool.get());

    const mono::ITexturePtr texture = mono::CreateTexture("res/textures/smoke.png");
    m_drawer = std::make_unique<mono::ParticleDrawer>(texture, *m_pool);
}

ScreenSparkles::~ScreenSparkles()
{ }

void ScreenSparkles::Draw(mono::IRenderer& renderer) const
{
    m_drawer->doDraw(renderer);
}

void ScreenSparkles::Update(unsigned int delta)
{
    m_emitter->doUpdate(delta);
    m_pool->doUpdate(delta);
}