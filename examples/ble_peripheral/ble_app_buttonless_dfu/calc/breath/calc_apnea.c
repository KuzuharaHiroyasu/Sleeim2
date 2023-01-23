/************************************************************************/
/* システム名   : RD8001快眠チェッカー									*/
/* ファイル名   : calc_apnea.c											*/
/* 機能         : 無呼吸判定演算処理									*/
/* 変更履歴     : 2018.01.11 Axia Soft Design 和田 初版作成				*/
/* 注意事項     : なし													*/
/************************************************************************/
#include <math.h>
#include "calc_apnea.h"
#include "apnea_param.h"
#include "movave_param.h"
#include "../state_manager.h"

/************************************************************/
/* プロトタイプ宣言											*/
/************************************************************/
void calc_apnea(const double* pData, int DSize, int Param1, double Param2, double Param3, double Param4);
void edgeWeighted_MovAve(const uint16_t* data_apnea, double* movave_, uint32_t datasize);
void heartBeat_Remov(double* dc_, const uint16_t *data_apnea, double* movave_, const uint16_t* data_snore, uint32_t datasize);
extern int  peak_modify_a(const double in_data[] , int16_t in_res[] , double ot_data[] , double ot_hz[] , int size , double delta, double th);
extern void peak_vallay_a(const double in[] , int16_t ot[] , int size, int width , int peak );
uint8_t get_apnea_state(void);

/************************************************************/
/* 定数定義													*/
/************************************************************/

/************************************************************/
/* 変数定義													*/
/************************************************************/
double	data_apnea_temp_[DATA_SIZE];
double	prms_temp[DATA_SIZE_PRMS];
double	data_movave_temp_[DATA_SIZE];
double	ppoint_temp[DATA_SIZE];
static uint8_t cnt_apnea_point;
static int	    apnea_ = APNEA_NONE;	// 呼吸状態
static uint32_t	    vib_start_time;
static uint8_t	    apnea_state_flg;
static uint8_t	    act_mode;

void calc_apnea_init(void)
{
    cnt_apnea_point = 0;
    act_mode = get_act_mode();
    vib_start_time = get_vib_start_time_exchange();
    apnea_state_flg = OFF;
    memset( data_apnea_temp_, 0, sizeof(data_apnea_temp_));
    memset( prms_temp, 0, sizeof(prms_temp));
    memset( ppoint_temp, 0, sizeof(ppoint_temp));
    memset( data_movave_temp_, 0, sizeof(data_movave_temp_));
}

void calc_apnea_judge(APNEA_UNIT *apneadata, SNORE_UNIT *snoredata)
{
    uint8_t  newstate;
    uint8_t  set_snore_mask  = 0x01;
    uint8_t  set_apnea_mask = 0x02;
    uint8_t  bit_shift = 0;
        
    calculator_apnea(apneadata->apnea_val, snoredata->snore_val);
    apneadata->apnea_cnt = 0;
    newstate = get_apnea_state();
	
    bit_shift = s_calc.phase_apnea * 2;
	
    // 振動開始時間(アプリからの設定値)がくるまではすべて通常呼吸とする
    if(s_unit.vib_start_cnt <= vib_start_time)
    {
	newstate = APNEA_NORMAL;
    }

    if(newstate == APNEA_ERROR){
	s_calc.info.dat.state |= (set_apnea_mask << bit_shift);		// 無呼吸状態ON
	if( s_unit.vib_start_cnt >= vib_start_time )
	{// 振動開始時間待機 
	    if(act_mode == ACT_MODE_SNORE_APNEA || act_mode == ACT_MODE_APNEA)
	    {// 振動モード（いびき + 無呼吸）か振動モード（無呼吸）
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
		    set_vib(set_vib_mode(vib_power));
		    set_kokyu_val_off(ON);
*/
	    }
	}
    }else{
	s_calc.info.dat.state &= ~(set_apnea_mask << bit_shift);		// 無呼吸状態OFF
    }
	
    // 無呼吸検知数更新
    if( ((s_calc.info.dat.state >> bit_shift) & set_apnea_mask) == set_apnea_mask ){
	s_unit.apnea_detect_cnt++;
	s_unit.apnea_detect_cnt_now++;
	
	// 無呼吸発生検知回数
	if(apnea_state_flg == OFF){
	    apnea_state_flg = ON;
	    s_unit.apnea_chg_detect_cnt++;
	}
    }else{
	// 継続無呼吸検知の終了処理(最大値を更新)
	if(s_unit.apnea_detect_cnt_now > s_unit.apnea_detect_cnt_max){
	    // 3分未満は保存する（３分以上はアプリ側で[不明]状態とするため）(3分 = 18回)
	    if(s_unit.apnea_detect_cnt_now < APNEA_DETECT_CNT_MAX)
	    {
		s_unit.apnea_detect_cnt_max = s_unit.apnea_detect_cnt_now;
	    }
	}
	s_unit.apnea_detect_cnt_now = 0;
	
	apnea_state_flg = OFF;
    }
    
    s_calc.phase_apnea++;
    if(s_calc.phase_apnea >= SEC_PHASE_NUM){
	s_calc.phase_apnea = SEC_PHASE_0_10;
//	apnea_data_max = true;
    }
}

/************************************************************************/
/* 関数     : calculator_apnea											*/
/* 関数名   : 無呼吸判定演算											*/
/* 引数     : int *data : 波形データ(呼吸音)							*/
/*          : int len   : 波形データサイズ								*/
/* 戻り値   : なし														*/
/* 変更履歴 : 2017.07.12 Axia Soft Design mmura	初版作成				*/
/************************************************************************/
/* 機能 :																*/
/************************************************************************/
/* 注意事項 :															*/
/* なし																	*/
/************************************************************************/
void calculator_apnea(const uint16_t *data_apnea, const uint16_t *data_snore)
{
    uint32_t datasize;						// 波形データのバイト数
    double* data_apnea_temp;
    double* movave_;
    int ii;
    
    //データサイズ制限
    datasize = DATA_SIZE;
    
    // 演算用データ移行
    data_apnea_temp = &data_apnea_temp_[0];	//calloc
    movave_ = &data_movave_temp_[0];	
    
    for(ii = 0; ii < datasize; ++ii){
	data_apnea_temp[ii] = data_apnea[ii];
    }
    
    // エッジ強調移動平均
    edgeWeighted_MovAve(data_apnea, movave_, datasize);
    
    // 心拍除去
    heartBeat_Remov(data_apnea_temp, data_apnea, movave_, data_snore, datasize);
    
    // 除去後の呼吸音の移動平均
    for (ii = 0; ii < datasize; ++ii) {
	movave_[ii] = 0;
	if (ii >= APNEA_PARAM_AVE_NUM_HALF && ii < (datasize - APNEA_PARAM_AVE_NUM_HALF))
	{
	    movave_[ii] += data_apnea_temp[ii - 2];
	    movave_[ii] += data_apnea_temp[ii - 1];
	    movave_[ii] += data_apnea_temp[ii];
	    movave_[ii] += data_apnea_temp[ii + 1];
	    movave_[ii] += data_apnea_temp[ii + 2];
	    movave_[ii] /= (double)APNEA_PARAM_AVE_NUM;
	}
    }
    
    // (35) - (47)
    calc_apnea(movave_, datasize, APNEA_PARAM_AVE_CNT, APNEA_PARAM_AVE_THRE, 
	       APNEA_PARAM_BIN_THRE, APNEA_PARAM_APNEA_THRE);
}

/************************************************************************/
/* 関数     : calc_apnea												*/
/* 関数名   : 無呼吸演算処理											*/
/* 引数     : なし														*/
/* 戻り値   : なし														*/
/* 変更履歴 : 2017.07.12 Axia Soft Design mmura	初版作成				*/
/************************************************************************/
/* 機能 :																*/
/************************************************************************/
/* 注意事項 :															*/
/* なし																	*/
/************************************************************************/
void calc_apnea(const double* pData, int DSize, int Param1, double Param2, double Param3, double Param4)
{
    double* prms;
    double* ppoint;
    int datasize;
    int ii;
    int jj;
    int loop=0;
    int apnea=0;
    
    apnea_ = APNEA_NORMAL;
    
    // (41) ... 使用していないため省略
    // (42)
    // (43) = prms
    datasize = DSize / 20;
    prms = &prms_temp[0];
    for(ii=0;ii<datasize;++ii){
	double tmp = 0.0f;
	prms[ii] = 0.0f;
	for(jj=0;jj<20;++jj){
	    tmp += (pData[ii*20 + jj]*pData[ii*20 + jj]);
	}
	tmp /= 20;
	prms[ii] = sqrt(tmp);
    }
    
    // (44) = ppoint
    // (45) = prms
    ppoint = &ppoint_temp[0];
    for(ii=0;ii<datasize;++ii){
	if(prms[ii] >= Param3){
	    ppoint[ii] = 1;
	    cnt_apnea_point = 0;
	}else{
	    ppoint[ii] = 0;
	    cnt_apnea_point++;
	    if(cnt_apnea_point >= APNEA_CONT_POINT)
	    {
		apnea_ = APNEA_ERROR;
		cnt_apnea_point = 0;
	    }
	}
    }
    
    // (46)
    if(apnea_ != APNEA_ERROR)
    {
	if(datasize == 0){
	    apnea_ = APNEA_NORMAL;
	}
	else if(datasize > 9){
	    apnea_ = APNEA_ERROR;
	    loop = datasize - 9;
	    for(ii = 0; ii < loop; ++ii){
		apnea = 0;
		for(jj = 0; jj < datasize; ++jj){
		    apnea += ppoint[ii + jj];
		    if(APNEA_JUDGE_CNT < apnea){
			// 通常呼吸と判断した時点で抜ける
			apnea_ = APNEA_NORMAL;
			break;
		    }
		}
	    }
	}else{
	    apnea_ = APNEA_NORMAL;
	}
    }
	
/* //低呼吸も無呼吸と判定するため
    // 完全無呼吸の判定
    if(apnea_ == APNEA_WARN){
	apnea_ = APNEA_ERROR;
	for(ii=0;ii<datasize;++ii){
	    if(pData[ii] > Param4){
		apnea_ = APNEA_WARN;
		break;
	    }
	}
    }
*/
}

/************************************************************************/
/* 関数     : edgeWeighted_MovAve										*/
/* 関数名   : エッジ強調移動平均										*/
/* 引数     : なし														*/
/* 戻り値   : なし														*/
/* 変更履歴 : 															*/
/************************************************************************/
/* 機能 :																*/
/************************************************************************/
/* 注意事項 :															*/
/* なし																	*/
/************************************************************************/
void edgeWeighted_MovAve(const uint16_t* data_apnea, double* movave_, uint32_t datasize)
{
    int ii;
    
    // エッジ強調移動平均
    for (ii = 0; ii < datasize; ++ii) {
	movave_[ii] = 0;

	if (ii >= APNEA_PARAM_AVE_NUM_HALF && ii < (datasize - APNEA_PARAM_AVE_NUM_HALF))
	{
	    movave_[ii] += data_apnea[ii - 2] * DIAMETER_END;
	    movave_[ii] += data_apnea[ii - 1] * DIAMETER_NEXT;
	    movave_[ii] += data_apnea[ii] * DIAMETER_CENTER;
	    movave_[ii] += data_apnea[ii + 1] * DIAMETER_NEXT;
	    movave_[ii] += data_apnea[ii + 2] * DIAMETER_END;
	    movave_[ii] /= (double)APNEA_PARAM_AVE_NUM;
	}
    }
}

/************************************************************************/
/* 関数     : heartBeat_Remov											*/
/* 関数名   : 心拍除去													*/
/* 引数     : なし														*/
/* 戻り値   : なし														*/
/* 変更履歴 : 															*/
/************************************************************************/
/* 機能 :																*/
/************************************************************************/
/* 注意事項 :															*/
/* なし																	*/
/************************************************************************/
void heartBeat_Remov(double* dc_, const uint16_t*data_apnea, double* movave_, const uint16_t* data_snore, uint32_t datasize)
{
    bool before_under = false;
    bool after_under = false;
    int i, j;
    
    // 心拍除去
    for (i = 0; i < datasize; i++)
    {
	if (i >= PREVIOUS_DATA_NUM && i < (datasize - PREVIOUS_DATA_NUM))
	{
	    if (movave_[i] >= MAX_EDGE_THRESHOLD && data_snore[i] <= MIN_SNORE_THRESHOLD)
	    {
		for (j = 1; j <= PREVIOUS_DATA_NUM; j++)
		{
		    if (data_apnea[i - j] < MIN_BREATH_THRESHOLD)
		    {
			before_under = true;
		    }
		    if (data_apnea[i + j] < MIN_BREATH_THRESHOLD)
		    {
			after_under = true;
		    }
		}
		
		if (before_under && after_under)
		{
		    dc_[i] = 0;
		    for (j = 1; j <= PREVIOUS_DATA_NUM; j++)
		    {
			dc_[i - j] = 0;
			dc_[i + j] = 0;
		    }
		}
		before_under = false;
		after_under = false;
	    }
	}
    }
}

/************************************************************************/
/* 関数     : get_state													*/
/* 関数名   : 状態取得													*/
/* 引数     : なし														*/
/* 戻り値   : なし														*/
/* 変更履歴 : 															*/
/************************************************************************/
/* 機能 :																*/
/************************************************************************/
/* 注意事項 :															*/
/* なし																	*/
/************************************************************************/
uint8_t get_apnea_state(void)
{
#if 1
    return apnea_;
#else
    UB ret = 0;
    ret |= ((apnea_ << 6) & 0xC0);
    
    return ret;
#endif
}

/*==============================================================================*/
/* EOF */
