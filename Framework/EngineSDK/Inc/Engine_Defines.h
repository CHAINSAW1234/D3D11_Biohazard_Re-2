#pragma once

#pragma warning (disable : 4251)
#pragma warning (disable : 26495)
#pragma warning (disable : 6297)

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXCollision.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

namespace Engine
{
	enum MOUSEKEYSTATE { DIMKS_LBUTTON, DIMKS_RBUTTON, DIMKS_WHEEL, DIMKS_X, DIMKS_END };
	enum MOUSEMOVESTATE { DIMMS_X, DIMMS_Y, DIMMS_WHEEL, DIMMS_END };
}

#include <physx/PxPhysics.h>
#include <physx/PxPhysicsAPI.h>
#include <physx/cooking/PxCooking.h>
using namespace physx;

#include "Effects11/d3dx11effect.h"
#include "DirectXTK/DDSTextureLoader.h"
#include "DirectXTK/WICTextureLoader.h"
#include "DirectXTK/ScreenGrab.h"
#include "DirectXTK/PrimitiveBatch.h"
#include "DirectXTK/VertexTypes.h"
#include "DirectXTK/Effects.h"

#include "DirectXTK/SpriteBatch.h"
#include "DirectXTK/SpriteFont.h"

/* Assimp */
#include "assimp\scene.h"
#include "assimp\Importer.hpp"
#include "assimp\postprocess.h"
#include "assimp\DefaultLogger.hpp"

#include <random>

using namespace DirectX;

#include <tchar.h> //fmod에서 사용하는 것 같음

#include "fmod.h"
#include "fmod.hpp"
#include "fmod_errors.h"

#include <io.h>

#pragma comment (lib, "fmod_vc.lib")

#include <algorithm>
#include <string>
#include <vector>
#include <list>
#include <stack>
#include <queue>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <random>

/* FileSystem */
#include <filesystem>
#include <iostream>
#include <cstdlib>

#include <fstream>

using namespace std;

namespace Engine
{
	const wstring g_strTransformTag = TEXT("Com_Transform");
	const wstring g_strModelTag = TEXT("Com_Model");
	const wstring g_strDirectionalTag = TEXT("LIGHT_SUN");
}

#define		VK_MAX			0xff

#define SOUND_MAX 1.0f
#define SOUND_MIN 0.0f
#define SOUND_DEFAULT 0.5f
#define SOUND_WEIGHT 0.1f
#define SOUND_CHANNEL_MAX 32

#include "Engine_Typedef.h"
#include "Engine_Macro.h"
#include "Engine_Function.h"
#include "Engine_Struct.h"
#include "Engine_Enums.h"

using namespace Engine;

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG

