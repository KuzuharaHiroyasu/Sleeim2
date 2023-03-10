#ifndef	__BLE_COMMAND__
#define	__BLE_COMMAND__

#include "ble_app.h"
#include "calc_data.h"

#define	BD_ADRS_NUM			6			// BLEAhX

const int8_t	version_product_tbl[]= {1, 0, 0, 0};

typedef enum{
	BLE_CMD_TYPE_NONE=0,
	BLE_CMD_TYPE_MODE_CHG,						// ç¶æå¤æ´(G1D)
	BLE_CMD_TYPE_DATE_SET,						// æ¥æè¨­å®
	BLE_CMD_TYPE_INFO,							// æ±åå¾
	BLE_CMD_TYPE_VERSION,						// ããã¸ã§ã³åå¾
	BLE_CMD_TYPE_DEVICE_INFO,					// ãã¤ã¹ç¶æ³åå¾
	BLE_CMD_TYPE_DATA_NEXT,						// NEXT
	BLE_CMD_TYPE_DATA_END,						// END
	BLE_CMD_TYPE_DATA_FRAME,					// ææ±(æ¥æç­
	BLE_CMD_TYPE_DATA_CALC,						// æ©å¨ãã¿
	BLE_CMD_TYPE_DATA_FIN,						// ãã¿åå¾å®äºç¥
	BLE_CMD_TYPE_DEVICE_SET,						// ãã¤ã¹è¨­å®å¤æ´
	BLE_CMD_TYPE_VIB_CONFIRM,					// ãã¤ãåä½ç¢ºèª
	BLE_CMD_TYPE_VIB_STOP,						// ãã¤ãåä½åæ­¢
	BLE_CMD_TYPE_POWER_OFF,						// é»æºOFF
	BLE_CMD_TYPE_MAX							// æå¤§å¤
}BLE_CMD_TYPE;

// ã³ãã³ã
#define	BLE_CMD_MODE_CHG		0xB0
#define	BLE_CMD_SET_CHG			0xC0
#define	BLE_CMD_DATE_SET		0xC1
#define	BLE_CMD_INFO			0xC2
#define	BLE_CMD_DATA_NEXT		0xE0	// NEXT
#define	BLE_CMD_DATA_END		0xE1	// END
#define	BLE_CMD_DATA_FRAME		0xE2	// æ
#define	BLE_CMD_DATA_CALC		0xE3	// æ¼ç®ãã¼ã¿
#define	BLE_CMD_DATA_FIN		0xE4	// å®äº
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

#define	BLE_CMD_INVALID			0xFF	// ã³ãã³ãç¡ãç¹æ®åç


// åä¿¡ãã¿é· â»ã³ãã³ãé¨å«ã
#define	BLE_CMD_LEN_MODE_CHG		2
#define	BLE_CMD_LEN_SET_CHG		3
#define	BLE_CMD_LEN_DATE_SET		8
#define	BLE_CMD_LEN_INFO		1
#define	BLE_CMD_LEN_DATA_NEXT		1	// NEXT
#define	BLE_CMD_LEN_DATA_END		1	// END
#define	BLE_CMD_LEN_DATA_FRAME		10	// æ
#define	BLE_CMD_LEN_DATA_CALC		7	// æ¼ç®ãã¼ã¿
#define	BLE_CMD_LEN_DATA_FIN		2	// æ¼ç®ãã¼ã¿
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

#define	BLE_CMD_ONLY_SIZE		1	// ã³ãã³ããã¿ã®ãµã¤ãº

// éä¿¡ãã¿é· â»ã³ãã³ãé¨å«ã
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
	uint8_t cmd;						/* åä¿¡ã³ãã³ã*/
	uint8_t len;						/* åä¿¡ã³ãã³ã*/
	void (*func)(void);				/* åä¿¡å¦ç*/
}BLE_RCV_CMD_TBL;

typedef struct BLE_INPUT{
	uint8_t rcv_data[BLE_DATA_SIZE_MAX];					/* åä¿¡ãã¿ */
	uint8_t rcv_len;											/* åä¿¡é· */
	uint8_t send_status;										/* éä¿¡ç¶æON:éä¿¡ä¸­ãOFF:éä¿¡ãªã */
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