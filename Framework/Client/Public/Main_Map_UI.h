#pragma once
#include "Map_Manager.h"

BEGIN(Client)

class CMain_Map_UI final : public CMap_Manager
{
private:
	enum class MAP_CHILD_TYPE { PARENT_MAP, BACKGROUND_MAP, LINE_MAP, END_MAP };

private:
	CMain_Map_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMain_Map_UI(const CMain_Map_UI& rhs);
	virtual ~CMain_Map_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public :
	_bool*		Distance_End()				{ return &m_isMainEnd;  }
	_vector*	Player_Between_Distance()	{ return &m_vMapOpen_Player_Distance; }

private :	
	void				Change_Search_Type(MAP_STATE_TYPE _searType);
	void				Region_Type();

	void				Search_Map_Type(MAP_STATE_TYPE _searType, LOCATION_MAP_VISIT _mapType);

	void				Rendering();
	void				Player_BetweenDistance();

private :
	_int				m_iWhichChild = { 0 };
	_vector				m_vMapOpen_Player_Distance = {};
	_bool				m_isMainEnd = { false };

public:
	static CMap_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
