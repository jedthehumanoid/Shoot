
#include "GridVisualizer.h"
#include "Rendering/IRenderer.h"
#include "Rendering/Color.h"
#include "Rendering/RenderBuffer/BufferFactory.h"
#include "Math/Matrix.h"
#include "Math/Quad.h"

namespace
{
    std::vector<math::Vector> BuildGridVertices(const math::Quad& bounds)
    {
        constexpr float cellSize = 50.0f;

        const float width = bounds.mB.x - bounds.mA.x;
        const float height = bounds.mB.y - bounds.mA.y;

        const float cellsWidth = width / cellSize;
        const float cellsHeight = height / cellSize;

        std::vector<math::Vector> vertices;

        for(int index = 0; index < cellsWidth; ++index)
        {
            const float x = bounds.mA.x + index * cellSize;

            vertices.push_back(math::Vector(x, bounds.mA.y));
            vertices.push_back(math::Vector(x, bounds.mB.y));
        }

        for(int index = 0; index < cellsHeight; ++index)
        {
            const float y = bounds.mA.y + index * cellSize;

            vertices.push_back(math::Vector(bounds.mA.x, y));
            vertices.push_back(math::Vector(bounds.mB.x, y));
        }
        
        return vertices;
    }
}

using namespace editor;

GridVisualizer::GridVisualizer(const bool& draw_grid)
    : m_draw_grid(draw_grid)
{
    const std::vector<math::Vector>& grid_vertices = BuildGridVertices(math::Quad(0, 0, 1200, 800));

    constexpr mono::Color::RGBA gray_color(1.0f, 1.0f, 1.0f, 0.2f);
    const std::vector<mono::Color::RGBA> color(grid_vertices.size(), gray_color);

    m_vertices = mono::CreateRenderBuffer(mono::BufferType::STATIC, mono::BufferData::FLOAT, 2, grid_vertices.size(), grid_vertices.data());
    m_colors = mono::CreateRenderBuffer(mono::BufferType::STATIC, mono::BufferData::FLOAT, 4, color.size(), color.data());
}

void GridVisualizer::Draw(mono::IRenderer& renderer) const
{
    if(!m_draw_grid)
        return;

    const math::Matrix& projection = math::Ortho(0.0f, 1200, 0.0f, 800, -10.0f, 10.0f);
    const mono::ScopedTransform projection_scope = mono::MakeProjectionScope(projection, &renderer);
    const mono::ScopedTransform view_scope = mono::MakeViewTransformScope(math::Matrix(), &renderer);
    const mono::ScopedTransform transform_scope = mono::MakeTransformScope(math::Matrix(), &renderer);

    renderer.DrawLines(m_vertices.get(), m_colors.get(), 0, m_vertices->Size());
}

math::Quad GridVisualizer::BoundingBox() const
{
    return math::InfQuad;
}
