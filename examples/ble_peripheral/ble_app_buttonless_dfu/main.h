#ifndef	__MAIN__
#define	__MAIN__

// タイマー周期（ｍｓ）
#define TIMER_MIC_GET_PERIOD	    50
#define TIMER_SW_PERIOD		    10
#define TIMER_ACL_PERIOD	    10000
#define TIMER_RESULT_PERIOD	    30000

// EEPROM
#define EEP_RECORD_SIZE		    8		    // 1レコードサイズ ※256の約数とするページ跨ぎを行いたくない為
#define EEP_ADRS_SIZE		    2		    // アドレスサイズ
#define EEP_DATA_SIZE_ALL	    131072	    // EEP全サイズ
#define EEP_DATA_SIZE_HALF	    ( 131072 / 2 )  // EEP半分サイズ
#define EEP_ACCESS_ONCE_SIZE	    256		    // アクセスサイズ１回
#define EEP_PAGE_CNT_MAX	    ( EEP_DATA_SIZE_ALL / EEP_ACCESS_ONCE_SIZE )		//ページ数最大

// データ数
#define EEP_CALC_DATA_NUM	    1440	    // 12時間分(1データ/30秒)
#define EEP_FRAME_MAX		    7		    // 1週間分

#define EEP_CALC_DATA_SIZE	    	    	    8
#define EEP_RESERVE_SIZE	    	    	    5760	    // 保存データ1に対して4バイト空きを設ける（4バイト × 1440個 = 5760バイト)
#define EEP_FRAM_ADD_SIZE	    	    	    128		    // フレーム用付加情報(時間,いびき検知数～最高無呼吸,演算回数)
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


// 先頭アドレス
#define EEP_ADRS_TOP_FRAME				    0													// フレームの先頭
#define EEP_ADRS_TOP_FRAME_DATE				    (uint32_t)( ( EEP_CALC_DATA_SIZE * 1440 ) + EEP_RESERVE_SIZE + 0 )
#define EEP_ADRS_TOP_FRAME_SNORE_DETECT_CNT		    (uint32_t)( ( EEP_CALC_DATA_SIZE * 1440 ) + EEP_RESERVE_SIZE + 8 )
#define EEP_ADRS_TOP_FRAME_APNEA_DETECT_CNT		    (uint32_t)( ( EEP_CALC_DATA_SIZE * 1440 ) + EEP_RESERVE_SIZE + 10 )
#define EEP_ADRS_TOP_FRAME_SNORE_TIME			    (uint32_t)( ( EEP_CALC_DATA_SIZE * 1440 ) + EEP_RESERVE_SIZE + 12 )
#define EEP_ADRS_TOP_FRAME_APNEA_TIME			    (uint32_t)( ( EEP_CALC_DATA_SIZE * 1440 ) + EEP_RESERVE_SIZE + 14 )
#define EEP_ADRS_TOP_FRAME_MAX_APNEA_TIME		    (uint32_t)( ( EEP_CALC_DATA_SIZE * 1440 ) + EEP_RESERVE_SIZE + 16 )
#define EEP_ADRS_TOP_FRAME_DEVICE_SET_INFO		    (uint32_t)( ( EEP_CALC_DATA_SIZE * 1440 ) + EEP_RESERVE_SIZE + 18 )
#define EEP_ADRS_TOP_FRAME_DEVICE_SET_VIB_START_TIME	    (uint32_t)( ( EEP_CALC_DATA_SIZE * 1440 ) + EEP_RESERVE_SIZE + 19 )
#define EEP_ADRS_TOP_FRAME_CALC_CNT			    (uint32_t)( ( EEP_CALC_DATA_SIZE * 1440 ) + EEP_RESERVE_SIZE + 20 )

#define EEP_ADRS_TOP_SETTING				    (uint32_t)( EEP_FRAME_SIZE * EEP_FRAME_MAX )				// 設定
#define EEP_ADRS_TOP_DEVICE_SET				    (uint32_t)( EEP_ADRS_TOP_SETTING + EEP_SETTING_SIZE )		// 警告機能
#define EEP_ADRS_DATA_TYPE				    (uint32_t)( EEP_DATA_SIZE_ALL - 1 )						// EEP種別(最終アドレス) ※通常時(0x00),プログラム転送(0xAA)

#define ON			    1
#define	OFF			    0

#define TIME_600MS_CNT_POW_OFF_STEP1_TIMEOUT    60
#define	TIME_250MS_CNT_POW_OFF_SW_LONG		25
#define TIME_20MS_CNT_POW_SW_SHORT		2
#define TIME_2000MS_CNT_POW_SW_LONG		200

typedef enum{
    SEC_PHASE_0_10 = 0,					// 0～10秒まで
    SEC_PHASE_10_20,					// 10～20秒まで
    SEC_PHASE_20_30,					// 20～30秒まで
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