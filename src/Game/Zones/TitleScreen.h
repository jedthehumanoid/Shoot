
#pragma once

#include "ZoneCreationContext.h"
#include "Zone/ZoneBase.h"
#include "EventHandler/EventToken.h"
#include "Events/EventFwd.h"

namespace game
{
    class TitleScreen : public mono::ZoneBase
    {
    public:

        TitleScreen(const ZoneCreationContext& context);
        ~TitleScreen();
        
        bool OnKeyUp(const event::KeyUpEvent& event);

        void OnLoad(mono::ICameraPtr& camera) override;
        int OnUnload() override;

        void Continue();
        void Remote();
        void Quit();

        mono::EventHandler& m_event_handler;
        mono::EventToken<event::KeyUpEvent> m_key_token;
        std::vector<struct MoveActionContext> m_move_contexts;
        int m_exit_zone = 0;
    };
}
