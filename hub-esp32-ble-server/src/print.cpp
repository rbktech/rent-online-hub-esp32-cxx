#include "print.hpp"

#include "HardwareSerial.h"
#include "settings.h"

CPrint::CPrint() = default;
CPrint::~CPrint() = default;

void CPrint::init(int delay_start)
{
    delay_s(delay_start);

    Serial.begin(115200);

    while(!Serial) {
        // Ожидание подключения последовательного порта. Нужно только для Leonardo
    }
}

void CPrint::printSettings()
{
    CSettings* stg = CSettings::getInstance();

    Serial.println("Settings:");

    Serial.println(stg->PORT);
    Serial.print(stg->SERV[0]);
    Serial.print(" ");
    Serial.print(stg->SERV[1]);
    Serial.print(" ");
    Serial.print(stg->SERV[2]);
    Serial.print(" ");
    Serial.println(stg->SERV[3]);
    Serial.println(stg->ssid);
    Serial.println(stg->pass);
    Serial.println(stg->addressLocal);
}

void CPrint::println(const char* message)
{
    Serial.println(message);
}

void CPrint::println(const char* message, const unsigned int& value)
{
    Serial.println(message + String(value));
}

void CPrint::println(const char* message, const char* value)
{
    Serial.println(message + String(value));
}

void CPrint::println(const char* message_0, const char* message_1, const char* message_2, const char* message_3)
{
    Serial.println(message_0 + String(message_1) + String(message_2) + String(message_3));
}

void CPrint::print(const char* message)
{
    Serial.print(message);
}

void CPrint::print(const char* message, const unsigned int& value)
{
    Serial.print(message + String(value));
}

void CPrint::printMessage(const char* device, uint8_t* data, size_t length)
{
    Serial.print(device);
    Serial.print(" size: ");
    Serial.print(length);
    Serial.print(" message: ");
    Serial.println(std::string((char*)data, length).c_str());
}

void CPrint::printMessage(const char* device, const char* data, unsigned int length)
{
    Serial.print(device);
    Serial.print(" size: ");
    Serial.print(length);
    Serial.print(" message: ");
    Serial.println(std::string((char*)data, length).c_str());
}

void CPrint::print(const char* message, const char* value)
{
    Serial.print(message + String(value));
}

std::string CPrint::toString(int value)
{
    return String(value).c_str();
}

void CPrint::delay_s(int value)
{
    delay(value);
}

void CPrint::printStatus()
{
    Serial.print("Status: ");

    /*switch (CSettings::getInstance()->getStatus()) {
        case INIT:
            println("INIT");
            break;
        case KEY_SUCCESS:
            println("KEY_SUCCESS");
            break;
        case EMPTY:
            println("EMPTY");
            break;
        case WAIT_USER:
            println("WAIT_USER");
            break;
        case START_REGISTRATION:
            println("START_REGISTRATION");
            break;
        case ADD_USER:
            println("ADD_USER");
            break;
        case REGISTRATION_USER:
            println("REGISTRATION_USER");
            break;
        case REGISTRATION_SERVER:
            println("REGISTRATION_SERVER");
            break;
        default:
            println("UNKNOWN");
    }*/
}