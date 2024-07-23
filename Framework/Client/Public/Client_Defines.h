#pragma once


#include "GameInstance.h"
#include "Client_Enumsh.h"

#define			HD_PLUS
//	#define			FHD
//	#define			QHD
//	#define			UHD

namespace Client
{
	/* Resolution */
#ifdef HD_PLUS
	const unsigned int	g_iWinSizeX = 1600;
	const unsigned int	g_iWinSizeY = 900;
#endif

#ifdef FHD
	const unsigned int	g_iWinSizeX = 1920;
	const unsigned int	g_iWinSizeY = 1080;
#endif

#ifdef QHD
	const unsigned int	g_iWinSizeX = 2560;
	const unsigned int	g_iWinSizeY = 1440;
#endif

#ifdef UHD
	const unsigned int	g_iWinSizeX = 3840;
	const unsigned int	g_iWinSizeY = 2160;
#endif
	//		static _int g_i44_005Type = 0; //저희 게임 디테일을 위해 죄송합니다- 예은

}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;
extern Client::LEVEL g_Level;
using namespace std;
using namespace Client;
	
#define MAP_NOTHING
//#define MAP_INTERACT
//#define MAP_TEST
//#define Map_NA
//	#define Map_Ye
//#define Map_TEST2

//#define TEST_ZOMBIE_MAINHALL

//#define NON_COLLISION_PROP
//	#define PROPS_COOKING

//#define UI_POS

//#define NO_COLLISTION

#define DISPLAY_FRAME
//	#define DISPLAY_NAVMESH
//	#define FRAME_LOCK

typedef struct Tag_Prototype_Inform
{
	_bool		bAnim = { false };
	string		strModelPath = { ("") }; 
	wstring		wstrModelPrototypeName = { TEXT("") }; 
	wstring		wstrGameObjectPrototypeName = { TEXT("") }; 
	string		strGameObjectPrototypeName = { ("") };

}PROTOTYPE_INFORM;

typedef struct Tag_Item_Recipe
{
	ITEM_NUMBER eCombination_Item;
	ITEM_NUMBER eResult_Item;
}ITEM_RECIPE;

#pragma region ANIM_BONE
#define ANIM_BONE_COUNT 10
#pragma endregion

#pragma region DECAL
#define DECAL
#pragma endregion
