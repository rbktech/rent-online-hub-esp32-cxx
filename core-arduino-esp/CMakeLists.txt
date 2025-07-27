cmake_minimum_required(VERSION 3.10)

file(STRINGS "version" VERSION)

project(lib-arduino-esp-core VERSION ${VERSION} LANGUAGES C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_C_FLAGS "-MMD ${ESP_C_FLAGS} -Wall -Wextra -Os -Werror=return-type ${ESP_DEFINES} -iprefix ${PATH_ESP32_ROOT}/tools/esp32-arduino-libs/idf-release_v5.5-b66b5448-v1/esp32/include/ ${ESP_INCLUDES} ${BUILD_OPT} ${FILE_OPTS}" CACHE INTERNAL "C Compiler options")
set(CMAKE_CXX_FLAGS "-MMD ${ESP_CPP_FLAGS} -Wall -Wextra -Os -Werror=return-type ${ESP_DEFINES} -iprefix ${PATH_ESP32_ROOT}/tools/esp32-arduino-libs/idf-release_v5.5-b66b5448-v1/esp32/include/ ${ESP_INCLUDES} ${BUILD_OPT} ${FILE_OPTS}" CACHE INTERNAL "C++ Compiler options")

add_definitions(-DF_CPU=240000000L -DARDUINO=10819 -DARDUINO_ESP32_DEV -DARDUINO_ARCH_ESP32 "-DARDUINO_BOARD=\"ESP32_DEV\"" "-DARDUINO_VARIANT=\"esp32\"" -DARDUINO_PARTITION_no_ota "-DARDUINO_HOST_OS=\"windows\"" "-DARDUINO_FQBN=\"esp32:esp32:esp32:JTAGAdapter=default,PSRAM=disabled,PartitionScheme=no_ota,CPUFreq=240,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600,LoopCore=1,EventsCore=1,DebugLevel=none,EraseFlash=none,ZigbeeMode=default\"" -DESP32=ESP32 -DCORE_DEBUG_LEVEL=0 -DARDUINO_RUNNING_CORE=1 -DARDUINO_EVENT_RUNNING_CORE=1 -DARDUINO_USB_CDC_ON_BOOT=0)

include_directories("${USER_HOME}/AppData/Local/Arduino15/libraries/ArduinoJson/src")

include_directories("${PATH_ESP32_ROOT}/tools/esp32-arduino-libs/idf-release_v5.5-b66b5448-v1/esp32/qio_qspi/include")

include_directories("${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32")
include_directories("${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/variants/esp32")
include_directories("${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/EEPROM/src")
include_directories("${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/WiFi/src")
include_directories("${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/Network/src")
include_directories("${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src")

add_library(${PROJECT_NAME} STATIC
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/EEPROM/src/EEPROM.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/WiFi/src/STA.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/WiFi/src/WiFiAP.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/WiFi/src/WiFiMulti.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/WiFi/src/WiFiGeneric.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/WiFi/src/AP.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/WiFi/src/WiFi.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/WiFi/src/WiFiSTA.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/WiFi/src/WiFiScan.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/Network/src/NetworkInterface.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/Network/src/NetworkUdp.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/Network/src/NetworkClient.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/Network/src/NetworkManager.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/Network/src/NetworkEvents.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/Network/src/NetworkServer.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/BLE2904.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/BLE2901.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/BLEAdvertisedDevice.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/BLE2902.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/BLEAdvertising.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/BLEAddress.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/BLEBeacon.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/BLECharacteristic.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/BLECharacteristicMap.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/BLEClient.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/BLEDescriptor.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/BLEDescriptorMap.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/BLEDevice.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/BLEEddystoneTLM.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/BLEEddystoneURL.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/BLEExceptions.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/BLEHIDDevice.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/BLERemoteCharacteristic.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/BLERemoteDescriptor.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/BLERemoteService.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/BLEScan.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/BLESecurity.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/BLEServer.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/BLEService.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/BLEServiceMap.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/BLEUUID.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/BLEUtils.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/BLEValue.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/FreeRTOS.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src/GeneralUtils.cpp"
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME arduino-esp-core)