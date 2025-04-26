/** A listener interface for power-up events to update the GUI. */

#ifndef __IPOWERUPLISTENER_H__
#define __IPOWERUPLISTENER_H__

#include <string>

class IPowerUpListener
{
public:
    virtual void OnPowerUpActivated(const std::string& powerUp, int duration) = 0;
    virtual void OnPowerUpDeactivated(const std::string& powerUp) = 0;
};

#endif