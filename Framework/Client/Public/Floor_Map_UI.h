#pragma once
#include "Map_Manager.h"

BEGIN(Client)

class CFloor_Map_UI final : public CMap_Manager
{
private:
	CFloor_Map_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFloor_Map_UI(const CFloor_Map_UI& rhs);
	virtual ~CFloor_Map_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public :
	MAP_FLOOR_TYPE			Get_SelectFloor() { return m_eSelect_Floor; }


private :
	void					Rendering();
	void					Floor_Sort();
	void					Floor_Select();

private :
	MAP_FLOOR_TYPE			m_eSelect_Floor = { MAP_FLOOR_TYPE::FLOOR_FREE };


public:
	static CMap_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
