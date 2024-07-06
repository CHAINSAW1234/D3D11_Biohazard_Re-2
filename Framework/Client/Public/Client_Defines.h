#pragma once


#include "GameInstance.h"
#include "Client_Enumsh.h"

namespace Client
{
	const unsigned int	g_iWinSizeX = 1600;
	const unsigned int	g_iWinSizeY = 900;
	//static _int g_i44_005Type = 0; //저희 게임 디테일을 위해 죄송합니다- 예은

}


extern HWND g_hWnd;
extern HINSTANCE g_hInst;
extern Client::LEVEL g_Level;
using namespace std;
using namespace Client;
	
//#define MAP_NOTHING
//#define MAP_INTERACT
#define MAP_TEST

//#define NON_COLLISION_PROP
#define PROPS_COOKING


#define NO_COLLISTION

#define DISPLAY_FRAME
#define DISPLAY_NAVMESH
//#define FRAME_LOCK

typedef struct Tag_Prototype_Inform
{
	_bool		bAnim = { false };
	string		strModelPath = { ("") }; //모델.bin 파일의 위치
	wstring		wstrModelPrototypeName = { TEXT("") }; // Prototype_Component_Model_
	wstring		wstrGameObjectPrototypeName = { TEXT("") }; //Prototype_GameObject_
	string		strGameObjectPrototypeName = { ("") }; // 이것은 IMGUI가 멍청해서 있는 것

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
