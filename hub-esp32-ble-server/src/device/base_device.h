#ifndef BASE_DEVICE_H
#define BASE_DEVICE_H

#include <string>

#include "settings.h"
#include "timer.h"

#define EMPTY ""
#define WAIT_USER "WAIT_USER"
#define SAVE_USER "SAVE_USER"

/*#define BRIDGE_STATUS_EMPTY ""
#define BRIDGE_STATUS_WAIT_USER "wait_user"
#define BRIDGE_STATUS_ADD_USER "add_user"
#define BRIDGE_STATUS_CODE_PASSWORD_SUCCESS "code_password_success"
#define BRIDGE_STATUS_CODE_PASSWORD_ERROR "code_password_error"
#define BRIDGE_STATUS_ID_HOUSE_ERROR "id_house_error"
#define BRIDGE_STATUS_REGISTRATION_SERVER "registration_server"*/

enum IDExchange : int {
    WIFE = 0,
    BLE_SERVER,
    BLE_CLIENT,
};

class CBaseDevice
{
private:
    IDExchange mId;

protected:
    std::string mNotifyMessage;

public:
    CBaseDevice(IDExchange id);
    ~CBaseDevice();

    virtual bool init() = 0;
    virtual void notify() = 0;
    virtual bool write(const char* message, const int& size) = 0;
    virtual bool connect() = 0;
    virtual void disconnect() = 0;

    std::string getNotifyMessage();
};

#endif // BASE_DEVICE_H