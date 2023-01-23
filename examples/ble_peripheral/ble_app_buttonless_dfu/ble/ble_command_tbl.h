/* BLE通信受信データ処理 関数テーブル */
static const BLE_RCV_CMD_TBL s_ble_rcv_func_tbl[BLE_CMD_TYPE_MAX] = {
	/* コマンド */					/* レングス */					/* 関数  */							/* 応答有無 */
	{0x00,			    0,				    NULL			    },	// なし
	{BLE_CMD_MODE_CHG,	    BLE_CMD_LEN_MODE_CHG,	    main_ble_rcv_mode_chg	    },	// 状態変更(G1D)
	{BLE_CMD_DATE_SET,	    BLE_CMD_LEN_DATE_SET,	    main_ble_rcv_date		    },	// 日時設定
	{BLE_CMD_INFO,		    BLE_CMD_LEN_INFO,		    main_ble_rcv_info		    },	// 情報取得
	{BLE_CMD_VERSION,	    BLE_CMD_LEN_VERSION,	    main_ble_rcv_version	    },	// バージョン取得
	{BLE_CMD_DEVICE_INFO,	    BLE_CMD_LEN_DEVICE_INFO,	    main_ble_rcv_device_info	    },	// デバイス状況取得
	{BLE_CMD_DATA_NEXT,	    BLE_CMD_LEN_DATA_NEXT,	    NULL			    },	// NEXT[送信専用]
	{BLE_CMD_DATA_END,	    BLE_CMD_LEN_DATA_END,	    main_ble_rcv_data_end	    },	// END[受信はSET時]
	{BLE_CMD_DATA_FRAME,	    BLE_CMD_LEN_DATA_FRAME,	    main_ble_rcv_data_frame	    },	// 枠情報(日時等)[受信はSET時]
	{BLE_CMD_DATA_CALC,	    BLE_CMD_LEN_DATA_CALC,	    main_ble_rcv_data_calc	    },	// 機器データ[受信はSET時]
	{BLE_CMD_DATA_FIN,	    BLE_CMD_LEN_DATA_FIN,	    main_ble_rcv_data_fin	    },	// データ取得完了通知
	{BLE_CMD_DEVICE_SET,	    BLE_CMD_LEN_DEVICE_SET,	    main_ble_rcv_device_set	    },	// デバイス設定変更
	{BLE_CMD_VIB_CONFIRM,	    BLE_CMD_LEN_VIB_CONFIRM,	    main_ble_rcv_vib_confirm	    },	// バイブ動作確認
	{BLE_CMD_VIB_STOP,	    BLE_CMD_LEN_VIB_STOP,	    main_ble_rcv_vib_stop	    },	// バイブ動作停止
};

/* BLE通信受信データ処理 関数テーブル */
static const BLE_RCV_CMD_TBL s_ble_diag_rcv_func_tbl[BLE_CMD_TYPE_MAX] = {
	/* コマンド */					/* レングス */					/* 関数  */							/* 応答有無 */
	{0x00,			    0,				    NULL			    },	// なし
	{BLE_CMD_MODE_CHG,	    BLE_CMD_LEN_MODE_CHG,	    main_ble_rcv_mode_chg	    },	// 状態変更(G1D)
	{BLE_CMD_VERSION,	    BLE_CMD_LEN_VERSION,	    main_ble_rcv_version	    },	// バージョン取得
	{BLE_CMD_DEVICE_INFO,	    BLE_CMD_LEN_DEVICE_INFO,	    main_ble_rcv_device_info	    },	// デバイス状況取得
	{BLE_CMD_DIAG_POWER_OFF,    BLE_CMD_LEN_DIAG_POWER_OFF,	    main_ble_diag_rcv_power_off	    },	// 電源OFF
	{BLE_CMD_DIAG_CHARGE,	    BLE_CMD_LEN_DIAG_CHARGE,	    main_ble_diag_rcv_charge	    },	// 充電検査
	{BLE_CMD_DIAG_LED,	    BLE_CMD_LEN_DIAG_LED,	    main_ble_diag_rcv_led	    },	// LED検査
	{BLE_CMD_DIAG_VIB,	    BLE_CMD_LEN_DIAG_VIB,	    main_ble_diag_rcv_vib	    },	// バイブレーション検査
	{BLE_CMD_DIAG_MIC,	    BLE_CMD_LEN_DIAG_MIC,	    main_ble_diag_rcv_mic	    },	// マイク検査
	{BLE_CMD_DIAG_ACL,	    BLE_CMD_LEN_DIAG_ACL,	    main_ble_diag_rcv_acl	    },	// 加速度センサー検査
	{BLE_CMD_DIAG_PHOTO,	    BLE_CMD_LEN_DIAG_PHOTO,	    main_ble_diag_rcv_photo	    },	// 装着センサー検査
	{BLE_CMD_DIAG_EEPROM,	    BLE_CMD_LEN_DIAG_EEPROM,	    main_ble_diag_rcv_eep	    },	// EEPROM検査
};