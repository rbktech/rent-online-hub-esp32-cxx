#include "ble_server.h"

CBLEServer::CBLEServer(IDExchange id)
    : CBaseDevice(id)
{
    mServer = nullptr;
    mService = nullptr;
    mCharacteristic = nullptr;

    mNotifyMessage = EMPTY;
}

CBLEServer::~CBLEServer() = default;

void CBLEServer::CBLECharacteristicCallbacks::onWrite(BLECharacteristic* pCharacteristic)
{
    std::string value = pCharacteristic->getValue().c_str();
    unsigned size = value.size();

    CPrint::printMessage("ble server: recv:", value.c_str(), size);

    if(size > 0)
        CDataExchange::getInstance()->parseMessage(value);
}

bool CBLEServer::init()
{
    CPrint::println("CBLEServer::init");

    mServer = BLEDevice::createServer();
    mServer->setCallbacks(new CBLEServerCallbacks());

    mService = mServer->createService(SERVICE_UUID);

    auto p = BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY;
    mCharacteristic = mService->createCharacteristic(CHARACTERISTIC_UUID, p);
    mCharacteristic->setCallbacks(new CBLECharacteristicCallbacks());

    mService->start();
    return true;
}

void CBLEServer::notify()
{
    if(mNotifyMessage.empty() == false) {

        if(CSettings::getInstance()->mConnectedClient == false) {

            // if(mServer->getConnectedCount() == 0 && CSettings::getInstance()->mConnectedServer == false)
            if(CSettings::getInstance()->mConnectedServer == false)
                CBLEServer::connect();

            if(mServer->getConnectedCount() == 1 && CSettings::getInstance()->mConnectedServer == true) {

                CPrint::printMessage("ble server: send:", mNotifyMessage.c_str(), mNotifyMessage.size());

                mCharacteristic->setValue(mNotifyMessage.c_str());
                mCharacteristic->notify();
                CPrint::delay_s(3);
                // delay(500); // delay(3);
            }
        }
    }
}

bool CBLEServer::write(const char* message, const int& size)
{
    CPrint::println("ble server: message: ", message);

    mNotifyMessage = message;
    CDataExchange::getInstance()->pop();
    return false;
}

bool CBLEServer::connect()
{
    CPrint::print("ble server: connect...");

    mServer->getAdvertising()->start();

    CSettings::getInstance()->mConnectedServer = true;

    CPrint::println("\tsuccess");

    return true;
}

void CBLEServer::disconnect()
{
    CPrint::print("ble server: disconnect...");

    mServer->disconnect(mServer->getConnId());
    mServer->getAdvertising()->stop();

    CSettings::getInstance()->mConnectedServer = false;

    CPrint::println("\tsuccess");
}