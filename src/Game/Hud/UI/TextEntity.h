
#pragma once

#include "FontIds.h"
#include "Zone/EntityBase.h"
#include "Rendering/Color.h"

#include <string>

namespace game
{
    class TextEntity : public mono::EntityBase
    {
    public:

        TextEntity(const std::string& text, FontId font, bool centered);

        void EntityDraw(mono::IRenderer& renderer) const override;
        void EntityUpdate(const mono::UpdateContext& update_context) override;

        const std::string m_text;
        const FontId m_font;
        const bool m_centered;

        mono::Color::RGBA m_text_color;
        mono::Color::RGBA m_shadow_color;
    };
}
