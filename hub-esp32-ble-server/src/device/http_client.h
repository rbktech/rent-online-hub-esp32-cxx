#pragma once

#include "base_device.h"
#include <WiFi.h>
#include <HTTPClient.h>

struct SWiFeParam {
    const std::string ssid;
    const std::string password;
};

struct SAddressParam {
    const std::string address;
    const uint16_t port = 0;
};

class CWiFe : public CBaseDevice
{
private:
    std::string mBuffer;

    uint8_t mCurrentWiFi;
    uint8_t mCurrentAddress;

    std::vector<SWiFeParam*> mWiFeParams;
    std::vector<SAddressParam*> mAddressParams;

    HTTPClient mHttpClient;

    SAddressParam* getCurrentAddress(const uint8_t& position) {

        if(position < mAddressParams.size()) {
            return mAddressParams.at(position);
        }

        return nullptr;
    }

    SWiFeParam* getCurrentWiFi(const uint8_t& position) {

        if(position < mWiFeParams.size()) {
            return mWiFeParams.at(position);
        }

        return nullptr;
    }

public:
    CWiFe(IDExchange id);
    ~CWiFe();

    uint8_t init() override;
    uint8_t connect() override { return 0; };
    uint8_t disconnect() override { return 0; };
    uint8_t send(const char* data, const int& size) override { return 0; };
    uint8_t recv(const char* data, const int& size) override;

    void addWiFe(const char* ssid, const char* password) {
        mWiFeParams.emplace_back(new SWiFeParam{ ssid, password });
    }

    void addAddress(const char* address, const uint16_t& port) {
        mAddressParams.emplace_back(new SAddressParam{ address, port });
    }

    void setCurrentWiFe(const uint8_t& position) {
        mCurrentWiFi = position;
    }

    void setCurrentAddress(const uint8_t& position) {
        mCurrentAddress = position;
    }

    uint8_t notify() override;
    uint8_t receive(char* data, int& size);
    uint8_t receive();

    const char* getMacAddress();

    uint8_t isConnectedEth();
    uint8_t isConnectedWiFi();

    uint8_t dataExchange(uint8_t* message, const size_t& size);
};
