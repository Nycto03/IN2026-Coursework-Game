#include <stdlib.h>
#include "GameUtil.h"
#include "Asteroid.h"
#include "BoundingShape.h"

Asteroid::Asteroid(void) : GameObject("Asteroid")
{
	mAngle = rand() % 360;
	mRotation = 0; // rand() % 90;
	mPosition.x = rand() / 2;
	mPosition.y = rand() / 2;
	mPosition.z = 0.0;
	mVelocity.x = 10.0 * cos(DEG2RAD*mAngle);
	mVelocity.y = 10.0 * sin(DEG2RAD*mAngle);
	mVelocity.z = 0.0;
}

Asteroid::~Asteroid(void)
{
}

bool Asteroid::CollisionTest(shared_ptr<GameObject> o)
{
	if (GetType() == o->GetType()) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void Asteroid::OnCollision(const GameObjectList& objects)
{
    for (auto& obj : objects) {
        // Check if the collided object is an asteroid
        if (obj->GetType() == GameObjectType("Asteroid")) {
            Asteroid* other = static_cast<Asteroid*>(obj.get());
            // Only handle collision if this asteroid's pointer is less than the other's
            if (this < other) {
                // Calculate the normal vector (from this to other)
                GLVector3f n = (other->GetPosition() - mPosition);
                n.normalize();
                // Get current velocities
                GLVector3f vA = mVelocity;
                GLVector3f vB = other->GetVelocity();
                // Calculate the dot product for the velocity difference
                float dot_prod = (vA - vB).dot(n);
                // Compute new velocities
                GLVector3f vA_new = vA - dot_prod * n;
                GLVector3f vB_new = vB + dot_prod * n; 
                // Update velocities
                SetVelocity(vA_new);
                other->SetVelocity(vB_new);
            }
        }
        else {
            // For now, remove asteroid on collision with non-asteroids (e.g., spaceship, bullet)
            mWorld->FlagForRemoval(GetThisPtr());
        }
    }
}
