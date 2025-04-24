#include <stdlib.h>
#include <cmath>
#include <memory>
#include "GameUtil.h"
#include "Asteroid.h"
#include "BoundingShape.h"
#include "Spaceship.h"
#include "GameWorld.h"


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
    return mBoundingShape->CollisionTest(o->GetBoundingShape());

}

void Asteroid::OnCollision(const GameObjectList& objects) {
    for (auto& obj : objects) {
        GameObjectType type = obj->GetType();

        //Asteroid - Asteroid collision = bounce
        if (obj->GetType() == GameObjectType("Asteroid")) {
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


        //Asteroid - Bullet collision, if large split the asteroid in 2, if small destroy (WIP)
        else if (obj->GetType() == GameObjectType("Bullet")) {
            if (mIsLarge) {
                CreateSmallAsteroids();
            }
            // always remove this asteroid
            mWorld->FlagForRemoval(GetThisPtr());
            // remove the bullet itself
            mWorld->FlagForRemoval(obj);
            return;
        }

        //Asteroid - Spaceship collision, if large asteroid destroy both, if small asteroid bounce (WIP)
        else if (obj->GetType() == GameObjectType("Spaceship")) {
            auto ship = std::static_pointer_cast<Spaceship>(obj);

            /*
            if (mIsLarge) {
                mWorld->FlagForRemoval(GetThisPtr());
            }
            else {
                GLVector3f n = (ship->GetPosition() - mPosition);
                n.normalize();
                GLVector3f vA = mVelocity, vS = ship->GetVelocity();
                float rel = (vA - vS).dot(n);
                if (rel > 0.0f) {
                    SetVelocity(vA - n * rel);
                    ship->SetVelocity(vS + n * rel);
                }
            }*/



            // destroy asteroid
            mWorld->FlagForRemoval(GetThisPtr());
            // if small, bounce; if large, just kill ship
            if (!mIsLarge) {
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
            }
            // destroy the ship
            mWorld->FlagForRemoval(obj);
            return;
        }
    }
}


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
