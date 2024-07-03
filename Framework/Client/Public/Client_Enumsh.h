#pragma once

namespace Client
{
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };

	enum class SHADER_PASS_VTXANIMMODEL {
		PASS_DEFAULT,
		PASS_DEFAULT_ATOS,
		PASS_DISSOLVE,
		PASS_LIGHTDEPTH,
		PASS_EMISSIVE,
		PASS_END
	};	

	enum UI_ID {
		ORTHOGONAL_UI,
		PERSPECTIVE_UI,
		BUTTON,
		ID_END
	};

	enum UI_OPERRATION{ POP_UP, UI_IDLE, HIDE, STATE_END };

	enum class UI_TUTORIAL_TYPE { TUTORIAL_WALK, TUTORIAL_AROUND, TUTORIAL_RUN, TUTORIAL_REROAD, TUTORIAL_AIM, TUTORIAL_EQUIP, INVENTORY_OPEN, TUTORIAL_END };

	enum class MAP_FLOOR_TYPE { BASEMENT_2, BASEMENT_1, FLOOR_1, FLOOR_2, FLOOR_3, FLOOR_FREE, FLOOR_END };

	enum MAP_DIRECTION { DIRECTION_WEST, DIRECTION_EAST, DIRECTION_MID };

	enum PLAYER_DIREECTION { DIRECTION_FRONT = 1, DIRECTION_BACK = 2, DIRECTION_LEFT = 4, DIRECTION_RIGHT = 8, DIRECTION_END };
	enum class DIRECTION { _F, _FL, _L, _BL, _B, _BR, _R, _FR, _END};

	enum MONSTER_TYPE { MT_ZOMBIE, MT_DEFAULT };
	enum MONSTER_STATE { MST_IDLE, MST_WALK, MST_BITE, MST_SLEEP, MST_TURN, MST_LIGHTLY_HOLD, MST_HOLD, MST_DEFAULT, MST_DAMAGE, MST_DOWN, MST_STANDUP, MST_TURNOVER, MST_END };

	/* Interact Hit Types */
	enum HIT_TYPE { HIT_SMALL, HIT_BIG, HIT_CRITICAL, HIT_EXPLOSION, HIT_END };

	enum LOCATION_MAP_VISIT
	{
		MAIN_HOLL,						/* 0. ���� Ȧ*/
		ENTRANCE,						/* 1. �Ա�*/
		RECEPT_ROOM,					/* 2. ������ */
		RECORD_ROOM,					/* 3. ��Ͻ�*/
		OPERATION_ROOM,					/* 4. ������*/
		SAFE_DEPOSIT_BOX_ROOM,			/* 5. ���� �ݰ��*/
		WEST_OFFICE,					/* 6. ���� �繫�� */
		DARK_ROOM,						/* 7. �Ͻ� */
		BOILER_HALLWAY,					/* 8. ���Ϸ��� ���� */
		BOILER_ROOM,					/* 9. ���Ϸ��� */
		RECORD_HALLWAY,					/* 10. ��Ͻ� ���� */
		OPERATION_HALLWAY,				/* 11. ������ ���� */
		DARKROOM_HALLWAY,				/* 12. �Ͻ� ���� */
		WEST_CNTRAL_HALLWAY,			/* 13. ���� �߾� ����*/
		/* 14. �� */
		LOUNGE_HALLWAY,					/* 15. �ްԽ� ���� */
		EAST_OFFICE,					/* 16. ���� �繫�� */
		LEFTSIDE_PRESSROOM_HALLWAY,		/* 17. ���ڽ� ���� ���� ��*/
		PRESS_ROOM,						/* 18. ���ڽ�*/
		QUESTION_ROOM,					/* 19. ������ */
		OBSERVATION_ROOM,				/* 20. ������ */
		REST_ROOM,						/* 21. �ްԽ� */
		WASHROOM,						/* 22. ȭ���*/
		SECURITY_OFFICE,				/* 23. ��� �繫�� */
		EAST_CENTRAL_HALLWAY,			/* 24. ���� �߾� ����*/
		UP_PRESSROOM_HALLWAY,			/* 25. ���ڽ� ���� ���� */
		LOCATION_MAP_VISIT_END
	};



	//�κ��丮 �ȿ����� ������ Ÿ��
	enum ITEM_TYPE {
		EQUIPABLE, // ���� ���
		CONSUMABLE_EQUIPABLE, //�Ҹ� ���
		USEABLE, // ��� ������
		CONSUMABLE, //�Ҹ� ������
		QUEST, // ����Ʈ������
		DRAG_SHADOW,//�巹�� ������
		COMBINABLE_PICKED_UP, //�Ⱦ��� �������ε� ���հ���
		UNCOMBINABLE_PICKED_UP, //�Ⱦ��� �������ε� ���� �Ұ�
		INVEN_ITEM_TYPE_END
	};

	enum INVENTORY_EVENT {
		EVENT_IDLE, 
		EQUIP_ITEM,
		UNEQUIP_ITEM,
		USE_ITEM,
		EXAMINE_ITEM,
		COMBINED_ITEM,
		HOTKEY_ASSIGNED_ITEM,
		REARRANGE_ITEM,
		DISCARD_ITEM,
		CONTEXTUI_SELECT,
		PICK_UP_ITEM,
		GET_ITEM,
		DROP_ITEM,
		INVEN_EVENT_END
	};

	enum ANIM_BONE_TYPE_COLLIDER_DOOR_SINGLE
	{
		ATC_SINGLE_DOOR_OPEN_L,
		ATC_SINGLE_DOOR_OPEN_R
	};

	enum ANIM_BONE_TYPE_COLLIDER_DOOR_DOUBLE
	{
		ATC_DOUBLE_DOOR_OPEN_L_SIDE_L,
		ATC_DOUBLE_DOOR_OPEN_L_SIDE_R,
		ATC_DOUBLE_DOOR_OPEN_R_SIDE_L,
		ATC_DOUBLE_DOOR_OPEN_R_SIDE_R,
		ATC_ROOT
	};

	enum DOUBLE_DOOR_MESH_TYPE
	{
		DOOR_L,
		DOOR_R
	};

	enum DOUBLE_DOOR_MODEL_TYPE
	{
		NORMAL_DOOR,
		FRONT_DOOR
	};
		
	enum FIRE_WALL_ROTATE_BONE_TYPE
	{
		DOOR,
	};
	
	enum OBJ_TYPE
	{
		OBJ_NOPART,
		OBJ_DOOR,
		OBJ_WINDOW,
		OBJ_CABINET,
		OBJ_SHUTTER,
		OBJ_STATUE,
		OBJ_BIGSTATUE,
		OBJ_HALL_STATUE,
		OBJ_EVENTPROP,
		OBJ_ITEM,
		OBJ_END
	};

	enum ANIM_BONE_TYPE_COLLIDER_CABINET
	{
		ATC_CABINET_DOOR
	};

	enum CABINET_MESH_TYPE
	{
		CABINET_DOOR = 2
	};


	// ���߿� ���� �ʱ� ���� ������ UNDISCOVERED�� ���� �ݿ��ϰڽ��ϴ�
	enum JOMBIE_BEHAVIOR_COLLIDER_TYPE
	{
		JOMBIE_BEHAVIOR_COLLIDER_BOX_100, // ���� �� �ִ� �ڽ� ���� ã������ �� �ڽ��� ��ġ�Ǿ� �ִ� ��ġ�� ã�� ���߽��ϴ�. ��ġ�� �˰� ��ôٸ� ������ �����ּ���.
		JOMBIE_BEHAVIOR_COLLIDER_BOX_50, //  ���� �� �ִ� �ڽ� ���� ã������ �� �ڽ��� ��ġ�Ǿ� �ִ� ��ġ�� ã�� ���߽��ϴ�. ��ġ�� �˰� ��ôٸ� ������ �����ּ���.
		JOMBIE_BEHAVIOR_COLLIDER_RAILING_FALL, // ���� O : em0000_0920_railing_fall
		JOMBIE_BEHAVIOR_COLLIDER_HIGH_RAILING_FALL,  // ����O, �� ������ �� �ִ�(���� �� ��� 3�� ����) : em0000_0855_Crawl_fall_bridge + em0000_0920_railing_fall
		JOMBIE_BEHAVIOR_COLLIDER_FALL, // ����X, �׳� �������ų� �� ������ �� �ִ� :  em0000_0920_railing_fall �Ұ���
		JOMBIE_BEHAVIOR_COLLIDER_STAIR, //��� - ���߿� �ٸ� LAYER�� ���� �� �� ������ ����غ����� (����: ����ݶ��̴��� ����� ���� �ݶ��̴��� �浹 ��ħ �߻��� ���� ó���� ������ �� ����)
		JOMBIE_BEHAVIOR_COLLIDER_WINDOW, //������� ���� ����� �ֽ��ϴ� em0000_window_em.motlist.85 
		JOMBIE_BEHAVIOR_COLLIDER_DOOR, //�� ����� ������� ���� �ֽ��ϴ�. em0000_door_em.motlist.85 
		JOMBIE_BEHAVIOR_COLLIDER_NON_WINDOW, //â���� ���� âƲ�� �ִ�(��Ͱ�ü�� �ƴ϶� �̹� �����ִ�)
		JOMBIE_BEHAVIOR_COLLIDER_END,
	};

	enum PLAYER_BEHAVIOR_COLLIDER_TYPE
	{
		PLAYER_BEHAVIOR_COLLIDER_CLIMB_BOX_UP, // ������ �ڽ�
		PLAYER_BEHAVIOR_COLLIDER_CLIMB_BOX_DOWN, // ������ �ڽ�
		PLAYER_BEHAVIOR_COLLIDER_CLIMB_WALL, // ������ ��
		PLAYER_BEHAVIOR_COLLIDER_STAIR, // ���
		PLAYER_BEHAVIOR_COLLIDER_END,
	};


	enum DOOR_TYPE
	{
		/* 1 Floor */
		ENTRANCE_DOWN_DOOR, /* �Ա� �Ʒ� */
		ENTRANCE_UP_DOOR, /* �Ա� �� */
		RECEPT_DOOR, /* ������ ��*/
		WEST_OFFICE_LEFT_DOOR, /* ���� �繫�� �� */
		WEST_OFFICE_RIGHT_DOOR, /* ����Ȧ�� ���� �繫�� �� */
		R_O_DOOR, /* ��Ͻǰ� ������ ���� ��*/
		R_O_RIGHT_DOOR, /* ��Ͻǰ� ������ ���� ������ ���� ��*/
		R_O_DOWN_DOOR, /* ��Ͻǰ� ������ ���� ������ �Ʒ� ��*/
		RECORD_DOOR, /* ��Ͻ� ��*/
		OPERATION_DOOR, /* ������ ��*/
		SAFE_DEPOSIT_BOX_DOOR, /* ���� �ݰ�� ��*/
		DARK_DOORK, /* �Ͻ� �� */
		OBSERVATION_DOOR, /* ������ ��*/
		EAST_OFFICE_LEFT_DOOR, /* ���� �繫�� �� */
		EAST_OFFICE_RIGHT_DOOR, /* ���� �繫�� �� */
		PRESS_DOOR, /* ���ڽ� �� */
		BOILER_UP_DOOR, /* ���Ϸ��� ���� ��*/
		BOILER_RIGHT_DOOR, /* ���Ϸ��� ������ �� */
		TOILET_DOOR, /* ȭ��� �� */
		REST_DOOR, /* �ްԽ� �� */
		PRESSLEFT_SIDEROOM_DOOR, /*���ڽ� ���� ���� �� �� */
		SECURITY_OFFICE_DOOR, /* ��� �繫�� �� */
		EMERGENCY_ESACPE_DOWN_F1_DOOR, /* ���Ż��� �Ʒ��� */
		EMERGENCY_ESACPE_LEFT_F1_DOOR, /* ���Ż��� ���ʹ� */

		/* 2 Floor */
		WASH_ROOM_LEFT_ROOR, /* ������ ���ϴ� ��*/
		WASH_ROOM_RIGHT_ROOR, /* ������ ���ϴ� ��*/
		STARS_OFFICE_DOOR, /* ��Ÿ�� �繫�� �� */
		ARMORY_DOOR, /* ��Ÿ�� �繫�ǰ� ����� ���Ṯ */
		LAUNDRY_DOOR, /* ��Ź�� ��*/
		LIBRARY_F2_RIGHT_DOOR, /* ����Ȧ�� �͵����� ���� �� */
		LIBRARY_F2_DOWN_DOOR, /* ������� �͵����� ���� �� */
		LOUNGE_DOOR, /* ����� ���� �� */
		WAITING_LEFT_DOOR, /* ���� ���� ��*/ //�Ⱦ���
		WAITING_RIGHT_DOOR, /* ���� ������ ��*/ //�Ⱦ���
		PRIVATE_COLLECTION_LEFT_DOOR, /* �����÷��� ������ �»��*/ //�Ⱦ���
		PRIVATE_COLLECTION_RIGHT_DOOR, /* �����÷��� ������ ����*/ //�Ⱦ���
		ART_STORAGE_DOOR, /* ����ǰ ������ �� */
		CHIEF_OFFICE_DOOR, /* ���� �繫�� �� */ //�Ⱦ���
		ROFE_DOOR, /* ���� ��*/
		EMERGENCY_ESACPE_F2_DOOR, /* ���Ż��� ��*/

		/* 3 Floor*/
		WEST_STORAGE_DOOR, /* ���� ����� ��*/
		WATCH_LEFT_TOWER, /* �ð�ž ���� ��*/
		WATCH_RIGHT_TOWER, /* �ð�ž ������ ��*/
		LIBRARY_F3_LEFT_DOOR, /* ������ ���� ��*/
		LIBRARY_F3_RIGHT_DOOR, /* ������ ������ ��*/
		UPPER_LEFT_SECTION_DOOR, /* �� �»�� ���� ��*/
		EAST_STORAGE_LEFT_DOOR, /* ���� ����� ���� �� */
		EAST_STORAGE_RIGHT_DOOR, /* ���� ����� ������ �� */
		EAST_STORAGE_DOWN_DOOR, /* ���� ����� �̵��Ұ� �� */
		BALCONY_DOOR, /* ���ڴ� ��*/

	};




	//���ҽ� ������ enum�� �־�ΰڽ��ϴ� �ִµ� ���� �ͺ��� ���µ� �ִ°� �����ϱ��
	//���� �̸� �Դϴ� 70������ �߰� �Ǿ� ���� �빮�ڴ� �ӽ���
	enum ITEM_NUMBER {
		emergencyspray01a,
		greenherb01a,
		redherb01a,
		blueherb01a,
		herbsgg01a,
		herbsgr01a,
		herbsgb01a,
		herbsggb01a,
		herbsggg01a,
		herbsgrb01a,
		herbsrb01a,
		greenherbitem01a,
		redherbitem01a,
		blueherbitem01a,
		handgun_bullet01a,
		shotgun_bullet01a,
		woodbarricade01a,
		blastingfuse01a,
		_9vbattery01a,
		gunpowder01a,
		gunpowder01b,
		strengtheningyellow01a,
		/*71���� ������*/
		vp70powerup,
		vp70longmagazine,
		shotgunpartsstock_00,
		shotgunpartsbarrel,
		/*73*/
		unicornmedal01a,
		spadekey01a,
		cardkeylv101a,
		cardkeylv201a,		// �����ϳ���?
		valvehandle01a,
		kingscepter01a,
		virginheart01a,
		blankkey01a,
		statuebook01a,
		statuehand01a,
		virginmedal01a,
		diakey01a,
		virginmedal02a,
		chaincutter01a,
		/*74*/
		rpddocument01a,
		rpddocumentblood01a,
		diary01a,
		document01a,
		pamphlet01a,
		guidepamphlet01a,
		memo01a,
		raccoonmonthly01a,
		sewercopamphlet01a,
		report01a,
		nestlcokout01a,
		sewerhintposter01a,
		rpdreport01a,
		rpdreport01b,
		chesshints01a,
		labopc01a,
		labopc01b,
		labopc01c,
		raccoonfigure01a,
		voicerecorder01a,
		mappolice01a,
		mapunderground01a,
		mapsewer01a,
		mapraccooncity01a,
		maplaboratoryhigh01a,
		maplaboratorymiddle01a,
		mapupperpolice01a,
		clairesbag01a,
		/*end*/
		HandGun,
		ShotGun,
		Flash_Bomb,
		Grenade,
		vp70stock,
		portablesafe,
		ITEM_NUMBER_END
	};

}
