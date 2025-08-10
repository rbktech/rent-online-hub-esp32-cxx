#include <Arduino.h>

#include "CBluetoothLe.h"

CWiFi* gWiFi = nullptr;
CBluetoothLe* gBle = nullptr;

void setup()
{
    Serial.begin(115200);

    EEPROM.begin(EEPROM_SIZE);

    PORT = EEPROM.read(EEPROM_ADDRESS_PORT_0);
    PORT |= EEPROM.read(EEPROM_ADDRESS_PORT_1) << 8;
    SERV[0] = EEPROM.read(EEPROM_ADDRESS_IP_0);
    SERV[1] = EEPROM.read(EEPROM_ADDRESS_IP_1);
    SERV[2] = EEPROM.read(EEPROM_ADDRESS_IP_2);
    SERV[3] = EEPROM.read(EEPROM_ADDRESS_IP_3);

    for(int i = 0; i < SIZE_WIFI_PARAM; i++)
        ssid[i] = EEPROM.read(EEPROM_ADDRESS_SSID + i);

    for(int i = 0; i < SIZE_WIFI_PARAM; i++)
        pass[i] = EEPROM.read(EEPROM_ADDRESS_PASS + i);

    while(!Serial) {
        ; // Ожидание подключения последовательного порта. Нужно только для Leonardo
    }

    delay(2000);

    Serial.println(PORT);
    Serial.print(SERV[0]);
    Serial.print(" ");
    Serial.print(SERV[1]);
    Serial.print(" ");
    Serial.print(SERV[2]);
    Serial.print(" ");
    Serial.println(SERV[3]);
    Serial.println(ssid);
    Serial.println(pass);

    gWiFi = new CWiFi();

    gBle = new CBluetoothLe();
    gBle->createServer();
}

bool gLockSettings = true;

void loop()
{
    gControl.init();

    if(gLockSettings == true) {
        if(gControl.settings() == true) {
            gBle->setMode(SERVER_OFF);
            gLockSettings = false;

            gBle->scan();
        }
    }

    if(gBle->getMode() == SERVER_OFF)
        gControl.check();

    if(gQueueInt.available()) {
        gWiFi->dataExchange(&CWiFi::sendData);

        if(gTimeoutInt == TIMEOUT) {
            unsigned char* ch = gQueueInt.pop();
            delete ch;
            gTimeoutInt = 0;
        }
    }

    if(gBle->getMode() == SERVER_ON) {
        if(gBle->getConnectedServer() == false)
            gBle->connectServer();
    }

    if(gQueueBleServer.available())
        gBle->commandServer();

    // if (gQueueBleServerSend.available())
    // gBle->serverSend();

    if(gQueueBleClient.available() && gBle->getConnectedServer() == false) {

        gBle->sendData();

        if(gTimeoutBle == TIMEOUT) {
            unsigned char* ch = gQueueBleClient.pop();
            delete ch;
            gTimeoutBle = 0;
        }
    }

    delay(100);
}
