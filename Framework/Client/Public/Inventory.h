#pragma once
#include "Customize_UI.h"

class CInventory abstract : public CCustomize_UI
{


protected :
	CInventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInventory(const CInventory& rhs);
	virtual ~CInventory() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected :
	void		Inventory_Render(_bool _render);



public:
	virtual CGameObject* Clone(void* pArg) override = 0;
	virtual void Free() override;
};

