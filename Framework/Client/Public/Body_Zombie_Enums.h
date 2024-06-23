#pragma once


enum class ZOMBIE_BODY_TYPE {
	BODY_MALE,
	BODY_FEMALE,
	BODY_HEAVY,
	BODY_END
};

enum class PLAYING_INDEX { INDEX_0, INDEX_1, INDEX_2, INDEX_3, INDEX_4, INDEX_5, INDEX_END };
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

#pragma endregion

#pragma region Damage

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

enum class ANIM_DAMAGE_DEFAULT {
	_FACEDOWN_ARM_L,
	_FACEDOWN_ARM_R,
	_FACEUP_ARM_L,
	_FACEUP_ARM_R,
	_END
};

enum class ANIM_DAMAGE_DOWN {
	_F_L,
	_B_L,
	_F_R,
	_B_R,
	_LOWSTATNCE_B,
	_FACEUP_F,
	_END
};

enum class ANIM_DAMAGE_ELECTRIC_SHOCK {
	_END_F,
	_FACE_UP_START,
	_FACE_UP_CANCEL,
	_FACE_DOWN_START,
	_FACE_DOWN_CANCEL,
	_END
};

enum class ANIM_DAMAGE_KNOCKBACK{
	_HEAD_F,
	_HEAD_FL,
	_HEAD_FR,
	_HEAD_B,
	_BODY_L,
	_BODY_R,
	_F,
	_END
};

enum class ANIM_DAMAGE_LOST {
	_B_F_L,
	_B_F_R,
	_C_F_L,
	_C_F_R,
	_A2_F_L,
	_A2_F_R,
	_C2_F_L,
	_C2_F_R,
	_D_F_L1,
	_D_F_L2,
	_D_F_R1,
	_D_F_R2,
	_C3_F_LL,
	_C3_F_LR,
	_C3_F_RR,
	_C3_F_RL,
	_END
};

enum class ANIM_DAMAGE_STAIRS_DOWN {
	_FACEDOWN1,
	_FACEDOWN2,
	_FACEUP1,
	_FACEUP2,
	_END
};

enum class ANIM_DAMAGE_STAIRS_UP {
	_FACEDOWN1,
	_FACEDOWN2,
	_FACEUP1,
	_FACEUP2,
	_END
};

enum class ANIM_DAMAGE_STUN {
	_HEAD_LEFT_SIDE_F,
	_BODY_F,
	_SHOULDERR_F,
	_LEG_L_F1,
	_LEG_L_F2,
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
	_TRIP_L_FOOT_UP,
	_TRIP_R_FOOT_UP,
	_L,
	_R,
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