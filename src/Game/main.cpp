
#include "System/System.h"
#include "System/Network.h"
#include "Audio/AudioSystem.h"
#include "Rendering/RenderSystem.h"
#include "Rendering/Text/TextFunctions.h"
#include "EventHandler/EventHandler.h"

#include "Camera.h"
#include "Factories.h"
#include "Weapons/WeaponFactory.h"
#include "FontIds.h"
#include "Zones/ZoneManager.h"
#include "Physics/CMFactory.h"
#include "GameConfig.h"


#include "SystemContext.h"
#include "TransformSystem.h"
#include "EntitySystem.h"
#include "Rendering/Sprite/SpriteSystem.h"
#include "Physics/PhysicsSystem.h"
#include "Entity/EntityManager.h"
#include "Entity/EntityLogicSystem.h"
#include "DamageSystem.h"

#include "Entity/ComponentFunctions.h"
#include "Entity/GameComponentFuncs.h"
#include "Entity/EntityLogicFactory.h"

namespace
{
    struct Options
    {
        int x = 0;
        int y = 0;
        int width = 1000;
        int height = 625;
        int start_zone = 4;
        const char* game_config = "res/game_config.json";
    };

    Options ParseCommandline(int argc, char* argv[])
    {
        Options options;

        for(int index = 0; index < argc; ++index)
        {
            const char* arg = argv[index];
            if(strcmp("--position", arg) == 0)
            {
                assert((index + 2) < argc);
                options.x = atoi(argv[++index]);
                options.y = atoi(argv[++index]);

            }
            else if(strcmp("--size", arg) == 0)
            {
                assert((index + 2) < argc);
                options.width = atoi(argv[++index]);
                options.height = atoi(argv[++index]);
            }
            else if(strcmp("--zone", arg) == 0)
            {
                assert((index + 1) < argc);
                options.start_zone = atoi(argv[++index]);
            }
            else if(strcmp("--config", arg) == 0)
            {
                assert((index + 1) < argc);
                options.game_config = argv[++index];
            }
        }

        return options;
    }
}

int main(int argc, char* argv[])
{
    constexpr size_t max_entities = 500;

    const Options options = ParseCommandline(argc, argv);

    game::Config game_config;
    game::LoadConfig(options.game_config, game_config);

    System::Initialize();
    network::Initialize();
    mono::InitializeAudio();

    mono::RenderInitParams render_params;
    render_params.pixels_per_meter = 32.0f;
    mono::InitializeRender(render_params);

    mono::PhysicsInitParams physics_params;
    physics_params.n_bodies = max_entities;
    physics_params.n_circle_shapes = 500;
    physics_params.n_segment_shapes = 500;
    physics_params.n_polygon_shapes = 500;

    mono::PhysicsSystemInitParams physics_system_params;
    physics_system_params.n_bodies = max_entities;
    physics_system_params.n_circle_shapes = max_entities;
    physics_system_params.n_segment_shapes = max_entities;
    physics_system_params.n_polygon_shapes = max_entities;

    mono::PhysicsFactory::Init(physics_params);

    {
        mono::SystemContext system_context;
        mono::EventHandler event_handler;

        EntityManager entity_manager(&system_context);
        game::RegisterGameComponents(entity_manager);
        RegisterSharedComponents(entity_manager);

        game::WeaponFactory weapon_factory(&entity_manager, &system_context);
        game::EntityLogicFactory logic_factory(&system_context, event_handler);

        game::weapon_factory = &weapon_factory;
        game::logic_factory = &logic_factory;
        game::entity_manager = &entity_manager;

        mono::TransformSystem* transform_system = system_context.CreateSystem<mono::TransformSystem>(max_entities);

        system_context.CreateSystem<mono::EntitySystem>(max_entities);
        system_context.CreateSystem<game::EntityLogicSystem>(max_entities);
        system_context.CreateSystem<mono::SpriteSystem>(max_entities, transform_system);
        system_context.CreateSystem<mono::PhysicsSystem>(physics_system_params, transform_system);
        system_context.CreateSystem<game::DamageSystem>(max_entities, &entity_manager);

        const math::Vector window_size = math::Vector(options.width, options.height);
        System::IWindow* window = System::CreateWindow("game", options.x, options.y, window_size.x, window_size.y, false);
        window->SetBackgroundColor(0.7, 0.7, 0.7);
        
        mono::LoadFont(game::FontId::PIXELETTE_SMALL,  "res/pixelette.ttf", 10.0f, 1.0f / 10.0f);
        mono::LoadFont(game::FontId::PIXELETTE_MEDIUM, "res/pixelette.ttf", 10.0f, 1.0f / 5.0f);
        mono::LoadFont(game::FontId::PIXELETTE_LARGE,  "res/pixelette.ttf", 10.0f, 1.0f / 3.0f);
        
        auto camera = std::make_shared<game::Camera>(22, 14, window_size.x, window_size.y, transform_system, event_handler);

        game::ZoneCreationContext zone_context;
        zone_context.event_handler = &event_handler;
        zone_context.game_config = &game_config;
        zone_context.system_context = &system_context;

        game::ZoneManager zone_manager(window, camera, zone_context, options.start_zone);
        zone_manager.Run();

        delete window;
    }

    mono::PhysicsFactory::Shutdown();
    mono::ShutdownRender();
    mono::ShutdownAudio();

    network::Shutdown();
    System::Shutdown();

    return 0;
}

