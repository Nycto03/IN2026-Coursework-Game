#include "GameUtil.h"
#include "PowerUpExtraLife.h"
#include "Spaceship.h"
#include <iostream>


// PowerUpExtraLife constructor
PowerUpExtraLife::PowerUpExtraLife() : GameObject("PowerUpExtraLife")
{
    Animation *anim = AnimationManager::GetInstance().GetAnimationByName("animated heart");
    auto sprite = std::make_shared<Sprite>(anim->GetWidth(), anim->GetHeight(), anim);
    sprite->SetLoopAnimation(true);
    SetSprite(sprite);

    SetBoundingShape(std::make_shared<BoundingSphere>(GetThisPtr(), 5.0f));
    float speed = (rand() / (float)RAND_MAX) * 2.0f + 1.0f;
    float angle = (rand() / (float)RAND_MAX) * 2.0f * 3.14159f;
    SetVelocity(GLVector3f(cos(angle) * speed, sin(angle) * speed, 0.0f));
}

bool PowerUpExtraLife::CollisionTest(std::shared_ptr<GameObject> o)
{
    if (o->GetType() != GameObjectType("Spaceship")) return false;
    if (mBoundingShape.get() == NULL) return false;
    if (o->GetBoundingShape().get() == NULL) return false;
    return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void PowerUpExtraLife::OnCollision(const GameObjectList& objects)
{
    for (const auto& obj : objects) {
        if (obj->GetType() == GameObjectType("Spaceship")) {
            std::cout << "Extra Life power-up collected" << std::endl;
            mWorld->FlagForRemoval(GetThisPtr());
            break;
        }
    }
}