#pragma once

namespace Engine
{
	/* For.API_Input */
	enum EKeyState { NONE, DOWN, PRESSING, UP };

	enum class SHADER_PASS_DEFERRED
	{
		PASS_DEBUG,
		PASS_DEBUG_CUBE,
		PASS_DEBUG_3D,
		PASS_COPY_DISCARD,
		PASS_SSAO,
		PASS_LIGHT_DIRECTIONAL,
		PASS_LIGHT_POINT,
		PASS_LIGHT_SPOT,
		PASS_LIGHT_RESULT,
		PASS_BLURX,
		PASS_BLURY,
		PASS_POST_PROCESSING_RESULT,
		PASS_RADIALBLUR,
		PASS_SSR,
		PASS_DOF, 
		PASS_DOF_BLURX, 
		PASS_DOF_BLURY,
		PASS_GODRAY,
		PASS_FXAA,
		PASS_BLOOM,
		PASS_SSD,
		PASS_BLURX_EFFECT,
		PASS_BLURY_EFFECT,
		PASS_BLURX_EFFECT_STRONG,
		PASS_BLURY_EFFECT_STRONG,
		PASS_END
	};

	enum Joint_Index
	{
		Neck = 167,
		Spine = 61,
		//Pelvis = 60,
		Shoulder_L = 109,
		Shoulder_R = 151,
		Elbow_L = 105,
		Elbos_R = 147,
		Acetabulum_L = 36,
		Acetabulum_R = 50,
		Knee_L = 29,
		knee_R = 43
	};

	enum SHADER_STATE { SSAO, SSR, DOF, VOLUMETRIC, FXAA, SHADER_STATE_END};

	enum COMPOSITE_NODE_TYPE {CNT_SELECTOR,CNT_SEQUENCE,CNT_DEFAULT, CNT_END};

	//For Physics_Controller
	enum COLLISION_CATEGORY 
	{
		CCT = 1 << 0,
		RAGDOLL = 1 << 1,
		COLLIDER = 1 << 2,
		STATIC_MESH = 1 << 3,
	};

	enum COLLIDER_TYPE 
	{
		HEAD = 1 << 0,
		CHEST = 1 << 1,
		PELVIS = 1 << 2,
		ARM_L = 1 << 3,
		ARM_R = 1 << 4,
		FOREARM_L = 1 << 5,
		FOREARM_R = 1 << 6,
		HAND_L = 1 << 7,
		HAND_R = 1 << 8,
		LEG_L = 1 << 9,
		LEG_R = 1 << 10,
		CALF_L = 1 << 11,
		CALF_R = 1 << 12,
		FOOT_L = 1 << 13,
		FOOT_R = 1 << 14,
		_END
	};

	enum RAGDOLL_TYPE
	{
		FULL = 1 << 0,
		LEFT_FOREARM = 1 << 1,
		LEFT_ARM = 1 << 2,
		RIGHT_FOREARM = 1 << 3,
		RIGHT_ARM = 1 << 4,
		LEFT_CALF = 1 << 5,
		LEFT_LEG = 1 << 6,
		RIGHT_CALF = 1 << 7,
		RIGHT_LEG = 1 << 8,
		BODY_DIVIDE = 1 << 9,
		TYPE_NONE = 1<<10
	};

	enum class MAP_FLOOR_TYPE { BASEMENT_2, BASEMENT_1, FLOOR_1, FLOOR_2, FLOOR_3, FLOOR_FREE, FLOOR_END };

}