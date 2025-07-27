#ifndef DATA_EXCHANGE_H
#define DATA_EXCHANGE_H

#include "device/base_device.h"
#include "print.hpp"
#include <map>
#include <queue>
#include <string>

#define NUMBER_OF_TIMES 4

class CTimeOut
{
private:
    int mValue = NUMBER_OF_TIMES;
    bool mFirst = false;

public:
    bool check()
    {
        CPrint::println("TimeOut: ", mValue);

        if(mFirst == false) {
            if(mValue-- == 0) {
                reset();
                return true;
            }
        }

        return false;
    }

    void reset()
    {
        mValue = NUMBER_OF_TIMES;
        mFirst = true;
    }

    void delay(IDExchange id)
    {
        mFirst = false;
        if(mValue < NUMBER_OF_TIMES) {

            if(id == IDExchange::BLE_CLIENT)
                CPrint::delay_s(2000);

            if(id == IDExchange::WIFE)
                CPrint::delay_s(1000);
        }
    }
};

class CDataExchange
{
private:
    static CDataExchange* mInstance;

    typedef void (CDataExchange::*FuncParse)(DynamicJsonDocument& doc);
    std::map<std::string, FuncParse> mListParse;

    std::queue<std::pair<IDExchange, std::string>> mListCommand;

    CBaseDevice* mListExchange[3]{ nullptr, nullptr, nullptr };

    void settings(DynamicJsonDocument& doc);

    void ble_bridge(DynamicJsonDocument& doc);

    void init_bridge_recv(DynamicJsonDocument& doc);

    void status_bridge_recv(DynamicJsonDocument& doc);

    void phone_bridge_server(DynamicJsonDocument& doc);

    void registration_start_server_recv(DynamicJsonDocument& doc);
    void registration_finish_server_recv(DynamicJsonDocument& doc);

    void (*mFunc)() = nullptr;

    CTimeOut mTimeOut;

public:
    CDataExchange();
    ~CDataExchange();

    static CDataExchange* getInstance();

    CBaseDevice* getDevice(IDExchange id);
    void addCommand(IDExchange id, const std::string& command);
    void clearCommand();
    bool emptyCommand();
    void pop();

    void process();
    void script();
    void setFunction(void (*func)());

    void printFirst();

    void parseMessage(std::string in);

    // void initMessage();
    // void addUserMessage();
    // void registration();

    // void getWake();

    // void deleteAllUser();

    /** Internet */
    // void wake();
    void ble_command();

    void status_bridge_send();

    void init_bridge_send();
    void create_sign_and_key(const uint8_t* data, const int& size);
    void create_database();

    bool emptyListCommand();
};

#endif // DATA_EXCHANGE_H