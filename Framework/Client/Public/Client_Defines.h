#pragma once


#include "GameInstance.h"
#include "Client_Enumsh.h"

namespace Client
{
	const unsigned int	g_iWinSizeX = 1600;
	const unsigned int	g_iWinSizeY = 900;
}


extern HWND g_hWnd;
extern HINSTANCE g_hInst;
extern Client::LEVEL g_Level;
using namespace std;
using namespace Client;
	
#define USE_UI
//#define USE_Player_Control
//#define USE_MapObject
//#define USE_MapInteractObject

typedef struct Tag_Prototype_Inform
{
	_bool		bAnim = { false };
	string		strModelPath = { ("") }; //��.bin ������ ��ġ
	wstring		wstrModelPrototypeName = { TEXT("") }; // Prototype_Component_Model_
	wstring		wstrGameObjectPrototypeName = { TEXT("") }; //Prototype_GameObject_
	string		strGameObjectPrototypeName = { ("") }; // �̰��� IMGUI�� ��û�ؼ� �ִ� ��

}PROTOTYPE_INFORM;