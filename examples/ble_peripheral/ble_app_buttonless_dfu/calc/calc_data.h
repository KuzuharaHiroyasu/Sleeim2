/************************************************************************/
/* システム名   : RD8001快眠チェッカー									*/
/* ファイル名   : calc_data.h											*/
/* 機能         : 演算共通データ										*/
/* 変更履歴     : 2018.01.15 Axia Soft Design 和田 初版作成				*/
/* 注意事項     : なし													*/
/************************************************************************/
#ifndef		_CALC_DATA_H_		/* 二重定義防止 */
#define		_CALC_DATA_H_

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "../main.h"

/************************************************************/
/*					マクロ定数定義							*/
/************************************************************/
//#define DATA_SIZE_SPO2	(128)
#define DATA_SIZE		(200)
#define DATA_SIZE_PRMS	(10)

// いびき判定結果
#define SNORE_OFF		0	// いびきなし
#define SNORE_ON		1	// いびきあり

#define BREATH_OFF		0	// 呼吸なし
#define BREATH_ON		1	// 呼吸あり

// 体の向き
#define BODY_DIRECTION_LEFT	0	// 左向き
#define BODY_DIRECTION_UP	1	// 上向き
#define BODY_DIRECTION_RIGHT	2	// 右向き
#define BODY_DIRECTION_DOWN	3	// 下向き

#define BODY_DIRECTION_MASK	3	// bitマスク用
#define BODY_DIRECTION_BIT	2	// 使用bit数

/************************************************************/
/* 						型定義								*/
/************************************************************/


/************************************************************/
/*					外部参照宣言							*/
/************************************************************/
extern void calc_data_init(void);

extern uint8_t cont_apnea_point;

typedef struct{
    union{
	uint8_t	byte[EEP_CALC_DATA_SIZE];
	struct{
	    uint8_t	snore_val[SEC_PHASE_NUM];	// いびきの大きさ
	    uint8_t	state;				// 状態
	    uint8_t	body_direct;			// 体の向き
	    uint8_t	photoref[SEC_PHASE_NUM];	// フォトセンサー
	}dat;
    }info;

    uint8_t phase_snore;		// 秒間フェイズ(いびき)
    uint8_t phase_apnea;		// 秒間フェイズ(呼吸)
    uint8_t phase_body_direct;		// 秒間フェイズ(体の向き)
    uint8_t phase_photoref;		// 秒間フェイズ(フォトセンサ)
}CALC;

typedef struct{
    uint16_t	apnea_val[DATA_SIZE];
    uint8_t	apnea_cnt;
}APNEA_UNIT;

typedef struct{
    uint16_t	snore_val[DATA_SIZE];
    uint8_t	snore_cnt;
}SNORE_UNIT;

typedef struct{
    uint16_t	breath_val[DATA_SIZE];
    uint8_t	breath_cnt;
}BREATH_UNIT;

typedef struct{
    APNEA_UNIT	apnea_data;
    SNORE_UNIT	snore_data;
    BREATH_UNIT breath_data;
}MIC_DATA_UNIT;

#define	INC_MAX(data,max)	\
{				\
	if( data < max ){	\
		data++;		\
	}			\
}

typedef struct{
    int8_t	acl_x;
    int8_t	acl_y;
    int8_t	acl_z;
}CALC_ACL;

extern CALC s_calc;
extern CALC_ACL s_calc_acl;
extern uint32_t get_vib_start_time_exchange( void );
void calc_acl(void);

#endif	/*_CALC_DATA_H_*/
/************************************************************/
/* END OF TEXT												*/
/************************************************************/
