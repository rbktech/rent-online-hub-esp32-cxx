#include "wifi_client.h"
#include <iostream>

CWiFe::CWiFe(IDExchange id)
    : CBaseDevice(id)
{
    mCurrentWiFi = 0;
    mCurrentAddress = 0;
}

CWiFe::~CWiFe()
{
    for(auto& p : mWiFeParams)
        delete p;
    for(auto& p : mAddressParams)
        delete p;
}

uint8_t CWiFe::init()
{
    if(WiFi.isConnected() == false) {

        SWiFeParam* param = getCurrentWiFi(mCurrentWiFi);
        if(param != nullptr) {

            std::cout << "WiFi init...";
            wl_status_t status = WiFi.begin(param->ssid.data(), param->password.data());
            if(status == WL_CONNECT_FAILED) {
                std::cout << "\terror" << std::endl;
                return 1;
            } else
                std::cout << "\tsuccess" << std::endl;

            std::cout << "WiFi connect...";
            while(WiFi.isConnected() == false) {
                CBaseDevice::delay(500);
                std::cout << ".";
            }
            std::cout << "\tsuccess" << std::endl;
            return 0;
        }
    } else
        std::cout << "WiFi error:\tstatus: wifi already connected" << std::endl;

    return 1;
}

const char* CWiFe::getMacAddress()
{
    if(WiFi.isConnected() == true)
        return WiFi.macAddress().c_str();

    return nullptr;
}

uint8_t CWiFe::notify()
{
    return 0;
}

uint8_t CWiFe::recv(const char* data, const int& size)
{
    return 0;
}


uint8_t CWiFe::isConnectedWiFi()
{
    return !WiFi.isConnected();
}

uint8_t CWiFe::dataExchange(uint8_t* message, const size_t& size)
{
    int httpCode = 0;
    std::string payload;

    SAddressParam* param = getCurrentAddress(mCurrentAddress);
    if(param != nullptr) {

        std::cout << "eth client: connect server...";
        if(mHttpClient.begin(param->address.c_str(), param->port) == true) {
            std::cout << "\tsuccess" << std::endl;

            mHttpClient.setTimeout(1000);
            std::cout << "eth client: post...";
            httpCode = mHttpClient.POST(message, size);
            if(httpCode > 0) {
                std::cout << "\tsuccess" << std::endl;

                if(httpCode == HTTP_CODE_OK) {
                    payload = mHttpClient.getString().c_str();
                }
            } else {
                std::cout << "\terror" << std::endl;
            }

            mHttpClient.end();

            std::cout << "eth client: message: " << message << " size: " << size << std::endl;
            std::cout << "payload: " << payload << std::endl;
            std::cout << "http code: " << httpCode << std::endl;

            if(payload.empty() == true)
                return 0;

        } else
            std::cout << "\terror" << std::endl;

    } else
        std::cout << "error:\tstatus: address not found" << std::endl;

    return 1;
}