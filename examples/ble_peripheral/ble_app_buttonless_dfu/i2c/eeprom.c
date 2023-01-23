#include "eeprom.h"

/**
 * @brief Write data to simulated EEPROM.
 *
 * Function uses the TWI interface to write data into EEPROM memory.
 *
 * @param     addr  Start address to write.
 * @param[in] pdata Pointer to data to send.
 * @param     size  Byte count of data to send.
 * @attention       Maximum number of bytes that may be written is @ref EEPROM_SIM_SEQ_WRITE_MAX.
 *                  In sequential write, all data must be in the same page
 *                  (higher address bits do not change).
 *
 * @return NRF_SUCCESS or reason of error.
 *
 * @attention If you wish to communicate with real EEPROM memory chip, check its readiness
 * after writing the data.
 */
ret_code_t eeprom_write(nrf_drv_twi_t const * p_instance, uint16_t addr, uint8_t const * pdata, size_t size)
{
    ret_code_t ret;
    /* Memory device supports only a limited number of bytes written in sequence */
    if (size > (EEPROM_SIM_SEQ_WRITE_MAX_BYTES))
    {
        return NRF_ERROR_INVALID_LENGTH;
    }
    /* All written data must be in the same page */
    if ((addr / (EEPROM_SIM_SEQ_WRITE_MAX_BYTES)) != ((addr + size - 1) / (EEPROM_SIM_SEQ_WRITE_MAX_BYTES)))
    {
        return NRF_ERROR_INVALID_ADDR;
    }
    do
    {
        uint8_t buffer[EEPROM_SIM_ADDRESS_LEN_BYTES + EEPROM_SIM_SEQ_WRITE_MAX_BYTES]; /* Addr + data */

        memcpy(buffer, &addr, EEPROM_SIM_ADDRESS_LEN_BYTES);
        memcpy(buffer + EEPROM_SIM_ADDRESS_LEN_BYTES, pdata, size);
        ret = nrf_drv_twi_tx(p_instance, EEPROM_SIM_ADDR, buffer, size + EEPROM_SIM_ADDRESS_LEN_BYTES, false);
    }while (0);

    return ret;
}


/**
 * @brief Read data from simulated EEPROM.
 *
 * Function uses the TWI interface to read data from EEPROM memory.
 *
 * @param     addr  Start address to read.
 * @param[in] pdata Pointer to the buffer to fill with data.
 * @param     size  Byte count of data to read.
 *
 * @return NRF_SUCCESS or reason of error.
 */
ret_code_t eeprom_read(nrf_drv_twi_t const * p_instance, uint16_t addr, uint8_t * pdata, size_t size)
{
    ret_code_t ret;
    if (size > (EEPROM_SIM_SIZE))
    {
        return NRF_ERROR_INVALID_LENGTH;
    }
    do
    {
       uint16_t addr16 = addr;
       ret = nrf_drv_twi_tx(p_instance, EEPROM_SIM_ADDR, (uint8_t *)&addr16, EEPROM_SIM_ADDRESS_LEN_BYTES, true);
       if (NRF_SUCCESS != ret)
       {
           break;
       }
       ret = nrf_drv_twi_rx(p_instance, EEPROM_SIM_ADDR, pdata, size);
    }while (0);
    return ret;
}

/**
 * @brief Check the string length but no more than the given number of characters.
 *
 * Function iterates through the string searching for the zero character.
 * Even when it is not found it stops iterating after maximum of @em nmax characters.
 *
 * @param[in] str  String to check.
 * @param     nmax Maximum number of characters to check.
 *
 * @return String length or @em nmax if no zero character is found.
 */
static size_t safe_strlen(char const * str, size_t nmax)
{
    size_t n=0;
    while ('\0' != *str++)
    {
        ++n;
        if (0 == --nmax)
            break;
    }
    return n;
}