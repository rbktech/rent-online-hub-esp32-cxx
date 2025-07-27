#pragma once

#include "data_exchange.h"
#include "device/base_device.h"
#include "print.hpp"
#include "settings.h"
#include <BLEDevice.h>

class CBLEClient : public CBaseDevice
{
private:
    BLEClient* mClient;
    BLERemoteCharacteristic* mCharacteristicWrite;

    static std::string mKey;

    class CBLEClientCallbacks : public BLEClientCallbacks
    {
        void onConnect(BLEClient* pclient) override
        {
            CPrint::println("\n**********************\nble client: connected\n**********************");
        }

        void onDisconnect(BLEClient* pclient) override
        {
            CDataExchange::getInstance()->getDevice(BLE_CLIENT)->disconnect();
            CPrint::println("\n**********************\nble client: disconnected\n**********************");
        }
    };

    CBLEClientCallbacks* mCallbacks = new CBLEClientCallbacks();

    static void notifyCallback(BLERemoteCharacteristic* pCharacteristic, uint8_t* data, size_t length, bool isNotify);
    void discoveredServices();

    bool send(const char* message, const int& size);

    void setNotify(BLERemoteCharacteristic* characteristics)
    {
        CPrint::printAddress("notifyCallback: ", &CBLEClient::notifyCallback);
        CSettings::getInstance()->mRestart = true;

        if(mClient == nullptr)
            CPrint::println("mClient == nullptr");

        characteristics->registerForNotify(&CBLEClient::notifyCallback);
        CSettings::getInstance()->mRestart = false;
    }

    bool init() override;
    void notify() override;
    bool write(const char* message, const int& size) override;
    bool connect() override;
    void disconnect() override;

public:
    CBLEClient(IDExchange id);
    ~CBLEClient();
};