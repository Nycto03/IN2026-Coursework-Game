#ifndef __POWERUPBULLETSIZEBOOST_H__
#define __POWERUPBULLETSIZEBOOST_H__

#include "GameObject.h"
#include "BoundingSphere.h"

class PowerUpBulletSizeBoost : public GameObject{
public:
    PowerUpBulletSizeBoost();
    virtual ~PowerUpBulletSizeBoost() {}

    bool CollisionTest(std::shared_ptr<GameObject> o) override;
    void OnCollision(const GameObjectList& objects) override;
};

#endif
