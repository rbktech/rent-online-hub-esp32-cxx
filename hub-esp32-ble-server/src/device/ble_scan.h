#pragma once

#include <BLEDevice.h>
#include <algorithm>

#include "print.hpp"
#include "settings.h"

class CBLEScan
{
private:
    static CBLEScan* mInstance;

    BLEAdvertisedDevice* mBLEAdvertisedDevice;

    class CBLEAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
    {
        std::string toUpper(std::string address);
        void onResult(BLEAdvertisedDevice advertisedDevice) override;
    };

public:
    CBLEScan();
    ~CBLEScan();

    static CBLEScan* getInstance();

    void start();

    BLEAdvertisedDevice* getDevice();
};