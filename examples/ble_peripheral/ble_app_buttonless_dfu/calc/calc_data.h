/************************************************************************/
/* �V�X�e����   : RD8001�����`�F�b�J�[									*/
/* �t�@�C����   : calc_data.h											*/
/* �@�\         : ���Z���ʃf�[�^										*/
/* �ύX����     : 2018.01.15 Axia Soft Design �a�c ���ō쐬				*/
/* ���ӎ���     : �Ȃ�													*/
/************************************************************************/
#ifndef		_CALC_DATA_H_		/* ��d��`�h�~ */
#define		_CALC_DATA_H_

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "../main.h"

/************************************************************/
/*					�}�N���萔��`							*/
/************************************************************/
//#define DATA_SIZE_SPO2	(128)
#define DATA_SIZE		(200)
#define DATA_SIZE_PRMS	(10)

// ���т����茋��
#define SNORE_OFF		0	// ���т��Ȃ�
#define SNORE_ON		1	// ���т�����

#define BREATH_OFF		0	// �ċz�Ȃ�
#define BREATH_ON		1	// �ċz����

// �̂̌���
#define BODY_DIRECTION_LEFT	0	// ������
#define BODY_DIRECTION_UP	1	// �����
#define BODY_DIRECTION_RIGHT	2	// �E����
#define BODY_DIRECTION_DOWN	3	// ������

#define BODY_DIRECTION_MASK	3	// bit�}�X�N�p
#define BODY_DIRECTION_BIT	2	// �g�pbit��

/************************************************************/
/* 						�^��`								*/
/************************************************************/


/************************************************************/
/*					�O���Q�Ɛ錾							*/
/************************************************************/
extern void calc_data_init(void);

extern uint8_t cont_apnea_point;

typedef struct{
    union{
	uint8_t	byte[EEP_CALC_DATA_SIZE];
	struct{
	    uint8_t	snore_val[SEC_PHASE_NUM];	// ���т��̑傫��
	    uint8_t	state;				// ���
	    uint8_t	body_direct;			// �̂̌���
	    uint8_t	photoref[SEC_PHASE_NUM];	// �t�H�g�Z���T�[
	}dat;
    }info;

    uint8_t phase_snore;		// �b�ԃt�F�C�Y(���т�)
    uint8_t phase_apnea;		// �b�ԃt�F�C�Y(�ċz)
    uint8_t phase_body_direct;		// �b�ԃt�F�C�Y(�̂̌���)
    uint8_t phase_photoref;		// �b�ԃt�F�C�Y(�t�H�g�Z���T)
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
