#ifndef __ASTEROID_H__
#define __ASTEROID_H__

#include "GameObject.h"
#include <memory>
#include "BoundingSphere.h"

class Asteroid : public GameObject {
public:
    // Default ctor: creates a large asteroid
    Asteroid();

    // Creates a large (isLarge=true) or small (isLarge=false) asteroid
    Asteroid(bool isLarge);

    virtual ~Asteroid();

    void Init();

    bool CollisionTest(std::shared_ptr<GameObject> o) override;
    void OnCollision(const GameObjectList& objects) override;

private:
    bool mIsLarge;                // track large vs small
    void CreateSmallAsteroids();  // spawn smaller pieces


    // Radii for bounding spheres
    static constexpr float LARGE_RADIUS = 20.0f;
    static constexpr float SMALL_RADIUS = 10.0f;
};

#endif