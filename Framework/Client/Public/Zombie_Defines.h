#pragma once

/* For.Status */
#define	STATUS_ZOMBIE_DEFAULT_RECOGNITION_DISTANCE		5.f
#define	STATUS_ZOMBIE_MAX_RECOGNITION_DISTANCE			7.f
#define STATUS_ZOMBIE_MAX_RECOGNITION_TIME				5.f

#define	STATUS_ZOMBIE_TRY_HOLD_TIME						1.5f
#define	STATUS_ZOMBIE_TRY_HOLD_RANGE					3.f
#define	STATUS_ZOMBIE_BITE_RANGE						1.f

#define	STATUS_ZOMBIE_VIEW_ANGLE						XMConvertToRadians(180.f)

#define	STATUS_ZOMBIE_HEALTH							100.f
#define	STATUS_ZOMBIE_ATTACK							1.f

#define STATUS_ZOMBIE_LIGHTLY_HOLD_RANGE				1.f
#define STATUS_ZOMBIE_TRY_LIGHTLY_HOLD_TIME				0.5f

#define STATUS_ZOMBIE_TRY_STANDUP_TIME					2.f

#define STATUS_ZOMBIE_STAMINA							50.f
#define STATUS_ZOMBIE_STAMINA_MAX						150.f
#define STATUS_ZOMBIE_STAMINA_CHARGING_PER_SEC			3.f

/* For.Use Stamina */
#define ZOMBIE_NEED_STAMINA_BITE						30.f
#define ZOMBIE_NEED_STAMINA_STANDUP						80.f
#define ZOMBIE_NEED_STAMINA_TURN_OVER					20.f