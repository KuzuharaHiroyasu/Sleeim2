typedef void (*MODE_FUNC)(void);
static MODE_FUNC p_user_main_mode_func[] = {user_main_mode_inital,	// SYSTEM_MODE_INITIAL			イニシャル
                                            user_main_mode_idle_com,	// SYSTEM_MODE_IDLE_COM			アイドル_通信待機
                                            user_main_mode_sensing,	// SYSTEM_MODE_SENSING			センシング
                                            user_main_mode_get,		// SYSTEM_MODE_GET				データ取得
					    user_main_mode_product_check,	// SYSTEM_MODE_PRODUCT_CHECK	自己診断
                                            user_main_mode_move,	// SYSTEM_MODE_MOVE				移行
                                            dummy,			// SYSTEM_MODE_NON				なし
};

typedef SYSTEM_MODE ( *EVENT_TABLE )( int event );
static EVENT_TABLE p_event_table[ EVENT_MAX ][ SYSTEM_MODE_MAX ] = {
// モード			INITIAL			IDLE_COM		SENSING			GET			PRODUCT_CHECK		MOVE			NON
/*イベントなし	*/	{ evt_non,		evt_non,		evt_non,		evt_non,		evt_non,		evt_non,		evt_non },
/*電源SW(短)	*/	{ evt_non,		evt_demo_vib,		evt_non,		evt_non,		evt_non,		evt_non,		evt_non },
/*電源SW押下(長)	*/	{ evt_idle_com,		evt_sensing_chg,	evt_idle_com,		evt_sensing_chg,	evt_non,		evt_non,		evt_non },
/*充電検知ポートO	*/	{ evt_idle_com,		evt_non,		evt_idle_com,		evt_non,		evt_non,		evt_non,		evt_non },
/*電池残量低下	*/	{ evt_non,		evt_non,		evt_idle_com,		evt_non,		evt_non,		evt_non,		evt_non },
/*データ取得		*/	{ evt_non,		evt_get,		evt_non,		evt_non,		evt_non,		evt_non,		evt_non },
/*自己診断(通信)	*/	{ evt_non,		evt_product_check,	evt_non,		evt_non,		evt_product_check,	evt_non,		evt_non },
/*完了		*/	{ evt_non,		evt_non,		evt_idle_com,		evt_idle_com,		evt_idle_com,		evt_non,		evt_non },
/*中断		*/	{ evt_non,		evt_non,		evt_idle_com,		evt_non,		evt_non,		evt_non,		evt_non },
/*タイムアウト	*/	{ evt_non,		evt_non,		evt_non,		evt_time_out,		evt_non,		evt_non,		evt_non },
/*取り外れﾀｲﾑｱｳﾄ	*/	{ evt_non,		evt_non,		evt_remove,		evt_non,		evt_non,		evt_non,		evt_non },
};