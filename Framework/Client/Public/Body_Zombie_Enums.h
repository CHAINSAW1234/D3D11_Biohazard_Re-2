#pragma once

/* 모델의 타입 */
enum class ZOMBIE_BODY_TYPE { BODY_MALE, BODY_FEMALE, BODY_HEAVY, BODY_END };
/* 어떤 그룹의 애니메이션 인지 Ordinary .... */
enum class ZOMBIE_BODY_ANIM_GROUP { _ADD, _BITE, _DAMAGE, _DEAD, _LOST, _ORDINARY, _SICK, _UNDISCOVERED, _END};
/* 어떤 종류의 애니메이션 인지 */
enum class ZOMBIE_BODY_ANIM_TYPE { _IDLE, _MOVE, _TURN, _BLEND_MASK, _DAMAGE, _DEAD, _BITE, _STAND_UP, _END };	
/* Playing Info Index */
enum class PLAYING_INDEX { 
	INDEX_0,	INDEX_1,	INDEX_2,	INDEX_3,	INDEX_4,	INDEX_5,	INDEX_6,	INDEX_7,	INDEX_8,	INDEX_9, 
	INDEX_10,	INDEX_11,	INDEX_12,	INDEX_13,	INDEX_14,	INDEX_15,	INDEX_16,	INDEX_17,	INDEX_18,	INDEX_19, 
	INDEX_20,	INDEX_21,	INDEX_22,	INDEX_23,	INDEX_24,	INDEX_25,	INDEX_26,	INDEX_27,	INDEX_28,	INDEX_29, 
	INDEX_END };
/* A ~ F 모션 타입 => 아이들 워크등 나뉘어있음 */
enum class MOTION_TYPE { MOTION_A, MOTION_B, MOTION_C, MOTION_D, MOTION_E, MOTION_F, MOTION_END };

#pragma region Body_Anims

///////////////////////////////////////////////////////////////
/////////////////////////.....BODY..../////////////////////////
/////////////////////////..ANIMATION../////////////////////////
/////////////////////////............./////////////////////////
///////////////////////////////////////////////////////////////

#pragma region Add Anims

enum class ANIM_ADD_ARM_L {
	_FRONT,
	_BACK,
	_END
};

enum class ANIM_ADD_ARM_R {
	_FRONT,
	_BACK,
	_END
};

enum class ANIM_ADD_BODY {
	_FRONT,
	_BACK,
	_FACE_DOWN,
	_FACE_UP,
	_END
};

enum class ANIM_ADD_HEAD {
	_FRONT,
	_BACK,
	_FACE_DOWN,
	_FACE_UP,
	_END
};

enum class ANIM_ADD_LEG_L {
	_FRONT,
	_BACK,
	_END
};

enum class ANIM_ADD_LEG_R {
	_FRONT,
	_BACK,
	_END
};

enum class ANIM_ADD_SHOULDER_L{
	_FRONT,
	_BACK,
	_END
};

enum class ANIM_ADD_SHOULDER_R {
	_FRONT,
	_BACK,
	_END
};

#pragma endregion 

#pragma region Bite Anims

enum class ANIM_BITE_DEFAULT_FRONT {
	_START_F,				//	좀비가 플레이어의 정면에서 덮치기 시작
	_DEFAULT,				//	루프 애니메이션
	_CREEP_START,			//	좀비가 플레이어의 정면에 덮치기 시작 => 엎드려진상태에서 시작
	_REJECT1,				//	좀비를 플레이어의 정면으로 밀어냄
	_REJECT2,				//	좀비를 플레이어의 좌측으로 뿌리침	( 정면에서 덮친 모션에서 이어짐 )
	_REJECT3,				//	좀비를 플레이어의 뒤로 뿌리침	( 정면에서 덮친 모션에서 이어짐 )
	_KILL_F,
	_END
};

enum class ANIM_BITE_DEFAULT_BACK {
	_START_B,				//	좀비가 플레이어의 뒤에서 덮침
	_REJECT_B,				//	좀비를 플레이어의 뒤로 뿌리침	( 뒤에서 덮친 모션에서 이어짐 )
	_KILL_B,
	_END
};

enum class ANIM_BITE_PUSH_DOWN {
	_DOWN_START_L,			//	플레이어가 왼쪽 , 좀비가 오른쪽 ( 다운 상태의 플레이어를 물어 뜯음 )
	_DOWN_START_R,			//	플레이어가 오른쪽 , 좀비가 왼쪽 ( 다운 상태의 플레이어를 물어 뜯음 )
	_DOWN_REJECT_L,			//	오른쪽에 있는 좀비를 플레이어 우측으로 밀쳐냄
	_DOWN_REJECT_R,			//	왼쪽에 있는 좀비를 플레이어 좌측으로 밀쳐냄
	_PUSH_DOWN_L1,
	_PUSH_DOWN_R1,
	_PUSH_DOWN_L2,
	_PUSH_DOWN_R2,
	_DOWN_KILL_R,
	_END
};

enum class ANIM_BITE_CREEP {
	_FACE_DOWN_L,
	_FACE_DOWN_R,
	_FACE_UP_L,
	_FACE_UP_R,
	_CREEP_REJECT_L,
	_CREEP_REJECT_R,
	_CREEP_KILL_L,
	_CREEP_KILL_R,
	_END
};

enum class ANIM_BITE_LIGHTLY_HOLD {
	_LIGHTLY_HOLD_START_L,
	_LIGHTLY_HOLD_START_R,
	_LIGHTLY_HOLD_END_L,
	_LIGHTLY_HOLD_END_R,
	_END
};

enum class ANIM_BITE_ETC {
	_BOX1M_L,
	_BOX1M_R,
	_DIRECT,
	_END
};

#pragma endregion

#pragma region Damage

//	맞고 멀리 날라가는 모션들
enum class ANIM_DAMAGE_BURST {
	_START_F,
	_LOOP_F,
	_END_F,
	_START_B,
	_LOOP_B,
	_END_B,
	_LOOP_F_SPECIAL,
	_LOOP_B_SPECIAL,
	_FACEDOWN_START_F,
	_FACEDOWN_START_B,
	_FACEDOWN_LOOP_L,
	_FACEDOWN_END_L,
	_FACEDOWN_LOOP_R,
	_FACEDOWN_END_R, 
	_FACEUP_START_F,
	_FACEUP_START_B,
	_FACEUP_START_L,
	_FACEUP_LOOP_L,
	_FACEUP_START_R,
	_FACEUP_LOOP_R,
	_END
};

//	누운 채로 맞아서 흔들리는 모션들
enum class ANIM_DAMAGE_DEFAULT {
	_FACEDOWN_BODY,
	_FACEDOWN_ARM_L,
	_FACEDOWN_ARM_R,
	_FACEDOWN_LEG_L,
	_FACEDOWN_LEG_R,
	_FACEUP_BODY,
	_FACEUP_ARM_L,
	_FACEUP_ARM_R,
	_FACEUP_LEG_L,
	_FACEUP_LEG_R,
	_END
};

//	맞고 뻗는 모션들 ( 거의 제자리 )
enum class ANIM_DAMAGE_DOWN {
	_F_L1,
	_F_L2,
	_B_L1,
	_B_L2,
	_F_R1,
	_F_R2,
	_B_R1,
	_B_R2,
	_LOWSTATNCE_F1,
	_LOWSTATNCE_F2,
	_LOWSTATNCE_B1,
	_LOWSTATNCE_B2,
	_FACEDOWN_F,
	_FACEUP_F,
	_SEPARAT_F1,
	_SEPARAT_F2,
	_SEPARAT_FACEDOWN,
	_SEPARAT_FACEUP,
	_END
};

//	감전
enum class ANIM_DAMAGE_ELECTRIC_SHOCK {
	_END_F1,
	_END_F2,
	_FACEUP_START1,
	_FACEUP_START2,
	_FACE_UP_CANCEL1,
	_FACE_UP_CANCEL2,
	_FACE_DOWN_START1,
	_FACE_DOWN_START2,
	_FACE_DOWN_CANCEL1,
	_FACE_DOWN_CANCEL2,
	_END
};

//	맞고 강하게 밀려나는 모션들 ( 방향 F등 => 공격의 방향 F면 B로 밀려남 )
enum class ANIM_DAMAGE_KNOCKBACK{
	_HEAD_F1,
	_HEAD_F2,
	_HEAD_F3,
	_HEAD_F4,
	_HEAD_FL1,
	_HEAD_FR1,
	_HEAD_FL2,
	_HEAD_FR2,
	_HEAD_B,
	_HEAD_L,
	_HEAD_R,
	_BODY_F,
	_BODY_B,
	_BODY_L,
	_BODY_R,
	_F,
	_F_L_SLANT,		//	기울여 치기 => 팔등 맞았을때 적용하기로 일단
	_F_R_SLANT,
	_B,
	_B_L_SLANT,
	_B_R_SLANT,
	_END
};

//	부위를 잃게되는 모션들? => 애니메이션 재생이후 부위 파괴상태 바꿔줘야하는 기점이될듯
enum class ANIM_DAMAGE_LOST {
	_A_F_L,
	_A_F_R,
	_B_F_L,
	_B_F_R,
	_C_F_L,
	_C_F_R,
	_A2_F_L,
	_A2_F_R,
	_B2_F_L,
	_B2_F_R,
	_C2_F_L,
	_C2_F_R,
	_B3_F_L,
	_B3_F_R,
	_D_F_L1,
	_D_F_L2,
	_D_F_R1,
	_D_F_R2,
	_B4_F_LL,
	_B4_F_LR,
	_B4_F_RR,
	_B4_F_RL,
	_C3_F_LL,
	_C3_F_LR,
	_C3_F_RR,
	_C3_F_RL,
	_END
};

//	계단에서 자빠지는 모션 ( FaceDown => 전방으로 쓰러짐, 아래로 내려감, Face Up => 후방으로 쓰러짐, 위로 올라감 )
enum class ANIM_DAMAGE_STAIRS_DOWN {
	_FACEDOWN1,
	_FACEDOWN2,
	_FACEUP1,
	_FACEUP2,
	_END
};

//	계단에서 자빠지는 모션 ( FaceDown => 전방으로 쓰러짐, 위로 올라감, Face Up => 후방으로 쓰러짐, 아래로 내려감 )
enum class ANIM_DAMAGE_STAIRS_UP {
	_FACEDOWN1,
	_FACEDOWN2,
	_FACEUP1,
	_FACEUP2,
	_END
};

//	부위 피격별 약하게 밀려나면서 경직이후 제자리 ( 넉백은 강하게 밀리고 스턴은 약하게 밀림 )
enum class ANIM_DAMAGE_STUN {
	_HEAD_F1,
	_HEAD_F2,
	_HEAD_F3,
	_HEAD_F4,
	_HEAD_B,
	_HEAD_LEFT_SIDE_F,
	_HEAD_RIGHT_SIDE_F,
	_BODY_F,
	_BODY_B,
	_SHOULDER_L_F,
	_SHOULDER_L_B,
	_SHOULDER_R_F,
	_SHOULDER_R_B,
	_LEG_L_F,
	_LEG_L_B,
	_LEG_R_F,
	_LEG_R_B,
	_CREEP1,
	_CREEP2,
	_END
};

#pragma endregion

#pragma region Dead

enum class ANIM_DEAD_DEFAULT {
	_B,
	_LOWSTATNCE_F1,
	_LOWSTATNCE_F2,
	_LOWSTATNCE_B,
	_FACEUP1,
	_FACEUP2,
	_END
};

#pragma endregion

#pragma region Lost

enum class ANIM_LOST_BITE {
	_DIRECT1,
	_DIRECT2,
	_END
};

enum class ANIM_LOST_DEAD2IDLE {
	_0,
	_1,
	_2,
	_3,
	_4,
	_5,
	_6,
	_END
};

enum class ANIM_LOST_HOLD {
	_0,
	_1,
	_FACEUP1,
	_FACEUP2,
	_END
};

enum class ANIM_LOST_IDLE {
	_LOOP1,
	_LOOP2,
	_LOOP3,
	_LOOP4,
	_ANKLE_LOOP1,
	_ANKLE_LOOP2,
	_END
};

enum class ANIM_LOST_TURN {
	_LOOP_L_A,
	_LOOP_R_A_SYM,
	_LOOP_L_C,
	_LOOP_R_C_SYM,
	_LOOP_R_C,
	_LOOP_L_C_SYM,
	_LOOP_L_D,
	_LOOP_R_D_SYM,
	_LOOP_R_D,
	_LOOP_L_D_SYM,
	_LOOP_L_E,
	_LOOP_R_E_SYM,
	_LOOP_R_E,
	_LOOP_L_E_SYM,
	_LOOP_L_F,
	_LOOP_R_F_SYM,
	_LOOP_R_F,
	_LOOP_L_F_SYM,
	_LOOP_L_G,
	_LOOP_R_G_SYM,
	_LOOP_R_G,
	_LOOP_L_G_SYM,
	_LOOP_L_H,
	_LOOP_R_H,
	_LOOP_L_I,
	_LOOP_R_I,
	_LOOP_L_J,
	_LOOP_R_J,
	_L_ANKLE_PIVOTTURN_L90,
	_L_ANKLE_PIVOTTURN_R90,
	_R_ANKLE_PIVOTTURN_L90,
	_R_ANKLE_PIVOTTURN_R90,
	_L_ANKLE_PIVOTTURN_R180,
	_R_ANKLE_PIVOTTURN_L180,
	_END
};

//	뒤집기 동작
enum class ANIM_LOST_TURN_OVER {
	_0,
	_1,
	_2,
	_L,
	_R,
	_END
};

enum class ANIM_LOST_WALK {
	_LOOP_A,
	_LOOP_B,
	_LOOP_B_SYM,
	_LOOP_C,
	_LOOP_C_SYM,
	_START_D,
	_START_D_SYM,
	_LOOP_D,
	_LOOP_D_SYM,
	_START_E,
	_START_E_SYM,
	_LOOP_E,
	_LOOP_E_SYM,
	_START_F,
	_START_F_SYM,
	_LOOP_F,
	_LOOP_F_SYM,
	_LOOP_G,
	_LOOP_G_SYM,
	_LOOP_H,
	_LOOP_I,
	_START_J,
	_LOOP_J,
	_L_ANKLE_LOOP,
	_L_ANKLE_TURNING_LOOP_L,
	_L_ANKLE_TURNING_LOOP_R,
	_R_ANKLE_LOOP,
	_R_ANKLE_TURNING_LOOP_L,
	_R_ANKLE_TURNING_LOOP_R,
	_L_ANKLE_TURN_L180,
	_R_ANKLE_TURN_R180,
	_END
};

#pragma endregion

#pragma region Ordinary

enum class ANIM_ORDINARY_BOX_CLIMBOVER {
	_FIFTY_CENTI_START,
	_FIFTY_CENTI_END,
	_HUNDRED_CENTI_START,
	_HUNDRED_CENTI_END,
	_END
};

enum class ANIM_ORDINARY_ETC {
	_CRAWL_BRIDGE_START,
	_CRAWL_BRIDGE_LOOP,
	_RAILLING_FALL,
	_END
};

enum class ANIM_ORDINARY_HOLD {
	_F1,
	_F2,
	_TRIP_L_FOOT_UP,			//	홀드 중 다리 맞으면 해당 모션으로 전환하기
	_TRIP_R_FOOT_UP,			//	홀드 중 다리 맞으면 해당 모션으로 전환하기
	_L90,
	_R90,
	_L180,
	_R180,
	_END
};

enum class ANIM_ORDINARY_IDLE {
	_LOOP_A,
	_LOOP_B,
	_LOOP_C,
	_LOOP_D,
	_LOOP_E,
	_LOOP_F,
	_END
};

enum class ANIM_ORDINARY_PIVOT_TURN {
	_L90_A,
	_R90_A,
	_L90_B,
	_R90_B,
	_L90_C,
	_R90_C,
	_L90_D,
	_R90_D,
	_L90_E,
	_R90_E,
	_L90_F,
	_R90_F,
	_L180_A,
	_R180_A,
	_L180_B,
	_R180_B,
	_L180_C,
	_R180_C,
	_L180_D,
	_R180_D,
	_L180_E,
	_R180_E,
	_L180_F,
	_R180_F,
	_END
};

enum class ANIM_ORDINARY_STAIRS_PIVOT_TURN{
	_L90,
	_R90,
	_L180,
	_R180,
	_END
};

enum class ANIM_ORDINARY_STAIRS_WALK {
	_START,
	_LOOP,
	_TURNING_LOOP_FL,
	_TURNING_LOOP_FR,
	_END
};

enum class ANIM_ORDINARY_STANDUP {
	_FACE_DOWN_F,
	_FACE_DOWN_B,
	_FACE_DOWN_L,
	_FACE_DOWN_R,
	_FACE_UP_F,
	_FACE_UP_B,
	_FACE_UP_L,
	_FACE_UP_R,
	_END
};

enum class ANIM_ORDINARY_WALK {
	_START_A,
	_LOOP_A,
	_TURNING_LOOP_FL_A,
	_TURNING_LOOP_FR_A,

	_START_B,
	_LOOP_B,
	_TURNING_LOOP_FL_B,
	_TURNING_LOOP_FR_B,
	_LOOP_TO_B_WALK,

	_START_C,
	_LOOP_C,
	_TURNING_LOOP_FL_C,
	_TURNING_LOOP_FR_C,
	_LOOP_TO_C_WALK,

	_START_D,
	_LOOP_D,
	_TURNING_LOOP_FL_D,
	_TURNING_LOOP_FR_D,
	_LOOP_TO_D_WALK,

	_START_E,
	_LOOP_E,
	_TURNING_LOOP_FL_E,
	_TURNING_LOOP_FR_E,
	_LOOP_TO_E_WALK,

	_START_F,
	_LOOP_F,
	_TURNING_LOOP_FL_F,
	_TURNING_LOOP_FR_F,
	_LOOP_TO_F_WALK,

	//	Totter 넝마 주이
	_TOTTER_F_A,
	_TOTTER_F_B,
	_TOTTER_F_C,
	_TOTTER_F_D,
	_TOTTER_F_E,
	_TOTTER_F_F,

	_TURN_L_180A,
	_TURN_R_180A,
	_TURN_L_180B,
	_TURN_R_180B,
	_TURN_L_180C,
	_TURN_R_180C,
	_TURN_L_180D,
	_TURN_R_180D,
	_TURN_L_180E,
	_TURN_R_180E,
	_TURN_L_180F,
	_TURN_R_180F,
	_END
};

enum class ANIM_ORDINARY_WALK_LOSE {
	_ONES_FOOTING_START,
	_ONES_FOOTING_LOOP,
	_END
};

#pragma	endregion

#pragma region Sick

enum class ANIM_SICK_FLASHGRANADE {
	_START,
	_LOOP1,
	_END1,
	_LOOP2,
	_END2,
	_LOOP3,
	_END3,
	_FALLDOWN_FACEDOWN_LOOP,
	_FACEDOWN_START,
	_FACEDOWN_LOOP,
	_FACEUP_START,
	_FACEUP_LOOP,
	_EAT_START,
	_EAT_LOOP,
	_HOLD,
	_END
};

enum class ANIM_SICK_KNIFE {
	_HOLD_EM1,
	_HOLD_EM2,
	_END
};

#pragma endregion

#pragma region Undiscovered

enum class ANIM_UNDISCOVERED_CAGE {
	_LOOP_00,
	_END_00,
	_START_01,
	_LOOP_01,
	_START_02,
	_END_02,
	_END
};

enum class ANIM_UNDISCOVERED_CAPTURE {
	_LOOP,
	_BUS_00_LOOP,
	_BUS_00_START,
	_BUS_01_LOOP,
	_BUS_01_START,
	_END
};

enum class ANIM_UNDISCOVERED_CELLING_FALL {
	_WAIT_LOOP,
	_START,
	_HIDE_LOOP,
	_END
};

enum class ANIM_UNDISCOVERED_DEAD {
	_LOOP1,
	_START1,
	_START2,
	_LOOP2,
	_START3,
	_LOOP3,
	_START4,
	_LOOP4,
	_START5,
	_LOOP5,
	_LOOP6,
	_START6,
	_START7,
	_END
};

enum class ANIM_UNDISCOVERED_DEAD_POSE {
	_FACEDOWN1,
	_FACEDOWN2,
	_FACEDOWN3,
	_FACEUP1,
	_FACEUP2,
	_FACEUP3,
	_FACEUP4,
	_WALLDOWN1,
	_WALLDOWN2,
	_WALLUP1,
	_WALLUP2,
	_WALLUP3,
	_DESK1,
	_DESK2,
	_END
};

enum class ANIM_UNDISCOVERED_EAT {
	_FACEDOWN_LOOP1,
	_FACEDOWN_LOOP2,
	_FACEDOWN_LOOP3,
	_FACEDOWN_LOOP4,
	_FACEUP_LOOP1,
	_FACEUP_LOOP2,
	_END_F,
	_END_B,
	_END_L,
	_END
};

enum class ANIM_UNDISCOVERED_FANCE {
	_LOOP_01,
	_END_01,
	_START_02,
	_LOOP_02,
	_END_02,
	_START_03,
	_LOOP_03,
	_END_03,
	_END
};

enum class ANIM_UNDISCOVERED_HEADBANG {
	_LOOP,
	_END
};

enum class ANIM_UNDISCOVERED_LOUNGE {
	_LOOP_02,
	_END_02,
	_LOOP_03,
	_END_03,
	_END
};

enum class ANIM_UNDISCOVERED_PRISON {
	_LOOP_00,
	_END_00,
	_LOOP_01,
	_END
};

enum class ANIM_UNDISCOVERED_RAILING_FALL {
	_WAIT_LOOP,
	_END
};

enum class ANIM_UNDISCOVERED_STUND {
	_LOOP1,
	_LOOP2,
	_END
};

#pragma endregion

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
#pragma endregion