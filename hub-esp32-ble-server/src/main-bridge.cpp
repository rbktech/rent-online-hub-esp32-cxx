#include "data_exchange.h"
#include "device/ble_scan.h"
#include "print.hpp"
#include "timer.h"

// bool gInit = false;

/*void setStatusStartRegistration()
{
    CSettings::getInstance()->setStatus(START_REGISTRATION);
}*/

/*void initBridge()
{
    CDataExchange::getInstance()->getDevice(BLE_SERVER)->disconnect();

    if(CSettings::getInstance()->getStatus() == MASTER_MODE)
        CSettings::getInstance()->setStatus(EMPTY);

    CDataExchange::getInstance()->getDevice(WIFE)->init();
    CBLEScan::getInstance()->start();

    if(CSettings::getInstance()->createId() == true) {
        CDataExchange::getInstance()->initMessage();

        if(CSettings::getInstance()->getStatus() >= EMPTY)
            // CDataExchange::getInstance()->getStatus();
        gInit = true;
    }
}*/

/*void IRAM_ATTR onTimerGetStatus()
{
    if(CDataExchange::getInstance()->emptyListCommand() == false)
        return;

    if(CSettings::getInstance()->getStatus() >= EMPTY)
        CDataExchange::getInstance()->getStatus();
}*/

/*void IRAM_ATTR onTimerWaitAddUser()
{
    CDataExchange::getInstance()->setFunction(&setStatusStartRegistration);
    CTimer::getInstance()->endTimer(CTimer::WAIT_ADD_USER);
}*/

void IRAM_ATTR onTimerNotify()
{
    CDataExchange::getInstance()->getDevice(IDExchange::BLE_SERVER)->notify();
}

void IRAM_ATTR onTimerRestart()
{
    if(CSettings::getInstance()->mRestart == true)
        esp_restart();
}

/*void IRAM_ATTR onTimerInit()
{
    CDataExchange::getInstance()->setFunction(&initBridge);
    // CTimer::getInstance()->startTimer(CTimer::STATUS, &onTimerGetStatus, 15);
    CTimer::getInstance()->endTimer(CTimer::INIT);
}*/

/*void IRAM_ATTR onTimerEndRegistration()
{
    CDataExchange::getInstance()->getDevice(IDExchange::BLE_SERVER)->disconnect();
    CSettings::getInstance()->setStatus(EMPTY);

    CTimer::getInstance()->endTimer(CTimer::INIT);

    CPrint::println("***********************REGISTRATION_FINISH***********************");
}*/

void setup()
{
    // CSettings::getInstance()->setStatus(EMPTY);

    // CPrint::init(3 * 1000);
    CPrint::init(1);
    CPrint::printSettings();

    // CTimer::getInstance()->initTimer(CTimer::NOTIFI, &onTimerNotify, 3);
    // CTimer::getInstance()->initTimer(CTimer::RESTART, &onTimerRestart, 1);

    BLEDevice::init(BLE_NAME);

    CBLEScan::getInstance()->start();

    CDataExchange::getInstance()->getDevice(BLE_SERVER)->init();
    CDataExchange::getInstance()->getDevice(BLE_SERVER)->connect();
    CPrint::delay_s(5 * 1000);
    CDataExchange::getInstance()->getDevice(BLE_SERVER)->disconnect();

    CDataExchange::getInstance()->getDevice(WIFE)->init();

    CDataExchange::getInstance()->init_bridge_send();

    /*if(CSettings::getInstance()->getStatus() == ADD_USER)
        CSettings::getInstance()->setStatus(START_REGISTRATION);

    if(CSettings::getInstance()->getStatus() == KEY_SUCCESS)
        CSettings::getInstance()->setStatus(INIT);

    int timeInit = 1;
    if(CSettings::getInstance()->getStatus() == EMPTY) {
        CDataExchange::getInstance()->getDevice(BLE_SERVER)->connect();
        CSettings::getInstance()->setStatus(MASTER_MODE);
        // timeInit = 10;
    }

    CTimer::getInstance()->startTimer(CTimer::INIT, &onTimerInit, timeInit);*/
}

void control()
{
    if(Serial.available()) {

        char ch = Serial.read();
        Serial.println(ch);

        if(CDataExchange::getInstance()->emptyCommand() == true) {

            if(ch == '6') {
                CDataExchange::getInstance()->ble_command();
            }
        }

        if(ch == '7') {
            CDataExchange::getInstance()->getDevice(BLE_CLIENT)->disconnect();
        }

        if(ch == '8') {
            CDataExchange::getInstance()->getDevice(IDExchange::BLE_SERVER)->disconnect();
        }

        if(ch == '9') {
            CDataExchange::getInstance()->printFirst();
        }
    }
}

int gCountStatus = 0;
int gCountServer = 0;

void onGetStatus()
{
    if(gCountStatus == 500000)
        CDataExchange::getInstance()->printFirst();


    if(gCountStatus++ == 500000) {
        gCountStatus = 0;

        if(CDataExchange::getInstance()->emptyCommand() == true)
            CDataExchange::getInstance()->status_bridge_send();
    }

    if(gCountServer++ == 60000) {
        gCountServer = 0;

        CDataExchange::getInstance()->getDevice(IDExchange::BLE_SERVER)->notify();
    }
}

void loop()
{
    try {

    control();
    // CDataExchange::getInstance()->script();
    CDataExchange::getInstance()->process();

    // if(gInit == true)
    //  if(CDataExchange::getInstance()->emptyListCommand() == true)
    //    registration();

    onGetStatus();

    } catch(...) {

        CPrint::println("EXCEPTION", "");
    }
}

/*void registration()
{
    switch (CSettings::getInstance()->getStatus()) {

        case KEY_SUCCESS: {

            if(CDataExchange::getInstance()->emptyListCommand() == true) {

                    CPrint::println("***********************KEY_SUCCESS***********************");

                    // CDataExchange::getInstance()->clearList();
                    CDataExchange::getInstance()->wake();
            }

            break;
        }

        case EMPTY: {

            if(CSettings::getInstance()->mConnectedServer == true) {
                CDataExchange::getInstance()->getDevice(IDExchange::BLE_SERVER)->disconnect();
            }

            break;
        }

        case WAIT_USER: {

            if(CSettings::getInstance()->mConnectedServer == false) {

                CDataExchange::getInstance()->getDevice(IDExchange::BLE_SERVER)->connect();
                CDataExchange::getInstance()->addCommand(IDExchange::BLE_SERVER, BRIDGE_STATUS_WAIT_USER);
            }

            break;
        }

        case START_REGISTRATION: {

            if(CDataExchange::getInstance()->emptyListCommand() == true) {
                if(CSettings::getInstance()->mConnectedClient == false) {

                    CPrint::println("***********************START_REGISTRATION***********************");

                    CDataExchange::getInstance()->getDevice(IDExchange::BLE_SERVER)->disconnect();

                    CDataExchange::getInstance()->deleteAllUser();
                    // CDataExchange::getInstance()->getWake();
                    CDataExchange::getInstance()->addUserMessage();

                    // Раз в 10 секунд
                    // Проверка если нет никаких поключений
                    // CDataExchange::getInstance()->addCommand(IDExchange::WIFE, "add_user");

                    // Если успех
                    // CSettings::getInstance()->setStatus(ADD_USER)
                }
            }

            break;
        }

        case ADD_USER: {

            CDataExchange* dataExchange = CDataExchange::getInstance();
            if(dataExchange != nullptr) {

                CBaseDevice* device = dataExchange->getDevice(IDExchange::BLE_SERVER);
                if(device != nullptr) {

                    if(device->getNotifyMessage() != BRIDGE_STATUS_ADD_USER) {

                        CPrint::println("***********************ADD_USER***********************");

                        device->connect();
                        dataExchange->addCommand(IDExchange::BLE_SERVER, BRIDGE_STATUS_ADD_USER);

                        // Включение таймера, через 40 секунд сброс в START_REGISTRATION и отключаем таймер
                        CTimer::getInstance()->startTimer(CTimer::WAIT_ADD_USER, &onTimerWaitAddUser, 40);

                        // Если пришел ответ c телефона "registration_user", то отключаем таймер
                        // CSettings::getInstance()->setStatus(REGISTRATION_USER)
                    }
                }
            }

            break;
        }

        case REGISTRATION_USER: {


            // CDataExchange::getInstance()->getDevice(IDExchange::BLE_SERVER)->disconnect(); ??

            // Раз в 10 секунд
            // Проверка если нет никаких поключений

            if(CDataExchange::getInstance()->emptyListCommand() == true) {

                CPrint::println("***********************REGISTRATION_USER***********************");

                // ОТправляем на сервер, что надо зарегистрировать пользователя
                CDataExchange::getInstance()->registration();
            }

            // Если пришел ответ с сервера "registration_server", то ставим флаг REGISTRATION_SERVER

            break;
        }

        case REGISTRATION_SERVER: {

            CDataExchange* dataExchange = CDataExchange::getInstance();
            if(dataExchange != nullptr) {

                CBaseDevice* device = dataExchange->getDevice(IDExchange::BLE_SERVER);
                if(device != nullptr) {

                    if(device->getNotifyMessage() != BRIDGE_STATUS_REGISTRATION_SERVER) {

                        CPrint::println("***********************REGISTRATION_SERVER***********************");

                        device->connect();
                        dataExchange->addCommand(IDExchange::BLE_SERVER, BRIDGE_STATUS_REGISTRATION_SERVER);

                        CTimer::getInstance()->startTimer(CTimer::INIT, &onTimerEndRegistration, 10);
                    }
                }
            }

            // Если пришел ответ c телефона "registration_finish"
            // CSettings::getInstance()->setStatus(EMPTY)

            break;
        }
    }
}*/