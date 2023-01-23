/************************************************************************/
/* �V�X�e����   : RD8001�����`�F�b�J�[									*/
/* �t�@�C����   : calc_apnea.h											*/
/* �@�\         : ���ċz���艉�Z����									*/
/* �ύX����     : 2017.07.12 Axia Soft Design mmura	���ō쐬			*/
/* ���ӎ���     : �Ȃ�													*/
/************************************************************************/
#ifndef		_CALC_APNEA_H_		/* ��d��`�h�~ */
#define		_CALC_APNEA_H_

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "../calc_data.h"

/************************************************************/
/*					�}�N���萔��`							*/
/************************************************************/
// ���ċz���茋��
#define APNEA_NORMAL		0	// �ُ�Ȃ�
#define APNEA_WARN		1	// ���ċz��Ԃ���
#define APNEA_ERROR		2	// ���S���ċz
#define APNEA_NONE		3	// ����G���[

#define APNEA_JUDGE_CNT 	0	// ���ċz����J�E���g
#define APNEA_CONT_POINT	10	// APNEA_PARAM_BIN_THRE�𒴂��Ă��Ȃ��A���f�[�^��

#define	APNEA_DETECT_CNT_MAX	18	// ���ċz�A�����m���(����ȏ�͕s���Ƃ���)(3��)

/************************************************************/
/* 						�^��`								*/
/************************************************************/


/************************************************************/
/*					�O���Q�Ɛ錾							*/
/************************************************************/
extern void	calc_apnea_init(void);
extern void	calc_apnea_judge(APNEA_UNIT *apneadata, SNORE_UNIT *snoredata);
extern void	calculator_apnea(const uint16_t *data_apnea, const uint16_t *data_snore);
extern uint8_t	get_state(void);

#endif	/*_CALC_APNEA_H_*/
/************************************************************/
/* END OF TEXT												*/
/************************************************************/
