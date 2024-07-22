#pragma once

/* For.Status */
#define	STATUS_ZOMBIE_DEFAULT_RECOGNITION_DISTANCE		3.5f		
#define	STATUS_ZOMBIE_MAX_RECOGNITION_DISTANCE			6.f
#define STATUS_ZOMBIE_MAX_RECOGNITION_TIME				5.f
#define STATUS_ZOMBIE_NEED_RECOGNITION_TIME				2.f

#define	STATUS_ZOMBIE_TRY_HOLD_TIME						1.5f
#define	STATUS_ZOMBIE_TRY_HOLD_RANGE					2.f
#define	STATUS_ZOMBIE_BITE_RANGE						1.f

#define	STATUS_ZOMBIE_VIEW_ANGLE						XMConvertToRadians(60.f)
#define STATUS_ZOMBIE_MAX_MOVE_ANGLE					XMConvertToRadians(120.f)

#define	STATUS_ZOMBIE_HEALTH							100.f
#define	STATUS_ZOMBIE_ATTACK							1.f

#define STATUS_ZOMBIE_LIGHTLY_HOLD_RANGE				1.f
#define STATUS_ZOMBIE_TRY_LIGHTLY_HOLD_TIME				0.5f

#define STATUS_ZOMBIE_TRY_STANDUP_TIME					2.f

#define STATUS_ZOMBIE_STAMINA							50.f
#define STATUS_ZOMBIE_STAMINA_MAX						150.f
#define STATUS_ZOMBIE_STAMINA_CHARGING_PER_SEC			3.f

/* For.Use Stamina */
#define ZOMBIE_NEED_STAMINA_BITE						50.f
#define ZOMBIE_NEED_STAMINA_STANDUP						30.f
#define ZOMBIE_NEED_STAMINA_TURN_OVER					20.f


/* For.Zombie Sound */
#pragma region VOICE
//	IDLE
#define ZOMBIE_NUM_SOUND_IDLE_MALE						3		
#define ZOMBIE_NUM_SOUND_IDLE_FEMALE					8		
#define ZOMBIE_NUM_SOUND_IDLE_MALE_BIG					1		//	미설정

#define ZOMBIE_MIN_VOLUME_IDLE							0.3f
#define ZOMBIE_MAX_VOLUME_IDLE							0.5f

//	MOVE
#define ZOMBIE_NUM_SOUND_MOVE_MALE						4
#define ZOMBIE_NUM_SOUND_MOVE_FEMALE					6	
#define ZOMBIE_NUM_SOUND_MOVE_MALE_BIG					1		//	미설정

#define ZOMBIE_MIN_VOLUME_MOVE							0.5f
#define ZOMBIE_MAX_VOLUME_MOVE							0.7f

//	HOLD
#define ZOMBIE_NUM_SOUND_HOLD_MALE						7
#define ZOMBIE_NUM_SOUND_HOLD_FEMALE					5
#define ZOMBIE_NUM_SOUND_HOLD_MALE_BIG					1		//	미설정

#define ZOMBIE_MIN_VOLUME_HOLD							0.5f
#define ZOMBIE_MAX_VOLUME_HOLD							0.7f

//	BITE
#define ZOMBIE_NUM_SOUND_BITE_MALE						1		//	미설정
#define ZOMBIE_NUM_SOUND_BITE_FEMALE					1		//	미설정	
#define ZOMBIE_NUM_SOUND_BITE_MALE_BIG					1		//	미설정

#define ZOMBIE_MIN_VOLUME_BITE							0.5f	//	미설정
#define ZOMBIE_MAX_VOLUME_BITE							0.7f	//	미설정

//	BITE_REJECT
#define ZOMBIE_NUM_SOUND_BITE_REJECT_MALE				4			
#define ZOMBIE_NUM_SOUND_BITE_REJECT_FEMALE				1		
#define ZOMBIE_NUM_SOUND_BITE_REJECT_MALE_BIG			1		//	미설정

#define ZOMBIE_MIN_VOLUME_BITE_REJECT					0.5f
#define ZOMBIE_MAX_VOLUME_BITE_REJECT					0.7f

//	BITE_KILL
#define ZOMBIE_NUM_SOUND_BITE_KILL_MALE					1		//	미설정	
#define ZOMBIE_NUM_SOUND_BITE_KILL_FEMALE				1		//	미설정
#define ZOMBIE_NUM_SOUND_BITE_KILL_MALE_BIG				1		//	미설정

#define ZOMBIE_MIN_VOLUME_BITE_KILL						0.5f	//	미설정
#define ZOMBIE_MAX_VOLUME_BITE_KILL						0.7f	//	미설정

//	BITE_CREEP
#define ZOMBIE_NUM_SOUND_BITE_CREEP_MALE				1		//	미설정
#define ZOMBIE_NUM_SOUND_BITE_CREEP_FEMALE				1		//	미설정	
#define ZOMBIE_NUM_SOUND_BITE_CREEP_MALE_BIG			1		//	미설정

#define ZOMBIE_MIN_VOLUME_BITE_CREEP					0.5f	//	미설정
#define ZOMBIE_MAX_VOLUME_BITE_CREEP					0.7f	//	미설정

//	BITE_CREEP_REJECT
#define ZOMBIE_NUM_SOUND_BITE_CREEP_REJECT_MALE			1		//	미설정	
#define ZOMBIE_NUM_SOUND_BITE_CREEP_REJECT_FEMALE		1		//	미설정
#define ZOMBIE_NUM_SOUND_BITE_CREEP_REJECT_MALE_BIG		1		//	미설정

#define ZOMBIE_MIN_VOLUME_BITE_CREEP_REJECT				0.5f	//	미설정 
#define ZOMBIE_MAX_VOLUME_BITE_CREEP_REJECT				0.7f	//	미설정 

//	BITE_CREEP_KILL
#define ZOMBIE_NUM_SOUND_BITE_CREEP_KILL_MALE			1		//	미설정	
#define ZOMBIE_NUM_SOUND_BITE_CREEP_KILL_FEMALE			1		//	미설정
#define ZOMBIE_NUM_SOUND_BITE_CREEP_KILL_MALE_BIG		1		//	미설정

#define ZOMBIE_MIN_VOLUME_BITE_CREEP_KILL				0.5f	//	미설정 
#define ZOMBIE_MAX_VOLUME_BITE_CREEP_KILL				0.7f	//	미설정 

//	KNOCK_BACK
#define ZOMBIE_NUM_SOUND_KNOCKBACK_MALE					1		//	미설정	
#define ZOMBIE_NUM_SOUND_KNOCKBACK_FEMALE				1		//	미설정		
#define ZOMBIE_NUM_SOUND_KNOCKBACK_MALE_BIG				1		//	미설정

#define ZOMBIE_MIN_VOLUME_KNOCKBACK						0.5f	//	미설정 
#define ZOMBIE_MAX_VOLUME_KNOCKBACK						0.7f	//	미설정 

//	STUN
#define ZOMBIE_NUM_SOUND_STUN_MALE						1		//	미설정	
#define ZOMBIE_NUM_SOUND_STUN_FEMALE					1		//	미설정		
#define ZOMBIE_NUM_SOUND_STUN_MALE_BIG					1		//	미설정

#define ZOMBIE_MIN_VOLUME_STUN							0.5f	//	미설정 
#define ZOMBIE_MAX_VOLUME_STUN							0.7f	//	미설정 

//	STAND_UP
#define ZOMBIE_NUM_SOUND_STAND_UP_MALE					1		//	미설정	
#define ZOMBIE_NUM_SOUND_STAND_UP_FEMALE				1		//	미설정		
#define ZOMBIE_NUM_SOUND_STAND_UP_MALE_BIG				1		//	미설정

#define ZOMBIE_MIN_VOLUME_STAND_UP						0.5f	//	미설정 
#define ZOMBIE_MAX_VOLUME_STAND_UP						0.7f	//	미설정 

#pragma endregion

#pragma region EF
/* EF */
#define ZOMBIE_NUM_SOUND_HIT							36
#define ZOMBIE_MIN_VOLUME_HIT							0.3f
#define ZOMBIE_MAX_VOLUME_HIT							0.7f

#pragma endregion

#pragma region BREAK

#define ZOMBIE_NUM_SOUND_BREAK_HEAD						10
#define ZOMBIE_NUM_SOUND_BREAK_OTHER					15

#define ZOMBIE_VOLUME_BREAK_HEAD						0.6f
#define ZOMBIE_VOLUME_BREAK_OTHER						0.6f

#pragma endregion

#pragma region INTERACT
/* Interact */
#define ZOMBIE_NUM_SOUND_KNOCK_WINDOW					6		
#define ZOMBIE_NUM_SOUND_BREAK_WINDOW					0		//	미설정 => 윈도우 자체에서 재생
#define ZOMBIE_NUM_SOUND_KNOCK_DOOR						4		
#define ZOMBIE_NUM_SOUND_OPEN_DOOR						7		

#define ZOMBIE_VOLUME_INTERACT							0.6f	//	미설정

#pragma endregion

#pragma region FOOT

/* Foot */
#define ZOMBIE_NUM_SOUND_FOOT							12
#define ZOMBIE_NUM_SOUND_FOOT_CREEP						1
#define ZOMBIE_MIN_VOLUME_FOOT							0.2f
#define ZOMBIE_MAX_VOLUME_FOOT							0.5f

#define ZOMBIE_NUM_SOUND_DROP_BODY						3
#define ZOMBIE_MIN_VOLUME_DROP_BODY						0.6f	
#define ZOMBIE_MAX_VOLUME_DROP_BODY						0.6f	

#pragma endregion