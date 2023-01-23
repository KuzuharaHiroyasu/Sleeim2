#include "i2c.h"
#include "eeprom.h"

static const nrf_drv_twi_t m_twi_master = NRF_DRV_TWI_INSTANCE(MASTER_TWI_INST);

static ret_code_t twi_master_init(void);
static ret_code_t i2c_read_register(nrf_drv_twi_t twi_instance, uint8_t device_addr, uint8_t register_addr, uint8_t *p_data, uint8_t bytes, bool no_stop);
static ret_code_t i2c_write_register(nrf_drv_twi_t twi_instance, uint8_t device_addr, uint8_t *tx_data, uint8_t bytes, bool no_stop);

void i2c_init(void)
{
    ret_code_t err_code;

    /* Initializing TWI master interface for EEPROM */
    err_code = twi_master_init();
    APP_ERROR_CHECK(err_code);
}

static ret_code_t twi_master_init(void)
{
    ret_code_t ret;
    const nrf_drv_twi_config_t config =
    {
       .scl                = TWI_SCL_M,
       .sda                = TWI_SDA_M,
       .frequency          = NRF_DRV_TWI_FREQ_400K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = false
    };

    ret = nrf_drv_twi_init(&m_twi_master, &config, NULL, NULL);

    if (NRF_SUCCESS == ret)
    {
        nrf_drv_twi_enable(&m_twi_master);
    }

    return ret;
}

static ret_code_t i2c_read_register(nrf_drv_twi_t twi_instance, uint8_t device_addr, uint8_t register_addr, uint8_t *p_data, uint8_t bytes, bool no_stop)
{
    ret_code_t err_code;

    err_code = nrf_drv_twi_tx(&twi_instance, device_addr, &register_addr, 1, no_stop);
    APP_ERROR_CHECK(err_code);

    if(err_code != NRF_SUCCESS) {
	return err_code;
    }

    err_code = nrf_drv_twi_rx(&twi_instance, device_addr, p_data, bytes);
    return err_code;
}

static ret_code_t i2c_write_register(nrf_drv_twi_t twi_instance, uint8_t device_addr, uint8_t *tx_data, uint8_t bytes, bool no_stop)
{
    ret_code_t err_code;

    err_code = nrf_drv_twi_tx(&twi_instance, device_addr, tx_data, bytes, no_stop);
    APP_ERROR_CHECK(err_code);

    if(err_code != NRF_SUCCESS) {
	return err_code;
    }

    return err_code;
} 

void i2c_eeprom_read(uint8_t addr, uint8_t *pdata, uint16_t len)
{
    ret_code_t ret;
    
    ret = eeprom_read(&m_twi_master, addr, pdata, len);
}

void i2c_eeprom_write(uint8_t addr, uint8_t *pdata, uint16_t len)
{
    ret_code_t ret;
    
    ret = eeprom_write(&m_twi_master, addr, pdata, len);
}

void i2c_acl_read(uint8_t device_addr, uint8_t register_addr, uint8_t *p_data, uint8_t bytes, bool no_stop)
{
    ret_code_t ret;
    ret = i2c_read_register(m_twi_master, device_addr, register_addr, p_data, bytes, no_stop);
}

void i2c_acl_write(uint8_t device_addr, uint8_t *tx_data, uint8_t bytes, bool no_stop)
{
    ret_code_t ret;

    ret = i2c_write_register(m_twi_master, device_addr, tx_data, bytes, no_stop);
}

void i2c_vib_read(void)
{
}

void i2c_vib_write(void)
{
}

void i2c_heart_rate_read(void)
{
}
