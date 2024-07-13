#pragma once

namespace Client
{
	enum CHANNELID
	{
		BGM,
		CH1, CH2, CH3, CH4, CH5, CH6, CH7, CH8, CH9, CH10, CH11, CH12, CH13, CH14, CH15, CH16, CH17,
		CH18, CH19, CH20, CH21, CH22, CH23, CH24, CH25, CH26, CH27, CH28, CH29, CH30, CH31,
		MAXCHANNEL
	};

	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };

	enum class SHADER_PASS_VTXANIMMODEL {
		PASS_DEFAULT,
		PASS_ALPHABLEND,
		PASS_DISSOLVE,
		PASS_LIGHTDEPTH,
		PASS_EMISSIVE,
		PASS_LIGHTDEPTH_CUBE,
		PASS_NONCULL,
		PASS_END
	};	

	enum class SHADER_PASS_VTXMODEL {
		PASS_DEFAULT,
		PASS_ALPHABLEND,
		PASS_LIGHTDEPTH,
		PASS_LIGHTDEPTH_CUBE,
		PASS_WIREFRAME,
		PASS_BLOOD,
		PASS_END
	};

	enum UI_ID {
		ORTHOGONAL_UI,
		PERSPECTIVE_UI,
		BUTTON,
		ID_END
	};

	enum class ITEM_READ_TYPE { INCIDENT_LOG_NOTE, OPERATE_REPORT_NOTE, TASK_NOTE, MEDICINAL_NOTE, OFFICER_NOTE, GUNPOWDER_NOTE, FIX_LOCKER_NOTE, HAND_HELD_SAFE_NOTE, RICKER_NOTE, SAFE_PASSWARD_NOTE, PAMPHLET, END_NOTE };
	// 사건 일지 , 작전실 노트 , 레온의 과제, 약초의 효능, 경찰의 수첩, 화약 일지 , 락커 고쳐줘, 휴대용 금고 설명서, 리커에 대한 설명
	// 맵 5번 : 
	// 
	//    hand-held safe manual
	// 금고 몇번 돌리는 지? 스타스 오피스pamphlet
	// 팜플렛

	enum UI_OPERRATION{ POP_UP, UI_IDLE, HIDE, STATE_END };

	enum class UI_TUTORIAL_TYPE { TUTORIAL_WALK, TUTORIAL_AROUND, TUTORIAL_RUN, TUTORIAL_REROAD, TUTORIAL_AIM, TUTORIAL_EQUIP, INVENTORY_OPEN, TUTORIAL_END };

	enum class MAP_FLOOR_TYPE { BASEMENT_2, BASEMENT_1, FLOOR_1, FLOOR_2, FLOOR_3, FLOOR_FREE, FLOOR_END };

	enum MAP_DIRECTION { DIRECTION_WEST, DIRECTION_EAST, DIRECTION_MID };

	enum PLAYER_DIREECTION { DIRECTION_FRONT = 1, DIRECTION_BACK = 2, DIRECTION_LEFT = 4, DIRECTION_RIGHT = 8, DIRECTION_END };
	
	enum class DIRECTION { _F, _FL, _L, _BL, _B, _BR, _R, _FR, _END};

	enum MONSTER_TYPE { MT_ZOMBIE, MT_DEFAULT };
	enum MONSTER_STATE { MST_IDLE, MST_WALK, MST_BITE, MST_CREEP,
		MST_KNOCK_WINDOW, MST_BREAK_WINDOW, MST_IN_WINDOW, MST_BREAK_IN_WINDOW, MST_HOLD_WINDOW, 
		MST_OPEN_DOOR, MST_RUB_DOOR, MST_KNOCK_DOOR,
		MST_SLEEP, MST_TURN, MST_LIGHTLY_HOLD, MST_HOLD, MST_DEFAULT, MST_DAMAGE, MST_DOWN, MST_STANDUP, MST_TURNOVER, MST_END };

	/* Interact Hit Types */
	enum HIT_TYPE { HIT_SMALL, HIT_BIG, HIT_CRITICAL, HIT_EXPLOSION, HIT_END };

	enum LOCATION_MAP_VISIT
	{
		/* 1 Floor */
		MAIN_HOLL,						/* 0. 메인 홀*/
		ENTRANCE,						/* 1. 입구*/
		RECEPT_ROOM,					/* 2. 리셉션 */
		RECORD_ROOM,					/* 3. 기록실*/
		OPERATION_ROOM,					/* 4. 작전실*/
		SAFE_DEPOSIT_BOX_ROOM,			/* 5. 안전 금고실*/
		WEST_OFFICE,					/* 6. 서쪽 사무실 */
		DARK_ROOM,						/* 7. 암실 */
		BOILER_HALLWAY,					/* 8. 보일러실 복도 */
		BOILER_ROOM,					/* 9. 보일러실 */
		RECORD_HALLWAY,					/* 10. 기록실 복도 */
		OPERATION_HALLWAY,				/* 11. 작전실 복도 */
		DARKROOM_HALLWAY,				/* 12. 암실 복도 */
		WEST_CNTRAL_HALLWAY,			/* 13. 서쪽 중앙 복도*/
		OUT_LOCATION,					/* 14. 밖 */
		LOUNGE_HALLWAY,					/* 15. 휴게실 복도 */
		EAST_OFFICE,					/* 16. 동쪽 사무실 */
		LEFTSIDE_PRESSROOM_HALLWAY,		/* 17. 기자실 왼쪽 작은 방*/
		PRESS_ROOM,						/* 18. 기자실*/
		QUESTION_ROOM,					/* 19. 취조실 */
		OBSERVATION_ROOM,				/* 20. 관찰실 */
		REST_ROOM,						/* 21. 휴게실 */
		WASHROOM,						/* 22. 화장실*/
		SECURITY_OFFICE,				/* 23. 경비 사무실 */
		EAST_CENTRAL_HALLWAY,			/* 24. 동쪽 중앙 복도*/
		UP_PRESSROOM_HALLWAY,			/* 25. 기자실 위쪽 복도 */

		/* 2 Floor */
		LEFT_WESH_ROOM,					/* 26. 왼쪽 샤워실 */
		RIGHT_WESH_ROOM,				/* 27. 오른쪽 샤워실 */
		STARS_OFFICE,					/* 28. STARS 사무실 */
		LOUNGE,							/* 29. 라운지 */
		LIBARAY,						/* 30. 도서관 */
		LAUNDRY_ROOM,					/* 31. 세탁실 */
		STAIRS_UP_2F,					/* 32. 2층 계단 */
		STARTS_OFFICE_LEFT_HALLWAY,		/* 33. STARS 사무실 왼쪽 복도 */
		WAITING_ROOM,					/* 34. 대기실 */
		EAST_HALLWAY_2F,				/* 35. 2층 동쪽 복도 */
		ART_STORAGE,					/* 36. 예술품 보관실*/
		NONE_SPACE1,					/* 37. */
		NONE_SPACE2,					/* 38. */
		NONE_SPACE3,					/* 39. */
		ROOF,							/* 40. 지붕 */
		EMERGENCY_ESCAPE_ROUTE,			/* 41. 비상탈출로 */

		/* 3 Floor */
		CLOCK_TOP,						/* 42. 시계탑 */
		LEFT_TOP_HALLWAY,				/* 43. 최왼쪽 상단 복도 */
		WEST_STORE,						/* 44. 서쪽 저장실 */
		BALCONY,						/* 45. 발코니 */
		EAST_STORE,						/* 46. 동쪽 저장실 */
		LET_TOP_LIBARAY_HALLWAY,		/* 47. 도서관 위쪽 복도 */
		BALCONY_UP_HALLWAY,				/* 48. 동쪽 저장실 오른쪽 복도*/

		// 추가 방
		OPERATION_RECORD_ROOM,					/* 49. 기록실 작전실 사이방*/
		WEAPON_ROOM,					/* 50. 기록실 작전실 사이방*/

		LOCATION_MAP_VISIT_END
	};


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
		DOCUMENT,
		HOTKEY,
		FULL_SLOT_COMBINABLE_PICKED_UP,// 자리가 꽉찼을때 픽업된 조합가능 아이템
		INTERACT,
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
		SWITCH_ITEM,
		INTERACT_ITEM,
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
		OBJ_LADDER,
		OBJ_LEVER,
		OBJ_MOVINGSHELF,
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
		JOMBIE_BEHAVIOR_COLLIDER_WINDOW, //윈도우는 따로 모션이 있습니다 em0000_window_em.motlist.85 
		JOMBIE_BEHAVIOR_COLLIDER_DOOR, //문 모션은 윈도우와 같이 있습니다. em0000_door_em.motlist.85 
		JOMBIE_BEHAVIOR_COLLIDER_NON_WINDOW, //창문은 없고 창틀만 있는(기믹객체가 아니라 이미 깨져있는)
		JOMBIE_BEHAVIOR_COLLIDER_END,
	};
	//

	enum PLAYER_BEHAVIOR_COLLIDER_TYPE
	{
		PLAYER_BEHAVIOR_COLLIDER_CLIMB_BOX_UP, // 작전실 박스
		PLAYER_BEHAVIOR_COLLIDER_CLIMB_BOX_DOWN, // 작전실 박스
		PLAYER_BEHAVIOR_COLLIDER_CLIMB_WALL, // 작전실 벽
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

		END_DOOR

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
		herbsrb01a,					//10
		greenherbitem01a,
		redherbitem01a,
		blueherbitem01a,
		handgun_bullet01a,
		shotgun_bullet01a,
		woodbarricade01a,
		blastingfuse01a,
		_9vbattery01a,
		gunpowder01a,
		gunpowder01b,				//20
		strengtheningyellow01a,
		/*71무기 부착물*/
		vp70powerup,
		vp70longmagazine,
		shotgunpartsstock_00,
		shotgunpartsbarrel,
		/*73*/
		unicornmedal01a,
		spadekey01a,
		cardkeylv101a,
		cardkeylv201a,		// 둘중하나만?
		valvehandle01a,				//30
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
		rpddocument01a,				//40
		rpddocumentblood01a,
		diary01a,
		document01a,
		pamphlet01a,
		guidepamphlet01a,
		memo01a,
		raccoonmonthly01a,
		sewercopamphlet01a,
		report01a,
		nestlcokout01a,				//50
		sewerhintposter01a,
		rpdreport01a,
		rpdreport01b,
		chesshints01a,
		labopc01a,
		labopc01b,
		labopc01c,
		raccoonfigure01a,
		voicerecorder01a,
		mappolice01a,				//60
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
		Flash_Bomb,					//70
		Grenade,
		vp70stock,
		portablesafe,
		//statuebookhand,
		ITEM_NUMBER_END
	};

}
