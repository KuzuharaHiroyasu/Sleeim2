#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "calc_data.h"
#include "state_manager.h"
#include "time.h"

T_UNIT s_unit = {0};
T_UNIT_SAVE s_unit_save = {0};
DEVICE_SET s_device = {0};

//static void user_main_calc_data_set_kyokyu_ibiki( void );

static void user_main_mode_common( void );
static void main_mode_chg( void );

static void user_main_mode_inital(void);
static void user_main_mode_idle_com(void);
static void user_main_mode_sensing(void);
static void user_main_mode_get(void);
static void user_main_mode_product_check(void);
static void user_main_mode_move(void);
static void dummy( void );

static void main_chg_system_mode( SYSTEM_MODE next_mode );

static SYSTEM_MODE evt_non( int evt);
static SYSTEM_MODE evt_idle_com( int evt);
static SYSTEM_MODE evt_sensing_chg( int evt);
static SYSTEM_MODE evt_get( int evt);
static SYSTEM_MODE evt_product_check( int evt);
static SYSTEM_MODE evt_remove( int evt);
static SYSTEM_MODE evt_time_out( int evt);
static SYSTEM_MODE evt_demo_vib( int evt);

static void user_main_mode_sensing_before( void );
static void user_main_mode_sensing_after(void);

#include "state_manager_tbl.h"

SYSTEM_MODE current_system_mode;	/* ���̃V�X�e�����[�h */
SYSTEM_MODE next_system_mode;		/* ���̃V�X�e�����[�h */
SYSTEM_MODE last_system_mode;		/* �O�̃V�X�e�����[�h */

void user_main_mode( void )
{
	// ���[�h���ʏ���
	user_main_mode_common();
	
	// �e�탂�[�h����
	p_user_main_mode_func[current_system_mode]();
}

static void user_main_mode_common( void )
{
	uint32_t now_time;
	
//	now_time = time_get_elapsed_time();
/*	
	// �^�C���A�E�g�`�F�b�N
	if( current_system_mode != last_system_mode ){
		system_mode_time_out_cnt = now_time;
	}
	
	if( SYSTEM_MODE_IDLE_COM == current_system_mode ){
		if(( now_time - s_unit.system_mode_time_out_cnt ) >= TIME_OUT_SYSTEM_MODE_IDLE_COM ){
			evt_act( EVENT_TIME_OUT );
		}
	}
*/	
	last_system_mode = current_system_mode;
}

static void main_chg_system_mode( SYSTEM_MODE next_mode )
{
	next_system_mode = next_mode;
	current_system_mode = SYSTEM_MODE_MOVE;
	main_mode_chg();
}

static void main_mode_chg( void )
{
	if( SYSTEM_MODE_MOVE != current_system_mode){
		return;
	}
	
	current_system_mode = next_system_mode;		// ���[�h�ύX
	
	if( SYSTEM_MODE_SENSING == current_system_mode ){
		
	}
	
	if( SYSTEM_MODE_IDLE_COM == current_system_mode ){
/*	    
		if( ON == s_unit.sensing_flg ){
			s_unit.sensing_flg = OFF;
			user_main_mode_sensing_after();
			set_vib(VIB_MODE_STANDBY);
		}
*/
	}
	
	if( SYSTEM_MODE_PRODUCT_CHECK == current_system_mode ){
//		s_ds.vuart.input.send_status = OFF;
	}

	if( SYSTEM_MODE_GET == current_system_mode ){
//		s_ds.vuart.input.send_status = OFF;
//		s_unit.get_mode_seq = 0;
	}
	
	// ���[�h���̎����ɃZ�b�g
//	set_evtUsr3Timer();
}

static void user_main_mode_inital(void)
{
}

static void user_main_mode_idle_com(void)
{
}

static void user_main_mode_sensing(void)
{
}

static void user_main_mode_get(void)
{
//    wrapper_data_get();
}

static void user_main_mode_product_check( void )
{
//    wrapper_product_check();
}

static void user_main_mode_move(void)
{
}

static void dummy( void )
{
}

SYSTEM_MODE get_current_mode(void)
{
	return current_system_mode;
}

bool evt_act( EVENT_NUM evt )
{
	SYSTEM_MODE	system_mode;
	
	
	system_mode = p_event_table[evt][current_system_mode]( evt );
	if( SYSTEM_MODE_NON == system_mode ){
		return false;
	}
	
	main_chg_system_mode( system_mode );
	
	return true;
}

static SYSTEM_MODE evt_non( int evt)
{
	return SYSTEM_MODE_NON;
};

static SYSTEM_MODE evt_idle_com( int evt)
{
    SYSTEM_MODE system_mode = SYSTEM_MODE_IDLE_COM;

    if(current_system_mode == SYSTEM_MODE_SENSING)
    {
	user_main_mode_sensing_after();
        calc_data_init();
    }
    return system_mode;
}

static SYSTEM_MODE evt_sensing_chg( int evt)
{
	SYSTEM_MODE system_mode = SYSTEM_MODE_SENSING;

	user_main_mode_sensing_before();
/*
	uint8_t bat = drv_i_port_bat_chg_detect();
	
	//�d�r�c�ʊm�F
	main_set_battery();
	
	if( s_unit.battery_sts == BAT_LEVEL_STS_MIN )
	{
		// �d�r�c�ʂȂ��Ȃ�ڍs���Ȃ�
		system_mode = SYSTEM_MODE_IDLE_COM;
		set_led( LED_PATT_GREEN_BLINK_LOW_BATT );	
	}
	else if( bat == ON ){
		// �[�d���Ȃ�Z���V���O�Ɉڍs���Ȃ�
		system_mode = SYSTEM_MODE_IDLE_COM;
	} else {
		calc_data_init();
	}
*/	
	return system_mode;
}

static SYSTEM_MODE evt_get( int evt)
{
    SYSTEM_MODE system_mode = SYSTEM_MODE_GET;
    
    return system_mode;
}

static SYSTEM_MODE evt_product_check( int evt)
{
	SYSTEM_MODE system_mode = SYSTEM_MODE_PRODUCT_CHECK;
	
//	s_unit.self_check.seq = DIAG_SEQ_START;
	
	return system_mode;
}

static SYSTEM_MODE evt_remove( int evt)
{
    SYSTEM_MODE system_mode = SYSTEM_MODE_IDLE_COM;
	
    s_unit.calc_cnt		    = s_unit_save.s_calc_cnt;
    s_unit.snore_detect_cnt	    = s_unit_save.s_snore_detect_cnt;
    s_unit.apnea_detect_cnt	    = s_unit_save.s_apnea_detect_cnt;
    s_unit.snore_chg_detect_cnt	    = s_unit_save.s_snore_chg_detect_cnt;
    s_unit.apnea_chg_detect_cnt	    = s_unit_save.s_apnea_chg_detect_cnt;
    s_unit.apnea_detect_cnt_max	    = s_unit_save.s_apnea_detect_cnt_max;
    s_unit.snore_detect_cnt_decided = s_unit_save.s_snore_detect_cnt_decided;
    s_unit.apnea_detect_cnt_decided = s_unit_save.s_apnea_detect_cnt_decided;
	
    if(current_system_mode == SYSTEM_MODE_SENSING)
    {
	user_main_mode_sensing_after();
        calc_data_init();
    }
    return system_mode;
}

static SYSTEM_MODE evt_time_out( int evt)
{
	SYSTEM_MODE system_mode = SYSTEM_MODE_IDLE_COM;
//	s_ds.vuart.input.send_status = OFF;
	return system_mode;
}

static SYSTEM_MODE evt_demo_vib( int evt)
{
	SYSTEM_MODE system_mode = SYSTEM_MODE_IDLE_COM;
/*	
	if(get_confirm_flg() == true)
	{
		set_confirm_flg(false);
		vib_off();
	}else{
		demo_vib_flg = true;
		s_unit.demo_vib_cnt = 0;
	}
*/
	return system_mode;
}

static void user_main_mode_sensing_before( void )
{
    struct tm* time_struct;
    uint32_t	wr_adrs = 0;
    
    // �������
    time_struct = nrf_cal_get_time();

    s_unit.date.year  = time_struct->tm_year;
    s_unit.date.month = time_struct->tm_mon;
    s_unit.date.week  = time_struct->tm_wday;
    s_unit.date.day   = time_struct->tm_mday;
    s_unit.date.hour  = time_struct->tm_hour;
    s_unit.date.min   = time_struct->tm_min;
    s_unit.date.sec   = time_struct->tm_sec;    

    // ������񏑂�����
    wr_adrs = ( s_unit.frame_num.write * EEP_FRAME_SIZE ) + EEP_ADRS_TOP_FRAME_DATE;
    i2c_eeprom_write( wr_adrs, (uint8_t*)&s_unit.date, EEP_DATE_SIZE);

    // �f�o�C�X�ݒ�
    s_unit.device_set_info = 0;
    // ���샂�[�h�ۑ�
    s_unit.device_set_info = s_device.info.dat.act_mode;
    // �o�C�u���[�V�����̋����ۑ�
    s_unit.device_set_info |= s_device.info.dat.vib_power << 2;
    // ���т����o���x�ۑ�
    s_unit.device_set_info |= s_device.info.dat.snore_detect_level << 4;

    // �f�o�C�X�ݒ菑������
    wr_adrs = ( s_unit.frame_num.write * EEP_FRAME_SIZE ) + EEP_ADRS_TOP_FRAME_DEVICE_SET_INFO;
    i2c_eeprom_write( wr_adrs, (uint8_t*)&s_unit.device_set_info, EEP_DEVICE_SET_INFO_SIZE );

    // �U���J�n�ݒ莞�ԏ�������
    wr_adrs = ( s_unit.frame_num.write * EEP_FRAME_SIZE ) + EEP_DEVICE_SET_VIB_START_TIME_SIZE;
    i2c_eeprom_write( wr_adrs, &s_device.info.dat.vib_start_time, EEP_DEVICE_SET_VIB_START_TIME_SIZE );

    s_unit.calc_cnt = 0;
    s_unit.snore_detect_cnt = 0;
    s_unit.apnea_detect_cnt = 0;
    s_unit.snore_time = 0;
    s_unit.apnea_time = 0;
    s_unit.max_apnea_sec = 0;
    s_unit.apnea_detect_cnt_max = 0;
    s_unit.apnea_detect_cnt_now = 0;

    s_unit.snore_chg_detect_cnt = 0;
    s_unit.apnea_chg_detect_cnt = 0;
    s_unit.snore_detect_cnt_decided = 0;
    s_unit.apnea_detect_cnt_decided = 0;
    s_unit.vib_start_cnt = 0;
    
    // ���Z�f�[�^������
    calc_data_init();

    // �Z���V���O�ڍs���Ƀo�C�u����
//    set_vib(VIB_MODE_SENSING);

    /* BLE�𖳌���(�d�͏���ʒጸ�̈�) */

    // BLE��LED������(�b��)���{���̓Z���V���O�ڍs��BLE�ؒf�ŏ�������
    
    // �Z���V���O�O��������
    
    // �����x�Z���T�[�^�C�}�[�X�^�[�g
    acl_timers_start();

    // ���ʕۑ��^�C�}�[�X�^�[�g
    result_timers_start();
}

static void user_main_mode_sensing_after(void)
{
    uint8_t	oikosi_flg = OFF;
    uint32_t	wr_adrs = 0;
    uint8_t	wr_data[3] = {0};

    if( 0 == s_unit.calc_cnt ){
        return;
    }

    // ���������͕ۑ����Ȃ�
    if( SENSING_CNT_MIN > s_unit.calc_cnt ){
        return;
    }
    
    acl_timers_stop();

    result_timers_stop();

    // ���Z�񐔏�������
    wr_adrs = ( s_unit.frame_num.write * EEP_FRAME_SIZE ) + EEP_ADRS_TOP_FRAME_CALC_CNT;
    i2c_eeprom_write( wr_adrs, (uint8_t*)&s_unit.calc_cnt, 2 );

    // ���т����ԁA���ċz���ԍX�V(30�b�ۑ��������ƂɊm�肵���J�E���g�����g�p����)
    s_unit.snore_time = s_unit.snore_detect_cnt_decided * SAMPLING_INTERVAL_SEC;
    s_unit.apnea_time = s_unit.apnea_detect_cnt_decided * SAMPLING_INTERVAL_SEC;
    // �ő喳�ċz���Ԃ͌p���񐔂̍ő�l����v�Z����
    s_unit.max_apnea_sec = s_unit.apnea_detect_cnt_max * SAMPLING_INTERVAL_SEC;

    // ���т����m����������
    wr_adrs = ( s_unit.frame_num.write * EEP_FRAME_SIZE ) + EEP_ADRS_TOP_FRAME_SNORE_DETECT_CNT;
    i2c_eeprom_write( wr_adrs, (uint8_t*)&s_unit.snore_chg_detect_cnt, EEP_SNORE_DETECT_CNT_SIZE );
    // ���ċz���m����������
    wr_adrs = ( s_unit.frame_num.write * EEP_FRAME_SIZE ) + EEP_ADRS_TOP_FRAME_APNEA_DETECT_CNT;
    i2c_eeprom_write( wr_adrs, (uint8_t*)&s_unit.apnea_chg_detect_cnt, EEP_APNEA_DETECT_CNT_SIZE );
    // ���т����ԏ�������
    wr_adrs = ( s_unit.frame_num.write * EEP_FRAME_SIZE ) + EEP_ADRS_TOP_FRAME_SNORE_TIME;
    i2c_eeprom_write( wr_adrs, (uint8_t*)&s_unit.snore_time, EEP_SNORE_TIME_SIZE );
    // ���ċz���ԏ�������
    wr_adrs = ( s_unit.frame_num.write * EEP_FRAME_SIZE ) + EEP_ADRS_TOP_FRAME_APNEA_TIME;
    i2c_eeprom_write( wr_adrs, (uint8_t*)&s_unit.apnea_time, EEP_APNEA_TIME_SIZE );
    // �ō����ċz���ԏ�������
    wr_adrs = ( s_unit.frame_num.write * EEP_FRAME_SIZE ) + EEP_ADRS_TOP_FRAME_MAX_APNEA_TIME;
    i2c_eeprom_write( wr_adrs, (uint8_t*)&s_unit.max_apnea_sec, EEP_MAX_APNEA_TIME_SIZE, ON );

    // �ǂ��z������
    if(( s_unit.frame_num.write == s_unit.frame_num.read ) && ( s_unit.frame_num.cnt > 0 ))
    {
        // �ǂ��z����Ă��܂��̂œǂݏo���|�C���^��i�߂�
        oikosi_flg = ON;
    }

    // �������ݘg�ԍ���i�߂�
    INC_MAX_INI(s_unit.frame_num.write, (EEP_FRAME_MAX - 1), 0);
    INC_MAX(s_unit.frame_num.cnt, EEP_FRAME_MAX );


    wr_adrs = EEP_ADRS_TOP_SETTING;
    if( OFF == oikosi_flg )
    {
        // �������݃|�C���^
        wr_data[0] = s_unit.frame_num.write;
        wr_data[1] = s_unit.frame_num.cnt;
        i2c_eeprom_write( wr_adrs + 1, &wr_data[0], 2 );
    }else{
        INC_MAX_INI(s_unit.frame_num.read, (EEP_FRAME_MAX  -1), 0);
        // �������݁A�ǂݏo���|�C���^
        wr_data[0] = s_unit.frame_num.read;
        wr_data[1] = s_unit.frame_num.write;
        wr_data[2] = s_unit.frame_num.cnt;
        i2c_eeprom_write( wr_adrs, &wr_data[0], 3 );
    }
}