#pragma once

namespace Client
{
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };

	enum class SHADER_PASS_VTXANIMMODEL {
		PASS_DEFAULT,
		PASS_DEFAULT_ATOS,
		PASS_DISSOLVE,
		PASS_LIGHTDEPTH,
		PASS_EMISSIVE,
		PASS_END
	};
}
