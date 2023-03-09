#include "../saadc.h"
#include "ble_command.h"
#include "../state_manager.h"
#include "time.h"

static void main_ble_rcv_mode_chg( void );
static void main_ble_rcv_date( void );
static void main_ble_rcv_info( void );
static void main_ble_rcv_version( void );
static void main_ble_rcv_device_info( void );
//void main_ble_set_mode( void );
static void main_ble_rcv_data_end( void );
static void main_ble_rcv_data_frame( void );
static void main_ble_rcv_data_calc( void );
static void main_ble_rcv_data_fin( void );
static void main_ble_rcv_device_set( void );
static void main_ble_rcv_vib_confirm( void );
static void main_ble_rcv_vib_stop( void );
static void main_ble_rcv_power_off( void );

//検査モード
static void main_ble_diag_rcv_power_off( void );
static void main_ble_diag_rcv_charge( void );
static void main_ble_diag_rcv_led( void );
static void main_ble_diag_rcv_vib( void );
static void main_ble_diag_rcv_mic( void );
static void main_ble_diag_rcv_acl( void );
static void main_ble_diag_rcv_photo( void );
static void main_ble_diag_rcv_eep( void );

static void set_ble_rcv_data(uint8_t *p_data, uint8_t len );
static void data_get_before( void );
static void data_get_after( void );
static uint8_t data_get_frame_before( void );

uint8_t	    bd_device_adrs[BD_ADRS_NUM];
uint16_t    get_mode_timeout = 0;
int	    diagScanDataSendCnt = 0;
uint32_t    tick_power_off = 0;
#include "ble_command_tbl.h"

const uint8_t s_eep_page0_tbl[EEP_ACCESS_ONCE_SIZE] = { 0 };

// デフォルト設定値
static uint8_t act_mode = ACT_MODE_SNORE;		    // 動作モード
static uint8_t vib_power = VIB_SET_MODE_GURADUALLY;	    // 振動の強さ
static uint8_t snore_detect_level = SNORE_DETECT_LEVEL_MID;  // いびき検出感度
static uint8_t vib_max_time = SET_MAX_VIB_CONT_10_MIN;	    // 振動連続最大時間
static uint8_t vib_start_time = VIB_START_TIME;		    // 振動開始時間

static BLE_INPUT ble_rx_data;
static PRODUCT_CHECK s_product_check = {0};

void main_set_bd_adrs(void)
{
    ret_code_t     err_code;
    ble_gap_addr_t device_addr;

    // Get BLE address.
    err_code = sd_ble_gap_addr_get(&device_addr);
    VERIFY_SUCCESS(err_code);

    bd_device_adrs[0] = device_addr.addr[0];
    bd_device_adrs[1] = device_addr.addr[1];
    bd_device_adrs[2] = device_addr.addr[2];
    bd_device_adrs[3] = device_addr.addr[3];
    bd_device_adrs[4] = device_addr.addr[4];
    bd_device_adrs[5] = device_addr.addr[5];

}

void main_ble_proc(uint8_t *p_data, uint8_t len )
{
    int i  =0;
	
    if( 0 == len ){
	return;		//受信なし
    }

    if( SYSTEM_MODE_INITIAL == get_current_mode() ){
    // RD8001暫定：イニシャル時は受信せずクリア
    // Debug_coment_out	
//	return;		//受信なし
    }
    
    // rcvデータ保存
    set_ble_rcv_data(p_data, len);

    /* 以降受信データ有り */
    for( i = BLE_CMD_TYPE_NONE; i < BLE_CMD_TYPE_MAX; i++){
        if( get_current_mode() != SYSTEM_MODE_PRODUCT_CHECK)
        {
	    if( s_ble_rcv_func_tbl[i].len == len )
	    {
		printf("len: %d \r\n", s_ble_rcv_func_tbl[i].len);
        	/* データ長が一致 */
		if(( s_ble_rcv_func_tbl[i].cmd == p_data[0] ) || ( s_ble_rcv_func_tbl[i].cmd == BLE_CMD_INVALID )){
		    /* 受信コマンドとコマンドテーブルが一致またはコマンド無効 */
		    if( NULL != s_ble_rcv_func_tbl[i].func ){
			/* 受信処理有り */
			s_ble_rcv_func_tbl[i].func();
		    }
		}
	    }
	}else{
	    if( s_ble_diag_rcv_func_tbl[i].len == len ){
                /* データ長が一致 */
                if(( s_ble_diag_rcv_func_tbl[i].cmd == p_data[0] ) || ( s_ble_diag_rcv_func_tbl[i].cmd == BLE_CMD_INVALID )){
                    /* 受信コマンドとコマンドテーブルが一致またはコマンド無効 */
                    if( NULL != s_ble_diag_rcv_func_tbl[i].func ){
	                /* 受信処理有り */
                        s_ble_diag_rcv_func_tbl[i].func();
                    }
                }
            }
        }
    }
}

void main_ble_send( uint8_t *p_data, uint8_t len )
{
    memset(write_buf, 0, sizeof(write_buf));
    for(int i=0; i<len; i++)
    {
	write_buf[i] = p_data[i];
    }    
//    set_ble_rx_res_cmd(p_data, len);
}
/*
void main_ble_set_mode( void )
{
	uint8_t tx[BLE_DATA_SIZE_MAX] = {0};
	
	
	// OK応答
	tx[0] = BLE_CMD_SET_CHG;
	tx[1] = 0x00;
	
	main_ble_send( &tx[0], BLE_SND_LEN_SET_CHG );

//	s_unit.calc_cnt = 0;
}
*/
static void main_ble_rcv_mode_chg( void )
{
    uint8_t ret = true;
    uint8_t tx[BLE_DATA_SIZE_MAX] = {0};
    
    if( SYSTEM_MODE_GET == ble_rx_data.rcv_data[1] ){
		ret = evt_act( EVENT_GET_DATA );
	if( true == ret){
	    get_mode_timeout = 0;
	} 
    }else if( SYSTEM_MODE_PRODUCT_CHECK == ble_rx_data.rcv_data[1] ){
	ret = evt_act( EVENT_PRODUCT_CHECK );
    }else{
    }
    
    if( false == ret ){
	// NG応答
	tx[0] = BLE_CMD_MODE_CHG;
	tx[1] = BLE_DATA_RESULT_NG;
	main_ble_send( &tx[0], BLE_SND_LEN_MODE_CHG );
    }else{
	// OK応答
	tx[0] = BLE_CMD_MODE_CHG;
	tx[1] = BLE_DATA_RESULT_OK;
	main_ble_send( &tx[0], BLE_SND_LEN_MODE_CHG );
    }
}

static void main_ble_rcv_date( void )
{
    uint8_t tx[BLE_DATA_SIZE_MAX] = {0};
    uint32_t year, month, week, day, hour, minute, second;

    year   = 2000 + ble_rx_data.rcv_data[1];
    month  = ble_rx_data.rcv_data[2];
    week   = ble_rx_data.rcv_data[3];
    day    = ble_rx_data.rcv_data[4];
    hour   = ble_rx_data.rcv_data[5];
    minute = ble_rx_data.rcv_data[6];
    second = ble_rx_data.rcv_data[7];

    nrf_cal_set_time(year, month, week, day, hour, minute, second);

    tx[0] = BLE_CMD_DATE_SET;
    tx[1] = BLE_DATA_RESULT_OK;
    main_ble_send( &tx[0], BLE_SND_LEN_DATE_SET );
}

static void main_ble_rcv_info( void )
{
	uint8_t tx[BLE_DATA_SIZE_MAX] = {0};
	uint8_t result = BLE_DATA_RESULT_OK;
	
	if( get_current_mode() != SYSTEM_MODE_IDLE_COM ){
		result = BLE_DATA_RESULT_NG;
	}
	
	tx[0] = BLE_CMD_INFO;
	tx[1] = result;
//	tx[2] = s_unit.battery_sts;
	main_ble_send( &tx[0], BLE_SND_LEN_INFO );
}

static void main_ble_rcv_version( void )
{
	uint8_t tx[BLE_DATA_SIZE_MAX] = {0};

	// OK応答
	tx[0] = BLE_CMD_VERSION;
	// マイコンバージョン
	tx[1] = BLE_DATA_RESULT_OK;
	tx[2] = version_product_tbl[0];
	tx[3] = version_product_tbl[1];
	tx[4] = version_product_tbl[2];
	tx[5] = version_product_tbl[3];
	main_ble_send( &tx[0], BLE_SND_LEN_VERSION );
}

static void main_ble_rcv_device_info( void )
{
    struct tm* time_struct;
    uint8_t tx[BLE_DATA_SIZE_MAX] = {0};
    uint8_t result = BLE_DATA_RESULT_OK;
	
    if( get_current_mode() != SYSTEM_MODE_IDLE_COM ){
	    result = BLE_DATA_RESULT_NG;
    }

    tx[0] = BLE_CMD_DEVICE_INFO;
    tx[1] = result;							// 結果
    tx[2] = bd_device_adrs[0];		// BDデバイスアドレス
    tx[3] = bd_device_adrs[1];
    tx[4] = bd_device_adrs[2];
    tx[5] = bd_device_adrs[3];
    tx[6] = bd_device_adrs[4];
    tx[7] = bd_device_adrs[5];
    tx[8] = s_unit.frame_num.cnt;

    time_struct = nrf_cal_get_time();

    tx[9]  = time_struct->tm_year;
    tx[10] = time_struct->tm_mon;
    tx[11] = time_struct->tm_wday;
    tx[12] = time_struct->tm_mday;
    tx[13] = time_struct->tm_hour;
    tx[14] = time_struct->tm_min;
    tx[15] = time_struct->tm_sec;
	
    main_ble_send( &tx[0], BLE_SND_LEN_DEVICE_INFO );
}

static void main_ble_rcv_data_end( void )
{
//	user_main_mode_sensing_after();
}

static void main_ble_rcv_data_frame( void )
{
    uint32_t wr_adrs;
    
    s_unit.date.year = ble_rx_data.rcv_data[1];
    s_unit.date.month = ble_rx_data.rcv_data[2];
    s_unit.date.week = ble_rx_data.rcv_data[3];
    s_unit.date.day = ble_rx_data.rcv_data[4];
    s_unit.date.hour = ble_rx_data.rcv_data[5];
    s_unit.date.min = ble_rx_data.rcv_data[6];
    s_unit.date.sec = ble_rx_data.rcv_data[7];
    	
    s_unit.snore_chg_detect_cnt  = ble_rx_data.rcv_data[9] << 8;
    s_unit.snore_chg_detect_cnt  |= ble_rx_data.rcv_data[8];
    s_unit.apnea_chg_detect_cnt  = ble_rx_data.rcv_data[11] << 8;
    s_unit.apnea_chg_detect_cnt  |= ble_rx_data.rcv_data[10];
    s_unit.snore_time  = ble_rx_data.rcv_data[13] << 8;
    s_unit.snore_time  |= ble_rx_data.rcv_data[12];
    s_unit.apnea_time  = ble_rx_data.rcv_data[15] << 8;
    s_unit.apnea_time  |= ble_rx_data.rcv_data[14];
    s_unit.max_apnea_sec  = ble_rx_data.rcv_data[17] << 8;
    s_unit.max_apnea_sec  |= ble_rx_data.rcv_data[16];


    // 日時情報書き込み
    wr_adrs = ( s_unit.frame_num.write * EEP_FRAME_SIZE ) + EEP_ADRS_TOP_FRAME_DATE;
    i2c_eeprom_write( wr_adrs, (uint8_t*)&s_unit.date, EEP_DATE_SIZE);
    // いびき検知数書き込み
    wr_adrs = ( s_unit.frame_num.write * EEP_FRAME_SIZE ) + EEP_ADRS_TOP_FRAME_SNORE_DETECT_CNT;
    i2c_eeprom_write( wr_adrs, (uint8_t*)&s_unit.snore_chg_detect_cnt, EEP_SNORE_DETECT_CNT_SIZE);
    // 無呼吸検知数書き込み
    wr_adrs = ( s_unit.frame_num.write * EEP_FRAME_SIZE ) + EEP_ADRS_TOP_FRAME_APNEA_DETECT_CNT;
    i2c_eeprom_write( wr_adrs, (uint8_t*)&s_unit.apnea_chg_detect_cnt, EEP_APNEA_DETECT_CNT_SIZE);
    // いびき時間書き込み
    wr_adrs = ( s_unit.frame_num.write * EEP_FRAME_SIZE ) + EEP_ADRS_TOP_FRAME_SNORE_TIME;
    i2c_eeprom_write( wr_adrs, (uint8_t*)&s_unit.snore_time, EEP_SNORE_TIME_SIZE);
    // 無呼吸時間書き込み
    wr_adrs = ( s_unit.frame_num.write * EEP_FRAME_SIZE ) + EEP_ADRS_TOP_FRAME_APNEA_TIME;
    i2c_eeprom_write( wr_adrs, (uint8_t*)&s_unit.apnea_time, EEP_APNEA_TIME_SIZE);
    // 最高無呼吸時間書き込み
    wr_adrs = ( s_unit.frame_num.write * EEP_FRAME_SIZE ) + EEP_ADRS_TOP_FRAME_MAX_APNEA_TIME;
    i2c_eeprom_write( wr_adrs, (uint8_t*)&s_unit.max_apnea_sec, EEP_MAX_APNEA_TIME_SIZE);
}

// 未使用
static void main_ble_rcv_data_calc( void )
{
/*
	s_unit.calc.info.dat.ibiki_val[0] = ble_rx_data.rcv_data[1];
	s_unit.calc.info.dat.ibiki_val[1] = ble_rx_data.rcv_data[2];
	s_unit.calc.info.dat.ibiki_val[2] = ble_rx_data.rcv_data[3];
	s_unit.calc.info.dat.state = ble_rx_data.rcv_data[4];
	s_unit.calc.info.dat.body_direct = ble_rx_data.rcv_data[5];
	s_unit.calc.info.dat.photoref[0] = ble_rx_data.rcv_data[6];
	s_unit.calc.info.dat.photoref[1] = ble_rx_data.rcv_data[7];
	s_unit.calc.info.dat.photoref[2] = ble_rx_data.rcv_data[8];
*/	
//	memcpy(&s_unit.calc.info.byte[0], &ble_rx_data.rcv_data[1], ( BLE_CMD_LEN_DATA_CALC - BLE_CMD_ONLY_SIZE ));
//	user_main_calc_result();
}

static void main_ble_rcv_data_fin( void )
{
//	s_unit.get_mode_seq = 7;
}

static void main_ble_rcv_device_set( void )
{
    uint8_t tx[BLE_DATA_SIZE_MAX] = {0};
    uint8_t result = BLE_DATA_RESULT_OK;
    
    if( get_current_mode() != SYSTEM_MODE_IDLE_COM ){
	    result = BLE_DATA_RESULT_NG;
    }else{
	s_device.info.dat.act_mode = ble_rx_data.rcv_data[1];
	s_device.info.dat.snore_detect_level = ble_rx_data.rcv_data[2];
	s_device.info.dat.vib_power = ble_rx_data.rcv_data[3];
	s_device.info.dat.vib_max_time = ble_rx_data.rcv_data[4];
	s_device.info.dat.vib_start_time = ble_rx_data.rcv_data[5];
	
	i2c_eeprom_write( EEP_ADRS_TOP_DEVICE_SET, (uint8_t*)&s_device, EEP_DEVICE_SET_SIZE);
    }
    
    tx[0] = BLE_CMD_DEVICE_SET;
    tx[1] = result;
    main_ble_send( &tx[0], BLE_SND_LEN_DEVICE_SET );
    
    if(result == BLE_DATA_RESULT_OK)
    {
	// 動作モード設定
	act_mode = s_device.info.dat.act_mode;
	// いびき感度設定
	snore_detect_level = s_device.info.dat.snore_detect_level;
	// snore_detect_level
	vib_power = s_device.info.dat.vib_power;
	// 振動動作最大継続時間
	vib_max_time = s_device.info.dat.vib_max_time;
	// 振動開始設定時間
	vib_start_time = s_device.info.dat.vib_start_time;
    }

}

static void main_ble_rcv_vib_confirm( void )
{
	uint8_t tx[BLE_DATA_SIZE_MAX] = {0};
	uint8_t result = BLE_DATA_RESULT_OK;
	uint8_t vib_power_conf;
	
	if( get_current_mode() != SYSTEM_MODE_IDLE_COM ){
		result = BLE_DATA_RESULT_NG;
	}else{
//		vib_power_conf = ble_rx_data.rcv_data[1];
	}
	
	tx[0] = BLE_CMD_VIB_CONFIRM;
	tx[1] = result;
	main_ble_send( &tx[0], BLE_SND_LEN_VIB_CONFIRM );
	
	if(result == BLE_DATA_RESULT_OK)
	{
#		// バイブレベル初期化
//		vib_level = VIB_LEVEL_1;
//		set_vib_level(vib_level);
		
//		set_vib_confirm(set_vib_mode(vib_power_conf));
	}
}

static void main_ble_rcv_vib_stop( void )
{
	uint8_t tx[BLE_DATA_SIZE_MAX] = {0};
	uint8_t result = BLE_DATA_RESULT_OK;
	
	if( get_current_mode() != SYSTEM_MODE_IDLE_COM ){
		result = BLE_DATA_RESULT_NG;
	}
	
	tx[0] = BLE_CMD_VIB_STOP;
	tx[1] = result;
	main_ble_send( &tx[0], BLE_SND_LEN_VIB_STOP );
	
	if(result == BLE_DATA_RESULT_OK)
	{
//		vib_stop();
	}
}

static void main_ble_diag_rcv_power_off( void )
{
    uint8_t tx[BLE_DATA_SIZE_MAX] = {0};
    uint8_t result = BLE_DATA_RESULT_OK;

    if( get_current_mode() != SYSTEM_MODE_PRODUCT_CHECK ){
        result = BLE_DATA_RESULT_NG;
    }

    tx[0] = BLE_CMD_DIAG_POWER_OFF;
    tx[1] = result;
    main_ble_send( &tx[0], BLE_SND_LEN_DIAG_POWER_OFF );

    if(result == BLE_DATA_RESULT_OK)
    {
        s_product_check.seq = DIAG_SEQ_SHUTDOWN;
	tick_power_off = 0;
    }
}

static void main_ble_diag_rcv_charge( void )
{
    uint8_t tx[BLE_DATA_SIZE_MAX] = {0};
    uint8_t result = BLE_DATA_RESULT_OK;
    
    tx[0] = BLE_CMD_DIAG_CHARGE;
    tx[1] = result;
    main_ble_send( &tx[0], BLE_SND_LEN_DIAG_CHARGE );
}

static void main_ble_diag_rcv_led( void )
{
    uint8_t tx[BLE_DATA_SIZE_MAX] = {0};
    uint8_t result = BLE_DATA_RESULT_OK;

    if(ble_rx_data.rcv_data[1] == BLE_DATA_DIAG_START)
    {
	// LED点灯
	led_ctrl(true);
    }else{
	// LED消灯
	led_ctrl(false);
    }

    tx[0] = BLE_CMD_DIAG_LED;
    tx[1] = result;
    main_ble_send( &tx[0], BLE_SND_LEN_DIAG_LED );
}

static void main_ble_diag_rcv_vib( void )
{
    uint8_t tx[BLE_DATA_SIZE_MAX] = {0};
    uint8_t result = BLE_DATA_RESULT_OK;

    if(ble_rx_data.rcv_data[1] == BLE_DATA_DIAG_START)
    {
	// バイブON
    }else{
        // バイブOFF
    }

    tx[0] = BLE_CMD_DIAG_VIB;
    tx[1] = result;
    main_ble_send( &tx[0], BLE_SND_LEN_DIAG_VIB );
}

static void main_ble_diag_rcv_mic( void )
{
    uint8_t tx[BLE_DATA_SIZE_MAX] = {0};
    uint8_t result = BLE_DATA_RESULT_OK;

    if(ble_rx_data.rcv_data[1] == BLE_DATA_DIAG_START)
    {
        s_product_check.seq = DIAG_SEQ_MIC_START;
    }else if(ble_rx_data.rcv_data[1] == BLE_DATA_DIAG_END){
        s_product_check.seq = DIAG_SEQ_MIC_END;
        diagScanDataSendCnt = 0;
    }

    tx[0] = BLE_CMD_DIAG_MIC;
    tx[1] = result;
    main_ble_send( &tx[0], BLE_SND_LEN_DIAG_MIC );
}

static void main_ble_diag_rcv_acl( void )
{
    uint8_t tx[BLE_DATA_SIZE_MAX] = {0};
    uint8_t result = BLE_DATA_RESULT_OK;

    if(ble_rx_data.rcv_data[1] == BLE_DATA_DIAG_START)
    {
        s_product_check.seq = DIAG_SEQ_ACL_START;
    }else if(ble_rx_data.rcv_data[1] == BLE_DATA_DIAG_END){
        s_product_check.seq = DIAG_SEQ_ACL_END;
        diagScanDataSendCnt = 0;
    }

    tx[0] = BLE_CMD_DIAG_ACL;
    tx[1] = result;
    main_ble_send( &tx[0], BLE_SND_LEN_DIAG_ACL );
}

static void main_ble_diag_rcv_photo( void )
{
    uint8_t tx[BLE_DATA_SIZE_MAX] = {0};
    uint8_t result = BLE_DATA_RESULT_OK;

    if(ble_rx_data.rcv_data[1] == BLE_DATA_DIAG_START)
    {
	s_product_check.seq = DIAG_SEQ_PHOTO_START;
    }else if(ble_rx_data.rcv_data[1] == BLE_DATA_DIAG_END){
        s_product_check.seq = DIAG_SEQ_PHOTO_END;
        diagScanDataSendCnt = 0;
    }

    tx[0] = BLE_CMD_DIAG_PHOTO;
    tx[1] = result;
    main_ble_send( &tx[0], BLE_SND_LEN_DIAG_PHOTO );
}

static void main_ble_diag_rcv_eep( void )
{
    s_product_check.seq = DIAG_SEQ_EEP_START;
}

static void set_ble_rcv_data(uint8_t *p_data, uint8_t len )
{
    ble_rx_data.rcv_len = len;
    memcpy( &ble_rx_data.rcv_data[0], &p_data[0], ble_rx_data.rcv_len );
}

void ds_set_ble_send_status( uint8_t status )
{
//	ble_rx_data.send_status = status;
}

uint8_t get_act_mode(void)
{
    return act_mode;
}

uint8_t get_snore_detect_level(void)
{
    return snore_detect_level;
}

uint8_t get_vib_power(void)
{
    return vib_power;
}

uint8_t get_vib_max_time(void)
{
    return vib_max_time;
}

uint8_t get_vib_start_time(void)
{
    return vib_start_time;
}

void data_get(void)
{
    uint8_t tx[BLE_DATA_SIZE_MAX];
    CALC calc_eep;				// 演算後データ
    uint32_t rd_adrs;
/*	
	if(s_unit.get_mode_timeout > GET_MODE_TIME_OUT)
	{
		// 5秒間送信中状態ならタイムアウト
		evt_act( EVENT_TIME_OUT );
		return;
	}
	
	// 送信中の場合はウェイト
	if( ON == s_ds.vuart.input.send_status ){
		// タイムアウト
		s_unit.get_mode_timeout++;
		return;
	} else {
		s_unit.get_mode_timeout = 0;
	}
*/	
    switch(s_unit.get_mode_seq){
	case GET_MODE_START:
	    data_get_before();
	    break;
	case GET_MODE_FRAME_DATA_READ:
        	// 日時読み出し
            rd_adrs = ( s_unit.frame_num_work.read * EEP_FRAME_SIZE ) + EEP_ADRS_TOP_FRAME_DATE;
            i2c_eeprom_read( rd_adrs, (uint8_t*)&s_unit.date, EEP_DATE_SIZE );
            // いびき検知数読み出し
            rd_adrs = ( s_unit.frame_num_work.read * EEP_FRAME_SIZE ) + EEP_ADRS_TOP_FRAME_SNORE_DETECT_CNT;
            i2c_eeprom_read( rd_adrs, (uint8_t*)&s_unit.snore_chg_detect_cnt, EEP_SNORE_DETECT_CNT_SIZE );
            // 無呼吸検知数読み出し
            rd_adrs = ( s_unit.frame_num_work.read * EEP_FRAME_SIZE ) + EEP_ADRS_TOP_FRAME_APNEA_DETECT_CNT;
            i2c_eeprom_read( rd_adrs, (uint8_t*)&s_unit.apnea_chg_detect_cnt, EEP_APNEA_DETECT_CNT_SIZE );
            // いびき時間読み出し
            rd_adrs = ( s_unit.frame_num_work.read * EEP_FRAME_SIZE ) + EEP_ADRS_TOP_FRAME_SNORE_TIME;
            i2c_eeprom_read( rd_adrs, (uint8_t*)&s_unit.snore_time, EEP_SNORE_TIME_SIZE );
            // 無呼吸時間読み出し
            rd_adrs = ( s_unit.frame_num_work.read * EEP_FRAME_SIZE ) + EEP_ADRS_TOP_FRAME_APNEA_TIME;
            i2c_eeprom_read( rd_adrs, (uint8_t*)&s_unit.apnea_time, EEP_APNEA_TIME_SIZE );
            // 最高無呼吸時間読み出し
            rd_adrs = ( s_unit.frame_num_work.read * EEP_FRAME_SIZE ) + EEP_ADRS_TOP_FRAME_MAX_APNEA_TIME;
            i2c_eeprom_read( rd_adrs, (uint8_t*)&s_unit.max_apnea_sec, EEP_MAX_APNEA_TIME_SIZE );
            // デバイス設定情報読み出し
            rd_adrs = ( s_unit.frame_num_work.read * EEP_FRAME_SIZE ) + EEP_ADRS_TOP_FRAME_DEVICE_SET_INFO;
            i2c_eeprom_read( rd_adrs, &s_unit.device_set_info, EEP_DEVICE_SET_INFO_SIZE );
            // 抑制開始設定時間読み出し
            rd_adrs = ( s_unit.frame_num_work.read * EEP_FRAME_SIZE ) + EEP_ADRS_TOP_FRAME_DEVICE_SET_VIB_START_TIME;
            i2c_eeprom_read( rd_adrs, &s_device.info.dat.vib_start_time, EEP_DEVICE_SET_VIB_START_TIME_SIZE );

            s_unit.get_mode_seq = GET_MODE_FRAME_DATA_SEND;
	    break;
	case GET_MODE_FRAME_DATA_SEND:
            tx[0] = BLE_CMD_DATA_FRAME;		//枠情報(日時他)
            tx[1] = s_unit.date.year;	
            tx[2] = s_unit.date.month;	
            tx[3] = s_unit.date.week;	
            tx[4] = s_unit.date.day;	
            tx[5] = s_unit.date.hour;	
            tx[6] = s_unit.date.min;	
            tx[7] = s_unit.date.sec;
            tx[8] =  ( s_unit.snore_chg_detect_cnt & 0x00ff );
            tx[9] = (( s_unit.snore_chg_detect_cnt & 0xff00 ) >> 8 );
            tx[10] =  ( s_unit.apnea_chg_detect_cnt & 0x00ff );
            tx[11] = (( s_unit.apnea_chg_detect_cnt & 0xff00 ) >> 8 );
            tx[12] =  ( s_unit.snore_time & 0x00ff );
            tx[13] = (( s_unit.snore_time & 0xff00 ) >> 8 );
            tx[14] =  ( s_unit.apnea_time & 0x00ff );
            tx[15] = (( s_unit.apnea_time & 0xff00 ) >> 8 );
            tx[16] =  ( s_unit.max_apnea_sec & 0x00ff );
            tx[17] = (( s_unit.max_apnea_sec & 0xff00 ) >> 8 );
            tx[18] = s_unit.device_set_info;
            tx[19] = s_device.info.dat.vib_start_time;

            main_ble_send( &tx[0], BLE_SND_LEN_DATA_FRAME );
            s_unit.get_mode_seq = GET_MODE_DETAIL_DATA_SEND;
	    break;
	case GET_MODE_DETAIL_DATA_SEND:
	    if( s_unit.calc_cnt <= s_unit.get_mode_calc_cnt ){
                s_unit.get_mode_seq = GET_MODE_NEXT_DATA_CHECK;
	    }else{
                // EEP読み出し
                // フレーム位置とデータ位置からEEPアドレスを算出
                rd_adrs = ( s_unit.frame_num_work.read * EEP_FRAME_SIZE ) + ( s_unit.get_mode_calc_cnt * EEP_CALC_DATA_SIZE );

                i2c_eeprom_read( rd_adrs, (uint8_t*)&calc_eep, EEP_CALC_DATA_SIZE );

                // BLE送信
                // スマホのIFに合わせる
                tx[0] = BLE_CMD_DATA_CALC;	// 演算データ
                tx[1] = calc_eep.info.dat.snore_val[0];
                tx[2] = calc_eep.info.dat.snore_val[1];
                tx[3] = calc_eep.info.dat.snore_val[2];
                tx[4] = calc_eep.info.dat.state;
                tx[5] = calc_eep.info.dat.body_direct;
                tx[6] = calc_eep.info.dat.photoref[0];
                tx[7] = calc_eep.info.dat.photoref[1];
                tx[8] = calc_eep.info.dat.photoref[2];
                main_ble_send( &tx[0], BLE_SND_LEN_DATA_CALC );
                s_unit.get_mode_calc_cnt++;
	    }
	    break;
	case GET_MODE_NEXT_DATA_CHECK:
            //読み出し枠番号進める
            INC_MAX_INI(s_unit.frame_num_work.read, ( EEP_FRAME_MAX - 1), 0);
            if( s_unit.frame_num_work.write == s_unit.frame_num_work.read ){
                // 終了
                s_unit.get_mode_seq = GET_MODE_DATA_END;
            }else{
                //継続
                tx[0] = BLE_CMD_DATA_NEXT;		// NEXT
                main_ble_send( &tx[0], BLE_SND_LEN_DATA_NEXT );
                data_get_frame_before();
            }
	    break;
	case GET_MODE_DATA_END:
            tx[0] = BLE_CMD_DATA_END;				// END
            main_ble_send( &tx[0], BLE_SND_LEN_DATA_END );
	    s_unit.get_mode_seq = GET_MODE_RES_WAIT;
            // タイムアウトタイマー初期化
//            s_unit.data_end_timeout = 0;
	    break;
	case GET_MODE_RES_WAIT:
	    data_get_after();
	    break;
/*
		s_unit.data_end_timeout++;
		if(s_unit.data_end_timeout >= DATA_END_TIME_OUT)
		{
			// タイムアウト
			evt_act( EVENT_TIME_OUT );
		}
	}else{
		user_main_mode_get_after();
	}
*/
	default:
	    break;
    }
}

static void data_get_before( void )
{
    uint8_t tx[BLE_DATA_SIZE_MAX];

    s_unit.frame_num_work = s_unit.frame_num;	//中断を考慮しワークにコピーしワークで実行する

    if(( s_unit.frame_num_work.write == s_unit.frame_num_work.read ) && 
       ( s_unit.frame_num_work.cnt == 0 )){
        //データなし時は完了
        tx[0] = BLE_CMD_DATA_END;		// END
        main_ble_send( &tx[0], BLE_SND_LEN_DATA_END );
        s_unit.get_mode_seq = GET_MODE_RES_WAIT;
    }

    data_get_frame_before();
}

static uint8_t data_get_frame_before( void )
{
    uint8_t ret = ON;
    uint16_t calc_cnt;
    uint32_t rd_adrs = 0;

    // データ初期化
    s_unit.get_mode_calc_cnt = 0;

    // フレーム位置とデータ位置からEEPアドレスを算出
    rd_adrs = ( s_unit.frame_num_work.read * EEP_FRAME_SIZE ) + EEP_ADRS_TOP_FRAME_CALC_CNT;

    i2c_eeprom_read( rd_adrs, (uint8_t*)&calc_cnt, 2 );

    if( calc_cnt > EEP_CALC_DATA_NUM ){
//        err_info(ERR_ID_MAIN);
        calc_cnt = 0;
    }

    s_unit.calc_cnt = calc_cnt;
    s_unit.get_mode_seq = GET_MODE_FRAME_DATA_READ;

    return ret;
}

static void data_get_after( void )
{
    uint32_t wr_adrs = 0;

    // ワークから正規領域へコピー
    s_unit.frame_num = s_unit.frame_num_work;

    wr_adrs = EEP_ADRS_TOP_SETTING;
    i2c_eeprom_write( wr_adrs, &s_unit.frame_num.read, 1, ON );
    s_unit.frame_num.cnt = 0;
    i2c_eeprom_write(( wr_adrs + 2 ), &s_unit.frame_num.cnt, 1, ON );

    // システム戻す
    evt_act( EVENT_COMPLETE );
}

void product_check(void)
{
    uint8_t	tx[BLE_DATA_SIZE_MAX] = {0};
    uint8_t	read_eep[EEP_ACCESS_ONCE_SIZE];
    uint8_t	diag_acl_data[10];
    uint16_t	diag_breath_val;
    uint16_t	diag_snore_val;
    uint16_t	diag_photoref_val;
    
    if( DIAG_SEQ_MIC_START == s_product_check.seq ){
	if(diagScanDataSendCnt <= BLE_DATA_DIAG_SEND_CNT)
	{
	    // 呼吸音取得・送信
	    diag_breath_val = get_saadc_value(SAADC_CH_BREATH);
	    tx[0] = BLE_CMD_DIAG_MIC_VAL;
	    // 上位8bit送信（受信側でビットシフトしなおす）
	    tx[1] = (uint8_t)(( diag_breath_val >> 2 ) & 0xff );
//	    if( OFF == s_ds.vuart.input.send_status ){
		main_ble_send( &tx[0], BLE_SND_LEN_DIAG_MIC_VAL );
		diagScanDataSendCnt++;
//	    }
	}
    }else if( DIAG_SEQ_ACL_START == s_product_check.seq ){
	if(diagScanDataSendCnt <= BLE_DATA_DIAG_SEND_CNT)
	{
	    main_acl_read();
	    
	    // 加速度データ送信
	    tx[0] = BLE_CMD_DIAG_ACL_VAL;
	    // X
	    tx[1] = ( s_calc_acl.acl_x & 0x00ff );
	    tx[2] = (( s_calc_acl.acl_x & 0xff00 ) >> 8 );
	    // Y
	    tx[3] = ( s_calc_acl.acl_y & 0x00ff );
	    tx[4] = (( s_calc_acl.acl_y & 0xff00 ) >> 8 );
	    // Z
	    tx[5] = ( s_calc_acl.acl_z & 0x00ff );
	    tx[6] = (( s_calc_acl.acl_z & 0xff00 ) >> 8 );
	    
//	    if( OFF == s_ds.vuart.input.send_status ){
		main_ble_send( &tx[0], BLE_SND_LEN_DIAG_ACL_VAL );
		diagScanDataSendCnt++;
//	    }
	}
    }else if( DIAG_SEQ_PHOTO_START == s_product_check.seq ){
	if(diagScanDataSendCnt <= BLE_DATA_DIAG_SEND_CNT)
	{
	    // 装着センサー値取得・送信
//	    diag_photoref_val = main_photo_read();
	    tx[0] = BLE_CMD_DIAG_PHOTO_VAL;
	    // 上位8bit送信（受信側でビットシフトしなおす）
	    tx[1] = (uint8_t)(( diag_photoref_val >> 2 ) & 0xff );
	    
//	    if( OFF == s_ds.vuart.input.send_status ){
		main_ble_send( &tx[0], BLE_SND_LEN_DIAG_PHOTO_VAL );
		diagScanDataSendCnt++;
//	    }
	}
    }else if( DIAG_SEQ_EEP_START == s_product_check.seq )
    {
	if( s_product_check.eep_cnt < EEP_PAGE_CNT_MAX )
	{
	    i2c_eeprom_write( s_product_check.eep_cnt * EEP_ACCESS_ONCE_SIZE, (uint8_t*)&s_eep_page0_tbl, EEP_ACCESS_ONCE_SIZE, ON );
	    INC_MAX( s_product_check.eep_cnt, EEP_PAGE_CNT_MAX );
	}
	if( s_product_check.eep_cnt == EEP_PAGE_CNT_MAX )
	{
	    s_product_check.seq = DIAG_SEQ_EEP_END;
	    s_product_check.eep_cnt = 0;
	}
    }else if( DIAG_SEQ_EEP_END == s_product_check.seq )
    {
	i2c_eeprom_read( s_product_check.eep_cnt * EEP_ACCESS_ONCE_SIZE, &read_eep[0], EEP_ACCESS_ONCE_SIZE );
	INC_MAX( s_product_check.eep_cnt, EEP_PAGE_CNT_MAX );
	if( 0 != memcmp( &s_eep_page0_tbl[0], &read_eep[0], EEP_ACCESS_ONCE_SIZE ) ){
	    tx[0] = BLE_CMD_DIAG_EEPROM;
	    tx[1] = BLE_DATA_RESULT_NG;
	    main_ble_send( &tx[0], BLE_SND_LEN_DIAG_EEPROM );
	    s_product_check.seq = DIAG_SEQ_SHUTDOWN;
	}
	if( s_product_check.eep_cnt == EEP_PAGE_CNT_MAX )
	{
	    tx[0] = BLE_CMD_DIAG_EEPROM;
	    tx[1] = BLE_DATA_RESULT_OK;
	    main_ble_send( &tx[0], BLE_SND_LEN_DIAG_EEPROM );
	    s_product_check.seq = DIAG_SEQ_SHUTDOWN;
	}
    }else if( DIAG_SEQ_SHUTDOWN == s_product_check.seq )
    {
	tick_power_off++;
	if(tick_power_off >= PERIOD_1SEC)
	{
	    // 電源OFF
//	    write1_sfr(P1, 4, 0);
	}
    }
    
    if( s_product_check.seq < DIAG_SEQ_EEP_START )
    {
	if(s_product_check.eep_cnt < EEP_PAGE_CNT_MAX)
	{
	    i2c_eeprom_write( s_product_check.eep_cnt * EEP_ACCESS_ONCE_SIZE, (uint8_t*)&s_eep_page0_tbl, EEP_ACCESS_ONCE_SIZE, ON );
	    INC_MAX( s_product_check.eep_cnt, EEP_PAGE_CNT_MAX );
	}
    }
}