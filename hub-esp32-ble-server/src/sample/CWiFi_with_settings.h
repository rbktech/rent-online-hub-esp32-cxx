#include <WiFi.h>

#include "settings.h"

#include <ArduinoJson.h>
#include <EEPROM.h>

class CWiFi
{
private:
    bool mExpected = false;
    WiFiClient* mClient = nullptr;

    void createClient()
    {
        mExpected = true;

        deleteClient();

        mClient = new WiFiClient();

        mExpected = false;
    }

    void connectClient()
    {
        mExpected = true;

        CSettings* stg = CSettings::getInstance();

        IPAddress SERVER(stg->SERV[0], stg->SERV[1], stg->SERV[2], stg->SERV[3]);

        Serial.print("int client: connect server...");
        if(mClient->connect(SERVER, stg->PORT) == true)
            Serial.println("\tsuccess:\tstatus: " + String(mClient->connected()));
        else
            Serial.println("\terror:\tstatus: " + String(mClient->connected()));

        mExpected = false;
    }

    void disconnectClient()
    {
        Serial.print("int client: disconnect...");
        mClient->stop();
        Serial.println("\tsuccess");
    }

    /** WiFi */

    void connectWiFi()
    {
        mExpected = true;

        deleteClient();

        Serial.print("int client: connect wifi...");
        while(!statusWiFi()) {
            delay(500);
            Serial.print(".");
        }
        Serial.println("\tsuccess");

        mExpected = false;
    }

    int statusWiFi()
    {
        if(WiFiClass::status() == WL_CONNECTED)
            return true;
        return false;
    }

    void disconnectWiFi()
    {
        Serial.println("int client: disconnect WiFi...");
        WiFi.disconnect();
        Serial.println("\tsuccess");
    }

    void deleteClient()
    {
        Serial.print("int client: delete...");
        if(mClient != nullptr) {
            delete mClient;
            mClient = nullptr;
            Serial.println("\tsuccess");
        } else
            Serial.println("\tnull");
    }

public:
    CWiFi()
    {
        CSettings* stg = CSettings::getInstance();

        Serial.print("WiFi init...");
        WiFi.begin(stg->ssid, stg->pass);
        stg->mMacBridge = std::string(WiFi.macAddress().c_str());
        Serial.print("\tmac: " + String(stg->mMacBridge.data()));
        Serial.println("\tsuccess");
    }

    ~CWiFi()
    {
        deleteClient();
    };

    /** Internet */

    void dataExchange(void (CWiFi::*func)())
    {
        if(mExpected == false) {
            if(statusWiFi() == true) {
                if(mClient != nullptr) {

                    connectClient();

                    // if (mClient->connected() == 1) {

                    sendData();
                    delay(250);
                    recvData();

                    //} else
                    // connectClient();
                    // (this->*func)();

                    disconnectClient();

                } else
                    createClient();
            } else
                connectWiFi();
        }
    }

    void recvData()
    {
        if(mClient->available()) {

            String message = mClient->readString();

            Serial.println("int client: recv: size: " + String(message.length()) + " message: " + message);

            DynamicJsonDocument doc(1024);
            DeserializationError error = deserializeJson(doc, message);
            if(error) {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.f_str());
                return;
            }

            String item_int = doc["INT"];
            Serial.println("int client: INT: " + item_int);
            if(item_int != "SUCCESS")
                return;

            String item_ble_serv = doc["BLE_SERV"];
            Serial.println("int client: BLE_SERV: " + item_ble_serv);
            if(item_ble_serv != "null")
                gQueueBleServer.push((uint8_t*)&item_ble_serv[0], item_ble_serv.length());

            String item_ble = doc["BLE"];
            Serial.println("int client: BLE: " + item_ble);
            if(item_ble != "null")
                gQueueBleClient.push((uint8_t*)&item_ble[0], item_ble.length());

            unsigned char* ch = gQueueInt.pop();
            delete ch;

            gTimeoutInt = 0;
        }
    }

    void sendData()
    {
        gTimeoutInt++;
        const char* message = (char*)gQueueInt.top();

        Serial.print("int client: send: ");
        int size = mClient->println(message);
        Serial.println("size: " + String(size) + " message: " + String(message));
    }

    /** Old */

    /*String recvData()
      {
      String message;

      if (mClient2->available()) {
        message = mClient2->readString();
        Serial.print("\t" + message);
        Serial.println("\tsuccess");
      }

      return message;
      }*/

    /*void sendMessage(TDataExchange* data)
      {
      WiFiClient* mClient = new WiFiClient();
      if (mClient == nullptr)
        return;

      IPAddress SERVER(SERV[0], SERV[1], SERV[2], SERV[3]);

      Serial.print("connect to server...");
      if (mClient->connect(SERVER, PORT) == false) {
        Serial.println("\terror");
        return;
      }
      Serial.println("\tsuccess");

      String message;
      DynamicJsonDocument doc(1024);

      doc[0]["command"] = "new";
      doc[0]["addressBridge"] = WiFi.macAddress();
      doc[0]["addressLock"] = gMacLock;
      doc[0]["key"] = gContain;

      serializeJson(doc, message);*/

    /*message += "[{";
      message += "\"command\":\"new\",";
      message += "\"addressBridge\":\"" + WiFi.macAddress() + "\"" + ",";
      message += "\"addressLock\":\"" + gMacLock + "\"" + ",";
      message += "\"key\":\"" + gContain + "\"";
      message += "}]";*/

    /*Serial.print("send message...");
      mClient->println(message);
      Serial.println("\tsuccess");

      Serial.print("wait message...");
      while (1) {

      Serial.write('.');
      if (mClient->available()) {
        message = mClient->readString();
        Serial.print("\t" + message);
        Serial.println("\tsuccess");
        break;
      }

      delay(1000);
      }

      mClient->stop();
      delete mClient;

      //        DynamicJsonDocument doc(1024);
      doc.clear();
      DeserializationError error = deserializeJson(doc, &message[0]);
      if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
      }

      const char* val = nullptr;

      val = doc[0]["command"];
      data->command = String(val);
      Serial.println("command: " + data->command);
      if (data->command == "null")
      return;

      val = doc[0]["result"];
      data->result = String(val);
      Serial.println("result: " + data->result);
      if (data->result == "null")
      return;

      Serial.print("Message1: ");
      for (int i = 0;; i++) {

      uint8_t b = doc[0]["data1"][i];
      if (b == 0)
        break;

      data->message1[i] = b;
      Serial.print(data->message1[i]);
      }
      Serial.println();

      Serial.print("Message2: ");
      for (int i = 0;; i++) {

      uint8_t b = doc[0]["data2"][i];
      if (b == 0)
        break;

      data->message2[i] = b;
      Serial.print(data->message2[i]);
      }
      Serial.println();

      // Serial.println("Message:" + String((char*)data->message));
      }*/
};
