/************************************************************************/
/* �V�X�e����   : RD8001�����`�F�b�J�[									*/
/* �t�@�C����   : calc_snore.h											*/
/* �@�\         : ���т����艉�Z����									*/
/* �ύX����     : 2018.07.25 Axia Soft Design mmura	���ō쐬			*/
/* ���ӎ���     : �Ȃ�													*/
/************************************************************************/
#ifndef		_CALC_SNORE_H_		/* ��d��`�h�~ */
#define		_CALC_SNORE_H_
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "../calc_data.h"
#include "../main.h"

/************************************************************/
/*					�}�N���萔��`							*/
/************************************************************/

/************************************************************/
/* 						�^��`								*/
/************************************************************/
// ��������ϑ���
#define SNORE_PARAM_SIZE	(10)
// ON�����(���т�)
#define SNORE_PARAM_ON_CNT	(8)
// ON�����(�ċz)
#define BREATH_PARAM_ON_CNT	(10)
// OFF�����
#define SNORE_PARAM_OFF_CNT	(0)
// ���т����� -> �Ȃ��ւ̔����
#define SNORE_PARAM_NORMAL_CNT	(80)
// ���e�덷 0.4s
#define SNORE_PARAM_GOSA	(8)

#define	SNORE_PARAM_DETECT_LEVEL_LOW	450
#define	SNORE_PARAM_DETECT_LEVEL_MID	350
#define	SNORE_PARAM_DETECT_LEVEL_HIGH	250

// �U���ő厞��
#define	VIB_MAX_CONT_5_MIN    30    // 10�b�� 1��(6) * 5 = 30�� = 5��
#define	VIB_MAX_CONT_10_MIN   60    // 10�b�� 1��(6) * 10 = 60�� = 10��
#define	VIB_MAX_CONT_NON    4320    // 10�b�� 1��(6) * 60 * 12 = 4320�� 

typedef enum{
	CALC_TYPE_SNORE_JUDGE = 0,
	CALC_TYPE_AUTO_SENSING,
	CALC_TYPE_MAX
}CALC_TYPE;

/************************************************************/
/*					�O���Q�Ɛ錾							*/
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
