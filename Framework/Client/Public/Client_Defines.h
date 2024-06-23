#pragma once


#include "GameInstance.h"
#include "Client_Enumsh.h"

namespace Client
{
	const unsigned int	g_iWinSizeX = 1600;
	const unsigned int	g_iWinSizeY = 900;

	enum LOCATION_MAP_VISIT
	{
		MAIN_HOLL,						/* 0. 메인 홀*/
		ENTRANCE,						/* 1. 입구*/
		RECEPT_ROOM,					/* 2. 리셉션 */
		RECORD_ROOM,					/* 3. 기록실*/
		OPERATION_ROOM,					/* 4. 작전실*/
		SAFE_DEPOSIT_BOX_ROOM,			/* 5. 안전 금고실*/
		WEST_OFFICE,					/* 6. 서쪽 사무실 */
		DARK_ROOM,						/* 7. 암실 */
		BOILER_HALLWAY,					/* 8. 보일러실 복도 */
		BOILER_ROOM,					/* 9. 보일러실 */
		RECORD_HALLWAY,					/* 10. 기록실 복도 */
		OPERATION_HALLWAY,				/* 11. 작전실 복도 */
		DARKROOM_HALLWAY,				/* 12. 암실 복도 */
		WEST_CNTRAL_HALLWAY,			/* 13. 서쪽 중앙 복도*/
		/* 14. 밖 */
		LOUNGE_HALLWAY,					/* 15. 휴게실 복도 */
		EAST_OFFICE,					/* 16. 동쪽 사무실 */
		LEFTSIDE_PRESSROOM_HALLWAY,		/* 17. 기자실 왼쪽 작은 방*/
		PRESS_ROOM,						/* 18. 기자실*/
		QUESTION_ROOM,					/* 19. 취조실 */
		OBSERVATION_ROOM,				/* 20. 관찰실 */
		REST_ROOM,						/* 21. 휴게실 */
		WASHROOM,						/* 22. 화장실*/
		SECURITY_OFFICE,				/* 23. 경비 사무실 */
		EAST_CENTRAL_HALLWAY,			/* 24. 동쪽 중앙 복도*/
		UP_PRESSROOM_HALLWAY,			/* 25. 기자실 위쪽 복도 */
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
	string		strModelPath = { ("") }; //모델.bin 파일의 위치
	wstring		wstrModelPrototypeName = { TEXT("") }; // Prototype_Component_Model_
	wstring		wstrGameObjectPrototypeName = { TEXT("") }; //Prototype_GameObject_
	string		strGameObjectPrototypeName = { ("") }; // 이것은 IMGUI가 멍청해서 있는 것

}PROTOTYPE_INFORM;

#pragma region ANIM_BONE
#define ANIM_BONE_COUNT 10
#pragma endregion