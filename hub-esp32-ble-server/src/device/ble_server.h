#pragma once

#include <BLEDevice.h>

#include "base_device.h"
#include "data_exchange.h"
#include "print.hpp"
#include "settings.h"

class CBLEServer : public CBaseDevice
{
private:
    BLEServer* mServer;
    BLEService* mService;
    BLECharacteristic* mCharacteristic;

    class CBLEServerCallbacks : public BLEServerCallbacks
    {
        void onConnect(BLEServer* pServer) override
        {
            if(CSettings::getInstance()->mConnectedClient == false) {

                // CSettings::getInstance()->mConnectedServer = true;
                CPrint::println("\n**********************\nble server: connected\n**********************");
            }
        };

        void onDisconnect(BLEServer* pServer) override
        {
            if(CSettings::getInstance()->mConnectedClient == false) {

                // CSettings::getInstance()->mConnectedServer = false;
                CDataExchange::getInstance()->getDevice(BLE_SERVER)->disconnect();
                CPrint::println("\n**********************\nble server: disconnected\n**********************");
            }
        }
    };

    class CBLECharacteristicCallbacks : public BLECharacteristicCallbacks
    {
        void onWrite(BLECharacteristic* pCharacteristic) override;
    };

    bool init() override;
    void notify() override;
    bool write(const char* message, const int& size) override;
    bool connect() override;
    void disconnect() override;

public:
    CBLEServer(IDExchange id);
    ~CBLEServer();
};