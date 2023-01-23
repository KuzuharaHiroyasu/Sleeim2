typedef void (*MODE_FUNC)(void);
static MODE_FUNC p_user_main_mode_func[] = {user_main_mode_inital,	// SYSTEM_MODE_INITIAL			�C�j�V����
                                            user_main_mode_idle_com,	// SYSTEM_MODE_IDLE_COM			�A�C�h��_�ʐM�ҋ@
                                            user_main_mode_sensing,	// SYSTEM_MODE_SENSING			�Z���V���O
                                            user_main_mode_get,		// SYSTEM_MODE_GET				�f�[�^�擾
					    user_main_mode_product_check,	// SYSTEM_MODE_PRODUCT_CHECK	���Ȑf�f
                                            user_main_mode_move,	// SYSTEM_MODE_MOVE				�ڍs
                                            dummy,			// SYSTEM_MODE_NON				�Ȃ�
};

typedef SYSTEM_MODE ( *EVENT_TABLE )( int event );
static EVENT_TABLE p_event_table[ EVENT_MAX ][ SYSTEM_MODE_MAX ] = {
// ���[�h			INITIAL			IDLE_COM		SENSING			GET			PRODUCT_CHECK		MOVE			NON
/*�C�x���g�Ȃ�	*/	{ evt_non,		evt_non,		evt_non,		evt_non,		evt_non,		evt_non,		evt_non },
/*�d��SW(�Z)	*/	{ evt_non,		evt_demo_vib,		evt_non,		evt_non,		evt_non,		evt_non,		evt_non },
/*�d��SW����(��)	*/	{ evt_idle_com,		evt_sensing_chg,	evt_idle_com,		evt_sensing_chg,	evt_non,		evt_non,		evt_non },
/*�[�d���m�|�[�gO	*/	{ evt_idle_com,		evt_non,		evt_idle_com,		evt_non,		evt_non,		evt_non,		evt_non },
/*�d�r�c�ʒቺ	*/	{ evt_non,		evt_non,		evt_idle_com,		evt_non,		evt_non,		evt_non,		evt_non },
/*�f�[�^�擾		*/	{ evt_non,		evt_get,		evt_non,		evt_non,		evt_non,		evt_non,		evt_non },
/*���Ȑf�f(�ʐM)	*/	{ evt_non,		evt_product_check,	evt_non,		evt_non,		evt_product_check,	evt_non,		evt_non },
/*����		*/	{ evt_non,		evt_non,		evt_idle_com,		evt_idle_com,		evt_idle_com,		evt_non,		evt_non },
/*���f		*/	{ evt_non,		evt_non,		evt_idle_com,		evt_non,		evt_non,		evt_non,		evt_non },
/*�^�C���A�E�g	*/	{ evt_non,		evt_non,		evt_non,		evt_time_out,		evt_non,		evt_non,		evt_non },
/*���O����ѱ��	*/	{ evt_non,		evt_non,		evt_remove,		evt_non,		evt_non,		evt_non,		evt_non },
};