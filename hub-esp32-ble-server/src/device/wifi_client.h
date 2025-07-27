#pragma once

#include "device/base_device.h"
#include <WiFi.h>

// #include <thread>

class CWiFe : public CBaseDevice
{
private:
    // std::thread* mThrRecv;
    WiFiClient mClient;

    bool init() override;
    void notify() override;
    bool write(const char* message, const int& size) override;
    bool connect() override;

public:
    CWiFe(IDExchange id);
    ~CWiFe();

    int receive(char* data, int& size);
    void receive();

    void disconnect() override;
};