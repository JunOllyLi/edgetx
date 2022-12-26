
#ifndef EDTX_I2C_DRIVER_H_
#define EDTX_I2C_DRIVER_H_

#define I2C_MASTER_FREQ_HZ 400000
#define I2C_MASTER_TIMEOUT_MS 1000

esp_err_t i2c_register_read(uint8_t device_address, uint8_t reg_addr, uint8_t *data, size_t len);
esp_err_t i2c_register_write_byte(uint8_t device_address, uint8_t reg_addr, uint8_t data);
esp_err_t i2c_register_write_uint16(uint8_t device_address, uint8_t reg_addr, uint16_t data);
esp_err_t i2c_register_write_buf(uint8_t device_address, uint8_t *buf, size_t len);
esp_err_t i2c_register_write_read_buf(uint8_t device_address, uint8_t *wbuf, size_t wlen, uint8_t *rbuf, size_t rlen);
esp_err_t i2c_master_init(void);

#endif