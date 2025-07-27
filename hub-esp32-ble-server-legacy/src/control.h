#include "secret.h"

class CControl
{
private:
    int mTimeoutCheck = 10;

    void createId()
    {
        gMacLock.toUpperCase();

        for(auto& p : gMacLock)
            if(p != ':')
                gId += p;

        for(auto& p : gMacBridge)
            if(p != ':')
                gId += p;
    }

public:
    void init()
    {
        if(!gId.isEmpty())
            return;

        if(!gMacLock.isEmpty() && !gMacBridge.isEmpty()) {

            createId();

            String message;
            DynamicJsonDocument doc(1024);

            doc[0]["id_house"] = gId;
            doc[0]["command"] = "get_key";
            doc[0]["bridge"] = gMacBridge;
            doc[0]["lock"] = gMacLock;

            serializeJson(doc, message);
            gQueueInt.push((uint8_t*)&message[0], message.length());
        }
    }

    void check()
    {
        Serial.println("Time: " + String(mTimeoutCheck));
        if(mTimeoutCheck == 10) {
            mTimeoutCheck = 0;

            String message;
            DynamicJsonDocument doc(1024);

            doc[0]["id_house"] = gId;
            doc[0]["command"] = "get_reg";

            serializeJson(doc, message);
            gQueueInt.push((uint8_t*)&message[0], message.length());
        } else
            mTimeoutCheck++;
    }

    static void wake()
    {
        String message;
        DynamicJsonDocument doc(1024);

        doc[0]["id_house"] = gId;
        doc[0]["command"] = "get_wake";

        serializeJson(doc, message);
        gQueueInt.push((uint8_t*)&message[0], message.length());
    }

    int mSettingsTimeout = 0;

    bool settings()
    {

        if(mSettingsTimeout > 100)
            return true;

        mSettingsTimeout++;
        return false;
    }
};
