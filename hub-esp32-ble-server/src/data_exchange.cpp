#include "data_exchange.h"

#include "device/ble_client.h"
#include "device/ble_server.h"
#include "device/wifi_client.h"

void IRAM_ATTR onTimerGetStatus()
{
    if(CDataExchange::getInstance()->emptyCommand() == true)
        CDataExchange::getInstance()->status_bridge_send();
}

CDataExchange::CDataExchange()
{
    mListParse.insert({ "settings", &CDataExchange::settings });

    mListParse.insert({ "ble_bridge", &CDataExchange::ble_bridge });

    mListParse.insert({ "init_bridge", &CDataExchange::init_bridge_recv });

    mListParse.insert({ "status_bridge", &CDataExchange::status_bridge_recv });

    mListParse.insert({ "phone_bridge_server", &CDataExchange::phone_bridge_server });

    mListParse.insert({ "registration_start", &CDataExchange::registration_start_server_recv });
    mListParse.insert({ "registration_finish", &CDataExchange::registration_finish_server_recv });

    mListExchange[IDExchange::WIFE] = new CWiFe(IDExchange::WIFE);
    mListExchange[IDExchange::BLE_SERVER] = new CBLEServer(IDExchange::BLE_SERVER);
    mListExchange[IDExchange::BLE_CLIENT] = new CBLEClient(IDExchange::BLE_CLIENT);
}

CDataExchange::~CDataExchange() = default;

CDataExchange* CDataExchange::getInstance()
{
    if(mInstance == nullptr)
        mInstance = new CDataExchange();

    return mInstance;
}

CDataExchange* CDataExchange::mInstance = nullptr;

void CDataExchange::process()
{
    if(emptyCommand() == false) {

        auto command = mListCommand.front();
        std::string message = command.second;

        mTimeOut.delay(command.first);

        if(mListExchange[command.first]->write(message.c_str(), message.size()) == true) {
            if(mTimeOut.check() == true)
                pop();
        }
    }
}

void CDataExchange::addCommand(IDExchange id, const std::string& command)
{
    mListCommand.push({ id, command });
}

void CDataExchange::clearCommand()
{
    std::queue<std::pair<IDExchange, std::string>> empty;
    std::swap(mListCommand, empty);
}

bool CDataExchange::emptyCommand()
{
    return mListCommand.empty();
}

void CDataExchange::parseMessage(std::string in)
{
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, String(in.c_str()));
    if(error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    String status = doc[0]["status"];
    Serial.println("status: " + status);
    if(status != "success")
        return CPrint::println("Data exchange: error");

    String command = doc[0]["command"];
    Serial.println("command: " + command);

    try {
        FuncParse func = mListParse.at(command.c_str());
        if(func != nullptr)
            (this->*func)(doc);

    } catch(std::out_of_range oor) {
        CPrint::println("Function parse: not found");
    }

    pop();
}

/** ---------------------------------------------------------------------------------------------------------------- */

void CDataExchange::settings(DynamicJsonDocument& doc)
{
    CSettings::getInstance()->setSetting(doc);
}

void CDataExchange::ble_bridge(DynamicJsonDocument& doc)
{
    String command_ble = doc[0]["command_ble"];
    Serial.println("Command ble: " + command_ble);

    String message = doc[0]["message"];
    Serial.println("message: " + message);
    if(message != "null")
        addCommand(IDExchange::BLE_CLIENT, message.c_str());
}

void CDataExchange::init_bridge_recv(DynamicJsonDocument& doc)
{
    CDataExchange::getInstance()->status_bridge_send(); // The first package is not to wait 15 sec.

    // CTimer::getInstance()->initTimer(CTimer::STATUS, &onTimerGetStatus, 5);
}

/*void CDataExchange::activateOn(DynamicJsonDocument& doc)
{
    CPrint::println("***********************activateOn***********************");

    CDataExchange::getInstance()->addCommand(BLE_SERVER, "WAIT_USER");

    if(CSettings::getInstance()->getStatus() >= EMPTY) {

        String code_password = doc[0]["code_password"];
        Serial.println("Code password: " + code_password);
        if(code_password == "null")
            return CPrint::println("Code password: null");

        CSettings::getInstance()->mCodePassword = code_password.c_str();

        if(CSettings::getInstance()->getStatus() == EMPTY) {

            CSettings::getInstance()->setStatus(WAIT_USER);
        }
    }
}*/

void CDataExchange::status_bridge_recv(DynamicJsonDocument& doc)
{
    CDataExchange::getInstance()->addCommand(BLE_SERVER, "");

    String item_case = doc[0]["case"];
    Serial.println("status_bridge: case: " + item_case);
    if(item_case != "null") {

        if(item_case == "delete_all_user") {
            ble_bridge(doc);
        }

        if(item_case == "activate_off") {
            getDevice(BLE_SERVER)->disconnect();
            addCommand(BLE_SERVER, EMPTY);
        }

        if(item_case == "activate_on") {
            addCommand(BLE_SERVER, WAIT_USER);
        }
    }
}

void CDataExchange::phone_bridge_server(DynamicJsonDocument& doc)
{
    String item_id_house = doc[0]["id_house"];
    Serial.println("phone_bridge_server: id_house: " + item_id_house);
    if(item_id_house != String(CSettings::getInstance()->guid.c_str()))
        return;

    String item_code_password = doc[0]["code_password"];
    Serial.println("phone_bridge_server: code_password: " + item_code_password);
    if(item_code_password == "null")
        return;

    String item_message = doc[0]["message"];
    Serial.println("phone_bridge_server: message: " + item_message);
    if(item_message != "null") {

        CTimer::getInstance()->endTimer(CTimer::STATUS);

        CPrint::println("***********************clearCommand1***********************");
        clearCommand();
        CPrint::println("***********************clearCommand2***********************");

        String message;
        serializeJson(doc, message);

        CDataExchange::getInstance()->addCommand(WIFE, message.c_str());
    }
}

void CDataExchange::registration_start_server_recv(DynamicJsonDocument& doc)
{
    String item_message = doc[0]["message"];
    Serial.println("registration_start_server_recv: message: " + item_message);
    if(item_message != "null") {
        CDataExchange::getInstance()->addCommand(BLE_SERVER, item_message.c_str());
        CDataExchange::getInstance()->addCommand(BLE_SERVER, item_message.c_str());
    }

    CTimer::getInstance()->startTimer(CTimer::STATUS);
}

void CDataExchange::registration_finish_server_recv(DynamicJsonDocument& doc)
{
    CDataExchange::getInstance()->addCommand(BLE_SERVER, SAVE_USER);
    CDataExchange::getInstance()->addCommand(BLE_SERVER, SAVE_USER);

    CTimer::getInstance()->startTimer(CTimer::STATUS);
}

/** ---------------------------------------------------------------------------------------------------------------- */

void CDataExchange::status_bridge_send()
{
    String message;
    DynamicJsonDocument doc(1024);

    doc[0]["command"] = "status_bridge";
    doc[0]["id_house"] = CSettings::getInstance()->guid;

    serializeJson(doc, message);

    CDataExchange::getInstance()->addCommand(WIFE, message.c_str());
}

/*void CDataExchange::initMessage()
{
    CSettings* stg = CSettings::getInstance();

    String message;
    DynamicJsonDocument doc(1024);

    doc[0]["id_house"] = stg->mId;
    doc[0]["command"] = "get_key";
    doc[0]["bridge"] = stg->mMacBridge;
    doc[0]["lock"] = stg->mMacLock;

    serializeJson(doc, message);

    CDataExchange::getInstance()->addCommand(WIFE, message.c_str());
}*/

/*void CDataExchange::registration_start_send()
{
    String message;
    DynamicJsonDocument doc(1024);

    doc[0]["command"] = "registration_start";
    doc[0]["status"] = "success";
    doc[0]["id_house"] = CSettings::getInstance()->mId;
    doc[0]["message"] = CSettings::getInstance()->mCodePassword;

    serializeJson(doc, message);

    CDataExchange::getInstance()->addCommand(IDExchange::WIFE, message.c_str());
}*/

/*void CDataExchange::wake()
{
    String message;
    DynamicJsonDocument doc(1024);

    doc[0]["id_house"] = String(CSettings::getInstance()->mId.data());
    doc[0]["command"] = "get_wake";
    doc[0]["status"] = "success";

    serializeJson(doc, message);

    CDataExchange::getInstance()->addCommand(WIFE, message.c_str());
}*/

void CDataExchange::ble_command()
{
    String message;
    DynamicJsonDocument doc(1024);

    doc[0]["command"] = "ble_command";
    doc[0]["id_house"] = CSettings::getInstance()->guid;

    serializeJson(doc, message);

    CDataExchange::getInstance()->addCommand(WIFE, message.c_str());
}

void CDataExchange::init_bridge_send()
{
    String message;
    DynamicJsonDocument doc(1024);

    doc[0]["command"] = "init_bridge";
    doc[0]["id_house"] = CSettings::getInstance()->guid.c_str();

    serializeJson(doc, message);

    CDataExchange::getInstance()->addCommand(WIFE, message.c_str());
}

void CDataExchange::create_sign_and_key(const uint8_t* data, const int& size)
{
    String message;
    String sendData;
    DynamicJsonDocument doc(1024);

    for(int i = 0; i < size; i++)
        sendData += (char)data[i];

    doc[0]["command"] = "create_sign_and_key";
    doc[0]["id_house"] = CSettings::getInstance()->guid;
    doc[0]["message"] = sendData;

    serializeJson(doc, message);

    CDataExchange::getInstance()->addCommand(WIFE, message.c_str());
}

void CDataExchange::create_database()
{
    String message;
    DynamicJsonDocument doc(1024);

    CSettings* stg = CSettings::getInstance();

    doc[0]["command"] = "create_database";
    doc[0]["id_house"] = stg->guid;
    doc[0]["address_house"] = stg->addressLocal;
    doc[0]["bridge"] = stg->mMacBridge;
    doc[0]["lock"] = stg->mMacLock;

    serializeJson(doc, message);

    CDataExchange::getInstance()->addCommand(WIFE, message.c_str());
}

/*void CDataExchange::getWake()
{
    String message;
    DynamicJsonDocument doc(1024);

    doc[0]["id_house"] = String(CSettings::getInstance()->mId.data());
    doc[0]["command"] = "get_wake_wake";
    doc[0]["status"] = "success";

    serializeJson(doc, message);

    CDataExchange::getInstance()->addCommand(WIFE, message.c_str());
}*/

/*void CDataExchange::deleteAllUser()
{
    String message;
    DynamicJsonDocument doc(1024);

    doc[0]["id_house"] = String(CSettings::getInstance()->mId.data());
    doc[0]["command"] = "delete_all_user";
    doc[0]["status"] = "success";

    serializeJson(doc, message);

    CDataExchange::getInstance()->addCommand(WIFE, message.c_str());
}*/

/** ---------------------------------------------------------------------------------------------------------------- */

void CDataExchange::setFunction(void (*func)())
{
    if(mFunc != nullptr)
        return CPrint::print("script busy");

    mFunc = func;
}

void CDataExchange::script()
{
    if(mFunc != nullptr) {
        mFunc();
        mFunc = nullptr;
    }
}

CBaseDevice* CDataExchange::getDevice(IDExchange id)
{
    return mListExchange[id];
}

void CDataExchange::pop()
{
    if(emptyCommand() == false) {

        mListCommand.pop();
        mTimeOut.reset();
    }
}

void CDataExchange::printFirst()
{
    CPrint::print("ListCommand is ");

    if(!emptyCommand()) {

        CPrint::print("no empty: ");

        switch(mListCommand.front().first) {
        case IDExchange::WIFE:
            CPrint::print("WIFE");
            break;
        case IDExchange::BLE_CLIENT:
            CPrint::print("BLE_CLIENT");
            break;
        case IDExchange::BLE_SERVER:
            CPrint::print("BLE_SERVER");
            break;
        }
    } else
        CPrint::print("empty: ");

    CPrint::println(" size: ", mListCommand.size());
}

bool CDataExchange::emptyListCommand()
{
    return mListCommand.empty();
}