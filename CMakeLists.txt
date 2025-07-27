cmake_minimum_required(VERSION 3.10)

project(Hub LANGUAGES C CXX ASM)

add_subdirectory(core-esp)
add_subdirectory(core-arduino-esp)
add_subdirectory(hub-esp32-ble-server)
add_subdirectory(hub-esp32-ble-server-legacy)