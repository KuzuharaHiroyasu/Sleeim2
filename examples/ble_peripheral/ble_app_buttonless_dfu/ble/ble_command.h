#ifndef	__BLE_COMMAND__
#define	__BLE_COMMAND__

#include "ble_app.h"
#include "calc_data.h"

#define	BD_ADRS_NUM			6			// BLE�A�h���X��

const int8_t	version_product_tbl[]= {1, 0, 0, 0};

typedef enum{
	BLE_CMD_TYPE_NONE=0,
	BLE_CMD_TYPE_MODE_CHG,						// 状態変更(G1D)
	BLE_CMD_TYPE_DATE_SET,						// 日時設�
	BLE_CMD_TYPE_INFO,							// 惱取�
	BLE_CMD_TYPE_VERSION,						// バ�ジョン取�
	BLE_CMD_TYPE_DEVICE_INFO,					// ッ�イス状況取�
	BLE_CMD_TYPE_DATA_NEXT,						// NEXT
	BLE_CMD_TYPE_DATA_END,						// END
	BLE_CMD_TYPE_DATA_FRAME,					// �惱(日時�
	BLE_CMD_TYPE_DATA_CALC,						// 機器�タ
	BLE_CMD_TYPE_DATA_FIN,						// �タ取得完亀�知
	BLE_CMD_TYPE_DEVICE_SET,						// ッ�イス設定変更
	BLE_CMD_TYPE_VIB_CONFIRM,					// バイブ動作確�
	BLE_CMD_TYPE_VIB_STOP,						// バイブ動作停止
	BLE_CMD_TYPE_POWER_OFF,						// 電源OFF
	BLE_CMD_TYPE_MAX							// 最大値
}BLE_CMD_TYPE;

// コマン�
#define	BLE_CMD_MODE_CHG		0xB0
#define	BLE_CMD_SET_CHG			0xC0
#define	BLE_CMD_DATE_SET		0xC1
#define	BLE_CMD_INFO			0xC2
#define	BLE_CMD_DATA_NEXT		0xE0	// NEXT
#define	BLE_CMD_DATA_END		0xE1	// END
#define	BLE_CMD_DATA_FRAME		0xE2	// �
#define	BLE_CMD_DATA_CALC		0xE3	// 演算データ
#define	BLE_CMD_DATA_FIN		0xE4	// 完�
#define	BLE_CMD_PRG_RESULT		0xD1
#define	BLE_CMD_PRG_CHECK		0xD3
#define	BLE_CMD_PRG_G1D_START		0xF0
#define	BLE_CMD_PRG_G1D_VER		0xF9

#define	BLE_CMD_INFO			0xC2
#define	BLE_CMD_VERSION			0xC3
#define	BLE_CMD_DEVICE_INFO		0xC5
#define	BLE_CMD_DEVICE_SET		0xC6
#define	BLE_CMD_VIB_CONFIRM		0xC7
#define	BLE_CMD_VIB_STOP		0xC8

#define BLE_CMD_DIAG_POWER_OFF		0xAA
#define BLE_CMD_DIAG_CHARGE		0xA0
#define BLE_CMD_DIAG_LED		0xA1
#define BLE_CMD_DIAG_VIB		0xA2
#define BLE_CMD_DIAG_MIC		0xA3
#define BLE_CMD_DIAG_ACL		0xA4
#define BLE_CMD_DIAG_PHOTO		0xA5
#define BLE_CMD_DIAG_EEPROM		0xA6
#define BLE_CMD_DIAG_MIC_VAL		0xA7
#define BLE_CMD_DIAG_ACL_VAL		0xA8
#define BLE_CMD_DIAG_PHOTO_VAL		0xA9

#define	BLE_CMD_INVALID			0xFF	// コマンド無し特殊��


// 受信�タ長 ※コマンド部含む
#define	BLE_CMD_LEN_MODE_CHG		2
#define	BLE_CMD_LEN_SET_CHG		3
#define	BLE_CMD_LEN_DATE_SET		8
#define	BLE_CMD_LEN_INFO		1
#define	BLE_CMD_LEN_DATA_NEXT		1	// NEXT
#define	BLE_CMD_LEN_DATA_END		1	// END
#define	BLE_CMD_LEN_DATA_FRAME		10	// �
#define	BLE_CMD_LEN_DATA_CALC		7	// 演算データ
#define	BLE_CMD_LEN_DATA_FIN		2	// 演算データ
#define	BLE_CMD_LEN_PRG_RESULT		5
#define	BLE_CMD_LEN_PRG_DATA		20
#define	BLE_CMD_LEN_PRG_CHECK		1
#define	BLE_CMD_LEN_PRG_G1D_START	1
#define	BLE_CMD_LEN_PRG_G1D_VER		1

#define	BLE_CMD_LEN_VERSION		1
#define	BLE_CMD_LEN_DEVICE_INFO		1
#define	BLE_CMD_LEN_DEVICE_SET		6
#define	BLE_CMD_LEN_VIB_CONFIRM		2
#define	BLE_CMD_LEN_VIB_STOP		1

#define	BLE_CMD_LEN_DIAG_POWER_OFF	1
#define BLE_CMD_LEN_DIAG_CHARGE		2
#define BLE_CMD_LEN_DIAG_LED		2
#define BLE_CMD_LEN_DIAG_VIB		2
#define BLE_CMD_LEN_DIAG_MIC		2
#define BLE_CMD_LEN_DIAG_ACL		2
#define BLE_CMD_LEN_DIAG_PHOTO		2
#define BLE_CMD_LEN_DIAG_EEPROM		1

#define	BLE_CMD_ONLY_SIZE		1	// コマンド�みのサイズ

// 送信�タ長 ※コマンド部含む
#define	BLE_SND_LEN_MODE_CHG		2
#define	BLE_SND_LEN_SET_CHG		2
#define	BLE_SND_LEN_DATE_SET		2
#define	BLE_SND_LEN_INFO		3
#define	BLE_SND_LEN_DATA_NEXT		1
#define	BLE_SND_LEN_DATA_END		1
#define	BLE_SND_LEN_DATA_FRAME		20
#define	BLE_SND_LEN_DATA_CALC		9

#define	BLE_SND_LEN_PRG_G1D_VER		4

#define	BLE_SND_LEN_VERSION		6
#define	BLE_SND_LEN_DEVICE_INFO		16
#define	BLE_SND_LEN_DEVICE_SET		2
#define	BLE_SND_LEN_VIB_CONFIRM		2
#define	BLE_SND_LEN_VIB_STOP		2

#define BLE_SND_LEN_DIAG_POWER_OFF	2
#define BLE_SND_LEN_DIAG_CHARGE		2
#define BLE_SND_LEN_DIAG_LED		2
#define BLE_SND_LEN_DIAG_VIB		2
#define BLE_SND_LEN_DIAG_MIC		2
#define BLE_SND_LEN_DIAG_ACL		2
#define BLE_SND_LEN_DIAG_PHOTO		2
#define BLE_SND_LEN_DIAG_EEPROM		2
#define BLE_SND_LEN_DIAG_MIC_VAL	2
#define BLE_SND_LEN_DIAG_ACL_VAL	7
#define BLE_SND_LEN_DIAG_PHOTO_VAL	2


#define BLE_DATA_SIZE_MAX		20

#define BLE_DATA_RESULT_OK		0
#define BLE_DATA_RESULT_NG		1

#define BLE_DATA_DIAG_START		0
#define BLE_DATA_DIAG_END		1
#define BLE_DATA_DIAG_SEND_CNT		200

#define PERIOD_1SEC			100

typedef struct{
	uint8_t cmd;						/* 受信コマン�*/
	uint8_t len;						/* 受信コマン�*/
	void (*func)(void);				/* 受信処�*/
}BLE_RCV_CMD_TBL;

typedef struct BLE_INPUT{
	uint8_t rcv_data[BLE_DATA_SIZE_MAX];					/* 受信�タ */
	uint8_t rcv_len;											/* 受信長 */
	uint8_t send_status;										/* 送信状�ON:送信中、OFF:送信な� */
}BLE_INPUT;

/*
typedef struct _DS_BLE{
	BLE_INPUT input;
}DS_BLE;


typedef struct{
	DS_BLE		vuart;
}DS;
*/
void main_set_bd_adrs(void);
void main_ble_proc(uint8_t *p_data, uint8_t len );
void main_ble_send( uint8_t *p_data, uint8_t len );
uint8_t get_act_modeget_act_mode(void);
uint8_t get_snore_detect_sens(void);
uint8_t get_vib_power(void);
uint8_t get_vib_max_time(void);
uint8_t get_vib_start_time(void);

void data_get(void);
void product_check(void);
#endif //__BLE_COMMAND__