#ifndef	__EEPROM__
#define	__EEPROM__
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>

#include "bsp_btn_ble.h"
#include "../config.h"
#include "nrf_drv_twi.h"
#include "nrf_cli.h"
#include "nrf_cli_uart.h"

ret_code_t eeprom_write(nrf_drv_twi_t const * p_instance, uint16_t addr, uint8_t const * pdata, size_t size);
ret_code_t eeprom_read(nrf_drv_twi_t const * p_instance, uint16_t addr, uint8_t * pdata, size_t size);
#endif // __EEPROM__