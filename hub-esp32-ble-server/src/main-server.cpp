#include "data_exchange.h"
#include "device/ble_scan.h"
// #include "print.hpp"
// #include "timer.h"

// #include "device/wifi_client.h"

#include <iostream>

// #include <queue>

void setup()
{
    CPrint::init(1);
    BLEDevice::init(BLE_NAME);
    CDataExchange::getInstance()->getDevice(BLE_SERVER)->init();
    CDataExchange::getInstance()->getDevice(BLE_SERVER)->connect();
}

void loop()
{
    try {

        CDataExchange::getInstance()->process();

        delay(1000);

    } catch(...) {
        printf("EXCEPTION");
    }
}