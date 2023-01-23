/* BLE�ʐM��M�f�[�^���� �֐��e�[�u�� */
static const BLE_RCV_CMD_TBL s_ble_rcv_func_tbl[BLE_CMD_TYPE_MAX] = {
	/* �R�}���h */					/* �����O�X */					/* �֐�  */							/* �����L�� */
	{0x00,			    0,				    NULL			    },	// �Ȃ�
	{BLE_CMD_MODE_CHG,	    BLE_CMD_LEN_MODE_CHG,	    main_ble_rcv_mode_chg	    },	// ��ԕύX(G1D)
	{BLE_CMD_DATE_SET,	    BLE_CMD_LEN_DATE_SET,	    main_ble_rcv_date		    },	// �����ݒ�
	{BLE_CMD_INFO,		    BLE_CMD_LEN_INFO,		    main_ble_rcv_info		    },	// ���擾
	{BLE_CMD_VERSION,	    BLE_CMD_LEN_VERSION,	    main_ble_rcv_version	    },	// �o�[�W�����擾
	{BLE_CMD_DEVICE_INFO,	    BLE_CMD_LEN_DEVICE_INFO,	    main_ble_rcv_device_info	    },	// �f�o�C�X�󋵎擾
	{BLE_CMD_DATA_NEXT,	    BLE_CMD_LEN_DATA_NEXT,	    NULL			    },	// NEXT[���M��p]
	{BLE_CMD_DATA_END,	    BLE_CMD_LEN_DATA_END,	    main_ble_rcv_data_end	    },	// END[��M��SET��]
	{BLE_CMD_DATA_FRAME,	    BLE_CMD_LEN_DATA_FRAME,	    main_ble_rcv_data_frame	    },	// �g���(������)[��M��SET��]
	{BLE_CMD_DATA_CALC,	    BLE_CMD_LEN_DATA_CALC,	    main_ble_rcv_data_calc	    },	// �@��f�[�^[��M��SET��]
	{BLE_CMD_DATA_FIN,	    BLE_CMD_LEN_DATA_FIN,	    main_ble_rcv_data_fin	    },	// �f�[�^�擾�����ʒm
	{BLE_CMD_DEVICE_SET,	    BLE_CMD_LEN_DEVICE_SET,	    main_ble_rcv_device_set	    },	// �f�o�C�X�ݒ�ύX
	{BLE_CMD_VIB_CONFIRM,	    BLE_CMD_LEN_VIB_CONFIRM,	    main_ble_rcv_vib_confirm	    },	// �o�C�u����m�F
	{BLE_CMD_VIB_STOP,	    BLE_CMD_LEN_VIB_STOP,	    main_ble_rcv_vib_stop	    },	// �o�C�u�����~
};

/* BLE�ʐM��M�f�[�^���� �֐��e�[�u�� */
static const BLE_RCV_CMD_TBL s_ble_diag_rcv_func_tbl[BLE_CMD_TYPE_MAX] = {
	/* �R�}���h */					/* �����O�X */					/* �֐�  */							/* �����L�� */
	{0x00,			    0,				    NULL			    },	// �Ȃ�
	{BLE_CMD_MODE_CHG,	    BLE_CMD_LEN_MODE_CHG,	    main_ble_rcv_mode_chg	    },	// ��ԕύX(G1D)
	{BLE_CMD_VERSION,	    BLE_CMD_LEN_VERSION,	    main_ble_rcv_version	    },	// �o�[�W�����擾
	{BLE_CMD_DEVICE_INFO,	    BLE_CMD_LEN_DEVICE_INFO,	    main_ble_rcv_device_info	    },	// �f�o�C�X�󋵎擾
	{BLE_CMD_DIAG_POWER_OFF,    BLE_CMD_LEN_DIAG_POWER_OFF,	    main_ble_diag_rcv_power_off	    },	// �d��OFF
	{BLE_CMD_DIAG_CHARGE,	    BLE_CMD_LEN_DIAG_CHARGE,	    main_ble_diag_rcv_charge	    },	// �[�d����
	{BLE_CMD_DIAG_LED,	    BLE_CMD_LEN_DIAG_LED,	    main_ble_diag_rcv_led	    },	// LED����
	{BLE_CMD_DIAG_VIB,	    BLE_CMD_LEN_DIAG_VIB,	    main_ble_diag_rcv_vib	    },	// �o�C�u���[�V��������
	{BLE_CMD_DIAG_MIC,	    BLE_CMD_LEN_DIAG_MIC,	    main_ble_diag_rcv_mic	    },	// �}�C�N����
	{BLE_CMD_DIAG_ACL,	    BLE_CMD_LEN_DIAG_ACL,	    main_ble_diag_rcv_acl	    },	// �����x�Z���T�[����
	{BLE_CMD_DIAG_PHOTO,	    BLE_CMD_LEN_DIAG_PHOTO,	    main_ble_diag_rcv_photo	    },	// �����Z���T�[����
	{BLE_CMD_DIAG_EEPROM,	    BLE_CMD_LEN_DIAG_EEPROM,	    main_ble_diag_rcv_eep	    },	// EEPROM����
};