#ifndef	__STATE_MANAGER__
#define	__STATE_MANAGER__

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "main.h"

// �d�r�c�ʏ��
#define BAT_LEVEL_STS_MAX	0	// �[�d���^��
#define BAT_LEVEL_STS_HIGH	1	// ��������
#define BAT_LEVEL_STS_LOW	2	// �c��P�������Ȃ�
#define BAT_LEVEL_STS_MIN	3	// �d�r�c�ʂȂ�

// ���샂�[�h
#define ACT_MODE_SNORE		0	// �U�����[�h�i���т��j
#define ACT_MODE_SNORE_APNEA	1	// �U�����[�h�i���т� + ���ċz�j
#define ACT_MODE_MONITOR	2	// ���j�^�����O���[�h
#define ACT_MODE_APNEA		3	// �U�����[�h�i���ċz�j

// �U���̋���
#define	VIB_SET_MODE_WEAK	0	// ��
#define	VIB_SET_MODE_MID	1	// ��
#define	VIB_SET_MODE_STRONG	2	// ��
#define	VIB_SET_MODE_GURADUALLY	3	// ���X

// ���т��̌��o���x
#define SNORE_DETECT_LEVEL_LOW	    0   // 450 ��  
#define SNORE_DETECT_LEVEL_MID	    1   // 350 ��
#define SNORE_DETECT_LEVEL_HIGH	    2   // 250 ��

// �U���ő�A������
#define SET_MAX_VIB_CONT_5_MIN		0	// 5��
#define SET_MAX_VIB_CONT_10_MIN		1	// 10��
#define SET_MAX_VIB_CONT_NON		2	// �ݒ肵�Ȃ�

// �U���J�n�J����(�f�t�H���g)
#define VIB_START_TIME			20	// �Z���V���O�J�n20���Ԃ͐U�����Ȃ� 20min

// ����֌W
#define SENSING_CNT_MIN			40	// �ŏ����莞�� (20���F30�b���ɉ��Z)
#define	SAMPLING_INTERVAL_SEC		10	// 10�b�Ԋu

typedef enum{
    // 莉墓ｧ倽�翫迥�諷倶�矩�
    SYSTEM_MODE_INITIAL = 0,				// 繧�繝九す繝｣繝ｫ
    SYSTEM_MODE_IDLE_COM,					// 繧�繧�繝峨Ν_騾壻��蠕ｩ
    SYSTEM_MODE_SENSING,					// 繧�繝ｳ繧�繝ｳ繧�
    SYSTEM_MODE_GET,						// 繝繧�蜿門�
    SYSTEM_MODE_PRODUCT_CHECK,					// 閾�蟾�險�譽�
    SYSTEM_MODE_MOVE,						// 遘ｻ陦
    SYSTEM_MODE_NON,						// 縺�縺
    SYSTEM_MODE_MAX
}SYSTEM_MODE;

typedef enum{
    EVENT_NON = 0,				// な�
    EVENT_POW_SW_SHORT,			// 電源SW押�短)
    EVENT_POW_SW_LONG,			// 電源SW押�長)
    EVENT_CHG_ON,			// 兛�検知ポ��N
    EVENT_BAT_LOW,			// 電�残量低�
    EVENT_GET_DATA,				// �タ取�
    EVENT_PRODUCT_CHECK,		// 自己診断(通信)
    EVENT_COMPLETE,				// 完�
    EVENT_STOP,					// 中断
    EVENT_TIME_OUT,				// タイ�アウ�
    EVENT_REMOVE_TIMEOUT,		// 取り外れタイ�アウ�
    EVENT_MAX,					// 最大
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

// �t���[��(�g)�ԍ�
typedef struct{
    uint8_t read;		// �t���[��(�g)�̓ǂݏo���ԍ�
    uint8_t write;		// �t���[��(�g)�̏������ݔԍ�
    uint8_t cnt;		// �t���[��(�g)�̏������ݐ��@���������ݐ�0�ƍő�̋�ʂ����Ȃ���
}FRAME_NUM_INFO;

// �������
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
	    uint8_t act_mode;		// ���샂�[�h
	    uint8_t snore_detect_level; // ���т����x
	    uint8_t vib_power;		// �U�����x
	    uint8_t vib_max_time;	// �U������ő�p������
	    uint8_t vib_start_time;	// �U���J�n�ݒ莞��
	}dat;
    }info;
}DEVICE_SET;

typedef struct{
    DATE    date;
    uint8_t battery_sts;			// �d�r���

    uint16_t calc_cnt;		    // ���Z�J�E���g(�ۑ��f�[�^��)
    uint16_t snore_detect_cnt;	    // ���т����m��
    uint16_t apnea_detect_cnt;	    // ���ċz���m��
    uint16_t snore_time;	    // ���т�����
    uint16_t apnea_time;	    // ���ċz����
    uint16_t max_apnea_sec;	    // �ő喳�ċz[�b]
    uint16_t apnea_detect_cnt_max;  // �p�����ċz���m��(�ő�l�ۑ��p)
    uint16_t apnea_detect_cnt_now;  // �p�����ċz���m��(���ݒl)

    uint16_t snore_chg_detect_cnt;	// ���т��������m��(�ʐM�ő���"���т����m��")
    uint16_t apnea_chg_detect_cnt;	// ���ċz�������m��(�ʐM�ő���"���ċz���m��)
    uint16_t snore_detect_cnt_decided;	// �m��ς݂��т����m��(�ۑ��������Ƃ̊m�肵���J�E���g��)
    uint16_t apnea_detect_cnt_decided;	// �m��ςݖ��ċz���m��(�ۑ��������Ƃ̊m�肵���J�E���g��)

    uint8_t device_set_info;		// ���莞�̃f�o�C�X�ݒ�
    uint8_t vib_start_cnt;		// �U���J�n�J�E���g
    uint8_t sensing_flg;
/*
    uint8_t phase_snore;		// �b�ԃt�F�C�Y(���т�)
    uint8_t phase_breath;		// �b�ԃt�F�C�Y(�ċz)
    uint8_t phase_body_direct;		// �b�ԃt�F�C�Y(�̂̌���)
    uint8_t phase_photoref;		// �b�ԃt�F�C�Y(�t�H�g�Z���T)
*/
    // �t���[��(�g)�ԍ�
    FRAME_NUM_INFO frame_num;			// �t���[��(�g)�ԍ�[EEP�R�s�[�G���A] ��EEP�Ƃ͈�v�����Ă���
    FRAME_NUM_INFO frame_num_work;		// �t���[��(�g)�ԍ����[�N

    uint8_t get_mode_seq;				// GET���[�h�V�[�P���X
    uint16_t get_mode_calc_cnt;
    uint16_t set_mode_calc_cnt;
}T_UNIT;

// ���O���^�C���A�E�g���̕ۑ��p
typedef struct{
    uint16_t s_calc_cnt;		    // ���Z�J�E���g(�ۑ��f�[�^��)
    uint16_t s_snore_detect_cnt;	    // ���т����m��
    uint16_t s_apnea_detect_cnt;	    // ���ċz���m��
    uint16_t s_snore_chg_detect_cnt;	    // ���т��������m��(�ʐM�ő���"���т����m��")
    uint16_t s_apnea_chg_detect_cnt;	    // ���ċz�������m��(�ʐM�ő���"���ċz���m��)
    uint16_t s_apnea_detect_cnt_max;	    // �p�����ċz���m��(�ő�l�ۑ��p)
    uint16_t s_snore_detect_cnt_decided;    // �m��ς݂��т����m��(�ۑ��������Ƃ̊m�肵���J�E���g��)
    uint16_t s_apnea_detect_cnt_decided;    // �m��ςݖ��ċz���m��(�ۑ��������Ƃ̊m�肵���J�E���g��)
}T_UNIT_SAVE;

typedef struct{
    uint32_t	last_time;
    uint16_t	eep_cnt;		// EEP������
    DIAG_SEQ	seq;	// �V�[�P���X
}PRODUCT_CHECK;

/* =====�ėp�}�N���֐�===== */
/* max�܂�data���C���N�������g */
#define	INC_MAX(data,max)   \
{			    \
    if( data < max ){	    \
	data++;		    \
    }			    \
}

/* max�܂�data���C���N�������g���ď���������珉���� */
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