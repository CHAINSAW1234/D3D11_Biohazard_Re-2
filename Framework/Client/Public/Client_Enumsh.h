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

	enum class MAP_FLOOR_TYPE { BASEMENT_2, BASEMENT_1, FLOOR_1, FLOOR_2, FLOOR_3, FLOOR_FREE, FLOOR_END };

	enum MAP_DIRECTION { DIRECTION_WEST, DIRECTION_EAST, DIRECTION_MID };

	enum PLAYER_DIREECTION { DIRECTION_FRONT = 1, DIRECTION_BACK = 2, DIRECTION_LEFT = 4, DIRECTION_RIGHT = 8, DIRECTION_END };

	enum MONSTER_TYPE { MT_ZOMBIE, MT_DEFAULT };
	enum MONSTER_STATE { MST_IDLE, MST_WALK, MST_BITE, MST_TURN, MST_DEFAULT, MST_DAMAGE, MST_DOWN, MST_STANDUP, MST_END };

	/* Interact Hit Types */
	enum HIT_TYPE { HIT_SMALL, HIT_BIG, HIT_CRITICAL, HIT_EXPLOSION, HIT_END };

	//���ҽ� ������ enum�� �־�ΰڽ��ϴ� �ִµ� ���� �ͺ��� ���µ� �ִ°� �����ϱ��
	enum ITEM_NUMBER {//���� �̸� �Դϴ� 70������ �߰� �Ǿ� ���� �빮�ڴ� �ӽ���
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
		submachinegun_bullet01a,
		magnumbulleta,
		biggun_bullet01a,
		inkribbon01a,
		woodbarricade01a,
		blastingfuse01a,
		gunpowder01a,
		gunpowder01b,
		strengtheningyellow01a,
		/*71*/
		vp70csparts01a,
		laserpointer01a,
		shotgunparts01a,
		submgparts01a,
		deserteaglecsparts01a,
		lasersightparts01a,
		ladysmithcsparts01a,
		vp70longmagazine,
		vp70powerup,
		vp70stock,
		ladysmithcsparts,
		lasersight,
		ladysmithpowerup,
		browninghlongmagazine,
		shotgunpartsstock,
		shotgunpartsbarrel,
		submgpartsmagazine,
		submgpartssilencer,
		deserteaglecspartsshight,
		deserteaglecspartsbarrel,
		grenadelauncherstock,
		flamethrowerpowerup,
		sparkshotpowerup,
		/*72*/
		oillighter01a,
		keypickingtool01a,
		photofilma01a,
		photofilmb01a,
		photofilmc01a,
		photofilmd01a,
		photofilme01a,
		/*73*/
		backdoorkey01a,
		keycase01a,
		extensioncord01a,
		squarecrank01a,
		unicornmedal01a,
		spadekey01a,
		cardkeylv101a,
		cardkeylv201a,
		valvehandle01a,
		starsbadge01a,
		kingscepter01a,
		hourglasskey01a,
		virginheart01a,
		bluejewelrybox01a,
		redjewelrybox01a,
		bishopplug01a,
		rookplug01a,
		kingplug01a,
		serpentstone01a,
		eaglestone01a,
		jaggerstone01a,
		handcuffskey01a,
		unicorndebris01a,
		sunstone01a,
		doorknob01a,
		drivergrip01a,
		driverheadplus01a,
		driverheadsocket01a,
		blankkey01a,
		wetwastepaper01a,
		statuebook01a,
		statuehand01a,
		floppydisk01a,
		virginmedal01a,
		diakey01a,
		carkey01a,
		condenser01a,
		cardkeylv001a,
		virginmedal02a,
		pocketbook01a,
		loveremblem01a,
		gearsmal01a,
		gear01a,
		gardenkey01a,
		knightplug01a,
		pawnplug01a,
		queenplug01a,
		blisterpack01a,
		swordofdamocles01a,
		orphanagekey01a,
		clubkey01a,
		manholeopener01a,
		plastickcontainer01a,
		plastickcontainer02a,
		eaglekey01a,
		heartkey01a,
		videotape01a,
		eaglemedal01a,
		brokeneaglekey01a,
		wolfkey01a,
		cardkeylv202a,
		valvehandle02a,
		listtagwatchb01a,
		medicinebottle01a,
		medicinebottle02a,
		medicinebottle03a,
		jointsnplugs01a,
		listtagpartsmaster01a,
		clisttagmaster01a,
		signalmodulator01a,
		trophy01a,
		memorysticklock01a,
		memorystickunlock01a,
		liftkey01a,
		llisttagguests01a,
		llisttaggeneral01a,
		llisttagadvanced01a,
		listtagpartsgeneral01a,
		listtagpartsadvanced01a,
		clisttagguests01a,
		clisttaggeneral01a,
		clisttagadvanced01a,
		videotape02a,
		modisk01a,
		samplecartridge01a,
		samplecartridge02a,
		antiviraldrugs01a,
		attachecase01a,
		leverswitches01a,
		prismpillar01a,
		prismpillar02a,
		prismpillar03a,
		laboratoryfuse01a,
		laboratoryfuse02a,
		pendant01a,
		scissors01a,
		chaincutter01a,
		cushiondoll01a,
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
		/*75*/
		oldkey01a,
		/*76*/
		keytag01a,
		/*77*/
		clairebox01a,
		leonbox01a,
		portablesafe01a,
		cookiescan01a,
		woodbox01a,
		/*end*/
		HandGun,
		ITEM_NUMBER_END 
	};


	//�κ��丮 �ȿ����� ������ Ÿ��
	enum ITEM_TYPE {
		EQUIPABLE, // ���� ���
		CONSUMABLE_EQUIPABLE, //�Ҹ� ���
		USEABLE, // ��� ������
		CONSUMABLE, //�Ҹ� ������
		QUEST, // ����Ʈ������
		DRAG_SHADOW,//�巹�� ������
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
		JOMBIE_BEHAVIOR_COLLIDER_END,
	};

	enum PLAYER_BEHAVIOR_COLLIDER_TYPE
	{
		PLAYER_BEHAVIOR_COLLIDER_CLIMB_BOX, // ������ �ڽ�
		PLAYER_BEHAVIOR_COLLIDER_CLIMB_WALL, // ������ ��
		PLAYER_BEHAVIOR_COLLIDER_CLIMB_WINDOW, // �����
		PLAYER_BEHAVIOR_COLLIDER_LADDER, //
		PLAYER_BEHAVIOR_COLLIDER_DOOR, //
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

}
