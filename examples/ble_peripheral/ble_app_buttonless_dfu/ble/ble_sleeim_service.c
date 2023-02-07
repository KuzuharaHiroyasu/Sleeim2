/**
 ***************************************************************************************************
 * @file        sleeim_service.c
 * @brief       Set Sleeim Service
 ***************************************************************************************************
 **************************************************************************************************/
#include "sdk_common.h"
#include "ble_sleeim_service.h"
#include "ble_srv_common.h"
#include "../main.h"
#include "app_error.h"

static void ble_indication_end(void);
static void ble_disconnect_timer_reset(void);

static void ble_check_device_info(void);
static void ble_check_inspection(void);
static void ble_check_operation(void);


/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_sleeim       sleeim Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_sleeim_t * p_sleeim, ble_evt_t const * p_ble_evt)
{
    p_sleeim->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}


/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_sleeim       SmartLock Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_sleeim_t * p_sleeim, ble_evt_t const * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_sleeim->conn_handle = BLE_CONN_HANDLE_INVALID;
}


/**
 ***************************************************************************************************
 *	@brief			on read function
 *	@details		Function for handling the Read event.
 *      @param[in] p_sleeim  Sleeim Service structure.
 *      @param[in] p_ble_evt    Event received from the BLE stack.
 *      @param[in] data         Response data.
 **************************************************************************************************/
static void on_read(ble_sleeim_t * p_sleeim, ble_evt_t const * p_ble_evt, uint8_t* data, uint8_t data_size)
{
    uint32_t err_code;

    ble_gatts_rw_authorize_reply_params_t reply_params;

    memset(&reply_params, 0, sizeof(reply_params));
      
    reply_params.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
    reply_params.params.read.p_data    = data;
    reply_params.params.read.len       = data_size;//sizeof(&data);//
    reply_params.params.read.offset    = 0;
    reply_params.params.read.update    = 1;
    reply_params.params.read.gatt_status = NRF_SUCCESS;
      
    sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gap_evt.conn_handle, &reply_params);
}

/**
 ***************************************************************************************************
 *	@brief			on write function
 *	@details		Function for handling the Write event.
 *      @param[in] p_sleeim  Sleeim Service structure.
 *      @param[in] p_ble_evt    Event received from the BLE stack.
 **************************************************************************************************/
static void on_write(ble_sleeim_t * p_sleeim, ble_evt_t const * p_ble_evt)
{
    bool result;
    bool analysis_result;
    uint32_t err_code;
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
	
    if ((p_evt_write->handle == p_sleeim->command_trans_handles.value_handle)
        && (p_evt_write->len >= 1)
        && (p_sleeim->sleeim_write_handler != NULL))
    {
        p_sleeim->sleeim_write_handler(p_ble_evt->evt.gap_evt.conn_handle, p_sleeim, &p_evt_write->data[0], p_evt_write->len);

        received_write_data_len = p_evt_write->len;
        write_flag = true;
	
	main_ble_proc(p_evt_write->data, received_write_data_len);
/*
        // Store received data in write buffer
        for(int i=0; i<p_evt_write->len; i++)
        {
            write_buf[i] = p_evt_write->data[i];
        }
*/
    }
}

/**
 ***************************************************************************************************
 *	@brief			on write function
 *	@details		Function for handling the Write event.
 *      @param[in] p_sleeim  Sleeim Service structure.
 *      @param[in] p_ble_evt    Event received from the BLE stack.
 **************************************************************************************************/
static void on_write_res(ble_sleeim_t * p_sleeim, ble_evt_t const * p_ble_evt)
{
    bool result;
    bool analysis_result;
    uint32_t err_code;
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.write;

    uint8_t handle=p_ble_evt->evt.gatts_evt.params.authorize_request.request.write.handle;
    ble_gatts_rw_authorize_reply_params_t auth_reply;
    memset(&auth_reply, 0, sizeof(auth_reply));
    auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
    auth_reply.params.write.offset    = 0;
    auth_reply.params.write.update    = 1;

    if(handle == p_sleeim->command_trans_handles.value_handle
        && (p_evt_write->len >= 1)
        && (p_sleeim->sleeim_write_handler != NULL))
    {
        auth_reply.params.write.gatt_status = BLE_GATT_STATUS_SUCCESS;
        err_code=sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gatts_evt.conn_handle,&auth_reply);
        //err_code=sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gap_evt.conn_handle, &auth_reply);
        APP_ERROR_CHECK(err_code);

        p_sleeim->sleeim_write_handler(p_ble_evt->evt.gatts_evt.conn_handle, p_sleeim, &p_evt_write->data[0], p_evt_write->len);
        received_write_data_len = p_evt_write->len;
        write_flag = true;

	main_ble_proc(p_evt_write->data, received_write_data_len);
/*
        // Store received data in write buffer
        for(int i=0; i<p_evt_write->len; i++)
        {
            write_buf[i] = p_evt_write->data[i];
        }
*/
    }
}

/**
 ***************************************************************************************************
 *	@brief			on BLE EVENT function
 *	@details		Function for handling the BLE event.
 *      @param[in] p_ble_evt    Event received from the BLE stack.
 **************************************************************************************************/
void ble_sleeim_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    uint32_t err_code;
    ble_sleeim_t * p_sleeim = (ble_sleeim_t *)p_context;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_sleeim, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_sleeim, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            on_write(p_sleeim, p_ble_evt);
            break;

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
            if ( p_ble_evt->evt.gatts_evt.params.authorize_request.type == BLE_GATTS_AUTHORIZE_TYPE_READ  )
            {
                if (p_ble_evt->evt.gatts_evt.params.authorize_request.request.read.handle == p_sleeim->sleeim_read_handles.value_handle)
                {
                    on_read(p_sleeim, p_ble_evt, &read_buf[0], 10);
                }
            }
            else if(p_ble_evt->evt.gatts_evt.params.authorize_request.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)
            {
                on_write_res(p_sleeim, p_ble_evt);
            }
            break;

        case BLE_GATTS_EVT_HVC:
            ble_indicate_ack = true;
            indication_packet_count += 1;
            ble_indicate_enable = true;
            break;


        default:
            // No implementation needed.
            break;
    }
}

/**
 ***************************************************************************************************
 *	@brief			LockState Characteristic Setting
 *	@details		Setting LockState Characteristic
 *      @param[in] p_sleeim  Sleeim Service structure.
 **************************************************************************************************/
static uint32_t sleeim_read_char_add(ble_sleeim_t * p_sleeim, const ble_sleeim_init_t * p_sleeim_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.write  = 0;
    char_md.char_props.notify   = 0;
    char_md.char_props.indicate = 0;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
    char_md.p_sccd_md         = NULL;
    
    ble_uuid.type = p_sleeim->uuid_type;
    ble_uuid.uuid = SLEEIM_READ_UUID_CHAR;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 1;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;
    //attr_md.read_perm.sm = 1;      // Security Mode 1 Level 1: Open link
    //attr_md.read_perm.lv = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = MAX_READ_NUM;//sizeof(uint8_t);
    attr_char_value.p_value   = NULL;

    return sd_ble_gatts_characteristic_add(p_sleeim->service_handle, 
                                               &char_md,
                                               &attr_char_value,
                                               &p_sleeim->sleeim_read_handles);
}

/**
 ***************************************************************************************************
 *	@brief			Write Characteristic Setting
 *	@details		Setting Write Characteristic
 *      @param[in] p_sleeim  Sleeim Service structure.
 **************************************************************************************************/
static uint32_t command_write_char_add(ble_sleeim_t * p_sleeim, const ble_sleeim_init_t * p_sleeim_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 0;
    char_md.char_props.write  = 1;
    char_md.char_props.notify = 0;
    char_md.char_props.indicate = 0;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = p_sleeim->uuid_type;
    ble_uuid.uuid = SLEEIM_WRITE_UUID_CHAR;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 1;  //Write with Response
    attr_md.vlen       = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = MAX_WRITE_NUM;//sizeof(uint8_t);
    attr_char_value.p_value   = NULL;

    return sd_ble_gatts_characteristic_add(p_sleeim->service_handle, 
                                               &char_md,
                                               &attr_char_value,
                                               &p_sleeim->command_trans_handles);
}

/**
 ***************************************************************************************************
 *	@brief			Indication Characteristic Setting
 *	@details		Setting Indication Characteristic
 *      @param[in] p_sleeim  Sleeim Service structure.
 **************************************************************************************************/
static uint32_t sleeim_indication_char_add(ble_sleeim_t * p_sleeim, const ble_sleeim_init_t * p_sleeim_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc       = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 0;
    char_md.char_props.write  = 0;
    char_md.char_props.notify = 0;
    char_md.char_props.indicate = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         =  &cccd_md;
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = p_sleeim->uuid_type;
    ble_uuid.uuid = SLEEIM_INDICATION_UUID_CHAR;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = MAX_INDICATION_NUM;//sizeof(uint8_t);
    attr_char_value.p_value   = NULL;

    return sd_ble_gatts_characteristic_add(p_sleeim->service_handle,                       
    &char_md,
    &attr_char_value,
    &p_sleeim->command_indication_handles);
}


/**
 ***************************************************************************************************
 *	@brief			Notification Characteristic Setting
 *	@details		Setting Notification Characteristic
 *      @param[in] p_sleeim  Sleeim Service structure.
 **************************************************************************************************/
static uint32_t sleeim_notification_char_add(ble_sleeim_t * p_sleeim, const ble_sleeim_init_t * p_sleeim_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc       = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 0;
    char_md.char_props.write  = 0;
    char_md.char_props.notify = 1;
    char_md.char_props.indicate = 0;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         =  &cccd_md;
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = p_sleeim->uuid_type;
    ble_uuid.uuid = SLEEIM_NOTIFICATION_UUID_CHAR;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = MAX_NOTIFICATION_NUM;//sizeof(uint8_t);
    attr_char_value.p_value   = NULL;

    return sd_ble_gatts_characteristic_add(p_sleeim->service_handle,                       
    &char_md,
    &attr_char_value,
    &p_sleeim->command_notification_handles);
}


/**
 ***************************************************************************************************
 *	@brief			Sleeim Service initialization
 *	@details		Initializing Sleeim Service
 *      @param[in] p_sleeim  Sleeim Service structure.
 **************************************************************************************************/
uint32_t ble_sleeim_init(ble_sleeim_t * p_sleeim, const ble_sleeim_init_t * p_sleeim_init)
{
    uint32_t              err_code;
    ble_uuid_t            ble_uuid;
    ble_add_char_params_t add_char_params;

    // Initialize service structure.
    p_sleeim->conn_handle       = BLE_CONN_HANDLE_INVALID;
    p_sleeim->sleeim_write_handler = p_sleeim_init->sleeim_write_handler;
    //p_sleeim->p_sleeim_state = p_sleeim_init->p_sleeim_state;

    // Add service.
    ble_uuid128_t base_uuid = {SLEEIM_UUID_BASE};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_sleeim->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_sleeim->uuid_type;
    ble_uuid.uuid = SLEEIM_UUID_SERVICE;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_sleeim->service_handle);
    VERIFY_SUCCESS(err_code);

    // Add Sleeim Read characteristic
    err_code = sleeim_read_char_add(p_sleeim, p_sleeim_init);
    VERIFY_SUCCESS(err_code);

    // Add Sleeim Indication characteristic
    err_code = sleeim_indication_char_add(p_sleeim, p_sleeim_init);
    VERIFY_SUCCESS(err_code);

    // Add Sleeim Notification characteristic
    err_code = sleeim_notification_char_add(p_sleeim, p_sleeim_init);
    VERIFY_SUCCESS(err_code);

    // Add Lock Sleeim Write characteristic
    err_code = command_write_char_add(p_sleeim, p_sleeim_init);
    VERIFY_SUCCESS(err_code);

    return err_code;
}

/**
 ***************************************************************************************************
  Notify Data
 ***************************************************************************************************
 
/**
 ***************************************************************************************************
 *	@brief			Notification transmission function
 *	@details		transmission Notification
 *      @param[in] p_sleeim  Sleeim Service structure.
 **************************************************************************************************/
uint32_t ble_sleeim_notification(ble_sleeim_t * p_sleeim, uint8_t *data, uint16_t len)
{
    uint32_t ret;
    ble_gatts_hvx_params_t params;

    memset(&params, 0, sizeof(params));
    params.type = BLE_GATT_HVX_NOTIFICATION;
    params.handle = p_sleeim->command_notification_handles.value_handle;
    params.p_data = data;
    params.p_len = &len;

    ret = sd_ble_gatts_hvx(p_sleeim->conn_handle, &params);
//    printf("ret = %d ", ret);
    return ret;
}

/**
 ***************************************************************************************************
  Indicate Data
 ***************************************************************************************************
 
/**
 ***************************************************************************************************
 *	@brief			Indication transmission function
 *	@details		transmission Indication
 *      @param[in] p_sleeim  Sleeim Service structure.
 **************************************************************************************************/
uint32_t ble_sleeim_indication(ble_sleeim_t * p_sleeim, uint8_t *data, uint16_t len)
{
    ble_gatts_hvx_params_t params;

    memset(&params, 0, sizeof(params));
    params.type = BLE_GATT_HVX_INDICATION;
    params.handle = p_sleeim->command_indication_handles.value_handle;
    params.p_data = data;
    params.p_len = &len;

    return sd_ble_gatts_hvx(p_sleeim->conn_handle, &params);
}

/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/ 
static uint8_t notification_test_cyc = 0;
static uint8_t indication_test_cyc = 0;
static uint8_t send_data[10] = "0123456789";
/*
void peripheral_notification_test(void)
{
    uint8_t indication_size = 10;//sizeof(data);

    if(true == sys_timer_limit[BLE_TEST_TIMER] && true == ble_connect_flag)
    {
        sys_timer_limit[BLE_TEST_TIMER] = false;

        for(int i=0; i<10; i++)
        {
            send_data[i] = '0'+ notification_test_cyc%10;
        }

        notification_exe(&send_data[0], indication_size);

        notification_test_cyc += 1;
    }
}

void peripheral_indication_test(void)
{
    uint8_t indication_size = 10;//sizeof(data);

    if(true == sys_timer_limit[BLE_TEST_TIMER] && true == ble_connect_flag
         && true == ble_indicate_enable)
    {
        sys_timer_limit[BLE_TEST_TIMER] = false;

        for(int i=0; i<10; i++)
        {
            send_data[i] = '0'+ indication_test_cyc%10;
        }

        indication_exe(&send_data[0], indication_size);
        ble_indicate_enable = false;

        indication_test_cyc += 1;
    }
}
*/
void peripheral_write_notification_test(void)
{
    if(true == ble_connect_flag && true == write_flag)
    {
        write_flag = false;
        notification_exe(&write_buf[0], received_write_data_len);
//	printf("%x ", write_buf[1]);
    }
}


void peripheral_write_indication_test(void)
{
    if(true == ble_connect_flag && true == write_flag
         && true == ble_indicate_enable)
    {
        write_flag = false;
        ble_indicate_enable = false;
        indication_exe(&write_buf[0], received_write_data_len);
    }
}
