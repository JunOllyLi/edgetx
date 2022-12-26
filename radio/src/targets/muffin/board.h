/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _BOARD_H_
#define _BOARD_H_

#include <inttypes.h>
#include "definitions.h"
#include "opentx_constants.h"
#include "hal.h"

#define SYSTEM_TICKS_1MS 1

enum usbMode {
  USB_UNSELECTED_MODE,
  USB_JOYSTICK_MODE,
  USB_MASS_STORAGE_MODE,
  USB_SERIAL_MODE,
#if defined(USB_SERIAL)
  USB_MAX_MODE=USB_SERIAL_MODE
#else
  USB_MAX_MODE=USB_MASS_STORAGE_MODE
#endif
};
#define getSelectedUsbMode() USB_UNSELECTED_MODE
#define setSelectedUsbMode(x);

#define pwrOffPressed() false

bool sportGetByte(uint8_t * byte);
// ADC driver
uint16_t getAnalogValue(uint8_t index);
uint16_t getBatteryVoltage();   // returns current battery voltage in 10mV steps
uint16_t getRTCBatteryVoltage();
void enableVBatBridge();
void disableVBatBridge();
bool isVBatBridgeEnabled();
void NVIC_SystemReset(void);
int usbPlugged();

// TODO-feather
#define MIXSRC_3POS 0

typedef struct {
  int pin;
} GPIO_TypeDef;
#define GPIO_ReadInputDataBit(x, y) (0)

#define backlightFullOn()

#if defined(RADIO_TX12) || defined(RADIO_ZORRO)
  #define  NAVIGATION_X7_TX12
#endif

#if defined(ROTARY_ENCODER_NAVIGATION)
// Rotary Encoder driver
void rotaryEncoderInit();
void rotaryEncoderCheck();
#endif

#define BACKLIGHT_LEVEL_MAX     100
#define BACKLIGHT_LEVEL_MIN     10

#define FLASHSIZE                       0x80000
#define BOOTLOADER_SIZE                 0x8000
#define FIRMWARE_ADDRESS                0x08000000

#define LUA_MEM_EXTRA_MAX               (0)    // max allowed memory usage for Lua bitmaps (in bytes)
#define LUA_MEM_MAX                     (0)    // max allowed memory usage for complete Lua  (in bytes), 0 means unlimited

#if defined(STM32F4)
  #define PERI1_FREQUENCY               42000000
  #define PERI2_FREQUENCY               84000000
#else
  #define PERI1_FREQUENCY               30000000
  #define PERI2_FREQUENCY               60000000
#endif

#define TIMER_MULT_APB1                 2
#define TIMER_MULT_APB2                 2

extern uint16_t sessionTimer;

// Board driver
void boardInit();
void boardOff();

// Timers driver
void init2MhzTimer();
void init5msTimer();
uint32_t ticksNow();

// PCBREV driver
enum {
  // X7
  PCBREV_X7_STD = 0,
  PCBREV_X7_40 = 1,
};

// SD driver
#define BLOCK_SIZE                      512 /* Block Size in Bytes */
#if !defined(SIMU) || defined(SIMU_DISKIO)
uint32_t sdIsHC();
uint32_t sdGetSpeed();
#define SD_IS_HC()                      (sdIsHC())
#define SD_GET_SPEED()                  (sdGetSpeed())
#define SD_GET_FREE_BLOCKNR()           (sdGetFreeSectors())
#else
#define SD_IS_HC()                      (0)
#define SD_GET_SPEED()                  (0)
#endif
#define __disk_read                     disk_read
#define __disk_write                    disk_write
#if defined(SIMU)
  #if !defined(SIMU_DISKIO)
    #define sdInit()
    #define sdDone()
  #endif
  #define sdMount()
  #define SD_CARD_PRESENT()               true
#else
void sdInit();
void sdMount();
void sdDone();
void sdPoll10ms();
uint32_t sdMounted();
bool SD_CARD_PRESENT(void);
#endif

// Flash Write driver
#define FLASH_PAGESIZE 256
void unlockFlash();
void lockFlash();
void flashWrite(uint32_t * address, const uint32_t * buffer);
uint32_t isFirmwareStart(const uint8_t * buffer);
uint32_t isBootloaderStart(const uint8_t * buffer);

// Pulses driver
void INTERNAL_MODULE_ON(void);
void INTERNAL_MODULE_OFF(void);
bool IS_INTERNAL_MODULE_ON(void);

#if (defined(INTERNAL_MODULE_PXX1) || defined(INTERNAL_MODULE_PXX2)) && (!defined(PCBX9LITE) || defined(PCBX9LITES))
  #define HARDWARE_INTERNAL_RAS
#endif

#define EXTERNAL_MODULE_ON()            EXTERNAL_MODULE_PWR_ON()

#if defined(EXTMODULE_USART)
  #define EXTERNAL_MODULE_OFF()         EXTERNAL_MODULE_PWR_OFF()
#else
  #define EXTERNAL_MODULE_OFF()         EXTERNAL_MODULE_PWR_OFF()
#endif

void intmoduleSerialStart(uint32_t baudrate, uint8_t rxEnable, uint16_t parity, uint16_t stopBits, uint16_t wordLength);
#if defined(INTERNAL_MODULE_MULTI)
void intmoduleTimerStart(uint32_t periodMs);
#endif
void intmoduleSendByte(uint8_t byte);
void intmoduleSendBuffer(const uint8_t * data, uint8_t size);
void intmoduleSendNextFrame();

void extmoduleSerialStart();
void extmoduleInvertedSerialStart(uint32_t baudrate);
void extmoduleSendBuffer(const uint8_t * data, uint8_t size);
void extmoduleSendNextFrame();
void extmoduleSendInvertedByte(uint8_t byte);

#if defined(RADIO_ZORRO)
#define ELRS_INTERNAL_BAUDRATE        5250000     // 5.25 Mbps
#elif defined(RADIO_TPRO)
#define ELRS_INTERNAL_BAUDRATE        1870000     // 1.87 Mbps
#else
#define ELRS_INTERNAL_BAUDRATE        400000
#endif


// Trainer driver
#define SLAVE_MODE()                    (g_model.trainerData.mode == TRAINER_MODE_SLAVE)

#if defined(TRAINER_DETECT_GPIO)
  // Trainer detect is a switch on the jack
  #define TRAINER_CONNECTED()           (GPIO_ReadInputDataBit(TRAINER_DETECT_GPIO, TRAINER_DETECT_GPIO_PIN) == TRAINER_DETECT_GPIO_PIN_VALUE)
#elif defined(PCBXLITES)
  // Trainer is on the same connector than Headphones
  enum JackState
  {
    SPEAKER_ACTIVE,
    HEADPHONE_ACTIVE,
    TRAINER_ACTIVE,
  };
  extern uint8_t jackState;
  #define TRAINER_CONNECTED()           (jackState == TRAINER_ACTIVE)
#elif defined(PCBXLITE)
  // No Tainer jack on Taranis X-Lite
  #define TRAINER_CONNECTED()           false
#else
  // Trainer detect catches PPM, detection would use more CPU
  #define TRAINER_CONNECTED()           true
#endif

#if defined(TRAINER_GPIO)
  void init_trainer_ppm();
  void stop_trainer_ppm();
  void init_trainer_capture();
  void stop_trainer_capture();
#else
  #define init_trainer_ppm()
  #define stop_trainer_ppm()
  #define init_trainer_capture()
  #define stop_trainer_capture()
#endif
#if defined(TRAINER_MODULE_CPPM)
  void init_trainer_module_cppm();
  void stop_trainer_module_cppm();
#else
  #define init_trainer_module_cppm()
  #define stop_trainer_module_cppm()
#endif
#if defined(TRAINER_MODULE_SBUS)
  void init_trainer_module_sbus();
  void stop_trainer_module_sbus();
#else
  #define init_trainer_module_sbus()
  #define stop_trainer_module_sbus()
#endif

#if defined(INTMODULE_HEARTBEAT_GPIO)
void init_intmodule_heartbeat();
void stop_intmodule_heartbeat();
void check_intmodule_heartbeat();
#else
#define init_intmodule_heartbeat()
#define stop_intmodule_heartbeat()
#define check_intmodule_heartbeat()
#endif

void check_telemetry_exti();

#define KEYS_GPIO_REG_MENU
#define KEYS_GPIO_REG_ENTER
#define KEYS_GPIO_REG_UP
#define KEYS_GPIO_REG_DOWN
#define KEYS_GPIO_REG_EXIT

#define TMR_5MS_CORE 0
#define TRAINER_PPM_OUT_TASK_CORE 0
#define MIXER_TASK_CORE 0
#define PULSES_TASK_CORE 0
#define MENU_TASK_CORE 1
#define AUDIO_TASK_CORE 1

/*
From Kconfig
  LCD D0 - D7: 14, 13, 12, 11, 10, 9, 46, 3
  LCD CS 45
  LCD DC 48
  LCD WR 21

#define I2C_SCL 40
#define I2C_SDA 39

//  MOSI -1
//  MISO -1
//  RESET -1
//  SCLK -1
//  TOUCH CS -1
//  TOUCH IRQ -1
*/

#define POT1_ADC_CHANNEL ADC_CHANNEL_5  // GPIO 6
#define POT2_ADC_CHANNEL ADC_CHANNEL_4  // GPIO 5
#define BATT_ADC_CHANNEL ADC_CHANNEL_6  // GPIO 7

#define BACKLITE_PIN 4
#define RMT_TX_PIN 41
#define TRAINER_IN_GPIO 42
#define FLYSKY_UART_RX_PIN 44

#define INTMOD_UART_PORT UART_NUM_2
#define INTMOD_RX_PIN 2
#define INTMOD_TX_PIN 1

#define I2C_MASTER_NUM (i2c_port_t)0

#define SD_DEDICATED_SPI
#ifdef SD_DEDICATED_SPI
#define SD_SPI_HOST SPI2_HOST
#define SDSPI_CLK 47
#define SDSPI_MOSI 0
#define SDSPI_MISO 15
#endif
#define SDCARD_CS_GPIO 38

#define I2S_DOUT 18
#define I2S_BCLK 17
#define I2S_LRCLK 16

// Keys driver
// on Adafruit_MCP23X17
enum EnumKeys
{
  KEY_ENTER,
  KEY_MENU = KEY_ENTER,
  KEY_EXIT,
  KEY_UP,   // must match the order of the button above
  KEY_PLUS = KEY_UP,
  KEY_DOWN,
  KEY_MINUS = KEY_DOWN,
  BUTTONS_ON_GPIOA = KEY_DOWN,
  KEY_RIGHT,
  KEY_LEFT,
  KEY_RADIO,
  BUTTONS_ON_MCP1 = KEY_RADIO,
  KEY_BIND,
  KEY_TELEM,

  KEY_COUNT,
  KEY_MAX = KEY_COUNT - 1,

  TRM_BASE,
  TRM_LH_DWN = TRM_BASE,
  TRM_LH_UP,
  TRM_LV_DWN,
  TRM_LV_UP,
  TRM_RV_DWN,
  TRM_RV_UP,
  TRM_RH_DWN,
  TRM_RH_UP,
  TRM_LAST = TRM_RH_UP,

  NUM_KEYS
};

#define KEYS_GPIO_REG_PGUP
#define KEYS_GPIO_REG_PGDN

#if defined(COLORLCD)
  #define KEY_RADIO                     KEY_RIGHT
  #define KEY_MODEL                     KEY_ENTER
  #define KEY_PGUP                      KEY_UP
  #define KEY_PGDN                      KEY_DOWN
#endif

#if defined(KEYS_GPIO_PIN_SHIFT)
#define IS_SHIFT_KEY(index)             (index == KEY_SHIFT)
#if defined(SIMU)
#define IS_SHIFT_PRESSED()              (readKeys() & (1 << KEY_SHIFT))
#else
#define IS_SHIFT_PRESSED()              (~KEYS_GPIO_REG_SHIFT & KEYS_GPIO_PIN_SHIFT)
#endif
#else
#define IS_SHIFT_KEY(index)             (false)
#define IS_SHIFT_PRESSED()              (false)
#endif

#define IS_TOGGLE(sw)                  (0)
#define IS_ROTARY_ENCODER_NAVIGATION_ENABLE() (0)

enum EnumSwitches
{
  SW_SA,
  SW_SB,
  SW_SC,
  SW_SD,
  SW_SE,
  SW_SF,
  SW_SG,
  SW_SH,
  SW_SI,
  SW_SJ,
  SW_SK,
  SW_SL,
  SW_SM,
  SW_SO,
  SW_SP,
  SW_SQ,
  SW_SR,
};

#define IS_3POS(x)                      ((x) == SW_SC)

enum EnumSwitchesPositions
{
  SW_SA0,
  SW_SA1,
  SW_SA2,
  SW_SB0,
  SW_SB1,
  SW_SB2,
  SW_SC0,
  SW_SC1,
  SW_SC2,
  SW_SD0,
  SW_SD1,
  SW_SD2,
  SW_SE0,
  SW_SE1,
  SW_SE2,
  SW_SF0,
  SW_SF1,
  SW_SF2,
  SW_SG0,
  SW_SG1,
  SW_SG2,
  SW_SH0,
  SW_SH1,
  SW_SH2,
  NUM_SWITCHES_POSITIONS
};

  #define NUM_SWITCHES                  8
  #define STORAGE_NUM_SWITCHES          NUM_SWITCHES
  #define DEFAULT_SWITCH_CONFIG         (SWITCH_NONE << 14) + (SWITCH_NONE << 12) + (SWITCH_NONE << 10) + (SWITCH_NONE << 8) + (SWITCH_2POS << 6) + (SWITCH_3POS << 4) + (SWITCH_2POS << 2) + (SWITCH_2POS << 0)
  #define DEFAULT_POTS_CONFIG           (POT_WITHOUT_DETENT << 0) + (POT_WITHOUT_DETENT << 2);

#if !defined(NUM_FUNCTIONS_SWITCHES)
  #define NUM_FUNCTIONS_SWITCHES        0
#endif

#define STORAGE_NUM_SWITCHES_POSITIONS  (STORAGE_NUM_SWITCHES * 3)

void keysInit();
uint32_t switchState(uint8_t index);
uint32_t readKeys();
uint32_t readTrims();
#if defined(FUNCTION_SWITCHES)
extern uint8_t fsPreviousState;
void evalFunctionSwitches();
void setFSStartupPosition();
uint8_t getFSLogicalState(uint8_t index);
uint8_t getFSPhysicalState(uint8_t index);
#endif

#define TRIMS_PRESSED()                 (readTrims())
#define KEYS_PRESSED()                  (readKeys())

// WDT driver
#define WDG_DURATION                      500 /*ms*/
  #define WDG_ENABLE(x) // TODO-feather
  #define WDG_RESET()
void watchdogInit(unsigned int duration);
#define WAS_RESET_BY_SOFTWARE()            false `// TODO-feather 
#define WAS_RESET_BY_WATCHDOG()            false // TODO-feather 
#define WAS_RESET_BY_WATCHDOG_OR_SOFTWARE() false // TODO-feather 

#define USART_Parity_No                      ((uint16_t)0x0000)
#define USART_Parity_Even                    ((uint16_t)0x0400)
#define USART_Parity_Odd                     ((uint16_t)0x0600) 
#define IS_USART_PARITY(PARITY) (((PARITY) == USART_Parity_No) || \
                                 ((PARITY) == USART_Parity_Even) || \
                                 ((PARITY) == USART_Parity_Odd))
#define USART_StopBits_1                     ((uint16_t)0x0000)
#define USART_StopBits_0_5                   ((uint16_t)0x1000)
#define USART_StopBits_2                     ((uint16_t)0x2000)
#define USART_StopBits_1_5                   ((uint16_t)0x3000)
#define IS_USART_STOPBITS(STOPBITS) (((STOPBITS) == USART_StopBits_1) || \
                                     ((STOPBITS) == USART_StopBits_0_5) || \
                                     ((STOPBITS) == USART_StopBits_2) || \
                                     ((STOPBITS) == USART_StopBits_1_5))
#define USART_WordLength_8b                  ((uint16_t)0x0000)
#define USART_WordLength_9b                  ((uint16_t)0x1000)
                                    
#define IS_USART_WORD_LENGTH(LENGTH) (((LENGTH) == USART_WordLength_8b) || \
                                      ((LENGTH) == USART_WordLength_9b))

// ADC driver
enum Analogs {
  STICK1,
  STICK2,
  STICK3,
  STICK4,
  POT_FIRST,
  POT1 = POT_FIRST,
  POT2,
  POT_LAST = POT2,
  TX_VOLTAGE,
  TX_RTC_VOLTAGE,
  NUM_ANALOGS
};

  #define NUM_POTS                      2
  #define NUM_SLIDERS                   0
  #define STORAGE_NUM_POTS              2
  #define STORAGE_NUM_SLIDERS           0

#define NUM_XPOTS                       0 //STORAGE_NUM_POTS
#define NUM_TRIMS                       4
#define NUM_MOUSE_ANALOGS               0
#define STORAGE_NUM_MOUSE_ANALOGS       0

#if defined(PCBXLITE)
  #define NUM_TRIMS_KEYS                4
#else
  #define NUM_TRIMS_KEYS                (NUM_TRIMS * 2)
#endif

#if defined(STICKS_PWM)
  #define NUM_PWMSTICKS                 4
  #define STICKS_PWM_ENABLED()          (!hardwareOptions.sticksPwmDisabled)
  void sticksPwmInit();
  void sticksPwmRead(uint16_t * values);
  extern volatile uint32_t pwm_interrupt_count; // TODO => reusable buffer (boot section)
#else
  #define STICKS_PWM_ENABLED()          false
#endif

PACK(typedef struct {
  uint8_t pcbrev:2;
  uint8_t sticksPwmDisabled:1;
  uint8_t pxx2Enabled:1;
}) HardwareOptions;

extern HardwareOptions hardwareOptions;

#if !defined(PXX2)
  #define IS_PXX2_INTERNAL_ENABLED()            (false)
  #define IS_PXX1_INTERNAL_ENABLED()            (true)
#elif !defined(PXX1) || defined(PCBXLITES) || defined(PCBX9LITE)
  #define IS_PXX2_INTERNAL_ENABLED()            (true)
  #define IS_PXX1_INTERNAL_ENABLED()            (false)
#elif defined(INTERNAL_MODULE_PXX1)
  #define IS_PXX2_INTERNAL_ENABLED()            (false)
  #define IS_PXX1_INTERNAL_ENABLED()            (true)
#else
  // TODO #define PXX2_PROBE
  // TODO #define IS_PXX2_INTERNAL_ENABLED()            (hardwareOptions.pxx2Enabled)
  #define IS_PXX2_INTERNAL_ENABLED()            (true)
  #define IS_PXX1_INTERNAL_ENABLED()            (true)
#endif

enum CalibratedAnalogs {
  CALIBRATED_STICK1,
  CALIBRATED_STICK2,
  CALIBRATED_STICK3,
  CALIBRATED_STICK4,
  CALIBRATED_POT_FIRST,
  CALIBRATED_POT1 = CALIBRATED_POT_FIRST,
  CALIBRATED_POT2,
  CALIBRATED_POT_LAST = CALIBRATED_POT2,
  CALIBRATED_SLIDER_FIRST,
  CALIBRATED_SLIDER_LAST = CALIBRATED_SLIDER_FIRST + NUM_SLIDERS - 1,
  NUM_CALIBRATED_ANALOGS
};

#if defined(PCBX9D)
  #define IS_POT(x)                     ((x)>=POT_FIRST && (x)<=POT2) // POT3 is only defined in software
#else
  #define IS_POT(x)                     ((x)>=POT_FIRST && (x)<=POT_LAST)
#endif

#define IS_SLIDER(x)                    ((x)>POT_LAST && (x)<TX_VOLTAGE)

extern uint16_t adcValues[NUM_ANALOGS];

// Battery driver
#if defined(PCBX9E)
  // NI-MH 9.6V
  #define BATTERY_WARN                  87 // 8.7V
  #define BATTERY_MIN                   85 // 8.5V
  #define BATTERY_MAX                   115 // 11.5V
#elif defined(PCBXLITE)
  // 2 x Li-Ion
  #define BATTERY_WARN                  66 // 6.6V
  #define BATTERY_MIN                   67 // 6.7V
  #define BATTERY_MAX                   83 // 8.3V
  #define BATTERY_TYPE_FIXED
#elif defined(RADIO_T8) || defined(RADIO_TLITE)
  // 1S Li-ion /  Lipo, LDO for 3.3V
  #define BATTERY_WARN                  35 // 3.5V
  #define BATTERY_MIN                   34 // 3.4V
  #define BATTERY_MAX                   42 // 4.2V
  #define BATTERY_TYPE_FIXED
#else
  // NI-MH 7.2V
  #define BATTERY_WARN                  65 // 6.5V
  #define BATTERY_MIN                   60 // 6.0V
  #define BATTERY_MAX                   80 // 8.0V
#endif

#if defined(PCBXLITE)
  #define BATT_SCALE                    131
#elif defined(PCB_MUFFIN) || defined(PCBX7)
  #define BATT_SCALE                    123
#elif defined(PCBX9LITE)
  #define BATT_SCALE                    117
#elif defined(RADIO_X9DP2019)
  #define BATT_SCALE                    117
#else
  #define BATT_SCALE                    150
#endif

#if defined(__cplusplus) && !defined(SIMU)
extern "C" {
#endif

// Power driver
#define SOFT_PWR_CTRL
void pwrInit();
uint32_t pwrCheck();
void pwrOn();
void pwrOff();
bool pwrPressed();
#if defined(PWR_BUTTON_PRESS)
#define STARTUP_ANIMATION
uint32_t pwrPressedDuration();
#endif
void pwrResetHandler();
#define pwrForcePressed()   false

#if defined(SIMU)
#define UNEXPECTED_SHUTDOWN()           false
#else
#define UNEXPECTED_SHUTDOWN()           (WAS_RESET_BY_WATCHDOG() || g_eeGeneral.unexpectedShutdown)
#endif

struct TouchState getInternalTouchState();
struct TouchState touchPanelRead();
bool touchPanelEventOccured();

// Backlight driver
void backlightInit();
void backlightDisable();
#define BACKLIGHT_DISABLE()             backlightDisable()
#define BACKLIGHT_FORCED_ON             101
uint8_t isBacklightEnabled();
#if !defined(__cplusplus)
  #define backlightEnable(...)
#elif defined(PCBX9E) || defined(PCBX9DP)
  void backlightEnable(uint8_t level = 0, uint8_t color = 0);
  #define BACKLIGHT_ENABLE()            backlightEnable(currentBacklightBright, g_eeGeneral.backlightColor)
#else
  void backlightEnable(uint8_t level = 0);
  #define BACKLIGHT_ENABLE()            backlightEnable(currentBacklightBright)
#endif

#if !defined(SIMU)
  void usbJoystickUpdate();
#endif
#if defined(RADIO_TX12)
  #define USB_NAME                     "Radiomaster TX12"
  #define USB_MANUFACTURER             'R', 'M', '_', 'T', 'X', ' ', ' ', ' '  /* 8 bytes */
  #define USB_PRODUCT                  'R', 'M', ' ', 'T', 'X', '1', '2', ' '  /* 8 Bytes */
#elif defined(RADIO_ZORRO)
  #define USB_NAME                     "Radiomaster Zorro"
  #define USB_MANUFACTURER             'R', 'M', '_', 'T', 'X', ' ', ' ', ' '  /* 8 bytes */
  #define USB_PRODUCT                  'R', 'M', ' ', 'Z', 'O', 'R', 'R', 'O'  /* 8 Bytes */
#elif defined(RADIO_T8)
  #define USB_NAME                     "Radiomaster T8"
  #define USB_MANUFACTURER             'R', 'M', '_', 'T', 'X', ' ', ' ', ' '  /* 8 bytes */
  #define USB_PRODUCT                  'R', 'M', ' ', 'T', '8', ' ', ' ', ' '  /* 8 Bytes */
#elif defined(RADIO_TLITE)
  #define USB_NAME                     "Jumper TLite"
  #define USB_MANUFACTURER             'J', 'U', 'M', 'P', 'E', 'R', ' ', ' '  /* 8 bytes */
  #define USB_PRODUCT                  'T', '-', 'L', 'I', 'T', 'E', ' ', ' '  /* 8 Bytes */
#elif defined(RADIO_TPRO)
  #define USB_NAME                     "Jumper TPRO"
  #define USB_MANUFACTURER             'J', 'U', 'M', 'P', 'E', 'R', ' ', ' '  /* 8 bytes */
  #define USB_PRODUCT                  'T', '-', 'P', 'R', 'O', ' ', ' ', ' '  /* 8 Bytes */
#else
  #define USB_NAME                     "FrSky Taranis"
  #define USB_MANUFACTURER             'F', 'r', 'S', 'k', 'y', ' ', ' ', ' '  /* 8 bytes */
  #define USB_PRODUCT                  'T', 'a', 'r', 'a', 'n', 'i', 's', ' '  /* 8 Bytes */
#endif

#if defined(__cplusplus) && !defined(SIMU)
}
#endif

// I2C driver: EEPROM + Audio Volume
#define EEPROM_SIZE                   (32*1024)

void i2cInit();


#define EEPROM_BLOCK_SIZE 256
int eepromInit(void);
void eepromReadBlock(uint8_t * buffer, size_t address, size_t size);
void eepromStartWrite(uint8_t * buffer, size_t address, size_t size);
void eepromStartRead(uint8_t * buffer, size_t address, size_t size);
uint8_t eepromIsTransferComplete();
uint8_t eepromReadStatus();
void eepromBlockErase(uint32_t address);

// Debug driver
void debugPutc(const char c);

// Telemetry driver
void telemetryPortInit(uint32_t baudrate, uint8_t mode);
void telemetryPortSetDirectionInput();
void telemetryPortSetDirectionOutput();
void telemetryClearFifo();
void sportSendByte(uint8_t byte);
void sportSendByteLoop(uint8_t byte);
void sportStopSendByteLoop();
void sportSendBuffer(const uint8_t * buffer, uint32_t count);
extern uint32_t telemetryErrors;

// soft-serial
void telemetryPortInvertedInit(uint32_t baudrate);

// PCBREV driver
#if defined(PCBX7ACCESS)
  #define HAS_SPORT_UPDATE_CONNECTOR()  true
#elif defined(PCB_MUFFIN) || defined(PCBX7)
  #define IS_PCBREV_40()                (hardwareOptions.pcbrev == PCBREV_X7_40)
  #define HAS_SPORT_UPDATE_CONNECTOR()  IS_PCBREV_40()
#elif defined(SPORT_UPDATE_PWR_GPIO)
  #define HAS_SPORT_UPDATE_CONNECTOR()  true
#else
  #define HAS_SPORT_UPDATE_CONNECTOR()  false
#endif

// Sport update driver
#if defined(SPORT_UPDATE_PWR_GPIO)
void sportUpdateInit();
void sportUpdatePowerOn();
void sportUpdatePowerOff();
void sportUpdatePowerInit();
#define SPORT_UPDATE_POWER_ON()         sportUpdatePowerOn()
#define SPORT_UPDATE_POWER_OFF()        sportUpdatePowerOff()
#define SPORT_UPDATE_POWER_INIT()       sportUpdatePowerInit()
#define IS_SPORT_UPDATE_POWER_ON()      (GPIO_ReadInputDataBit(SPORT_UPDATE_PWR_GPIO, SPORT_UPDATE_PWR_GPIO_PIN) == Bit_SET)
#else
#define sportUpdateInit()
#define SPORT_UPDATE_POWER_ON()
#define SPORT_UPDATE_POWER_OFF()
#define SPORT_UPDATE_POWER_INIT()
#define IS_SPORT_UPDATE_POWER_ON()      (false)
#endif

// Audio driver
void audioInit() ;
void audioEnd() ;
void dacStart();
void dacStop();
#define VOLUME_LEVEL_MAX  23
#define VOLUME_LEVEL_DEF  12
#if !defined(SOFTWARE_VOLUME)
void setScaledVolume(uint8_t volume);
void setVolume(uint8_t volume);
int32_t getVolume();
#endif
#if defined(AUDIO_SPEAKER_ENABLE_GPIO)
void initSpeakerEnable();
void enableSpeaker();
void disableSpeaker();
#else
static inline void initSpeakerEnable() { }
static inline void enableSpeaker() { }
static inline void disableSpeaker() { }
#endif
#if defined(HEADPHONE_TRAINER_SWITCH_GPIO)
void initHeadphoneTrainerSwitch();
void enableHeadphone();
void enableTrainer();
#else
static inline void initHeadphoneTrainerSwitch() { }
static inline void enableHeadphone() { }
static inline void enableTrainer() { }
#endif
#if defined(JACK_DETECT_GPIO)
void initJackDetect();
bool isJackPlugged();
#endif
void audioInit();
void setSampleRate(uint32_t frequency);
void audioConsumeCurrentBuffer();

#define audioDisableIrq()               taskDISABLE_INTERRUPTS()
#define audioEnableIrq()                taskENABLE_INTERRUPTS()

// Haptic driver
void hapticInit();
void hapticOff();
#if defined(HAPTIC_PWM)
  void hapticOn(uint32_t pwmPercent);
#else
  void hapticOn();
#endif

// Aux serial port driver
#if defined(AUX_SERIAL_GPIO)
#define DEBUG_BAUDRATE                  400000
#define LUA_DEFAULT_BAUDRATE            115200
#define AUX_SERIAL
extern uint8_t auxSerialMode;
#if defined __cplusplus
void auxSerialSetup(unsigned int baudrate, bool dma, uint16_t length=8, uint16_t parity=0, uint16_t stop=0); // TODO-feather
#endif
void auxSerialInit(unsigned int mode, unsigned int protocol);
void auxSerialPutc(char c);
#define auxSerialTelemetryInit(protocol) auxSerialInit(UART_MODE_TELEMETRY, protocol)
void auxSerialSbusInit();
void auxSerialStop();
#define AUX_SERIAL_POWER_ON()
#define AUX_SERIAL_POWER_OFF()
#endif

// BT driver
#define BLUETOOTH_BOOTLOADER_BAUDRATE   230400
#define BLUETOOTH_DEFAULT_BAUDRATE      115200
#if defined(PCBX9E)
#define BLUETOOTH_FACTORY_BAUDRATE      9600
#else
#define BLUETOOTH_FACTORY_BAUDRATE      57600
#endif
#define BT_TX_FIFO_SIZE    64
#define BT_RX_FIFO_SIZE    256
void bluetoothInit(uint32_t baudrate, bool enable);
void bluetoothWriteWakeup();
uint8_t bluetoothIsWriting();
void bluetoothDisable();
#if defined(PCBX9LITES) || defined(PCBX7ACCESS)
  #define IS_BLUETOOTH_CHIP_PRESENT()     (true)
#elif defined(PCBX9LITE)
  #define IS_BLUETOOTH_CHIP_PRESENT()     (false)
#elif defined(BLUETOOTH_PROBE) && !defined(SIMU)
  extern volatile uint8_t btChipPresent;
  #define IS_BLUETOOTH_CHIP_PRESENT()     (btChipPresent)
#else
  #define IS_BLUETOOTH_CHIP_PRESENT()     (true)
#endif

// USB Charger
#if defined(USB_CHARGER)
void usbChargerInit();
bool usbChargerLed();
#endif

// LED driver
void ledInit();
void ledOff();
void ledRed();
void ledGreen();
void ledBlue();
#if defined(FUNCTION_SWITCHES)
void fsLedOff(uint8_t);
void fsLedOn(uint8_t);
#endif

// LCD driver
#if defined(USE_OLED_FEATHERWING)
#define LCD_W                           128
#define LCD_H                           64
#define LCD_DEPTH                       1
void lcdRefresh(bool wait=true); // TODO uint8_t wait to simplify this
#else
#if !defined(CONFIG_LV_TFT_DISPLAY_CONTROLLER_RA8875)
#define LCD_W                           480
#define LCD_H                           320
#else
#define LCD_W                           480
#define LCD_H                           272
#endif
#define LCD_DEPTH                       16
void lcdRefresh();
bool touchPanelInit(void);
#endif
#define IS_LCD_RESET_NEEDED()           true
#define LCD_CONTRAST_MIN                10
#define LCD_CONTRAST_MAX                30
#if defined(RADIO_TX12)  || defined(RADIO_TPRO) || defined(RADIO_FAMILY_JUMPER_T12) || defined(RADIO_TPRO)
  #define LCD_CONTRAST_DEFAULT          25
#else
  #define LCD_CONTRAST_DEFAULT          15
#endif

#if defined(PCBX9D) || defined(PCBX9E) || (defined(PCBX9DP) && PCBREV < 2019)
#define IS_LCD_RESET_NEEDED()           (!WAS_RESET_BY_WATCHDOG_OR_SOFTWARE())
#else
#define IS_LCD_RESET_NEEDED()           true
#endif

void lcdInit();
void lcdInitFinish();
void lcdOff();

// TODO lcdRefreshWait() stub in simpgmspace and remove LCD_DUAL_BUFFER
#if defined(LCD_DMA) && !defined(LCD_DUAL_BUFFER) && !defined(SIMU)
void lcdRefreshWait();
#else
#define lcdRefreshWait()
#endif
void lcdSetRefVolt(unsigned char val);
void lcdSetContrast(bool useDefault = false);

// Top LCD driver
void toplcdInit();
void toplcdOff();
void toplcdRefreshStart();
void toplcdRefreshEnd();
void setTopFirstTimer(int32_t value);
void setTopSecondTimer(uint32_t value);
void setTopRssi(uint32_t rssi);
void setTopBatteryState(int state, uint8_t blinking);
void setTopBatteryValue(uint32_t volts);

#define USART_FLAG_ERRORS (USART_FLAG_ORE | USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE)

#if defined(__cplusplus)
#include "fifo.h"
//#include "dmafifo.h"

#if defined(CROSSFIRE)
#define TELEMETRY_FIFO_SIZE             128
#else
#define TELEMETRY_FIFO_SIZE             64
#endif

extern Fifo<uint8_t, TELEMETRY_FIFO_SIZE> telemetryFifo;
//typedef DMAFifo<32> AuxSerialRxFifo;
//extern AuxSerialRxFifo auxSerialRxFifo;
#endif

// Gyro driver
#define GYRO_VALUES_COUNT               6
#define GYRO_BUFFER_LENGTH              (GYRO_VALUES_COUNT * sizeof(int16_t))
int gyroInit();
int gyroRead(uint8_t buffer[GYRO_BUFFER_LENGTH]);
#define GYRO_MAX_DEFAULT                30
#define GYRO_MAX_RANGE                  60
#define GYRO_OFFSET_MIN                 -30
#define GYRO_OFFSET_MAX                 10

#define INTMODULE_FIFO_SIZE         512

#define BATTERY_DIVIDER 23711
#define VOLTAGE_DROP 45


// WiFi

#ifndef ESP_NOW_ETH_ALEN
#define ESP_NOW_ETH_ALEN 6
#endif

void initWiFi();
void startWiFi( char *ssid_zchar, char *passwd_zchar, char* ftppass_zchar);
void stopWiFi();
const char* getWiFiStatus();
bool isWiFiStarted(uint32_t expire=500);

void startWiFiESPNow();
void stopWiFiESPNow();
void init_espnow();
void disable_espnow();
void pause_espnow();
void resume_espnow();
void init_bind_espnow();
void stop_bind_espnow();
bool is_binding_espnow();

#endif // _BOARD_H_
