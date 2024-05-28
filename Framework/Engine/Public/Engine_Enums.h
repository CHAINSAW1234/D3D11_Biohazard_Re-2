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
}