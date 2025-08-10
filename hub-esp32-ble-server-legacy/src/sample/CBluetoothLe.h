#include "CWiFi.h"
#include <BLEDevice.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

#define SERVICE_UUID ""
#define CHARACTERISTIC_UUID ""

#define SERVER_ON 0
#define SERVER_OFF 1
// #define MODE_CLIENT 1
// #define MODE_SERVER 2

static BLEUUID writeServiceUUID(WRITE_SERVICE_UUID);
static BLEUUID writeCharacteristicUUID(WRITE_CHARACTERISTIC_UUID);

class CBluetoothLe
{
private:
    int mMode = SERVER_ON;

    bool mExpected = false;
    bool mConnectedServer = false;
    bool mConnectedClient = false;

    BLEClient* mClient = nullptr;
    BLEServer* mServer = nullptr;
    BLEAdvertisedDevice* mDevice = nullptr;
    BLERemoteCharacteristic* mCharacteristic = nullptr;

    /**
       Scan callbacks
    */
    class CBLEAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
    {
    private:
        CBluetoothLe* mBle = nullptr;

        void onResult(BLEAdvertisedDevice advertisedDevice) override
        {

            std::string nameDevice = advertisedDevice.getName().c_str();

            if(nameDevice == BLE_CONNECT_NAME) {

                advertisedDevice.getScan()->stop();

                gMacLock = String(advertisedDevice.getAddress().toString());

                mBle->mDevice = new BLEAdvertisedDevice(advertisedDevice);

                mBle->mExpected = false;

                Serial.println("Find device: " + String(&nameDevice.data()[0]) + " mac: " + gMacLock);

                Serial.println("stop scan...");
            }
        }

    public:
        CBLEAdvertisedDeviceCallbacks(CBluetoothLe* ble)
            : mBle(ble) {};
    };

    /**
       Client callbacks
    */
    class CBLEClientCallbacks : public BLEClientCallbacks
    {
    private:
        CBluetoothLe* mBle = nullptr;

        void onConnect(BLEClient* pClient) override
        {
            mBle->mConnectedClient = true;
            mBle->mConnectedServer = false;
            Serial.println("\n**********************\nble client: connect\n**********************");
        }

        void onDisconnect(BLEClient* pClient) override
        {
            mBle->mConnectedClient = false;
            Serial.println("\n**********************\nble client: disconnect\n**********************");
        }

    public:
        CBLEClientCallbacks(CBluetoothLe* ble)
            : mBle(ble) {};
    };

    void connectClient()
    {
        mExpected = true;

        if(mDevice != nullptr) {

            if(mClient == nullptr) {

                Serial.print("ble client: create...");
                mClient = BLEDevice::createClient();
                mClient->setClientCallbacks(new CBLEClientCallbacks(this));
                Serial.println("\tsuccess");
            }

            Serial.print("ble client: connected...");
            bool result = mClient->connect(mDevice);
            if(result == true) {
                Serial.println("\tsuccess");
                discoveredServices();
            } else
                Serial.println("\terror");

        } else
            Serial.println("ERROR: DEVICE IS NULLPTR");

        mExpected = false;
    }

    void discoveredServices()
    {
        if(mClient != nullptr) {

            // std::map<std::__cxx11::basic_string<char>, BLERemoteService*>*
            auto listServices = mClient->getServices();
            if(listServices != nullptr) {
                for(auto& p : *listServices) {

                    Serial.print("service: " + String(p.first.data()));

                    auto listCharacteristics = p.second->getCharacteristics();
                    if(listCharacteristics != nullptr) {

                        for(auto& l : *listCharacteristics) {

                            Serial.print("\n\tcharacteristic: " + String(l.first.data()) + "\t");

                            if(l.second->canRead()) {
                                Serial.print("R");
                            }

                            if(l.second->canWrite()) {
                                Serial.print("W");

                                mCharacteristic = l.second;
                            }

                            if(l.second->canNotify()) {
                                Serial.print("N");

                                l.second->registerForNotify(notifyCallback);

                                const uint8_t notificationOn[] = { 0x1, 0x0 };

                                // std::map<std::__cxx11::basic_string<char>, BLERemoteDescriptor*>*
                                auto listDesc = l.second->getDescriptors();
                                for(auto& d : *listDesc) {
                                    d.second->writeValue((uint8_t*)notificationOn, 2, true);
                                }
                            }
                        }

                        Serial.println("");
                    }
                }
            }
        }
    }

    static void notifyCallback(BLERemoteCharacteristic* pCharacteristic, uint8_t* pData, size_t length, bool isNotify)
    {
        // Serial.print("Notify callback for characteristic ");
        // Serial.print(pCharacteristic->getUUID().toString().c_str());

        Serial.println("BLE CLIENT: RECV: size: " + String(length) +
            " message: " + String(std::string((char*)pData, length).data()));

        if((char)pData[0] == '{' && (char)pData[1] == '%' && (char)pData[length - 1] == '}') {

            String sendData;
            for(int i = 0; i < length; i++)
                sendData += (char)pData[i];

            unsigned char* ch = gQueueBleClient.pop();
            delete ch;

            String message;
            DynamicJsonDocument doc(1024);

            doc[0]["id_house"] = gId;
            doc[0]["address_house"] = "ул. , д. , кв. ";
            doc[0]["command"] = "set_key";
            doc[0]["message"] = sendData;
            doc[0]["bridge"] = gMacBridge;
            doc[0]["lock"] = gMacLock;

            serializeJson(doc, message);

            // Serial.print("PUSH INT: " + message);
            gQueueInt.push((uint8_t*)&message[0], message.length());
        }

        if((char)pData[0] == '{' && (char)pData[length - 1] == '}') {
            Serial.println("!!!!{}!!!!");

            if((char)pData[1] == 'K' && (char)pData[2] == 'Y' && (char)pData[3] == 'P') {
                gQueueBleServer.push(pData, length);
            }
        }

        unsigned char* ch = gQueueBleClient.pop();
        delete ch;

        gTimeoutBle = 0;
    }

public:
    CBluetoothLe()
    {
        Serial.print("BLEDevice init...");
        BLEDevice::init(BLE_NAME);
        Serial.println("\tsuccess");
    }

    ~CBluetoothLe()
    {
        delete mServer;
        delete mClient;
        delete mDevice;
        delete mCharacteristic;
    }

    int getMode()
    {
        return mMode;
    }

    void setMode(int mode)
    {
        mMode = mode;
    }

    void scan()
    {
        mExpected = true;

        Serial.print("BLEScan init...");
        BLEScan* pBLEScan = BLEDevice::getScan();
        pBLEScan->setAdvertisedDeviceCallbacks(new CBLEAdvertisedDeviceCallbacks(this));
        pBLEScan->setActiveScan(true);
        Serial.println("\t\tsuccess");

        Serial.println("start scan...");
        pBLEScan->start(30);
    }

    void disconnectClient()
    {
        Serial.print("ble client: disconnect...");
        if(mClient != nullptr) {
            mClient->disconnect();
            Serial.println("\tsuccess");
        } else
            Serial.println("\tnull");
    }

    void deleteClient()
    {
        Serial.print("ble client: delete...");
        if(mClient != nullptr) {
            // delete mClient;
            // mClient = nullptr;
            Serial.println("\tsuccess");
        } else
            Serial.println("\tnull");
    }

    void sendData()
    {
        if(mExpected == false) {
            if(mDevice != nullptr) {
                if(mConnectedClient) {
                    if(mCharacteristic != nullptr) {

                        gTimeoutBle++;

                        const char* message = (char*)gQueueBleClient.top();
                        Serial.print("write characteristic...");
                        mCharacteristic->writeValue(message);
                        Serial.print("\t" + String(message));
                        Serial.println("\tsuccess");

                    } else
                        Serial.println("ERROR: CHARACTERISTIC IS NULLPTR");

                } else
                    connectClient();

            } else
                scan();
        }
    }

    bool getConnectedClient()
    {
        return mConnectedClient;
    }

    /**
       -------------------------------------------------------------------------------------------------------------------------------------------
       Server
       -------------------------------------------------------------------------------------------------------------------------------------------
    */
    class CBLEServerCallbacks : public BLEServerCallbacks
    {

    private:
        CBluetoothLe* mBle = nullptr;

        void onConnect(BLEServer* pServer) override
        {
            mBle->mConnectedServer = true;
            Serial.println("\n**********************\nble server: connect\n**********************");
        };

        void onDisconnect(BLEServer* pServer) override
        {
            mBle->mConnectedServer = false;
            Serial.println("\n**********************\nble server: disconnect\n**********************");
        }

    public:
        CBLEServerCallbacks(CBluetoothLe* ble)
            : mBle(ble) {};
    };

    bool getConnectedServer()
    {
        return mConnectedServer;
    }

    BLEService* mService = nullptr;
    BLECharacteristic* mCharacteristicServer = nullptr;

    CBLEServerCallbacks* mCBLEServerCallbacks = nullptr;

    void createServer()
    {

        if(mServer == nullptr) {
            Serial.print("ble server: CREATE...\t");
            mServer = BLEDevice::createServer();

            mCBLEServerCallbacks = new CBLEServerCallbacks(this);
            mServer->setCallbacks(mCBLEServerCallbacks);
            // mServer->setCallbacks(new CBLEServerCallbacks(this));

            mService = mServer->createService(SERVICE_UUID);
            {
                mCharacteristicServer = mService->createCharacteristic(CHARACTERISTIC_UUID,
                    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE |
                        BLECharacteristic::PROPERTY_NOTIFY);
                mCharacteristicServer->setCallbacks(new CBLECharacteristicCallbacks(this));
            }
            mService->start();
        }

        connectServer();
    }

    BLEAdvertising* mAdvertising = nullptr;

    void connectServer()
    {
        Serial.print("ble server: WAIT connected...");
        if(mServer == nullptr) {
            Serial.println("\tnull");
            return;
        }

        mServer->getAdvertising()->start();

        Serial.println("\tsuccess");
    }

    void disconnectServer()
    {
        Serial.print("ble server: DISConnected...");
        if(mServer == nullptr) {
            Serial.println("\tnull");
            return;
        }

        mServer->getAdvertising()->stop();

        // BLEAdvertising *pAdvertising = mServer->getAdvertising();
        // mAdvertising->stop();

        // mService->stop();

        // mServer->removeService(mService);

        mServer->disconnect(mServer->getConnId());
        // mServer->removePeerDevice(mServer->getConnId(), false);

        // deleteServer();

        Serial.println("\tsuccess");
    }

    /*void deleteServer()
      {
      Serial.print("ble server: delete...");
      if (mServer != nullptr && mCBLEServerCallbacks != nullptr) {

        delete mCBLEServerCallbacks;
        mCBLEServerCallbacks = nullptr;

        delete mServer;
        mServer = nullptr;
        Serial.println("\tsuccess");
      } else
        Serial.println("\tnull");
      }*/

    class CBLECharacteristicCallbacks : public BLECharacteristicCallbacks
    {
    private:
        CBluetoothLe* mBle = nullptr;

        void onWrite(BLECharacteristic* pCharacteristic) override
        {

            std::string value = pCharacteristic->getValue().c_str();
            int size = value.length();

            if(size > 0) {

                Serial.println("ble server: recv: " + String(value.data()));

                DynamicJsonDocument doc(1024);
                DeserializationError error = deserializeJson(doc, String(value.data()));
                if(error) {
                    Serial.print(F("deserializeJson() failed: "));
                    Serial.println(error.f_str());
                    return;
                }

                String item_command_settings = doc[0]["command"];
                Serial.println("ble server: item_command_settings: " + item_command_settings);
                if(item_command_settings == "settings") {

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

                        int size = item_wifi_name.length();
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

                        int size = item_wifi_password.length();
                        if(size < SIZE_WIFI_PARAM)
                            for(int i = 0; i < size; i++)
                                pass[i] = item_wifi_password[i];

                        for(int i = 0; i < SIZE_WIFI_PARAM; i++)
                            EEPROM.write(EEPROM_ADDRESS_PASS + i, pass[i]);
                    }

                    mBle->mMode = SERVER_OFF;
                    EEPROM.commit();
                    return;
                }

                String item_id = doc[0]["id_house"];
                Serial.println("ble server: id: " + item_id);
                if(item_id != gId)
                    return;

                String item_command = doc[0]["command"];
                Serial.println("ble server: command: " + item_command);
                if(item_command != "323247324687326472332")
                    return;

                /*String item_message = doc[0]["message"];
                  Serial.println("ble server: message: " + item_message);
                  if (item_message == "null")
                  return;*/

                CControl::wake();

                String message;
                doc[0]["command"] = "set_reg";
                serializeJson(doc, message);
                gQueueInt.push((uint8_t*)&message[0], message.length());
            }
        }

    public:
        CBLECharacteristicCallbacks(CBluetoothLe* ble)
            : mBle(ble) {};
    };

    void commandServer()
    {
        const unsigned char* item = gQueueBleServer.top();
        if(item == nullptr) {
            Serial.println("ble server: queue: null");
            return;
        }
        String message = String((char*)item);

        if(message == "START" && mMode != SERVER_ON) {

            disconnectClient();
            // deleteClient();
            createServer();

            mMode = SERVER_ON;

            Serial.println("ble server: start");
        } else if(message == "FINISH") {

            serverSend((uint8_t*)"SUCCESS", 7);

            disconnectServer();
            // deleteServer();

            mMode = SERVER_OFF;

            Serial.println("ble server: finish");
        } else if(message == "{HVS}") {

            // mMode = SERVER_ON;

            // gQueueBleServerSend.push((uint8_t*)&message[0], message.length());

            // if (mConnectedServer == false)
            // return;

            // for (int i = 0; i < 5; i++)
            // serverSend((uint8_t*)&message[0], message.length());
        }

        unsigned char* ch = gQueueBleServer.pop();
        if(ch != nullptr)
            delete ch;
    }

    void serverSend(uint8_t* data, const int& size)
    // void serverSend()
    {
        /*
          const unsigned char* item = gQueueBleServerSend.top();
          if (item == nullptr) {
          Serial.println("ble server send: queue: null");
          return;
          }
          String message = String((char*)item);
          Serial.println("ble server: send: " + message);
        */

        Serial.println("ble server: send: " + String((char*)&data));

        mCharacteristicServer->setValue(data, size);
        mCharacteristicServer->notify();
        delay(3);
    }
};
