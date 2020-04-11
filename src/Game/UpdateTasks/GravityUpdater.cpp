
#include "GravityUpdater.h"
#include "Zone/IEntity.h"
#include "Math/Vector.h"
#include "Physics/IBody.h"

#include <cmath>
#include <functional>

using namespace game;

GravityUpdater::GravityUpdater(mono::IEntity* moon1, mono::IEntity* moon2)
    : mMoon1(moon1)
    , mMoon2(moon2)
    , mElapsedTime(0)
{ }

void GravityUpdater::doUpdate(const mono::UpdateContext& update_context)
{
    using namespace std::placeholders;

    mElapsedTime += update_context.delta_ms;
    if(mElapsedTime < 16)
        return;
    
    //mZone->ForEachBody(std::bind(&GravityUpdater::GravityFunc, this, _1));
    mElapsedTime = 0;
}

void GravityUpdater::GravityFunc(mono::IBody* body)
{
    math::Vector impulse;
    
    math::Vector newPos = body->GetPosition() - mMoon1->Position();
    const float distance = math::Length(newPos);
    if(distance < 300.0f)
    {
        const float gravity = 1e4f;
        const float value = -gravity / (distance * std::sqrt(distance));
    
        math::Normalize(newPos);
        newPos *= value;
        
        impulse += newPos;
    }
    
    math::Vector newPos2 = body->GetPosition() - mMoon2->Position();
    const float distance2 = math::Length(newPos2);
    if(distance2 < 200.0f)
    {
        const float gravity = 1e4f;
        const float value = -gravity / (distance2 * std::sqrt(distance2));
        
        math::Normalize(newPos2);
        newPos2 *= value;
        
        impulse += newPos2;
    }
    
    body->ApplyImpulse(impulse, body->GetPosition());
}

