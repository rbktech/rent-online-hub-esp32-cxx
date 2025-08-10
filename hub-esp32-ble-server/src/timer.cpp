#include "timer.h"

#include <stdexcept>

CTimer* CTimer::mInstance = nullptr;

CTimer* CTimer::getInstance()
{
    if(mInstance == nullptr)
        mInstance = new CTimer();

    return mInstance;
}

void funcTimer()
{

}

void CTimer::initTimer(const int& id, FuncTimer funcTimer, const int& sec)
{
    try {

        mListTimer.at(id);

    } catch(const std::out_of_range& oor) {

        /* Use 1st timer of 4 */
        /* 1 tick take 1/(80MHZ/80) = 1us, so we set divider 80 and count up */
        // hw_timer_t* timer = timerBegin(id, 80, true);
        hw_timer_t* timer = timerBegin(80);

        /* Attach onTimer function to our timer */
        // timerAttachInterrupt(timer, funcTimer, true);
        timerAttachInterrupt(timer, funcTimer);

        /* Set alarm to call onTimer function every second 1 tick is 1us
        => 1 second is 1000000us */
        /* Repeat the alarm (third parameter) */
        // timerAlarmWrite(timer, sec * 1000000, true);
        timerWrite(timer, sec * 1000000);

        /* Start an alarm */
        // timerAlarmEnable(timer);
        timerEnd(timer);

        mListTimer.insert({ id, timer });
    }
}

void CTimer::startTimer(const int& id)
{
    try {

        hw_timer_t* timer = mListTimer.at(id);
        // timerAlarmEnable(timer);
        timerEnd(timer);

    } catch(const std::out_of_range& oor) {
        return;
    }
}

void CTimer::endTimer(const int& id)
{
    try {

        hw_timer_t* timer = mListTimer.at(id);
        // timerAlarmDisable(timer);
        timerStop(timer);

    } catch(const std::out_of_range& oor) {
        return;
    }
}
