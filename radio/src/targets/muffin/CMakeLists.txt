option(SHUTDOWN_CONFIRMATION "Shutdown confirmation" OFF)
option(LCD_DUAL_BUFFER "Dual LCD Buffer" OFF)
option(AFHDS3 "AFHDS3 TX Module" OFF)
option(GHOST "Ghost TX Module" OFF)
option(INTERNAL_MODULE_PPM "Support for PPM internal module" OFF)
option(AUTOUPDATE "Auto update internal chips from SD" OFF)

set(EXTERNAL_MODULE YES)
add_definitions(-DHARDWARE_TRIMS)
add_definitions(-DFLYSKY_HALL_GIMBAL)

set(PWR_BUTTON "SWITCH" CACHE STRING "Pwr button type (PRESS/SWITCH)")
set(HAPTIC YES)

if(NOT DEFINED USE_OLED_FEATHERWING)
set(USE_OLED_FEATHERWING ON)
endif()

if(USE_OLED_FEATHERWING)
set(GUI_DIR 128x64)
set(BITMAPS_DIR ${GUI_DIR})
set(LCD_DRIVER oled_feather_wing_driver.cpp)
set(BITMAPS_TARGET 9x_bitmaps)
set(FONTS_TARGET 9x_fonts_1bit)
set(GVAR_SCREEN model_gvars.cpp)
add_definitions(-DUSE_OLED_FEATHERWING)
else()
set(GUI_DIR colorlcd)
set(BITMAPS_DIR 480x272)
set(LCD_DRIVER lcd_lvgl_esp.cpp)
add_definitions(-DCOLORLCD -DLIBOPENUI -DHARDWARE_TOUCH -DHARDWARE_KEYS)
add_definitions(-DBATTGRAPH -DTHRTRACE)
set(BITMAPS_TARGET muffin_bitmaps)
set(STORAGE_MODELSLIST YES)
endif()

set(STATUS_LEDS YES)
set(CROSSFIRE ON)
add_definitions(-DREQUIRED_SDCARD_VERSION="${SDCARD_VERSION}" -DHAPTIC -DCROSSFIRE -DBOLD_FONT) # TODO-feather
add_definitions(-DLUA_INPUTS -DTIMERS -DSOFTWARE_VOLUME) # TODO-feather
add_definitions(-DPWR_BUTTON_${PWR_BUTTON})
option(INTERNAL_MODULE_MULTI "Support for MULTI internal module" ON)
option(INTERNAL_MODULE_ELRS "Support for ELRS internal module" OFF)
option(INTERNAL_MODULE_ESPNOW "Support for ESPNOW internal module" ON)
option(INTERNAL_MODULE_BT_POWERUP "Support for Bluetooth internal module for POWERUP" ON)
set(FLAVOUR feather)
set(NAVIGATION_TYPE 9x)
set(CPU_TYPE ESP32)
set(LINKER_SCRIPT targets/muffin/ldscript.ld)
set(FIRMWARE_TARGET_SRC
  ${FIRMWARE_TARGET_SRC}
  )
set(ROTARY_ENCODER YES)
set(LUA_EXPORT lua_export_muffin)
set(BLUETOOTH NO)
set(USB_CHARGER YES)
set(SBUS ON)
add_definitions(-DDEBUG)
set(AUX_SERIAL_DRIVER ../common/esp32/aux_serial_driver.cpp)

add_definitions(-DRADIO_ZORRO)
add_definitions(-DEEPROM_VARIANT=0x4006)
add_definitions(-DMANUFACTURER_RADIOMASTER)

set(TIMER_DRIVER ../common/esp32/timers_driver.cpp)

if(GYRO)
  add_definitions(-DGYRO)
  set(TARGET_SRC
    ${TARGET_SRC}
    #../common/arm/stm32/lsm6ds_driver.cpp
    )
  set(SRC
    ${SRC}
    gyro.cpp
    )
endif()

if(INTERNAL_MODULE_SERIAL)
  add_definitions(-DINTERNAL_MODULE_SERIAL)
  if(PXX_FREQUENCY STREQUAL HIGH)
    add_definitions(-DPXX_FREQUENCY_HIGH)
  endif()
  set(TARGET_SRC
    ${TARGET_SRC}
    #../common/arm/stm32/intmodule_serial_driver.cpp
    )
elseif(INTERNAL_MODULE_PXX1 OR INTERNAL_MODULE_PXX2)
  set(TARGET_SRC
    ${TARGET_SRC}
    #intmodule_pulses_driver.cpp
    )
endif()

if(INTERNAL_MODULE_BT_POWERUP)
add_definitions(-DINTERNAL_MODULE_BT_POWERUP)
  set(TARGET_SRC
    ${TARGET_SRC}
    ../common/esp32/esp_ble_powerup.c
    ../common/esp32/pulses_bt_powerup.cpp
    )
endif()

if(INTERNAL_MODULE_ESPNOW)
add_definitions(-DINTERNAL_MODULE_ESPNOW)
  set(TARGET_SRC
    ${TARGET_SRC}
    ../common/esp32/pulses_espnow.cpp
    )
endif()

if(INTERNAL_MODULE_MULTI)
  option(DSM2 "DSM2 TX Module" ON)
  add_definitions(-DDSM2)
  option(MULTIMODULE "DIY Multiprotocol TX Module (https://github.com/pascallanger/DIY-Multiprotocol-TX-Module)" ON)
  if(MULTIMODULE)
    add_definitions(-DMULTIMODULE)
    set(SRC ${SRC} pulses/multi.cpp telemetry/spektrum.cpp telemetry/hitec.cpp telemetry/hott.cpp telemetry/mlink.cpp telemetry/multi.cpp io/multi_firmware_update.cpp)
  endif()
  add_definitions(-DHARDWARE_INTERNAL_MODULE)
  add_definitions(-DINTERNAL_MODULE_MULTI)
  add_definitions(-DINTERNAL_MODULE_SERIAL_TELEMETRY)
  set(TARGET_SRC
    ${TARGET_SRC}
    ../common/esp32/intmodule_serial_driver.cpp
    )
endif()

if(ROTARY_ENCODER)
  set(TARGET_SRC
    ${TARGET_SRC}
    #../common/arm/stm32/rotary_encoder_driver.cpp
    )
endif()

if(USB_CHARGER)
  set(TARGET_SRC ${TARGET_SRC} usb_charger_driver.cpp)
  add_definitions(-DUSB_CHARGER)
endif()

if(BLUETOOTH)
  add_definitions(-DBLUETOOTH)
  set(TARGET_SRC
    ${TARGET_SRC}
    #../common/arm/stm32/bluetooth_driver.cpp
    )
  set(SRC
    ${SRC}
    bluetooth.cpp
    )
endif()

if(AUTOUPDATE)
  add_definitions(-DAUTOUPDATE)
endif()

set(HSE_VALUE 12000000)
set(SDCARD YES)
set(EEPROM EEPROM_RAW)
set(TARGET_DIR muffin)
set(PPM_LIMITS_SYMETRICAL YES)

if(SPLASH STREQUAL OFF)
  message(FATAL_ERROR "Option SPLASH=OFF not supported on Taranis")
endif()

add_definitions(-DAUDIO -DVOICE -DRTCLOCK)
add_definitions(-DSBUS_TRAINER)

set(SRC
  ${SRC}
  main.cpp
  tasks.cpp
  audio.cpp
  haptic.cpp
  dump.cpp
  pulses/pulses.cpp
  pulses/crossfire.cpp
  telemetry/telemetry.cpp
  telemetry/telemetry_sensors.cpp
  telemetry/crossfire.cpp
  telemetry/frsky.cpp
  telemetry/frsky_d.cpp
  telemetry/frsky_sport.cpp
  telemetry/frsky_pxx2.cpp
  telemetry/flysky_ibus.cpp
  crc.cpp
  vario.cpp
  )

set(SRC
  ${SRC}
  io/frsky_firmware_update.cpp
  io/multi_firmware_update.cpp
  )

foreach(LANGUAGE ${TTS_LANGUAGES})
  string(TOLOWER ${LANGUAGE} lang_lower)
  set(SRC ${SRC} translations/tts_${lang_lower}.cpp)
endforeach()

if(USE_OLED_FEATHERWING)
set(GUI_SRC
  ${GUI_SRC}
  ${GVAR_SCREEN}
  model_input_edit.cpp
  model_mix_edit.cpp
  model_display.cpp
  radio_diagkeys.cpp
  radio_diaganas.cpp
  view_channels.cpp
  view_telemetry.cpp
  view_about.cpp
  bmp.cpp
  )
else()
if (MULTIMODULE)
  add_definitions(-DMULTI_PROTOLIST)
    set(SRC ${SRC}
      io/multi_protolist.cpp
  )
endif()
include_directories(${RADIO_SRC_DIR}/fonts/colorlcd gui/${GUI_DIR} gui/${GUI_DIR}/layouts)

file(GLOB THEMES_SRC RELATIVE ${RADIO_SRC_DIR}/gui/colorlcd ${RADIO_SRC_DIR}/gui/colorlcd/themes/*.cpp)
file(GLOB LAYOUTS_SRC RELATIVE ${RADIO_SRC_DIR}/gui/colorlcd ${RADIO_SRC_DIR}/gui/colorlcd/layouts/*.cpp)
file(GLOB WIDGETS_SRC RELATIVE ${RADIO_SRC_DIR}/gui/colorlcd ${RADIO_SRC_DIR}/gui/colorlcd/widgets/*.cpp)
endif()

if(STATUS_LEDS)
  set(LED_DRIVER led_driver.cpp)
endif()

set(SRC
  ${SRC}
  sbus.cpp
  )

set(FIRMWARE_SRC
  ${FIRMWARE_SRC}
  syscalls.c
  hal/adc_driver.cpp
  serial.cpp
  )

set(PULSES_SRC
  ${PULSES_SRC}
  pulses.cpp
  ppm.cpp
  modules_helpers.cpp
  )

if(DSM2)
  set(PULSES_SRC
    ${PULSES_SRC}
    dsm2.cpp
    )
endif()

if(SBUS)
  set(PULSES_SRC
    ${PULSES_SRC}
    sbus.cpp
    )
endif()

set(TARGET_SRC
  ${TARGET_SRC}
  ${LED_DRIVER}
  ${AUX_SERIAL_DRIVER}
  top_lcd_driver.cpp
  sh1107.cpp
  trainer_driver.cpp
  ${LCD_DRIVER}
  haptic_driver.cpp
  ../common/esp32/sound_i2s.cpp
  telemetry_driver.cpp
  ../common/esp32/diskio.cpp
  backlight_driver.cpp
  ${EEPROM_MEDIA}
  ${TIMER_DRIVER}
  flyskyHallStick_driver.cpp
  ../common/esp32/i2c_driver.cpp
  #../common/arm/stm32/audio_dac_driver.cpp
  ../common/esp32/adc_hal_driver.cpp
  #../common/arm/stm32/heartbeat_driver.cpp
  ../common/esp32/mixer_scheduler_driver.cpp
  )

set(TARGET_SRC
  ${TARGET_SRC}
  ../common/esp32/esp32_rmt_pulse_driver.cpp
)

set(FIRMWARE_TARGET_SRC
  ${FIRMWARE_TARGET_SRC}
  #${LCD_DRIVER}
  board.cpp
  keys_driver.cpp

  pwr_driver.cpp
  rtc_driver.cpp
  #../common/arm/loadboot.cpp
  )

if(LCD_DUAL_BUFFER)
  add_definitions(-DLCD_DUAL_BUFFER)
endif()


if(INTERNAL_MODULE_ELRS)
  add_definitions(-DHARDWARE_INTERNAL_MODULE)
  add_definitions(-DINTERNAL_MODULE_ELRS)
  add_definitions(-DINTERNAL_MODULE_SERIAL_TELEMETRY)
  set(INTERNAL_MODULE_MULTI NO)
  set(TARGET_SRC
    ${TARGET_SRC}
    #../common/arm/stm32/intmodule_serial_driver.cpp
    )
endif()

set(TARGET_SRC
  ${TARGET_SRC}
  ../common/esp32/ftp-server/WiFi.c
  ../common/esp32/ftp-server/initWiFi.cpp
  ../common/esp32/ftp-server/ftp.c
  ../common/esp32/ota_server/ota_server.c
  )
include_directories(${RADIO_SRC_DIR}/targets/common/esp32/ftp-server gui/${GUI_DIR} ${RADIO_SRC_DIR}/targets/common/esp32/ota_server/include)

if(INTERNAL_MODULE_PPM)
  add_definitions(-DHARDWARE_INTERNAL_MODULE)
  add_definitions(-DINTERNAL_MODULE_PPM)
endif()

if(INTERNAL_MODULE_ELRS)
  add_definitions(-DHARDWARE_INTERNAL_MODULE)
  add_definitions(-DINTERNAL_MODULE_ELRS)
endif()

if(EXTERNAL_MODULE)
  set(TARGET_SRC
    ${TARGET_SRC}
    extmodule_driver.cpp
    )
  add_definitions(-DHARDWARE_EXTERNAL_MODULE)
endif()

set(RADIO_DEPENDENCIES ${RADIO_DEPENDENCIES} ${BITMAPS_TARGET})
