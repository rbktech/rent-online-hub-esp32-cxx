#ifndef PRINT_HPP
#define PRINT_HPP

#include <string>

class CPrint
{
public:
    CPrint();
    ~CPrint();

    static void delay_s(int value);
    static void init(int delay_start);
    static void printSettings();
    static void println(const char* message);
    static void println(const char* message, const char* value);
    static void println(const char* message, const unsigned int& value);
    static void println(const char* message_0, const char* message_1, const char* message_2, const char* message_3);
    static void print(const char* message);
    static void print(const char* message, const char* value);
    static void print(const char* message, const unsigned int& value);
    static void printMessage(const char* device, uint8_t* data, size_t length);
    static void printMessage(const char* device, const char* data, unsigned int length);
    static std::string toString(int value);
    static void printStatus();

    template <typename T> static void printAddress(const char* message, T* obj)
    {
        CPrint::print(message);

        char buffer[30];
        sprintf(buffer, "with %%p:  x    = %p\n", obj);
        println(buffer);
    }
};

#endif // PRINT_HPP
