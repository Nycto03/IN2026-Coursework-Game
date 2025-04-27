#ifndef __POWERUPEXTRALIFE_H__
#define __POWERUPEXTRALIFE_H__

#include "GameObject.h"
#include "BoundingSphere.h"

class PowerUpExtraLife : public GameObject
{
public:
    PowerUpExtraLife();
    virtual ~PowerUpExtraLife() {}

    bool CollisionTest(std::shared_ptr<GameObject> o) override;
    void OnCollision(const GameObjectList& objects) override;
};

#endif