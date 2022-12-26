
#ifndef _EXTMOD_DRIVER_H_
#define _EXTMOD_DRIVER_H_

void extmoduleSerialStart();

void extmoduleSendNextFramePpm(void* pulses, uint16_t length,
                               uint16_t ppm_delay, bool polarity);

void extmoduleSendNextFrameSoftSerial(const void* pulses, uint16_t length,
                                      bool polarity = true);

void extmoduleInitTxPin();
void extmoduleSendInvertedByte(uint8_t byte);

#endif