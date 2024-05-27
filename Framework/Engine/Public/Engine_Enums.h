#pragma once

namespace Engine
{
	/* For.API_Input */
	enum EKeyState { NONE, DOWN, PRESSING, UP };

	enum class SHADER_PASS_DEFERRED
	{
		PASS_DEBUG,
		PASS_DEBUG_CUBE,
		PASS_LIGHT_DIRECTIONAL,
		PASS_LIGHT_POINT,
		PASS_LIGHT_SPOT,
		PASS_LIGHT_RESULT,
		PASS_EMISSIVE,
		PASS_BLOOM,
		PASS_POST_PROCESSING,
		PASS_POST_PROCESSING_RESULT,
		PASS_RADIALBLUR,
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
}