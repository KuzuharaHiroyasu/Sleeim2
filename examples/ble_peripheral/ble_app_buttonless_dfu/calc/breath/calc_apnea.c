/************************************************************************/
/* �V�X�e����   : RD8001�����`�F�b�J�[									*/
/* �t�@�C����   : calc_apnea.c											*/
/* �@�\         : ���ċz���艉�Z����									*/
/* �ύX����     : 2018.01.11 Axia Soft Design �a�c ���ō쐬				*/
/* ���ӎ���     : �Ȃ�													*/
/************************************************************************/
#include <math.h>
#include "calc_apnea.h"
#include "apnea_param.h"
#include "movave_param.h"
#include "../state_manager.h"

/************************************************************/
/* �v���g�^�C�v�錾											*/
/************************************************************/
void calc_apnea(const double* pData, int DSize, int Param1, double Param2, double Param3, double Param4);
void edgeWeighted_MovAve(const uint16_t* data_apnea, double* movave_, uint32_t datasize);
void heartBeat_Remov(double* dc_, const uint16_t *data_apnea, double* movave_, const uint16_t* data_snore, uint32_t datasize);
extern int  peak_modify_a(const double in_data[] , int16_t in_res[] , double ot_data[] , double ot_hz[] , int size , double delta, double th);
extern void peak_vallay_a(const double in[] , int16_t ot[] , int size, int width , int peak );
uint8_t get_apnea_state(void);

/************************************************************/
/* �萔��`													*/
/************************************************************/

/************************************************************/
/* �ϐ���`													*/
/************************************************************/
double	data_apnea_temp_[DATA_SIZE];
double	prms_temp[DATA_SIZE_PRMS];
double	data_movave_temp_[DATA_SIZE];
double	ppoint_temp[DATA_SIZE];
static uint8_t cnt_apnea_point;
static int	    apnea_ = APNEA_NONE;	// �ċz���
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
	
    // �U���J�n����(�A�v������̐ݒ�l)������܂ł͂��ׂĒʏ�ċz�Ƃ���
    if(s_unit.vib_start_cnt <= vib_start_time)
    {
	newstate = APNEA_NORMAL;
    }

    if(newstate == APNEA_ERROR){
	s_calc.info.dat.state |= (set_apnea_mask << bit_shift);		// ���ċz���ON
	if( s_unit.vib_start_cnt >= vib_start_time )
	{// �U���J�n���ԑҋ@ 
	    if(act_mode == ACT_MODE_SNORE_APNEA || act_mode == ACT_MODE_APNEA)
	    {// �U�����[�h�i���т� + ���ċz�j���U�����[�h�i���ċz�j
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
	s_calc.info.dat.state &= ~(set_apnea_mask << bit_shift);		// ���ċz���OFF
    }
	
    // ���ċz���m���X�V
    if( ((s_calc.info.dat.state >> bit_shift) & set_apnea_mask) == set_apnea_mask ){
	s_unit.apnea_detect_cnt++;
	s_unit.apnea_detect_cnt_now++;
	
	// ���ċz�������m��
	if(apnea_state_flg == OFF){
	    apnea_state_flg = ON;
	    s_unit.apnea_chg_detect_cnt++;
	}
    }else{
	// �p�����ċz���m�̏I������(�ő�l���X�V)
	if(s_unit.apnea_detect_cnt_now > s_unit.apnea_detect_cnt_max){
	    // 3�������͕ۑ�����i�R���ȏ�̓A�v������[�s��]��ԂƂ��邽�߁j(3�� = 18��)
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
/* �֐�     : calculator_apnea											*/
/* �֐���   : ���ċz���艉�Z											*/
/* ����     : int *data : �g�`�f�[�^(�ċz��)							*/
/*          : int len   : �g�`�f�[�^�T�C�Y								*/
/* �߂�l   : �Ȃ�														*/
/* �ύX���� : 2017.07.12 Axia Soft Design mmura	���ō쐬				*/
/************************************************************************/
/* �@�\ :																*/
/************************************************************************/
/* ���ӎ��� :															*/
/* �Ȃ�																	*/
/************************************************************************/
void calculator_apnea(const uint16_t *data_apnea, const uint16_t *data_snore)
{
    uint32_t datasize;						// �g�`�f�[�^�̃o�C�g��
    double* data_apnea_temp;
    double* movave_;
    int ii;
    
    //�f�[�^�T�C�Y����
    datasize = DATA_SIZE;
    
    // ���Z�p�f�[�^�ڍs
    data_apnea_temp = &data_apnea_temp_[0];	//calloc
    movave_ = &data_movave_temp_[0];	
    
    for(ii = 0; ii < datasize; ++ii){
	data_apnea_temp[ii] = data_apnea[ii];
    }
    
    // �G�b�W�����ړ�����
    edgeWeighted_MovAve(data_apnea, movave_, datasize);
    
    // �S������
    heartBeat_Remov(data_apnea_temp, data_apnea, movave_, data_snore, datasize);
    
    // ������̌ċz���̈ړ�����
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
/* �֐�     : calc_apnea												*/
/* �֐���   : ���ċz���Z����											*/
/* ����     : �Ȃ�														*/
/* �߂�l   : �Ȃ�														*/
/* �ύX���� : 2017.07.12 Axia Soft Design mmura	���ō쐬				*/
/************************************************************************/
/* �@�\ :																*/
/************************************************************************/
/* ���ӎ��� :															*/
/* �Ȃ�																	*/
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
    
    // (41) ... �g�p���Ă��Ȃ����ߏȗ�
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
			// �ʏ�ċz�Ɣ��f�������_�Ŕ�����
			apnea_ = APNEA_NORMAL;
			break;
		    }
		}
	    }
	}else{
	    apnea_ = APNEA_NORMAL;
	}
    }
	
/* //��ċz�����ċz�Ɣ��肷�邽��
    // ���S���ċz�̔���
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
/* �֐�     : edgeWeighted_MovAve										*/
/* �֐���   : �G�b�W�����ړ�����										*/
/* ����     : �Ȃ�														*/
/* �߂�l   : �Ȃ�														*/
/* �ύX���� : 															*/
/************************************************************************/
/* �@�\ :																*/
/************************************************************************/
/* ���ӎ��� :															*/
/* �Ȃ�																	*/
/************************************************************************/
void edgeWeighted_MovAve(const uint16_t* data_apnea, double* movave_, uint32_t datasize)
{
    int ii;
    
    // �G�b�W�����ړ�����
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
/* �֐�     : heartBeat_Remov											*/
/* �֐���   : �S������													*/
/* ����     : �Ȃ�														*/
/* �߂�l   : �Ȃ�														*/
/* �ύX���� : 															*/
/************************************************************************/
/* �@�\ :																*/
/************************************************************************/
/* ���ӎ��� :															*/
/* �Ȃ�																	*/
/************************************************************************/
void heartBeat_Remov(double* dc_, const uint16_t*data_apnea, double* movave_, const uint16_t* data_snore, uint32_t datasize)
{
    bool before_under = false;
    bool after_under = false;
    int i, j;
    
    // �S������
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
/* �֐�     : get_state													*/
/* �֐���   : ��Ԏ擾													*/
/* ����     : �Ȃ�														*/
/* �߂�l   : �Ȃ�														*/
/* �ύX���� : 															*/
/************************************************************************/
/* �@�\ :																*/
/************************************************************************/
/* ���ӎ��� :															*/
/* �Ȃ�																	*/
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
