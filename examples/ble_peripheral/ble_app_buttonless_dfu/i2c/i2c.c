#include "i2c.h"
#include "eeprom.h"

static const nrf_drv_twi_t m_twi_master = NRF_DRV_TWI_INSTANCE(MASTER_TWI_INST);

static ret_code_t twi_master_init(void);
static ret_code_t i2c_read_register(nrf_drv_twi_t twi_instance, uint8_t device_addr, uint8_t register_addr, uint8_t *p_data, uint8_t bytes, bool no_stop);
static ret_code_t i2c_read_register_sensor(nrf_drv_twi_t twi_instance, uint8_t device_addr, uint8_t *p_data, uint8_t bytes);
static ret_code_t i2c_write_register(nrf_drv_twi_t twi_instance, uint8_t device_addr, uint8_t *tx_data, uint8_t bytes, bool no_stop);

static volatile bool m_xfer_done = false;

uint8_t debug_device_addr;
uint8_t debug_tx_data;
uint8_t debug_register_addr;
void i2c_init(void)
{
    ret_code_t err_code;

    /* Initializing TWI master interface for EEPROM */
    err_code = twi_master_init();
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief TWI events handler.
 */
void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{
    switch (p_event->type)
    {
	
        case NRF_DRV_TWI_EVT_DONE:
            switch (p_event->xfer_desc.type)
            {
                case NRF_DRV_TWI_XFER_TX:
                    printf("TWI TX Done. \r\n");
	            m_xfer_done = true;
                    break;
                case NRF_DRV_TWI_XFER_RX:
                    printf("TWI RX Done. \r\n");
	            m_xfer_done = true;
                    break;
                default:
                    break;
            }
            break;
	case NRF_DRV_TWI_EVT_ADDRESS_NACK:
	    	printf("NRF_DRV_TWI_EVT_ADDRESS_NACK. 0x%x 0x%x \r\n", debug_device_addr, debug_tx_data);
            break;
	case NRF_DRV_TWI_EVT_DATA_NACK:
	    	printf("NRF_DRV_TWI_EVT_DATA_NACK. 0x%x 0x%x \r\n", debug_device_addr, debug_tx_data);
            break;
        default:
            break;
    }
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

    ret = nrf_drv_twi_init(&m_twi_master, &config, twi_handler, NULL);

    if (NRF_SUCCESS == ret)
    {
        nrf_drv_twi_enable(&m_twi_master);
    }

    return ret;
}

static ret_code_t i2c_read_register(nrf_drv_twi_t twi_instance, uint8_t device_addr, uint8_t register_addr, uint8_t *p_data, uint8_t bytes, bool no_stop)
{
    ret_code_t err_code;
   
    m_xfer_done = false;
    debug_device_addr = device_addr;
    debug_tx_data = &register_addr;
    if(&register_addr == 0x3B)
    {
	printf("RM_OB1203_PpgRead \r\n");
    }else if(register_addr == 0x3B)
    {
	printf("RM_OB1203_PpgRead \r\n");
    }
    printf("register_addr: 0x%x \r\n", register_addr);
    err_code = nrf_drv_twi_tx(&twi_instance, device_addr, register_addr, sizeof(register_addr), no_stop);

    do
    {
	__WFE();
    }while (m_xfer_done == false);

    if(err_code != NRF_SUCCESS) {
	return err_code;
    }

    m_xfer_done = false;
    debug_device_addr = device_addr;
    err_code = nrf_drv_twi_rx(&twi_instance, device_addr, p_data, bytes);

    do
    {
	__WFE();
    }while (m_xfer_done == false);

    if(err_code != NRF_SUCCESS) {
	return err_code;
    }

    if(register_addr == 0x3B)
    {
	printf("RM_OB1203_PpgRead \r\n");
    }
    return err_code;
}

static ret_code_t i2c_write_register(nrf_drv_twi_t twi_instance, uint8_t device_addr, uint8_t *tx_data, uint8_t bytes, bool no_stop)
{
    ret_code_t err_code;

    m_xfer_done = false;
    debug_device_addr = device_addr;
    debug_tx_data = tx_data;
    
    if(tx_data == 0x53)
    {
	printf("tx_data 0x53  \r\n");
    }else if(tx_data == 0x15){
	printf("tx_data 0x15  \r\n");
    }
    printf("tx_data: 0x%x, bytes: 0x%x \r\n", tx_data, bytes);
    err_code = nrf_drv_twi_tx(&twi_instance, device_addr, tx_data, bytes, no_stop);

    do
    {
	__WFE();
    }while (m_xfer_done == false);
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

void i2c_acl_read(uint8_t device_addr, uint8_t register_addr, uint8_t *p_data, uint8_t bytes)
{
    ret_code_t ret;
    ret = i2c_read_register(m_twi_master, device_addr, register_addr, p_data, bytes, true);
}

ret_code_t i2c_acl_write(uint8_t device_addr, uint8_t *tx_data, uint8_t bytes, bool no_stop)
{
    ret_code_t ret;

    ret = i2c_write_register(m_twi_master, device_addr, tx_data, bytes, no_stop);
    return ret;
}

ret_code_t i2c_vib_read(uint8_t device_addr, uint8_t register_addr, uint8_t *p_data, uint8_t bytes, bool no_stop)
{
    ret_code_t ret;

    ret = i2c_read_register(m_twi_master, device_addr, register_addr, p_data, bytes, no_stop);
    return ret;
}

ret_code_t i2c_vib_write(uint8_t device_addr, uint8_t *tx_data, uint8_t bytes, bool no_stop)
{
    ret_code_t ret;

    ret = i2c_write_register(m_twi_master, device_addr, tx_data, bytes, no_stop);
    return ret;
}

ret_code_t i2c_heart_rate_read(uint8_t device_addr, uint8_t register_addr, uint8_t *p_data, uint8_t bytes, bool no_stop)
{
    ret_code_t ret;

    ret = i2c_read_register(m_twi_master, device_addr, register_addr, p_data, bytes, no_stop);
    return ret;
}

ret_code_t i2c_heart_rate_write(uint8_t *tx_data, uint8_t bytes, uint8_t wait)
{
    ret_code_t ret;
    uint8_t device_addr = HEART_RATE_SENSOR_ADDR;
    if(tx_data == 0x15 && bytes == 0x02)
    {
	printf("debug");
    }

    ret = i2c_write_register(m_twi_master, device_addr, tx_data, bytes, wait);
    return ret;
}
