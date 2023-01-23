/************************************************************************/
/* システム名   : RD8001快眠チェッカー									*/
/* ファイル名   : calc_snore.h											*/
/* 機能         : いびき判定演算処理									*/
/* 変更履歴     : 2018.07.25 Axia Soft Design mmura	初版作成			*/
/* 注意事項     : なし													*/
/************************************************************************/
#ifndef		_CALC_SNORE_H_		/* 二重定義防止 */
#define		_CALC_SNORE_H_
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "../calc_data.h"
#include "../main.h"

/************************************************************/
/*					マクロ定数定義							*/
/************************************************************/

/************************************************************/
/* 						型定義								*/
/************************************************************/
// 判定向け観測回数
#define SNORE_PARAM_SIZE	(10)
// ON判定回数(いびき)
#define SNORE_PARAM_ON_CNT	(8)
// ON判定回数(呼吸)
#define BREATH_PARAM_ON_CNT	(10)
// OFF判定回数
#define SNORE_PARAM_OFF_CNT	(0)
// いびきあり -> なしへの判定回数
#define SNORE_PARAM_NORMAL_CNT	(80)
// 許容誤差 0.4s
#define SNORE_PARAM_GOSA	(8)

#define	SNORE_PARAM_DETECT_LEVEL_LOW	450
#define	SNORE_PARAM_DETECT_LEVEL_MID	350
#define	SNORE_PARAM_DETECT_LEVEL_HIGH	250

// 振動最大時間
#define	VIB_MAX_CONT_5_MIN    30    // 10秒毎 1分(6) * 5 = 30回 = 5分
#define	VIB_MAX_CONT_10_MIN   60    // 10秒毎 1分(6) * 10 = 60回 = 10分
#define	VIB_MAX_CONT_NON    4320    // 10秒毎 1分(6) * 60 * 12 = 4320回 

typedef enum{
	CALC_TYPE_SNORE_JUDGE = 0,
	CALC_TYPE_AUTO_SENSING,
	CALC_TYPE_MAX
}CALC_TYPE;

/************************************************************/
/*					外部参照宣言							*/
/************************************************************/
extern void	calc_snore_init(void);
extern uint8_t	calc_snore_get(void);
extern uint8_t	calc_breath_get(void);
extern void	set_snore_sens( uint8_t sens );
extern void	Reset(uint8_t calc_type);
extern void	calc_snore_judge(SNORE_UNIT *snore_data);
extern void	calc_breath_judge(BREATH_UNIT *breath_data);

#endif	/*_CALC_SNORE_H_*/
/************************************************************/
/* END OF TEXT												*/
/************************************************************/
