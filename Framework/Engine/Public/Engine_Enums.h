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
		PASS_EMISSIVE,
		PASS_MOTIONBLUR,
		PASS_BLURX,
		PASS_BLURY,
		PASS_POST_PROCESSING,
		PASS_POST_PROCESSING_RESULT,
		PASS_RADIALBLUR,
		PASS_SSR,
		PASS_DOF, 
		PASS_DOF_BLURX, 
		PASS_DOF_BLURY,
		PASS_GODRAY,
		PASS_FXAA,
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

	enum SHADER_STATE { SSAO, MOTION_BLUR, SSR, DOF, VOLUMETRIC, FXAA, SHADER_STATE_END};

	enum COMPOSITE_NODE_TYPE {CNT_SELECTOR,CNT_SEQUENCE,CNT_DEFAULT, CNT_END};

	//For Physics_Controller
	enum COLLISION_CATEGORY {
		CCT = 1 << 0,
		RAGDOLL = 1 << 1,
		COLLIDER = 1 << 2,
	};

	enum COLLIDER_TYPE {
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
	};
}