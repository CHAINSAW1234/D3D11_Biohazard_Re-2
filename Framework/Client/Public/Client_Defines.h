#pragma once


#include "GameInstance.h"
#include "Client_Enumsh.h"

namespace Client
{
	const unsigned int	g_iWinSizeX = 1600;
	const unsigned int	g_iWinSizeY = 900;

	enum LOCATION_MAP_VISIT
	{
		MAIN_HOLL,						/* 0. ���� Ȧ*/
		ENTRANCE,						/* 1. �Ա�*/
		RECEPT_ROOM,					/* 2. ������ */
		RECORD_ROOM,					/* 3. ��Ͻ�*/
		OPERATION_ROOM,					/* 4. ������*/
		SAFE_DEPOSIT_BOX_ROOM,			/* 5. ���� �ݰ��*/
		WEST_OFFICE,					/* 6. ���� �繫�� */
		DARK_ROOM,						/* 7. �Ͻ� */
		BOILER_HALLWAY,					/* 8. ���Ϸ��� ���� */
		BOILER_ROOM,					/* 9. ���Ϸ��� */
		RECORD_HALLWAY,					/* 10. ��Ͻ� ���� */
		OPERATION_HALLWAY,				/* 11. ������ ���� */
		DARKROOM_HALLWAY,				/* 12. �Ͻ� ���� */
		WEST_CNTRAL_HALLWAY,			/* 13. ���� �߾� ����*/
		/* 14. �� */
		LOUNGE_HALLWAY,					/* 15. �ްԽ� ���� */
		EAST_OFFICE,					/* 16. ���� �繫�� */
		LEFTSIDE_PRESSROOM_HALLWAY,		/* 17. ���ڽ� ���� ���� ��*/
		PRESS_ROOM,						/* 18. ���ڽ�*/
		QUESTION_ROOM,					/* 19. ������ */
		OBSERVATION_ROOM,				/* 20. ������ */
		REST_ROOM,						/* 21. �ްԽ� */
		WASHROOM,						/* 22. ȭ���*/
		SECURITY_OFFICE,				/* 23. ��� �繫�� */
		EAST_CENTRAL_HALLWAY,			/* 24. ���� �߾� ����*/
		UP_PRESSROOM_HALLWAY,			/* 25. ���ڽ� ���� ���� */
		LOCATION_MAP_VISIT_END
	};
}


extern HWND g_hWnd;
extern HINSTANCE g_hInst;
extern Client::LEVEL g_Level;
using namespace std;
using namespace Client;
	
//#define MAP_NOTHING
//#define MAP_JUSTMAP
#define MAP_INTERACT

//#define NON_COLLISION_PROP
#define PROPS_COOKING

#define DISPLAY_FRAME
#define DISPLAY_NAVMESH
//	#define FRAME_LOCK

typedef struct Tag_Prototype_Inform
{
	_bool		bAnim = { false };
	string		strModelPath = { ("") }; //��.bin ������ ��ġ
	wstring		wstrModelPrototypeName = { TEXT("") }; // Prototype_Component_Model_
	wstring		wstrGameObjectPrototypeName = { TEXT("") }; //Prototype_GameObject_
	string		strGameObjectPrototypeName = { ("") }; // �̰��� IMGUI�� ��û�ؼ� �ִ� ��

}PROTOTYPE_INFORM;

#pragma region ANIM_BONE
#define ANIM_BONE_COUNT 10
#pragma endregion