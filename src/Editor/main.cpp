
#include "System/System.h"
#include "System/UID.h"
#include "Camera/Camera.h"
#include "Rendering/RenderSystem.h"
#include "Rendering/Text/TextSystem.h"
#include "EventHandler/EventHandler.h"
#include "Engine.h"
#include "SystemContext.h"

#include "Editor.h"
#include "EditorConfig.h"
#include "FontIds.h"

#include "TransformSystem/TransformSystem.h"
#include "EntitySystem/EntitySystem.h"
#include "EntitySystem/EntityManager.h"
#include "Rendering/Sprite/SpriteSystem.h"

#include "Component.h"
#include "Entity/ComponentFunctions.h"
#include "Entity/LoadEntity.h"

int main()
{
    constexpr size_t max_entities = 500;

    System::InitializeContext init_context;
    init_context.working_directory = ".";
    System::Initialize(init_context);
    System::SetUIDOffset(max_entities +1);

    mono::RenderInitParams render_params;
    render_params.pixels_per_meter = 32.0f;
    mono::InitializeRender(render_params);

    {
        mono::EventHandler event_handler;
        mono::SystemContext system_context;

        mono::TransformSystem* transform_system = system_context.CreateSystem<mono::TransformSystem>(max_entities);
        mono::EntitySystem* entity_system = system_context.CreateSystem<mono::EntitySystem>(max_entities);
        system_context.CreateSystem<mono::SpriteSystem>(max_entities, transform_system);
        system_context.CreateSystem<mono::TextSystem>(max_entities, transform_system);

        mono::EntityManager entity_manager(entity_system, &system_context, shared::LoadEntityFile, ComponentNameFromHash);
        shared::RegisterSharedComponents(entity_manager);

        editor::Config config;
        editor::LoadConfig("res/editor_config.json", config);
        
        System::IWindow* window = System::CreateWindow(
            "editor", config.window_position.x, config.window_position.y, config.window_size.x, config.window_size.y, System::WindowOptions::NONE);

        mono::Camera camera;

        shared::LoadFonts();

        auto editor = std::make_unique<editor::Editor>(window, entity_manager, event_handler, system_context, config);
        mono::Engine(window, &camera, &system_context, &event_handler).Run(editor.get());

        const System::Position& position = window->Position();
        const System::Size& size = window->Size();
        config.window_position = math::Vector(position.x, position.y);
        config.window_size = math::Vector(size.width, size.height);

        editor::SaveConfig("res/editor_config.json", config);

        system_context.DestroySystems();
    
        delete window;
    }

    mono::ShutdownRender();
    System::Shutdown();

    return 0;
}


