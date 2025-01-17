
#include "TrackingBehaviour.h"

#include "Navigation/NavMesh.h"

#include "Paths/IPath.h"
#include "Paths/PathFactory.h"

#include "Physics/IBody.h"
#include "Physics/IConstraint.h"
#include "Physics/PhysicsSystem.h"
#include "Rendering/Color.h"

#include "IDebugDrawer.h"
#include "Factories.h"

#include <cassert>
#include <string>

using namespace game;

TrackingBehaviour::TrackingBehaviour(mono::IBody* body, mono::PhysicsSystem* physics_system)
    : m_entity_body(body)
    , m_physics_system(physics_system)
    , m_tracking_position(math::INF, math::INF)
    , m_current_position(0.0f)
    , m_meter_per_second(1.0f)
{
    assert(body->GetType() == mono::BodyType::DYNAMIC);

    m_control_body = m_physics_system->CreateKinematicBody();
    m_control_body->SetPosition(body->GetPosition());
    m_spring = m_physics_system->CreateSpring(m_control_body, body, 0.0f, 200.0f, 10.0f);
}

TrackingBehaviour::~TrackingBehaviour()
{
    m_physics_system->ReleaseConstraint(m_spring);
    m_physics_system->ReleaseKinematicBody(m_control_body);
}

void TrackingBehaviour::SetTrackingSpeed(float meter_per_second)
{
    m_meter_per_second = meter_per_second;
}

TrackingResult TrackingBehaviour::Run(const mono::UpdateContext& update_context, const math::Vector& tracking_position)
{
    const float distance_to_last = math::DistanceBetween(m_tracking_position, tracking_position);
    if(distance_to_last > 1.0f)
    {
        const bool path_updated = UpdatePath(tracking_position);
        if(!path_updated)
            return TrackingResult::NO_PATH;

        m_tracking_position = tracking_position;
    }

    m_current_position += m_meter_per_second * update_context.delta_s;

/*
    {
        g_debug_drawer->DrawLine(m_path->GetPathPoints(), 1.0f, mono::Color::RED);
        g_debug_drawer->DrawPoint(m_path->GetPositionByLength(m_current_position), 4.0f, mono::Color::GREEN);

        char buffer[1024] = {};
        std::sprintf(buffer, "%.2f / %.2f", m_current_position, m_path->Length());
        g_debug_drawer->DrawScreenText(buffer, math::Vector(1, 1), mono::Color::OFF_WHITE);
    }
*/

    if(m_current_position > m_path->Length())
        return TrackingResult::AT_TARGET;

    const math::Vector& path_position = m_path->GetPositionByLength(m_current_position);
    m_control_body->SetPosition(path_position);

    return TrackingResult::TRACKING;
}

bool TrackingBehaviour::UpdatePath(const math::Vector& tracking_position)
{
    if(!g_navmesh)
        return false;

    const math::Vector position = m_entity_body->GetPosition();

    const int start = game::FindClosestIndex(*g_navmesh, position);
    const int end = game::FindClosestIndex(*g_navmesh, tracking_position);

    if(start == end || start == -1 || end == -1)
        return false;

    const std::vector<int>& nav_path = game::AStar(*g_navmesh, start, end);
    if(nav_path.empty())
        return false;

    const std::vector<math::Vector>& points = PathToPoints(*g_navmesh, nav_path);
    m_path = mono::CreatePath(points);
    m_current_position = m_path->GetLengthFromPosition(position);

    return true;
}
