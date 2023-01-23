/************************************************************************/
/* システム名   : RD8001快眠チェッカー									*/
/* ファイル名   : calc_apnea.h											*/
/* 機能         : 無呼吸判定演算処理									*/
/* 変更履歴     : 2017.07.12 Axia Soft Design mmura	初版作成			*/
/* 注意事項     : なし													*/
/************************************************************************/
#ifndef		_CALC_APNEA_H_		/* 二重定義防止 */
#define		_CALC_APNEA_H_

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "../calc_data.h"

/************************************************************/
/*					マクロ定数定義							*/
/************************************************************/
// 無呼吸判定結果
#define APNEA_NORMAL		0	// 異常なし
#define APNEA_WARN		1	// 無呼吸区間あり
#define APNEA_ERROR		2	// 完全無呼吸
#define APNEA_NONE		3	// 判定エラー

#define APNEA_JUDGE_CNT 	0	// 無呼吸判定カウント
#define APNEA_CONT_POINT	10	// APNEA_PARAM_BIN_THREを超えていない連続データ数

#define	APNEA_DETECT_CNT_MAX	18	// 無呼吸連続検知上限(これ以上は不明とする)(3分)

/************************************************************/
/* 						型定義								*/
/************************************************************/


/************************************************************/
/*					外部参照宣言							*/
/************************************************************/
extern void	calc_apnea_init(void);
extern void	calc_apnea_judge(APNEA_UNIT *apneadata, SNORE_UNIT *snoredata);
extern void	calculator_apnea(const uint16_t *data_apnea, const uint16_t *data_snore);
extern uint8_t	get_state(void);

#endif	/*_CALC_APNEA_H_*/
/************************************************************/
/* END OF TEXT												*/
/************************************************************/
