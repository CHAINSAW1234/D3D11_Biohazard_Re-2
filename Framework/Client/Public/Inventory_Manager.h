#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "Button_UI.h"

BEGIN(Client)

class CInventory_Manager final : public CBase
{
	enum Inven_DataType{};

private:
	CInventory_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CInventory_Manager() = default;

public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);

public:
	void Set_OnOff_Inven(_bool bInput);


private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	vector<CButton_UI*> m_vecInventoryBox;


	_float2 m_fInterval = { 74.f, 76.f };


public:
	static CInventory_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END