#include "wifi_client.h"
#include "data_exchange.h"
#include "print.hpp"
#include "settings.h"

CWiFe::CWiFe(IDExchange id)
    : CBaseDevice(id)
{
    // mThrRecv = new std::thread(&CWiFe::receive, this);
}

// CWiFe::~CWiFe() = default;
CWiFe::~CWiFe()
{
    //delete mThrRecv;
}

bool CWiFe::init()
{
    if(WiFi.isConnected() == false) {

        CSettings* stg = CSettings::getInstance();

        Serial.print("WiFi init...");
        wl_status_t status = WiFi.begin(stg->ssid, stg->pass);
        if(status == WL_CONNECT_FAILED) {
            Serial.println("\terror");
            return false;
        } else
            Serial.println("\tsuccess");

        Serial.print("int client: connect wifi...");
        while(WiFi.isConnected() == false) {
            CPrint::delay_s(500);
            Serial.print(".");
        }
        Serial.println("\tsuccess");

        CSettings::getInstance()->mMacBridge = WiFi.macAddress().c_str();
        CPrint::println("Find device: ", "BRIDGE", " mac: ", CSettings::getInstance()->mMacBridge.c_str());
    }

    return true;
}

void CWiFe::notify()
{
}

bool CWiFe::write(const char* sendData, const int& size)
{
    if(init() != true) {
        CPrint::println("int client: ", "wifi not connected");
        return false;
    }

    if(connect() != true) {
        CPrint::println("int client: ", "not connected");
        return false;
    }

    unsigned sizeSend = mClient.write(sendData, size);
    if(sizeSend != size) {
        CPrint::printMessage("int client: send:", "error", sizeSend);
        return false;
    } else
        CPrint::printMessage("int client: send:", sendData, size);

    CPrint::delay_s(250);

    if(mClient.available()) {

        std::string recvData = mClient.readString().c_str();

        CPrint::printMessage("int client: recv:", recvData.c_str(), size);

        CDataExchange::getInstance()->parseMessage(recvData);
    } else
        CPrint::println("int client: recv: timeout");

    CWiFe::disconnect();

    return true;
}

int CWiFe::receive(char* data, int& size)
{
    if(mClient.available()) {

        return mClient.readBytes(data, size);
    }

    return 0;
}

void CWiFe::receive()
{
    if(mClient.available()) {

        mClient.read();
    }
}

bool CWiFe::connect()
{
    CSettings* stg = CSettings::getInstance();
    IPAddress SERVER(stg->SERV[0], stg->SERV[1], stg->SERV[2], stg->SERV[3]);

    Serial.print("int client: connect server...");
    if(mClient.connect(SERVER, stg->PORT) != true) {
        Serial.println("\terror:\tstatus: " + String(mClient.connected()));
        return false;
    }

    Serial.println("\tsuccess:\tstatus: " + String(mClient.connected()));
    return true;
}

void CWiFe::disconnect()
{
    mClient.stop();
}