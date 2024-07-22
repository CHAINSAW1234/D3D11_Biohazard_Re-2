#pragma once

enum class ZOMBIE_START_TYPE { 
	_OUT_DOOR, _DOOR_RUB, _CREEP, _IDLE, _RAG_DOLL, _HIDE_LOCKER, 
	_DEAD_FAKE1 /* 벽에 기대 앉은 모션 */, _DEAD_FAKE2 /* 벽에 기대 앉은 모션 */, _DEAD_FAKE3 /* 벽에 기대 앉은 모션 */, _DEAD_FAKE4 /* 책상에 누움*/,
	_END };

enum class BREAK_PART { 
	_R_UPPER_HUMEROUS = 0, _R_LOWER_HUMEROUS, _R_UPPER_RADIUS,	//	Non Lower Radius
	_L_UPPER_HUMEROUS = 4, _L_LOWER_HUMEROUS, _L_UPPER_RADIUS,	//	Non Lower Radius

	_R_UPPER_FEMUR = 8, _R_LOWER_FEMUR, _R_UPPER_TABIA, _R_LOWER_TABIA,
	_L_UPPER_FEMUR, _L_LOWER_FEMUR, _L_UPPER_TABIA, _L_LOWER_TABIA,

	_HEAD,

	_END 
};

enum class ZOMBIE_SOUND_TYPE {
	//	Effect
	_HIT, 
	_BITE_EF,

	//	Foot
	_FOOT,					
	_FOOT_CREEP,
	_DROP_BODY,					

	//	Voice
	_IDLE_MALE, _IDLE_FEMALE, _IDLE_MALE_BIG,											//	=> 평시에 내는 소리 갑자기 크르릉 거리거나 보통 정적임
	_MOVE_MALE, _MOVE_FEMALE, _MOVE_MALE_BIG,											//	=> 플레이어를 향해 오면서 위협하듯 크르릉 하는소리
	_STUN_MALE, _STUN_FEMALE, _STUN_MALE_BIG,											//	=> 맞고 살짝 밀려날때 내는 소리들 으에!! 윽.. 이런 느낌? ( 우리 게임상 권총으로 쐇을때 한번씩 밀리는 모션에서 낼 소리 )
	_KNOCKBACK_MALE, _KNOCKBACK_FEMALE, _KNOCKBACK_MALE_BIG,							//	=> 맞고 많이 밀려날때 내는 소리들 ( 우리 게임상으로 샷건으로 쐇을때 크게 밀리는 모션에서 낼 소리 )

	_BITE_CREEP_MALE, _BITE_CREEP_FEMALE, _BITE_CREEP_MALE_BIG,							//	=> 땅바닥 기는중에 무는 모션에서 내는 소리 (덮치기 시작때 짧은... )
	_BITE_CREEP_REJECT_MALE, _BITE_CREEP_REJECT_FEMALE, _BITE_CREEP_REJECT_MALE_BIG,	//	=> 땅바닥 기는중에 무는 모션에서 플레이어가 죽지않고서 다시쳐내는 모션에서 내는소리 ( 우리게임에서 물리고 발로 걷어차는모션 )
	_BITE_CREEP_KILL_MALE, _BITE_CREEP_KILL_FEMALE, _BITE_CREEP_KILL_MALE_BIG,			//	=> 땅바닥 기는중에 물고 플레이어 완전히 죽이는 모션

	_BITE_MALE, _BITE_FEMALE, _BITE_MALE_BIG,											//	=> 서있는 중에 무는 모션 ( 시작 )
	_BITE_REJECT_MALE, _BITE_REJECT_FEMALE, _BITE_REJECT_MALE_BIG,						//	=> 서있는 중에 무는 모션에서 플레이어가 죽지않고서 다시 쳐내는 모션
	_BITE_KILL_MALE, _BITE_KILL_FEMALE, _BITE_KILL_MALE_BIG,							//	=> 서있는 중에 물고 플레이어 완전히 죽이는 모션

	_HOLD_MALE, _HOLD_FEMALE, _HOLD_MALE_BIG,											//	=> 물기 모션전에 좀비가 덮치려고 시도하는 모션 

	_STAND_UP_MALE, _STAND_UP_FEMALE, _STAND_UP_MALE_BIG,								//	=> 좀비가 바닥에서 일어나는 모션 => 누워있는상태에서 일어남으로 전환 보통 끄으으으응....

	//	Cloth
	_STAND_UP_CLOTH,				//	=> 일어날때 옷 쓸리는 소리들...
	_IN_WINDOW_CLOTH,				//	=> 창문 넘을때 옷쓸리는 소리

	//	Interact
	_KNOCK_WINDOW, _BREAK_WINDOW,		
	_KNOCK_DOOR, _OPEN_DOOR,
	

	//	Break
	_BREAK_HEAD,
	_BREAK_OTHER, 

	_END
};

enum class ZOMBIE_SOUND_CH {
	_VOICE, _CLOTH, _FOOT, _EF, _INTERACT, _BREAK,_HEAD_BREAK,_BITE_DROP,_BLOOD_0, _BLOOD_1, _BLOOD_2, _BLOOD_3, _BLOOD_4, _BLOOD_5, _BLOOD_6,
	_BLOOD_7, _BLOOD_8, _BLOOD_9,_END,
};

enum class BODY_MESH_PART {
	_R_UPPER_HUMEROUS, _R_LOWER_HUMEROUS, _R_UPPER_RADIUS,	_R_LOWER_RADIUS,
	_L_UPPER_HUMEROUS, _L_LOWER_HUMEROUS, _L_UPPER_RADIUS,	_L_LOWER_RADIUS,

	_R_UPPER_FEMUR, _R_LOWER_FEMUR, _R_UPPER_TABIA, _R_LOWER_TABIA,
	_L_UPPER_FEMUR, _L_LOWER_FEMUR, _L_UPPER_TABIA, _L_LOWER_TABIA,

	_HEAD,

	_END
};

enum class SHIRT_MESH_PART {
	_R_ARM, _L_ARM, _BODY, _INNER, _END
};

enum class PANTS_MESH_PART {
	_R_LEG, _R_SHOES, _L_LEG, _L_SHOES, _BODY, _END
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