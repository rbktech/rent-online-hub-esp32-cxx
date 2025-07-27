cmake_minimum_required(VERSION 3.10)

file(STRINGS "version" VERSION)

project(hub-esp32-ble-server-legacy VERSION ${VERSION} LANGUAGES C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(PORT 33)

set(CMAKE_CXX_FLAGS "-MMD ${ESP_CPP_FLAGS} -Wall -Wextra -Os -Werror=return-type ${ESP_DEFINES} -iprefix ${PATH_ESP32_ROOT}/tools/esp32-arduino-libs/idf-release_v5.5-b66b5448-v1/esp32/include/ ${ESP_INCLUDES} ${BUILD_OPT} ${FILE_OPTS}" CACHE INTERNAL "C++ Compiler options")

set(CMAKE_EXE_LINKER_FLAGS "-Wl,--Map=${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.map -Wl,--wrap=esp_panic_handler ${ESP_LD_FLAGS} ${ESP_LD_SCRIPTS} -Wl,-EL" CACHE INTERNAL "Linker options")

add_definitions(-DF_CPU=240000000L -DARDUINO=10819 -DARDUINO_ESP32_DEV -DARDUINO_ARCH_ESP32 "-DARDUINO_BOARD=\"ESP32_DEV\"" "-DARDUINO_VARIANT=\"esp32\"" -DARDUINO_PARTITION_no_ota "-DARDUINO_HOST_OS=\"windows\"" "-DARDUINO_FQBN=\"esp32:esp32:esp32:JTAGAdapter=default,PSRAM=disabled,PartitionScheme=no_ota,CPUFreq=240,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600,LoopCore=1,EventsCore=1,DebugLevel=none,EraseFlash=none,ZigbeeMode=default\"" -DESP32=ESP32 -DCORE_DEBUG_LEVEL=0 -DARDUINO_RUNNING_CORE=1 -DARDUINO_EVENT_RUNNING_CORE=1 -DARDUINO_USB_CDC_ON_BOOT=0)

include_directories("src")
include_directories("src/queue")
include_directories("src/sample")

include_directories("${USER_HOME}/AppData/Local/Arduino15/packages/libraries/ArduinoJson/src")

include_directories("${PATH_ESP32_ROOT}/tools/esp32-arduino-libs/idf-release_v5.5-b66b5448-v1/esp32/qio_qspi/include")

include_directories("${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32")
include_directories("${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/variants/esp32")
include_directories("${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/EEPROM/src")
include_directories("${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/WiFi/src")
include_directories("${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/Network/src")
include_directories("${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/libraries/BLE/src")

link_directories("${PATH_ESP32_ROOT}/tools/esp32-arduino-libs/idf-release_v5.5-b66b5448-v1/esp32/lib")
link_directories("${PATH_ESP32_ROOT}/tools/esp32-arduino-libs/idf-release_v5.5-b66b5448-v1/esp32/ld")
link_directories("${PATH_ESP32_ROOT}/tools/esp32-arduino-libs/idf-release_v5.5-b66b5448-v1/esp32/qio_qspi")

link_directories("$ENV{CACHE}/hubs/${CMAKE_BUILD_TYPE}/core-esp")
link_directories("$ENV{CACHE}/hubs/${CMAKE_BUILD_TYPE}/core-arduino-esp")

#*****************************

add_executable(${PROJECT_NAME}
        src/main-server.cpp
)

target_link_libraries(${PROJECT_NAME} arduino-esp-core)
target_link_libraries(${PROJECT_NAME} esp-core)
target_link_libraries(${PROJECT_NAME} "-Wl,--start-group ${ESP_LD_LIBS} -Wl,--end-group")

#*****************************

configure_file("${PATH_ESP32_ROOT}/hardware/esp32/3.0.0-alpha3/tools/partitions/no_ota.csv" "${CMAKE_CURRENT_BINARY_DIR}/partitions.csv" COPYONLY)

#*****************************

add_custom_target(${PROJECT_NAME}_bin ${ESPTOOL} --chip esp32 elf2image --flash_mode dio --flash_freq 80m --flash_size 4MB --elf-sha256-offset 0xb0 -o "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.elf"
        DEPENDS ${PROJECT_NAME}
)

add_custom_target(${PROJECT_NAME}_part ${ESPPART} -q "${CMAKE_CURRENT_BINARY_DIR}/partitions.csv" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.partitions.bin"
        DEPENDS
)

add_custom_target(${PROJECT_NAME}_bootloader ${ESPTOOL} --chip esp32 elf2image --flash_mode dio --flash_freq 80m --flash_size 4MB -o "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bootloader.bin" "${PATH_ESP32_ROOT}/tools/esp32-arduino-libs/idf-release_v5.5-b66b5448-v1/esp32/bin/bootloader_qio_80m.elf"
        DEPENDS
)

add_custom_target(${PROJECT_NAME}_merge ${ESPTOOL} --chip esp32 merge_bin -o "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.merged.bin" --fill-flash-size 4MB --flash_mode keep --flash_freq keep --flash_size keep 0x1000 "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bootloader.bin" 0x8000 "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.partitions.bin" 0xe000 "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/tools/partitions/boot_app0.bin" 0x10000 "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin"
        DEPENDS ${PROJECT_NAME}_bootloader ${PROJECT_NAME}_part ${PROJECT_NAME}_bin
)

add_custom_target(${PROJECT_NAME}_size ${ESPSIZE} -A "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.elf"
        DEPENDS ${PROJECT_NAME}
)

add_custom_target(${PROJECT_NAME}_flash ${ESPTOOL} --chip esp32 --port COM${PORT} --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode keep --flash_freq keep --flash_size keep 0x1000 "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bootloader.bin" 0x8000 "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.partitions.bin" 0xe000 "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/tools/partitions/boot_app0.bin" 0x10000 "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin"
        DEPENDS ${PROJECT_NAME}_bootloader ${PROJECT_NAME}_part ${PROJECT_NAME}_bin
)