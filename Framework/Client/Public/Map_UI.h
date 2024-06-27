#pragma once
#include "Client_Defines.h"
#include "Customize_UI.h"

BEGIN(Client)

class CMap_UI final : public CCustomize_UI
{
private :
	enum class MAP_CHILD_TYPE { PARENT_MAP, BACKGROUND_MAP, LINE_MAP, END_MAP };

public:
	enum class MAP_UI_TYPE { MAIN_MAP, MASK_MAP, FONT_MAP, BACKGROUND_MAP, WINDOW_MAP, DOOR_MAP, FONT_MASK_MAP, TARGET_MAP, ITEM_MAP, PLAYER_MAP, NAMELINE_MAP, SEARCH_TYPE_MAP, FLOOR_TYPE_MAP, END_MAP };
	enum class MAP_STATE_TYPE { NONE_STATE, SEARCH_STATE, SEARCH_CLEAR_STATE };
	enum class SUB_TARGET_TYPE { LEFT_TARGET, RIGHT_TARGET, UP_TARGET, DOWN_TARGET, END_TARGET };
	enum class MAP_FLOOR_TYPE { BASEMENT_2, BASEMENT_1, FLOOR_1, FLOOR_2, FLOOR_3, FLOOR_FREE, FLOOR_END };

private:
	CMap_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMap_UI(const CMap_UI& rhs);
	virtual ~CMap_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public :
	void					Search_Map_Type(MAP_STATE_TYPE _searType, LOCATION_MAP_VISIT _mapType);
	void					Change_Search_Type(MAP_STATE_TYPE _searType, CMap_UI* pMap);

private :
	void					Find_InMap_Player();
	void					Find_MapStateType(CUSTOM_UI_DESC* CustomUIDesc); /* Map Type 지정 */
	void					Floor_Sort();
private :
	void					FloorType_Search();
	void					Render_Condition(_float fTimeDelta);
	void					Search_TabWindow();


private :
	void					Transform_Condition(_float fTimeDelta);
	void					Mouse_Pos(_float fTimeDelta);
	void					Transform_Adjustment();
	void					Player_Transform(_float fTimeDelta);

private:
	void					Map_Player_Control(_float fTimeDelta);
	void					Map_Target_Control(_float fTimeDelta);

	/* EX) */
private:
	void					EX_ColorChange();

private : /* Init */
	wstring					m_wstrFile							= { TEXT("") };
	_uint					m_iWhichChild						= { 0 };

	/* EX */
	_uint					m_iCnt								= { 0 };
	_bool					m_isGara							= { false };

private :
	MAP_UI_TYPE				m_eMapComponent_Type				= { MAP_UI_TYPE::END_MAP };

	/* 층수 */
	MAP_FLOOR_TYPE			m_eFloorType						= { MAP_FLOOR_TYPE::FLOOR_FREE };	/* 맵에서 사용할 것*/
	MAP_FLOOR_TYPE			m_eCurrent_Floor					= { MAP_FLOOR_TYPE::FLOOR_1 };		/* 현재 가리키고 있는 맵 */
	MAP_FLOOR_TYPE			m_eCurrent_PlayerFloor				= { MAP_FLOOR_TYPE::FLOOR_1 };		/* 현재 플레이어가 존재하는 곳 */
	MAP_FLOOR_TYPE			m_eSelect_Floor						= { MAP_FLOOR_TYPE::FLOOR_FREE };		/* 현재 선택된 곳 */


	/* 현재 진행 상황 */
	MAP_STATE_TYPE			m_eMapState							= { MAP_STATE_TYPE::NONE_STATE };

	
private : /* Mouse Move*/
	_float					m_fMouseSensor						= { 15.f };

	_bool					m_isTransfrom_Setting				= { false };
	_bool					m_isPrevRender						= { false }; /* Player Target 객체에도 처음 플레이어를 찾기 위해 사용함*/
	_bool					m_isBlurBlending					= { false };

private : /* Player */
	/* Map Player */
	CMap_UI*				m_pInMap_Player						= { nullptr };
	CTransform*				m_pInMap_PlayerTrans				= { nullptr };
	class CTab_Window*		m_pTab_Window						= { nullptr };

	/* InGame Player */
	CTransform*				m_pPlayerTransform					= { nullptr };
	_float4					m_vPlayer_MovePos					= {};
	_float4					m_vPlayer_InitPos					= {};

private : /* Target Sub */
	SUB_TARGET_TYPE			m_eSubTarget_Type					= { SUB_TARGET_TYPE::END_TARGET };		
	CMap_UI*				m_pTarget_Main						= { nullptr }; // Map Player
	CTransform*				m_pTarget_Transform					= { nullptr }; // Map Player
	_float					m_fTarget_Distance					= {};

public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END;