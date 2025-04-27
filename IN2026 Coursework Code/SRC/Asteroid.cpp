#include <stdlib.h>
#include <cmath>
#include <memory>
#include <iostream> // For std::cerr
#include "GameUtil.h"
#include "Asteroid.h"
#include "BoundingShape.h"
#include "Spaceship.h"
#include "GameWorld.h"
#include "AnimationManager.h"
#include "Animation.h"
#include "Sprite.h"


Asteroid::Asteroid()
    : Asteroid(true)
{
}


// Main constructor, sets the size flag, shape, and initializes
Asteroid::Asteroid(bool isLarge) : GameObject("Asteroid"), mIsLarge(isLarge)
{

	mAngle = rand() % 360;
	mRotation = 0; // rand() % 90;
	mPosition.x = rand() / 2;
	mPosition.y = rand() / 2;
	mPosition.z = 0.0;

    // compute speed & velocity
    float speed = 10.0f;
    mVelocity.x = speed * cos(DEG2RAD * mAngle);
    mVelocity.y = speed * sin(DEG2RAD * mAngle);
    mVelocity.z = 0.0f;

}

void Asteroid::Init()
{
    float r = mIsLarge ? LARGE_RADIUS : SMALL_RADIUS;
    mBoundingShape = std::make_shared<BoundingSphere>(GetThisPtr(), r);
    // Set scale for rendering: large = 0.2f, small = 0.1f
    SetScale(mIsLarge ? 0.2f : 0.1f);
}

Asteroid::~Asteroid(void)
{
}

bool Asteroid::CollisionTest(shared_ptr<GameObject> o)
{
	//if (GetType() == o->GetType()) return false;
	/*if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
    */

    if (!mBoundingShape || !o->GetBoundingShape())
        return false;
    //stops the asteroids destroying powerups
    else if (o->GetType() == GameObjectType("PowerUpExtraLife") ||
        o->GetType() == GameObjectType("PowerUpInvulnerability") ||
        o->GetType() == GameObjectType("PowerUpBulletSizeBoost"))
        return false;
    return mBoundingShape->CollisionTest(o->GetBoundingShape());

}

void Asteroid::OnCollision(const GameObjectList& objects) {
    for (auto& obj : objects) {
        GameObjectType type = obj->GetType();

        //Asteroid - Asteroid collision = bounce
        if (type == GameObjectType("Asteroid")) {
            auto other = std::static_pointer_cast<Asteroid>(obj);
            if (this < other.get()) {
                GLVector3f n = (other->GetPosition() - mPosition);
                n.normalize();
                GLVector3f vA = mVelocity, vB = other->GetVelocity();
                float rel = (vA - vB).dot(n);
                if (rel > 0.0f) {
                    SetVelocity(vA - n * rel);
                    other->SetVelocity(vB + n * rel);
                }
            }
        }
        // Asteroid-Bullet collision: split if large, remove asteroid and bullet
        else if (type == GameObjectType("Bullet")) {
            mDestroyedBySpaceship = false;
            mWorld->FlagForRemoval(GetThisPtr());
            //mWorld->FlagForRemoval(obj);
            if (mIsLarge) {
                CreateSmallAsteroids();
            }
        }

        //Asteroid - Spaceship collision, if large asteroid destroy both, if small asteroid bounce (WIP)
        else if (type == GameObjectType("Spaceship")) {
            auto ship = std::static_pointer_cast<Spaceship>(obj);
            if (mIsLarge) {
                //Mark as destroyed by spaceship
                mDestroyedBySpaceship = true; 
                mWorld->FlagForRemoval(GetThisPtr());
                mWorld->FlagForRemoval(obj);
            }
            else {
                GLVector3f delta = ship->GetPosition() - mPosition;
                GLVector3f n = delta;
                n.normalize();
                GLVector3f vA = mVelocity;
                GLVector3f vS = ship->GetVelocity();
                float rel = (vA - vS).dot(n);
                if (rel > 0.0f) {
                    SetVelocity(vA - n * rel);
                    ship->SetVelocity(vS + n * rel);
                }
                // No removal for small asteroids
            }
        }
    }
}



void Asteroid::CreateSmallAsteroids()
{
    const int pieces = 2;
    float speed = std::max(mVelocity.length(), 5.0f); // Ensure minimum speed

    // Get the animation from AnimationManager
    Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
    if (!anim_ptr) {
        std::cerr << "Error: Animation 'asteroid1' not found." << std::endl;
        return;
    }

    // Create a sprite using the animation
    shared_ptr<Sprite> asteroid_sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
    asteroid_sprite->SetLoopAnimation(true);

    for (int i = 0; i < pieces; ++i) {
        auto small = std::make_shared<Asteroid>(false);
        // Slight offset to prevent immediate collisions
        GLVector3f offset = GLVector3f((rand() % 10 - 5) / 10.0f, (rand() % 10 - 5) / 10.0f, 0.0f);
        small->SetPosition(mPosition + offset);
        float angleOff = static_cast<float>(rand() % 360);
        float newAngle = mAngle + angleOff;

        GLVector3f vel(
            speed * cos(DEG2RAD * newAngle),
            speed * sin(DEG2RAD * newAngle),
            0.0f
        );
        small->SetVelocity(vel);
        small->SetSprite(asteroid_sprite);
        small->Init();
        mWorld->AddObject(small);
    }
}
/*
void Asteroid::CreateSmallAsteroids()
{
    // Spawn two smaller asteroids at current position
    const int pieces = 2;
    float speed = mVelocity.length();
    for (int i = 0; i < pieces; ++i) {
        auto small = std::make_shared<Asteroid>(false);
        // place at same spot
        small->SetPosition(mPosition);
        // give them random velocity based on original
        float angleOff = static_cast<float>(rand() % 360);
        float newAngle = mAngle + angleOff;

        GLVector3f vel(
            speed * cos(DEG2RAD * newAngle),
            speed * sin(DEG2RAD * newAngle),
            0.0f
        );
        small->SetVelocity(vel);
        small->Init();
        mWorld->AddObject(small);
    }
}
*/
