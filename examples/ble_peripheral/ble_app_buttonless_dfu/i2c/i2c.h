#ifndef	__I2C__
#define	__I2C__
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>

#include "bsp_btn_ble.h"
#include "../config.h"
#include "nrf_drv_twi.h"
#include "nrf_cli.h"
#include "nrf_cli_uart.h"

void i2c_eeprom_read(uint8_t addr, uint8_t *pdata, uint16_t len);
void i2c_eeprom_write(uint8_t addr, uint8_t *pdata, uint16_t len);

void i2c_acl_read(uint8_t device_addr, uint8_t register_addr, uint8_t *p_data, uint8_t bytes);
ret_code_t i2c_acl_write(uint8_t device_addr, uint8_t *tx_data, uint8_t bytes, bool no_stop);

ret_code_t i2c_vib_read(uint8_t device_addr, uint8_t register_addr, uint8_t *p_data, uint8_t bytes, bool no_stop);
ret_code_t i2c_vib_write(uint8_t device_addr, uint8_t *tx_data, uint8_t bytes, bool no_stop);

ret_code_t i2c_heart_rate_read(uint8_t device_addr, uint8_t register_addr, uint8_t *p_data, uint8_t bytes, bool no_stop);
ret_code_t i2c_heart_rate_write(uint8_t *tx_data, uint8_t bytes, uint8_t wait);
#endif // __I2C__