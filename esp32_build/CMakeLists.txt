# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

add_definitions(-DLV_CONF_KCONFIG_EXTERNAL_INCLUDE="lv_conf_edgetx.h")

set(EDGETX_SRCDIR "${CMAKE_SOURCE_DIR}/..")

list(APPEND EXTRA_COMPONENT_DIRS "${EDGETX_SRCDIR}/radio/src/thirdparty/libopenui/thirdparty/lvgl")
add_definitions(-I${CMAKE_SOURCE_DIR} -I${EDGETX_SRCDIR}/radio/src)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(PCB "MUFFIN" CACHE STRING "Project Muffin")
option(BOOTLOADER "Include Bootloader" OFF)

project(edgetx)

target_link_libraries(${project_elf} PRIVATE tx_firmware board_lib stdc++)
