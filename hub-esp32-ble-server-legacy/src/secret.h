#include "queue/queue.h"
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <queue>

std::queue<int> ss;

#define TIMEOUT 1
#define BLE_NAME ""
#define BLE_CONNECT_NAME ""
#define WRITE_SERVICE_UUID ""
#define WRITE_CHARACTERISTIC_UUID ""

uint8_t gTimeoutBle = 0;
uint8_t gTimeoutInt = 0;

CQueue<uint8_t> gQueueBleServer;
CQueue<uint8_t> gQueueBleClient;
CQueue<uint8_t> gQueueInt;

String gMacLock;
String gMacBridge;
String gId;

#define EEPROM_SIZE 100
#define EEPROM_ADDRESS_IP_0 0
#define EEPROM_ADDRESS_IP_1 1
#define EEPROM_ADDRESS_IP_2 2
#define EEPROM_ADDRESS_IP_3 3
#define EEPROM_ADDRESS_PORT_0 5
#define EEPROM_ADDRESS_PORT_1 6
#define EEPROM_ADDRESS_SSID 7
#define EEPROM_ADDRESS_PASS 52
#define SIZE_WIFI_PARAM 45

int PORT = 0;
int SERV[4] = { 0, 0, 0, 0 };
char ssid[SIZE_WIFI_PARAM] = { 0 }; //  SSID-имя вашей сети (name)
char pass[SIZE_WIFI_PARAM] = { 0 }; // пароль вашей сети

const char BLE_WIFE = 1;
const char WIFE_BLE = 2;

void parseMessage(const char* message)
{
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, message);
    if(error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    const char* val = nullptr;

    val = doc[0]["sss"];
    if(val != nullptr) {
        Serial.println("sss != nullptr");
    } else
        Serial.println("sss == nullptr");

    val = doc[0]["command"];
    if(val != nullptr) {
        Serial.println("command != nullptr");
    } else
        Serial.println("command == nullptr");
}
