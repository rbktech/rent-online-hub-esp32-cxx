#ifndef SETTINGS_H
#define SETTINGS_H

#include <ArduinoJson.h>
#include <string>

#define EEPROM_SIZE 169
#define EEPROM_ADDRESS_IP_0 0
#define EEPROM_ADDRESS_IP_1 1
#define EEPROM_ADDRESS_IP_2 2
#define EEPROM_ADDRESS_IP_3 3
#define EEPROM_ADDRESS_PORT_0 5
#define EEPROM_ADDRESS_PORT_1 6
#define EEPROM_BRIDGE_STATUS 7
#define EEPROM_ADDRESS_SSID 8
#define EEPROM_ADDRESS_PASS 54
#define EEPROM_ADDRESS_LOCAL 99

#define SIZE_ADDRESS_LOCAL 70
#define SIZE_WIFI_PARAM 45

#define SERVICE_UUID ""
#define CHARACTERISTIC_UUID ""

#define BLE_NAME ""
#define BLE_CONNECT_NAME ""
#define WRITE_SERVICE_UUID ""
#define WRITE_CHARACTERISTIC_UUID ""

enum : uint8_t {
    PROCCESS = 100,

    INIT = 0,
    KEY_SUCCESS,
    EMPTY,
    WAIT_USER,
    START_REGISTRATION,
    ADD_USER, // Если это флаг при включении то сбрасываем в START_REGISTRATION
    REGISTRATION_USER,
    REGISTRATION_SERVER,
    MASTER_MODE,
};

class CSettings
{
private:
    static CSettings* mInstance;
    uint8_t mStatus;

public:
    std::string guid = "";

    int PORT = 0;
    int SERV[4] = { 0, 0, 0, 0 };
    char ssid[SIZE_WIFI_PARAM] = { 0 };            // SSID-имя вашей сети (name)
    char pass[SIZE_WIFI_PARAM] = { 0 };            // пароль вашей сети
    char addressLocal[SIZE_ADDRESS_LOCAL] = { 0 }; // "ул. , д. , кв. "

    std::string mId;
    std::string mMacLock;
    std::string mMacBridge;
    std::string mCodePassword;

    bool mConnectedServer = false;
    bool mConnectedClient = false;

    bool mRestart = false;

    CSettings();
    ~CSettings();

    static CSettings* getInstance();

    void setSetting(const DynamicJsonDocument& doc);

    bool createId();

    // void setStatus(uint8_t status);
    // uint8_t getStatus();
};

#endif // SETTINGS_H