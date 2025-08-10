#include "settings.h"

#include "print.hpp"
#include <EEPROM.h>

CSettings::CSettings()
{
    EEPROM.begin(EEPROM_SIZE);

    PORT = EEPROM.read(EEPROM_ADDRESS_PORT_0);
    PORT |= EEPROM.read(EEPROM_ADDRESS_PORT_1) << 8;
    SERV[0] = EEPROM.read(EEPROM_ADDRESS_IP_0);
    SERV[1] = EEPROM.read(EEPROM_ADDRESS_IP_1);
    SERV[2] = EEPROM.read(EEPROM_ADDRESS_IP_2);
    SERV[3] = EEPROM.read(EEPROM_ADDRESS_IP_3);
    mStatus = EEPROM.read(EEPROM_BRIDGE_STATUS);

    for(int i = 0; i < SIZE_WIFI_PARAM; i++)
        ssid[i] = (char)EEPROM.read(EEPROM_ADDRESS_SSID + i);

    for(int i = 0; i < SIZE_WIFI_PARAM; i++)
        pass[i] = (char)EEPROM.read(EEPROM_ADDRESS_PASS + i);

    for(int i = 0; i < SIZE_ADDRESS_LOCAL; i++)
        addressLocal[i] = (char)EEPROM.read(EEPROM_ADDRESS_LOCAL + i);

    // PORT = 21432; // java
    PORT = 31249; //

    SERV[0] = 0;
    SERV[1] = 0;
    SERV[2] = 0;
    SERV[3] = 0;

    /*SERV[0] = 0;
    SERV[1] = 0;
    SERV[2] = 0;
    SERV[3] = 0;*/

    memset(ssid, 0, SIZE_WIFI_PARAM);

    memset(pass, 0, SIZE_WIFI_PARAM);

    //memset(addressLocal, 0, SIZE_ADDRESS_LOCAL);
    //memcpy(addressLocal, "petrograd\0", 10);
}

CSettings::~CSettings()
{
}

CSettings* CSettings::getInstance()
{
    if(mInstance == nullptr)
        mInstance = new CSettings();

    return mInstance;
}

CSettings* CSettings::mInstance = nullptr;

void CSettings::setSetting(const DynamicJsonDocument& doc)
{
    EEPROM.begin(EEPROM_SIZE);
    for(int i = 0; i <= EEPROM_SIZE; i++)
        EEPROM.write(i, 0);
    EEPROM.commit();

    String item_ip_address_00 = doc[0]["ip_address_00"];
    Serial.println("ble server: ip_address_00: " + item_ip_address_00);
    if(item_ip_address_00 != "null") {
        SERV[0] = item_ip_address_00.toInt();
        EEPROM.write(EEPROM_ADDRESS_IP_0, SERV[0]);
    }

    String item_ip_address_01 = doc[0]["ip_address_01"];
    Serial.println("ble server: ip_address_01: " + item_ip_address_01);
    if(item_ip_address_01 != "null") {
        SERV[1] = item_ip_address_01.toInt();
        EEPROM.write(EEPROM_ADDRESS_IP_1, SERV[1]);
    }

    String item_ip_address_02 = doc[0]["ip_address_02"];
    Serial.println("ble server: ip_address_02: " + item_ip_address_02);
    if(item_ip_address_02 != "null") {
        SERV[2] = item_ip_address_02.toInt();
        EEPROM.write(EEPROM_ADDRESS_IP_2, SERV[2]);
    }

    String item_ip_address_03 = doc[0]["ip_address_03"];
    Serial.println("ble server: ip_address_03: " + item_ip_address_03);
    if(item_ip_address_03 != "null") {
        SERV[3] = item_ip_address_03.toInt();
        EEPROM.write(EEPROM_ADDRESS_IP_3, SERV[3]);
    }

    String item_port = doc[0]["port"];
    Serial.println("ble server: port: " + item_port);
    if(item_port != "null") {
        PORT = item_port.toInt();
        EEPROM.write(EEPROM_ADDRESS_PORT_0, PORT);
        EEPROM.write(EEPROM_ADDRESS_PORT_1, PORT >> 8);
    }

    String item_wifi_name = doc[0]["wifi_name"];
    Serial.println("ble server: wifi_name: " + item_wifi_name);
    if(item_wifi_name != "null") {

        memset(ssid, 0, SIZE_WIFI_PARAM);

        unsigned size = item_wifi_name.length();
        if(size < SIZE_WIFI_PARAM)
            for(int i = 0; i < size; i++)
                ssid[i] = item_wifi_name[i];

        for(int i = 0; i < SIZE_WIFI_PARAM; i++)
            EEPROM.write(EEPROM_ADDRESS_SSID + i, ssid[i]);
    }

    String item_wifi_password = doc[0]["wifi_password"];
    Serial.println("ble server: wifi_password: " + item_wifi_password);
    if(item_wifi_password != "null") {

        memset(pass, 0, SIZE_WIFI_PARAM);

        unsigned size = item_wifi_password.length();
        if(size < SIZE_WIFI_PARAM)
            for(int i = 0; i < size; i++)
                pass[i] = item_wifi_password[i];

        for(int i = 0; i < SIZE_WIFI_PARAM; i++)
            EEPROM.write(EEPROM_ADDRESS_PASS + i, pass[i]);
    }

    String address_local = doc[0]["address_local"];
    Serial.println("ble server: address_local: " + address_local);
    if(address_local != "null") {

        memset(addressLocal, 0, SIZE_ADDRESS_LOCAL);

        unsigned size = address_local.length();
        if(size < SIZE_ADDRESS_LOCAL)
            for(int i = 0; i < size; i++)
                addressLocal[i] = address_local[i];

        for(int i = 0; i < SIZE_ADDRESS_LOCAL; i++)
            EEPROM.write(EEPROM_ADDRESS_LOCAL + i, addressLocal[i]);
    }

    EEPROM.commit();
}

bool CSettings::createId()
{
    if(mMacLock.empty()) {
        CPrint::print("Mac address lock is emprty");
        return false;
    }

    if(mMacBridge.empty()) {
        CPrint::print("Mac address bridge is emprty");
        return false;
    }

    for(auto& p : mMacLock)
        if(p != ':')
            mId += p;

    for(auto& p : mMacBridge)
        if(p != ':')
            mId += p;

    CPrint::println("ID: ", mId.c_str());
    return true;
}

/*void CSettings::setStatus(uint8_t status)
{
    if(mStatus != status) {
        mStatus = status;
        EEPROM.write(EEPROM_BRIDGE_STATUS, status);
        EEPROM.commit();
    }
}

uint8_t CSettings::getStatus()
{
    return mStatus;
}*/