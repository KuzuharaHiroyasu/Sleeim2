#ifndef	__STATE_MANAGER__
#define	__STATE_MANAGER__

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "main.h"

// 電池残量状態
#define BAT_LEVEL_STS_MAX	0	// 充電満タン
#define BAT_LEVEL_STS_HIGH	1	// 数日持つ
#define BAT_LEVEL_STS_LOW	2	// 残り１日持たない
#define BAT_LEVEL_STS_MIN	3	// 電池残量なし

// 動作モード
#define ACT_MODE_SNORE		0	// 振動モード（いびき）
#define ACT_MODE_SNORE_APNEA	1	// 振動モード（いびき + 無呼吸）
#define ACT_MODE_MONITOR	2	// モニタリングモード
#define ACT_MODE_APNEA		3	// 振動モード（無呼吸）

// 振動の強さ
#define	VIB_SET_MODE_WEAK	0	// 弱
#define	VIB_SET_MODE_MID	1	// 中
#define	VIB_SET_MODE_STRONG	2	// 強
#define	VIB_SET_MODE_GURADUALLY	3	// 徐々

// いびきの検出感度
#define SNORE_DETECT_LEVEL_LOW	    0   // 450 低  
#define SNORE_DETECT_LEVEL_MID	    1   // 350 中
#define SNORE_DETECT_LEVEL_HIGH	    2   // 250 高

// 振動最大連続時間
#define SET_MAX_VIB_CONT_5_MIN		0	// 5分
#define SET_MAX_VIB_CONT_10_MIN		1	// 10分
#define SET_MAX_VIB_CONT_NON		2	// 設定しない

// 振動開始カ時間(デフォルト)
#define VIB_START_TIME			20	// センシング開始20分間は振動しない 20min

// 測定関係
#define SENSING_CNT_MIN			40	// 最小測定時間 (20分：30秒毎に加算)
#define	SAMPLING_INTERVAL_SEC		10	// 10秒間隔

typedef enum{
    // 闔牙｢難ｽｧ蛟ｽｸ鄙ｫ霑･ｶ隲ｷ蛟ｶｸ遏ｩ蜑
    SYSTEM_MODE_INITIAL = 0,				// 郢ｧ､郢昜ｹ昴☆郢晢ｽ｣郢晢ｽｫ
    SYSTEM_MODE_IDLE_COM,					// 郢ｧ｢郢ｧ､郢晏ｳｨﾎ拈鬨ｾ螢ｻｿ｡陟包ｽｩ
    SYSTEM_MODE_SENSING,					// 郢ｧｻ郢晢ｽｳ郢ｧｷ郢晢ｽｳ郢ｧｰ
    SYSTEM_MODE_GET,						// 郢晉ｹｧｿ陷ｿ髢ｾ
    SYSTEM_MODE_PRODUCT_CHECK,					// 髢ｾｪ陝ｾｱ髫ｪｺ隴ｽｭ
    SYSTEM_MODE_MOVE,						// 驕假ｽｻ髯ｦ
    SYSTEM_MODE_NON,						// 邵ｺｪ邵ｺ
    SYSTEM_MODE_MAX
}SYSTEM_MODE;

typedef enum{
    EVENT_NON = 0,				// 縺ｪ縺
    EVENT_POW_SW_SHORT,			// 髮ｻ貅心W謚ｼ荳遏ｭ)
    EVENT_POW_SW_LONG,			// 髮ｻ貅心W謚ｼ荳髟ｷ)
    EVENT_CHG_ON,			// 蜈崕讀懃衍繝昴繝N
    EVENT_BAT_LOW,			// 髮ｻ豎谿矩㍼菴惹ｸ
    EVENT_GET_DATA,				// 繝繧ｿ蜿門ｾ
    EVENT_PRODUCT_CHECK,		// 閾ｪ蟾ｱ險ｺ譁ｭ(騾壻ｿ｡)
    EVENT_COMPLETE,				// 螳御ｺ
    EVENT_STOP,					// 荳ｭ譁ｭ
    EVENT_TIME_OUT,				// 繧ｿ繧､繝繧｢繧ｦ繝
    EVENT_REMOVE_TIMEOUT,		// 蜿悶ｊ螟悶ｌ繧ｿ繧､繝繧｢繧ｦ繝
    EVENT_MAX,					// 譛螟ｧ
}EVENT_NUM;

typedef enum{
    GET_MODE_START = 0,
    GET_MODE_FRAME_DATA_READ,
    GET_MODE_FRAME_DATA_SEND,
    GET_MODE_DETAIL_DATA_SEND,
    GET_MODE_NEXT_DATA_CHECK,
    GET_MODE_DATA_END,
    GET_MODE_RES_WAIT,
    GET_MODE_FINISH,
    GET_MODE_MAX,
}GET_MODE;

typedef enum diag_seq{
    DIAG_SEQ_START=0,
    DIAG_SEQ_CHARGE,
    DIAG_SEQ_LED,
    DIAG_SEQ_VIB,
    DIAG_SEQ_MIC_START,
    DIAG_SEQ_MIC_END,
    DIAG_SEQ_ACL_START,
    DIAG_SEQ_ACL_END,
    DIAG_SEQ_PHOTO_START,
    DIAG_SEQ_PHOTO_END,
    DIAG_SEQ_EEP_START,
    DIAG_SEQ_EEP_END,
    DIAG_SEQ_SHUTDOWN
}DIAG_SEQ;

// フレーム(枠)番号
typedef struct{
    uint8_t read;		// フレーム(枠)の読み出し番号
    uint8_t write;		// フレーム(枠)の書き込み番号
    uint8_t cnt;		// フレーム(枠)の書き込み数　※書き込み数0と最大の区別がつかない為
}FRAME_NUM_INFO;

// 日時情報
typedef struct{
    uint8_t year;
    uint8_t month;
    uint8_t week;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
}DATE;

typedef struct{
    union{
	uint8_t	byte[EEP_DEVICE_SET_SIZE];
	struct{
	    uint8_t act_mode;		// 動作モード
	    uint8_t snore_detect_level; // いびき感度
	    uint8_t vib_power;		// 振動強度
	    uint8_t vib_max_time;	// 振動動作最大継続時間
	    uint8_t vib_start_time;	// 振動開始設定時間
	}dat;
    }info;
}DEVICE_SET;

typedef struct{
    DATE    date;
    uint8_t battery_sts;			// 電池状態

    uint16_t calc_cnt;		    // 演算カウント(保存データ数)
    uint16_t snore_detect_cnt;	    // いびき検知数
    uint16_t apnea_detect_cnt;	    // 無呼吸検知数
    uint16_t snore_time;	    // いびき時間
    uint16_t apnea_time;	    // 無呼吸時間
    uint16_t max_apnea_sec;	    // 最大無呼吸[秒]
    uint16_t apnea_detect_cnt_max;  // 継続無呼吸検知数(最大値保存用)
    uint16_t apnea_detect_cnt_now;  // 継続無呼吸検知数(現在値)

    uint16_t snore_chg_detect_cnt;	// いびき発生検知数(通信で送る"いびき検知数")
    uint16_t apnea_chg_detect_cnt;	// 無呼吸発生検知数(通信で送る"無呼吸検知数)
    uint16_t snore_detect_cnt_decided;	// 確定済みいびき検知数(保存周期ごとの確定したカウント数)
    uint16_t apnea_detect_cnt_decided;	// 確定済み無呼吸検知数(保存周期ごとの確定したカウント数)

    uint8_t device_set_info;		// 測定時のデバイス設定
    uint8_t vib_start_cnt;		// 振動開始カウント
    uint8_t sensing_flg;
/*
    uint8_t phase_snore;		// 秒間フェイズ(いびき)
    uint8_t phase_breath;		// 秒間フェイズ(呼吸)
    uint8_t phase_body_direct;		// 秒間フェイズ(体の向き)
    uint8_t phase_photoref;		// 秒間フェイズ(フォトセンサ)
*/
    // フレーム(枠)番号
    FRAME_NUM_INFO frame_num;			// フレーム(枠)番号[EEPコピーエリア] ※EEPとは一致させておく
    FRAME_NUM_INFO frame_num_work;		// フレーム(枠)番号ワーク

    uint8_t get_mode_seq;				// GETモードシーケンス
    uint16_t get_mode_calc_cnt;
    uint16_t set_mode_calc_cnt;
}T_UNIT;

// 取り外しタイムアウト時の保存用
typedef struct{
    uint16_t s_calc_cnt;		    // 演算カウント(保存データ数)
    uint16_t s_snore_detect_cnt;	    // いびき検知数
    uint16_t s_apnea_detect_cnt;	    // 無呼吸検知数
    uint16_t s_snore_chg_detect_cnt;	    // いびき発生検知数(通信で送る"いびき検知数")
    uint16_t s_apnea_chg_detect_cnt;	    // 無呼吸発生検知数(通信で送る"無呼吸検知数)
    uint16_t s_apnea_detect_cnt_max;	    // 継続無呼吸検知数(最大値保存用)
    uint16_t s_snore_detect_cnt_decided;    // 確定済みいびき検知数(保存周期ごとの確定したカウント数)
    uint16_t s_apnea_detect_cnt_decided;    // 確定済み無呼吸検知数(保存周期ごとの確定したカウント数)
}T_UNIT_SAVE;

typedef struct{
    uint32_t	last_time;
    uint16_t	eep_cnt;		// EEP消去回数
    DIAG_SEQ	seq;	// シーケンス
}PRODUCT_CHECK;

/* =====汎用マクロ関数===== */
/* maxまでdataをインクリメント */
#define	INC_MAX(data,max)   \
{			    \
    if( data < max ){	    \
	data++;		    \
    }			    \
}

/* maxまでdataをインクリメントして上限超えたら初期化 */
#define	INC_MAX_INI(data,max,ini)   \
{				    \
    if( data < max ){		    \
	data++;			    \
    }else{			    \
	data = ini;		    \
    }				    \
}

extern T_UNIT s_unit;
extern DEVICE_SET s_device;
 
SYSTEM_MODE get_current_mode(void);
bool evt_act( EVENT_NUM evt );
void user_main_mode( void );
#endif //__STATE_MANAGER__