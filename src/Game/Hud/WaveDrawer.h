
#pragma once

#include "MonoFwd.h"
#include "EventHandler/EventToken.h"
#include "Rendering/IDrawable.h"

#include <string>

namespace game
{
    class WaveDrawer : public mono::IDrawable
    {
    public:
    
        WaveDrawer(mono::EventHandler& event_handler);
        ~WaveDrawer();

        void doDraw(mono::IRenderer& renderer) const override;
        math::Quad BoundingBox() const override;

        bool WaveStarted(const struct WaveStartedEvent& event);
        bool HordeCompleted(const struct HordeCompletedEvent& event);

    private:
        mono::EventHandler& m_event_handler;
        mono::EventToken<struct WaveStartedEvent> m_wave_started_token;
        mono::EventToken<struct HordeCompletedEvent> m_horde_completed_token;

        std::string m_current_text;
    };
}