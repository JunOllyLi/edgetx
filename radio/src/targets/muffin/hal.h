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

#ifndef _HAL_H_
#define _HAL_H_

// TODO-feather copied from FS-i6X, to keep compiler happy
#define KEYS_GPIO_PIN_RIGHT 1024
#define KEYS_GPIO_PIN_LEFT 1024
#define KEYS_GPIO_PIN_UP 1024
#define KEYS_GPIO_PIN_DOWN 1024

// Rotary Encoder
#if defined(PCB_MUFFIN)
  //#define ROTARY_ENCODER_NAVIGATION
  #define ROTARY_ENCODER_GPIO              GPIOE
  #define ROTARY_ENCODER_GPIO_PIN_A        GPIO_Pin_9 // PE.9
  #define ROTARY_ENCODER_GPIO_PIN_B        GPIO_Pin_10 // PE.10
  #define ROTARY_ENCODER_POSITION()        ((ROTARY_ENCODER_GPIO->IDR >> 9) & 0x03)
  #define ROTARY_ENCODER_EXTI_LINE1        EXTI_Line9
  #define ROTARY_ENCODER_EXTI_LINE2        EXTI_Line10
  #define ROTARY_ENCODER_EXTI_IRQn1        EXTI9_5_IRQn
  #define ROTARY_ENCODER_EXTI_IRQHandler1  EXTI9_5_IRQHandler
  #define ROTARY_ENCODER_EXTI_IRQn2        EXTI15_10_IRQn
  #define ROTARY_ENCODER_EXTI_IRQHandler2  EXTI15_10_IRQHandler
  #define ROTARY_ENCODER_EXTI_PortSource   EXTI_PortSourceGPIOE
  #define ROTARY_ENCODER_EXTI_PinSource1   EXTI_PinSource9
  #define ROTARY_ENCODER_EXTI_PinSource2   EXTI_PinSource10
#endif

#if defined(ROTARY_ENCODER_NAVIGATION)
  #define ROTARY_ENCODER_RCC_APB1Periph   RCC_APB1Periph_TIM5
  #define ROTARY_ENCODER_TIMER            TIM5
  #define ROTARY_ENCODER_TIMER_IRQn       TIM5_IRQn
  #define ROTARY_ENCODER_TIMER_IRQHandler TIM5_IRQHandler
#else
  #define ROTARY_ENCODER_RCC_APB1Periph   0
#endif

// Trims
#if defined(PCB_MUFFIN)
  #define TRIMS_GPIO_REG_LHL            GPIOD->IDR
  #define TRIMS_GPIO_PIN_LHL            GPIO_Pin_15 // PD.15
  #define TRIMS_GPIO_REG_LHR            GPIOC->IDR
  #define TRIMS_GPIO_PIN_LHR            GPIO_Pin_1  // PC.01
  #define TRIMS_GPIO_REG_LVD            GPIOE->IDR
  #define TRIMS_GPIO_PIN_LVD            GPIO_Pin_6  // PE.06
  #define TRIMS_GPIO_REG_LVU            GPIOE->IDR
  #define TRIMS_GPIO_PIN_LVU            GPIO_Pin_5  // PE.05
  #define TRIMS_GPIO_REG_RVD            GPIOC->IDR
  #define TRIMS_GPIO_PIN_RVD            GPIO_Pin_3  // PC.03
  #define TRIMS_GPIO_REG_RHL            GPIOE->IDR
  #define TRIMS_GPIO_PIN_RHL            GPIO_Pin_3  // PE.03
  #define TRIMS_GPIO_REG_RVU            GPIOC->IDR
  #define TRIMS_GPIO_PIN_RVU            GPIO_Pin_2  // PC.02
  #define TRIMS_GPIO_REG_RHR            GPIOE->IDR
  #define TRIMS_GPIO_PIN_RHR            GPIO_Pin_4  // PE.04
#endif

// Switches
#if defined(PCB_MUFFIN)
  #define STORAGE_SWITCH_A
  #define HARDWARE_SWITCH_A
  #define SWITCHES_GPIO_REG_A           GPIOC->IDR
  #define SWITCHES_GPIO_PIN_A           GPIO_Pin_13  // PC.13
#endif

#if defined(PCB_MUFFIN)
  #define STORAGE_SWITCH_B
  #define HARDWARE_SWITCH_B
  #define SWITCHES_GPIO_REG_B_L         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_B_L         GPIO_Pin_15 // PE.15
  #define SWITCHES_GPIO_REG_B_H         GPIOA->IDR
  #define SWITCHES_GPIO_PIN_B_H         GPIO_Pin_5  // PA.05
#endif

#if defined(PCB_MUFFIN)
  #define STORAGE_SWITCH_C
  #define HARDWARE_SWITCH_C
  #define SWITCHES_GPIO_REG_C_L         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_C_L         GPIO_Pin_0  // PE.00
  #define SWITCHES_GPIO_REG_C_H         GPIOD->IDR
  #define SWITCHES_GPIO_PIN_C_H         GPIO_Pin_11  // PD.11
#endif

#if defined(PCB_MUFFIN)
  #define STORAGE_SWITCH_D
  #define HARDWARE_SWITCH_D
  #define SWITCHES_GPIO_REG_D           GPIOE->IDR
  #define SWITCHES_GPIO_PIN_D           GPIO_Pin_8  // PE.08
#endif

#if defined(PCB_MUFFIN)
  #define STORAGE_SWITCH_E
  #define HARDWARE_SWITCH_E
  #define SWITCHES_GPIO_REG_E           GPIOE->IDR
  #define SWITCHES_GPIO_PIN_E           GPIO_Pin_7  // PE.07
#endif

#if defined(PCB_MUFFIN)
  #define STORAGE_SWITCH_F
  #define HARDWARE_SWITCH_F
  #define SWITCHES_GPIO_REG_F           GPIOE->IDR
  #define SWITCHES_GPIO_PIN_F           GPIO_Pin_1 // PE.01
#endif

#if defined(PCB_MUFFIN)
  #define STORAGE_SWITCH_G
  #define HARDWARE_SWITCH_G
  #define SWITCHES_GPIO_REG_G           GPIOE->IDR
  #define SWITCHES_GPIO_PIN_G           GPIO_Pin_14 // PE.14
#endif

#if defined(PCB_MUFFIN)
  #define STORAGE_SWITCH_H
  #define HARDWARE_SWITCH_H
  #define SWITCHES_GPIO_REG_H           GPIOD->IDR
  #define SWITCHES_GPIO_PIN_H           GPIO_Pin_14 // PD.14
#endif

#if defined(PCB_MUFFIN)
  #define KEYS_RCC_AHB1Periph           (RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE)
  #define KEYS_GPIOA_PINS               (KEYS_GPIO_PIN_ENTER | SWITCHES_GPIO_PIN_B_H)
  #define KEYS_GPIOB_PINS               (KEYS_GPIO_PIN_SYS)
  #define KEYS_GPIOC_PINS               (KEYS_GPIO_PIN_EXIT | TRIMS_GPIO_PIN_LHR | TRIMS_GPIO_PIN_RVD | TRIMS_GPIO_PIN_RVU | SWITCHES_GPIO_PIN_A)
  #define KEYS_GPIOD_PINS               (KEYS_GPIO_PIN_PAGEUP | KEYS_GPIO_PIN_PAGEDN | KEYS_GPIO_PIN_TELE | TRIMS_GPIO_PIN_LHL |SWITCHES_GPIO_PIN_C_H | SWITCHES_GPIO_PIN_H)
  #define KEYS_GPIOE_PINS               (KEYS_GPIO_PIN_MDL | TRIMS_GPIO_PIN_LVD | TRIMS_GPIO_PIN_LVU | TRIMS_GPIO_PIN_RHL | TRIMS_GPIO_PIN_RHR | SWITCHES_GPIO_PIN_B_L | SWITCHES_GPIO_PIN_C_L | SWITCHES_GPIO_PIN_D | SWITCHES_GPIO_PIN_E | SWITCHES_GPIO_PIN_F | SWITCHES_GPIO_PIN_G | ROTARY_ENCODER_GPIO_PIN_A | ROTARY_ENCODER_GPIO_PIN_B)
#endif

// ADC
#define ADC_MAIN                        ADC1
#define ADC_DMA                         DMA2
#define ADC_DMA_SxCR_CHSEL              0
#define ADC_DMA_Stream                  DMA2_Stream4
#define ADC_SET_DMA_FLAGS()             ADC_DMA->HIFCR = (DMA_HIFCR_CTCIF4 | DMA_HIFCR_CHTIF4 | DMA_HIFCR_CTEIF4 | DMA_HIFCR_CDMEIF4 | DMA_HIFCR_CFEIF4)
#define ADC_TRANSFER_COMPLETE()         (ADC_DMA->HISR & DMA_HISR_TCIF4)
#define ADC_SAMPTIME                    2   // sample time = 28 cycles
#if defined(RADIO_TLITE)
  #define ADC_MAIN_SMPR1               (ADC_SAMPTIME << 0) + (ADC_SAMPTIME << 3) + (ADC_SAMPTIME << 6) + (ADC_SAMPTIME << 9) + (ADC_SAMPTIME << 12) + (ADC_SAMPTIME << 15) + (ADC_SAMPTIME << 18) + (ADC_SAMPTIME << 21) + ((ADC_SAMPTIME + 1) << 24); // TLite needs +1 for proper RTC Bat measurement.
  #define ADC_MAIN_SMPR2               (ADC_SAMPTIME << 0) + (ADC_SAMPTIME << 3) + (ADC_SAMPTIME << 6) + (ADC_SAMPTIME << 9) + (ADC_SAMPTIME << 12) + (ADC_SAMPTIME << 15) + (ADC_SAMPTIME << 18) + (ADC_SAMPTIME << 21) + (ADC_SAMPTIME << 24) + (ADC_SAMPTIME << 27);
#else
  #define ADC_MAIN_SMPR1               (ADC_SAMPTIME << 0) + (ADC_SAMPTIME << 3) + (ADC_SAMPTIME << 6) + (ADC_SAMPTIME << 9) + (ADC_SAMPTIME << 12) + (ADC_SAMPTIME << 15) + (ADC_SAMPTIME << 18) + (ADC_SAMPTIME << 21) + (ADC_SAMPTIME << 24);
  #define ADC_MAIN_SMPR2               (ADC_SAMPTIME << 0) + (ADC_SAMPTIME << 3) + (ADC_SAMPTIME << 6) + (ADC_SAMPTIME << 9) + (ADC_SAMPTIME << 12) + (ADC_SAMPTIME << 15) + (ADC_SAMPTIME << 18) + (ADC_SAMPTIME << 21) + (ADC_SAMPTIME << 24) + (ADC_SAMPTIME << 27);
#endif
#if defined(PCB_MUFFIN)
  #define HARDWARE_POT1
  #define HARDWARE_POT2
  #define ADC_RCC_AHB1Periph            (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_DMA2)
  #define ADC_RCC_APB1Periph            0
  #define ADC_RCC_APB2Periph            RCC_APB2Periph_ADC1
  #define ADC_GPIO_PIN_STICK_RV         GPIO_Pin_0  // PA.00
  #define ADC_GPIO_PIN_STICK_RH         GPIO_Pin_1  // PA.01
  #define ADC_GPIO_PIN_STICK_LV         GPIO_Pin_2  // PA.02
  #define ADC_GPIO_PIN_STICK_LH         GPIO_Pin_3  // PA.03
  #define ADC_CHANNEL_STICK_RV          ADC_Channel_0  // ADC1_IN0
  #define ADC_CHANNEL_STICK_RH          ADC_Channel_1  // ADC1_IN1
  #define ADC_CHANNEL_STICK_LV          ADC_Channel_2  // ADC1_IN2
  #define ADC_CHANNEL_STICK_LH          ADC_Channel_3  // ADC1_IN3
  #define ADC_GPIO_PIN_POT1             GPIO_Pin_0  // PB.00
  #define ADC_GPIO_PIN_POT2             GPIO_Pin_6  // PA.06
  #define ADC_GPIO_PIN_BATT             GPIO_Pin_0  // PC.00
  #define ADC_GPIOA_PINS                (ADC_GPIO_PIN_STICK_RV | ADC_GPIO_PIN_STICK_RH | ADC_GPIO_PIN_STICK_LH | ADC_GPIO_PIN_STICK_LV | ADC_GPIO_PIN_POT2)
  #define ADC_GPIOB_PINS                ADC_GPIO_PIN_POT1
  #define ADC_GPIOC_PINS                ADC_GPIO_PIN_BATT
  #define ADC_CHANNEL_POT1              ADC_Channel_8
  #define ADC_CHANNEL_POT2              ADC_Channel_6
  #define ADC_CHANNEL_BATT              ADC_Channel_10
  #define ADC_VREF_PREC2                330
#endif

// PWR and LED driver
#define PWR_RCC_AHB1Periph              (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE)

#if defined(PCB_MUFFIN)
  #define PWR_SWITCH_GPIO               GPIOD
  #define PWR_SWITCH_GPIO_PIN           GPIO_Pin_1  // PD.01
  #define PWR_ON_GPIO                   GPIOD
  #define PWR_ON_GPIO_PIN               GPIO_Pin_0  // PD.00
#endif

#if defined(PCB_MUFFIN)
  #define STATUS_LEDS
  #define GPIO_LED_GPIO_ON              GPIO_SetBits
  #define GPIO_LED_GPIO_OFF             GPIO_ResetBits
  #define LED_GREEN_GPIO                GPIOA
  #define LED_GREEN_GPIO_PIN            GPIO_Pin_7  // PA.07
  #define LED_RED_GPIO                  GPIOE
  #define LED_RED_GPIO_PIN              GPIO_Pin_13 // PE.13
  #define LED_BLUE_GPIO                 GPIOE
  #define LED_BLUE_GPIO_PIN             GPIO_Pin_2  // PE.02
#endif

#if defined(FUNCTION_SWITCHES)
  #define FS_RCC_AHB1Periph             RCC_AHB1Periph_GPIOF
  #define GPIO_FSLED_GPIO_ON            GPIO_SetBits
  #define GPIO_FSLED_GPIO_OFF           GPIO_ResetBits
  #define FSLED_GPIO                    GPIOF
  #define FSLED_GPIO_PIN_1              GPIO_Pin_5
  #define FSLED_GPIO_PIN_2              GPIO_Pin_4
  #define FSLED_GPIO_PIN_3              GPIO_Pin_3
  #define FSLED_GPIO_PIN_4              GPIO_Pin_2
  #define FSLED_GPIO_PIN_5              GPIO_Pin_1
  #define FSLED_GPIO_PIN_6              GPIO_Pin_0
#endif

// Internal Module
#if defined(PCB_MUFFIN)
  #define INTMODULE_RCC_APB1Periph      0
  #define INTMODULE_RCC_APB2Periph      RCC_APB2Periph_USART1
  #define INTMODULE_RCC_AHB1Periph      (RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_DMA2)
  #define INTMODULE_PWR_GPIO            GPIOC
  #define INTMODULE_PWR_GPIO_PIN        GPIO_Pin_4  // PC.04
  #define INTMODULE_GPIO                GPIOB
  #define INTMODULE_TX_GPIO_PIN         GPIO_Pin_6  // PB.06
  #define INTMODULE_RX_GPIO_PIN         GPIO_Pin_7  // PB.07
  #define INTMODULE_GPIO_PinSource_TX   GPIO_PinSource6
  #define INTMODULE_GPIO_PinSource_RX   GPIO_PinSource7
  #define INTMODULE_USART               USART1
  #define INTMODULE_GPIO_AF             GPIO_AF_USART1
  #define INTMODULE_USART_IRQHandler    USART1_IRQHandler
  #define INTMODULE_USART_IRQn          USART1_IRQn
  //#define INTMODULE_DMA_STREAM          DMA2_Stream7
  #define INTMODULE_DMA_STREAM_IRQ         DMA2_Stream7_IRQn
  #define INTMODULE_DMA_STREAM_IRQHandler  DMA2_Stream7_IRQHandler
  #define INTMODULE_DMA_FLAG_TC         DMA_IT_TCIF7
  #define INTMODULE_DMA_CHANNEL         DMA_Channel_4
  #define INTMODULE_BOOTCMD_GPIO        GPIOB
  #define INTMODULE_BOOTCMD_GPIO_PIN    GPIO_Pin_1  // PB.01
  #define INIT_INTMODULE_BOOTCMD_PIN()  GPIO_ResetBits(INTMODULE_BOOTCMD_GPIO, INTMODULE_BOOTCMD_GPIO_PIN);
#endif

// External Module
#if defined(PCB_MUFFIN)
  #define EXTMODULE_RCC_APB2Periph      (RCC_APB2Periph_TIM8 | RCC_APB2Periph_USART6)
    #define EXTMODULE_RCC_AHB1Periph    (RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_DMA2)
    #define EXTMODULE_PWR_GPIO          0
    #define EXTMODULE_PWR_GPIO_PIN      0// GPIO_Pin_8  // PD.08
  #define HARDWARE_EXTERNAL_MODULE_SIZE_SML
  #define EXTERNAL_MODULE_PWR_ON()      // TODO-feather
  #define EXTERNAL_MODULE_PWR_OFF()     // TODO-feather
  #define IS_EXTERNAL_MODULE_ON()       false // TODO-feather
  #define EXTMODULE_TX_GPIO             GPIOC
  #define EXTMODULE_USART_GPIO          EXTMODULE_TX_GPIO
  #define EXTMODULE_TX_GPIO_PIN         GPIO_Pin_6  // PC.06
  #define EXTMODULE_TX_GPIO_PinSource   GPIO_PinSource6
  #define EXTMODULE_RX_GPIO_PIN         GPIO_Pin_7  // PC.07
  #define EXTMODULE_RX_GPIO_PinSource   GPIO_PinSource7
  #define EXTMODULE_TIMER               TIM8
  #define EXTMODULE_TIMER_FREQ          (PERI2_FREQUENCY * TIMER_MULT_APB2)
  #define EXTMODULE_TIMER_CC_IRQn       TIM8_CC_IRQn
  #define EXTMODULE_TIMER_CC_IRQHandler TIM8_CC_IRQHandler
  #define EXTMODULE_TIMER_TX_GPIO_AF    GPIO_AF_TIM8 // TIM8_CH1
  #define EXTMODULE_TIMER_DMA_CHANNEL           DMA_Channel_7
  #define EXTMODULE_TIMER_DMA_STREAM            DMA2_Stream1
  #define EXTMODULE_TIMER_DMA_STREAM_IRQn       DMA2_Stream1_IRQn
  #define EXTMODULE_TIMER_DMA_STREAM_IRQHandler DMA2_Stream1_IRQHandler
  #define EXTMODULE_TIMER_DMA_FLAG_TC           DMA_IT_TCIF1
  #define EXTMODULE_TIMER_OUTPUT_ENABLE         TIM_CCER_CC1E
  #define EXTMODULE_TIMER_OUTPUT_POLARITY       TIM_CCER_CC1P
  #define EXTMODULE_USART_GPIO_AF               GPIO_AF_USART6
  #define EXTMODULE_USART                       USART6
  #define EXTMODULE_USART_IRQn                  USART6_IRQn
  #define EXTMODULE_USART_IRQHandler            USART6_IRQHandler
  #define EXTMODULE_USART_TX_DMA_CHANNEL        DMA_Channel_5
  #define EXTMODULE_USART_TX_DMA_STREAM         DMA2_Stream6
  #define EXTMODULE_USART_RX_DMA_CHANNEL        DMA_Channel_5
  #define EXTMODULE_USART_RX_DMA_STREAM         DMA2_Stream1
#endif

// Trainer Port
  #define TRAINER_RCC_AHB1Periph        (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_DMA1)
  #define TRAINER_RCC_APB1Periph        RCC_APB1Periph_TIM3
  #define TRAINER_GPIO                  GPIOC
  #define TRAINER_IN_GPIO_PIN           GPIO_Pin_8  // PC.08
  #define TRAINER_IN_GPIO_PinSource     GPIO_PinSource8
  #define TRAINER_OUT_GPIO_PIN          GPIO_Pin_9  // PC.09
  #define TRAINER_OUT_GPIO_PinSource    GPIO_PinSource9
  #define TRAINER_DETECT_GPIO           GPIOA
  #define TRAINER_DETECT_GPIO_PIN       GPIO_Pin_8  // PA.08
  #define TRAINER_DETECT_GPIO_PIN_VALUE Bit_RESET
  #define TRAINER_TIMER                 TIM3
  #define TRAINER_TIMER_IRQn            TIM3_IRQn
  #define TRAINER_GPIO_AF               GPIO_AF_TIM3
  #define TRAINER_DMA                   DMA1
  #define TRAINER_DMA_CHANNEL           DMA_Channel_5
  #define TRAINER_DMA_STREAM            DMA1_Stream2
  #define TRAINER_DMA_IRQn              DMA1_Stream2_IRQn
  #define TRAINER_DMA_IRQHandler        DMA1_Stream2_IRQHandler
  #define TRAINER_DMA_FLAG_TC           DMA_IT_TCIF2
  #define TRAINER_TIMER_IRQn            TIM3_IRQn
  #define TRAINER_TIMER_IRQHandler      TIM3_IRQHandler
  #define TRAINER_TIMER_FREQ            (PERI1_FREQUENCY * TIMER_MULT_APB1)
  #define TRAINER_OUT_CCMR2             TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4PE;
  #define TRAINER_IN_CCMR2              TIM_CCMR2_IC3F_0 | TIM_CCMR2_IC3F_1 | TIM_CCMR2_CC3S_0;
  #define TRAINER_OUT_COUNTER_REGISTER  TRAINER_TIMER->CCR4
  #define TRAINER_IN_COUNTER_REGISTER   TRAINER_TIMER->CCR3
  #define TRAINER_SETUP_REGISTER        TRAINER_TIMER->CCR1
  #define TRAINER_OUT_INTERRUPT_FLAG    TIM_SR_CC1IF
  #define TRAINER_OUT_INTERRUPT_ENABLE  TIM_DIER_CC1IE
  #define TRAINER_IN_INTERRUPT_ENABLE   TIM_DIER_CC3IE
  #define TRAINER_IN_INTERRUPT_FLAG     TIM_SR_CC3IF
  #define TRAINER_OUT_CCER              TIM_CCER_CC4E
  #define TRAINER_IN_CCER               TIM_CCER_CC3E
  #define TRAINER_CCER_POLARYTY         TIM_CCER_CC4P

// Serial Port
  #define TRAINER_BATTERY_COMPARTMENT
  #define AUX_SERIAL_RCC_AHB1Periph         (RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_DMA1)
  #define AUX_SERIAL_RCC_APB1Periph         RCC_APB1Periph_USART3
  #define AUX_SERIAL_RCC_APB2Periph         0
  #define AUX_SERIAL_GPIO                   GPIOB
  #define AUX_SERIAL_GPIO_PIN_TX            GPIO_Pin_10 // PB.10
  #define AUX_SERIAL_GPIO_PIN_RX            GPIO_Pin_11 // PB.11
  #define AUX_SERIAL_GPIO_PinSource_TX      GPIO_PinSource10
  #define AUX_SERIAL_GPIO_PinSource_RX      GPIO_PinSource11
  #define AUX_SERIAL_GPIO_AF                GPIO_AF_USART3
  #define AUX_SERIAL_USART                  USART3
  #define AUX_SERIAL_USART_IRQHandler       USART3_IRQHandler
  #define AUX_SERIAL_USART_IRQn             USART3_IRQn
  #define AUX_SERIAL_DMA_Stream_RX          DMA1_Stream1
  #define AUX_SERIAL_DMA_Channel_RX         DMA_Channel_4

// No aux2 on taranis
#define AUX2_SERIAL_RCC_AHB1Periph           0
#define AUX2_SERIAL_RCC_APB1Periph           0
#define AUX2_SERIAL_RCC_APB2Periph           0

// Telemetry
#define TELEMETRY_RCC_AHB1Periph        (RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_DMA1)
#define TELEMETRY_RCC_APB1Periph        RCC_APB1Periph_USART2
#define TELEMETRY_RCC_APB2Periph        RCC_APB2Periph_TIM11
#define TELEMETRY_DIR_GPIO              GPIOD
#define TELEMETRY_DIR_GPIO_PIN          GPIO_Pin_4  // PD.04
#if defined(PCBXLITE) || defined(PCBX9LITE) || defined(RADIO_X9DP2019) || defined(RADIO_X7ACCESS)
  #define TELEMETRY_DIR_OUTPUT()          TELEMETRY_DIR_GPIO->BSRRH = TELEMETRY_DIR_GPIO_PIN
  #define TELEMETRY_DIR_INPUT()           TELEMETRY_DIR_GPIO->BSRRL = TELEMETRY_DIR_GPIO_PIN
#else
  #define TELEMETRY_DIR_OUTPUT()          TELEMETRY_DIR_GPIO->BSRRL = TELEMETRY_DIR_GPIO_PIN
  #define TELEMETRY_DIR_INPUT()           TELEMETRY_DIR_GPIO->BSRRH = TELEMETRY_DIR_GPIO_PIN
#endif
#define TELEMETRY_GPIO                  GPIOD
#define TELEMETRY_TX_GPIO_PIN           GPIO_Pin_5  // PD.05
#define TELEMETRY_RX_GPIO_PIN           GPIO_Pin_6  // PD.06
#define TELEMETRY_GPIO_PinSource_TX     GPIO_PinSource5
#define TELEMETRY_GPIO_PinSource_RX     GPIO_PinSource6
#define TELEMETRY_GPIO_AF               GPIO_AF_USART2
#define TELEMETRY_USART                 USART2
#define TELEMETRY_DMA_Stream_TX         DMA1_Stream6
#define TELEMETRY_DMA_Channel_TX        DMA_Channel_4
#define TELEMETRY_DMA_TX_Stream_IRQ     DMA1_Stream6_IRQn
#define TELEMETRY_DMA_TX_IRQHandler     DMA1_Stream6_IRQHandler
#define TELEMETRY_DMA_TX_FLAG_TC        DMA_IT_TCIF6
#define TELEMETRY_USART_IRQHandler      USART2_IRQHandler
#define TELEMETRY_USART_IRQn            USART2_IRQn
#define TELEMETRY_EXTI_PortSource       EXTI_PortSourceGPIOD
#define TELEMETRY_EXTI_PinSource        EXTI_PinSource6
#define TELEMETRY_EXTI_LINE             EXTI_Line6
#define TELEMETRY_EXTI_IRQn             EXTI9_5_IRQn
#define TELEMETRY_EXTI_TRIGGER          EXTI_Trigger_Rising

#if defined(RADIO_X7) || defined(RADIO_X7ACCESS) || defined(RADIO_TX12) || defined(PCB_MUFFIN) || defined(RADIO_T8) || defined(RADIO_TPRO)
  #define TELEMETRY_EXTI_REUSE_INTERRUPT_ROTARY_ENCODER
#elif defined(PCBXLITE) || defined(PCBX9LITE) || defined(RADIO_X9DP2019)
  #define TELEMETRY_EXTI_IRQHandler       EXTI9_5_IRQHandler
#else
  #define TELEMETRY_EXTI_REUSE_INTERRUPT_INTMODULE_HEARTBEAT
#endif

#define TELEMETRY_TIMER                 TIM11
#define TELEMETRY_TIMER_IRQn            TIM1_TRG_COM_TIM11_IRQn
#define TELEMETRY_TIMER_IRQHandler      TIM1_TRG_COM_TIM11_IRQHandler

// PCBREV
  #define PCBREV_RCC_AHB1Periph         0


// USB Charger
#if defined(USB_CHARGER)
  #define USB_CHARGER_RCC_AHB1Periph      RCC_AHB1Periph_GPIOB
  #define USB_CHARGER_GPIO                GPIOF
  #define USB_CHARGER_GPIO_PIN            GPIO_Pin_10  // PF.10
#else
  #define USB_CHARGER_RCC_AHB1Periph      0
#endif

// S.Port update connector
#if defined(PCB_MUFFIN)
  #define SPORT_MAX_BAUDRATE            400000
  #define SPORT_UPDATE_RCC_AHB1Periph   0
#endif

// Heartbeat for iXJT / ISRM synchro
#define INTMODULE_HEARTBEAT_TRIGGER               EXTI_Trigger_Falling
  #define INTMODULE_HEARTBEAT_RCC_AHB1Periph      0

#if defined(INTMODULE_HEARTBEAT_GPIO) && defined(HARDWARE_EXTERNAL_MODULE)
  // Trainer CPPM input on heartbeat pin
  #define TRAINER_MODULE_CPPM
  #define TRAINER_MODULE_RCC_AHB1Periph           RCC_AHB1Periph_GPIOC
  #define TRAINER_MODULE_RCC_APB2Periph           RCC_APB2Periph_USART6
  #define TRAINER_MODULE_RCC_APB1Periph           RCC_APB1Periph_TIM3
  #define TRAINER_MODULE_CPPM_TIMER               TRAINER_TIMER
  #define TRAINER_MODULE_CPPM_GPIO                INTMODULE_HEARTBEAT_GPIO
  #define TRAINER_MODULE_CPPM_GPIO_PIN            INTMODULE_HEARTBEAT_GPIO_PIN
  #define TRAINER_MODULE_CPPM_GPIO_PinSource      INTMODULE_HEARTBEAT_EXTI_PinSource
  #define TRAINER_MODULE_CPPM_INTERRUPT_ENABLE    TIM_DIER_CC2IE
  #define TRAINER_MODULE_CPPM_INTERRUPT_FLAG      TIM_SR_CC2IF
  #define TRAINER_MODULE_CPPM_CCMR1               (TIM_CCMR1_IC2F_0 | TIM_CCMR1_IC2F_1 | TIM_CCMR1_CC2S_0)
  #define TRAINER_MODULE_CPPM_CCER                TIM_CCER_CC2E
  #define TRAINER_MODULE_CPPM_COUNTER_REGISTER    TRAINER_TIMER->CCR2
  #define TRAINER_MODULE_CPPM_TIMER_IRQn          TRAINER_TIMER_IRQn
  #define TRAINER_MODULE_CPPM_GPIO_AF             GPIO_AF_TIM3
  // Trainer SBUS input on heartbeat pin
  #define TRAINER_MODULE_SBUS
  #define TRAINER_MODULE_SBUS_GPIO_AF             GPIO_AF_USART6
  #define TRAINER_MODULE_SBUS_USART               USART6
  #define TRAINER_MODULE_SBUS_GPIO                INTMODULE_HEARTBEAT_GPIO
  #define TRAINER_MODULE_SBUS_GPIO_PIN            INTMODULE_HEARTBEAT_GPIO_PIN
  #define TRAINER_MODULE_SBUS_GPIO_PinSource      INTMODULE_HEARTBEAT_EXTI_PinSource
  #define TRAINER_MODULE_SBUS_DMA_STREAM          DMA2_Stream1
  #define TRAINER_MODULE_SBUS_DMA_CHANNEL         DMA_Channel_5
#else
  #define TRAINER_MODULE_RCC_AHB1Periph           0
  #define TRAINER_MODULE_RCC_APB2Periph           0
  #define TRAINER_MODULE_RCC_APB1Periph           0
#endif

// USB
#define USB_RCC_AHB1Periph_GPIO         RCC_AHB1Periph_GPIOA
#define USB_GPIO                        GPIOA
#define USB_GPIO_PIN_VBUS               GPIO_Pin_9  // PA.09
#define USB_GPIO_PIN_DM                 GPIO_Pin_11 // PA.11
#define USB_GPIO_PIN_DP                 GPIO_Pin_12 // PA.12
#define USB_GPIO_PinSource_DM           GPIO_PinSource11
#define USB_GPIO_PinSource_DP           GPIO_PinSource12
#define USB_GPIO_AF                     GPIO_AF_OTG1_FS

// BackLight
  #define BACKLIGHT_RCC_AHB1Periph      RCC_AHB1Periph_GPIOD
  #define BACKLIGHT_RCC_APB1Periph      RCC_APB1Periph_TIM4
  #define BACKLIGHT_RCC_APB2Periph      0
  #define BACKLIGHT_TIMER_FREQ          (PERI1_FREQUENCY * TIMER_MULT_APB1)
  #define BACKLIGHT_TIMER               TIM4
  #define BACKLIGHT_GPIO                GPIOD
  #define BACKLIGHT_GPIO_PIN            GPIO_Pin_13 // PD.13
  #define BACKLIGHT_GPIO_PinSource      GPIO_PinSource13
  #define BACKLIGHT_GPIO_AF             GPIO_AF_TIM4
  #define BACKLIGHT_CCMR1               TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2 // Channel2, PWM
  #define BACKLIGHT_CCER                TIM_CCER_CC2E
  #define BACKLIGHT_COUNTER_REGISTER    BACKLIGHT_TIMER->CCR2
#define KEYS_BACKLIGHT_RCC_AHB1Periph        0

// LCD driver
  #define LCD_VERTICAL_INVERT
#if defined(PCB_MUFFIN) || defined(PCBX7)
  #define LCD_RCC_AHB1Periph            (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_DMA1)
  #define LCD_RCC_APB1Periph            RCC_APB1Periph_SPI3
  #define LCD_SPI_GPIO                  GPIOC
  #define LCD_MOSI_GPIO_PIN             GPIO_Pin_12 // PC.12
  #define LCD_MOSI_GPIO_PinSource       GPIO_PinSource12
  #define LCD_CLK_GPIO_PIN              GPIO_Pin_10 // PC.10
  #define LCD_CLK_GPIO_PinSource        GPIO_PinSource10
  #define LCD_A0_GPIO_PIN               GPIO_Pin_11 // PC.11
  #define LCD_NCS_GPIO                  GPIOF
  #define LCD_NCS_GPIO_PIN              GPIO_Pin_7 // PF.07
  #define LCD_RST_GPIO                  GPIOD
  #define LCD_RST_GPIO_PIN              GPIO_Pin_12 // PD.12
  #define LCD_DMA                       DMA1
  #define LCD_DMA_Stream                DMA1_Stream7
  #define LCD_DMA_Stream_IRQn           DMA1_Stream7_IRQn
  #define LCD_DMA_Stream_IRQHandler     DMA1_Stream7_IRQHandler
  #define LCD_DMA_FLAGS                 (DMA_HIFCR_CTCIF7 | DMA_HIFCR_CHTIF7 | DMA_HIFCR_CTEIF7 | DMA_HIFCR_CDMEIF7 | DMA_HIFCR_CFEIF7)
  #define LCD_DMA_FLAG_INT              DMA_HIFCR_CTCIF7
  #define LCD_SPI                       SPI3
  #define LCD_GPIO_AF                   GPIO_AF_SPI3
#endif
#define LCD_RCC_APB2Periph              0

// I2C Bus: EEPROM and CAT5137 digital pot for volume control
#define I2C_RCC_APB1Periph              RCC_APB1Periph_I2C1
#define I2C                             I2C1
#define I2C_GPIO_AF                     GPIO_AF_I2C1
#if defined(PCB_MUFFIN) || defined(PCBX9LITE)
  #define I2C_RCC_AHB1Periph            RCC_AHB1Periph_GPIOB
  #define I2C_SPI_GPIO                  GPIOB
  #define I2C_SDA_GPIO_PIN              GPIO_Pin_9  // PB.09
  #define I2C_SCL_GPIO_PIN              GPIO_Pin_8  // PB.08
  #define I2C_WP_GPIO                   GPIOD
  #define I2C_WP_GPIO_PIN               GPIO_Pin_10  // PD.10
  #define I2C_SDA_GPIO_PinSource        GPIO_PinSource9
  #define I2C_SCL_GPIO_PinSource        GPIO_PinSource8
#endif
#ifdef ESP_PLATFORM
#else
#define I2C_SPEED                       400000
#endif
#define I2C_ADDRESS_EEPROM              0xA2
#define I2C_FLASH_PAGESIZE              64

// Second I2C Bus: IMU
  #define GYRO_RCC_AHB1Periph           0
  #define GYRO_RCC_APB1Periph           0

// SD - SPI2
#define SD_RCC_AHB1Periph               (RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_DMA1)
#define SD_RCC_APB1Periph               RCC_APB1Periph_SPI2
#define SD_GPIO_PRESENT_GPIO            GPIOD
  #define SD_GPIO_PRESENT_GPIO_PIN        GPIO_Pin_9  // PD.09
#define SD_GPIO                         GPIOB
#define SD_GPIO_PIN_CS                  GPIO_Pin_12 // PB.12
#define SD_GPIO_PIN_SCK                 GPIO_Pin_13 // PB.13
#define SD_GPIO_PIN_MISO                GPIO_Pin_14 // PB.14
#define SD_GPIO_PIN_MOSI                GPIO_Pin_15 // PB.15
#define SD_GPIO_AF                      GPIO_AF_SPI2
#define SD_GPIO_PinSource_CS            GPIO_PinSource12
#define SD_GPIO_PinSource_SCK           GPIO_PinSource13
#define SD_GPIO_PinSource_MISO          GPIO_PinSource14
#define SD_GPIO_PinSource_MOSI          GPIO_PinSource15
#define SD_SPI                          SPI2
#define SD_SPI_BaudRatePrescaler        SPI_BaudRatePrescaler_4 // 10.5<20MHZ, make sure < 20MHZ

#if !defined(BOOT)
  #define SD_USE_DMA                    // Enable the DMA for SD
  #define SD_DMA_Stream_SPI_RX          DMA1_Stream3
  #define SD_DMA_Stream_SPI_TX          DMA1_Stream4
  #define SD_DMA_FLAG_SPI_TC_RX         DMA_FLAG_TCIF3
  #define SD_DMA_FLAG_SPI_TC_TX         DMA_FLAG_TCIF4
  #define SD_DMA_Channel_SPI            DMA_Channel_0
#endif

// Audio
#define AUDIO_RCC_APB1Periph            (RCC_APB1Periph_TIM6 | RCC_APB1Periph_DAC)
#define AUDIO_OUTPUT_GPIO               GPIOA
#define AUDIO_OUTPUT_GPIO_PIN           GPIO_Pin_4  // PA.04
#define AUDIO_DMA_Stream                DMA1_Stream5
#define AUDIO_DMA_Stream_IRQn           DMA1_Stream5_IRQn
#define AUDIO_TIM_IRQn                  TIM6_DAC_IRQn
#define AUDIO_TIM_IRQHandler            TIM6_DAC_IRQHandler
#define AUDIO_DMA_Stream_IRQHandler     DMA1_Stream5_IRQHandler
#define AUDIO_TIMER                     TIM6
#define AUDIO_DMA                       DMA1

  #define AUDIO_RCC_AHB1Periph          (RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_DMA1)
  #define AUDIO_MUTE_GPIO               GPIOE
  #define AUDIO_MUTE_GPIO_PIN           GPIO_Pin_12  // PE.12
  #define AUDIO_MUTE_DELAY              500  // ms
  #define AUDIO_UNMUTE_DELAY            150  // ms

// Haptic
  #define HAPTIC_PWM
  #define HAPTIC_RCC_AHB1Periph         RCC_AHB1Periph_GPIOB
  #define HAPTIC_RCC_APB1Periph         RCC_APB1Periph_TIM2
  #define HAPTIC_RCC_APB2Periph         0
  #define HAPTIC_GPIO_PinSource         GPIO_PinSource3
  #define HAPTIC_GPIO                   GPIOF
  #define HAPTIC_GPIO_PIN               GPIO_Pin_8  // PF.08
  #define HAPTIC_GPIO_AF                GPIO_AF_TIM2
  #define HAPTIC_TIMER                  TIM2
  #define HAPTIC_TIMER_FREQ             (PERI1_FREQUENCY * TIMER_MULT_APB1)
  #define HAPTIC_COUNTER_REGISTER       HAPTIC_TIMER->CCR2
  #define HAPTIC_CCMR1                  TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2
  #define HAPTIC_CCER                   TIM_CCER_CC2E
  #define BACKLIGHT_BDTR                TIM_BDTR_MOE

// Bluetooth
  #define STORAGE_BLUETOOTH
  #define BT_RCC_AHB1Periph             0
  #define BT_RCC_APB1Periph             0
  #define BT_RCC_APB2Periph             0

// Xms Interrupt
#define INTERRUPT_xMS_RCC_APB1Periph    RCC_APB1Periph_TIM14
#define INTERRUPT_xMS_TIMER             TIM14
#define INTERRUPT_xMS_IRQn              TIM8_TRG_COM_TIM14_IRQn
#define INTERRUPT_xMS_IRQHandler        TIM8_TRG_COM_TIM14_IRQHandler

// 2MHz Timer
#define TIMER_2MHz_RCC_APB1Periph       RCC_APB1Periph_TIM7
#define TIMER_2MHz_TIMER                TIM7

// Mixer scheduler timer
#define MIXER_SCHEDULER_TIMER_RCC_APB1Periph RCC_APB1Periph_TIM13
#define MIXER_SCHEDULER_TIMER                TIM13
#define MIXER_SCHEDULER_TIMER_FREQ           (PERI1_FREQUENCY * TIMER_MULT_APB1)
#define MIXER_SCHEDULER_TIMER_IRQn           TIM8_UP_TIM13_IRQn
#define MIXER_SCHEDULER_TIMER_IRQHandler     TIM8_UP_TIM13_IRQHandler

#endif // _HAL_H_
