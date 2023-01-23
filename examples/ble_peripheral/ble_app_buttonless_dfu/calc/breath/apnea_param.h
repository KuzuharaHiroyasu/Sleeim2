/************************************************************************/
/* �V�X�e����   : ���ċz����											*/
/* �t�@�C����   : apnea_param.h											*/
/* �@�\         : ���ċz���Z�p�����[�^									*/
/* �ύX����     : 2018.05.16 Axia Soft Design mmura	���ō쐬			*/
/* ���ӎ���     : �Ȃ�                                                  */
/************************************************************************/

#ifndef		_APNEA_PARAM_H_			/* ��d��`�h�~ */
#define		_APNEA_PARAM_H_

/************************************************************/
/* �}�N��													*/
/************************************************************/
// ���f�[�^�␳
#define APNEA_PARAM_RAW			20000.0f

// �ړ����σf�[�^��
#define APNEA_PARAM_AVE_NUM				5
#define APNEA_PARAM_AVE_NUM_HALF		(APNEA_PARAM_AVE_NUM / 2)

// �s�[�N�Ԋu50%臒l
#define APNEA_PARAM_PEAK_THRE			87.85f

// �ړ����ςɗp����f�[�^�̐� 0.05 * 90 = 4.5s���̃f�[�^
#define APNEA_PARAM_AVE_CNT				90
// �ړ�����50%臒l
#define APNEA_PARAM_AVE_THRE			0.015f
// 2�l��50%臒l
#define APNEA_PARAM_BIN_THRE			20

// ���т�臒l
#define APNEA_PARAM_SNORE				0.0125f

// ���S���ċz����臒l
#define APNEA_PARAM_APNEA_THRE			(35 / APNEA_PARAM_RAW)


// �V���т����Z���� ---------------------------------------
// ��������ϑ���
#define SNORE_PARAM_SIZE				(10)
// ON�����
#define SNORE_PARAM_ON_CNT				(8)
// OFF�����
#define SNORE_PARAM_OFF_CNT				(0)
// ���т�臒l[���x�F��]
#define SNORE_PARAM_THRE_WERK			(450)
// ���т�臒l[���x�F��]
#define SNORE_PARAM_THRE_DURING			(350)
// ���т�臒l[���x�F��]
#define SNORE_PARAM_THRE_STRENGTH		(250)
// ���т����� -> �Ȃ��ւ̔����
#define SNORE_PARAM_NORMAL_CNT			(80)
// ���e�덷 0.4s
#define SNORE_PARAM_GOSA				(8)


#endif

/************************************************************/
/* END OF TEXT												*/
/************************************************************/

