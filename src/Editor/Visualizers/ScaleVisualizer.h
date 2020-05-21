
#pragma once

#include "Rendering/IDrawable.h"

namespace editor
{
    class ScaleVisualizer : public mono::IDrawable
    {
    public:

        virtual void Draw(mono::IRenderer& renderer) const;
        virtual math::Quad BoundingBox() const;
    };
}
