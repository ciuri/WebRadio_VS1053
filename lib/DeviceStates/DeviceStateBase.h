#ifndef DEVICE_STATE_BASE
#define DEVICE_STATE_BASE
#include <DTOs.h>

class DeviceStateBase{

public:
    virtual void HandleLoop() = 0;
    virtual void HandleUp() = 0;
    virtual void HandleDown() = 0;
    virtual void HandleLeft() = 0;
    virtual void HandleRight() = 0;
    virtual bool HandleEnter()= 0;
    virtual bool HandleBack() = 0;
};

#endif