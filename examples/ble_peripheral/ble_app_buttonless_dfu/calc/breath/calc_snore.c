/************************************************************************/
/* �V�X�e����   : RD8001�����`�F�b�J�[									*/
/* �t�@�C����   : calc_snore.c											*/
/* �@�\         : ���т����艉�Z����									*/
/* �ύX����     : 2018.07.25 Axia Soft Design mmura ���ō쐬			*/
/* ���ӎ���     : �Ȃ�													*/
/************************************************************************/
#include <stdlib.h>
#include "calc_snore.h"
#include "apnea_param.h"
#include "../state_manager.h"

/************************************************************/
/* �v���g�^�C�v�錾											*/
/************************************************************/
static void calc_snore_proc(const uint16_t *pData, uint8_t calc_type);
static int proc_on(int Pos, uint8_t calc_type);
static int proc_off(int Pos, uint8_t calc_type);
static void Save(uint8_t calc_type);
static void Judge(uint8_t calc_type);
static uint16_t get_snore_detect_level_exchange( void );
static uint32_t get_vib_max_time_exchange( void );

/************************************************************/
/* �萔��`													*/
/************************************************************/
#define RIREKI		3
#define RIREKI_BREATH	2
/************************************************************/
/* �ϐ���`													*/
/************************************************************/
static uint8_t	SnoreFlg_; // ON�J�E���g�� or OFF�J�E���g��
static uint8_t	SnoreCnt_; // ON�A����, OFF�A���� ���p
static int8_t	SnoreTime_[RIREKI];
static int8_t	BreathTime_[RIREKI_BREATH];
static int16_t	thresholds_over_num[DATA_SIZE];
static uint8_t	SnoreState_;		// ���т�
static uint8_t	BreathState_;
static uint8_t	snoreJudgeOnFlg_;	// ���т�����ON�t���O
static uint8_t	judgeSkipFlg_;	// ���т�����X�L�b�v�t���O

static uint16_t	auto_sensing_thre = 100;

// ���т�����ݒ�֘A
static uint32_t	    vib_start_time;
static uint16_t	    snore_level;
static uint32_t	    vib_max_time;
static uint32_t	    vib_max_cnt;
static uint32_t	    vib_max_interval_cnt;
static uint8_t	    snore_state_flg;
static uint8_t	    act_mode;

/************************************************************************/
/* �֐�     : calculator_apnea											*/
/* �֐���   : ���ċz���艉�Z											*/
/* ����     : int *data : �g�`�f�[�^									*/
/* �߂�l   : �Ȃ�														*/
/* �ύX���� : 2017.07.12 Axia Soft Design mmura	���ō쐬				*/
/************************************************************************/
/* �@�\ :																*/
/************************************************************************/
/* ���ӎ��� :															*/
/* �Ȃ�																	*/
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
    	
    // 10�b�Ԃ̕��ϒl
    for( i = 0; i < snore_data->snore_cnt; ++i )
    {
	sum += snore_data->snore_val[i];
    }
    average = sum / snore_data->snore_cnt;
    
    // AD�l10bit�f�[�^��2bit�V�t�g���ď��8bit�݂̂�ۑ�����(����2bit�͌덷�͈͂Ŗ��Ȃ�)
    s_calc.info.dat.snore_val[s_calc.phase_snore] = (uint8_t)(( average >> 2 ) & 0xff );

    // ���т����Z
    calc_snore_proc(snore_data->snore_val, CALC_TYPE_SNORE_JUDGE);
    newstate = calc_snore_get();
    
    // �U���ő厞�ԃI�[�o�[���̃��Z�b�g����
    if(vib_max_cnt >= vib_max_time)
    {
	if( vib_max_interval_cnt <= vib_max_time )
	{
	    vib_max_interval_cnt++;
	}else{
	    // �U���ő厞�ԃI�[�o�[���̃C���^�[�o������
	    vib_max_interval_cnt = 0;
	    vib_max_cnt = 0;
	}
    }
	    
    bit_shift = s_calc.phase_snore * 2;
    if(newstate == SNORE_ON){
	s_calc.info.dat.state |= (set_snore_mask << bit_shift);		// ���т����ON

	if( s_unit.vib_start_cnt >= vib_start_time )
	{// �U���J�n���ԑҋ@ 
	    if(act_mode == ACT_MODE_SNORE_APNEA || act_mode == ACT_MODE_SNORE)
	    {// �U�����[�h�i���т��j�A�U�����[�h�i���т��{�ċz���X�j
		if(vib_max_cnt < vib_max_time)
		{// �U���ő厞�Ԗ���
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
	s_calc.info.dat.state &= ~(set_snore_mask << bit_shift);		// ���т����OFF
	if(vib_max_interval_cnt == 0)
	{
	    vib_max_cnt = 0;	// �U���ő厞�ԃ��Z�b�g
	}
/*
    if(vib_power == VIB_SET_MODE_GRADUALLY_STRONGER)
    {
	    if(act_mode == ACT_MODE_SUPPRESS_SNORE_APNEA)
	    {// �}�����[�h�i���т� + ���ċz�j�̏ꍇ
		    if( ((s_unit.calc.info.dat.state >> bit_shift) & set_apnea_mask) == 0x00 )
		    {// ���ċz����`�F�b�N
			    // ���ċz���������Ă��Ȃ��ꍇ�o�C�u���x����������
			    vib_level = VIB_LEVEL_1;
		    }
	    } else if(act_mode == ACT_MODE_SUPPRESS_SNORE)
	    {// �}�����[�h�i���т��j�̏ꍇ
		    vib_level = VIB_LEVEL_1;
	    }
    }
*/
    }
    // ���т����m���X�V
    if( ((s_calc.info.dat.state >> bit_shift) & set_snore_mask) == set_snore_mask ){
	s_unit.snore_chg_detect_cnt++;
	// ���т��������m��
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
/* �֐�     : calc_snore_proc											*/
/* �֐���   : ���т����艉�Z											*/
/* ����     : int *pData : �g�`�f�[�^									*/
/* �߂�l   : �Ȃ�														*/
/* �ύX���� : 2018.07.25 Axia Soft Design mmura	���ō쐬				*/
/************************************************************************/
/* �@�\ :																*/
/************************************************************************/
/* ���ӎ��� :															*/
/* �Ȃ�																	*/
/************************************************************************/
static void calc_snore_proc(const uint16_t *pData, uint8_t calc_type)
{
    int ii;
    int jj;
    int loop;
    int size;
    int pos=0;
    
    // 臒l�𒴂����񐔂̈ړ��݌v���Ƃ�
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
/* �֐�     : proc_on													*/
/* �֐���   : ���т�ON������											*/
/* ����     : int Data : �g�`�f�[�^										*/
/* �߂�l   : �Ȃ�														*/
/* �ύX���� : 2018.07.25 Axia Soft Design mmura	���ō쐬				*/
/************************************************************************/
/* �@�\ :																*/
/************************************************************************/
/* ���ӎ��� :															*/
/* �Ȃ�																	*/
/************************************************************************/
static int proc_on(int Pos, uint8_t calc_type)
{
    int ii;
    int loop = DATA_SIZE - SNORE_PARAM_SIZE;
    int pos = loop;
    
    // OFF�m�肵�Ă���ꏊ����肷��
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
/* �֐�     : proc_off													*/
/* �֐���   : ���т�OFF������											*/
/* ����     : int Data : �g�`�f�[�^										*/
/* �߂�l   : �Ȃ�														*/
/* �ύX���� : 2018.07.25 Axia Soft Design mmura	���ō쐬				*/
/************************************************************************/
/* �@�\ :																*/
/************************************************************************/
/* ���ӎ��� :															*/
/* �Ȃ�																	*/
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

    // ON�m�肵�Ă���ꏊ����肷��
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
/* �֐�     : Save														*/
/* �֐���   : ���т����Ԃ�ۑ�											*/
/* ����     : �Ȃ�														*/
/* �߂�l   : �Ȃ�														*/
/* �ύX���� : 2018.07.25 Axia Soft Design mmura	���ō쐬				*/
/************************************************************************/
/* �@�\ :																*/
/************************************************************************/
/* ���ӎ��� :															*/
/* �Ȃ�																	*/
/************************************************************************/
static void Save(uint8_t calc_type)
{
    int ii;

	
    if(calc_type == CALC_TYPE_SNORE_JUDGE)
    {
        if(SnoreState_ == SNORE_ON)
        {
            // ���т����̃m�C�Y��̓����̎C�ꉹ�Ȃǂ͔��肵�Ȃ�
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
/* �֐�     : Judge														*/
/* �֐���   : ���т�����												*/
/* ����     : �Ȃ�														*/
/* �߂�l   : �Ȃ�														*/
/* �ύX���� : 2018.07.25 Axia Soft Design mmura	���ō쐬				*/
/************************************************************************/
/* �@�\ :																*/
/************************************************************************/
/* ���ӎ��� :															*/
/* �Ȃ�																	*/
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
/* �֐�     : Reset														*/
/* �֐���   : �ʏ�ċz�ւ̕��A����										*/
/* ����     : �Ȃ�														*/
/* �߂�l   : �Ȃ�														*/
/* �ύX���� : 2018.07.25 Axia Soft Design mmura	���ō쐬				*/
/************************************************************************/
/* �@�\ :																*/
/************************************************************************/
/* ���ӎ��� :															*/
/* �Ȃ�																	*/
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
/* �֐�     : set_snore_thre											*/
/* �֐���   : ���т����o���x���擾										*/
/* ����     : UB sens													*/
/* �߂�l   : �Ȃ�														*/
/* �ύX���� : 2018.08.05 oneA ���� �O��			���ō쐬				*/
/************************************************************************/
/* �@�\ :																*/
/************************************************************************/
/* ���ӎ��� :															*/
/* �Ȃ�																	*/
/************************************************************************/
static uint16_t get_snore_detect_level_exchange( void )
{
    uint16_t level;
    uint8_t sens = get_snore_detect_level();

    switch ( sens )
    {
        case SNORE_DETECT_LEVEL_LOW:		// ��
            level = SNORE_PARAM_DETECT_LEVEL_LOW;
            break;
        case SNORE_DETECT_LEVEL_MID:		// ��
            level = SNORE_PARAM_DETECT_LEVEL_MID;
            break;
        case SNORE_DETECT_LEVEL_HIGH:		// ��
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
        case SET_MAX_VIB_CONT_5_MIN:		// 5��
            max_time = VIB_MAX_CONT_5_MIN;
            break;
        case SET_MAX_VIB_CONT_10_MIN:		// 10��
            max_time = VIB_MAX_CONT_10_MIN;
            break;
        case SET_MAX_VIB_CONT_NON:		// �Ȃ�
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
