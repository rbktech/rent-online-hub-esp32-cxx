#include "base_device.h"

CBaseDevice::CBaseDevice(IDExchange id)
{
    mId = id;
}

CBaseDevice::~CBaseDevice() = default;

std::string CBaseDevice::getNotifyMessage()
{
    return mNotifyMessage;
}