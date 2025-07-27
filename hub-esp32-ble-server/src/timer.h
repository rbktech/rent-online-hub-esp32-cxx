#pragma once

#include <esp32-hal-timer.h>
#include <functional>
#include <map>

class CTimer
{

private:
    static CTimer* mInstance;

    std::map<int, hw_timer_t*> mListTimer;

public:
    typedef void (*FuncTimer)();

    enum { STATUS = 0, RESTART, NOTIFI };

    static CTimer* getInstance();

    void initTimer(const int& id, FuncTimer funcTimer, const int& sec);

    void startTimer(const int& id);
    void endTimer(const int& id);
};