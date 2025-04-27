#include "GameUtil.h"
#include "GameWorld.h"
#include "Bullet.h"
#include "Spaceship.h"
#include "BoundingSphere.h"
#include "Asteroid.h"
#include "PowerUpExtraLife.h"
#include "PowerUpInvulnerability.h"
#include "PowerUpBulletSizeBoost.h"

using namespace std;

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////






//Updated Spaceship code
/** Default constructor. */
Spaceship::Spaceship()
    : GameObject("Spaceship"), mThrust(0), mIsInvulnerable(false), mInvulnerabilityTimer(0), mOriginalBoundingShape(nullptr), mBulletRadius(2.0f)
{
}

/** Construct a spaceship with given position, velocity, acceleration, angle, and rotation. */
Spaceship::Spaceship(GLVector3f p, GLVector3f v, GLVector3f a, GLfloat h, GLfloat r)
    : GameObject("Spaceship", p, v, a, h, r), mThrust(0), mIsInvulnerable(false), mInvulnerabilityTimer(0), mOriginalBoundingShape(nullptr)
{
}

/** Copy constructor. */
Spaceship::Spaceship(const Spaceship& s)
    : GameObject(s), mThrust(0), mIsInvulnerable(s.mIsInvulnerable), mInvulnerabilityTimer(s.mInvulnerabilityTimer), mBulletRadius(s.mBulletRadius), mOriginalBoundingShape(s.mOriginalBoundingShape)
{
}

/** Destructor. */
Spaceship::~Spaceship(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Reset the spaceship to initial state with invulnerability. */
void Spaceship::Reset()
{
    mPosition.x = 0.0f;
    mPosition.y = 0.0f;
    mPosition.z = 0.0f;
    mVelocity.x = 0.0f;
    mVelocity.y = 0.0f;
    mVelocity.z = 0.0f;
    mAngle = 0.0f;
    mRotation = 0.0f;
    mThrust = 0.0f;

    // Store the original bounding shape if not already stored
    if (mBoundingShape && !mOriginalBoundingShape) {
        mOriginalBoundingShape = mBoundingShape;
    }

    // Start invulnerability
    mBoundingShape = nullptr;
    mIsInvulnerable = true;
    mInvulnerabilityTimer = 3000; // 3 seconds
}

/** Update this spaceship. */
void Spaceship::Update(int t)
{
    if (mIsInvulnerable) {
        mInvulnerabilityTimer -= t;
        if (mInvulnerabilityTimer <= 0 && mOriginalBoundingShape) {
            mBoundingShape = mOriginalBoundingShape;
            mIsInvulnerable = false;
            FirePowerUpDeactivated("Invulnerability");
        }
    }
    // Call parent update function
    GameObject::Update(t);
}

/** Render this spaceship. */
void Spaceship::Render(void)
{
    if (mSpaceshipShape.get() != NULL) mSpaceshipShape->Render();

    // If ship is thrusting
    if ((mThrust > 0) && (mThrusterShape.get() != NULL)) {
        mThrusterShape->Render();
    }

    GameObject::Render();
}

/** Fire the rockets. */
void Spaceship::Thrust(float t)
{
    mThrust = t;
    // Increase acceleration in the direction of ship
    mAcceleration.x = mThrust * cos(DEG2RAD * mAngle);
    mAcceleration.y = mThrust * sin(DEG2RAD * mAngle);
}

/** Set the rotation. */
void Spaceship::Rotate(float r)
{
    mRotation = r;
}

/** Shoot a bullet. */
void Spaceship::Shoot(void)
{
    // Check the world exists
    if (!mWorld) return;
    // Construct a unit length vector in the direction the spaceship is headed
    GLVector3f spaceship_heading(cos(DEG2RAD * mAngle), sin(DEG2RAD * mAngle), 0);
    spaceship_heading.normalize();
    // Calculate the point at the nose of the spaceship from position and heading
    GLVector3f bullet_position = mPosition + (spaceship_heading * 4);
    // Calculate how fast the bullet should travel
    float bullet_speed = 30;
    // Construct a vector for the bullet's velocity
    GLVector3f bullet_velocity = mVelocity + spaceship_heading * bullet_speed;
    // Construct a new bullet
    shared_ptr<GameObject> bullet
    (new Bullet(bullet_position, bullet_velocity, mAcceleration, mAngle, 0, 2000));
    bullet->SetBoundingShape(make_shared<BoundingSphere>(bullet->GetThisPtr(), mBulletRadius));
    bullet->SetShape(mBulletShape);
    // Add the new bullet to the game world
    mWorld->AddObject(bullet);
}

bool Spaceship::CollisionTest(shared_ptr<GameObject> o)
{


    if (mIsInvulnerable) return false;
    if (o->GetType() != GameObjectType("Asteroid") &&
        o->GetType() != GameObjectType("PowerUpExtraLife") &&
        o->GetType() != GameObjectType("PowerUpInvulnerability") &&
        o->GetType() != GameObjectType("PowerUpBulletSizeBoost")) return false;
    
    
    //if (o->GetType() != GameObjectType("Asteroid")) return false;
    if (mBoundingShape.get() == NULL) return false;
    if (o->GetBoundingShape().get() == NULL) return false;
    return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void Spaceship::OnCollision(const GameObjectList& objects) {
    for (const auto& obj : objects) {
        GameObjectType objType = obj->GetType();
        if (objType == GameObjectType("Asteroid")) {
            auto asteroid = std::static_pointer_cast<Asteroid>(obj);  
            if (asteroid->IsLarge()) {                               
                mWorld->FlagForRemoval(GetThisPtr());
            }
        }
        else if (objType == GameObjectType("PowerUpExtraLife")) {
            FirePowerUpActivated("ExtraLife", 0);
        }
        else if (objType == GameObjectType("PowerUpInvulnerability")) {
            ActivateInvulnerability(10000); // 10 seconds
        }
        else if (objType == GameObjectType("PowerUpBulletSizeBoost")) {
            BoostBulletSize(1.5f); // Increase radius by 50%
        }
    }
}


//Original code, kept for debugging

/*
* 
* 
* 
* 
*
**  Default constructor. *
Spaceship::Spaceship()
	: GameObject("Spaceship"), mThrust(0)
{
}

/** Construct a spaceship with given position, velocity, acceleration, angle, and rotation. *
Spaceship::Spaceship(GLVector3f p, GLVector3f v, GLVector3f a, GLfloat h, GLfloat r)
	: GameObject("Spaceship", p, v, a, h, r), mThrust(0)
{
}

/** Copy constructor. *
Spaceship::Spaceship(const Spaceship& s)
	: GameObject(s), mThrust(0)
{
}

/** Destructor. *
Spaceship::~Spaceship(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Update this spaceship. *
void Spaceship::Update(int t)
{
	// Call parent update function
	GameObject::Update(t);
}

/** Render this spaceship. *
void Spaceship::Render(void)
{
	if (mSpaceshipShape.get() != NULL) mSpaceshipShape->Render();

	// If ship is thrusting
	if ((mThrust > 0) && (mThrusterShape.get() != NULL)) {
		mThrusterShape->Render();
	}

	GameObject::Render();
}

/** Fire the rockets. *
void Spaceship::Thrust(float t)
{
	mThrust = t;
	// Increase acceleration in the direction of ship
	mAcceleration.x = mThrust * cos(DEG2RAD * mAngle);
	mAcceleration.y = mThrust * sin(DEG2RAD * mAngle);
}

/** Set the rotation. *
void Spaceship::Rotate(float r)
{
	mRotation = r;
}

/** Shoot a bullet. *
void Spaceship::Shoot(void)
{
	// Check the world exists
	if (!mWorld) return;
	// Construct a unit length vector in the direction the spaceship is headed
	GLVector3f spaceship_heading(cos(DEG2RAD * mAngle), sin(DEG2RAD * mAngle), 0);
	spaceship_heading.normalize();
	// Calculate the point at the node of the spaceship from position and heading
	GLVector3f bullet_position = mPosition + (spaceship_heading * 4);
	// Calculate how fast the bullet should travel
	float bullet_speed = 30;
	// Construct a vector for the bullet's velocity
	GLVector3f bullet_velocity = mVelocity + spaceship_heading * bullet_speed;
	// Construct a new bullet
	shared_ptr<GameObject> bullet
	(new Bullet(bullet_position, bullet_velocity, mAcceleration, mAngle, 0, 2000));
	bullet->SetBoundingShape(make_shared<BoundingSphere>(bullet->GetThisPtr(), 2.0f));
	bullet->SetShape(mBulletShape);
	// Add the new bullet to the game world
	mWorld->AddObject(bullet);

}

bool Spaceship::CollisionTest(shared_ptr<GameObject> o)
{
	if (o->GetType() != GameObjectType("Asteroid")) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void Spaceship::OnCollision(const GameObjectList& objects)
{
	mWorld->FlagForRemoval(GetThisPtr());
}

*/



//Power up methods
void Spaceship::ActivateInvulnerability(int duration)
{
    //utilises the variable I originally made as a spawn invulerability 
    mIsInvulnerable = true;
    mInvulnerabilityTimer = duration;
    mBoundingShape = nullptr;
    FirePowerUpActivated("Invulnerability", duration);
}


void Spaceship::BoostBulletSize(float boost)
{
    mBulletRadius *= boost;
}

void Spaceship::FirePowerUpActivated(const std::string& powerUp, int duration)
{
    for (PowerUpListenerList::iterator lit = mPowerUpListeners.begin(); lit != mPowerUpListeners.end(); ++lit) {
        (*lit)->OnPowerUpActivated(powerUp, duration);
    }
}

void Spaceship::FirePowerUpDeactivated(const std::string& powerUp)
{
    for (PowerUpListenerList::iterator lit = mPowerUpListeners.begin(); lit != mPowerUpListeners.end(); ++lit) {
        (*lit)->OnPowerUpDeactivated(powerUp);
    }
}