
#pragma once

#include "IGameSystem.h"
#include "MonoFwd.h"
#include "EventHandler/EventToken.h"
#include "Math/Vector.h"
#include <vector>
#include <array>

#include "Rendering/IDrawable.h"

namespace mono
{
    class TransformSystem;
}

namespace game
{
    class ClientManager;

    class PositionPredictionSystem : public mono::IGameSystem
    {
    public:
        
        PositionPredictionSystem(
            size_t num_records,
            const ClientManager* client_manager,
            mono::TransformSystem* transform_system,
            mono::EventHandler* event_handler);
        ~PositionPredictionSystem();

        uint32_t Id() const override;
        const char* Name() const override;
        uint32_t Capacity() const override;
        void Update(const mono::UpdateContext& update_context) override;

        bool HandlePredicitonMessage(const struct TransformMessage& transform_message);

        const ClientManager* m_client_manager;
        mono::TransformSystem* m_transform_system;
        mono::EventHandler* m_event_handler;
        mono::EventToken<TransformMessage> m_transform_token;

        struct RemoteTransform
        {
            uint32_t timestamp;
            math::Vector position;
            float rotation;
        };

        struct PredictionData
        {
            math::Vector predicted_position;
            std::array<RemoteTransform, 8> prediction_buffer;
        };

        std::vector<PredictionData> m_prediction_data;
    };


    class PredictionSystemDebugDrawer : public mono::IDrawable
    {
    public:

        PredictionSystemDebugDrawer(const PositionPredictionSystem* prediction_system);
        void doDraw(mono::IRenderer& renderer) const;
        math::Quad BoundingBox() const;

        const PositionPredictionSystem* m_prediction_system;
    };
}