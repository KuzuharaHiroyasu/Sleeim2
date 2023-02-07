#ifndef	__MAIN__
#define	__MAIN__

// �^�C�}�[�����i�����j
#define TIMER_MIC_GET_PERIOD	    50
#define TIMER_SW_PERIOD		    10
#define TIMER_ACL_PERIOD	    10000
#define TIMER_RESULT_PERIOD	    30000

// EEPROM
#define EEP_RECORD_SIZE		    8		    // 1���R�[�h�T�C�Y ��256�̖񐔂Ƃ���y�[�W�ׂ����s�������Ȃ���
#define EEP_ADRS_SIZE		    2		    // �A�h���X�T�C�Y
#define EEP_DATA_SIZE_ALL	    131072	    // EEP�S�T�C�Y
#define EEP_DATA_SIZE_HALF	    ( 131072 / 2 )  // EEP�����T�C�Y
#define EEP_ACCESS_ONCE_SIZE	    256		    // �A�N�Z�X�T�C�Y�P��
#define EEP_PAGE_CNT_MAX	    ( EEP_DATA_SIZE_ALL / EEP_ACCESS_ONCE_SIZE )		//�y�[�W���ő�

// �f�[�^��
#define EEP_CALC_DATA_NUM	    1440	    // 12���ԕ�(1�f�[�^/30�b)
#define EEP_FRAME_MAX		    7		    // 1�T�ԕ�

#define EEP_CALC_DATA_SIZE	    	    	    8
#define EEP_RESERVE_SIZE	    	    	    5760	    // �ۑ��f�[�^1�ɑ΂���4�o�C�g�󂫂�݂���i4�o�C�g �~ 1440�� = 5760�o�C�g)
#define EEP_FRAM_ADD_SIZE	    	    	    128		    // �t���[���p�t�����(����,���т����m���`�ō����ċz,���Z��)
#define EEP_FRAME_SIZE		    	    	    (uint32_t)(( EEP_CALC_DATA_SIZE * 1440 ) + EEP_RESERVE_SIZE + EEP_FRAM_ADD_SIZE )
#define EEP_SETTING_SIZE	    	    	    3
#define EEP_DEVICE_SET_SIZE	    	    	    5
#define EEP_DATE_SIZE		    	    	    7
#define EEP_SNORE_DETECT_CNT_SIZE   	    	    2
#define EEP_APNEA_DETECT_CNT_SIZE 	    	    2
#define EEP_SNORE_TIME_SIZE	    	    	    2
#define EEP_APNEA_TIME_SIZE	    	    	    2
#define EEP_MAX_APNEA_TIME_SIZE   	    	    2
#define EEP_DEVICE_SET_INFO_SIZE    	    	    1
#define EEP_DEVICE_SET_VIB_START_TIME_SIZE	    1


// �擪�A�h���X
#define EEP_ADRS_TOP_FRAME				    0													// �t���[���̐擪
#define EEP_ADRS_TOP_FRAME_DATE				    (uint32_t)( ( EEP_CALC_DATA_SIZE * 1440 ) + EEP_RESERVE_SIZE + 0 )
#define EEP_ADRS_TOP_FRAME_SNORE_DETECT_CNT		    (uint32_t)( ( EEP_CALC_DATA_SIZE * 1440 ) + EEP_RESERVE_SIZE + 8 )
#define EEP_ADRS_TOP_FRAME_APNEA_DETECT_CNT		    (uint32_t)( ( EEP_CALC_DATA_SIZE * 1440 ) + EEP_RESERVE_SIZE + 10 )
#define EEP_ADRS_TOP_FRAME_SNORE_TIME			    (uint32_t)( ( EEP_CALC_DATA_SIZE * 1440 ) + EEP_RESERVE_SIZE + 12 )
#define EEP_ADRS_TOP_FRAME_APNEA_TIME			    (uint32_t)( ( EEP_CALC_DATA_SIZE * 1440 ) + EEP_RESERVE_SIZE + 14 )
#define EEP_ADRS_TOP_FRAME_MAX_APNEA_TIME		    (uint32_t)( ( EEP_CALC_DATA_SIZE * 1440 ) + EEP_RESERVE_SIZE + 16 )
#define EEP_ADRS_TOP_FRAME_DEVICE_SET_INFO		    (uint32_t)( ( EEP_CALC_DATA_SIZE * 1440 ) + EEP_RESERVE_SIZE + 18 )
#define EEP_ADRS_TOP_FRAME_DEVICE_SET_VIB_START_TIME	    (uint32_t)( ( EEP_CALC_DATA_SIZE * 1440 ) + EEP_RESERVE_SIZE + 19 )
#define EEP_ADRS_TOP_FRAME_CALC_CNT			    (uint32_t)( ( EEP_CALC_DATA_SIZE * 1440 ) + EEP_RESERVE_SIZE + 20 )

#define EEP_ADRS_TOP_SETTING				    (uint32_t)( EEP_FRAME_SIZE * EEP_FRAME_MAX )				// �ݒ�
#define EEP_ADRS_TOP_DEVICE_SET				    (uint32_t)( EEP_ADRS_TOP_SETTING + EEP_SETTING_SIZE )		// �x���@�\
#define EEP_ADRS_DATA_TYPE				    (uint32_t)( EEP_DATA_SIZE_ALL - 1 )						// EEP���(�ŏI�A�h���X) ���ʏ펞(0x00),�v���O�����]��(0xAA)

#define ON			    1
#define	OFF			    0

#define TIME_600MS_CNT_POW_OFF_STEP1_TIMEOUT    60
#define	TIME_250MS_CNT_POW_OFF_SW_LONG		25
#define TIME_20MS_CNT_POW_SW_SHORT		2
#define TIME_2000MS_CNT_POW_SW_LONG		200

typedef enum{
    SEC_PHASE_0_10 = 0,					// 0�`10�b�܂�
    SEC_PHASE_10_20,					// 10�`20�b�܂�
    SEC_PHASE_20_30,					// 20�`30�b�܂�
    SEC_PHASE_NUM,
}SEC_PHASE;

typedef struct{
    uint8_t	pow_sw_last;
    uint8_t	power_off_timer;
    uint8_t	power_off_ope_start;
    uint8_t	sw_power_off;
    uint32_t	sw_time_cnt;
}SW;
extern uint8_t write_buf[10];
extern uint16_t write_len;

extern uint8_t read_buf[10];
extern bool ble_connect_flag;
extern uint8_t received_write_data_len;
extern bool write_flag;
extern bool ble_indicate_ack;
extern uint8_t indication_packet_count;
extern bool ble_indicate_enable;

void acl_timers_start(void);
void acl_timers_stop(void);
void result_timers_start(void);
void result_timers_stop(void);

void notification_exe(uint8_t * data, uint8_t len);
void indication_exe(uint8_t * data, uint8_t len);
#endif // __MAIN__