/************************************************************************/
/* システム名   : RD8001快眠チェッカー									*/
/* ファイル名   : calc_snore.c											*/
/* 機能         : いびき判定演算処理									*/
/* 変更履歴     : 2018.07.25 Axia Soft Design mmura 初版作成			*/
/* 注意事項     : なし													*/
/************************************************************************/
#include <stdlib.h>
#include "calc_snore.h"
#include "apnea_param.h"
#include "../state_manager.h"

/************************************************************/
/* プロトタイプ宣言											*/
/************************************************************/
static void calc_snore_proc(const uint16_t *pData, uint8_t calc_type);
static int proc_on(int Pos, uint8_t calc_type);
static int proc_off(int Pos, uint8_t calc_type);
static void Save(uint8_t calc_type);
static void Judge(uint8_t calc_type);
static uint16_t get_snore_detect_level_exchange( void );
static uint32_t get_vib_max_time_exchange( void );

/************************************************************/
/* 定数定義													*/
/************************************************************/
#define RIREKI		3
#define RIREKI_BREATH	2
/************************************************************/
/* 変数定義													*/
/************************************************************/
static uint8_t	SnoreFlg_; // ONカウント中 or OFFカウント中
static uint8_t	SnoreCnt_; // ON連続回数, OFF連続回数 兼用
static int8_t	SnoreTime_[RIREKI];
static int8_t	BreathTime_[RIREKI_BREATH];
static int16_t	thresholds_over_num[DATA_SIZE];
static uint8_t	SnoreState_;		// いびき
static uint8_t	BreathState_;
static uint8_t	snoreJudgeOnFlg_;	// いびき判定ONフラグ
static uint8_t	judgeSkipFlg_;	// いびき判定スキップフラグ

static uint16_t	auto_sensing_thre = 100;

// いびき判定設定関連
static uint32_t	    vib_start_time;
static uint16_t	    snore_level;
static uint32_t	    vib_max_time;
static uint32_t	    vib_max_cnt;
static uint32_t	    vib_max_interval_cnt;
static uint8_t	    snore_state_flg;
static uint8_t	    act_mode;

/************************************************************************/
/* 関数     : calculator_apnea											*/
/* 関数名   : 無呼吸判定演算											*/
/* 引数     : int *data : 波形データ									*/
/* 戻り値   : なし														*/
/* 変更履歴 : 2017.07.12 Axia Soft Design mmura	初版作成				*/
/************************************************************************/
/* 機能 :																*/
/************************************************************************/
/* 注意事項 :															*/
/* なし																	*/
/************************************************************************/
void calc_snore_init(void)
{
    int ii;
    
    SnoreFlg_ = 0;
    SnoreCnt_ = 0;
    SnoreState_ = SNORE_OFF;
    BreathState_ = BREATH_OFF;
    snoreJudgeOnFlg_ = OFF;
    judgeSkipFlg_ = OFF;
    for(ii=0;ii<RIREKI;++ii){
	SnoreTime_[ii] = -1;
    }
    for(ii=0;ii<RIREKI_BREATH;++ii){
	BreathTime_[ii] = -1;
    }
    
    act_mode = get_act_mode();
    vib_start_time = get_vib_start_time_exchange();
    snore_level = get_snore_detect_level_exchange();
    vib_max_time = get_vib_max_time_exchange();
    vib_max_cnt = 0;
    vib_max_interval_cnt = 0;
    snore_state_flg = OFF;
    memset( thresholds_over_num, 0, sizeof(thresholds_over_num));
}

void calc_snore_judge(SNORE_UNIT *snore_data)
{
    int		i;
    uint32_t	average =0;
    uint32_t	sum =0;
    uint8_t	newstate;
    uint8_t	set_snore_mask  = 0x01;
    uint8_t	set_apnea_mask = 0x02;
    uint8_t	bit_shift = 0;
    	
    // 10秒間の平均値
    for( i = 0; i < snore_data->snore_cnt; ++i )
    {
	sum += snore_data->snore_val[i];
    }
    average = sum / snore_data->snore_cnt;
    
    // AD値10bitデータを2bitシフトして上位8bitのみを保存する(下位2bitは誤差範囲で問題なし)
    s_calc.info.dat.snore_val[s_calc.phase_snore] = (uint8_t)(( average >> 2 ) & 0xff );

    // いびき演算
    calc_snore_proc(snore_data->snore_val, CALC_TYPE_SNORE_JUDGE);
    newstate = calc_snore_get();
    
    // 振動最大時間オーバー時のリセット処理
    if(vib_max_cnt >= vib_max_time)
    {
	if( vib_max_interval_cnt <= vib_max_time )
	{
	    vib_max_interval_cnt++;
	}else{
	    // 振動最大時間オーバー時のインターバル満了
	    vib_max_interval_cnt = 0;
	    vib_max_cnt = 0;
	}
    }
	    
    bit_shift = s_calc.phase_snore * 2;
    if(newstate == SNORE_ON){
	s_calc.info.dat.state |= (set_snore_mask << bit_shift);		// いびき状態ON

	if( s_unit.vib_start_cnt >= vib_start_time )
	{// 振動開始時間待機 
	    if(act_mode == ACT_MODE_SNORE_APNEA || act_mode == ACT_MODE_SNORE)
	    {// 振動モード（いびき）、振動モード（いびき＋呼吸レス）
		if(vib_max_cnt < vib_max_time)
		{// 振動最大時間未満
		    vib_max_cnt++;
/*
		    if(vib_power == VIB_SET_MODE_GRADUALLY_STRONGER)
		    {
			    set_vib_level(vib_level);
			    vib_level++;
			    if(vib_level > VIB_LEVEL_12)
			    {
				    vib_level = VIB_LEVEL_9;
			    }
		    }
		    vib_startflg = true;
		    set_vib(set_vib_mode(vib_power));
*/
		}
	    }
	}
    }else{
	s_calc.info.dat.state &= ~(set_snore_mask << bit_shift);		// いびき状態OFF
	if(vib_max_interval_cnt == 0)
	{
	    vib_max_cnt = 0;	// 振動最大時間リセット
	}
/*
    if(vib_power == VIB_SET_MODE_GRADUALLY_STRONGER)
    {
	    if(act_mode == ACT_MODE_SUPPRESS_SNORE_APNEA)
	    {// 抑制モード（いびき + 無呼吸）の場合
		    if( ((s_unit.calc.info.dat.state >> bit_shift) & set_apnea_mask) == 0x00 )
		    {// 無呼吸判定チェック
			    // 無呼吸判定もされていない場合バイブレベルを初期化
			    vib_level = VIB_LEVEL_1;
		    }
	    } else if(act_mode == ACT_MODE_SUPPRESS_SNORE)
	    {// 抑制モード（いびき）の場合
		    vib_level = VIB_LEVEL_1;
	    }
    }
*/
    }
    // いびき検知数更新
    if( ((s_calc.info.dat.state >> bit_shift) & set_snore_mask) == set_snore_mask ){
	s_unit.snore_chg_detect_cnt++;
	// いびき発生検知回数
	if(snore_state_flg == OFF){
	    snore_state_flg = ON;
	    s_unit.snore_detect_cnt_decided++;
	}
    }else{
	snore_state_flg = OFF;
    }
    
    s_calc.phase_snore++;
    if(s_calc.phase_snore >= SEC_PHASE_NUM){
	s_calc.phase_snore = SEC_PHASE_0_10;
//		snore_data_max = true;
    }	

    snore_data->snore_cnt = 0;
}

void calc_breath_judge(BREATH_UNIT *breath_data)
{
    uint8_t	newstate;

    calc_snore_proc(breath_data->breath_val, CALC_TYPE_AUTO_SENSING);
    newstate = calc_breath_get();
    if(newstate == BREATH_ON){
	evt_act( EVENT_POW_SW_LONG );
    }
    breath_data->breath_cnt = 0;
}

/************************************************************************/
/* 関数     : calc_snore_proc											*/
/* 関数名   : いびき判定演算											*/
/* 引数     : int *pData : 波形データ									*/
/* 戻り値   : なし														*/
/* 変更履歴 : 2018.07.25 Axia Soft Design mmura	初版作成				*/
/************************************************************************/
/* 機能 :																*/
/************************************************************************/
/* 注意事項 :															*/
/* なし																	*/
/************************************************************************/
static void calc_snore_proc(const uint16_t *pData, uint8_t calc_type)
{
    int ii;
    int jj;
    int loop;
    int size;
    int pos=0;
    
    // 閾値を超えた回数の移動累計をとる
    loop = DATA_SIZE - SNORE_PARAM_SIZE;
    for(ii=0;ii<loop;++ii){
	thresholds_over_num[ii] = 0;
	size = ii+SNORE_PARAM_SIZE;
	for(jj=ii;jj<size;++jj){
	    if(calc_type == CALC_TYPE_SNORE_JUDGE)
	    {
		if(pData[jj] >= snore_level){
		    thresholds_over_num[ii] += 1;
		}
	    }else if(calc_type == CALC_TYPE_AUTO_SENSING){
		if(pData[jj] >= auto_sensing_thre){
		    thresholds_over_num[ii] += 1;
		}
	    }
	}
    }
	
    while(pos < loop){
	switch(SnoreFlg_){
	case 1:
	    pos = proc_on(pos, calc_type);
	    break;
	case 0:
	    pos = proc_off(pos, calc_type);
	    break;
	default:
	    calc_snore_init();
	    return;
	}
    }
    
    if(snoreJudgeOnFlg_ == OFF)
    {
        if(calc_type == CALC_TYPE_SNORE_JUDGE)
        {
	    SnoreState_ = SNORE_OFF;
        }else if(calc_type == CALC_TYPE_AUTO_SENSING){
	    BreathState_ = BREATH_OFF;
        }
    }
    judgeSkipFlg_ = OFF;
    snoreJudgeOnFlg_ = OFF;
}

/************************************************************************/
/* 関数     : proc_on													*/
/* 関数名   : いびきON時処理											*/
/* 引数     : int Data : 波形データ										*/
/* 戻り値   : なし														*/
/* 変更履歴 : 2018.07.25 Axia Soft Design mmura	初版作成				*/
/************************************************************************/
/* 機能 :																*/
/************************************************************************/
/* 注意事項 :															*/
/* なし																	*/
/************************************************************************/
static int proc_on(int Pos, uint8_t calc_type)
{
    int ii;
    int loop = DATA_SIZE - SNORE_PARAM_SIZE;
    int pos = loop;
    
    // OFF確定している場所を特定する
    for(ii=Pos;ii<loop;++ii){
	if(thresholds_over_num[ii] <= SNORE_PARAM_OFF_CNT){
	    SnoreFlg_ = 0;
	    pos = ii;
	    Save(calc_type);
	    if(judgeSkipFlg_ == OFF)
	    {
		Judge(calc_type);
	    }
	    break;
	}else{
	    SnoreCnt_ += 1;
	    if (SnoreCnt_ >= SNORE_PARAM_NORMAL_CNT) {
		Reset(calc_type);
	    }			
	}
    }
    
    return pos;
}

/************************************************************************/
/* 関数     : proc_off													*/
/* 関数名   : いびきOFF時処理											*/
/* 引数     : int Data : 波形データ										*/
/* 戻り値   : なし														*/
/* 変更履歴 : 2018.07.25 Axia Soft Design mmura	初版作成				*/
/************************************************************************/
/* 機能 :																*/
/************************************************************************/
/* 注意事項 :															*/
/* なし																	*/
/************************************************************************/
static int proc_off(int Pos, uint8_t calc_type)
{
    int ii;
    int loop = DATA_SIZE - SNORE_PARAM_SIZE;
    int pos = loop;
    int onCnt;

    if(calc_type == CALC_TYPE_SNORE_JUDGE)
    {
	onCnt = SNORE_PARAM_ON_CNT;
    }else if(calc_type == CALC_TYPE_AUTO_SENSING){
	onCnt = BREATH_PARAM_ON_CNT;
    }

    // ON確定している場所を特定する
    for(ii=Pos;ii<loop;++ii){
	if(thresholds_over_num[ii] >= onCnt){
	    SnoreFlg_ = 1;
	    SnoreCnt_ = 0;
	    pos = ii;
	    break;
	}else{
	    SnoreCnt_ += 1;
	    if(SnoreCnt_ >= SNORE_PARAM_NORMAL_CNT){
		Reset(calc_type);
	    }
	}
    }
    
    return pos;
}

/************************************************************************/
/* 関数     : Save														*/
/* 関数名   : いびき時間を保存											*/
/* 引数     : なし														*/
/* 戻り値   : なし														*/
/* 変更履歴 : 2018.07.25 Axia Soft Design mmura	初版作成				*/
/************************************************************************/
/* 機能 :																*/
/************************************************************************/
/* 注意事項 :															*/
/* なし																	*/
/************************************************************************/
static void Save(uint8_t calc_type)
{
    int ii;

	
    if(calc_type == CALC_TYPE_SNORE_JUDGE)
    {
        if(SnoreState_ == SNORE_ON)
        {
            // いびき中のノイズや体動時の擦れ音などは判定しない
            for(ii=0;ii<RIREKI-1;++ii){
                if(abs(SnoreCnt_-SnoreTime_[ii]) > SNORE_PARAM_GOSA){
		    judgeSkipFlg_ = ON;
		    SnoreCnt_ = 0;
	            return;
	        }
            }
		judgeSkipFlg_ = OFF;
            }
        for(ii=1;ii<RIREKI;++ii){
    	    SnoreTime_[RIREKI-ii] = SnoreTime_[RIREKI-ii-1];
        }
        SnoreTime_[0] = SnoreCnt_;
    }else if(calc_type == CALC_TYPE_AUTO_SENSING){
        for(ii=1;ii<RIREKI_BREATH;++ii){
	    BreathTime_[RIREKI_BREATH-ii] = BreathTime_[RIREKI_BREATH-ii-1];
        }
	BreathTime_[0] = SnoreCnt_;
    }
    SnoreCnt_ = 0;
}

/************************************************************************/
/* 関数     : Judge														*/
/* 関数名   : いびき判定												*/
/* 引数     : なし														*/
/* 戻り値   : なし														*/
/* 変更履歴 : 2018.07.25 Axia Soft Design mmura	初版作成				*/
/************************************************************************/
/* 機能 :																*/
/************************************************************************/
/* 注意事項 :															*/
/* なし																	*/
/************************************************************************/
static void Judge(uint8_t calc_type)
{
    int ii;
    
    if(calc_type == CALC_TYPE_SNORE_JUDGE)
    {
        for(ii=0;ii<RIREKI;++ii){
            if(SnoreTime_[ii] == -1){
                SnoreState_ = SNORE_OFF;
                return;
            }
        }

        for(ii=0;ii<RIREKI-1;++ii){
            if(abs(SnoreTime_[0]-SnoreTime_[ii+1]) > SNORE_PARAM_GOSA){
                SnoreState_ = SNORE_OFF;
                return;
            }
        }
	SnoreState_ = SNORE_ON;
    }else if(calc_type == CALC_TYPE_AUTO_SENSING){
        for(ii=0;ii<RIREKI_BREATH;++ii){
            if(BreathTime_[ii] == -1){
                BreathState_ = BREATH_OFF;
                return;
            }
        }

        for(ii=0;ii<RIREKI_BREATH-1;++ii){
            if(abs(BreathTime_[0]-BreathTime_[ii+1]) > SNORE_PARAM_GOSA){
                BreathState_ = BREATH_OFF;
                return;
            }
        }
	BreathState_ = BREATH_ON;
    }

    snoreJudgeOnFlg_ = ON;
}

/************************************************************************/
/* 関数     : Reset														*/
/* 関数名   : 通常呼吸への復帰処理										*/
/* 引数     : なし														*/
/* 戻り値   : なし														*/
/* 変更履歴 : 2018.07.25 Axia Soft Design mmura	初版作成				*/
/************************************************************************/
/* 機能 :																*/
/************************************************************************/
/* 注意事項 :															*/
/* なし																	*/
/************************************************************************/
void Reset(uint8_t calc_type)
{
    int ii;
    
    if(calc_type == CALC_TYPE_SNORE_JUDGE)
    {
        for(ii=0;ii<RIREKI;++ii){
            SnoreTime_[ii] = -1;
        }
        SnoreState_ = SNORE_OFF;
    }else if(calc_type == CALC_TYPE_AUTO_SENSING){
        for(ii=0;ii<RIREKI_BREATH;++ii){
	    BreathTime_[ii] = -1;
        }
        BreathState_ = BREATH_OFF;
    }
}


uint8_t calc_snore_get(void)
{
    return SnoreState_;
}

uint8_t calc_breath_get(void)
{
    return BreathState_;
}

/************************************************************************/
/* 関数     : set_snore_thre											*/
/* 関数名   : いびき検出レベル取得										*/
/* 引数     : UB sens													*/
/* 戻り値   : なし														*/
/* 変更履歴 : 2018.08.05 oneA 葛原 弘安			初版作成				*/
/************************************************************************/
/* 機能 :																*/
/************************************************************************/
/* 注意事項 :															*/
/* なし																	*/
/************************************************************************/
static uint16_t get_snore_detect_level_exchange( void )
{
    uint16_t level;
    uint8_t sens = get_snore_detect_level();

    switch ( sens )
    {
        case SNORE_DETECT_LEVEL_LOW:		// 低
            level = SNORE_PARAM_DETECT_LEVEL_LOW;
            break;
        case SNORE_DETECT_LEVEL_MID:		// 中
            level = SNORE_PARAM_DETECT_LEVEL_MID;
            break;
        case SNORE_DETECT_LEVEL_HIGH:		// 高
            level = SNORE_PARAM_DETECT_LEVEL_HIGH;
            break;
        default:
            level = SNORE_PARAM_DETECT_LEVEL_MID;
            break;
    }
    return level;
}

static uint32_t get_vib_max_time_exchange( void )
{
    uint32_t max_time;
    uint8_t set_vib_max_time = get_vib_max_time();

    switch ( set_vib_max_time )
    {
        case SET_MAX_VIB_CONT_5_MIN:		// 5分
            max_time = VIB_MAX_CONT_5_MIN;
            break;
        case SET_MAX_VIB_CONT_10_MIN:		// 10分
            max_time = VIB_MAX_CONT_10_MIN;
            break;
        case SET_MAX_VIB_CONT_NON:		// なし
            max_time = VIB_MAX_CONT_NON;
            break;
        default:
            max_time = VIB_MAX_CONT_10_MIN;
            break;
    }
    return max_time;
}

/*==============================================================================*/
/* EOF */
