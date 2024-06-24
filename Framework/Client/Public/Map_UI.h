#pragma once
#include "Client_Defines.h"
#include "Customize_UI.h"

BEGIN(Client)

class CMap_UI final : public CCustomize_UI
{
private :
	enum class MAP_CHILD_TYPE { PARENT_MAP, BACKGROUND_MAP, LINE_MAP, END_MAP };

public:
	enum class MAP_STATE_TYPE { NONE_STATE, SEARCH_STATE, SEARCH_CLEAR_STATE };

private :
	enum class SUB_TARGET_TYPE { LEFT_TARGET, RIGHT_TARGET, UP_TARGET, DOWN_TARGET, END_TARGET };

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

private :
	void					Render_Condition();
	void					Search_TabWindow();

	void					Transform_Condition(_float fTimeDelta);
	void					Mouse_Pos(_float fTimeDelta);
	void					Transform_Adjustment();

	void					Map_Player_Control(_float fTimeDelta);

	void					Map_Target_Control(_float fTimeDelta);

	/* EX) */
	void					EX_ColorChange();


private :
	CTransform*				m_pPlayerTransform					= { nullptr };
	_float4					m_vPlayer_InitPosition				= {};
	_float4					m_vPlayer_InitPosition_Distance		= {};

private :
	_uint					m_iWhichChild						= { 0 };
	_float					fMouseSensor						= { 20.f };
	_uint					m_iCnt								= { 0 };

	_bool					m_isTransfrom_Setting				= { false };
	_bool					m_isGara							= { false };
	_bool					m_isPrevRender						= { false };
	_bool					m_isBlurBlending					= { false };

private : /* Target Main ฐüทร */
	CMap_UI*				m_pInMap_Player						= { nullptr };
	CTransform*				m_pInMap_PlayerTrans				= { nullptr };
	
	MAP_STATE_TYPE			m_eMapState							= { MAP_STATE_TYPE::NONE_STATE };
	class CTab_Window*		m_pTab_Window						= { nullptr };


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