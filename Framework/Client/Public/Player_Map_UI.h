#pragma once
#include "Map_Manager.h"

BEGIN(Client)

class CPlayer_Map_UI final : public CMap_Manager
{
private:
	CPlayer_Map_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Map_UI(const CPlayer_Map_UI& rhs);
	virtual ~CPlayer_Map_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public :
	_bool*					Get_PlayerFloorSetting() { return &m_isPlayer_FloorSetting; }


private:
	void					Rendering();
	void					Open_Map();
	_bool					IsDistanceMeasured_Completely(_bool _find);

	void					Map_Player_Control(_float fTimeDelta);
	void					Map_Player_Moving(_float fTimeDelta);
	void					Map_Player_Setting();


private:
	CTransform*				m_pPlayerTransform		= { nullptr };
	MAP_FLOOR_TYPE			m_ePlayer_Floor			= { MAP_FLOOR_TYPE::FLOOR_1 };

	_bool					m_isBlurBlending		= { false };
	_float2					m_fCurrent_ModelScaled	= {}; /* ¸ðµ¨ ¸Ê ½ºÄÉÀÏ */

private :
	/* Player Position */
	_float4					m_vPlayer_MovePos		= {};
	_float4					m_vPlayer_InitPos		= {}; 
	_float4					m_vBackGround_Center	= {};

	class CFloor_Map_UI*	m_FloorSearch_Obj		= { nullptr };

private :
	_bool					m_isPlayer_FloorSetting = { false };
	_float4					m_fOriginPos = {};

public:
	static CMap_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
