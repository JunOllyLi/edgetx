idf_component_register(SRCS "espidf5_edgetx_main.cpp"
                    INCLUDE_DIRS ""
                    REQUIRES lvgl lvgl_esp32_drivers
                )

set(DISABLE_COMPANION YES)
set(GVARS YES)
set(LUA YES)
set(DEBUG YES)
set(CMAKE_BUILD_TYPE Debug)

option(EdgeTX_SUPERBUILD "Cross-compile EdgeTX for each toolchain" OFF)

add_subdirectory(${EDGETX_SRCDIR} edgetx)

target_compile_options(tx_firmware PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++17>)
target_compile_options(tx_firmware PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-Wno-deprecated-enum-enum-conversion>)
target_compile_options(tx_firmware PRIVATE -Wno-ignored-qualifiers -Wno-error=dangling-pointer= -Wno-error=format -Wno-format -Wno-missing-field-initializers -Wno-extra)

