#pragma once

#pragma warning( disable : 4717 )
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
	enum EASING_TYPE {
		Ease_InQuad = 0, Ease_OutQuad, Ease_InOutQuad, Ease_InCubic, Ease_OutCubic,
		Ease_InOutCubic = 5, Ease_InQuart, Ease_OutQuart, Ease_InOutQuart, Ease_InQuint,
		Ease_OutQuint = 10, Ease_InOutQuint, Ease_InSine, Ease_OutSine, Ease_InOutSine,
		Ease_InExpo = 15, Ease_OutExpo, Ease_InOutExpo, Ease_InCirc, Ease_OutCirc,
		Ease_InOutCirc = 20, Ease_Linear, Ease_InBounce, Ease_OutBounce, Ease_InOutBounce,
		Ease_InBack = 25, Ease_OutBack, Ease_InOutBack, Ease_InElastic,
		Ease_OutElastic = 30, Ease_InOutElastic, Ease_END
	};
}

#include <physx/PxPhysics.h>
#include <physx/PxPhysicsAPI.h>
#include <physx/cooking/PxCooking.h>
#include <physx/extensions/PxTetMakerExt.h>
#include <physx/extensions/PxSoftBodyExt.h>
#include <physx/extensions/PxRemeshingExt.h>
#include <physx/gpu/PxPhysicsGpu.h>
#include <physx/gpu/PxGpu.h>
#include "physx/cudamanager/PxCudaContextManager.h"
#include "physx/cudamanager/PxCudaContext.h"
#include "physx/extensions/PxParticleExt.h"
#include "physx/PxParticleBuffer.h"
#include "physx/omnipvd/PxOmniPvd.h"
#include "physx/extensions/PxParticleClothCooker.h"
#include "physx/PxConstraint.h"
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


// SimpleMath
#include <DirectXTK/SimpleMath.h>

/* FileSystem */
#include <filesystem>
#include <iostream>
#include <cstdlib>

#include <fstream>

/*rapidjson*/
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
//#include <rapidjson/stringbuffer.h>


/* ImGui */
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

using namespace std;

namespace Engine
{
	const wstring g_strTransformTag = TEXT("Com_Transform");
	const wstring g_strModelTag = TEXT("Com_Model");
	const wstring g_strCameraTag = TEXT("Layer_ZZZCamera");
	const wstring g_strDirectionalTag = TEXT("LIGHT_SUN");
	const wstring g_strFontTag = TEXT("Font_Default");
}

#define		VK_MAX			0xff

#define SOUND_MAX 1.0f
#define SOUND_MIN 0.0f
#define SOUND_DEFAULT 0.5f
#define SOUND_WEIGHT 0.1f
#define SOUND_CHANNEL_MAX 32

//For Ragdoll
#define MAX_BONES 512

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


//For Thread Pool
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#pragma region Physics Defines

#define CONTROLLER_GROUND_GAP 1.1f
#define CONTROLLER_GROUND_GAP_ZOMBIE 0.95f

#define CONNECT_PVD
#pragma endregion

#pragma region Culling region

#define CULLING_DISTANCE 45.f

#pragma endregion

#pragma region Animation LOD

#define ANIMATION_LOD

#pragma endregion

#pragma region Skinned Mesh Decal

#define SKINNING_THREAD_GROUP_SIZE 64
#define RAYCAST_THREAD_GROUP_SIZE 64 
#define CALC_DECAL_MAP_THREAD_GROUP_SIZE 64 

#pragma endregion

#pragma region Sound 2D

#define SOUND_CHANNEL_SIZE 1024

#pragma endregion