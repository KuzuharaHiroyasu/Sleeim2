#include "calc_data.h"
#include "../state_manager.h"
#include <string.h>

/************************************************************/
/* 定数定義													*/
/************************************************************/

/************************************************************/
/* 変数定義													*/
/************************************************************/
MIC_DATA_UNIT	mic_data = {0};
CALC		s_calc = {0};
CALC_ACL	s_calc_acl = {0};

/************************************************************************/
/* 関数     : calc_data_init											*/
/* 関数名   : 演算用データ初期化										*/
/* 引数     : なし														*/
/* 戻り値   : なし														*/
/* 変更履歴 : 2019.10.11 oneA 葛原				初版作成				*/
/************************************************************************/
/* 機能 :																*/
/************************************************************************/
/* 注意事項 :															*/
/* なし																	*/
/************************************************************************/
void calc_data_init(void)
{
    memset( &mic_data, 0, sizeof(mic_data));

    // いびき演算初期化
    calc_snore_init();
    
    // 呼吸レス演算初期化
    calc_apnea_init();
}

void calc_data_set(uint16_t snore_value, uint16_t breath_value)
{
    if(get_current_mode() == SYSTEM_MODE_SENSING){
	
	if( mic_data.apnea_data.apnea_cnt < DATA_SIZE ){
	    mic_data.apnea_data.apnea_val[mic_data.apnea_data.apnea_cnt] = breath_value;
	}
	if( mic_data.snore_data.snore_cnt < DATA_SIZE ){
	    mic_data.snore_data.snore_val[mic_data.snore_data.snore_cnt] = snore_value;
	}
	
	// データフルで演算呼出
	if( mic_data.apnea_data.apnea_cnt >= ( DATA_SIZE - 1 )){
	    calc_apnea_judge(&mic_data.apnea_data, &mic_data.snore_data);
	}

	if( mic_data.snore_data.snore_cnt >= ( DATA_SIZE - 1 )){
	    calc_snore_judge(&mic_data.snore_data);
	}
	s_unit.vib_start_cnt++;

	INC_MAX( mic_data.apnea_data.apnea_cnt, DATA_SIZE );
	INC_MAX( mic_data.snore_data.snore_cnt, DATA_SIZE );
    }else if(get_current_mode() == SYSTEM_MODE_IDLE_COM){
	if( mic_data.breath_data.breath_cnt < DATA_SIZE ){
	    mic_data.breath_data.breath_val[mic_data.breath_data.breath_cnt] = snore_value;
	}
	if( mic_data.breath_data.breath_cnt >= ( DATA_SIZE - 1 )){
	    calc_breath_judge(&mic_data.breath_data);
	}
	INC_MAX( mic_data.breath_data.breath_cnt, DATA_SIZE );
    }
}

uint32_t get_vib_start_time_exchange( void )
{
    return (get_vib_start_time() * 6);
}

void calc_acl(void)
{
    uint8_t	body_direct = BODY_DIRECTION_LEFT;
    uint8_t	clear_mask = BODY_DIRECTION_MASK;
    uint8_t	bit_shift = 0;

    // 体の向き判定
    if( 0 <= s_calc_acl.acl_x )
    {// 上 or 右
        if( 0 <= s_calc_acl.acl_z )
        {   // 上
	    body_direct = BODY_DIRECTION_UP;
	} else {
            // 右
            body_direct = BODY_DIRECTION_RIGHT;
	}
    } else {
        // 下 or 左
        if( 0 <= s_calc_acl.acl_z )
        {   // 左
	    body_direct = BODY_DIRECTION_LEFT;
        } else {
	    // 下
	    body_direct = BODY_DIRECTION_DOWN;
        }
    }

    // 10秒ごとの判定値をbitで設定する
    bit_shift = s_calc.phase_body_direct * BODY_DIRECTION_BIT;
    s_calc.info.dat.body_direct &= ~(clear_mask << bit_shift);
    s_calc.info.dat.body_direct |= (body_direct << bit_shift);
    s_calc.phase_body_direct++;

    if(s_calc.phase_body_direct >= SEC_PHASE_NUM){
	s_calc.phase_body_direct = SEC_PHASE_0_10;
    }
}

/*==============================================================================*/
/* EOF */
