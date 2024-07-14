#pragma once

enum class ZOMBIE_START_TYPE { _OUT_DOOR, _DOOR_RUB, _CREEP, _IDLE, _RAG_DOLL, _END };

enum class BREAK_PART { 
	_R_UPPER_HUMEROUS = 0, _R_LOWER_HUMEROUS, _R_UPPER_RADIUS,	//	Non Lower Radius
	_L_UPPER_HUMEROUS = 4, _L_LOWER_HUMEROUS, _L_UPPER_RADIUS,	//	Non Lower Radius

	_R_UPPER_FEMUR = 8, _R_LOWER_FEMUR, _R_UPPER_TABIA, _R_LOWER_TABIA,
	_L_UPPER_FEMUR, _L_LOWER_FEMUR, _L_UPPER_TABIA, _L_LOWER_TABIA,

	_HEAD,

	_END 
};

enum class MESH_PART {
	_R_UPPER_HUMEROUS, _R_LOWER_HUMEROUS, _R_UPPER_RADIUS,	_R_LOWER_RADIUS,
	_L_UPPER_HUMEROUS, _L_LOWER_HUMEROUS, _L_UPPER_RADIUS,	_L_LOWER_RADIUS,

	_R_UPPER_FEMUR, _R_LOWER_FEMUR, _R_UPPER_TABIA, _R_LOWER_TABIA,
	_L_UPPER_FEMUR, _L_LOWER_FEMUR, _L_UPPER_TABIA, _L_LOWER_TABIA,

	_HEAD,

	_END
};

enum class ZOMBIE_BODY_TYPE { _MALE, _FEMALE, _MALE_BIG, _END };
enum class ZOMBIE_CLOTHES_TYPE { _SHIRTS, _HAT, _PANTS, _END };

enum class ZOMBIE_MALE_PANTS { _00, _01, _02, _03, _04, _05, _06, _END };
enum class ZOMBIE_MALE_FACE { _00, _01, _02, _03, _04, _05, _06, _07, _08, _09, _10, _END };
enum class ZOMBIE_MALE_SHIRTS { _00, _01, _02, _03, _04, _05, _06, _08, _09, _11, _12, _70, _72, _73, _81, _END };
enum class ZOMBIE_MALE_HAT { _00, _01, _END };

enum class ZOMBIE_FEMALE_PANTS { _00, _01, _02, _04, _END };
enum class ZOMBIE_FEMALE_FACE { _00, _01, _02, _03, _04, _END };
enum class ZOMBIE_FEMALE_SHIRTS { _00, _01, _02, _03, _04, _05, _END };

enum class ZOMBIE_MALE_BIG_PANTS { _00, _01, _02, _70, _END };
enum class ZOMBIE_MALE_BIG_FACE { _00, _01, _03, _04, _70, _END };
enum class ZOMBIE_MALE_BIG_SHIRTS { _00, _01, _02, _03, _70, _END };