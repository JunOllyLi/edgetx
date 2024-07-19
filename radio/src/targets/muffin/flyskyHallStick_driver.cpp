/*
 * Copyright (C) EdgeTX
 *
 * Based on code named
 *   opentx - https://github.com/opentx/opentx
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

#include "opentx.h"
#include "flyskyHallStick_driver.h"
#include "esp32_rmt_pulse.h"
#include "hal/adc_driver.h"
#include "crc.h"

unsigned char HallCmd[264];

static void flysky_hall_stick_decode_cb(rmt_ctx_t *ctx, rmt_rx_done_event_data_t *rxdata);

STRUCT_HALL HallProtocol = { 0 };
STRUCT_HALL HallProtocolTx = { 0 };
signed short hall_raw_values[FLYSKY_HALL_CHANNEL_COUNT];
unsigned short hall_adc_values[FLYSKY_HALL_CHANNEL_COUNT];

void HallSendBuffer(uint8_t * buffer, uint32_t count)
{
}

uint8_t HallGetByte(uint8_t * byte)
{
  return 0;
}

void reset_hall_stick( void )
{
  unsigned short crc16_res = 0xffff;

  HallCmd[0] = FLYSKY_HALL_PROTOLO_HEAD;
  HallCmd[1] = 0xD1;
  HallCmd[2] = 0x01;
  HallCmd[3] = 0x01;

  crc16_res = crc16(CRC_1021, HallCmd, 4, 0xffff);

  HallCmd[4] = crc16_res & 0xff;
  HallCmd[5] = crc16_res >>8 & 0xff;

  HallSendBuffer( HallCmd, 6);
}

static StaticTask_t rx_task_buf;
EXT_RAM_BSS_ATTR static rmt_ctx_t ctxbuf;
void flysky_hall_stick_init()
{
  /* Why use RMT instead of UART?
   * 1. for fun
   * 2. don't need to worry about finding a pin for TX which is not used.
   */
  ctxbuf.task_struct = &rx_task_buf;
  esp32_rmt_rx_init(&ctxbuf, FLYSKY_UART_RX_PIN, 128,
    25,
    flysky_hall_stick_decode_cb,
    70,  // 14bytes of 8n1, total maximum of 70 pulses (each byte max 5 low/high pair)
    500000,
    900 // FlySky hall stick communicates at a little bit less that 1Mbps. So minimum pulse is around 1000ns
    );
  reset_hall_stick();
}

void Parse_Character(STRUCT_HALL *hallBuffer, unsigned char ch)
{
  switch (hallBuffer->status) {
    case GET_START: {
      if (FLYSKY_HALL_PROTOLO_HEAD == ch) {
        hallBuffer->head = FLYSKY_HALL_PROTOLO_HEAD;
        hallBuffer->status = GET_ID;
        hallBuffer->msg_OK = 0;
      }
      break;
    }
    case GET_ID: {
      hallBuffer->hallID.ID = ch;
      hallBuffer->status = GET_LENGTH;
      break;
    }
    case GET_LENGTH: {
      hallBuffer->length = ch;
      hallBuffer->dataIndex = 0;
      hallBuffer->status = GET_DATA;
      if (0 == hallBuffer->length) {
        hallBuffer->status = GET_CHECKSUM;
        hallBuffer->checkSum = 0;
      }
      break;
    }
    case GET_DATA: {
      hallBuffer->data[hallBuffer->dataIndex++] = ch;
      if (hallBuffer->dataIndex >= hallBuffer->length) {
        hallBuffer->checkSum = 0;
        hallBuffer->dataIndex = 0;
        hallBuffer->status = GET_STATE;
      }
      break;
    }
    case GET_STATE: {
      hallBuffer->checkSum = 0;
      hallBuffer->dataIndex = 0;
      hallBuffer->status = GET_CHECKSUM;
    }
    // fall through
    case GET_CHECKSUM: {
      hallBuffer->checkSum |= ch << ((hallBuffer->dataIndex++) * 8);
      if (hallBuffer->dataIndex >= 2) {
        hallBuffer->dataIndex = 0;
        hallBuffer->status = CHECKSUM;
        // fall through
      } else {
        break;
      }
    }
    case CHECKSUM: {
      if (hallBuffer->checkSum ==
          crc16(CRC_1021, &hallBuffer->head, hallBuffer->length + 3, 0xffff)) {
        hallBuffer->msg_OK = 1;
      }
      hallBuffer->status = GET_START;
    }
  }
  return;
}

void flysky_hall_process_byte(uint8_t byte)
{
  HallProtocol.index++;

  Parse_Character(&HallProtocol, byte);
  if ( HallProtocol.msg_OK )
  {
    HallProtocol.msg_OK = 0;
    HallProtocol.stickState = HallProtocol.data[HallProtocol.length - 1];

    switch ( HallProtocol.hallID.hall_Id.receiverID )
    {
    case TRANSFER_DIR_TXMCU:
      if(HallProtocol.hallID.hall_Id.packetID == FLYSKY_HALL_RESP_TYPE_VALUES) {
        memcpy(hall_raw_values, HallProtocol.data, sizeof(hall_raw_values));
        for ( uint8_t channel = 0; channel < 4; channel++ )
        {
          hall_adc_values[channel] = 19000 - (FLYSKY_OFFSET_VALUE + hall_raw_values[channel]);
          setAnalogValue(channel, hall_adc_values[channel]);
        }
        //TRACE("Gimbal reading %d %d %d %d", hall_adc_values[0], hall_adc_values[1], hall_adc_values[2], hall_adc_values[3]);
      }
      break;
    }
    globalData.flyskygimbals = true;
  }
}

static void flysky_hall_stick_decode_cb(rmt_ctx_t *ctx, rmt_rx_done_event_data_t *rxdata)
{
  int b = -1;

  uint16_t data = 0;
  for (size_t i = 0; i < rxdata->num_symbols; i++) {
    uint32_t low = rxdata->received_symbols[i].duration0;
    uint32_t high = rxdata->received_symbols[i].duration1;

    uint32_t low_bits = low / 43;
    uint32_t high_bits = high / 43;

    b += low_bits;
    data |= ((1 << high_bits) - 1) << b;
    b += high_bits;

    if (b >= 8) {
      flysky_hall_process_byte((uint8_t)(data & 0xFF));
      data = 0;
      b = -1;
    }
  }
}

