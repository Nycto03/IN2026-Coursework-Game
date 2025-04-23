#ifndef __ASTEROID_H__
#define __ASTEROID_H__

#include "GameObject.h"
#include <memory>

class Asteroid : public GameObject {
public:
    // Default ctor: creates a large asteroid
    Asteroid();

    // Creates a large (isLarge=true) or small (isLarge=false) asteroid
    Asteroid(bool isLarge);

    ~Asteroid();

    bool CollisionTest(std::shared_ptr<GameObject> o) override;
    void OnCollision(const GameObjectList& objects) override;

private:
    bool mIsLarge;                // track large vs small
    void CreateSmallAsteroids();  // spawn smaller pieces
};

#endif