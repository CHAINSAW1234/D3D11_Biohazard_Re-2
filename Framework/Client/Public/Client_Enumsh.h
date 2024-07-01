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

	enum MAP_DIRECTION { DIRECTION_WEST, DIRECTION_EAST, DIRECTION_MID };
	enum PLAYER_DIREECTION { DIRECTION_FRONT = 1, DIRECTION_BACK = 2, DIRECTION_LEFT = 4, DIRECTION_RIGHT = 8, DIRECTION_END };
	enum class DIRECTION { _F, _FL, _L, _BL, _B, _BR, _R, _FR, _END};

	enum MONSTER_TYPE { MT_ZOMBIE, MT_DEFAULT };
	enum MONSTER_STATE { MST_IDLE, MST_WALK, MST_BITE, MST_TURN, MST_LIGHTLY_HOLD, MST_HOLD, MST_DEFAULT, MST_DAMAGE, MST_DOWN, MST_STANDUP, MST_END };

	/* Interact Hit Types */
	enum HIT_TYPE { HIT_SMALL, HIT_BIG, HIT_CRITICAL, HIT_EXPLOSION, HIT_END };

	//인벤토리 안에서의 아이템 타입
	enum ITEM_TYPE {
		EQUIPABLE, // 장착 장비
		CONSUMABLE_EQUIPABLE, //소모 장비
		USEABLE, // 사용 아이템
		CONSUMABLE, //소모 아이템
		QUEST, // 퀘스트아이템
		DRAG_SHADOW,//드레그 쉐도우
		COMBINABLE_PICKED_UP, //픽업된 아이템인데 조합가능
		UNCOMBINABLE_PICKED_UP, //픽업된 아이템인데 조합 불가
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



	//리소스 없더라도 enum값 넣어두겠습니다 있는데 없는 것보단 없는데 있는게 나으니깐요
	//파일 이름 입니다 70까지만 추가 되어 있음 대문자는 임시임
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
		ShotGun,
		ITEM_NUMBER_END
	};
}
