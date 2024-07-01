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

	//리소스 없더라도 enum값 넣어두겠습니다 있는데 없는 것보단 없는데 있는게 나으니깐요
	enum ITEM_NUMBER {//파일 이름 입니다 70까지만 추가 되어 있음 대문자는 임시임
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


	//인벤토리 안에서의 아이템 타입
	enum ITEM_TYPE {
		EQUIPABLE, // 장착 장비
		CONSUMABLE_EQUIPABLE, //소모 장비
		USEABLE, // 사용 아이템
		CONSUMABLE, //소모 아이템
		QUEST, // 퀘스트아이템
		DRAG_SHADOW,//드레그 쉐도우
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


	// 나중에 좀비 초기 생성 셋팅은 UNDISCOVERED의 것을 반영하겠습니다
	enum JOMBIE_BEHAVIOR_COLLIDER_TYPE
	{
		JOMBIE_BEHAVIOR_COLLIDER_BOX_100, // 넘을 수 있는 박스 모델은 찾았지만 이 박스가 배치되어 있는 위치를 찾지 못했습니다. 위치를 알고 계시다면 저에게 말해주세요.
		JOMBIE_BEHAVIOR_COLLIDER_BOX_50, //  넘을 수 있는 박스 모델은 찾았지만 이 박스가 배치되어 있는 위치를 찾지 못했습니다. 위치를 알고 계시다면 저에게 말해주세요.
		JOMBIE_BEHAVIOR_COLLIDER_RAILING_FALL, // 난간 O : em0000_0920_railing_fall
		JOMBIE_BEHAVIOR_COLLIDER_HIGH_RAILING_FALL,  // 난간O, 기어서 떨어질 수 있는(서쪽 끝 계단 3층 참고) : em0000_0855_Crawl_fall_bridge + em0000_0920_railing_fall
		JOMBIE_BEHAVIOR_COLLIDER_FALL, // 난간X, 그냥 떨어지거나 기어서 떨어질 수 있는 :  em0000_0920_railing_fall 불가능
		JOMBIE_BEHAVIOR_COLLIDER_STAIR, //계단 - 나중에 다른 LAYER로 빼서 줄 수 있으니 고려해보세용 (이유: 계단콜라이더와 좀비와 난간 콜라이더의 충돌 겹침 발생시 상태 처리가 귀찮을 수 있음)
		JOMBIE_BEHAVIOR_COLLIDER_WINDOW, //윈도우는 따로 모션이 있습니다 em0000_window_em.motlist.85 
		JOMBIE_BEHAVIOR_COLLIDER_DOOR, //문 모션은 윈도우와 같이 있습니다. em0000_door_em.motlist.85 
		JOMBIE_BEHAVIOR_COLLIDER_END,
	};

	enum PLAYER_BEHAVIOR_COLLIDER_TYPE
	{
		PLAYER_BEHAVIOR_COLLIDER_CLIMB_BOX, // 작전실 박스
		PLAYER_BEHAVIOR_COLLIDER_CLIMB_WALL, // 작전실 벽
		PLAYER_BEHAVIOR_COLLIDER_CLIMB_WINDOW, // 조사실
		PLAYER_BEHAVIOR_COLLIDER_LADDER, //
		PLAYER_BEHAVIOR_COLLIDER_DOOR, //
		PLAYER_BEHAVIOR_COLLIDER_END,
	};


	enum DOOR_TYPE
	{
		/* 1 Floor */
		ENTRANCE_DOWN_DOOR, /* 입구 아래 */
		ENTRANCE_UP_DOOR, /* 입구 위 */
		RECEPT_DOOR, /* 리셉션 문*/
		WEST_OFFICE_LEFT_DOOR, /* 서쪽 사무실 문 */
		WEST_OFFICE_RIGHT_DOOR, /* 메인홀쪽 서쪽 사무실 문 */
		R_O_DOOR, /* 기록실과 작전실 사이 문*/
		R_O_RIGHT_DOOR, /* 기록실과 작전실 사이 구역의 왼쪽 문*/
		R_O_DOWN_DOOR, /* 기록실과 작전실 사이 구역의 아랫 문*/
		RECORD_DOOR, /* 기록실 문*/
		OPERATION_DOOR, /* 작전실 문*/
		SAFE_DEPOSIT_BOX_DOOR, /* 안전 금고실 문*/
		DARK_DOORK, /* 암실 문 */
		OBSERVATION_DOOR, /* 관찰실 문*/
		EAST_OFFICE_LEFT_DOOR, /* 동쪽 사무실 문 */
		EAST_OFFICE_RIGHT_DOOR, /* 동쪽 사무실 문 */
		PRESS_DOOR, /* 기자실 문 */
		BOILER_UP_DOOR, /* 보일러실 위쪽 문*/
		BOILER_RIGHT_DOOR, /* 보일러실 오른쪽 문 */
		TOILET_DOOR, /* 화장실 문 */
		REST_DOOR, /* 휴게실 문 */
		PRESSLEFT_SIDEROOM_DOOR, /*기자실 왼쪽 작은 방 문 */
		SECURITY_OFFICE_DOOR, /* 경비 사무실 문 */
		EMERGENCY_ESACPE_DOWN_F1_DOOR, /* 비상탈출로 아래문 */
		EMERGENCY_ESACPE_LEFT_F1_DOOR, /* 비상탈출로 왼쪽문 */

		/* 2 Floor */
		WASH_ROOM_LEFT_ROOR, /* 샤워실 좌하단 문*/
		WASH_ROOM_RIGHT_ROOR, /* 샤워실 우하단 문*/
		STARS_OFFICE_DOOR, /* 스타즈 사무실 문 */
		ARMORY_DOOR, /* 스타즈 사무실과 무기고 연결문 */
		LAUNDRY_DOOR, /* 세탁실 문*/
		LIBRARY_F2_RIGHT_DOOR, /* 메인홀과 와도서관 사이 문 */
		LIBRARY_F2_DOWN_DOOR, /* 라운지와 와도서관 사이 문 */
		LOUNGE_DOOR, /* 라운지 왼쪽 문 */
		WAITING_LEFT_DOOR, /* 대기실 왼쪽 문*/ //안쓰요
		WAITING_RIGHT_DOOR, /* 대기실 오른쪽 문*/ //안쓰요
		PRIVATE_COLLECTION_LEFT_DOOR, /* 개인컬렉션 보관실 좌상단*/ //안쓰요
		PRIVATE_COLLECTION_RIGHT_DOOR, /* 개인컬렉션 보관실 우상단*/ //안쓰요
		ART_STORAGE_DOOR, /* 예술품 보관실 문 */
		CHIEF_OFFICE_DOOR, /* 서장 사무실 문 */ //안쓰요
		ROFE_DOOR, /* 지붕 문*/
		EMERGENCY_ESACPE_F2_DOOR, /* 비상탈출로 문*/

		/* 3 Floor*/
		WEST_STORAGE_DOOR, /* 서쪽 저장실 문*/
		WATCH_LEFT_TOWER, /* 시계탑 왼쪽 문*/
		WATCH_RIGHT_TOWER, /* 시계탑 오른쪽 문*/
		LIBRARY_F3_LEFT_DOOR, /* 도서관 왼쪽 문*/
		LIBRARY_F3_RIGHT_DOOR, /* 도서관 오른쪽 문*/
		UPPER_LEFT_SECTION_DOOR, /* 최 좌상단 구역 문*/
		EAST_STORAGE_LEFT_DOOR, /* 동쪽 저장실 왼쪽 문 */
		EAST_STORAGE_RIGHT_DOOR, /* 동쪽 저장실 오른쪽 문 */
		EAST_STORAGE_DOWN_DOOR, /* 동쪽 저장실 이동불가 문 */
		BALCONY_DOOR, /* 발코니 문*/

	};

}
