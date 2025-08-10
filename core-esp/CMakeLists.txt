cmake_minimum_required(VERSION 3.10)

file(STRINGS "version" VERSION)

project(lib-esp-core VERSION ${VERSION} LANGUAGES C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_C_FLAGS "-MMD ${ESP_C_FLAGS} -Wall -Wextra -Os -Werror=return-type ${ESP_DEFINES} -iprefix ${PATH_ESP32_ROOT}/tools/esp32-arduino-libs/idf-release_v5.5-b66b5448-v1/esp32/include/ ${ESP_INCLUDES} ${BUILD_OPT} ${FILE_OPTS}" CACHE INTERNAL "C Compiler options")
set(CMAKE_CXX_FLAGS "-MMD ${ESP_CPP_FLAGS} -Wall -Wextra -Os -Werror=return-type ${ESP_DEFINES} -iprefix ${PATH_ESP32_ROOT}/tools/esp32-arduino-libs/idf-release_v5.5-b66b5448-v1/esp32/include/ ${ESP_INCLUDES} ${BUILD_OPT} ${FILE_OPTS}" CACHE INTERNAL "C++ Compiler options")

add_definitions(-DF_CPU=240000000L -DARDUINO=10819 -DARDUINO_ESP32_DEV -DARDUINO_ARCH_ESP32 "-DARDUINO_BOARD=\"ESP32_DEV\"" "-DARDUINO_VARIANT=\"esp32\"" -DARDUINO_PARTITION_no_ota "-DARDUINO_HOST_OS=\"windows\"" "-DARDUINO_FQBN=\"esp32:esp32:esp32:JTAGAdapter=default,PSRAM=disabled,PartitionScheme=no_ota,CPUFreq=240,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600,LoopCore=1,EventsCore=1,DebugLevel=none,EraseFlash=none,ZigbeeMode=default\"" -DESP32=ESP32 -DCORE_DEBUG_LEVEL=0 -DARDUINO_RUNNING_CORE=1 -DARDUINO_EVENT_RUNNING_CORE=1 -DARDUINO_USB_CDC_ON_BOOT=0)

include_directories("${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32")
include_directories("${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/variants/esp32")
include_directories("${PATH_ESP32_ROOT}/tools/esp32-arduino-libs/idf-release_v5.5-b66b5448-v1/esp32/qio_qspi/include")

add_library(${PROJECT_NAME} STATIC
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/esp32-hal-cpu.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/ColorFormat.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/esp32-hal-adc.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/esp32-hal-gpio.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/esp32-hal-dac.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/esp32-hal-bt.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/esp32-hal-i2c-ng.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/esp32-hal-i2c-slave.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/esp32-hal-i2c.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/esp32-hal-ledc.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/esp32-hal-matrix.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/esp32-hal-misc.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/esp32-hal-periman.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/esp32-hal-psram.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/esp32-hal-rgb-led.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/esp32-hal-rmt.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/esp32-hal-sigmadelta.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/esp32-hal-spi.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/esp32-hal-time.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/esp32-hal-timer.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/esp32-hal-tinyusb.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/esp32-hal-touch-ng.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/esp32-hal-touch.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/esp32-hal-uart.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/firmware_msc_fat.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/stdlib_noniso.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/wiring_pulse.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/wiring_shift.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/libb64/cdecode.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/libb64/cencode.c"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/Esp.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/FirmwareMSC.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/HEXBuilder.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/HWCDC.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/HardwareSerial.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/FunctionalInterrupt.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/IPAddress.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/MD5Builder.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/MacAddress.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/Print.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/SHA1Builder.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/Stream.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/StreamString.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/Tone.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/USB.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/USBCDC.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/USBMSC.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/WMath.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/WString.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/base64.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/cbuf.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/chip-debug-report.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/freertos_stats.cpp"
        "${PATH_ESP32_ROOT}/hardware/esp32/3.3.0/cores/esp32/main.cpp"
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME esp-core)