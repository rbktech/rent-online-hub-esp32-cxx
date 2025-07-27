#include "ble_client.h"
#include "ble_scan.h"

CBLEClient::CBLEClient(IDExchange id)
    : CBaseDevice(id)
{
    mClient = nullptr;
    mCharacteristicWrite = nullptr;
}

CBLEClient::~CBLEClient() = default;

// std::string CBLEClient::mKey = "Hello";
std::string CBLEClient::mKey;

void CBLEClient::notifyCallback(BLERemoteCharacteristic* pCharacteristic, uint8_t* data, size_t length, bool isNotify)
{
    CPrint::println("CBLEClient::notifyCallback");

    if(isNotify)
        (void)pCharacteristic;

    CPrint::printMessage("ble client: recv: ", data, length);

    if(length > 0) {

        if((char)data[0] == '<' && (char)data[1] == '$' && (char)data[length - 1] == '>') {

            CDataExchange::getInstance()->pop();

            if(memcmp(mKey.c_str(), data, length) != 0) {

                mKey.assign((char*)data, length);

                CDataExchange::getInstance()->create_sign_and_key(data, length);
                // CDataExchange::getInstance()->ble_command();
            }

            CPrint::println("ble client: key: ", mKey.c_str());
        }

        /*if(memcmp(data, "{GYH}", length) == 0) {
            if(CSettings::getInstance()->getStatus() == START_REGISTRATION)
                CSettings::getInstance()->setStatus(ADD_USER);
        }*/

        if(memcmp(data, "{SKSS}", length) == 0 || memcmp(data, "{SNOKI}", length) == 0) {
            CDataExchange::getInstance()->pop();
            // CDataExchange::getInstance()->create_database();
        }
    }
}

/*void CBLEClient::discoveredServices()
{
    auto listServices = mClient->getServices();
    CPrint::println("65");
    if(listServices != nullptr) {
        CPrint::println("66");
        for(auto& itemServices : *listServices) {

            auto listCharacteristics = itemServices.second->getCharacteristics();
            CPrint::println("67");
            if(listCharacteristics != nullptr) {

                CPrint::println("68");
                for(auto& itemCharacteristics : *listCharacteristics) {

                    auto characteristics = itemCharacteristics.second;

                    CPrint::print(characteristics->getUUID().toString().c_str(), " : ");

                    if(characteristics->canWrite()) {
                        CPrint::print("R");
                        mCharacteristic = characteristics;
                    }

                    if(characteristics->canNotify()) {

                        CPrint::print("N");
                        setNotify(characteristics);
                        // characteristics->registerForNotify(notifyCallback);

                        CPrint::println("71");
                        auto listDesc = characteristics->getDescriptors();
                        CPrint::println("72");
                        if(listDesc != nullptr) {
                            CPrint::println("73");
                            for(auto& itemDesc : *listDesc) {
                                CPrint::println("74");
                                uint8_t notificationOn[2] = { 0x1, 0x0 };
                                CPrint::println("75");
                                itemDesc.second->writeValue(notificationOn, 2, true);
                                CPrint::println("76");
                            }
                        }
                    }
                }
            }
        }
    }
}*/

void CBLEClient::discoveredServices()
{
    CPrint::println("CBLEClient::discoveredServices");

    auto listServices = mClient->getServices();
    if(listServices != nullptr) {
        for(auto& p : *listServices) {

            // CPrint::print("service: ", p.first.data());

            auto listCharacteristics = p.second->getCharacteristics();
            if(listCharacteristics != nullptr) {
                for(auto& l : *listCharacteristics) {

                    // CPrint::print("\n\tcharacteristic: ");
                    // CPrint::print(l.first.data());
                    // CPrint::print("\t");

                    if(l.second->canRead()) {
                        // CPrint::print("R");
                    }

                    if(l.second->canWrite()) {
                        // CPrint::print("W");
                        mCharacteristicWrite = l.second;
                    }

                    if(l.second->canNotify()) {

                        // CPrint::print("N");

                        setNotify(l.second);

                        const uint8_t notificationOn[] = { 0x1, 0x0 };

                        auto listDesc = l.second->getDescriptors();
                        if(listDesc != nullptr)
                            for(auto& d : *listDesc)
                                d.second->writeValue((uint8_t*)notificationOn, 2, true);
                    }
                }

                // CPrint::println("");
            }
        }
    }
}

bool CBLEClient::init()
{
    CPrint::println("CBLEClient::init");

    if(mClient == nullptr) {
        mClient = BLEDevice::createClient();
        mClient->setClientCallbacks(mCallbacks);
    }

    CPrint::println("ble client: init...\tsuccess");

    return true;
}

void CBLEClient::notify()
{
    CPrint::println("CBLEClient::notify");
}

bool CBLEClient::write(const char* message, const int& size)
{
    CPrint::println("CBLEClient::write");

    if(CSettings::getInstance()->mConnectedServer == false) {

        init();
        while(connect() == false)
            CPrint::print("");

        return send(message, size);
    }

    return false;
}

bool CBLEClient::send(const char* message, const int& size)
{
    CPrint::println("CBLEClient::send");

    if(CSettings::getInstance()->mConnectedClient == true) {

        if(mCharacteristicWrite != nullptr) {

            CPrint::printMessage("ble client: send:", message, size);

            mCharacteristicWrite->writeValue(message, size);
            return true;
        }
    }

    return false;
}

bool CBLEClient::connect()
{
    CPrint::println("CBLEClient::connect");

    if(CSettings::getInstance()->mConnectedClient == false) {

        CPrint::print("ble client: connect...");
        bool result = mClient->connect(CBLEScan::getInstance()->getDevice());
        if(result == true) {
            discoveredServices();
            CSettings::getInstance()->mConnectedClient = true;
            CPrint::println("\tsuccess");
            return true;
        } else
            CPrint::println("\terror");
    }

    return true;
}

void CBLEClient::disconnect()
{
    CPrint::print("ble client: disconnect...");

    if(mClient != nullptr) {

        mClient->disconnect();
        delete mClient;
        mClient = nullptr;
        mCharacteristicWrite = nullptr;
    }

    CSettings::getInstance()->mConnectedClient = false;

    CPrint::println("\tsuccess");
}