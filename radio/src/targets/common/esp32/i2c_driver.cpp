
#include <opentx.h>
#include <driver/i2c.h>
#include "i2c_driver.h"

esp_err_t i2c_register_read(uint8_t device_address, uint8_t reg_addr, uint8_t *data, size_t len)
{
    return i2c_master_write_read_device(I2C_MASTER_NUM, device_address, &reg_addr, 1, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

esp_err_t i2c_register_write_byte(uint8_t device_address, uint8_t reg_addr, uint8_t data)
{
    int ret;
    uint8_t write_buf[2] = {reg_addr, data};

    ret = i2c_master_write_to_device(I2C_MASTER_NUM, device_address, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);

    return ret;
}

esp_err_t i2c_register_write_uint16(uint8_t device_address, uint8_t reg_addr, uint16_t data)
{
    int ret;
    uint8_t write_buf[3] = {reg_addr, (uint8_t)(data >> 8), (uint8_t)(data & 0xFF)};

    ret = i2c_master_write_to_device(I2C_MASTER_NUM, device_address, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);

    return ret;
}

esp_err_t i2c_register_write_read_buf(uint8_t device_address, uint8_t *wbuf, size_t wlen, uint8_t *rbuf, size_t rlen) {
    esp_err_t ret;
    ret = i2c_master_write_read_device(I2C_MASTER_NUM, device_address,
                                       wbuf, wlen, rbuf, rlen,
                                       I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    return ret;
}

esp_err_t i2c_register_write_buf(uint8_t device_address, uint8_t *buf, size_t len)
{
    int ret;
    ret = i2c_master_write_to_device(I2C_MASTER_NUM, device_address, buf, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);

    return ret;
}

#if 0
esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA,
        .scl_io_num = I2C_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master = { .clk_speed = I2C_MASTER_FREQ_HZ }
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
}
#endif
