#pragma once
#include "Client_Defines.h"
#include "Customize_UI.h"

BEGIN(Client)

class CMap_Manager abstract : public CCustomize_UI
{
public:
	enum class MAP_UI_TYPE {
		MAIN_MAP, MASK_MAP, FONT_MAP, BACKGROUND_MAP,
		WINDOW_MAP, DOOR_MAP, FONT_MASK_MAP, TARGET_MAP, TARGET_NOTIFY, MAP, ITEM_MAP, PLAYER_MAP,
		NAMELINE_MAP, SEARCH_TYPE_MAP, FLOOR_TYPE_MAP, END_MAP
	};

protected:
	enum class MAP_STATE_TYPE { NONE_STATE, SEARCH_STATE, SEARCH_CLEAR_STATE };


protected :
	CMap_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMap_Manager(const CMap_Manager& rhs);
	virtual ~CMap_Manager() = default;

protected:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public : /* Getter */
	MAP_FLOOR_TYPE			Get_Floor_Type()		{ return m_eFloorType;  } /* 배치 되어야 할 Floor Type 반환 */
	MAP_FLOOR_TYPE*			Get_ViewFloor_Type()	{ return &m_eCurrent_ViewFloor; } /* 배치 되어야 할 Floor Type 반환 */



protected :
	/* Fuction */
	CGameObject*			Find_MapPlayer();
	CGameObject*			Find_BackGround();

	void					Exception_Handle(); /* 예외 처리*/
	void					Transform_Adjustment();
	void					Transform_Control(_float fTimeDelta);

	void					Mouse_Pos(_float fTimeDelta);

protected :
	void					Rendering(_float fTimeDelta);
	void					Open_Map();
	void					Find_MapStateType();

	_bool					m_isTargetLimit = { false };
protected :
	MAP_UI_TYPE				m_eMapComponent_Type		= { MAP_UI_TYPE::END_MAP };
	_float					m_fOrigin_Blending			= {};
	_bool					m_isTransfrom_Setting		= { false }; /* z 값 세팅 */
	wstring					m_wstrFile					= { TEXT("") };


protected : /* Render 관련*/
	MAP_STATE_TYPE			m_eMapState					= { MAP_STATE_TYPE::NONE_STATE }; /* 현재 이 지역의 상태는 무엇인가?*/
	LOCATION_MAP_VISIT		m_eMap_Location				= { LOCATION_MAP_VISIT::LOCATION_MAP_VISIT_END }; /* 이 지역은 어떤 지역인가? */

	_bool					m_isPrevRender				= { false };


protected : /* Variable */
	/* 1. Player */
	class CPlayer_Map_UI*	m_pMapPlayer				= { nullptr };
	CTransform*				m_pMapPlayer_Transform		= { nullptr };

	/* 2. Map Inventory */
	class CTab_Window*		m_pTab_Window				= { nullptr };

	/*3. Floor */
	MAP_FLOOR_TYPE			m_eFloorType				= { MAP_FLOOR_TYPE::FLOOR_FREE }; /* 기본 타입 */
	MAP_FLOOR_TYPE			m_eCurrent_ViewFloor		= { MAP_FLOOR_TYPE::FLOOR_1 }; /* 현재 가리키고 있는 맵 : Player 가 가지고 있을 예정 */
	
	/* 4. Control */
	_bool					m_isMouse_Control			= { false }; /* 마우스로 당길 수 있는 객체 */



	_bool					m_isLastPosition = { false };
	_float4x4				m_vLastMatrix;
	_float					m_fMouseSensor = { 10.f };
	_float2					m_vPlayer_MovePos = {};

public:
	virtual CGameObject* Clone(void* pArg) override = 0;
	virtual void Free() override;
};

END;
