#pragma once


#include "GameInstance.h"
#include "Client_Enumsh.h"

namespace Client
{
	const unsigned int	g_iWinSizeX = 1920;
	const unsigned int	g_iWinSizeY = 1080;
}


extern HWND g_hWnd;
extern HINSTANCE g_hInst;

using namespace std;
using namespace Client;



typedef struct Tag_Prototype_Inform
{
	_bool			bAnim = { false };
	string		strModelPath = { ("") }; //모델.bin 파일의 위치
	wstring		wstrModelPrototypeName = { TEXT("") }; // Prototype_Component_Model_
	wstring		wstrGameObjectPrototypeName = { TEXT("") }; //Prototype_GameObject_
	string		strGameObjectPrototypeName = { ("") }; // 이것은 IMGUI가 멍청해서 있는 것

}PROTOTYPE_INFORM;

