/**
 ***************************************************************************************************
 * @file        orange_service.h
 * @brief       Set Orange Service
 ***************************************************************************************************
 **************************************************************************************************/

#ifndef BLE_ORANGE_H__
#define BLE_ORANGE_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"


#ifdef __cplusplus
extern "C" {
#endif

/**@brief   Macro for defining a ble_orange instance.
 *
 * @param   _name   Name of the instance.
 * @hideinitializer
 */
#define BLE_ORANGE_DEF(_name)                                                                          \
static ble_orange_t _name;                                                                            \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                 \
                     BLE_ORANGE_BLE_OBSERVER_PRIO,                                                     \
                     ble_orange_on_ble_evt, &_name)

//BASE-UUID:77bfxxxx-b245-d7d5-baaf-7cb4aec8129c
//BASE-UUID:1e36xxxx-3b3f-405f-9c2f-94c3cd5dacf4
#define ORANGE_UUID_BASE {0x9c, 0x12, 0xc8, 0xae, 0xb4, 0x7c, 0xaf, 0xba, \
                              0xd5, 0xd7, 0x45, 0xb2, 0x00, 0x00, 0xbf, 0x77}
//
////BASE-UUID:4ab4xxxx-65bc-126e-3b8c-8345de91b0d3
//#define ORANGE_UUID_BASE {0xd3, 0xb0, 0x91, 0xde, 0x45, 0x83, 0x8c, 0x3b, \
//                              0x6e, 0x12, 0xbc, 0x65, 0x00, 0x00, 0xb4, 0x4a}

#define BLE_WRITE_DATA_LENGTH (NRF_SDH_BLE_GATT_MAX_MTU_SIZE-3)
#define BLE_INDICATION_DATA_LENGTH (NRF_SDH_BLE_GATT_MAX_MTU_SIZE-3)
#define BLE_NOTIFICATION_DATA_LENGTH (NRF_SDH_BLE_GATT_MAX_MTU_SIZE-3)
#define BLE_READ_DATA_LENGTH (32)

#define ORANGE_UUID_SERVICE               0xABC0
#define ORANGE_READ_UUID_CHAR             0xABC1  //Read
#define ORANGE_INDICATION_UUID_CHAR       0xABC2  //Indication
#define ORANGE_NOTIFICATION_UUID_CHAR     0xABC3  //Notification
#define ORANGE_WRITE_UUID_CHAR            0xABC4  //Write

#define	MAX_READ_NUM            BLE_READ_DATA_LENGTH
#define	MAX_INDICATION_NUM	BLE_INDICATION_DATA_LENGTH
#define	MAX_NOTIFICATION_NUM	BLE_NOTIFICATION_DATA_LENGTH
#define	MAX_WRITE_NUM           BLE_WRITE_DATA_LENGTH
#define	MAX_NOTIFY_NUM          BLE_NOTIFY_DATA_LENGTH
#define BLE_MAX_DATA_LEN (BLE_GATT_ATT_MTU_DEFAULT - 3) /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */

typedef enum
{
    BLE_ORANGE_EVT_NOTIFICATION_ENABLED,  
    BLE_ORANGE_EVT_NOTIFICATION_DISABLED  
} ble_orange_evt_type_t;

typedef struct
{
    ble_orange_evt_type_t evt_type; 
} ble_orange_evt_t;

// Forward declaration of the ble_orange_t type.
typedef struct ble_orange_s ble_orange_t;

typedef void (*ble_orange_write_handler_t) (uint16_t conn_handle, ble_orange_t * p_orange, const uint8_t *receive_buff, uint8_t length);

/**
 ***************************************************************************************************
 *	@brief			Smart Lock Service init structure.
 *	@details		This structure contains all options and data needed for
 *                              initialization of the service.
 **************************************************************************************************/
typedef struct
{
    ble_orange_write_handler_t orange_write_handler; /**< Event handler to be called when the Characteristic is written. */
    //uint8_t *                   p_orange_state;
} ble_orange_init_t;

/**
 ***************************************************************************************************
 *	@brief			Smart Lock Service structure.
 *	@details		This structure contains various status information for the service.
 **************************************************************************************************/
struct ble_orange_s
{
    uint16_t                    service_handle;      /**< Handle of Smart Lock Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t    orange_read_handles;  /**< Handles related to the LockState Characteristic. */
    ble_gatts_char_handles_t    command_notification_handles; /**< Handles related to the CommandResponse Characteristic. */
    ble_gatts_char_handles_t    command_indication_handles; /**< Handles related to the CommandResponse Characteristic. */
    ble_gatts_char_handles_t    command_trans_handles;  /**< Handles related to the CommandTransmission Characteristic. */
    uint8_t                     uuid_type;           /**< UUID type for the Smart Lock Service. */
    uint16_t                    conn_handle; 
    //uint8_t *                   p_orange_state;
    ble_orange_write_handler_t orange_write_handler;   /**< Event handler to be called when the LED Characteristic is written. */
};

 /**
 ***************************************************************************************************
 *	@brief			Function for initializing the Smart Lock Service.
 *	@param[out] p_orange
 *                Smart Lock Service structure. This structure must be supplied by
 *                the application. It is initialized by this function and will later
 *                be used to identify this particular service instance.
 *	@param[in] p_orange_init
 *                Information needed to initialize the service.
 *	@retval NRF_SUCCESS 
 *                If the service was initialized successfully. Otherwise, an error code is returned.
 **************************************************************************************************/
uint32_t ble_orange_init(ble_orange_t * p_orange, const ble_orange_init_t * p_orange_init);


/**@brief Function for handling the application's BLE stack events.
 *
 * @details This function handles all events from the BLE stack that are of interest to the LED Button Service.
 *
 * @param[in] p_ble_evt  Event received from the BLE stack.
 * @param[in] p_context  Smart Lock Service structure.
 */
void ble_orange_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);


/**@brief Function for sending a button state notification.
 *
 * @param[in] p_orange Smart Lock Service structure.
 * @param[in] data  Notification data.
 * @param[in] len  length of Notification data.
 *
 * @retval NRF_SUCCESS If the notification was sent successfully. Otherwise, an error code is returned.
 */
uint32_t ble_orange_notification(ble_orange_t * p_orange, uint8_t *data, uint16_t len);
uint32_t ble_orange_indication(ble_orange_t * p_orange, uint8_t *data, uint16_t len);
void ble_indication_start(uint8_t ble_type, uint8_t ble_command);
void ble_write_check(void);
void ble_indication_exe(void);
void ble_notification_exe(void);
void peripheral_notification_test(void);
void peripheral_indication_test(void);
void peripheral_write_notification_test(void);
void peripheral_write_indication_test(void);


#ifdef __cplusplus
}
#endif

#endif // BLE_ORANGE_H__

/** @} */
