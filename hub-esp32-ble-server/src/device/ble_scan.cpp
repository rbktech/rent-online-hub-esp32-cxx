#include "ble_scan.h"

std::string CBLEScan::CBLEAdvertisedDeviceCallbacks::toUpper(std::string address)
{
    std::transform(address.begin(), address.end(), address.begin(), ::toupper);
    return address;
}

void CBLEScan::CBLEAdvertisedDeviceCallbacks::onResult(BLEAdvertisedDevice advertisedDevice)
{
    std::string nameDevice = advertisedDevice.getName().c_str();
    if(nameDevice == "NAMEDEVICE") {

        std::string mac = toUpper(advertisedDevice.getAddress().toString().c_str());

        if(mac == "MAC:ADDRESS") {

            advertisedDevice.getScan()->stop();

            CSettings::getInstance()->mMacLock = toUpper(advertisedDevice.getAddress().toString().c_str());

            CPrint::println("Find device: ", nameDevice.c_str(), " mac: ", CSettings::getInstance()->mMacLock.c_str());

            CPrint::println("stop scan...");

            CBLEScan::getInstance()->mBLEAdvertisedDevice = new BLEAdvertisedDevice(advertisedDevice);
        }
    }
}

CBLEScan::CBLEScan()
{
    mBLEAdvertisedDevice = nullptr;
}

CBLEScan::~CBLEScan() = default;

CBLEScan* CBLEScan::getInstance()
{
    if(mInstance == nullptr)
        mInstance = new CBLEScan();

    return mInstance;
}

CBLEScan* CBLEScan::mInstance = nullptr;

void CBLEScan::start()
{
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new CBLEAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    CPrint::print("start scan...");
    pBLEScan->start(30);

    if(mBLEAdvertisedDevice == nullptr) {
        CPrint::print(".");
        CPrint::delay_s(100);
    }

    delete pBLEScan;
}

BLEAdvertisedDevice* CBLEScan::getDevice()
{
    return mBLEAdvertisedDevice;
}